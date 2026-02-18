#include "TZBot/TZBot.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "Helpers.h"
#include "TZBot/Exceptions.h"
#include "TZRequestQueue.h"
#include "encryption/EncryptionFactory.h"

TZBot::TZBot(const std::string& ip, const uint16_t port, const std::string& apiKey, const std::string& cipher) : ip(ip), port(port), apiKey(apiKey) {
    if (!isValidIP(ip)) throw std::invalid_argument("Invalid IP address");

    if (!cipher.empty()) encryption = new EncryptionFactory(cipher);
    else encryption = nullptr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) throw std::runtime_error("Error creating socket");

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        close(fd);
        throw std::runtime_error("Invalid remote address");
    }

    if (connect(fd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        close(fd);
        throw std::runtime_error("Connect failed");
    }

    requestQueue = new TZRequestQueue;
    running.store(true);
    eventThread = std::thread(&TZBot::eventLoop, this);
}

TZBot::~TZBot() {
    running.store(false, std::memory_order_release);
    requestQueue->abort();
    eventThread.join();

    delete requestQueue;
    delete encryption;
    close(fd);
}

std::future<TZResponse> TZBot::enqueue(TZRequest req) {
    std::promise<TZResponse> promise;
    std::future<TZResponse> future = promise.get_future();

    req.setApiKey(apiKey);

    requestQueue->push(std::move(req), std::move(promise));
    return future;
}

void TZBot::setFlags(const std::initializer_list<TZFlags> flags) {
    uint8_t tempFlags = 0;
    for (auto flag : flags) {
        tempFlags |= static_cast<uint8_t>(flag);
    }

    if (tempFlags & static_cast<uint8_t>(TZFlags::AES) && tempFlags & static_cast<uint8_t>(TZFlags::CHACHA20)) throw std::invalid_argument("Only one encryption algorithm is applicable");

    this->applyFlags = tempFlags;
}

void TZBot::eventLoop() {
    std::vector<uint8_t> buf;

    while (running.load(std::memory_order_acquire)) {
        buf.resize(4096);
        try {
            auto [request, promise] = requestQueue->pop();
            std::vector<uint8_t> requestData;
            requestToBytes(request, requestData);

            send(fd, requestData.data(), requestData.size(), 0);
            ssize_t bytesRead = recv(fd, buf.data(), buf.size(), 0);

            if (bytesRead < 0) {
                promise.set_exception(std::make_exception_ptr(SocketReadException()));
                continue;
            }

            buf.resize(bytesRead);

            std::optional<TZResponse> response = parseResponse(buf);
            if (!response.has_value()) {
                promise.set_exception(std::make_exception_ptr(PacketParseException()));
                continue;
            }

            promise.set_value(response.value());

            buf.clear();
        } catch (QueueAbortException& e) {
            break;
        }
    }
}

void TZBot::requestToBytes(const TZRequest& request, std::vector<uint8_t>& out) const {
    out = {};

    nlohmann::json requestJson;
    request.to_json(requestJson);

    std::vector<uint8_t> requestDataBytes;

    if (applyFlags & static_cast<uint8_t>(TZFlags::MSGPACK)) {
        requestDataBytes = nlohmann::json::to_msgpack(requestJson);
    } else {
        std::string json = requestJson.dump();
        requestDataBytes = std::vector<uint8_t>(json.begin(), json.end());
    }

    if (applyFlags & static_cast<uint8_t>(TZFlags::GZIP)) {
        requestDataBytes = gzipCompress(requestDataBytes);
    }

    std::vector<uint8_t> header;
    header.resize(7);

    header[0] = 't';
    header[1] = 'z';
    header[2] = 7;
    header[3] = request.getRequestType();
    header[4] = applyFlags;

    if ((applyFlags & static_cast<uint8_t>(TZFlags::AES) || applyFlags & static_cast<uint8_t>(TZFlags::CHACHA20)) && encryption != nullptr) {
        uint16_t lengthAfterEncryption = requestDataBytes.size() + 16 + 12;

        header[5] = (lengthAfterEncryption >> 8) & 0xFF;
        header[6] = lengthAfterEncryption & 0xFF;

        if (applyFlags & static_cast<uint8_t>(TZFlags::AES)) requestDataBytes = encryption->AESEncrypt(requestDataBytes, header);
        else if (applyFlags & static_cast<uint8_t>(TZFlags::CHACHA20)) requestDataBytes = encryption->ChaCha20Encrypt(requestDataBytes, header);
    } else {
        header[5] = (requestDataBytes.size() >> 8) & 0xFF;
        header[6] = requestDataBytes.size() & 0xFF;
    }

    out.resize(header.size() + requestDataBytes.size());
    out.insert(out.begin(), header.begin(), header.end());
    out.insert(out.begin() + header.size(), requestDataBytes.begin(), requestDataBytes.end());
}

std::optional<TZResponse> TZBot::parseResponse(const std::vector<uint8_t>& resp) {
    if (resp.size() < 6) return std::nullopt;
    if (resp.at(0) != 't' || resp.at(1) != 'z' || resp.at(2) != 6) return std::nullopt;

    std::vector header(resp.begin(), resp.begin() + resp.at(2));
    uint8_t flags = resp.at(3);
    uint16_t payloadLength = (resp.at(4) << 8) | resp.at(5);

    if (resp.size() != payloadLength + header.at(2)) return std::nullopt;
    std::vector body(resp.begin() + resp.at(2), resp.end());

    if (flags & static_cast<uint8_t>(TZFlags::AES) || flags & static_cast<uint8_t>(TZFlags::CHACHA20)) {
        if (encryption == nullptr) return std::nullopt;

        if (flags & static_cast<uint8_t>(TZFlags::AES)) body = encryption->AESDecrypt(body, header);
        else if (flags & static_cast<uint8_t>(TZFlags::CHACHA20)) body = encryption->ChaCha20Decrypt(body, header);
    }

    if (flags & static_cast<uint8_t>(TZFlags::GZIP)) {
        body = gzipDecompress(body);
    }

    nlohmann::json response;
    if (flags & static_cast<uint8_t>(TZFlags::MSGPACK)) response = nlohmann::json::from_msgpack(body);
    else response = nlohmann::json::parse(body);

    return TZResponse::from_json(response);
}
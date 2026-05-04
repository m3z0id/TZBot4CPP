#include "TZBot/TZBot.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "Helpers.h"
#include "TZBot/Exceptions.h"
#include "TZBot/TZFlags.h"
#include "TZRequestQueue.h"
#include "encryption/EncryptionFactory.h"

TZBot::TZBot(const std::string& ip, const uint16_t port, const std::string& apiKey, const std::array<uint8_t, 32>* cipherKey) : ip(ip), port(port), apiKey(apiKey) {
    if (!isValidIP(ip)) {
        if (const std::string ipRes = resolve(ip); !ipRes.empty()) this->ip = ipRes;
        else throw std::invalid_argument("Invalid IP address");
    } else this->ip = ip;

    if (cipherKey != nullptr && cipherKey->size() == 32) encryption = new EncryptionFactory(*cipherKey);
    else encryption = nullptr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) throw std::runtime_error("Error creating socket");

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, this->ip.c_str(), &serv_addr.sin_addr) <= 0) {
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

std::future<TZResponse> TZBot::enqueue(TZRequest req) const {
    std::promise<TZResponse> promise;
    std::future<TZResponse> future = promise.get_future();

    req.setApiKey(apiKey);

    requestQueue->push(std::move(req), std::move(promise));
    return future;
}

void TZBot::setFlags(const uint8_t flags) {
    static constexpr uint8_t MAX_FLAGS = static_cast<uint8_t>(TZFlags::AES) | static_cast<uint8_t>(TZFlags::CHACHA20) | static_cast<uint8_t>(TZFlags::MSGPACK);
    if (flags > MAX_FLAGS) throw std::invalid_argument("Invalid flags inputted!");

    if (flags & static_cast<uint8_t>(TZFlags::AES) && flags & static_cast<uint8_t>(TZFlags::CHACHA20)) throw std::invalid_argument("Only one encryption algorithm is applicable");
    this->applyFlags = flags;
}

void TZBot::eventLoop() const {
    std::vector<uint8_t> buf;

    while (running.load(std::memory_order_acquire)) {
        buf.resize(4096);
        try {
            auto [request, promise] = requestQueue->pop();
            std::vector<uint8_t> requestData = requestToBytes(request);

            send(fd, requestData.data(), requestData.size(), 0);
            ssize_t bytesRead = recv(fd, buf.data(), buf.size(), 0);

            if (bytesRead < 1) {
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
        } catch (QueueAbortException&) {
            break;
        }
    }
}

std::vector<uint8_t> TZBot::requestToBytes(const TZRequest& request) const {
    nlohmann::json requestJson = request.toJson();

    std::vector<uint8_t> data;
    if (applyFlags & static_cast<uint8_t>(TZFlags::MSGPACK)) data = nlohmann::json::to_msgpack(requestJson);
    else {
        std::string json = requestJson.dump();
        data.assign(json.begin(), json.end());
    }

    bool doEncrypt = ((applyFlags & static_cast<uint8_t>(TZFlags::AES) ||
                       applyFlags & static_cast<uint8_t>(TZFlags::CHACHA20)) &&
                      encryption != nullptr);

    size_t bodySize = data.size();
    if (doEncrypt) bodySize = 12 + data.size() + 16;

    constexpr size_t HEADER_SIZE = 7;
    std::vector<uint8_t> out;
    out.resize(HEADER_SIZE + bodySize);

    out[0] = 't';
    out[1] = 'z';
    out[2] = 7;
    out[3] = request.getRequestType();
    out[4] = applyFlags;
    out[5] = static_cast<uint8_t>((bodySize >> 8) & 0xFF);
    out[6] = static_cast<uint8_t>(bodySize & 0xFF);

    if (doEncrypt) {  // pointer can't be null here
        std::vector<uint8_t> encrypted;
        if (applyFlags & static_cast<uint8_t>(TZFlags::AES)) encrypted = encryption->AESEncrypt(data, {out.begin(), out.begin() + 7});
        else encrypted = encryption->ChaCha20Encrypt(data, {out.begin(), out.begin() + 7});

        std::memcpy(out.data() + HEADER_SIZE, encrypted.data(), encrypted.size());
    } else std::memcpy(out.data() + HEADER_SIZE, data.data(), data.size());

    return out;
}

std::optional<TZResponse> TZBot::parseResponse(const std::vector<uint8_t>& resp) const {
    if (resp.size() < 6) return std::nullopt;

    const uint8_t* raw = resp.data();
    uint8_t headerLen = raw[2];

    if (raw[0] != 't' || raw[1] != 'z' || headerLen < 6) return std::nullopt;

    uint8_t flags = raw[3];
    uint16_t payloadLength = (raw[4] << 8) | raw[5];

    if (resp.size() != static_cast<size_t>(payloadLength + headerLen)) return std::nullopt;

    const uint8_t* bodyPtr = raw + headerLen;
    size_t bodyLen = payloadLength;

    std::vector<uint8_t> transformedData;
    if ((flags & (static_cast<uint8_t>(TZFlags::AES) | static_cast<uint8_t>(TZFlags::CHACHA20))) && encryption) {
        std::vector<uint8_t> headerAAD(raw, raw + headerLen);
        std::vector<uint8_t> encryptedBody(bodyPtr, bodyPtr + bodyLen);

        if (flags & static_cast<uint8_t>(TZFlags::AES)) transformedData = encryption->AESDecrypt(encryptedBody, headerAAD);
        else transformedData = encryption->ChaCha20Decrypt(encryptedBody, headerAAD);

        bodyPtr = transformedData.data();
        bodyLen = transformedData.size();
    }

    try {
        nlohmann::json j;
        if (flags & static_cast<uint8_t>(TZFlags::MSGPACK)) j = nlohmann::json::from_msgpack(bodyPtr, bodyPtr + bodyLen);
        else j = nlohmann::json::parse(bodyPtr, bodyPtr + bodyLen);
        return TZResponse::fromJson(j);
    } catch (...) {
        return std::nullopt;
    }
}
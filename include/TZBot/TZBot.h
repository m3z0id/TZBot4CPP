#pragma once
#include <atomic>
#include <future>

#include "TZResponse.h"
#include "TZRequest.h"

class EncryptionFactory;
class TZRequestQueue;

class TZBot {
private:
    std::atomic<bool> running;

    std::string ip;
    uint16_t port;

    int fd;
    TZRequestQueue* requestQueue;
    std::string apiKey;
    uint8_t applyFlags = 0;

    std::thread eventThread;
    void eventLoop() const;

    EncryptionFactory* encryption;
    [[nodiscard]] std::optional<TZResponse> parseResponse(const std::vector<uint8_t>& resp) const;
    [[nodiscard]] std::vector<uint8_t> requestToBytes(const TZRequest& request) const;
public:
    TZBot(const std::string& ip, uint16_t port, const std::string& apiKey, const std::string& cipher);
    ~TZBot();

    [[nodiscard]] std::future<TZResponse> enqueue(TZRequest req) const;
    void setFlags(uint8_t flags);
};

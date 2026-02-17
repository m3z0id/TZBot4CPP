#pragma once
#include <atomic>
#include <future>

#include "TZFlags.h"
#include "TZRequest.h"
#include "TZRequestQueue.h"
#include "TZResponse.h"
#include "encryption/EncryptionFactory.h"

class TZBot {
private:
    std::atomic<bool> running;

    std::string ip;
    uint16_t port;

    int fd;
    TZRequestQueue requestQueue;
    std::string apiKey;
    uint8_t applyFlags = 0;

    std::thread eventThread;
    void eventLoop();

    EncryptionFactory* encryption;
    std::optional<TZResponse> parseResponse(const std::vector<uint8_t>& resp);
    void requestToBytes(const TZRequest& request, std::vector<uint8_t>& out) const;
public:
    TZBot(const std::string& ip, uint16_t port, const std::string& apiKey, const std::string& cipher);
    ~TZBot();

    std::future<TZResponse> enqueue(TZRequest req);
    void setFlags(std::initializer_list<TZFlags> flags);
};

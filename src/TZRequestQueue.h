#pragma once

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

#include "TZBot/TZRequest.h"
#include "TZBot/TZResponse.h"

class TZRequestQueue {
    std::queue<std::pair<TZRequest, std::promise<TZResponse>>> queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool aborted = false;

public:
    std::pair<TZRequest, std::promise<TZResponse>> pop();
    void push(TZRequest&& req, std::promise<TZResponse>&& promise);
    void abort();
};

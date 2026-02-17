#pragma once

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

#include "TZRequest.h"
#include "TZResponse.h"

class TZRequestQueue {
    bool aborted = false;
    std::queue<std::pair<TZRequest, std::promise<TZResponse>>> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    std::pair<TZRequest, std::promise<TZResponse>> pop();
    void push(TZRequest&& req, std::promise<TZResponse>&& promise);
    void abort();
};

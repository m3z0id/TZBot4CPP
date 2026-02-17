#include "../include/TZRequestQueue.h"

std::pair<TZRequest, std::promise<TZResponse>> TZRequestQueue::pop() {
    std::unique_lock lock(mtx);

    cv.wait(lock, [this] {
        return !queue.empty() || aborted;
    });

    if (aborted) throw std::runtime_error("TZRequestQueue::pop: aborted");

    auto item = std::move(queue.front());
    queue.pop();

    return std::move(item);
}

void TZRequestQueue::push(TZRequest&& req, std::promise<TZResponse>&& promise)  {
    {
        std::lock_guard lock(mtx);
        queue.emplace(std::move(req), std::move(promise));
    }
    cv.notify_one();
}

void TZRequestQueue::abort() {
    {
        std::lock_guard lock(mtx);
        aborted = true;
    }
    cv.notify_all();
}

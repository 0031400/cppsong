#pragma once
#include "utils/headers.hpp"
#include <algorithm>
#include <boost/asio/redirect_error.hpp>
#include <boost/system/detail/error_code.hpp>
#include <chrono>
#include <deque>
#include <memory>
#include <optional>
template <typename T> class AsyncQueue {
public:
  explicit AsyncQueue(asio::io_context &io) : io_(io) {}
  async<void> put(T value) {
    if (!get_waiters_.empty()) {
      auto waiter = get_waiters_.front();
      get_waiters_.pop_front();
      waiter->value = std::move(value);
      waiter->complete();
      co_return;
    }
    queue_.push_back(std::move(value));
    co_return;
  }
  async<T> get() {
    if (!queue_.empty()) {
      T value = std::move(queue_.front());
      queue_.pop_front();
      co_return value;
    }
    auto waiter = std::make_shared<GetWaiter>(io_);
    get_waiters_.push_back(waiter);
    co_await waiter->wait();
    co_return std::move((waiter->value).value());
  }

private:
  struct GetWaiter {
    asio::steady_timer timer;
    bool done = false;
    std::optional<T> value;
    explicit GetWaiter(asio::io_context &io) : timer(io) {}
    async<void> wait() {
      timer.expires_at(std::chrono::steady_clock::time_point::max());
      boost::system::error_code ec;
      co_await timer.async_wait(asio::redirect_error(asio::use_awaitable, ec));
      co_return;
    }
    void complete() {
      if (done) {
        return;
      }
      done = true;
      timer.cancel();
    }
  };
  asio::io_context &io_;
  std::deque<T> queue_;
  std::deque<std::shared_ptr<GetWaiter>> get_waiters_;
};
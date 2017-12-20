/**
 *  Copyright (c) 2015 by Contributors
 */
#ifndef PS_INTERNAL_THREADSAFE_QUEUE_H_
#define PS_INTERNAL_THREADSAFE_QUEUE_H_
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "ps/base.h"
namespace ps {

/**
 * \brief thread-safe queue allowing push and waited pop
 */
template<typename T> class ThreadsafeQueue {
 public:
  ThreadsafeQueue() { }
  ~ThreadsafeQueue() { }

  /**
   * \brief push an value into the end. threadsafe.
   * \param new_value the value
   */
  void Push(std::shared_ptr<T> new_value) {
    mu_.lock();
    queue_.push(new_value);
    mu_.unlock();
    cond_.notify_all();
  }

//  /**
//   * \brief wait until pop an element from the beginning, threadsafe
//   * \param value the poped value
//   */
//  void WaitAndPop(T* value) {
//      std::unique_lock<std::mutex> lk(mu_);
//      cond_.wait(lk, [this]{return !queue_.empty();});
//      *value = std::move(queue_.front());
//      queue_.pop();
//    }

  std::shared_ptr<T> WaitAndPop() {
    std::shared_ptr<T> value;
    std::unique_lock<std::mutex> lk(mu_);
    cond_.wait(lk, [this]{return !queue_.empty();});
    value = queue_.front();
    queue_.pop();
    return value;
  }

 private:
  mutable std::mutex mu_;
  std::queue<std::shared_ptr<T> > queue_;
  std::condition_variable cond_;
};

}  // namespace ps

// bool TryPop(T& value) {
//   std::lock_guard<std::mutex> lk(mut);
//   if(data_queue.empty())
//     return false;
//   value=std::move(data_queue.front());
//   data_queue.pop();
//   return true;
// }
#endif  // PS_INTERNAL_THREADSAFE_QUEUE_H_

#include "thread_pool.hpp"

// Constructor, creates the required threads
ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            // Worker loop
            for (;;) {
                std::function<void()> task;

                {
                    // Block the queue to safe extract a task
                    std::unique_lock<std::mutex> lock(this->queue_mutex);

                    // The thread sleep until a task exists or the pool stops
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    // If the pool stops and there are not tasks, the thread ends
                    if (this->stop && this->tasks.empty())
                        return;

                    // We extract the task
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                // Execute the task out of the lock to not block other threads
                task();
            }
        });
    }
}

// Destructor
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    // Notify all threads to they see that 'stop' is true
    condition.notify_all();
    // We wait to each thread finish their last task
    for (std::thread &worker : workers) {
        worker.join();
    }
}

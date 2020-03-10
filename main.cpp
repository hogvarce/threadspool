#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

class ThreadPoll {
    public:
        ThreadPoll() {
            stop = false;
            notified = false;
            unsigned int coreCount = std::thread::hardware_concurrency();
            for (int i = 0; i < coreCount; ++i) {
                threads.push_back(std::thread(&ThreadPoll::runner, this));
            }
        }
        ~ThreadPoll() {
            stop = true;
            for (auto &thread : threads)
                thread.join();
        }
        void addTask(std::function<void()> Func) {
            std::unique_lock<std::mutex> lck (mtx);
            tasks.push_back(Func);
            notified = true;
            cond_var.notify_all();
        }
    private:
        std::vector<std::thread> threads;
        std::vector<std::function<void()>> tasks;
        std::condition_variable cond_var;
        std::mutex mtx;
        std::atomic_bool notified;
        bool stop;
        void runner() {
            while(!stop) {
                std::function<void()> func;
                {
                    std::unique_lock<std::mutex> lck (mtx);
                    cond_var.wait(lck, [this]{return notified.load();});
                    if (tasks.empty()) continue;
                    func = tasks.back();
                    tasks.pop_back();
                }
                func();
            }
        }
};

void test() {
    std::cout << std::this_thread::get_id() << std::endl;
}

void Print() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    for (auto i = 0; i < 5; ++i) {
        for (auto j = 0; j < 10; j++) {
            std::cout << std::this_thread::get_id();
        }
        std::cout << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

int main() {
    ThreadPoll pool;
    test();
    pool.addTask(Print);
    pool.addTask(test);
    std::cout << std::this_thread::get_id() << std::endl;
}
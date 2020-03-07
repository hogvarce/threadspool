#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>

class ThreadPoll {
    public:
        ThreadPoll() {
            stop = false;
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
        }
    private:
        std::vector<std::thread> threads;
        std::vector<std::function<void()>> tasks;
        std::mutex mtx;
        bool stop;
        void runner() {
            while(!stop) {
                std::unique_lock<std::mutex> lck (mtx);
                if (!tasks.empty())
                    tasks[0]();
            }
        }
};

void test() {
    std::cout << "I`m thread" << std::endl;
}
int main() {
    ThreadPoll pool;
    pool.addTask(test);
    std::cout << "main ends" << std::endl;
}
//
// Created by vityha on 26.03.22.
//
#include <iostream>
#include <chrono>
#include "../src/thread_safe_queue.h"
#include <thread>
#include <vector>
#include <mutex>

void getElement(int threadNum, ThreadSafeQueue<int> &que, std::mutex &coutMutex) {
    using namespace std::chrono_literals;

    int el;
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        el = que.deque();
        std::cout << threadNum << " thread: " << el << std::endl;
    }
    std::this_thread::sleep_for(250ms);

    while(el != 9){
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            el = que.deque();
            std::cout << threadNum << " thread: " << el << std::endl;
        }
        std::this_thread::sleep_for(250ms);
    }

    que.enque(9);
}

int main() {
    ThreadSafeQueue<int> que;
    int threadsNum = 10;
    std::vector<std::thread> threads(threadsNum);
    std::mutex coutMutex;

    for(int i = 0; i < threadsNum; i++){
        que.enque(i);
    }

    for(int i = 0; i < threadsNum; i++){
        threads[i] = std::thread(getElement, i, std::ref(que), std::ref(coutMutex));
    }

    for (std::thread &th: threads) {
        if (th.joinable())
            th.join();
    }

    return 0;
}
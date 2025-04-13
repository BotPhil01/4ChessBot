#include <ctime>
#include <functional>
#include <thread>
#include <stop_token>
#include <cassert>
#include <memory>
#include <iostream>
#include <mutex>


#ifndef CHESSTIMER_H
#define CHESSTIMER_H
namespace chessTimer {
    class Timer {
        inline static bool s_started = false;
        double m_maxDuration;
        std::jthread m_timingThread;
        std::mutex &m_startMutex;

        public:
        inline static bool s_finished = false;
            Timer(std::mutex &mutex, double maxDuration = 5.0) :
            m_maxDuration(maxDuration),
                m_startMutex(mutex)
            {
                auto f = std::bind_front(&Timer::childInit, this);
                m_timingThread = std::jthread(f);
            }

            // Timer(const Timer &other) {
            // }

            void start() {
                Timer::s_started = true;
            }

            bool awaitFinish() {
                while (!Timer::s_finished) {
                    continue;
                }
                return Timer::s_finished;
            }

        private:
            // in the timing thread
            void childInit() {
                // locks the mutex
                std::unique_lock<std::mutex> lock(m_startMutex);
                // block while we havent started
                while (!Timer::s_started) {
                    continue;
                }
                // unlock the mutex to start the work
                lock.unlock();
                count();
                Timer::s_finished = true;
            }

            void count() {
                std::cout << "timer starting\n";
                const std::time_t startTime = std::time(nullptr);
                std::time_t currentTime = std::time(nullptr);
                double difference = difftime(currentTime, startTime);
                while (difference < m_maxDuration) {
                    currentTime = std::time(nullptr);
                    difference = difftime(currentTime, startTime);
                    // std::cout << difference << "\n";
                }
                std::cout << "timer ending\n";
            }
    };

}
#endif

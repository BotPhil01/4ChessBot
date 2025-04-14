#include <chrono>
#include <ctime>
#include <functional>
#include <system_error>
#include <thread>
#include <stop_token>
#include <cassert>
#include <memory>
#include <iostream>
#include <mutex>
#include <semaphore>

#ifndef CHESSTIMER_H
#define CHESSTIMER_H
namespace chessTimer {
    // a flag that can be signalled on and off and can be waited on
    // blocks when the 
    class Flag {
        // flag is in a true state if we can successfully acquire the semaphore
        std::binary_semaphore m_sem;

        public:

        Flag() :
            m_sem{0}
        {}

        bool operator=(bool val) {
            return set(val);
        }

        bool set(bool val) {
            if (val) {
                m_sem.release();
                return true;
            } else {
                return m_sem.try_acquire();
            }
        }

        // return the state of the flag
        // does not block
        bool getState() {
            bool res = m_sem.try_acquire();
            if (res) {
                m_sem.release();
            }
            return res;
        }
        // explicit operator bool() {
        //     bool res = m_sem.try_acquire();
        //     if (res) {
        //         m_sem.release();
        //     }
        //     return res;
        // }

        // wait for the flag to be in a true state
        void wait() {
            m_sem.acquire();
            m_sem.release();
        }
    };
    class Timer {
        // convert these into semaphores
        Flag m_started;
        Flag m_mutexLocked;

        // bool m_started = false;
        // bool m_mutexLocked = false;
        double m_maxDuration;
        std::jthread m_timingThread;
        std::mutex &m_startMutex;

        public:
        // bool p_finished = false;
        Flag p_finished;
        inline static bool s_terminating = false;
            Timer(std::mutex &mutex, double maxDuration = 5.0) :
            m_maxDuration(maxDuration),
                m_startMutex(mutex)
            {
                threadInit();
            }

            void start() {
                m_started = true;
            }

            bool awaitFinish() {
                p_finished.wait();
                // while (!p_finished) {
                //     continue;
                // }
                return p_finished.getState();
            }

            void reset() {
                m_started = false;
                p_finished = false;
                m_mutexLocked = false;
                threadInit();
            }

            void terminate() {
                m_started = true;
                s_terminating = true;
            }
        private:
            void threadInit() {
                auto f = std::bind_front(&chessTimer::Timer::childInit, this);
                m_timingThread = std::jthread(f);
                // ensure that the child thread has locked it successfully
                // sometimes after exiting this the thread pool threads are able to start execution without the 

                using namespace std::chrono_literals;
                std::chrono::milliseconds time = 10ms;

                // block while the mutex hasnt locked
                // this is because sometimes the threadPool can create children and start them before the mutex has been locked
                // its weird ik and probably prone to errors but its the best I can do
                // might be an idea to add a system to exit with an error code if unable to lock the mutex after some time
                // kind of a way to allow bad pcs to not shut down entirely if struggling to init the child
                // the alternative because this keeps the thread awake is to sleep the thread or introduce another mutex
                // The problem with sleeping is that it could be a lot of time for an uneccessary wait
                // The problem with introducing another mutex is the risk of deadlocking if poor logic
                m_mutexLocked.wait();
            }
            // in the timing thread
            void childInit() {
                // locks the mutex
                std::unique_lock<std::mutex> lock(m_startMutex, std::defer_lock);
                try {
                    lock.lock();
                } catch (const std::system_error &e) {
                    std::cout << "System error: " << e.code() << "\n" << e.what();
                    assert(false);
                }
                // cant use try_lock because threadInit is also attempting to lock it as it must block 
                m_mutexLocked = true;
                // block while we havent started

                m_started.wait();
                // unlock the mutex to start the work
                lock.unlock();
                if (!s_terminating) {
                    count();
                }
                p_finished = true;
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

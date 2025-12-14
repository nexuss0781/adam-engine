// ===================================================================================
//  File:    JobSystem.h
//  Project: Adam Engine
//  Purpose: Declares the JobSystem, a multi-threaded task scheduler designed for
//           high-performance, parallel execution of simulation logic.
// ===================================================================================

#pragma once

#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <atomic>

// A Job is simply a function that takes no arguments and returns nothing.
using Job = std::function<void()>;

class JobSystem
{
public:
    /**
     * @brief Constructs the JobSystem and initializes its worker threads.
     */
    JobSystem();

    /**
     * @brief Destructor. Waits for all threads to finish and cleans up.
     */
    ~JobSystem();

    /**
     * @brief Dispatches a single job to be executed by a worker thread.
     * @param job The work to be done.
     */
    void Dispatch(Job job);

    /**
     * @brief Waits until all dispatched jobs have been completed.
     * @details This is a blocking call and is the primary synchronization point.
     *          The main thread calls this after dispatching all work for a frame.
     */
    void Wait();
    
    /**
     * @brief Gets the number of worker threads in the system.
     * @return The thread count.
     */
    uint32_t GetThreadCount() const { return m_numThreads; }

    // --- Disable copying and moving ---
    JobSystem(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&) = delete;

private:
    /**
     * @brief The main loop that each worker thread executes.
     * @details Continuously polls the job queue and executes work until stopped.
     */
    void WorkerLoop();

    uint32_t m_numThreads;
    std::vector<std::thread> m_threads;
    std::vector<Job> m_jobQueue;
    
    std::mutex m_queueMutex;
    std::condition_variable m_workerCondition;
    std::condition_variable m_mainCondition;

    std::atomic<uint32_t> m_jobsInProgress;
    std::atomic<bool> m_stop;
};

// ===================================================================================
//  File:    JobSystem.cpp
//  Project: Adam Engine
//  Purpose: Implements the JobSystem thread pool.
// ===================================================================================

#include "JobSystem.h"

JobSystem::JobSystem()
    : m_jobsInProgress(0), m_stop(false)
{
    // Determine the optimal number of threads. We leave one core free for the
    // main thread and operating system to avoid contention.
    m_numThreads = std::max(1u, std::thread::hardware_concurrency() - 1);

    for (uint32_t i = 0; i < m_numThreads; ++i)
    {
        m_threads.emplace_back([this] { this->WorkerLoop(); });
    }
}

JobSystem::~JobSystem()
{
    // Signal all threads to stop
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stop = true;
    }
    m_workerCondition.notify_all();

    // Wait for all threads to finish execution
    for (auto& thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void JobSystem::Dispatch(Job job)
{
    m_jobsInProgress++;
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_jobQueue.push_back(std::move(job));
    }
    m_workerCondition.notify_one();
}

void JobSystem::Wait()
{
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_mainCondition.wait(lock, [this] {
        return m_jobsInProgress == 0;
    });
}

void JobSystem::WorkerLoop()
{
    while (true)
    {
        Job job;

        { // Acquire lock to check for work
            std::unique_lock<std::mutex> lock(m_queueMutex);

            // Wait until there's a job or we're stopping
            m_workerCondition.wait(lock, [this] {
                return !m_jobQueue.empty() || m_stop;
            });

            if (m_stop && m_jobQueue.empty())
            {
                return; // Exit loop
            }

            // Pop a job from the queue
            job = std::move(m_jobQueue.front());
            m_jobQueue.erase(m_jobQueue.begin());
        } // Release lock

        // Execute the job
        job();

        // Atomically decrement the job counter
        m_jobsInProgress--;

        // Notify the main thread if all jobs might be done
        m_mainCondition.notify_one();
    }
}

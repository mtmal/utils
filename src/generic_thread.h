////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2023 Mateusz Malinowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atomic>
#include <pthread.h>
#include <semaphore.h>

template <typename Derived>
class GenericThread
{
public:
    /**
     * Basic class constructor.
     */
    GenericThread() : mThread(0)
    {
        pthread_mutex_init(&mMutex, nullptr);
        sem_init(&mSemaphore, 0, 0);
        mRun.store(false, std::memory_order_release);
    }

    /**
     * Class destructor that stops the thread. It does not call the thread cancellation nor process any thread-generated outputs.
     */
    virtual ~GenericThread()
    {
        stopThread(false);
        sem_destroy(&mSemaphore);
        pthread_mutex_destroy(&mMutex);
    }

    /**
     * Starts the thread.
     */
    bool startThread()
    {
        bool retVal = (0 == pthread_create(&mThread, nullptr, GenericThread::startThread, static_cast<Derived*>(this)));
        mRun.store(retVal, std::memory_order_release);
        return retVal;
    }

    /**
     * Stops the thread and returns any data it may have created.
     *  @param force a flag to indicate if the termination should be forced by cancelling the thread.
     *  @param[out] threadReturn data returned by the thread. nullptr assumes no return data.
     */
    void stopThread(const bool force = false, void* threadReturn = nullptr)
    {
        mRun.store(false, std::memory_order_release);
        if (mThread > 0)
        {
            if (force)
            {
                pthread_cancel(mThread);
            }
            pthread_join(mThread, &threadReturn);
            mThread = 0;
        }
    }

    /**
     *  @return true if the thread is running.
     */
    inline bool isRunning() const
    {
        return mRun.load(std::memory_order_acquire);
    }

protected:
    /** Mutex for locking resources. */
    pthread_mutex_t mMutex;
    /** Semaphore for thread synchronisation. */
    sem_t mSemaphore;

private:
    /**
     * Starts the thread. @p instance needs to implement threadBody function.
     *  @param instance an instance of the derived class.
     *  @return thread return values.
     */
    static void* startThread(void* instance)
    {
        return static_cast<Derived*>(instance)->threadBody();
    }

    /** Pthread related to this class. */
    pthread_t mThread;
    /** A flag indicating if the thread is running. */
    std::atomic<bool> mRun;
};

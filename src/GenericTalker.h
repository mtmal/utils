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

#ifndef GENERIC_TALKER_H_
#define GENERIC_TALKER_H_

#include <atomic>
#include <unordered_map>
#include "IGenericListener.h"
#include "ScopedLock.h"

/**
 * Class implementation of a generic talker class. Handy to broadcast data 
 * to multiple listeners that can ad hoc register or unregister.
 */
template<typename... Args>
class GenericTalker
{
public:
    /**
     * Basic constructor that initialises a lock.
     */
    GenericTalker() : mTalk(true)
    {
        pthread_mutex_init(&mLock, nullptr);
    }

    /**
     * Basic destructor that destroys the lock.
     */
    virtual ~GenericTalker()
    {
        pthread_mutex_destroy(&mLock);
    }

    /**
     * Registers a listener and returns a random ID. It checks if generated keey already exists,
     * but there is no boundry in the number of tries.
     *  @param listener a listener which should be notified by the talker.
     *  @return id at which the listener was inserted into a map. It is needed for unregistration.
     */
    int registerListener(IGenericListener<Args...>& listener)
    {
        int id;
        ScopedLock lock(mLock);
        do
        {
            id = rand();
        } while(mListeners.find(id) != mListeners.end());
        mListeners.insert(std::pair<int, IGenericListener<Args...>& >(id, listener));
        return id;
    }

    /**
     * Unregisters a listener.
     *  @param id the ID at which listener was registered.
     */
    void unregisterListener(const int id)
    {
        ScopedLock lock(mLock);
        mListeners.erase(mListeners.find(id));
    }

    /**
     * Tests if an id is already registered.
     *  @param id an ID to test.
     *  @return true if the @p id is in the map.
     */
    bool testId(const int id) const
    {
        ScopedLock lock(mLock);
        return (mListeners.find(id) != mListeners.end());
    }

    /**
     * Pauses the talker from broadcasting updates.
     */
    inline void pause()
    {
        mTalk = false;
    }

    /**
     * Resumes the talker to broadcast updates.
     */
    inline void resume()
    {
        mTalk = true;
    }

    /**
     *  @return true if the talker is broadcasting updates.
     */
    inline bool isTalking() const
    {
        return mTalk.load(std::memory_order_relaxed);
    }

protected:
    /**
     * Notifies all listeners with a new data.
     *  @param data new data to broadcast to listeners.
     */
    void notifyListeners(const Args&... data) const
    {
        if (isTalking())
        {
            ScopedLock lock(mLock);
            for (const std::pair<int, IGenericListener<Args...>& >& listener : mListeners)
            {
                listener.second.update(data...);
            }
        }
    }

private:
    /** Flag indicating if the talker is should broadcast updates or not. */
    std::atomic<bool> mTalk;
	/** The list of listeners registered to the talker class. */
	std::unordered_map<int, IGenericListener<Args...>& > mListeners;
	/** Lock for accessing the listeners map. */
	mutable pthread_mutex_t mLock;
};

#endif /* GENERIC_TALKER_H_ */
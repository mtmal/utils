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
#include "registration_base.h"
#include "scoped_lock.h"


template<typename... Args>
class GenericListener;

/**
 * Class implementation of a generic talker class. Handy to broadcast data 
 * to multiple listeners that can ad hoc register or unregister.
 */
template<typename... Args>
class GenericTalker : public RegistrationBase<GenericTalker<Args...>, GenericListener<Args...>>
{
public:
    /**
     * Basic constructor that initialises a lock.
     */
    GenericTalker() : mTalk(true)
    {
    }

    /**
     * Pauses the talker from broadcasting updates.
     */
    inline void pause()
    {
        mTalk.store(false, std::memory_order_release);
    }

    /**
     * Resumes the talker to broadcast updates.
     */
    inline void resume()
    {
        mTalk.store(true, std::memory_order_release);
    }

    /**
     *  @return true if the talker is broadcasting updates.
     */
    inline bool isTalking() const
    {
        return mTalk.load(std::memory_order_acquire);
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
            ScopedLock lock(this->mLock);
            for (GenericListener<Args...>* listener : this->mItems)
            {
                listener->update(data...);
            }
        }
    }

private:
    /** Flag indicating if the talker is should broadcast updates or not. */
    std::atomic<bool> mTalk;
};

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

#include <algorithm>
#include <vector>
#include "ScopedLock.h"


/**
 * A tool that handles registration between classes. Derived class registered to RegisterTo,
 * and vice versa. This enables for talker to register to listeners, or for listener to register
 * to talkers. As internally pointers to RegisterTo are stored in a vector, when the class goes
 * out of scope, it will notify all RegisterTo instances to remove itself from their respective
 * lists.
 */
template<typename Derived, typename RegisterTo>
class RegistrationBase
{
public:
    /**
     * Constructor which initialises recursive mutex.
     */
    RegistrationBase()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mLock, &attr);
    }


    /**
     * Restructor that unregisters all RegisterTo instances from itself.
     */
    virtual ~RegistrationBase()
    {
        for (RegisterTo* item : mItems)
        {
            item->unregisterFrom(static_cast<Derived*>(this));
        }
        pthread_mutex_destroy(&mLock);
    }

    /**
     * Registers an item to this class. Registration is done only 
     * if the @p item was not already registered.
     *  @param item a pointer to either talker or listener.
     */
    void registerTo(RegisterTo* item)
    {
        ScopedLock lock(mLock);
        if (std::find(mItems.begin(), mItems.end(), item) == mItems.end())
        {
            mItems.push_back(item);
            // handshake. Make sure item knows we are registering.
            item->registerTo(static_cast<Derived*>(this));
        }
    }

    /**
     * Unregisters an item from this class. Unregistration is done only if
     * the @p item was already registered.
     *  @param item a pointer to either talker or listener.
     */
    void unregisterFrom(RegisterTo* item)
    {
        ScopedLock lock(mLock);
        auto it = std::find(mItems.begin(), mItems.end(), item);
        if (it != mItems.end())
        {
            mItems.erase(it);
            // handshake. Make sure item knows we are unregistering.
            item->unregisterFrom(static_cast<Derived*>(this));
        }
    }

protected:
	/** The list of items registered to this class. */
    std::vector<RegisterTo*> mItems;
	/** Lock for accessing the list of items. */
	mutable pthread_mutex_t mLock;
};

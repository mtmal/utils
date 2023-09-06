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

#include <cstdio>
#include <string>
#include <unistd.h>
#include <GenericThread.h>

std::atomic<int> globalCounter = 0;

class Counter : public GenericThread<Counter>
{
public:
    Counter(const int maxIter, const std::string& name) : GenericThread<Counter>(), mMaxIter(maxIter), mCounter(0), mName(name) {}

    virtual ~Counter()
    {
        printf("My name: %s, local counter: %d, global counter: %d \n", mName.c_str(), mCounter, globalCounter.load(std::memory_order_relaxed));
    }

    void* threadBody()
    {
        while (mCounter < mMaxIter)
        {
            ++mCounter;
            globalCounter.fetch_add(1);
        }
        return nullptr;
    }

private:
    int mMaxIter;
    int mCounter;
    std::string mName;
};

int main()
{
    Counter c1 = Counter(10, "C1");
    Counter c2 = Counter(40, "C2");
    c1.startThread();
    c2.startThread();
    sleep(1);
    return 0;
}
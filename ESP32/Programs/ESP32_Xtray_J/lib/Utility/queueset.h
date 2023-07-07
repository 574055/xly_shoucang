#ifndef QUEUESET_H
#define QUEUESET_H

#include <etl/deque.h>


/**
 * Data structure for queueing things, but ensuring the items queued are unique.
 */
template <typename T, size_t N>
struct queueset
{
    etl::deque<T, N> container;

    void push(const T& x)
    {
        if (container.size() == N)
            return;

        for (size_t i = 0; i < container.size(); i++) {
            if (container[i] == x)
                return; // Don't add if item is already in the container.
        }
        container.push_back(x);
    }

    const T& front()
    {
        return container.front();
    }

    void pop()
    {
        if (container.empty())
            return;
        container.pop_front();
    }

    size_t size() const
    {
        return container.size();
    }

    bool empty() const
    {
        return container.empty();
    }
};

#endif
#ifndef __MYQUEUE_H__
#define __MYQUEUE_H__

#include <iostream>
#include <algorithm>

#include "MyVector_c543d983.h"

template <typename DataType>
class MyQueue : private MyVector<DataType>
{
private:
    size_t dataStart, dataEnd;

    // changes the size of the array to newSize
    void resize(size_t newSize)
    {
        MyVector<DataType>::resize(newSize);
    }

    // requests for newCapacity amount of space
    void reserve(size_t newCapacity)
    {
        if (newCapacity <= this->capacity())
            return;

        // Create a new vector with the new capacity
        MyVector<DataType> newVector(newCapacity);

        // Copy elements from the old array to the new array
        size_t currentSize = size();
        for (size_t i = 0; i < currentSize; ++i)
        {
            newVector.push_back((*this)[(dataStart + i) % this->capacity()]);
        }

        // Swap the underlying data of the current queue with the new vector
        this->MyVector<DataType>::operator=(std::move(newVector));

        // Update the dataStart and dataEnd indices
        dataStart = 0;
        dataEnd = currentSize;
    }

public:
    // default constructor
    explicit MyQueue(size_t initSize = 0) : MyVector<DataType>(initSize), dataStart(0), dataEnd(initSize)
    {
        // No additional initialization needed
    }

    // copy constructor
    MyQueue(const MyQueue &rhs) : MyVector<DataType>(rhs), dataStart(rhs.dataStart), dataEnd(rhs.dataEnd)
    {
        // No additional initialization needed
    }

    // move constructor
    MyQueue(MyQueue &&rhs) : MyVector<DataType>(std::move(rhs)), dataStart(rhs.dataStart), dataEnd(rhs.dataEnd)
    {
        rhs.dataStart = 0;
        rhs.dataEnd = 0;
    }

    // destructor
    ~MyQueue()
    {
        // No additional cleanup needed
    }

    // copy assignment
    MyQueue &operator=(const MyQueue &rhs)
    {
        MyVector<DataType>::operator=(rhs);
        dataStart = rhs.dataStart;
        dataEnd = rhs.dataEnd;
        return *this;
    }

    // move assignment
    MyQueue &operator=(MyQueue &&rhs)
    {
        MyVector<DataType>::operator=(std::move(rhs));
        dataStart = rhs.dataStart;
        dataEnd = rhs.dataEnd;
        rhs.dataStart = 0;
        rhs.dataEnd = 0;
        return *this;
    }

    // insert x into the queue
    void enqueue(const DataType &x)
    {
        if (size() == this->capacity())
        {
            reserve(2 * this->capacity());
        }
        (*this)[dataEnd] = x;
        dataEnd = (dataEnd + 1) % this->capacity();
    }

    // insert x into the queue
    void enqueue(DataType &&x)
    {
        if (size() == this->capacity())
        {
            reserve(2 * this->capacity());
        }
        (*this)[dataEnd] = std::move(x);
        dataEnd = (dataEnd + 1) % this->capacity();
    }

    // remove the first element from the queue
    void dequeue(void)
    {
        if (!empty())
        {
            dataStart = (dataStart + 1) % this->capacity();
        }
    }

    // access the first element of the queue
    const DataType &front(void) const
    {
        return (*this)[dataStart];
    }

    // check if the queue is empty; return TRUE if empty; FALSE otherwise
    bool empty(void) const
    {
        return dataStart == dataEnd;
    }

    // access the size of the queue
    size_t size() const
    {
        if (dataEnd >= dataStart)
        {
            return dataEnd - dataStart;
        }
        else
        {
            return this->capacity() - dataStart + dataEnd;
        }
    }

    // access the capacity of the queue
    size_t capacity(void) const
    {
        return this->MyVector<DataType>::capacity();
    }
};

#endif // __MYQUEUE_H__
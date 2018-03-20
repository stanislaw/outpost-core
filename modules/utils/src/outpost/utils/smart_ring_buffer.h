/*
 * Copyright (c) 2017, German Aerospace Center (DLR)
 *
 * This file is part of the development version of OUTPOST.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2017, Jan-Gerd Mess (DLR RY-AVS)
 */
// ----------------------------------------------------------------------------

/**
 * \file
 * \author  Jan-Gerd Mess
 *
 * \brief FIFO ring buffer data structure for SmartBuffers.
 */

#ifndef MU_COMMON_UTILS_SMART_RING_BUFFER_H
#define MU_COMMON_UTILS_SMART_RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "smart_buffer.h"
#include "bounded_array.h"

namespace outpost
{
namespace utils
{

/**
 * Ring buffer for SmartBuffers.
 *
 *
 * \author  Jan-Gerd Mess
 */
class SmartRingBuffer
{
public:
    explicit inline
	SmartRingBuffer(outpost::BoundedArray<SmartBufferPointer> buffer, outpost::BoundedArray<uint8_t> flags) :
		mBuffer(buffer),
		mFlags(flags),
        mReadIndex(0),
        mNumberOfElements(0)
    {
    }

    virtual ~SmartRingBuffer()
    {
    }

    /**l
     * Get the number of currently inactive count
     *
     * \return number of free count in the buffer
     */
    inline size_t
    getFreeSlots() const
    {
        size_t freeSlots = mBuffer.getNumberOfElements() - mNumberOfElements;
        return freeSlots;
    }

    /**
     * Get the number of currently active count.
     *
     * \return number of taken count in the buffer
     */
    inline size_t
    getUsedSlots() const
    {
        return mNumberOfElements;
    }

    /**
     * Write element to the buffer.
     *
     * \param[in] e new element of type T
     *
     * \retval false on overflow (element not added; buffer full)
     * \retval true  on success (element added)
     */
    inline bool
    append(const SmartBufferPointer& p,
           uint8_t flags = 0)
    {
        bool appended = false;
        if ((mNumberOfElements < mBuffer.getNumberOfElements()))
        {
            // calculate write index
            int writeIndex = increment(mReadIndex, mNumberOfElements);

            mFlags[writeIndex] = flags;
            mBuffer[writeIndex] = p;
            ++mNumberOfElements;

            appended = true;
        }

        return appended;
    }

    /**
     * Check if the buffer is empty.
     *
     * \retval true  if the buffer is empty.
     * \retval false if the buffer contains at least one element.
     */
    inline bool
    isEmpty() const
    {
        return (mNumberOfElements == 0);
    }

    /**
     * Read element from the buffer.
     *
     */
    inline const SmartBufferPointer&
    read() const
    {
        return mBuffer[mReadIndex];
    }

    inline SmartBufferPointer&
    read()
    {
        return mBuffer[mReadIndex];
    }

    inline uint8_t
    readFlags() const
    {
        return mFlags[mReadIndex];
    }

    inline void
    setFlags(uint8_t flags)
    {
        mFlags[mReadIndex] = flags;
    }

    /**
     *
     *
     * \retval false on underflow (no element read; buffer empty)
     * \retval true  on success
     */
    inline bool
    pop()
    {
        bool elementRemoved = false;

        if (mNumberOfElements > 0)
        {
            mBuffer[mReadIndex] = SmartBufferPointer();
        	mReadIndex = increment(mReadIndex, 1);
            --mNumberOfElements;
            elementRemoved = true;
        }

        return elementRemoved;
    }

    /**
     * Provides the means to access a specific element.
     *
     * - Head: index = 0
     * - Tail: index = getUsedcount()
     *
     * \param[in] index of the element in question
     */
    inline const SmartBufferPointer&
    peek(size_t index) const
    {
        if (index <= mBuffer.getNumberOfElements())
        {
            size_t position = increment(mReadIndex, index);
            return mBuffer[position];
        }

        return empty;
    }

    inline uint8_t
    peekFlags(size_t index) const
    {
        size_t position = increment(mReadIndex, index);
        return mFlags[position];
    }

    /**
     * Quick way of resetting the FIFO.
     */
    inline void
    reset()
    {
        mReadIndex = 0;
        mNumberOfElements = 0;
        for (size_t i = 0; i < mBuffer.getNumberOfElements(); i++)
        {
        	mBuffer[i] = SmartBufferPointer();
        }
    }

private:
    // Disable copy constructor
	SmartRingBuffer(const SmartRingBuffer& o);

    // Disable copy assignment operator
	SmartRingBuffer&
	operator=(const SmartRingBuffer& o);

	inline size_t
	increment(size_t index, size_t count) const
	{
        size_t next = (index + count) % mBuffer.getNumberOfElements();
        return next;
	}

	SmartBufferPointer empty;

	const outpost::BoundedArray<SmartBufferPointer> mBuffer;
	const outpost::BoundedArray<uint8_t> mFlags;

	size_t mReadIndex;
	size_t mNumberOfElements;
};

/**
 * Storage provider for SmartRingBuffer.
 *
 * \author  Jan-Gerd Mess
 */
template <size_t totalNumberOfElements>
class SmartRingBufferStorage : public SmartRingBuffer
{
public:
	inline
	SmartRingBufferStorage() :
		SmartRingBuffer(outpost::toArray(mBufferStorage), outpost::toArray(mFlags))
    {
    }

    virtual ~SmartRingBufferStorage()
    {
    }

private:
    SmartBufferPointer mBufferStorage[totalNumberOfElements];
    uint8_t mFlags[totalNumberOfElements];
};

}
}

#endif

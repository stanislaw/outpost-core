/*
 * Copyright (c) 2013, German Aerospace Center (DLR)
 * All Rights Reserved.
 *
 * See the file "LICENSE" for the full license governing this code.
 */

#include "semaphore.h"

#include <time.h>
#include <cobc/rtos/failure_handler.h>

#include "internal/time.h"

// ----------------------------------------------------------------------------
cobc::rtos::Semaphore::Semaphore(uint32_t count) : sid()
{
    // shared semaphores are disabled
    if (sem_init(&sid, 0, count) != 0) {
        rtos::FailureHandler::fatal(rtos::FailureCode::resourceAllocationFailed());
    }
}

cobc::rtos::Semaphore::~Semaphore()
{
    sem_destroy(&sid);
}

bool
cobc::rtos::Semaphore::acquire(time::Duration timeout)
{
    timespec t = toRelativeTime(timeout);
    return (sem_timedwait(&sid, &t) == 0);
}

// ----------------------------------------------------------------------------
cobc::rtos::BinarySemaphore::BinarySemaphore(State::Type initial) :
    value(initial)
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&signal, NULL);
}

cobc::rtos::BinarySemaphore::~BinarySemaphore()
{
    pthread_cond_destroy(&signal);
    pthread_mutex_destroy(&mutex);
}

bool
cobc::rtos::BinarySemaphore::acquire()
{
    pthread_mutex_lock(&mutex);
    while (value == State::acquired)
    {
        pthread_cond_wait(&signal, &mutex);
    }
    value = State::acquired;
    pthread_mutex_unlock(&mutex);

    return true;
}

bool
cobc::rtos::BinarySemaphore::acquire(time::Duration timeout)
{
    timespec time = toAbsoluteTime(timeout);

    pthread_mutex_lock(&mutex);
    while (value == State::acquired)
    {
        if (pthread_cond_timedwait(&signal, &mutex, &time) != 0)
        {
            // Timeout or other error has occurred
            // => semaphore can't be acquired
            pthread_mutex_unlock(&mutex);
            return false;
        }
    }
    value = State::acquired;
    pthread_mutex_unlock(&mutex);

    return true;
}

void
cobc::rtos::BinarySemaphore::release()
{
    pthread_mutex_lock(&mutex);
    value = State::released;
    pthread_cond_signal(&signal);
    pthread_mutex_unlock(&mutex);
}

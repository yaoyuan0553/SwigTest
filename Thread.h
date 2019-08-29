//
// Created by yuan on 8/28/19.
//

#pragma once
#ifndef SWIGTEST_THREAD_H
#define SWIGTEST_THREAD_H

#include <thread>

#include "Utility.h"

struct ThreadInterface {
    virtual ~ThreadInterface() = default;

    virtual void run() = 0;
    virtual void runOnCurrentThread() = 0;
    virtual void wait() = 0;
};

class Thread : public ThreadInterface {
protected:
    std::thread thread_;
    bool threadStarted_ = false;

    /* to be implemented by subclasses of this class */
    virtual void internalRun() = 0;

    Thread() = default;
    Thread(Thread&&) noexcept = default;

public:

    /* starts thread execution on a new thread (parallel) */
    void run() override
    {
        thread_ = std::thread(&Thread::internalRun, this);
        threadStarted_ = true;
    }

    /* starts thread execution on current thread (sequential) */
    void runOnCurrentThread() override
    {
        internalRun();
    }

    /* caller blocked until this thread finishes */
    void wait() override
    {
        if (!threadStarted_)
            PERROR("cannot wait() on threads that haven't started");

        thread_.join();
    }
};


#endif //SWIGTEST_THREAD_H

//
// Created by yuan on 8/28/19.
//

#pragma once
#ifndef SWIGTEST_THREADPOOL_H
#define SWIGTEST_THREADPOOL_H

#include "Thread.h"

#include <vector>
#include <type_traits>

/* helper for testing if a given type T is a subclass of Thread */
template <typename T>
using is_thread_subclass_t = std::enable_if_t<std::is_base_of_v<ThreadInterface, T>>;

struct ThreadPoolInterface {
    /**
     * @brief run all threads
     */
    virtual void runAll() = 0;
    /**
     * @brief wait for all threads to finish
     */
    virtual void waitAll() = 0;
    virtual ~ThreadPoolInterface() = default;
};

class ThreadPool : public ThreadPoolInterface {
    std::vector<ThreadInterface*> threadPool_;
public:

    /**
     * @brief add a Thread object with a given class and arguments of its corresponding constructor
     * @tparam ThreadType A subclass of ThreadInterface
     * @tparam Args required Argument types of ThreadType's constructor
     * @param args required arguments of ThreadType's constructor
     */
    template <typename ThreadType, typename... Args, typename = is_thread_subclass_t<ThreadType>>
    void add(Args&&... args)
    {
        threadPool_.push_back(new ThreadType(std::forward<Args>(args)...));
    }

    /**
     * @brief add n Thread objects with the same given class and arguments of its corresponding constructor
     * @tparam ThreadType A subclass of ThreadInterface
     * @tparam Args required Argument types of ThreadType's constructor
     * @param n number of same threads to add
     * @param args required arguments of ThreadType's constructor
     */
    template <typename ThreadType, typename... Args, typename = is_thread_subclass_t<ThreadType>>
    void add(int n, Args&&... args)
    {
        for (int i = 0; i < n; i++)
            threadPool_.push_back(new ThreadType(std::forward<Args>(args)...));
    }

    /**
     * @brief wait for all threads to finish
     */
    void runAll() override
    {
        for (ThreadInterface* ti : threadPool_)
            ti->run();
    }

    /**
     * @brief wait for all threads to finish
     */
    void waitAll() override
    {
        for (ThreadInterface* ti : threadPool_)
            ti->wait();
    }

    /**
     * @brief delete all allocated threads
     */
    ~ThreadPool() override
    {
        for (ThreadInterface* ti : threadPool_)
            delete ti;
    }
};


#endif //SWIGTEST_THREADPOOL_H

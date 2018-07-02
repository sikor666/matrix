#pragma once

#include "Time/Travel.hpp"

#include <future>
#include <iostream>
#include <numeric>
#include <vector>

namespace Future
{

using ElementType = double;
using VectorType = std::vector<ElementType>;
using VectorIterator = VectorType::iterator;
using TaskType = ElementType(VectorIterator, VectorIterator, ElementType);

// task: place the value in px
auto set = [](
    std::promise<ElementType>&& px,
    std::function<TaskType> task,
    VectorIterator begin,
    VectorIterator end)
{
    try
    {
        px.set_value(task(begin, end, 0.0));
    }
    catch (...)
    {
        // pass the exception to the future's thread
        px.set_exception(std::current_exception());
    }
};

// task: get the result from fx
auto get = [](std::future<ElementType>& fx)
{
    try
    {
        // if necessary, wait for the value to get computed
        return fx.get();
    }
    catch (...)
    {
    }

    return ElementType{};
};

// compute the sum of [beg:end) starting with the initial value init
auto accum = [](VectorIterator begin, VectorIterator end, ElementType init)
{
    return std::accumulate(begin, end, init);
};

class ITask
{
public:
    virtual ElementType execute() = 0;

    virtual ~ITask() = default;
};

class FromPackagedTask : public ITask
{
public:
    FromPackagedTask(VectorType& vec) : v(vec)
    {
    }

    ElementType execute()
    {
        std::packaged_task<TaskType> pt0{ accum };          // package the task (i.e., accum)
        std::packaged_task<TaskType> pt1{ accum };
        std::packaged_task<TaskType> pt2{ accum };
        std::packaged_task<TaskType> pt3{ accum };

        std::future<ElementType> f0{ pt0.get_future() };    // get hold of pt0’s future
        std::future<ElementType> f1{ pt1.get_future() };    // get hold of pt1’s future
        std::future<ElementType> f2{ pt2.get_future() };
        std::future<ElementType> f3{ pt3.get_future() };

        auto v0 = std::begin(v);
        auto sz = std::size(v);

        // start a thread for ptx, move is necessary because ptx is non-copyable object,
        // copy constructor and assignment operator is deleted, std::packaged_task is move-only
        std::thread t0{ std::move(pt0), v0            , v0 + sz * 0.25, 0.0 };
        std::thread t1{ std::move(pt1), v0 + sz * 0.25, v0 + sz * 0.50, 0.0 };
        std::thread t2{ std::move(pt2), v0 + sz * 0.50, v0 + sz * 0.75, 0.0 };
        std::thread t3{ std::move(pt3), v0 + sz * 0.75, v0 + sz       , 0.0 };

        t0.detach();
        t1.detach();
        t2.detach();
        t3.detach();

        return get(f0) + get(f1) + get(f2) + get(f3);
    }

private:
    VectorType& v;
};

class FromAsync : public ITask
{
public:
    FromAsync(VectorType& vec) : v(vec)
    {
    }

    ElementType execute()
    {
        auto v0 = std::begin(v);
        auto sz = std::size(v);

        auto f0 = std::async(std::launch::async, accum, v0,             v0 + sz * 0.25, 0.0);
        auto f1 = std::async(std::launch::async, accum, v0 + sz * 0.25, v0 + sz * 0.50, 0.0);
        auto f2 = std::async(std::launch::async, accum, v0 + sz * 0.50, v0 + sz * 0.75, 0.0);
        auto f3 = std::async(std::launch::async, accum, v0 + sz * 0.75, v0 + sz       , 0.0);

        return get(f0) + get(f1) + get(f2) + get(f3);
    }

private:
    VectorType& v;
};

class FromPromise : public ITask
{
public:
    FromPromise(VectorType& vec) : v(vec)
    {
    }

    ElementType execute()
    {
        std::promise<ElementType> p0;
        std::promise<ElementType> p1;
        std::promise<ElementType> p2;
        std::promise<ElementType> p3;

        std::future<ElementType> f0 = p0.get_future();
        std::future<ElementType> f1 = p1.get_future();
        std::future<ElementType> f2 = p2.get_future();
        std::future<ElementType> f3 = p3.get_future();

        auto v0 = std::begin(v);
        auto sz = std::size(v);

        // std::promise is move-only, copy constructor and assignment operator is deleted
        std::thread t0{ set, std::move(p0), accum, v0            , v0 + sz * 0.25 };
        std::thread t1{ set, std::move(p1), accum, v0 + sz * 0.25, v0 + sz * 0.50 };
        std::thread t2{ set, std::move(p2), accum, v0 + sz * 0.50, v0 + sz * 0.75 };
        std::thread t3{ set, std::move(p3), accum, v0 + sz * 0.75, v0 + sz        };

        t0.detach();
        t1.detach();
        t2.detach();
        t3.detach();

        return get(f0) + get(f1) + get(f2) + get(f3);
    }

private:
    VectorType& v;
};

class Looper
{
private:
    std::vector<std::unique_ptr<ITask>> tasks;

public:
    void add(std::unique_ptr<ITask>&& command)
    {
        tasks.push_back(std::move(command));
    }

    void run()
    {
        for (const auto &task : tasks)
        {
            Time::Travel<> travel;

            auto result = task->execute();

            std::cout << "f(v) = " << result << ", t = " << travel.distance() << std::endl;
        }
    }
};

} // namespace Future

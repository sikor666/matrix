#include <future>
#include <numeric>
#include <iostream>

using ElementType = double;
using VectorType = std::vector<ElementType>;
using VectorIterator = VectorType::iterator;
using TaskType = ElementType(VectorIterator, VectorIterator, ElementType);

// task: place the value in px pointer wtf
auto set = [](
    std::promise<ElementType>* px,
    std::function<TaskType> task,
    VectorIterator begin,
    VectorIterator end)
{
    try
    {
        px->set_value(task(begin, end, 0.0));
    }
    catch (...)
    {
        // pass the exception to the future's thread
        px->set_exception(std::current_exception());
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

// TODO(sikor): template, decltype, tests, parts, time
auto compt(VectorType& v)
{
    std::packaged_task<TaskType> pt0{ accum };          // package the task (i.e., accum)
    std::packaged_task<TaskType> pt1{ accum };

    std::future<ElementType> f0{ pt0.get_future() };    // get hold of pt0’s future
    std::future<ElementType> f1{ pt1.get_future() };    // get hold of pt1’s future

    auto begin = std::begin(v);
    auto end = std::end(v);
    auto size = std::size(v);

    // start a thread for ptx, move is necessary because ptx is non-copyable object,
    // copy constructor is deleted, std::packaged_task is move-only.
    std::thread t1{ std::move(pt0), begin, begin + size / 2, 0.0 };
    std::thread t2{ std::move(pt1), begin + size / 2, end, 0.0 };

    t1.join();
    t2.join();

    return get(f0) + get(f1);
}

auto compa(VectorType& v)
{
    auto v0 = std::begin(v);
    auto sz = std::size(v);
    auto f0 = std::async(std::launch::async, accum, v0            , v0 + sz * 0.25, 0.0);
    auto f1 = std::async(std::launch::async, accum, v0 + sz * 0.25, v0 + sz * 0.50, 0.0);
    auto f2 = std::async(std::launch::async, accum, v0 + sz * 0.50, v0 + sz * 0.75, 0.0);
    auto f3 = std::async(std::launch::async, accum, v0 + sz * 0.75, v0 + sz       , 0.0);

    return get(f0) + get(f1) + get(f2) + get(f3);
}

auto compp(VectorType& v)
{
    std::promise<ElementType> p0;
    std::promise<ElementType> p1;

    std::future<ElementType> f0 = p0.get_future();
    std::future<ElementType> f1 = p1.get_future();

    auto begin = std::begin(v);
    auto end = std::end(v);
    auto size = std::size(v);

    std::thread t1{ set, &p0, accum, begin, begin + size / 2 };
    std::thread t2{ set, &p1, accum, begin + size / 2, end };

    t1.detach();
    t2.detach();;

    return get(f0) + get(f1);
}

int main()
{
    VectorType v{ 0,1,2,3,4,5,6,7,8 };

    std::cout << compt(v) << std::endl;
    std::cout << compa(v) << std::endl;
    std::cout << compp(v) << std::endl;

    std::system("pause");
}

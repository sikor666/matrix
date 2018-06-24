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

// TODO(sikor): template, decltype, tests, parts
auto compt(VectorType& v)
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
    // copy constructor is deleted, std::packaged_task is move-only.
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
    std::promise<ElementType> p2;
    std::promise<ElementType> p3;

    std::future<ElementType> f0 = p0.get_future();
    std::future<ElementType> f1 = p1.get_future();
    std::future<ElementType> f2 = p2.get_future();
    std::future<ElementType> f3 = p3.get_future();

    auto v0 = std::begin(v);
    auto sz = std::size(v);

    std::thread t0{ set, &p0, accum, v0            , v0 + sz * 0.25 };
    std::thread t1{ set, &p1, accum, v0 + sz * 0.25, v0 + sz * 0.50 };
    std::thread t2{ set, &p2, accum, v0 + sz * 0.50, v0 + sz * 0.75 };
    std::thread t3{ set, &p3, accum, v0 + sz * 0.75, v0 + sz        };

    t0.detach();
    t1.detach();
    t2.detach();
    t3.detach();

    return get(f0) + get(f1) + get(f2) + get(f3);
}

int main()
{
    using namespace std::chrono;

    try
    {
        VectorType v(100000000);

        std::generate(std::begin(v), std::end(v), [n = 0]() mutable { return n++; });

        auto tt0 = high_resolution_clock::now();
        auto ctr = compt(v);
        auto tt1 = high_resolution_clock::now();

        auto ta0 = high_resolution_clock::now();
        auto car = compa(v);
        auto ta1 = high_resolution_clock::now();

        auto tp0 = high_resolution_clock::now();
        auto ctp = compp(v);
        auto tp1 = high_resolution_clock::now();

        std::cout << "ct(v) = " << ctr << ", t = " << duration_cast<microseconds>(tt1 - tt0).count() << std::endl;
        std::cout << "ca(v) = " << car << ", t = " << duration_cast<microseconds>(ta1 - ta0).count() << std::endl;
        std::cout << "cp(v) = " << ctp << ", t = " << duration_cast<microseconds>(tp1 - tp0).count() << std::endl;

    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught exception \"" << ex.what() << "\"\n";
    }

    std::system("pause");
}

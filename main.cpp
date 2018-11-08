#include "Future/Looper.hpp"

#include <algorithm>
#include <iostream>

int main()
{
    try
    {
        Future::VectorType v(1000000);
        std::generate(std::begin(v), std::end(v), [n = 0]() mutable { return n++; });

        Future::Looper looper;

        looper.add(std::make_unique<Future::FromPackagedTask>(v));
        looper.add(std::make_unique<Future::FromAsync>(v));
        looper.add(std::make_unique<Future::FromPromise>(v));

        looper.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught exception \"" << ex.what() << "\"\n";
    }

    std::system("pause");
}

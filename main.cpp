#include "Future/Looper.hpp"

int main()
{
    try
    {
        using namespace Future;

        VectorType v(1000000);
        std::generate(std::begin(v), std::end(v), [n = 0]() mutable { return n++; });

        Looper looper;

        looper.add(std::make_unique<FromPackagedTask>(v));
        looper.add(std::make_unique<FromAsync>(v));
        looper.add(std::make_unique<FromPromise>(v));

        looper.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught exception \"" << ex.what() << "\"\n";
    }

    std::system("pause");
}

#include "ITask.hpp"
#include "Future/Looper.hpp"
#include "Image/Matrix.hpp"

#include <boost/program_options.hpp>

#include <algorithm>

namespace po = boost::program_options;

int main(int ac, char* av[])
{
    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("image", po::value<std::string>(), "set image data")
            ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        std::string resource;

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 0;
        }

        if (vm.count("image"))
        {
            resource = vm["image"].as<std::string>();

            std::cout << "Image data was set to " << image << ".\n";

        }
        else
        {
            std::cout << "Image data was not set.\n";
        }

        VectorType v(1000000);
        std::generate(std::begin(v), std::end(v), [n = 0]() mutable { return n++; });

        Future::Looper looper;

        looper.add(std::make_unique<Future::FromPackagedTask>(v));
        looper.add(std::make_unique<Future::FromAsync>(v));
        looper.add(std::make_unique<Future::FromPromise>(v));
        looper.add(std::make_unique<Image::LoadFromRes>(resource));

        looper.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught exception \"" << ex.what() << "\"\n";
    }
}

#include <iostream>

int main()
{
    try
    {
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught exception \"" << ex.what() << "\"\n";
    }

    std::system("pause");
}

#include <iostream>
#include <array>

int main()
{
    std::array<int, 6> data = {1,2,3,4,5,6};

    data.at(1) = 88;

    std::cout << "data.at(2)" << data.at(2) << std::endl;

    return 0;
}
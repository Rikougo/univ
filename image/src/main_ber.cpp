#include <iostream>
#include <stbi_image.h>
#include <common.hpp>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Please input two images." << std::endl;
        exit(-1);
    }

    std::cout << Common::ComputeBER(argv[1], argv[2]) << std::endl;
}
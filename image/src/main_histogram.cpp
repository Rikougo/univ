#include <iostream>
#include <common.hpp>
#include <stbi_image.h>
#include <array>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Please input an image path." << std::endl;
        exit(-1);
    }

    std::vector<std::array<int, 256>> l_histogram = Common::ComputeHistogram(argv[1]);

    for(int l_index = 0; l_index < 256; l_index++) {
        std::cout << l_index << " ";
        for(int l_cIndex = 0; l_cIndex < l_histogram.size(); l_cIndex++) {
            std::cout << l_histogram[l_cIndex][l_index] << " ";
        }
        std::cout << std::endl;
    }
}
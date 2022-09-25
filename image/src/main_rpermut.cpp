#include <iostream>
#include <string>
#include <common.hpp>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: rpermut <input_image> <output_image> <key>" << std::endl;
        exit(-1);
    }

    Common::ImageData l_rPermutData = Common::RevertPermut(argv[1], std::stoi(argv[3]));
    Common::ImageData::SaveImage(l_rPermutData, argv[2]);
}
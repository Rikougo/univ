#include <iostream>
#include <string>
#include <common.hpp>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: encrypt <input_image> <output_image> <key>" << std::endl;
        exit(-1);
    }

    Common::ImageData l_encrpyted = Common::EncryptData(argv[1], std::stoi(argv[3]));

    Common::ImageData::SaveImage(l_encrpyted, argv[2]);
}
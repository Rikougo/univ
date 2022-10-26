#include <iostream>
#include <string>
#include <common.hpp>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: decrypt <input_image> <output_image> <key>" << std::endl;
        exit(-1);
    }

    Common::ImageData l_decrypted = Common::DecryptData(argv[1], std::stoi(argv[3]));
    Common::ImageData::SaveImage(l_decrypted, argv[2]);
}
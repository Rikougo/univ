#include <iostream>
#include <string>
#include <common.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: encrypt <input_image> <output_image> <key>" << std::endl;
        exit(-1);
    }

    Common::ImageData l_encrpyted = Common::EncryptData(argv[1], std::stoi(argv[3]));

    stbi_write_png(argv[2], l_encrpyted.width, l_encrpyted.height, l_encrpyted.channels, l_encrpyted.data, 0);
}
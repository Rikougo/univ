#include <iostream>
#include <string>
#include <common.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: decrypt <input_image> <output_image> <key>" << std::endl;
        exit(-1);
    }

    Common::ImageData l_decrypted = Common::DecryptData(argv[1], std::stoi(argv[3]));

    stbi_write_png(argv[2], l_decrypted.width, l_decrypted.height, l_decrypted.channels, l_decrypted.data, 0);
}
#include <iostream>
#include <string>
#include <common.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: decrypt_bf <input_image> <output_image>" << std::endl;
        exit(-1);
    }

    Common::ImageData l_result;
    float l_minEntropy = FLT_MAX;
    int l_goodKey = 0;

    for(int l_testKey = 0; l_testKey < 256; l_testKey++) {
        Common::ImageData l_decrypted = Common::DecryptData(argv[1], l_testKey);

        float l_entropy = Common::ComputeEntropy(l_decrypted);

        if (l_entropy < l_minEntropy) {
            std::cout << l_entropy << std::endl;
            l_minEntropy = l_entropy;
            l_goodKey = l_testKey;
            l_result = l_decrypted;
        }
    }

    std::cout << "Found key : " << l_goodKey << "; Entropy : " << l_minEntropy << std::endl;

    stbi_write_png(argv[2], l_result.width, l_result.height, l_result.channels, l_result.data, 0);
}
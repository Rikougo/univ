#include <vector>
#include <array>
#include <iostream>
#include <random>

namespace Common {
    struct ImageData {
        unsigned char* data;
        int width, height, channels;

        static ImageData FromPath(const char* p_path);
    };

    // METRICS
    float ComputePSNR(const char* p_fPath, const char* p_sPath);
    float ComputePSNR(ImageData &p_fData, ImageData &p_sData);

    float ComputEntropy(const char* p_path);
    float ComputeEntropy(ImageData &p_data);

    std::vector<std::array<int, 256>> ComputeHistogram(const char* p_path);
    std::vector<std::array<int, 256>> ComputeHistogram(ImageData &p_data);

    // PERMUTATION
    ImageData PermutData(const char* p_path, int p_key);
    ImageData RevertPermut(const char* p_path, int p_key);

    // ENCRYPTION
    ImageData EncryptData(const char* p_path, int p_key);
    ImageData DecryptData(const char* p_path, int p_key);
}
#include <vector>
#include <array>
#include <iostream>
#include <random>
#include <string>

namespace Common
{
    struct ImageData
    {
        unsigned char *data;
        int width, height, channels;

        int TotalSize() { return width * height * channels; }
        static ImageData FromPath(const char *p_path);
        static void SaveImage(ImageData &p_data, const char *p_path);
        static void SaveImage(ImageData &p_data, std::string p_path);
    };

    using ImagePair = std::pair<ImageData, ImageData>;

    static int SUBSTITUTE_BIT = 0;

    // METRICS
    float ComputePSNR(const char *p_fPath, const char *p_sPath);
    float ComputePSNR(ImageData &p_fData, ImageData &p_sData);

    float ComputEntropy(const char *p_path);
    float ComputeEntropy(ImageData &p_data);

    std::vector<std::array<int, 256>> ComputeHistogram(const char *p_path);
    std::vector<std::array<int, 256>> ComputeHistogram(ImageData &p_data);

    // PERMUTATION
    ImageData PermutData(const char *p_path, int p_key);
    ImageData RevertPermut(const char *p_path, int p_key);

    // ENCRYPTION
    ImageData EncryptData(const char *p_path, int p_key);
    ImageData DecryptData(const char *p_path, int p_key);

    std::pair<ImagePair, ImagePair> EncryptECB(const char *p_path, unsigned char p_key);
    std::pair<ImagePair, ImagePair> EncryptCBC(const char *p_path, unsigned char p_key);
    std::pair<ImagePair, ImagePair> EncryptCFB(const char *p_path, unsigned char p_key);
    std::pair<ImagePair, ImagePair> EncryptOFB(const char *p_path, unsigned char p_key);

    int swapBits(unsigned int n, unsigned int p1, unsigned int p2);
    ImageData RandomNoise(const ImageData &p_data);

    ImagePair HideData(ImageData const &p_source, unsigned char *const p_data, int p_dataLen, int p_key, int p_substitute);
}
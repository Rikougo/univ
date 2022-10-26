#include <vector>
#include <array>
#include <iostream>
#include <random>
#include <string>
#include <functional>

namespace Common
{
    enum PoolingType {
        MAX,
        AVERAGE,
        SUM
    };

    struct ImageData
    {
        unsigned char *data;
        int width, height, channels;

        int TotalSize() const { return width * height * channels; }
        static ImageData FromPath(const char *p_path);
        static void SaveImage(ImageData &p_data, const char *p_path);
        static void SaveImage(ImageData &p_data, std::string p_path);
    };

    struct ConvolutionalFilter {
        unsigned int width, height;
        float* data;

        unsigned char ApplyFilter(ImageData const &p_source, unsigned int p_x, unsigned int p_y, unsigned int p_c) const;
    };
    
    struct Node
    {
        float value;
        std::vector<float> weights;
    };

    struct NNSize
    {
        size_t layerAmount;
        size_t layerSize;
    };

    class NeuralNetwork
    {
    public:
        static const size_t FIRST_LAYER = 0;

    public:
        NeuralNetwork(size_t p_layerAmount, size_t p_layerSize);
        ~NeuralNetwork() = default;

        void setRandomWeights(size_t p_seed);
        void setActivation(std::function<float(float)> p_activation);
        std::vector<float> work(std::vector<float> p_firstLayer);

        NNSize size() const noexcept;
        std::vector<Node> at(size_t p_layerIndex);

    private:
        std::vector<float> propagate(size_t p_layerIndex);

    private:
        std::vector<Node> m_layers;
        std::function<float(float)> m_activation;

        NNSize m_size;
    };

    using ImagePair = std::pair<ImageData, ImageData>;

    static int SUBSTITUTE_BIT = 0;

    // METRICS
    float ComputePSNR(const char *p_fPath, const char *p_sPath);
    float ComputePSNR(ImageData &p_fData, ImageData &p_sData);

    float ComputEntropy(const char *p_path);
    float ComputeEntropy(ImageData &p_data);

    float ComputeBER(ImageData const &p_left, ImageData const &p_right);
    float ComputeBER(const char *p_left, const char *p_right);

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

    ImageData ApplyCNFilter(ImageData const &p_source, ConvolutionalFilter const &p_filter);
    ImageData Pooling(ImageData const &p_source, PoolingType p_type);
}
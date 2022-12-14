#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image.h>
#include <stbi_image_write.h>
#include <AES.h>
#include <common.hpp>

namespace Common
{
    ImageData ImageData::FromPath(const char *p_path)
    {
        int l_width, l_height, l_channels;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);

        ImageData l_imgData;
        l_imgData.data = l_data;
        l_imgData.width = l_width;
        l_imgData.height = l_height;
        l_imgData.channels = l_channels;

        return l_imgData;
    }

    void ImageData::SaveImage(ImageData &p_data, const char *p_path)
    {
        stbi_write_png(p_path, p_data.width, p_data.height, p_data.channels, p_data.data, 0);
    }

    void ImageData::SaveImage(ImageData &p_data, std::string p_path)
    {
        stbi_write_png(p_path.c_str(), p_data.width, p_data.height, p_data.channels, p_data.data, 0);
    }

    float ComputePSNR(ImageData &p_fData, ImageData &p_sData)
    {
        if (p_sData.width != p_fData.width || p_sData.height != p_fData.height || p_fData.channels != p_sData.channels)
        {
            return 0.0f;
        }

        int l_size = p_fData.width * p_fData.height;

        int l_totalDiff = 0;
        for (int l_index = 0; l_index < l_size * p_fData.channels; l_index += p_fData.channels)
        {
            int l_dist = 0;

            for (int l_cIndex = 0; l_cIndex < p_fData.channels; l_cIndex++)
            {
                int l_diff = (int)p_fData.data[l_index + l_cIndex] - (int)p_sData.data[l_index + l_cIndex];
                l_dist += l_diff * l_diff;
            }

            l_totalDiff += l_dist;
        }

        return 10 * log10((255.0f * 255.0f) / ((float)l_totalDiff / (float)l_size));
    }
    float ComputePSNR(const char *p_fPath, const char *p_sPath)
    {
        auto l_first = ImageData::FromPath(p_fPath);
        auto l_second = ImageData::FromPath(p_sPath);
        return ComputePSNR(l_first, l_second);
    }

    std::vector<std::array<int, 256>> ComputeHistogram(ImageData &p_data)
    {

        std::vector<std::array<int, 256>> l_histogram(p_data.channels, std::array<int, 256>{0});

        int l_size = p_data.width * p_data.height;
        for (int l_index = 0; l_index < l_size; l_index++)
        {
            int l_cursor = l_index * p_data.channels;
            for (int l_cIndex = 0; l_cIndex < p_data.channels; l_cIndex++)
            {
                l_histogram[l_cIndex][p_data.data[l_cursor + l_cIndex]]++;
            }
        }

        return l_histogram;
    }
    std::vector<std::array<int, 256>> ComputeHistogram(const char *p_path)
    {
        auto l_image = ImageData::FromPath(p_path);
        return ComputeHistogram(l_image);
    }

    float ComputEntropy(const char *p_path)
    {
        auto l_image = ImageData::FromPath(p_path);
        return ComputeEntropy(l_image);
    }
    float ComputeEntropy(ImageData &p_data)
    {
        std::vector<std::array<int, 256>> l_histogram = ComputeHistogram(p_data);

        float l_sum = 0.0f;
        for (int i = 0; i < 256; i++)
        {
            float l_value = 0.0f;
            for (int l_cIndex = 0; l_cIndex < l_histogram.size(); l_cIndex++)
            {
                l_value += (l_histogram[l_cIndex][i] * 1.0f) / (p_data.width * p_data.height * 1.0f);
            }
            l_value /= 3.0f;
            if (l_value == 0.0f)
                continue;
            l_sum -= l_value * (log(l_value) / log(2));
        }

        return l_sum;
    }

    float ComputeBER(ImageData const &p_left, ImageData const &p_right)
    {
        if (p_left.TotalSize() != p_right.TotalSize())
            return 1.0f;

        int l_ber = 0;
        for (int i = 0; i < p_left.TotalSize(); i++)
        {
            for (int j = 0; j < 8; j++)
            {
                unsigned char l_leftValue = (p_left.data[i] & (1 << j));
                unsigned char l_rightValue = (p_right.data[i] & (1 << j));

                if (l_leftValue != l_rightValue)
                    l_ber++;
            }
        }

        return (float)l_ber / (float)p_left.TotalSize();
    }
    float ComputeBER(const char *p_left, const char *p_right)
    {
        return ComputeBER(ImageData::FromPath(p_left), ImageData::FromPath(p_right));
    }

    ImageData PermutData(const char *p_path, int p_key)
    {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        std::size_t l_size = l_width * l_height;

        std::cout << l_width << "x" << l_height << " (" << l_size << ")" << std::endl;

        unsigned char *l_newData = new unsigned char[l_size * l_channels];
        bool *l_writenPos = new bool[l_size];
        for (int l_index = 0; l_index < l_size; l_index++)
            l_writenPos[l_index] = false;

        std::default_random_engine l_generator;
        l_generator.seed(p_key);
        std::uniform_int_distribution<int> l_distribution(0, l_size - 1);
        for (std::size_t l_index = 0; l_index < l_size * l_channels; l_index += l_channels)
        {
            int l_randomPos = l_distribution(l_generator);

            while (l_writenPos[l_randomPos])
            {
                l_randomPos = (l_randomPos + 1) % l_size;
            }

            for (int l_cIndex = 0; l_cIndex < l_channels; l_cIndex++)
            {
                l_newData[l_randomPos * l_channels + l_cIndex] = l_data[l_index + l_cIndex];
            }

            l_writenPos[l_randomPos] = true;
        }

        ImageData l_result;
        l_result.data = l_newData;
        l_result.width = l_width;
        l_result.height = l_height;
        l_result.channels = l_channels;
        return l_result;
    }
    ImageData RevertPermut(const char *p_path, int p_key)
    {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);

        std::size_t l_size = l_width * l_height;

        unsigned char *l_decryptedData = new unsigned char[l_size * 3];
        bool *l_readData = new bool[l_size];
        for (int l_index = 0; l_index < l_size; l_index++)
        {
            l_readData[l_index] = false;
        }

        std::default_random_engine l_generator;
        l_generator.seed(p_key);
        std::uniform_int_distribution<int> l_distribution(0, l_size - 1);
        for (std::size_t l_index = 0; l_index < l_size * l_channels; l_index += l_channels)
        {
            int l_randomPos = l_distribution(l_generator);

            while (l_readData[l_randomPos])
                l_randomPos = (l_randomPos + 1) % l_size;

            for (int l_cIndex = 0; l_cIndex < l_channels; l_cIndex++)
                l_decryptedData[l_index + l_cIndex] = l_data[l_randomPos * l_channels + l_cIndex];

            l_readData[l_randomPos] = true;
        }

        ImageData l_result;
        l_result.data = l_decryptedData;
        l_result.width = l_width;
        l_result.height = l_height;
        l_result.channels = l_channels;
        return l_result;
    }

    ImageData EncryptData(const char *p_path, int p_key)
    {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        std::size_t l_size = l_width * l_height;
        unsigned char *l_newData = new unsigned char[l_size * l_channels];

        std::default_random_engine l_generator;
        l_generator.seed(p_key);
        std::uniform_int_distribution<int> l_distribution(0, l_size - 1);
        for (std::size_t l_index = 0; l_index < l_size * l_channels; l_index++)
        {
            char l_value = 0;

            int l_rValue = l_distribution(l_generator);

            if (l_index == 0)
            {
                l_value = (l_rValue + l_data[l_index]) % 256;
            }
            else
            {
                l_value = (l_newData[l_index - 1] + l_data[l_index] + l_rValue) % 256;
            }

            l_newData[l_index] = l_value;
        }

        ImageData l_result;
        l_result.data = l_newData;
        l_result.width = l_width;
        l_result.height = l_height;
        l_result.channels = l_channels;
        return l_result;
    }
    ImageData DecryptData(const char *p_path, int p_key)
    {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        std::size_t l_size = l_width * l_height;
        unsigned char *l_newData = new unsigned char[l_size * l_channels];

        std::default_random_engine l_generator;
        l_generator.seed(p_key);
        std::uniform_int_distribution<int> l_distribution(0, l_size - 1);
        for (std::size_t l_index = 0; l_index < l_size * l_channels; l_index++)
        {
            char l_value = 0;
            int l_rValue = l_distribution(l_generator);

            if (l_index == 0)
            {
                l_value = (l_data[l_index] - l_rValue) % 256;
            }
            else
            {
                l_value = (l_data[l_index] - l_data[l_index - 1] - l_rValue) % 256;
            }

            l_newData[l_index] = l_value;
        }

        ImageData l_result;
        l_result.data = l_newData;
        l_result.width = l_width;
        l_result.height = l_height;
        l_result.channels = l_channels;
        return l_result;
    }

    std::pair<ImagePair, ImagePair> EncryptECB(const char *p_path, unsigned char p_key)
    {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        unsigned char l_key[1] = {p_key};

        AES l_aes = AES(AESKeyLength::AES_128);

        unsigned char *l_encrypted = l_aes.EncryptECB(l_data, l_width * l_height * l_channels, l_key);
        unsigned char *l_decrypted = l_aes.DecryptECB(l_encrypted, l_width * l_height * l_channels, l_key);

        ImageData l_resultEncrypted;
        l_resultEncrypted.data = l_encrypted;
        l_resultEncrypted.width = l_width;
        l_resultEncrypted.height = l_height;
        l_resultEncrypted.channels = l_channels;

        ImageData l_noiseEncrypted = RandomNoise(l_resultEncrypted);
        ImageData l_noiseDecrypted;
        l_noiseDecrypted.data = l_aes.DecryptECB(l_noiseEncrypted.data, l_width * l_height * l_channels, l_key);
        l_noiseDecrypted.width = l_width;
        l_noiseDecrypted.height = l_height;
        l_noiseDecrypted.channels = l_channels;

        ImageData l_resultDecrypted;
        l_resultDecrypted.data = l_decrypted;
        l_resultDecrypted.width = l_width;
        l_resultDecrypted.height = l_height;
        l_resultDecrypted.channels = l_channels;

        return {{l_resultEncrypted, l_resultDecrypted}, {l_noiseEncrypted, l_noiseDecrypted}};
    }

    std::pair<ImagePair, ImagePair> EncryptCBC(const char *p_path, unsigned char p_key)
    {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        unsigned char l_key[1] = {p_key};

        AES l_aes = AES(AESKeyLength::AES_128);

        unsigned char *l_initVector = new unsigned char[8];
        for (int i = 0; i < 8; i++)
            l_initVector[i] = 0;

        unsigned char *l_encrypted = l_aes.EncryptCBC(l_data, l_width * l_height * l_channels, l_key, l_initVector);
        unsigned char *l_decrypted = l_aes.DecryptCBC(l_encrypted, l_width * l_height * l_channels, l_key, l_initVector);

        ImageData l_resultEncrypted;
        l_resultEncrypted.data = l_encrypted;
        l_resultEncrypted.width = l_width;
        l_resultEncrypted.height = l_height;
        l_resultEncrypted.channels = l_channels;

        ImageData l_noiseEncrypted = RandomNoise(l_resultEncrypted);
        ImageData l_noiseDecrypted;
        l_noiseDecrypted.data = l_aes.DecryptCBC(l_noiseEncrypted.data, l_width * l_height * l_channels, l_key, l_initVector);
        l_noiseDecrypted.width = l_width;
        l_noiseDecrypted.height = l_height;
        l_noiseDecrypted.channels = l_channels;

        ImageData l_resultDecrypted;
        l_resultDecrypted.data = l_decrypted;
        l_resultDecrypted.width = l_width;
        l_resultDecrypted.height = l_height;
        l_resultDecrypted.channels = l_channels;

        return {{l_resultEncrypted, l_resultDecrypted}, {l_noiseEncrypted, l_noiseDecrypted}};
    }

    std::pair<ImagePair, ImagePair> EncryptCFB(const char *p_path, unsigned char p_key)
    {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        unsigned char l_key[1] = {p_key};

        AES l_aes = AES(AESKeyLength::AES_128);

        unsigned char *l_initVector = new unsigned char[8];
        for (int i = 0; i < 8; i++)
            l_initVector[i] = 0;

        unsigned char *l_encrypted = l_aes.EncryptCFB(l_data, l_width * l_height * l_channels, l_key, l_initVector);
        unsigned char *l_decrypted = l_aes.DecryptCFB(l_encrypted, l_width * l_height * l_channels, l_key, l_initVector);

        ImageData l_resultEncrypted;
        l_resultEncrypted.data = l_encrypted;
        l_resultEncrypted.width = l_width;
        l_resultEncrypted.height = l_height;
        l_resultEncrypted.channels = l_channels;

        ImageData l_noiseEncrypted = RandomNoise(l_resultEncrypted);
        ImageData l_noiseDecrypted;
        l_noiseDecrypted.data = l_aes.DecryptCFB(l_noiseEncrypted.data, l_width * l_height * l_channels, l_key, l_initVector);
        l_noiseDecrypted.width = l_width;
        l_noiseDecrypted.height = l_height;
        l_noiseDecrypted.channels = l_channels;

        ImageData l_resultDecrypted;
        l_resultDecrypted.data = l_decrypted;
        l_resultDecrypted.width = l_width;
        l_resultDecrypted.height = l_height;
        l_resultDecrypted.channels = l_channels;

        return {{l_resultEncrypted, l_resultDecrypted}, {l_noiseEncrypted, l_noiseDecrypted}};
    }

    std::pair<ImagePair, ImagePair> EncryptOFB(const char *p_path, unsigned char p_key)
    {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        unsigned char l_key[1] = {p_key};

        AES l_aes = AES(AESKeyLength::AES_128);

        unsigned char *l_initVector = new unsigned char[8];
        for (int i = 0; i < 8; i++)
            l_initVector[i] = 0;

        unsigned char *l_encrypted = l_aes.EncryptOFB(l_data, l_width * l_height * l_channels, l_key, l_initVector);
        unsigned char *l_decrypted = l_aes.DecryptOFB(l_encrypted, l_width * l_height * l_channels, l_key, l_initVector);

        ImageData l_resultEncrypted;
        l_resultEncrypted.data = l_encrypted;
        l_resultEncrypted.width = l_width;
        l_resultEncrypted.height = l_height;
        l_resultEncrypted.channels = l_channels;

        ImageData l_noiseEncrypted = RandomNoise(l_resultEncrypted);
        ImageData l_noiseDecrypted;
        l_noiseDecrypted.data = l_aes.DecryptOFB(l_noiseEncrypted.data, l_width * l_height * l_channels, l_key, l_initVector);
        l_noiseDecrypted.width = l_width;
        l_noiseDecrypted.height = l_height;
        l_noiseDecrypted.channels = l_channels;

        ImageData l_resultDecrypted;
        l_resultDecrypted.data = l_decrypted;
        l_resultDecrypted.width = l_width;
        l_resultDecrypted.height = l_height;
        l_resultDecrypted.channels = l_channels;

        return {{l_resultEncrypted, l_resultDecrypted}, {l_noiseEncrypted, l_noiseDecrypted}};
    }

    int swapBits(unsigned int n, unsigned int p1, unsigned int p2)
    {
        /* Move p1'th to rightmost side */
        unsigned int bit1 = (n >> p1) & 1;

        /* Move p2'th to rightmost side */
        unsigned int bit2 = (n >> p2) & 1;

        /* XOR the two bits */
        unsigned int x = (bit1 ^ bit2);

        /* Put the xor bit back to their original positions */
        x = (x << p1) | (x << p2);

        /* XOR 'x' with the original number so that the
        two sets are swapped */
        unsigned int result = n ^ x;

        return result;
    }

    ImageData RandomNoise(const ImageData &p_data)
    {
        ImageData l_result;
        l_result.width = p_data.width;
        l_result.height = p_data.height;
        l_result.channels = p_data.channels;

        size_t l_totalSize = l_result.width * l_result.height * l_result.channels;

        unsigned char *l_noiseData = new unsigned char[l_totalSize];

        std::cout << "Adding noise" << std::endl;

        std::default_random_engine l_generator;
        std::uniform_int_distribution<int> l_distribution(0, 9);
        std::uniform_int_distribution<int> l_bitDistribution(0, 7);
        for (int i = 0; i < l_totalSize; i++)
        {
            unsigned char l_value = p_data.data[i];
            if (l_distribution(l_generator) < 2)
            {
                std::pair<int, int> l_swap{l_bitDistribution(l_generator), l_bitDistribution(l_generator)};
                l_noiseData[i] = swapBits((unsigned int)l_value, l_swap.first, l_swap.second);
            }
            else
            {
                l_noiseData[i] = l_value;
            }
        }

        l_result.data = l_noiseData;

        return l_result;
    }

    ImagePair HideData(ImageData const &p_source, unsigned char *const p_data, int p_dataLen, int p_key, int p_substitute = 0)
    {
        SUBSTITUTE_BIT = p_substitute;

        ImageData l_result;
        l_result.width = p_source.width;
        l_result.height = p_source.height;
        l_result.channels = p_source.channels;
        l_result.data = new unsigned char[l_result.TotalSize()];
        memcpy(l_result.data, p_source.data, l_result.TotalSize());

        if (p_dataLen * 8 >= l_result.TotalSize())
        {
            std::cout << "Hidden data is too wide for image capacity.";
            exit(-1);
        }

#ifdef _WIN32
        std::default_random_engine l_generator;
        std::uniform_int_distribution<int> l_distribution(0, l_result.TotalSize() - 1);
        l_generator.seed(p_key);
#else
        srand(p_key);
#endif

        for (int i = 0; i < p_dataLen; i++)
        {
            for (int j = 0; j < 8; j++)
            {
#ifdef _WIN32
                int l_randomPos = l_distribution(l_generator);
#else
                int l_randomPos = rand() % l_result.TotalSize();
#endif
                unsigned char l_value = p_data[i];
                unsigned char l_bitValue = (l_value & (1 << j)) >> j;

                unsigned char l_srcBitValue = (l_result.data[l_randomPos] & (1 << SUBSTITUTE_BIT));

                l_result.data[l_randomPos] = l_result.data[l_randomPos] - l_srcBitValue + (l_bitValue << SUBSTITUTE_BIT);
            }
        }

#ifdef _WIN32
        l_generator.seed(p_key);
#else
        srand(p_key);
#endif

        ImageData l_readImage;
        l_readImage.width = 64;
        l_readImage.height = 64;
        l_readImage.channels = 1;
        l_readImage.data = new unsigned char[l_readImage.TotalSize()];

        for (int i = 0; i < l_readImage.TotalSize(); i++)
        {
            unsigned char l_value = 0;
            for (int j = 0; j < 8; j++)
            {
#ifdef _WIN32
                int l_randomPos = l_distribution(l_generator);
#else
                int l_randomPos = rand() % l_result.TotalSize();
#endif
                unsigned char l_srcBitValue = (l_result.data[l_randomPos] & (1 << SUBSTITUTE_BIT)) >> SUBSTITUTE_BIT;
                l_value += (l_srcBitValue) << j;
            }
            l_readImage.data[i] = l_value;
        }

        return {l_result, l_readImage};
    }

    

    // NEURAL NETWORK
    NeuralNetwork::NeuralNetwork(size_t p_layerAmount, size_t p_layerSize)
    {
        m_layers = std::vector<Node>(p_layerAmount * p_layerSize, {0.0f,
                                                                   std::vector<float>(p_layerSize, 0.0f)});

        m_size = NNSize{p_layerAmount, p_layerSize};

        std::cout << "Init neural network... Layer size : " << m_layers.size() << std::endl;
    }

    void NeuralNetwork::setRandomWeights(size_t p_seed = 0)
    {
        std::default_random_engine l_generator;
        std::uniform_real_distribution<float> l_distribution(-1.0f, 1.0f);

        if (p_seed != 0)
            l_generator.seed(p_seed);

        for (auto &l_node : m_layers)
        {
            size_t l_weightSize = l_node.weights.size();
            for (int i = 0; i < l_weightSize; i++)
            {
                l_node.weights[i] = l_distribution(l_generator);
            }
        }
    }

    void NeuralNetwork::setActivation(std::function<float(float)> p_activation)
    {
        m_activation = p_activation;
    }

    std::vector<float> NeuralNetwork::work(std::vector<float> p_firstLayer)
    {
        size_t l_firstLayerSize = p_firstLayer.size();

        std::cout << "Start working..." << std::endl;

        for (int i = 0; i < l_firstLayerSize; i++)
        {
            m_layers[i].value = p_firstLayer[i];
        }

        std::cout << "Intialization done. Start propagation..." << std::endl;

        return propagate(NeuralNetwork::FIRST_LAYER);
    }

    std::vector<float> NeuralNetwork::propagate(size_t p_layerIndex)
    {
        // reach last layer, return value
        if (p_layerIndex >= m_size.layerAmount - 1)
        {
            std::cout << "Last layer hit, initializing return last layer..." << std::endl;

            size_t l_lastOffset = (m_size.layerAmount - 1) * m_size.layerSize;
            std::vector<float> l_lastLayer(m_size.layerSize, 0.0f);
            for (int i = l_lastOffset; i < m_layers.size(); i++)
            {
                l_lastLayer[i - l_lastOffset] = m_layers[i].value;
            }

            std::cout << "Return last layer ready." << std::endl;

            return l_lastLayer;
        }

        size_t l_offset = p_layerIndex * m_size.layerSize;
        size_t l_nextOffset = (p_layerIndex + 1) * m_size.layerSize;

        std::cout << "Preparing propagation... " << l_offset << ", " << l_nextOffset << std::endl;

        for (int i = 0; i < m_size.layerSize; i++)
        {
            m_layers[l_nextOffset + i].value = 0.0f;

            std::cout << "Set value to 0" << std::endl;

            for (int j = 0; j < m_size.layerSize; j++)
            {
                m_layers[l_nextOffset + i].value += m_layers[l_nextOffset + i].weights[j] * m_layers[l_offset + j].value;
            }

            m_layers[l_nextOffset + i].value = m_activation(m_layers[l_nextOffset + i].value);
        }

        std::cout << "Propagation done. Go to next." << std::endl;

        return propagate(p_layerIndex + 1);
    }

    std::vector<Node> NeuralNetwork::at(size_t p_layerIndex)
    {
        if (p_layerIndex >= m_size.layerAmount)
        {
            throw std::exception("Layer index out of bound");
        }

        std::vector<Node> l_result(m_size.layerSize);

        size_t l_offset = p_layerIndex * m_size.layerSize;
        for (int i = 0; i < m_size.layerSize; i++)
        {
            l_result[i] = m_layers[l_offset + i];
        }

        return l_result;
    }

    NNSize NeuralNetwork::size() const noexcept
    {
        return m_size;
    }

    unsigned char ConvolutionalFilter::ApplyFilter(ImageData const &p_source, unsigned int p_x, unsigned int p_y, unsigned int p_c) const {
        unsigned int l_result = 0;

        for(int i = 0; i < height; i++) {
            unsigned int l_yOffset = (p_y + i) * p_source.width;

            for(int j = 0; j < width; j++) {
                l_result += (unsigned int)(data[i * width + j] * (float)p_source.data[(l_yOffset + (p_x + j)) * p_source.channels + p_c]);
            }
        }
        
        return (unsigned char) l_result;
    }

    ImageData Common::ApplyCNFilter(ImageData const &p_source, ConvolutionalFilter const &p_filter) {
        unsigned int l_newWidth = p_source.width - p_filter.width;
        unsigned int l_newHeight = p_source.height - p_filter.height;

        ImageData l_result{};
        l_result.width = l_newWidth; l_result.height = l_newHeight;
        l_result.channels = p_source.channels;
        l_result.data = new unsigned char[l_newWidth * l_newHeight * l_result.channels];

        std::cout << "Channels : " << l_result.channels << std::endl;

        for(int i = 0; i < l_newHeight; i++) {
            for(int j = 0; j < l_newWidth; j++) {
                for(int c = 0; c < l_result.channels; c++) {
                    l_result.data[(i * l_newWidth + j) * l_result.channels + c] = p_filter.ApplyFilter(p_source, j, i, c);
                }
            }
        }

        return l_result;
    }

    ImageData Common::Pooling(ImageData const &p_source, PoolingType p_type) {
        ImageData l_result{};
        l_result.width = p_source.width / 2; l_result.height = p_source.height / 2;
        l_result.channels = p_source.channels;
        l_result.data = new unsigned char[l_result.width * l_result.height];

        // std::cout << "Width, height : " << p_source.width << "x" << p_source.height << ", " << l_result.width << "x" << l_result.height << std::endl;

        for(int i = 0; i < p_source.height - (p_source.height % 2); i += 2) {
            for(int j = 0; j < p_source.width - (p_source.width % 2); j += 2) {
                unsigned int l_poolOffset = (i / 2 * l_result.width + j /2);
                switch (p_type) {
                    case MAX:
                        l_result.data[l_poolOffset] = std::max(
                            std::max(p_source.data[(i * l_result.width + j)], p_source.data[((i+1) * l_result.width + j)]), 
                            std::max(p_source.data[(i * l_result.width + (j+1))], p_source.data[((i+1) * l_result.width + (j+1))]));
                        break;
                    case AVERAGE:
                    break;
                    case SUM:
                    break;
                }
            }
        }

        return l_result;
    }
}
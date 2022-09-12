#define STB_IMAGE_IMPLEMENTATION
#include <stbi_image.h>
#include <stbi_image_write.h>
#include <common.hpp>

namespace Common
{
    ImageData ImageData::FromPath(const char* p_path) {
        int l_width, l_height, l_channels;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);

        ImageData l_imgData;
        l_imgData.data = l_data;
        l_imgData.width = l_width;
        l_imgData.height = l_height;
        l_imgData.channels = l_channels;

        return l_imgData;
    }

    float ComputePSNR(const char *p_fPath, const char *p_sPath)
    {
        return ComputePSNR(ImageData::FromPath(p_fPath), ImageData::FromPath(p_sPath));
    }
    float ComputePSNR(ImageData &p_fData, ImageData &p_sData) {
        if (p_sData.width != p_fData.width || p_sData.height != p_fData.height || p_fData.height != p_sData.channels)
        {
            return 0.0f;
        }

        int l_size = p_fData.width * p_fData.height;

        int l_totalDiff = 0;
        for (int l_index = 0; l_index < l_size * p_fData.height; l_index += p_fData.height)
        {
            int l_dist = 0;

            for (int l_cIndex = 0; l_cIndex < p_fData.height; l_cIndex++)
            {
                int l_diff = (int)p_fData.data[l_index] - (int)p_sData.data[l_index];
                l_dist += l_diff * l_diff;
            }

            l_totalDiff += l_dist;
        }

        return 10 * log10((255.0f * 255.0f) / (float)(l_totalDiff / l_size));
    }

    std::vector<std::array<int, 256>> ComputeHistogram(const char* p_path) {
        return ComputeHistogram(ImageData::FromPath(p_path));
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

    float ComputEntropy(const char* p_path) {
        return ComputeEntropy(ImageData::FromPath(p_path));
    }
    float ComputeEntropy(ImageData &p_data) {
        std::vector<std::array<int, 256>> l_histogram = ComputeHistogram(p_data);

        float l_sum = 0.0f;
        for(int i = 0; i < 256; i++) {
            float l_value = 0.0f;
            for (int l_cIndex = 0; l_cIndex < l_histogram.size(); l_cIndex++) {
                l_value += (l_histogram[l_cIndex][i] * 1.0f) / (p_data.width * p_data.height * 1.0f);
            }
            l_value /= 3.0f;
            if (l_value == 0.0f) continue;
            l_sum -= l_value * (log(l_value) / log(2));
        }

        return l_sum;
    }

    ImageData PermutData(const char *p_path, int p_key)
    {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        std::size_t l_size = l_width * l_height;

        std::cout << l_width << "x" << l_height << " (" << l_size << ")" << std::endl;

        unsigned char *l_newData = new unsigned char[l_size * l_channels];
        bool *l_writenPos = new bool[l_size];
        for (int l_index = 0; l_index < l_size; l_index++) l_writenPos[l_index] = false;

        std::default_random_engine l_generator;
        l_generator.seed(p_key);
        std::uniform_int_distribution<int> l_distribution(0, l_size - 1);
        for (std::size_t l_index = 0; l_index < l_size * l_channels; l_index += l_channels)
        {
            int l_randomPos = l_distribution(l_generator);


            while (l_writenPos[l_randomPos]) {
                l_randomPos = (l_randomPos + 1) % l_size;
            }

            for(int l_cIndex = 0; l_cIndex < l_channels; l_cIndex++) {
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
    ImageData RevertPermut(const char* p_path, int p_key) {
        int l_channels, l_width, l_height;
        unsigned char *l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);

        std::size_t l_size = l_width * l_height;

        unsigned char* l_decryptedData = new unsigned char[l_size * 3];
        bool* l_readData = new bool[l_size];
        for(int l_index = 0; l_index < l_size; l_index++) { l_readData[l_index] = false; }

        std::default_random_engine l_generator;
        l_generator.seed(p_key);
        std::uniform_int_distribution<int> l_distribution(0, l_size - 1);
        for (std::size_t l_index = 0; l_index < l_size * l_channels; l_index += l_channels)
        {
            int l_randomPos = l_distribution(l_generator);

            while (l_readData[l_randomPos])
                l_randomPos = (l_randomPos + 1) % l_size;

            for(int l_cIndex = 0; l_cIndex < l_channels; l_cIndex++)
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

    ImageData EncryptData(const char* p_path, int p_key){
        int l_channels, l_width, l_height;
        unsigned char* l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        std::size_t l_size = l_width * l_height;
        unsigned char* l_newData = new unsigned char[l_size * l_channels];

        std::default_random_engine l_generator;
        l_generator.seed(p_key);
        std::uniform_int_distribution<int> l_distribution(0, l_size - 1);
        for(std::size_t l_index = 0; l_index < l_size * l_channels; l_index++) {
            char l_value = 0;

            int l_rValue = l_distribution(l_generator);

            if (l_index == 0) {
                l_value = (l_rValue + l_data[l_index]) % 256;
            } else {
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
    ImageData DecryptData(const char* p_path, int p_key){
        int l_channels, l_width, l_height;
        unsigned char* l_data = stbi_load(p_path, &l_width, &l_height, &l_channels, 0);
        std::size_t l_size = l_width * l_height;
        unsigned char* l_newData = new unsigned char[l_size * l_channels];

        std::default_random_engine l_generator;
        l_generator.seed(p_key);
        std::uniform_int_distribution<int> l_distribution(0, l_size - 1);
        for(std::size_t l_index = 0; l_index < l_size * l_channels ; l_index++) {
            char l_value = 0;
            int l_rValue = l_distribution(l_generator);

            if (l_index == 0) {
                l_value = (l_data[l_index] - l_rValue) % 256;
            } else {
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
}
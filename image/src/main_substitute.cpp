#include <common.hpp>
#include <filesystem>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Wrong usage : substitute <hiddenPath> <sourcePath>";
        exit(-1);
    }

    std::filesystem::path l_hiddenPath{std::string(argv[1])};
    std::filesystem::path l_Path{std::string(argv[2])};

    Common::ImageData l_hidden = Common::ImageData::FromPath(l_hiddenPath.string().c_str());
    Common::ImageData l_source = Common::ImageData::FromPath(l_Path.string().c_str());

    int l_bitPlace = argc == 4 ? std::clamp(std::stoi(argv[3]), 0, 7) : 0;
    Common::ImagePair l_result = Common::HideData(l_source, l_hidden.data, l_hidden.TotalSize(), 123, l_bitPlace);

    Common::ImageData::SaveImage(l_result.first, "hidden.png");
    Common::ImageData::SaveImage(l_result.second, "read.png");
}
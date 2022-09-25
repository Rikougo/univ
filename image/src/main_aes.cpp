#include <common.hpp>
#include <format>
#include <filesystem>

int main(int argc, char* argv[]) {
    std::filesystem::path l_path{std::string(argv[1])};

    auto l_ECBresult = Common::EncryptECB(l_path.string().c_str(), 123);
    auto l_CBCresult = Common::EncryptCBC(l_path.string().c_str(), 123);
    auto l_CFBresult = Common::EncryptCFB(l_path.string().c_str(), 123);
    auto l_OFBresult = Common::EncryptOFB(l_path.string().c_str(), 123);

    std::cout << "Encryption done" << std::endl;

    Common::ImageData::SaveImage(l_ECBresult.first.first, std::format("AES/{}_ECBencrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_ECBresult.first.second, std::format("AES/{}_ECBdecrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_ECBresult.second.first, std::format("AES/{}_ECBNoiseencrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_ECBresult.second.second, std::format("AES/{}_ECBNoisedecrypted.png", l_path.stem().string()));

    Common::ImageData::SaveImage(l_CBCresult.first.first, std::format("AES/{}_CBCencrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_CBCresult.first.second, std::format("AES/{}_CBCdecrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_CBCresult.second.first, std::format("AES/{}_CBCNoiseencrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_CBCresult.second.second, std::format("AES/{}_CBCNoisedecrypted.png", l_path.stem().string()));

    Common::ImageData::SaveImage(l_CFBresult.first.first, std::format("AES/{}_CFBencrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_CFBresult.first.second, std::format("AES/{}_CFBdecrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_CFBresult.second.first, std::format("AES/{}_CFBNoiseencrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_CFBresult.second.second, std::format("AES/{}_CFBNoisedecrypted.png", l_path.stem().string()));

    Common::ImageData::SaveImage(l_OFBresult.first.first, std::format("AES/{}_OFBencrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_OFBresult.first.second, std::format("AES/{}_OFBdecrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_OFBresult.second.first, std::format("AES/{}_OFBNoiseencrypted.png", l_path.stem().string()));
    Common::ImageData::SaveImage(l_OFBresult.second.second, std::format("AES/{}_OFBNoisedecrypted.png", l_path.stem().string()));
}
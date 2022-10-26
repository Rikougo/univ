#include <common.hpp>

int main(int argc, char* argv[]) {
    Common::ConvolutionalFilter l_gaussianFilter{};
    l_gaussianFilter.width = 3; l_gaussianFilter.height = 3;
    l_gaussianFilter.data = new float[9] {
        1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f,
        2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f,
        1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f
    };

    Common::ConvolutionalFilter l_blurFilter{};
    l_blurFilter.width = 3; l_blurFilter.height = 3;
    l_blurFilter.data = new float[9] {
        1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f,
        1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f,
        1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f
    };

    Common::ConvolutionalFilter l_laplacianFilter{};
    l_laplacianFilter.width = 3; l_laplacianFilter.height = 3;
    l_laplacianFilter.data = new float[9] {
        0,  1, 0,
        1, -4, 1,
        0,  1, 0
    };

    Common::ConvolutionalFilter l_sobelFilter{};
    l_sobelFilter.width = 3; l_sobelFilter.height = 3;
    l_sobelFilter.data = new float[9] {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };

    Common::ImageData l_image = Common::ImageData::FromPath(argv[1]);

    Common::ImageData l_gaussianResult = Common::ApplyCNFilter(l_image, l_gaussianFilter);
    Common::ImageData l_blurResult = Common::ApplyCNFilter(l_image, l_blurFilter);
    Common::ImageData l_laplacianResult = Common::ApplyCNFilter(l_image, l_laplacianFilter);
    Common::ImageData l_sobelResult = Common::ApplyCNFilter(l_image, l_sobelFilter);
    Common::ImageData::SaveImage(l_gaussianResult, "cn_filter_gaussian.png");
    Common::ImageData::SaveImage(l_blurResult, "cn_filter_blur.png");
    Common::ImageData::SaveImage(l_laplacianResult, "cn_filter_laplacian.png");
    Common::ImageData::SaveImage(l_sobelResult, "cn_filter_sobel.png");
}
#ifndef IMAGE_SPATIAL_TRANSFORMATION_H
#define IMAGE_SPATIAL_TRANSFORMATION_H

#include "ImageIO.h"
#include <vector>
#include <string>

namespace iipt {

    class SpatialTransformation {
        // This class provides spatial transformation methods for image processing.
        // It includes low-pass and high-pass filters, sharpening, unsharp masking, and highboost filtering.

        // Note: The methods are static and can be called without creating an instance of this class.
        // They operate directly on the Image object passed as a parameter.
        public:

            enum class PaddingType {
                None,
                Zero,
                Replicate,
                Mirror
            };

            static PaddingType askPaddingType();  // Helper function to interactively ask user for padding type

            // Public API (used in GUI or application logic)
            static void applyBoxFilter(Image& img, int kernelSize, PaddingType padding = PaddingType::None);
            static void applyGaussianFilter(Image& img, int kernelSize, float sigma, PaddingType padding = PaddingType::None);
            static void applyMedianFilter(Image& img, int kernelSize, PaddingType padding = PaddingType::None);

            static void applyLaplacianBasic(Image& img, bool inverted = false, PaddingType padding = PaddingType::None);
            static void applyLaplacianFull(Image& img, bool inverted = false, PaddingType padding = PaddingType::None);
            static void applySobel(Image& img, PaddingType padding = PaddingType::None);

            static void applySharpening(Image& img, const std::string& method, PaddingType padding = PaddingType::None);
            static void applyUnsharpMasking(Image& img, const std::string& kernelType, int kernelSize, float sigma = 1.0f, PaddingType padding = PaddingType::None);
            static void applyHighboostFiltering(Image& img, const std::string& kernelType, int kernelSize, float K, float sigma = 1.0f, PaddingType padding = PaddingType::None);

        private:
            // Internal cores (used in implementation and testing)
            static std::vector<unsigned char> boxFilterCore(const std::vector<unsigned char>& input, int width, int height, int channels, int kernelSize, PaddingType padding);
            static std::vector<unsigned char> gaussianFilterCore(const std::vector<unsigned char>& input, int width, int height, int channels, int kernelSize, PaddingType padding, float sigma);
            static std::vector<unsigned char> medianFilterCore(const std::vector<unsigned char>& input, int width, int height, int channels, int kernelSize, PaddingType padding);

            static std::vector<unsigned char> laplacianBasicCore(const std::vector<unsigned char>& input, int width, int height, int channels, bool inverted, PaddingType padding);
            static std::vector<unsigned char> laplacianFullCore(const std::vector<unsigned char>& input, int width, int height, int channels, bool inverted, PaddingType padding);
            static std::vector<unsigned char> sobelCore(const std::vector<unsigned char>& input, int width, int height, int channels, PaddingType padding);

            static std::vector<unsigned char> sharpeningCore(const std::vector<unsigned char>& input, int width, int height, int channels, const std::string& method, PaddingType padding);
            static std::vector<unsigned char> unsharpMaskingCore(const std::vector<unsigned char>& input, int width, int height, int channels, const std::string& kernelType, int kernelSize, float sigma, PaddingType padding);
            static std::vector<unsigned char> highboostFilteringCore(const std::vector<unsigned char>& input, int width, int height, int channels, const std::string& kernelType, int kernelSize, float K, float sigma, PaddingType padding);

            static std::vector<std::vector<float>> generateGaussianKernel(int size, float sigma);
            static std::vector<unsigned char> convolve(const std::vector<unsigned char>& input,
                                                        int width, int height, int channels,
                                                        const std::vector<std::vector<float>>& kernel,
                                                        PaddingType padding);


    };

} // namespace iipt

#endif // IMAGE_SPATIAL_TRANSFORMATION_H

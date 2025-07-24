#include "ImageIO.h"
#include "ImageIntensityTransformation.h"
#include "ImageSpatialTransformation.h"
#include "ImageConverter.h"
#include "ImageMorphology.h"
#include "ImageUtils.h"

#include <iostream>
#include <string>

using namespace iipt;

int main() {
    std::string inputFile = "../../testImages/test1.bmp";
    std::string outputFile = "../../resultImages/output.bmp";

    Image img;

    // Load the BMP file
    if (!img.loadBMP(inputFile)) {
        std::cerr << "Failed to load BMP file.\n";
        return 1;
    }

    std::cout << "Image loaded successfully!\n";
    std::cout << "Width: " << img.width << ", Height: " << img.height << ", Channels: " << img.channels << "\n";

    // Image processing options -----------------------------------------------------------------------------------


    std::cout << "From the following options, what do you want to do?\n"
              << "1. Intensity Transformation\n"
              << "2. Histogram\n"
              << "3. Spatial Transformation\n"
              << "4. Image Conversion\n"
              << "5. Image Morphology\n"
              << "6. Edge Detection\n"
              << "Type the number: ";

    int choice1;
    std::cin >> choice1;

    if (std::cin.fail()) {
        std::cerr << "Invalid input. Exiting.\n";
        return EXIT_FAILURE;
    }

    switch (choice1) {
    case 1: {
        // Intensity Transformation
        std::cout << "What type of transformation do you want to perform?\n"
                << "1. Negative Transformation\n"
                << "2. Log Transformation\n"
                << "3. Gamma Transformation\n"
                << "Type the number: ";

        int choice2;
        std::cin >> choice2;
        if (std::cin.fail()) {
                std::cerr << "Invalid input. Exiting." << std::endl;
                return EXIT_FAILURE;
        }

        switch (choice2) {
        case 1:
            ImageIntensityTransformation::applyNegative(img);
            break;
        case 2: {
            float c;
            std::cout << "Enter scale value c for log transform: ";
            std::cin >> c;
            if (std::cin.fail() || c <= 0) {
                std::cerr << "Invalid scale value.\n";
                return EXIT_FAILURE;
            }
            ImageIntensityTransformation::applyLog(img, c);
            break;
        }
        case 3: {
            float c, gamma;
            std::cout << "Enter scale value c for gamma transform: ";
            std::cin >> c;
            std::cout << "Enter gamma value: ";
            std::cin >> gamma;
            if (std::cin.fail() || c <= 0 || gamma <= 0) {
                std::cerr << "Invalid gamma or scale value.\n";
                return EXIT_FAILURE;
            }
            ImageIntensityTransformation::applyGamma(img, gamma, c);
            break;
        }
        default:
            std::cerr << "Invalid transformation choice.\n";
            return EXIT_FAILURE;
        }
        break;
    }
    case 2: {
        // Histogram
        break;
    }
    case 3: {
        // Spatial Transformation

        SpatialTransformation::PaddingType padding = SpatialTransformation::askPaddingType();

        std::cout << "Choose spatial operation:\n"
                << "1. Low Pass Filter\n"
                << "2. High Pass Filter\n"
                << "3. Sharpening\n"
                << "4. Unsharp Masking\n"
                << "5. Highboost Filtering\n"
                << "Type the number: ";

        int subChoice;
        std::cin >> subChoice;

        std::vector<unsigned char> outputData;

        switch (subChoice) {
        case 1: {
            // Low Pass Filter
            std::cout << "Choose filter type:\n"
                    << "1. Box Filter\n"
                    << "2. Gaussian Filter\n"
                    << "3. Median Filter\n"
                    << "Type the number: ";
            int type;
            std::cin >> type;

            int kernelSize;
            std::cout << "Enter kernel size (odd number like 3, 5, 7): ";
            std::cin >> kernelSize;

            if (type == 1)
                SpatialTransformation::applyBoxFilter(img, kernelSize, padding);
            else if (type == 2) {
                float sigma;
                std::cout << "Enter sigma for Gaussian: ";
                std::cin >> sigma;
                SpatialTransformation::applyGaussianFilter(img, kernelSize, sigma, padding);
            } else if (type == 3)
                SpatialTransformation::applyMedianFilter(img, kernelSize, padding);
            else {
                std::cerr << "Invalid filter type.\n";
                return EXIT_FAILURE;
            }
            break;
        }

        case 2: {
            // High Pass Filter
            std::cout << "Choose Laplacian filter:\n"
                    << "1. Basic Laplacian\n"
                    << "2. Full Laplacian\n"
                    << "3. Basic Inverted Laplacian\n"
                    << "4. Full Inverted Laplacian\n"
                    << "Type the number: ";
            int type;
            std::cin >> type;

            bool inverted = (type >= 3);
            if (type == 1 || type == 3)
                SpatialTransformation::applyLaplacianBasic(img, inverted, padding);
            else if (type == 2 || type == 4)
                SpatialTransformation::applyLaplacianFull(img, inverted, padding);
            else {
                std::cerr << "Invalid Laplacian type.\n";
                return EXIT_FAILURE;
            }
            break;
        }

        case 3: {
            // Sharpening
            std::cout << "Choose a high pass filter for sharpening:\n"
                    << "1. Basic Laplacian\n"
                    << "2. Full Laplacian\n"
                    << "3. Basic Inverted Laplacian\n"
                    << "4. Full Inverted Laplacian\n"
                    << "5. Sobel\n"
                    << "Type the number: ";
            int type;

            std::cin >> type;

            const char* methods[] = {"Basic Laplacian", "Full Laplacian", "Basic Inverted Laplacian", "Full Inverted Laplacian", "Sobel"};
            if (type < 1 || type > 5) {
                std::cerr << "Invalid sharpening method.\n";
                return EXIT_FAILURE;
            }
            SpatialTransformation::applySharpening(img, methods[type - 1], padding);
            break;
        }

        case 4: {
            // Unsharp Masking
            std::cout << "Choose blur kernel:\n"
                    << "1. Box\n"
                    << "2. Gaussian\n"
                    << "3. Median\n"
                    << "Type the number: ";
            int type;
            std::cin >> type;

            int k;
            float sigma = 1.0f;
            std::cout << "Enter kernel size: ";
            std::cin >> k;
            if (type == 2) {
                std::cout << "Enter sigma: ";
                std::cin >> sigma;
            }

            const char* kernels[] = {"box", "gaussian", "median"};
            if (type < 1 || type > 3) {
                std::cerr << "Invalid blur type.\n";
                return EXIT_FAILURE;
            }

            SpatialTransformation::applyUnsharpMasking(img, kernels[type - 1], k, sigma, padding);
            break;
        }

        case 5: {
            // Highboost Filtering
            std::cout << "Choose blur kernel:\n"
                    << "1. Box\n"
                    << "2. Gaussian\n"
                    << "3. Median\n"
                    << "Type the number: ";
            int type;
            std::cin >> type;

            int k;
            float K, sigma = 1.0f;
            std::cout << "Enter kernel size: ";
            std::cin >> k;
            std::cout << "Enter gain K: ";
            std::cin >> K;
            if (type == 2) {
                std::cout << "Enter sigma: ";
                std::cin >> sigma;
            }

            const char* kernels[] = {"box", "gaussian", "median"};
            if (type < 1 || type > 3) {
                std::cerr << "Invalid blur type.\n";
                return EXIT_FAILURE;
            }

            SpatialTransformation::applyHighboostFiltering(img, kernels[type - 1], k, K, sigma, padding);
            break;
        }

        default:
            std::cerr << "Invalid choice for spatial transformation.\n";
            return EXIT_FAILURE;
        }

        break;
    }
    case 4: {
        // Image Conversion

        std::cout << "What type of conversion do you want to perform?\n"
                << "1. RGB to Grayscale\n"
                << "2. Grayscale to Binary\n"
                << "Type the number: ";

        int choice41;
        std::cin >> choice41;

        switch (choice41) {
        case 1:
            // RGB to Grayscale
            RGBToGrayscaleConverter::convert(img);
            break;
        case 2:
            // Grayscale to Binary

            std::cout << "Choose thresholding method:\n"
                    << "1. Fixed Threshold\n"
                    << "2. Otsu's Method\n"
                    << "3. Adaptive Mean Threshold\n"
                    << "4. Adaptive Gaussian Threshold\n"
                    << "Type the number: ";
            int method;
            std::cin >> method;

            switch (method) {
            case 1: {
                int threshold;
                std::cout << "Enter threshold (0-255): ";
                std::cin >> threshold;
                GrayscaleToBinaryConverter::fixedThreshold(img, threshold);
                break;
            }
            case 2:
                GrayscaleToBinaryConverter::otsuThreshold(img);
                break;
            case 3: {
                int blockSize, C;
                std::cout << "Enter block size (odd number): ";
                std::cin >> blockSize;
                std::cout << "Enter constant C: ";
                std::cin >> C;
                GrayscaleToBinaryConverter::adaptiveMeanThreshold(img, blockSize, C);
                break;
            }
            case 4: {
                int blockSize, C;
                std::cout << "Enter block size (odd number): ";
                std::cin >> blockSize;
                std::cout << "Enter constant C: ";
                std::cin >> C;
                GrayscaleToBinaryConverter::adaptiveGaussianThreshold(img, blockSize, C);
                break;
            }
            default:
                std::cerr << "Invalid thresholding method.\n";
                return EXIT_FAILURE;
            }
            break;
        default:
            std::cerr << "Invalid choice for image conversion.\n";
            return EXIT_FAILURE;
        }


        break;
    }
    case 5: {
        // Image Morphology
        std::cout << "Choose morphological operation:\n"
                << "1. Erosion\n"
                << "2. Dilation\n"
                << "3. Opening\n"
                << "4. Closing\n"
                << "5. Boundary Extraction\n"
                << "Type the number: ";

        int morphChoice;
        std::cin >> morphChoice;

        // Create structuring element
        std::string shape;
        int size;
        std::cout << "Enter structuring element shape (square, cross, circle, line_horizontal, line_vertical): ";
        std::cin >> shape;
        std::cout << "Enter structuring element size (odd number > 1): ";
        std::cin >> size;

        // Create the structuring element
        std::vector<std::vector<int>> se = ImageUtils::createStructuringElement(shape, size);

        ImageUtils::PaddingType padding = ImageUtils::askPaddingType();


        switch (morphChoice) {
        case 1:
            ImageMorphology::erosion(img, se, padding);
            break;
        case 2:
            ImageMorphology::dilation(img, se, padding);
            break;
        case 3:
            ImageMorphology::opening(img, se, padding);
            break;
        case 4:
            ImageMorphology::closing(img, se, padding);
            break;
        case 5:
            ImageMorphology::boundaryExtract(img, se, padding);
            break;
        default:
            std::cerr << "Invalid choice for morphological operation.\n";
            return EXIT_FAILURE;
        }

        break;
    }
    case 6: {
        // Edge Detection
        break;
    }   
    default:
        break;
    }

    // Save it back under a new name
    if (!img.saveBMP(outputFile)) {
        std::cerr << "Failed to save BMP file.\n";
        return 1;
    }

    std::cout << "Image saved to: " << outputFile << "\n";

    return 0;
}

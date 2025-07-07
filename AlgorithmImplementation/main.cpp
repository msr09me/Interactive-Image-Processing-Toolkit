#include "ImageIO.h"
#include "ImageIntensityTransformation.h"
#include <iostream>
#include <string>

using namespace iipt;

int main() {
    std::string inputFile = "../../testImages/lena512.bmp";
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
              << "3. Spatial Filtering\n"
              << "4. Imager Conversion\n"
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
        // Spatial Filtering
        break;
    }
    case 4: {
        // Image Conversion
        break;
    }
    case 5: {
        // Image Morphology
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

#pragma once

#include <vector>
#include <string>

namespace iipt {

    class ImageUtils {
        public:

            enum class PaddingType {
                None,
                Zero,
                Replicate,
                Mirror
            };

            static PaddingType askPaddingType();  // Helper function to interactively ask user for padding type

            static std::vector<unsigned char> padImage(
                const std::vector<unsigned char>& data,
                int width, int height,
                int channels,
                int padX, int padY,
                PaddingType type
            );
            // Generate structuring element with a given shape and size
            static std::vector<std::vector<int>> createStructuringElement(const std::string& shape, int size);
    };

}

#pragma once

#include "ImageIO.h"
#include "ImageUtils.h"

namespace iipt {

    class ImageMorphology {
        
        public:
            static void erosion(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding);
            static void dilation(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding);
            static void opening(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding);
            static void closing(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding);
            static void boundaryExtract(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding);
    };

}

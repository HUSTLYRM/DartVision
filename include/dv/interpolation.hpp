#pragma once

#include <type_traits>

#include "dv/image.hpp"


namespace dv
{
    namespace interpolation
    {
        using namespace image;

        template <typename SrcImage, typename DstImage>
        inline void nearest_neighbor(SrcImage &src, DstImage &dst)
        {
            static_assert(is_image<SrcImage>::value, "SrcImage must be an Image");
            static_assert(is_image<DstImage>::value, "DstImage must be an Image");
            static_assert(std::is_same<typename SrcImage::PixelT,
                                       typename DstImage::PixelT>::value,
                          "SrcImage and DstImage must have the same Pixel Format");

            const size_t src_width = src.width();
            const size_t src_height = src.height();
            const size_t dst_width = dst.width();
            const size_t dst_height = dst.height();

            if (src_width == 0 || src_height == 0 ||
                dst_width == 0 || dst_height == 0)
            {
                // dst = Image<typename DstPixelT, dst_width, dst_height>();
                return;
            }

            for (size_t y = 0; y < dst_height; ++y)
            {
                for (size_t x = 0; x < dst_width; ++x)
                {
                    size_t src_x = x * src_width / dst_width;
                    size_t src_y = y * src_height / dst_height;
                    dst(x, y) = src(src_x, src_y);
                }
            }
        }

    }
}
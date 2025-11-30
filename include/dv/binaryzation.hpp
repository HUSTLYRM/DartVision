#pragma once

#include "dv/image.hpp"

namespace dv
{
    namespace binaryzation
    {
        using namespace image;
        using namespace pixel_format;

        template <PixelFormat PF, size_t WIDTH, size_t HEIGHT>
        void threshold(const Image<PF, WIDTH, HEIGHT> &src,
                       Image<PixelFormat::Binary, WIDTH, HEIGHT> &dst,
                       typename PixelFormatTrait<PF>::type t_low,
                       typename PixelFormatTrait<PF>::type t_high)
        {
            for (size_t y = 0; y < HEIGHT; ++y)
            {
                for (size_t x = 0; x < WIDTH; ++x)
                {
                    auto pixel = src(x, y);
                    dst(x, y) = (pixel >= t_low && pixel <= t_high) * 255;
                }
            }
        }

        template <PixelFormat PF, size_t WIDTH, size_t HEIGHT>
        void threshold(const Image<PF, WIDTH, HEIGHT> &src,
                       Image<PixelFormat::Binary, WIDTH, HEIGHT> &dst,
                       typename PixelFormatTrait<PF>::type t)
        {
            auto t_max = PixelFormatTrait<PF>::max();
            threshold(src, dst, t, t_max);
        }

    }
}
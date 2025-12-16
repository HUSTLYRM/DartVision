#pragma once

#include "dv/image.hpp"

namespace dv
{
    namespace binaryzation
    {
        using namespace image;
        using namespace pixel_format;

        template <PixelFormat PF, typename TPFT, size_t WIDTH, size_t HEIGHT>
        void threshold(const Image<PF, WIDTH, HEIGHT> &src,
                       Image<PixelFormat::Binary, WIDTH, HEIGHT> &dst,
                        TPFT t_low,
                        TPFT t_high)
        {
            for (size_t y = 0; y < HEIGHT; ++y)
            {
                for (size_t x = 0; x < WIDTH; ++x)
                {
                    TPFT pixel;
                    pixel_cast(src(x, y), pixel);
                    if (pixel <= t_low)
                        dst(x, y) = BinaryPixel{0};
                    else if (pixel >= t_high)
                        dst(x, y) = BinaryPixel{0};
                    else
                        dst(x, y) = BinaryPixel{255};
                }
            }
        }

        template <PixelFormat PF, typename TPFT, size_t WIDTH, size_t HEIGHT>
        void threshold(const Image<PF, WIDTH, HEIGHT> &src,
                       Image<PixelFormat::Binary, WIDTH, HEIGHT> &dst,
                       TPFT t)
        {
            auto t_max = t.max();
            threshold(src, dst, t, t_max);
        }

    }
}
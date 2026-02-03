#pragma once

#include "dv/image.hpp"

namespace dv
{
    namespace binaryzation
    {
        using namespace image;
        using namespace pixel_format;

        template <PixelFormat PF, typename TPFT, size_t WIDTH, size_t HEIGHT>
        inline void threshold(const Image<PF, WIDTH, HEIGHT> &src,
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
                    if (pixel >= t_low && pixel <= t_high)
                        dst(x, y) = BinaryPixel{255};
                    else
                        dst(x, y) = BinaryPixel{0};
                }
            }
        }

        template <PixelFormat PF, typename TPFT, size_t WIDTH, size_t HEIGHT>
        inline void threshold(const Image<PF, WIDTH, HEIGHT> &src,
                       Image<PixelFormat::Binary, WIDTH, HEIGHT> &dst,
                       TPFT t)
        {
            auto t_max = t.max();
            threshold(src, dst, t, t_max);
        }

        template <size_t WIDTH, size_t HEIGHT>
        inline void otsu(const Image<PixelFormat::Grayscale, WIDTH, HEIGHT> &src,
                  Image<PixelFormat::Binary, WIDTH, HEIGHT> &dst)
        {
            // histogram
            constexpr size_t HIST_SIZE = 256;
            size_t hist[HIST_SIZE] = {0};
            for (size_t y = 0; y < HEIGHT; ++y)
            {
                for (size_t x = 0; x < WIDTH; ++x)
                {
                    auto pixel = src(x, y);
                    size_t idx = static_cast<size_t>(pixel);
                    if (idx >= HIST_SIZE)
                    {
                        idx = HIST_SIZE - 1;
                    }
                    hist[idx]++;
                }
            }

            // total number of pixels
            size_t total = WIDTH * HEIGHT;

            float sum = 0;
            for (size_t t = 0; t < HIST_SIZE; ++t)
            {
                sum += t * hist[t];
            }

            float sumB = 0;
            size_t wB = 0;
            size_t wF = 0;

            float varMax = 0;
            size_t threshold = 0;

            for (size_t t = 0; t < HIST_SIZE; ++t)
            {
                wB += hist[t];
                if (wB == 0)
                    continue;
                wF = total - wB;
                if (wF == 0)
                    break;

                sumB += t * hist[t];

                float mB = sumB / wB;
                float mF = (sum - sumB) / wF;

                float varBetween = static_cast<float>(wB) * static_cast<float>(wF) * (mB - mF) * (mB - mF);

                if (varBetween > varMax)
                {
                    varMax = varBetween;
                    threshold = t;
                }
            }

            binaryzation::threshold(src, dst, GrayscalePixel{static_cast<uint8_t>(threshold)});
        }

        
    }
}

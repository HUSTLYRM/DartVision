#pragma once

#include <cstdint>
#include <cstddef>

#include "dv/pixel_format.hpp"

namespace dv
{
    namespace image
    {
        using namespace pixel_format;

        template <PixelFormat PF,
                  size_t WIDTH,
                  size_t HEIGHT>
        class Image
        {
        public:
            static constexpr PixelFormat pixel_format = PF;

            Image() = default;
            ~Image() = default;

            using PixelT = typename PixelFormatTrait<PF>::type;

            PixelFormat format() const { return format_; }
            size_t width() const { return WIDTH; }
            size_t height() const { return HEIGHT; }

            PixelT &operator()(size_t x, size_t y)
            {
                if (x >= WIDTH || y >= HEIGHT)
                {
                    return data_[0];
                }
                return data_[y * WIDTH + x];
            }

            const PixelT &operator()(size_t x, size_t y) const
            {
                if (x >= WIDTH || y >= HEIGHT)
                {
                    return data_[0];
                }
                return data_[y * WIDTH + x];
            }

        private:
            PixelFormat format_ = PF;
            PixelT data_[WIDTH * HEIGHT];
        };

        template <typename T>
        struct is_image : std::false_type
        {
        };

        template <PixelFormat PF, size_t W, size_t H>
        struct is_image<image::Image<PF, W, H>> : std::true_type
        {
        };

        template <size_t WIDTH, size_t HEIGHT>
        void raw_to_rgb565(uint8_t *src, Image<PixelFormat::RGB565, WIDTH, HEIGHT> &dst)
        {
            for (size_t i = 0; i < WIDTH * HEIGHT * 2; i += 2)
            {
                uint8_t byte[2] = {src[i + 1], src[i]};
                RGB565Pixel pixel;
                pixel = *reinterpret_cast<RGB565Pixel *>(byte);
                dst(i / 2 % WIDTH, i / 2 / WIDTH) = pixel;
            }
        }

        template <size_t WIDTH, size_t HEIGHT>
        void rgb565_to_lab(const Image<PixelFormat::RGB565, WIDTH, HEIGHT> &src,
                             Image<PixelFormat::LAB, WIDTH, HEIGHT> &dst)
        {
            for (size_t y = 0; y < HEIGHT; ++y)
            {
                for (size_t x = 0; x < WIDTH; ++x)
                {
                    const auto &rgb565_pixel = src(x, y);
                    LABPixel lab_pixel;
                    rgb_to_lab(rgb565_pixel, lab_pixel);
                    dst(x, y) = lab_pixel;
                }
            }
        }

        template <size_t WIDTH, size_t HEIGHT>
        void binary_to_rgb565(const Image<PixelFormat::Binary, WIDTH, HEIGHT> &src,
                             Image<PixelFormat::RGB565, WIDTH, HEIGHT> &dst)
        {
            for (size_t y = 0; y < HEIGHT; ++y)
            {
                for (size_t x = 0; x < WIDTH; ++x)
                {
                    const auto &binary_pixel = src(x, y);
                    RGB565Pixel rgb565_pixel = binary_pixel ? PixelFormatTrait<PixelFormat::RGB565>::max() : PixelFormatTrait<PixelFormat::RGB565>::type{0, 0, 0};
                    dst(x, y) = rgb565_pixel;
                }
            }
        }
    }

}
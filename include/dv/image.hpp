#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>

#include "dv/pixel_format.hpp"

namespace dv
{
    namespace image
    {
        using namespace pixel_format;

        template <PixelFormat PF,
                  size_t WIDTH,
                  size_t HEIGHT,
                  typename Derived>
        class ImageBase
        {
        public:
            static constexpr PixelFormat pixel_format = PF;
            using PixelT = typename PixelFormatTrait<PF>::type;

            PixelFormat format() const { return format_; }
            size_t width() const { return WIDTH; }
            size_t height() const { return HEIGHT; }

            decltype(auto) operator()(size_t x, size_t y)
            {
                return static_cast<Derived*>(this)->get(x, y);
            }

            decltype(auto) operator()(size_t x, size_t y) const
            {
                return static_cast<const Derived*>(this)->get(x, y);
            }

            void* get_data_ptr(){
                return static_cast<Derived*>(this)->get_data_ptr();
            }
            
            const void* get_data_ptr() const {
                return static_cast<const Derived*>(this)->get_data_ptr();
            }
            
            const size_t get_data_size() const {
                return static_cast<Derived*>(this)->get_data_size();
            }
            
        protected:
            PixelFormat format_ = PF;
            
        };

        template <PixelFormat PF,
                  size_t WIDTH,
                  size_t HEIGHT>
        class Image : public ImageBase<PF, WIDTH, HEIGHT, Image<PF, WIDTH, HEIGHT>>
        {
        public:
            static constexpr PixelFormat pixel_format = PF;

            Image() = default;
            ~Image() = default;

            using PixelT = typename PixelFormatTrait<PF>::type;

            PixelT &get(size_t x, size_t y)
            {
                return data_[y * WIDTH + x];
            }

            const PixelT &get(size_t x, size_t y) const
            {
                return data_[y * WIDTH + x];
            }

            void* get_data_ptr() {
                return static_cast<void*>(data_);
            }

            const void* get_data_ptr() const {
                return static_cast<const void*>(data_);
            }

            const size_t get_data_size() const {
                return sizeof(data_);
            }

        private:
            alignas(32)
            PixelT data_[WIDTH * HEIGHT];
        };


        template <size_t WIDTH, size_t HEIGHT>
        class Image<PixelFormat::Binary, WIDTH, HEIGHT> : public ImageBase<PixelFormat::Binary, WIDTH, HEIGHT, Image<PixelFormat::Binary, WIDTH, HEIGHT>>
        {
        public:
            using PixelT = typename PixelFormatTrait<PixelFormat::Binary>::type;

            class Proxy {
            public:
                Proxy(std::uint8_t* byte_ptr, uint8_t mask, bool oob = false)
                    : byte_ptr_(byte_ptr), mask_(mask), oob_(oob) {}

                operator uint8_t() const {
                    if (oob_ || byte_ptr_ == nullptr) return 0;
                    return (*byte_ptr_ & mask_) ? 255 : 0;
                }

                operator PixelT() const {
                    if (oob_ || byte_ptr_ == nullptr) return PixelT{0};
                    return (*byte_ptr_ & mask_) ? PixelT{255} : PixelT{0};
                }

                Proxy &operator=(PixelT v) {
                    if (oob_ || byte_ptr_ == nullptr) return *this;
                    if (v == PixelT{0}) {
                        *byte_ptr_ &= ~mask_;
                    } else {
                        *byte_ptr_ |= mask_;
                    }
                    return *this;
                }

                Proxy &operator=(const Proxy &other) {
                    return *this = static_cast<PixelT>(other);
                }

            private:
                std::uint8_t* byte_ptr_;
                uint8_t mask_;
                bool oob_;
            };

            Proxy get(size_t x, size_t y) {
                if (x >= WIDTH || y >= HEIGHT) {
                    return Proxy(nullptr, 0, true);
                }
                size_t idx = y * WIDTH + x;
                size_t byte_index = idx / 8;
                uint8_t bit_mask = uint8_t(1u << (idx % 8));
                return Proxy(&data_[byte_index], bit_mask, false);
            }

            PixelT get(size_t x, size_t y) const {
                if (x >= WIDTH || y >= HEIGHT) return PixelT{0};
                size_t idx = y * WIDTH + x;
                size_t byte_index = idx / 8;
                uint8_t bit_mask = uint8_t(1u << (idx % 8));
                return (data_[byte_index] & bit_mask) ? PixelT{255} : PixelT{0};
            }

            void* get_data_ptr() {
                return static_cast<void*>(data_);
            }

            const void* get_data_ptr() const {
                return static_cast<const void*>(data_);
            }

            const size_t get_data_size() const {
                return sizeof(data_);
            }

        private:
            static constexpr size_t BIT_COUNT = WIDTH * HEIGHT;
            static constexpr size_t BYTE_COUNT = (BIT_COUNT + 7) / 8;
            uint8_t data_[BYTE_COUNT]{};
        };


        template <typename T>
        struct is_image : std::false_type
        {
        };

        template <PixelFormat PF, size_t W, size_t H>
        struct is_image<image::Image<PF, W, H>> : std::true_type
        {
        };


        template <typename from, typename to>
        inline void image_cast(const from &src, to &dst)
        {
            for (size_t y = 0; y < src.height(); ++y)
            {
                for (size_t x = 0; x < src.width(); ++x)
                {
                    typename to::PixelT pixel;
                    pixel_cast(src(x, y), pixel);
                    dst(x, y) = pixel;
                }
            }
        }

        template <size_t WIDTH, size_t HEIGHT>
        inline void raw_to_rgb565(uint8_t *src, Image<PixelFormat::RGB565, WIDTH, HEIGHT> &dst)
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
        inline void rgb565_to_raw(const Image<PixelFormat::RGB565, WIDTH, HEIGHT> &src, uint8_t *dst)
        {
            for (size_t i = 0; i < WIDTH * HEIGHT; ++i)
            {
                const auto &pixel = src(i % WIDTH, i / WIDTH);
                uint16_t raw_pixel = *reinterpret_cast<const uint16_t *>(&pixel);
                dst[i * 2] = static_cast<uint8_t>(raw_pixel & 0x00FF);
                dst[i * 2 + 1] = static_cast<uint8_t>((raw_pixel & 0xFF00) >> 8);
            }
        }

        template <typename ImageType>
        inline void copy(const ImageType &src, ImageType &dst)
        {
            static_assert(is_image<ImageType>::value, "ImageType must be an Image");
    
            auto* src_ptr = src.get_data_ptr();
            auto* dst_ptr = dst.get_data_ptr();
            size_t size = src.get_data_size();

            std::memcpy((void*)dst_ptr, src_ptr, size);
        }
        
    }

}
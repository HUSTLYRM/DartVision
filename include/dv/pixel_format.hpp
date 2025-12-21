#pragma once

#include <cstdint>
#include <cmath>
#include <array>

namespace dv
{
    namespace pixel_format
    {
        enum class PixelFormat
        {
            Binary,
            Grayscale,
            RGB565,
            RGB,
            LAB,
        };

        template <PixelFormat PF>
        struct PixelFormatTrait;
        
        struct GrayscalePixel {
            uint8_t value;
            operator uint8_t() & { return value; }

            static constexpr GrayscalePixel min()
            {
                return {0};
            }
            static constexpr GrayscalePixel max()
            {
                return {255};
            }

            bool operator<=(const GrayscalePixel &other) const
            {
                return value <= other.value;
            }
            bool operator>=(const GrayscalePixel &other) const
            {
                return value >= other.value;
            }

            bool operator==(const GrayscalePixel &other) const
            {
                return value == other.value;
            }

            GrayscalePixel& operator=(const uint8_t& v){
                value = v;
                return *this;
            }
            
        };


        template <>
        struct PixelFormatTrait<PixelFormat::Grayscale>
        {
            using type = GrayscalePixel;
            
        };

        struct RGB565Pixel
        {
            uint16_t r : 5;
            uint16_t g : 6;
            uint16_t b : 5;


            static constexpr RGB565Pixel min()
            {
                return {0, 0, 0};
            }
            static constexpr RGB565Pixel max()
            {
                return {31, 63, 31};
            }

            bool operator<=(const RGB565Pixel &other) const
            {
                return (r <= other.r) && (g <= other.g) && (b <= other.b);
            }
            bool operator>=(const RGB565Pixel &other) const
            {
                return (r >= other.r) && (g >= other.g) && (b >= other.b);
            }
        };

        using BinaryPixel = GrayscalePixel;
        template <>
        struct PixelFormatTrait<PixelFormat::Binary>
        {
            using type = BinaryPixel;
        };



        template <>
        struct PixelFormatTrait<PixelFormat::RGB565>
        {
            using type = RGB565Pixel;
        };

        struct RGBPixel
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;

            static constexpr RGBPixel min()
            {
                return {0, 0, 0};
            }
            static constexpr RGBPixel max()
            {
                return {255, 255, 255};
            }

            bool operator<=(const RGBPixel &other) const
            {
                return (r <= other.r) && (g <= other.g) && (b <= other.b);
            }
            bool operator>=(const RGBPixel &other) const
            {
                return (r >= other.r) && (g >= other.g) && (b >= other.b);
            }
        };

        template <>
        struct PixelFormatTrait<PixelFormat::RGB>
        {
            using type = RGBPixel;
        };

        struct LABPixel
        {
            int8_t l;
            int8_t a;
            int8_t b;

            bool operator<=(const LABPixel &other) const
            {
                return (l <= other.l) && (a <= other.a) && (b <= other.b);
            }
            bool operator>=(const LABPixel &other) const
            {
                return (l >= other.l) && (a >= other.a) && (b >= other.b);
            }
        };
        template <>
        struct PixelFormatTrait<PixelFormat::LAB>
        {
            using type = LABPixel;
            static constexpr type min()
            {
                return {0, -128, -128};
            }
            static constexpr type max()
            {
                return {100, 127, 127};
            }
        };

        inline const auto rgb565_to_rgb_lookup_tables_init_(){
            static std::array<RGBPixel, 65536> rgb565_to_rgb_lookup_table_;
            for (uint32_t i = 0; i <= 0xFFFF; ++i)
            {
                uint16_t pix = static_cast<uint16_t>(i);
                // RGB565: R:5 G:6 B:5
                uint8_t r5 = (pix >> 11) & 0x1F;
                uint8_t g6 = (pix >> 5) & 0x3F;
                uint8_t b5 = pix & 0x1F;
                // expand to 0..255
                uint8_t R8 = (r5 * 255 + 15) / 31;
                uint8_t G8 = (g6 * 255 + 31) / 63;
                uint8_t B8 = (b5 * 255 + 15) / 31;
                rgb565_to_rgb_lookup_table_[i] = RGBPixel{R8, G8, B8};
            }
            return rgb565_to_rgb_lookup_table_;
        }

        inline const auto rgb565_to_lab_lookup_tables_init_()
        {
            std::array<LABPixel, 65536> rgb565_to_lab_lookup_table;

            auto rgb565_to_rgb8 = [](uint16_t pix, uint8_t *r, uint8_t *g, uint8_t *b) -> void
            {
                // RGB565: R:5 G:6 B:5
                uint8_t r5 = (pix >> 11) & 0x1F;
                uint8_t g6 = (pix >> 5) & 0x3F;
                uint8_t b5 = pix & 0x1F;
                // expand to 0..255
                *r = (r5 * 255 + 15) / 31;
                *g = (g6 * 255 + 31) / 63;
                *b = (b5 * 255 + 15) / 31;
            };

            auto srgb_to_linear = [](float c) -> float
            {
                if (c <= 0.04045f)
                    return c / 12.92f;
                return powf((c + 0.055f) / 1.055f, 2.4f);
            };

            auto lab_f = [](float t) -> float
            {
                const float delta = 6.0f / 29.0f; // cube root threshold -> 0.008856 ~= (6/29)^3
                if (t > (delta * delta * delta))
                    return cbrtf(t);
                return (t / (3 * delta * delta)) + (4.0f / 29.0f); // equivalent of 7.787*t + 16/116
            };
            auto rgb565_to_lab = [rgb565_to_rgb8, srgb_to_linear, lab_f](uint16_t pix, float *L, float *a, float *b) -> void
            {
                uint8_t R8, G8, B8;
                rgb565_to_rgb8(pix, &R8, &G8, &B8);
                // normalize to 0..1
                float r = R8 / 255.0f;
                float g = G8 / 255.0f;
                float bb = B8 / 255.0f;
                // sRGB -> linear
                r = srgb_to_linear(r);
                g = srgb_to_linear(g);
                bb = srgb_to_linear(bb);
                // linear RGB -> XYZ (D65)
                // using matrix for sRGB D65
                float X = 0.4124564f * r + 0.3575761f * g + 0.1804375f * bb;
                float Y = 0.2126729f * r + 0.7151522f * g + 0.0721750f * bb;
                float Z = 0.0193339f * r + 0.1191920f * g + 0.9503041f * bb;
                // Normalize by reference white (D65)
                const float Xn = 0.95047f;
                const float Yn = 1.00000f;
                const float Zn = 1.08883f;
                float fx = lab_f(X / Xn);
                float fy = lab_f(Y / Yn);
                float fz = lab_f(Z / Zn);
                *L = 116.0f * fy - 16.0f;
                *a = 500.0f * (fx - fy);
                *b = 200.0f * (fy - fz);
            };

            for (uint32_t i = 0; i <= 0xFFFF; ++i)
            {
                float L, a, b;
                rgb565_to_lab(static_cast<uint16_t>(i), &L, &a, &b);
                rgb565_to_lab_lookup_table[i] = LABPixel{
                    static_cast<int8_t>(std::round(L)),
                    static_cast<int8_t>(std::round(a)),
                    static_cast<int8_t>(std::round(b))};
            }
            return rgb565_to_lab_lookup_table;
        }


        template<typename SRCT, typename DSTT>
        inline void pixel_cast(const SRCT&, DSTT&){
            static_assert(false, "Unsupported pixel format conversion");
            // throw std::runtime_error("Unsupported pixel format conversion");
        }

        template<typename T>
        inline void pixel_cast(const T& src, T& dst){
            dst = src;
        }

        template<>
        inline void pixel_cast(const RGB565Pixel &rgb565, RGBPixel &rgb)
        {
            const auto & rgb565_to_rgb_lookup_table_ = rgb565_to_rgb_lookup_tables_init_();

            rgb = rgb565_to_rgb_lookup_table_[*reinterpret_cast<const uint16_t *>(&rgb565)];
        }

        template<>
        inline void pixel_cast(const RGBPixel &rgb, RGB565Pixel &rgb565)
        {
            // compress to RGB565
            uint8_t r5 = static_cast<uint8_t>((rgb.r * 31 + 127) / 255);
            uint8_t g6 = static_cast<uint8_t>((rgb.g * 63 + 127) / 255);
            uint8_t b5 = static_cast<uint8_t>((rgb.b * 31 + 127) / 255);
            rgb565 = RGB565Pixel{r5, g6, b5};
        }

        template<>
        inline void pixel_cast(const RGB565Pixel &rgb565, LABPixel &lab)
        {
            const static auto & rgb565_to_lab_lookup_table = rgb565_to_lab_lookup_tables_init_();

            lab = rgb565_to_lab_lookup_table[*reinterpret_cast<const uint16_t *>(&rgb565)];
        }

        template<>
        inline void pixel_cast(const RGBPixel &rgb, GrayscalePixel &gray)
        {
            // Using Rec. 601 luma formula
            gray.value = static_cast<uint8_t>(std::round(0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b));
        }

        template<>
        inline void pixel_cast(const RGB565Pixel &rgb565, GrayscalePixel &gray)
        {
            RGBPixel rgb;
            pixel_cast(rgb565, rgb);
            pixel_cast(rgb, gray);
        }

        template<>
        inline void pixel_cast(const GrayscalePixel &gray, RGBPixel &rgb)
        {
            rgb = RGBPixel{gray.value, gray.value, gray.value};
        }

        template<>
        inline void pixel_cast(const GrayscalePixel &gray, RGB565Pixel &rgb565)
        {
            RGBPixel rgb;
            pixel_cast(gray, rgb);
            pixel_cast(rgb, rgb565);
        }
    }
}
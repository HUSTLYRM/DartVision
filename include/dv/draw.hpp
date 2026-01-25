#pragma once

#include <cstdint>
#include <cmath>
#include "dv/image.hpp"

namespace dv
{
    namespace draw
    {
        using namespace image;
        using namespace pixel_format;

        template <PixelFormat PF, size_t W, size_t H, typename Derived>
        inline void point(ImageBase<PF, W, H, Derived>& img, int x, int y, 
                         typename PixelFormatTrait<PF>::type color)
        {
            if (x >= 0 && x < static_cast<int>(W) && y >= 0 && y < static_cast<int>(H)) {
                img(x, y) = color;
            }
        }

        template <PixelFormat PF, size_t W, size_t H, typename Derived>
        inline void line(ImageBase<PF, W, H, Derived>& img, int x0, int y0, int x1, int y1,
                        typename PixelFormatTrait<PF>::type color)
        {
            int dx = std::abs(x1 - x0);
            int dy = std::abs(y1 - y0);
            int sx = (x0 < x1) ? 1 : -1;
            int sy = (y0 < y1) ? 1 : -1;
            int err = dx - dy;

            int x = x0;
            int y = y0;

            while (true) {
                point(img, x, y, color);

                if (x == x1 && y == y1) break;

                int e2 = 2 * err;
                if (e2 > -dy) {
                    err -= dy;
                    x += sx;
                }
                if (e2 < dx) {
                    err += dx;
                    y += sy;
                }
            }
        }

        template <PixelFormat PF, size_t W, size_t H, typename Derived>
        inline void circle(ImageBase<PF, W, H, Derived>& img, int cx, int cy, int radius,
                          typename PixelFormatTrait<PF>::type color)
        {
            int x = 0;
            int y = radius;
            int d = 3 - 2 * radius;

            while (x <= y) {
                point(img, cx + x, cy + y, color);
                point(img, cx - x, cy + y, color);
                point(img, cx + x, cy - y, color);
                point(img, cx - x, cy - y, color);
                point(img, cx + y, cy + x, color);
                point(img, cx - y, cy + x, color);
                point(img, cx + y, cy - x, color);
                point(img, cx - y, cy - x, color);

                if (d < 0) {
                    d = d + 4 * x + 6;
                } else {
                    d = d + 4 * (x - y) + 10;
                    y--;
                }
                x++;
            }
        }

        template <PixelFormat PF, size_t W, size_t H, typename Derived>
        inline void filled_circle(ImageBase<PF, W, H, Derived>& img, int cx, int cy, int radius,
                                 typename PixelFormatTrait<PF>::type color)
        {
            int x = 0;
            int y = radius;
            int d = 3 - 2 * radius;

            while (x <= y) {
                for (int i = cx - x; i <= cx + x; i++) {
                    point(img, i, cy + y, color);
                    point(img, i, cy - y, color);
                }
                for (int i = cx - y; i <= cx + y; i++) {
                    point(img, i, cy + x, color);
                    point(img, i, cy - x, color);
                }

                if (d < 0) {
                    d = d + 4 * x + 6;
                } else {
                    d = d + 4 * (x - y) + 10;
                    y--;
                }
                x++;
            }
        }

        template <PixelFormat PF, size_t W, size_t H, typename Derived>
        inline void rect(ImageBase<PF, W, H, Derived>& img, int x0, int y0, int x1, int y1,
                        typename PixelFormatTrait<PF>::type color)
        {
            line(img, x0, y0, x1, y0, color);
            line(img, x1, y0, x1, y1, color);
            line(img, x1, y1, x0, y1, color);
            line(img, x0, y1, x0, y0, color);
        }

        template <PixelFormat PF, size_t W, size_t H, typename Derived>
        inline void filled_rect(ImageBase<PF, W, H, Derived>& img, int x0, int y0, int x1, int y1,
                               typename PixelFormatTrait<PF>::type color)
        {
            int xmin = (x0 < x1) ? x0 : x1;
            int xmax = (x0 < x1) ? x1 : x0;
            int ymin = (y0 < y1) ? y0 : y1;
            int ymax = (y0 < y1) ? y1 : y0;

            for (int y = ymin; y <= ymax; y++) {
                for (int x = xmin; x <= xmax; x++) {
                    point(img, x, y, color);
                }
            }
        }

        struct SimpleBitmapFont {
            static constexpr uint8_t char_width = 5;
            static constexpr uint8_t char_height = 7;

            static inline constexpr uint8_t font_data[][5] = {
                // 空格
                {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
                // !
                {0b00100, 0b00100, 0b00100, 0b00000, 0b00100},
                // "
                {0b01010, 0b01010, 0b00000, 0b00000, 0b00000},
                // #
                {0b01010, 0b11111, 0b01010, 0b11111, 0b01010},
                // $
                {0b00100, 0b01110, 0b00100, 0b01110, 0b00100},
                // %
                {0b11000, 0b00100, 0b00100, 0b00100, 0b00011},
                // &
                {0b01000, 0b10100, 0b01000, 0b10101, 0b01010},
                // '
                {0b00100, 0b00100, 0b00000, 0b00000, 0b00000},
                // (
                {0b00010, 0b00100, 0b00100, 0b00100, 0b00010},
                // )
                {0b01000, 0b00100, 0b00100, 0b00100, 0b01000},
                // *
                {0b00100, 0b10101, 0b01110, 0b10101, 0b00100},
                // +
                {0b00100, 0b00100, 0b11111, 0b00100, 0b00100},
                // ,
                {0b00000, 0b00000, 0b00000, 0b00100, 0b01000},
                // -
                {0b00000, 0b00000, 0b11111, 0b00000, 0b00000},
                // .
                {0b00000, 0b00000, 0b00000, 0b00000, 0b00100},
                // /
                {0b00001, 0b00010, 0b00100, 0b01000, 0b10000},
                // 0
                {0b01110, 0b10011, 0b10101, 0b11001, 0b01110},
                // 1
                {0b00100, 0b01100, 0b00100, 0b00100, 0b01110},
                // 2
                {0b01110, 0b10001, 0b00010, 0b00100, 0b11111},
                // 3
                {0b11111, 0b00010, 0b00100, 0b10001, 0b01110},
                // 4
                {0b00010, 0b00110, 0b01010, 0b11111, 0b00010},
                // 5
                {0b11111, 0b10000, 0b11110, 0b00001, 0b11110},
                // 6
                {0b01110, 0b10000, 0b11110, 0b10001, 0b01110},
                // 7
                {0b11111, 0b00001, 0b00010, 0b00100, 0b01000},
                // 8
                {0b01110, 0b10001, 0b01110, 0b10001, 0b01110},
                // 9
                {0b01110, 0b10001, 0b01111, 0b00001, 0b01110},
                // :
                {0b00000, 0b00100, 0b00000, 0b00100, 0b00000},
                // ;
                {0b00000, 0b00100, 0b00000, 0b00100, 0b01000},
                // <
                {0b00010, 0b00100, 0b01000, 0b00100, 0b00010},
                // =
                {0b00000, 0b11111, 0b00000, 0b11111, 0b00000},
                // >
                {0b01000, 0b00100, 0b00010, 0b00100, 0b01000},
                // ?
                {0b01110, 0b10001, 0b00010, 0b00000, 0b00100},
                // @
                {0b01110, 0b10101, 0b10111, 0b10000, 0b01110},
                // A-Z
                {0b01110, 0b10001, 0b11111, 0b10001, 0b10001}, // A
                {0b11110, 0b10001, 0b11110, 0b10001, 0b11110}, // B
                {0b01110, 0b10001, 0b10000, 0b10001, 0b01110}, // C
                {0b11110, 0b10001, 0b10001, 0b10001, 0b11110}, // D
                {0b11111, 0b10000, 0b11110, 0b10000, 0b11111}, // E
                {0b11111, 0b10000, 0b11110, 0b10000, 0b10000}, // F
                {0b01110, 0b10001, 0b10000, 0b10011, 0b01110}, // G
                {0b10001, 0b10001, 0b11111, 0b10001, 0b10001}, // H
                {0b01110, 0b00100, 0b00100, 0b00100, 0b01110}, // I
                {0b00111, 0b00010, 0b00010, 0b10010, 0b01100}, // J
                {0b10001, 0b10010, 0b11100, 0b10010, 0b10001}, // K
                {0b10000, 0b10000, 0b10000, 0b10000, 0b11111}, // L
                {0b10001, 0b11011, 0b10101, 0b10001, 0b10001}, // M
                {0b10001, 0b11001, 0b10101, 0b10011, 0b10001}, // N
                {0b01110, 0b10001, 0b10001, 0b10001, 0b01110}, // O
                {0b11110, 0b10001, 0b11110, 0b10000, 0b10000}, // P
                {0b01110, 0b10001, 0b10001, 0b10010, 0b01101}, // Q
                {0b11110, 0b10001, 0b11110, 0b10010, 0b10001}, // R
                {0b01110, 0b10001, 0b01000, 0b00101, 0b01110}, // S
                {0b11111, 0b00100, 0b00100, 0b00100, 0b00100}, // T
                {0b10001, 0b10001, 0b10001, 0b10001, 0b01110}, // U
                {0b10001, 0b10001, 0b10001, 0b01010, 0b00100}, // V
                {0b10001, 0b10001, 0b10101, 0b11011, 0b10001}, // W
                {0b10001, 0b01010, 0b00100, 0b01010, 0b10001}, // X
                {0b10001, 0b01010, 0b00100, 0b00100, 0b00100}, // Y
                {0b11111, 0b00010, 0b00100, 0b01000, 0b11111}, // Z
            };
        };

        template <PixelFormat PF, size_t W, size_t H, typename Derived>
        inline void char_draw(ImageBase<PF, W, H, Derived>& img, int x, int y, char ch,
                             typename PixelFormatTrait<PF>::type color, int scale = 1)
        {
            // 支持的字符范围：空格(32)到'Z'(90)
            int char_idx = static_cast<int>(ch) - 32;
            if (char_idx < 0 || char_idx > 58) {
                return; // 不支持的字符
            }

            const auto& font = SimpleBitmapFont::font_data[char_idx];

            for (int row = 0; row < SimpleBitmapFont::char_height; row++) {
                uint8_t row_data = font[row];
                for (int col = 0; col < SimpleBitmapFont::char_width; col++) {
                    if (row_data & (1 << (SimpleBitmapFont::char_width - 1 - col))) {
                        for (int sy = 0; sy < scale; sy++) {
                            for (int sx = 0; sx < scale; sx++) {
                                point(img, x + col * scale + sx, y + row * scale + sy, color);
                            }
                        }
                    }
                }
            }
        }

        template <PixelFormat PF, size_t W, size_t H, typename Derived>
        inline void text(ImageBase<PF, W, H, Derived>& img, int x, int y, const char* text,
                        typename PixelFormatTrait<PF>::type color, int scale = 1)
        {
            int cur_x = x;
            const int char_width = SimpleBitmapFont::char_width * scale;
            const int char_height = SimpleBitmapFont::char_height * scale;

            for (const char* p = text; *p != '\0'; p++) {
                if (*p == '\n') {
                    cur_x = x;
                    y += char_height + scale;
                } else {
                    char_draw(img, cur_x, y, *p, color, scale);
                    cur_x += char_width + scale;
                }
            }
        }

    }
}


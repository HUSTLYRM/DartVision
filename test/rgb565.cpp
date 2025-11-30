#include <iostream>

#include <dv.hpp>

int main()
{
    std::cout << "Dart Vision Test Suite" << std::endl;

    auto file = fopen("img.bin", "rb");
    if (!file)
    {
        std::cerr << "Failed to open img.bin" << std::endl;
        return -1;
    }
    const size_t width = 320;
    const size_t height = 240;
    uint8_t *raw_data = new uint8_t[width * height * 2];
    fread(raw_data, 1, width * height * 2, file);
    fclose(file);

    dv::image::Image<dv::pixel_format::PixelFormat::RGB565, 320, 240> img_rgb565;
    dv::image::raw_to_rgb565(raw_data, img_rgb565);
    delete[] raw_data;
    std::cout << "Image loaded: " << img_rgb565.width() << "x" << img_rgb565.height() << std::endl;

    auto new_img = dv::image::Image<dv::pixel_format::PixelFormat::RGB565, 160, 120>();
    dv::interpolation::nearest_neighbor(img_rgb565, new_img);
    std::cout << "Resized Image: " << new_img.width() << "x" << new_img.height() << std::endl;

    file = fopen("out.bin" , "wb");
    if (!file)
    {
        std::cerr << "Failed to open out.bin for writing" << std::endl;
        return -1;
    }
    for (size_t y = 0; y < new_img.height(); ++y)
    {
        for (size_t x = 0; x < new_img.width(); ++x)
        {
            auto pixel = new_img(x, y);
            uint16_t pix_val;
            pix_val = *reinterpret_cast<uint16_t *>(&pixel);
            uint8_t bytes[2] = {static_cast<uint8_t>((pix_val >> 8) & 0xFF), static_cast<uint8_t>(pix_val & 0xFF)};
            fwrite(bytes, 2, 1, file);
        }
    }
    fclose(file);
    return 0;
}
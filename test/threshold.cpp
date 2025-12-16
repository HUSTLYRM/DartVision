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

    auto lab_img = dv::image::Image<dv::pixel_format::PixelFormat::LAB, 320, 240>();
    dv::image::image_cast(img_rgb565, lab_img);

    auto bin_img = dv::image::Image<dv::pixel_format::PixelFormat::Binary, 320, 240>();
    dv::binaryzation::threshold(lab_img, bin_img, dv::pixel_format::LABPixel{0, 0, 0}, dv::pixel_format::LABPixel{100, 127, 127});

    auto converted_img = dv::image::Image<dv::pixel_format::PixelFormat::RGB565, 320, 240>();
    dv::image::image_cast(bin_img, converted_img);
    file = fopen("out.bin" , "wb");
    if (!file)
    {
        std::cerr << "Failed to open out.bin for writing" << std::endl;
        return -1;
    }
    for (size_t y = 0; y < converted_img.height(); ++y)
    {
        for (size_t x = 0; x < converted_img.width(); ++x)
        {
            // std::cout << static_cast<int>(converted_img(x, y).r) << " " << static_cast<int>(converted_img(x, y).g) << " " << static_cast<int>(converted_img(x, y).b) << std::endl;
            // std::cout << sizeof(dv::pixel_format::RGB565Pixel);
            uint16_t pixel = *reinterpret_cast<uint16_t *>(&converted_img(x, y));
            // std::cout << pixel << std::endl;
            fwrite(&pixel, 2, 1, file);
        }
    }
    fclose(file);
    return 0;
}
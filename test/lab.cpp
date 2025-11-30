#include <iostream>

#include <dv.hpp>
#include <time.h>

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

    auto time_0 = clock();

    for(int i = 0; i < 1000; i++) {
    
    
    // delete[] raw_data;
    // std::cout << "Image loaded: " << img_rgb565.width() << "x" << img_rgb565.height() << std::endl;

    auto new_img = dv::image::Image<dv::pixel_format::PixelFormat::LAB, 320, 240>();

    dv::image::rgb565_to_lab(img_rgb565, new_img);
    // std::cout << "Converted to LAB format." << std::endl;

    
    }
    auto time_1 = clock();
    double elapsed_secs = double(time_1 - time_0) / CLOCKS_PER_SEC;

    std::cout << "Time taken for conversion: " << elapsed_secs/1000 << " seconds." << std::endl;

    
    
    return 0;
}
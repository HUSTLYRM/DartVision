from PIL import Image
import struct

def image_to_rgb565_bin(input_path, output_path):
    img = Image.open(input_path).convert('RGB')
    img = img.resize((320,240))
    width, height = img.size
    print(f"Image size: {width}x{height}")

    pixels = img.load()

    with open(output_path, 'wb') as f:
        for y in range(height):
            for x in range(width):
                r, g, b = pixels[x, y]

                rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

                f.write(struct.pack('<H', rgb565))

    print(f"RGB565 binary saved to: {output_path}")
    print(f"h x w: {width} x {height}")

if __name__ == "__main__":
    image_to_rgb565_bin("img.jpeg", "img.bin")
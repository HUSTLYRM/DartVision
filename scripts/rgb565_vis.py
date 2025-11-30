import numpy as np
import matplotlib.pyplot as plt
import struct

def rgb565_to_rgb(rgb565):
    r = (rgb565 >> 11) & 0x1F
    g = (rgb565 >> 5) & 0x3F
    b = rgb565 & 0x1F
    return (r << 3, g << 2, b << 3)

def visualize_rgb565_bin(bin_path, width, height):
    with open(bin_path, "rb") as f:
        data = f.read()

    if len(data) != width * height * 2:
        print("bin file size mismatch")
        print(f"File size: {len(data)} bytes, expected: {width*height*2} bytes")
        return

    pixels = []
    for i in range(0, len(data), 2):
        b1, b2 = data[i], data[i + 1]
        rgb565 = b1 | (b2 << 8)
        pixels.append(rgb565)

    pixels = np.array(pixels, dtype=np.uint16)

    rgb_img = np.zeros((height, width, 3), dtype=np.uint8)

    idx = 0
    for y in range(height):
        for x in range(width):
            rgb_img[y, x] = rgb565_to_rgb(pixels[idx])
            idx += 1

    plt.figure(figsize=(6, 6))
    plt.title("RGB565 Visualized from BIN")
    plt.imshow(rgb_img)
    plt.axis("off")
    plt.show()


if __name__ == "__main__":
    visualize_rgb565_bin(
        bin_path="../build/out.bin",
        width=320,
        height=240
    )
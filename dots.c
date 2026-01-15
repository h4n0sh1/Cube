// 3D Formula Renderer - Pure C
// Outputs PNG image that opens anywhere!

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 600

// Simple pixel buffer (RGB)
static uint32_t pixels[WIDTH * HEIGHT];

// Color helpers
#define RGB(r, g, b) ((uint32_t)(((r) << 16) | ((g) << 8) | (b)))
#define BLACK RGB(0, 0, 0)
#define WHITE RGB(255, 255, 255)
#define RED   RGB(255, 0, 0)
#define GREEN RGB(0, 255, 0)
#define BLUE  RGB(0, 0, 255)

// Fill the entire buffer with a color
void fill(uint32_t color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        pixels[i] = color;
    }
}

// Draw a single pixel (with bounds checking)
void draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        pixels[y * WIDTH + x] = color;
    }
}

// Draw a filled circle (for visible points)
void draw_point(int cx, int cy, int radius, uint32_t color) {
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {
                draw_pixel(cx + dx, cy + dy, color);
            }
        }
    }
}

// Save the pixel buffer to a PNG file
int save_png(const char *filename) {
    // Convert to RGB byte array for stb
    uint8_t *rgb = malloc(WIDTH * HEIGHT * 3);
    if (!rgb) return 0;

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        rgb[i * 3 + 0] = (pixels[i] >> 16) & 0xFF;
        rgb[i * 3 + 1] = (pixels[i] >> 8) & 0xFF;
        rgb[i * 3 + 2] = pixels[i] & 0xFF;
    }

    int result = stbi_write_png(filename, WIDTH, HEIGHT, 3, rgb, WIDTH * 3);
    free(rgb);
    return result;
}

int main(int argc, char *argv[]) {
    printf("3D Formula Renderer\n");
    printf("===================\n\n");

    // Clear to black
    fill(BLACK);

    // Draw a white point in the center
    draw_point(WIDTH / 2, HEIGHT / 2, 10, WHITE);

    // Draw some colored points around it
    draw_point(WIDTH / 2 - 100, HEIGHT / 2, 8, RED);
    draw_point(WIDTH / 2 + 100, HEIGHT / 2, 8, GREEN);
    draw_point(WIDTH / 2, HEIGHT / 2 - 100, 8, BLUE);

    // Save to file
    const char *output = "output.png";
    if (save_png(output)) {
        printf("Success! Image saved to: %s\n", output);
        printf("Open it with any image viewer.\n");
    } else {
        return 1;
    }

    return 0;
}

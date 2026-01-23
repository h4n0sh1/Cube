// 3D Cube Renderer - Pure C Software Rasterizer
// Outputs PNG image with a 3D wireframe cube

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 600
#define PI 3.14159265358979323846

// Simple pixel buffer (RGB)
static uint32_t pixels[WIDTH * HEIGHT];

// Color helpers
#define RGB(r, g, b) ((uint32_t)(((r) << 16) | ((g) << 8) | (b)))
#define BLACK RGB(0, 0, 0)
#define WHITE RGB(255, 255, 255)
#define RED   RGB(255, 0, 0)
#define GREEN RGB(0, 255, 0)
#define BLUE  RGB(0, 0, 255)
#define CYAN  RGB(0, 255, 255)
#define YELLOW RGB(255, 255, 0)
#define MAGENTA RGB(255, 0, 255)

// 3D Vector structure
typedef struct {
    float x, y, z;
} Vec3;

// 2D Point (screen coordinates)
typedef struct {
    int x, y;
} Point2D;

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

// Draw a filled circle (for visible points/vertices)
void draw_point(int cx, int cy, int radius, uint32_t color) {
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {  
                draw_pixel(cx + dx, cy + dy, color);
            }
        }
    }
}

// Draw a line using Bresenham's algorithm
void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

// Rotate a point around the X axis
Vec3 rotate_x(Vec3 p, float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (Vec3){
        p.x,
        p.y * c - p.z * s,
        p.y * s + p.z * c
    };
}

// Rotate a point around the Y axis
Vec3 rotate_y(Vec3 p, float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (Vec3){
        p.x * c + p.z * s,
        p.y,
        -p.x * s + p.z * c
    };
}

// Rotate a point around the Z axis
Vec3 rotate_z(Vec3 p, float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (Vec3){
        p.x * c - p.y * s,
        p.x * s + p.y * c,
        p.z
    };
}

// Project 3D point to 2D screen coordinates (perspective projection)
Point2D project(Vec3 p, float fov, float viewer_distance) {
    // Move the cube away from the camera
    float z = p.z + viewer_distance;
    
    // Perspective divide
    float factor = fov / z;
    
    // Project and center on screen
    return (Point2D){
        (int)(p.x * factor + WIDTH / 2),
        (int)(-p.y * factor + HEIGHT / 2)  // Flip Y (screen Y grows down)
    };
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

// Draw a 3D wireframe cube
void draw_cube(float size, float rot_x, float rot_y, float rot_z, 
               float fov, float distance, uint32_t edge_color, uint32_t vertex_color) {
    // Define the 8 vertices of a cube centered at origin
    Vec3 vertices[8] = {
        {-size, -size, -size},  // 0: back-bottom-left
        { size, -size, -size},  // 1: back-bottom-right
        { size,  size, -size},  // 2: back-top-right
        {-size,  size, -size},  // 3: back-top-left
        {-size, -size,  size},  // 4: front-bottom-left
        { size, -size,  size},  // 5: front-bottom-right
        { size,  size,  size},  // 6: front-top-right
        {-size,  size,  size},  // 7: front-top-left
    };

    // Define 12 edges (pairs of vertex indices)
    int edges[12][2] = {
        // Back face
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        // Front face
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        // Connecting edges
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    // Transform and project all vertices
    Point2D projected[8];
    for (int i = 0; i < 8; i++) {
        Vec3 v = vertices[i];
        
        // Apply rotations
        v = rotate_x(v, rot_x);
        v = rotate_y(v, rot_y);
        v = rotate_z(v, rot_z);
        
        // Project to 2D
        projected[i] = project(v, fov, distance);
    }

    // Draw all edges
    for (int i = 0; i < 12; i++) {
        int v0 = edges[i][0];
        int v1 = edges[i][1];
        draw_line(projected[v0].x, projected[v0].y,
                  projected[v1].x, projected[v1].y, edge_color);
    }

    // Draw vertices as small circles
    for (int i = 0; i < 8; i++) {
        draw_point(projected[i].x, projected[i].y, 4, vertex_color);
    }
}

int main(int argc, char *argv[]) {
    printf("3D Cube Renderer\n");
    printf("================\n\n");

    // Clear to black
    fill(BLACK);

    // Cube parameters
    float cube_size = 1.0f;
    float fov = 500.0f;           // Field of view (larger = less perspective)
    float distance = 4.0f;        // Distance from camera

    // Rotation angles (in radians)
    float rot_x = 0.5f;           // ~30 degrees
    float rot_y = 0.8f;           // ~45 degrees  
    float rot_z = 0.0f;

    // Draw the cube
    draw_cube(cube_size, rot_x, rot_y, rot_z, fov, distance, CYAN, YELLOW);

    // Save to file
    const char *output = "output.png";
    if (save_png(output)) {
        printf("Success! Image saved to: %s\n", output);
        printf("Open it with any image viewer.\n");
    } else {
        printf("Failed to save image!\n");
        return 1;
    }

    return 0;
}

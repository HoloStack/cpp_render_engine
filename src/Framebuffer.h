#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Vec3.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <limits>

class Framebuffer {
private:
    int width, height;
    std::vector<Color> colorBuffer;
    std::vector<float> depthBuffer;

public:
    Framebuffer(int w, int h) : width(w), height(h) {
        colorBuffer.resize(width * height);
        depthBuffer.resize(width * height);
        clear();
    }

    void clear(Color color = Color(0, 0, 0)) {
        std::fill(colorBuffer.begin(), colorBuffer.end(), color);
        std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::max());
    }

    void setPixel(int x, int y, const Color& color, float depth = 0.0f) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            int index = y * width + x;
            if (depth < depthBuffer[index]) {
                colorBuffer[index] = color;
                depthBuffer[index] = depth;
            }
        }
    }

    Color getPixel(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return colorBuffer[y * width + x];
        }
        return Color(0, 0, 0);
    }

    float getDepth(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return depthBuffer[y * width + x];
        }
        return std::numeric_limits<float>::max();
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Lesson 1: Bresenham's Line Drawing Algorithm
    void drawLine(int x0, int y0, int x1, int y1, const Color& color) {
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;

        while (true) {
            setPixel(x0, y0, color);
            
            if (x0 == x1 && y0 == y1) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

    // Helper function for barycentric coordinates
    Vec3 barycentric(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c) const {
        float denom = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
        if (std::abs(denom) < 0.001f) return Vec3(-1, 0, 0); // Degenerate triangle
        
        float w0 = ((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y)) / denom;
        float w1 = ((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y)) / denom;
        float w2 = 1.0f - w0 - w1;
        
        return Vec3(w0, w1, w2);
    }

    // Lesson 2: Triangle rasterization
    void drawTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Color& color) {
        Vec2 bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Vec2 bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        Vec2 clamp(width - 1, height - 1);

        // Find bounding box
        bboxmin.x = std::max(0.0f, std::min({bboxmin.x, v0.x, v1.x, v2.x}));
        bboxmin.y = std::max(0.0f, std::min({bboxmin.y, v0.y, v1.y, v2.y}));
        bboxmax.x = std::min(clamp.x, std::max({bboxmax.x, v0.x, v1.x, v2.x}));
        bboxmax.y = std::min(clamp.y, std::max({bboxmax.y, v0.y, v1.y, v2.y}));

        Vec2 P;
        for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
            for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
                Vec3 bc_screen = barycentric(P, Vec2(v0.x, v0.y), Vec2(v1.x, v1.y), Vec2(v2.x, v2.y));
                if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
                
                // Lesson 3: Z-buffer (depth testing)
                float z = v0.z * bc_screen.x + v1.z * bc_screen.y + v2.z * bc_screen.z;
                setPixel((int)P.x, (int)P.y, color, z);
            }
        }
    }

    // Save as PPM file
    void saveToPPM(const std::string& filename) const {
        std::ofstream file(filename);
        file << "P3\n" << width << " " << height << "\n255\n";
        for (int y = height - 1; y >= 0; y--) {
            for (int x = 0; x < width; x++) {
                Color pixel = getPixel(x, y);
                file << (int)pixel.r << " " << (int)pixel.g << " " << (int)pixel.b << " ";
            }
            file << "\n";
        }
    }
};

#endif

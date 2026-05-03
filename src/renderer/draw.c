#include "draw.h"

#include <math.h>

void draw_fill_color(uint32_t *pixels, int width, int height, uint32_t color)
{
    int count = width * height;
    for (int i = 0; i < count; i++) {
        pixels[i] = color;
    }
}

/* 2D cross product of vectors (a->b) and (a->p).
   Positive when p is to the left of edge a->b (CCW winding). */
static float edge_fn(float ax, float ay, float bx, float by, float px, float py)
{
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

void draw_triangle(uint32_t *pixels, int width, int height,
                   float x0, float y0,
                   float x1, float y1,
                   float x2, float y2,
                   uint32_t color)
{
    /* Bounding box, clamped to framebuffer. */
    int minX = (int)floorf(fminf(x0, fminf(x1, x2)));
    int minY = (int)floorf(fminf(y0, fminf(y1, y2)));
    int maxX = (int)ceilf (fmaxf(x0, fmaxf(x1, x2)));
    int maxY = (int)ceilf (fmaxf(y0, fmaxf(y1, y2)));

    if (minX < 0)       minX = 0;
    if (minY < 0)       minY = 0;
    if (maxX >= width)  maxX = width  - 1;
    if (maxY >= height) maxY = height - 1;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            /* Sample at pixel center. */
            float px = (float)x + 0.5f;
            float py = (float)y + 0.5f;

            float w0 = edge_fn(x1, y1, x2, y2, px, py);
            float w1 = edge_fn(x2, y2, x0, y0, px, py);
            float w2 = edge_fn(x0, y0, x1, y1, px, py);

            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
                pixels[y * width + x] = color;
            }
        }
    }
}

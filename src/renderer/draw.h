#pragma once

/* Internal renderer drawing primitives — not part of the public API. */

#include <stdint.h>

/* Fill every pixel with a solid color (ARGB8888). */
void draw_fill_color(uint32_t *pixels, int width, int height, uint32_t color);

/* Fill a triangle defined by three screen-space vertices.
   Vertices should be in counter-clockwise order for correct winding.
   Pixels outside the framebuffer bounds are safely clipped. */
void draw_triangle(uint32_t *pixels, int width, int height,
                   float x0, float y0,
                   float x1, float y1,
                   float x2, float y2,
                   uint32_t color);

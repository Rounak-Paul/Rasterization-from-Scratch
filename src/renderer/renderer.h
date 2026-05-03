#pragma once

#include <stdbool.h>
#include <stdint.h>

/*
 * Renderer — a simple CPU software rasterizer surface backed by SDL3.
 *
 * Typical usage:
 *
 *   Renderer *r = renderer_create(800, 600, "My Window");
 *   while (renderer_poll_events(r)) {
 *       renderer_clear(r, 0xFF000000);
 *       renderer_draw_indexed(r, verts, vert_count, indices, idx_count, color);
 *       renderer_present(r);
 *   }
 *   renderer_destroy(r);
 */

typedef struct Renderer Renderer;

/* 2D screen-space vertex. */
typedef struct {
    float x, y;
} Vertex2D;

/* Lifecycle ---------------------------------------------------------------- */

Renderer *renderer_create(int width, int height, const char *title);
void      renderer_destroy(Renderer *renderer);

/* Per-frame ---------------------------------------------------------------- */

/* Clear the framebuffer to color (ARGB8888). Pass 0xFF000000 for opaque black. */
void renderer_clear(Renderer *renderer, uint32_t color);

/* Upload the pixel buffer and blit it to the window. */
void renderer_present(Renderer *renderer);

/* Drain the SDL event queue.
   Returns false when the user requests a quit. */
bool renderer_poll_events(Renderer *renderer);

/* Framebuffer dimensions. */
int renderer_get_width(const Renderer *renderer);
int renderer_get_height(const Renderer *renderer);

/* Drawing ------------------------------------------------------------------ */

/*
 * Draw filled triangles from a vertex + index buffer.
 *
 *   vertices      — array of Vertex2D in screen space
 *   vertex_count  — number of entries in vertices[]
 *   indices       — flat list of vertex indices, every 3 form one triangle
 *   index_count   — must be a multiple of 3
 *   color         — ARGB8888 fill color for all triangles
 */
void renderer_draw_indexed(Renderer    *renderer,
                           const Vertex2D  *vertices, int vertex_count,
                           const uint32_t  *indices,  int index_count,
                           uint32_t         color);


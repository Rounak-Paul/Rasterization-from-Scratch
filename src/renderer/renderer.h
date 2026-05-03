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
 *       renderer_clear(r);
 *       // ... draw into renderer_get_pixels(r) ...
 *       renderer_present(r);
 *   }
 *   renderer_destroy(r);
 */

typedef struct Renderer Renderer;

/* Lifecycle ---------------------------------------------------------------- */

/* Open a window of (width x height) and allocate a matching pixel buffer.
   Returns NULL on failure; check SDL_GetError() for details. */
Renderer *renderer_create(int width, int height, const char *title);

/* Release all resources and close the window. */
void renderer_destroy(Renderer *renderer);

/* Per-frame ---------------------------------------------------------------- */

/* Zero out the entire pixel buffer (renders as opaque black). */
void renderer_clear(Renderer *renderer);

/* Upload the pixel buffer to the GPU texture and blit it to the window. */
void renderer_present(Renderer *renderer);

/* Drain the SDL event queue.
   Returns false when the user requests a quit (close button / Alt-F4 / etc.). */
bool renderer_poll_events(Renderer *renderer);

/* Pixel buffer access ------------------------------------------------------ */

/* Raw pixel buffer in ARGB8888 format, row-major, width * height elements.
   Modify freely between renderer_clear() and renderer_present(). */
uint32_t *renderer_get_pixels(Renderer *renderer);

/* Framebuffer dimensions. */
int renderer_get_width(const Renderer *renderer);
int renderer_get_height(const Renderer *renderer);

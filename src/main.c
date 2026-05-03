#include "renderer/renderer.h"

#include <stdio.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE  "Rasterization from Scratch"

int main(void)
{
    Renderer *renderer = renderer_create(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer.\n");
        return 1;
    }

    while (renderer_poll_events(renderer)) {
        /* Clear framebuffer to black (all zeros). */
        renderer_clear(renderer);

        /* Future blog posts will draw into renderer_get_pixels() here. */

        renderer_present(renderer);
    }

    renderer_destroy(renderer);
    return 0;
}

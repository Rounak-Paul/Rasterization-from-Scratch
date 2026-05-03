#include "app.h"

#include <math.h>
#include <stdint.h>

void app_update(Renderer *renderer, float time)
{
    int width  = renderer_get_width(renderer);
    int height = renderer_get_height(renderer);

    /* Sine-driven clear color — each channel at a different frequency and phase. */
    uint8_t r = (uint8_t)((sinf(time * 1.0f)         * 0.5f + 0.5f) * 255.0f);
    uint8_t g = (uint8_t)((sinf(time * 1.3f + 2.09f) * 0.5f + 0.5f) * 255.0f);
    uint8_t b = (uint8_t)((sinf(time * 0.7f + 4.19f) * 0.5f + 0.5f) * 255.0f);
    uint32_t bg = (0xFFu << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;

    renderer_clear(renderer, bg);

    /* Triangle vertex and index buffers. */
    const Vertex2D vertices[] = {
        { (float)width * 0.5f,  (float)height * 0.13f },  /* 0: top   */
        { (float)width * 0.12f, (float)height * 0.87f },  /* 1: left  */
        { (float)width * 0.88f, (float)height * 0.87f },  /* 2: right */
    };
    const uint32_t indices[] = { 0, 1, 2 };

    renderer_draw_indexed(renderer,
                          vertices, 3,
                          indices,  3,
                          0xFFFFFFFF);
}

#include "renderer.h"

#include <SDL3/SDL.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct Renderer {
    SDL_Window   *window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture  *framebuffer;
    uint32_t     *pixels;
    int           width;
    int           height;
    bool          quit;
};

/* -------------------------------------------------------------------------- */

Renderer *renderer_create(int width, int height, const char *title)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return NULL;
    }

    Renderer *r = calloc(1, sizeof(Renderer));
    if (!r) return NULL;

    r->width  = width;
    r->height = height;
    r->quit   = false;

    r->window = SDL_CreateWindow(title, width, height, 0);
    if (!r->window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        goto fail;
    }

    r->sdl_renderer = SDL_CreateRenderer(r->window, NULL);
    if (!r->sdl_renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        goto fail;
    }

    /* Streaming texture — we upload CPU pixels every frame. */
    r->framebuffer = SDL_CreateTexture(
        r->sdl_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );
    if (!r->framebuffer) {
        SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
        goto fail;
    }

    r->pixels = calloc((size_t)(width * height), sizeof(uint32_t));
    if (!r->pixels) {
        SDL_Log("renderer_create: out of memory");
        goto fail;
    }

    return r;

fail:
    renderer_destroy(r);
    return NULL;
}

void renderer_destroy(Renderer *renderer)
{
    if (!renderer) return;
    free(renderer->pixels);
    if (renderer->framebuffer)  SDL_DestroyTexture(renderer->framebuffer);
    if (renderer->sdl_renderer) SDL_DestroyRenderer(renderer->sdl_renderer);
    if (renderer->window)       SDL_DestroyWindow(renderer->window);
    free(renderer);
    SDL_Quit();
}

/* -------------------------------------------------------------------------- */

void renderer_clear(Renderer *renderer)
{
    memset(renderer->pixels, 0,
           (size_t)(renderer->width * renderer->height) * sizeof(uint32_t));
}

void renderer_present(Renderer *renderer)
{
    /* Upload the CPU pixel buffer into the streaming texture. */
    SDL_UpdateTexture(renderer->framebuffer, NULL,
                      renderer->pixels,
                      renderer->width * (int)sizeof(uint32_t));

    SDL_RenderClear(renderer->sdl_renderer);
    SDL_RenderTexture(renderer->sdl_renderer, renderer->framebuffer, NULL, NULL);
    SDL_RenderPresent(renderer->sdl_renderer);
}

bool renderer_poll_events(Renderer *renderer)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            renderer->quit = true;
        }
    }
    return !renderer->quit;
}

/* -------------------------------------------------------------------------- */

uint32_t *renderer_get_pixels(Renderer *renderer)
{
    return renderer->pixels;
}

int renderer_get_width(const Renderer *renderer)
{
    return renderer->width;
}

int renderer_get_height(const Renderer *renderer)
{
    return renderer->height;
}

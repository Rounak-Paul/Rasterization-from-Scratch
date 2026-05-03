# Rasterization from Scratch: Project Setup

## What we're building

Theory is done. Now we write code.

This series implements a software rasterizer — entirely on the CPU, writing pixels directly into a framebuffer. No OpenGL. No Vulkan. No shortcuts.

The goal: understand exactly what a GPU does, by doing it ourselves.

* * *

## Source code

Everything is on GitHub:

[**github.com/Rounak-Paul/rasterization-from-scratch**](https://github.com/Rounak-Paul/rasterization-from-scratch)

* * *

## Stack

*   **C11**
    
*   **SDL3** — window + framebuffer access only, nothing else
    
*   **CMake** — build system
    

* * *

## The renderer API

```c
Renderer *renderer_create(int width, int height, const char *title);
void      renderer_destroy(Renderer *renderer);

bool      renderer_poll_events(Renderer *renderer);
void      renderer_clear(Renderer *renderer);
void      renderer_present(Renderer *renderer);

uint32_t *renderer_get_pixels(Renderer *renderer);
```

`renderer_get_pixels()` returns a raw `uint32_t*` in ARGB8888 format. That's where all drawing happens — just write pixel values into the array.

* * *

## The loop

```c
while (renderer_poll_events(renderer)) {
    renderer_clear(renderer);

    // draw here

    renderer_present(renderer);
}
```

`renderer_clear` zeros the buffer. `renderer_present` uploads it to the screen via a streaming SDL texture.

Right now "draw here" is empty. The window opens, shows black, closes when you quit.

* * *

## Build

```sh
git clone --recurse-submodules https://github.com/Rounak-Paul/rasterization-from-scratch
cd rasterization-from-scratch
cmake -S . -B build
cmake --build build --parallel
./build/rasterizer
```

* * *

## Renderer internals

The `Renderer` struct holds everything:

```c
struct Renderer {
    SDL_Window   *window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture  *framebuffer;
    uint32_t     *pixels;
    int           width;
    int           height;
    bool          quit;
};
```

Two separate things live here: the SDL rendering context and our own CPU pixel buffer. They're kept in sync once per frame — not continuously.

### The pixel buffer

`pixels` is a plain heap-allocated array:

```c
r->pixels = calloc((size_t)(width * height), sizeof(uint32_t));
```

Row-major, ARGB8888. Pixel at `(x, y)` is `pixels[y * width + x]`. That's the only addressing scheme used throughout this entire series.

### The streaming texture

SDL doesn't let you write to the screen directly. Instead we create a *streaming* texture — one that's designed to be re-uploaded from the CPU every frame:

```c
r->framebuffer = SDL_CreateTexture(
    r->sdl_renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    width, height
);
```

### The present path

Each call to `renderer_present` does three things:

1.  Upload — copy the CPU buffer into the GPU texture:
    

```c
SDL_UpdateTexture(renderer->framebuffer, NULL,
                  renderer->pixels,
                  renderer->width * (int)sizeof(uint32_t));
```

2.  Blit — stretch the texture to fill the window:
    

```c
SDL_RenderTexture(renderer->sdl_renderer, renderer->framebuffer, NULL, NULL);
```

3.  Swap — flip the back buffer:
    

```c
SDL_RenderPresent(renderer->sdl_renderer);
```

`SDL_UpdateTexture` is not fast — it's a full CPU→GPU upload every frame. That's fine for a software rasterizer; the bottleneck will always be the math we write, not the upload.

### Clear

```c
void renderer_clear(Renderer *renderer)
{
    memset(renderer->pixels, 0,
           (size_t)(renderer->width * renderer->height) * sizeof(uint32_t));
}
```

Just a `memset`. ARGB8888 all-zeros is opaque black (`0x00000000` — technically transparent black, but SDL composites it onto a black background so it looks the same).

* * *

## Final takeaway

The renderer is a flat pixel array.  
Everything from here — lines, triangles, shading — is just math written into that array.

Next post: drawing a triangle.
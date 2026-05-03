# Rasterization from Scratch: Drawing a Triangle

## What we're doing

Last post: black window, nothing drawn.

This post: a full-screen cycling color and a filled triangle — the first real pixels we write ourselves.

Two new things to the codebase: a `draw` module inside the renderer for drawing primitives, and an `app` module so `main.c` stops doing everything.

---

## Code organization

```
src/
├── main.c              ← entry point only
├── app.h / app.c       ← what to draw each frame
└── renderer/
    ├── renderer.h      ← public API
    ├── renderer.c
    ├── draw.h          ← internal, not exposed to app
    └── draw.c
```

`main.c` has one job: create the renderer, run the loop, destroy the renderer.  
`app.c` has one job: decide what to draw each frame — via the renderer's public API only.  
`draw.c` is internal to the renderer. The app never sees it.

This mirrors how a real GPU renderer works: the application submits draw calls through an API, it doesn't know or care how triangles become pixels underneath.

---

## Passing time

`main.c` now passes elapsed seconds to `app_update`:

```c
while (renderer_poll_events(renderer)) {
    float time = (float)SDL_GetTicks() / 1000.0f;

    app_update(renderer, time);
    renderer_present(renderer);
}
```

`app_update` owns the clear — it decides what color to clear to each frame. `main.c` has no opinion about that.

---

## The renderer's draw API

`renderer_clear` now takes a color — no separate fill function needed:

```c
void renderer_clear(Renderer *renderer, uint32_t color);
```

For geometry, the renderer exposes a proper draw call — vertex buffer, index buffer, color:

```c
void renderer_draw_indexed(Renderer       *renderer,
                           const Vertex2D *vertices, int vertex_count,
                           const uint32_t *indices,  int index_count,
                           uint32_t        color);
```

`Vertex2D` is just `{ float x, y; }`. Every three indices form one triangle. The app never touches pixels directly, never includes `draw.h`.

This is the same model as a real GPU API: submit vertex data + index data, the renderer handles everything else.

---

## Filling the screen with color

`draw_fill_color` is the simplest possible draw call — loop over every pixel and write a value:

```c
void draw_fill_color(uint32_t *pixels, int width, int height, uint32_t color)
{
    int count = width * height;
    for (int i = 0; i < count; i++) {
        pixels[i] = color;
    }
}
```

Pixels are ARGB8888. High byte is alpha (always `0xFF`), then R, G, B.

---

## Cycling color with sine

In `app_update`, each RGB channel is driven by a sine wave at a different frequency and phase:

```c
uint8_t r = (uint8_t)((sinf(time * 1.0f)         * 0.5f + 0.5f) * 255.0f);
uint8_t g = (uint8_t)((sinf(time * 1.3f + 2.09f) * 0.5f + 0.5f) * 255.0f);
uint8_t b = (uint8_t)((sinf(time * 0.7f + 4.19f) * 0.5f + 0.5f) * 255.0f);

uint32_t color = (0xFFu << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;

renderer_clear(renderer, color);
```

`sinf` outputs `[-1, 1]`. The `* 0.5f + 0.5f` maps that to `[0, 1]`. Multiply by 255 and cast to `uint8_t` for the channel value.

The three channels never sync up — different frequencies and offsets `2.09` (~2π/3) and `4.19` (~4π/3) spread them evenly around the cycle. The result is a smooth, never-repeating color drift.

![screenshot]

---

## Drawing a triangle

Now something more interesting: a filled triangle at fixed screen-space coordinates.

The math is exactly what post 1 covered. For every pixel in the triangle's bounding box, evaluate three edge functions. If all three are non-negative, the pixel is inside.

```c
void draw_triangle(uint32_t *pixels, int width, int height,
                   float x0, float y0,
                   float x1, float y1,
                   float x2, float y2,
                   uint32_t color)
{
    int minX = (int)floorf(fminf(x0, fminf(x1, x2)));
    int minY = (int)floorf(fminf(y0, fminf(y1, y2)));
    int maxX = (int)ceilf (fmaxf(x0, fmaxf(x1, x2)));
    int maxY = (int)ceilf (fmaxf(y0, fmaxf(y1, y2)));

    /* Clamp to framebuffer bounds */
    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= width)  maxX = width  - 1;
    if (maxY >= height) maxY = height - 1;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            float px = (float)x + 0.5f;
            float py = (float)y + 0.5f;

            float w0 = edge(x1, y1, x2, y2, px, py);
            float w1 = edge(x2, y2, x0, y0, px, py);
            float w2 = edge(x0, y0, x1, y1, px, py);

            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
                pixels[y * width + x] = color;
            }
        }
    }
}
```

The edge function is a 2D cross product — positive on one side of the edge, negative on the other:

```c
static float edge(float ax, float ay, float bx, float by, float px, float py)
{
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}
```

Sample point is the pixel center `(x + 0.5, y + 0.5)` — avoids artifacts on shared edges between adjacent triangles.

In `app_update`, define vertex and index buffers and submit one draw call:

```c
const Vertex2D vertices[] = {
    { width * 0.5f,  height * 0.13f },  /* 0: top   */
    { width * 0.12f, height * 0.87f },  /* 1: left  */
    { width * 0.88f, height * 0.87f },  /* 2: right */
};
const uint32_t indices[] = { 0, 1, 2 };

renderer_draw_indexed(renderer, vertices, 3, indices, 3, 0xFFFFFFFF);
```

The app declares geometry as data. The renderer iterates the index triplets and rasterizes each triangle. Adding a second triangle later is just adding more indices — the draw call doesn't change.

![screenshot]

---

## Final takeaway

`draw_fill_color` and `draw_triangle` are both just loops writing `uint32_t` values into an array — internal implementation the app never sees.

There's no GPU, no shader, no API between us and the pixels. That's the point.

Next post: interpolating vertex colors across the triangle using barycentric coordinates.

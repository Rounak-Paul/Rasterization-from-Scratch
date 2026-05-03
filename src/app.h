#pragma once

#include "renderer/renderer.h"

/* Called once per frame. time is elapsed seconds since startup. */
void app_update(Renderer *renderer, float time);

// src/renderer.h

#ifndef RENDERER_H
#define RENDERER_H

// Define GLFW_INCLUDE_NONE antes de incluir GLFW/glfw3.h
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void renderer_init();
void renderer_clear();
void renderer_draw_world();
void renderer_cleanup();
void renderer_reset();

#endif  // RENDERER_H

// src/camera.h

#ifndef CAMERA_H
#define CAMERA_H

// Define GLFW_INCLUDE_NONE antes de incluir GLFW/glfw3.h
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Declarações das funções...

void camera_init();
void camera_cleanup();
void camera_process_input(GLFWwindow* window, float deltaTime);
void camera_process_mouse(double xpos, double ypos);
float* camera_get_view_matrix();
float* camera_get_projection_matrix();

#endif  // CAMERA_H

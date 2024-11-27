#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "world.h"

void player_init();
void player_update(float deltaTime, GLFWwindow* window);
void player_process_mouse(double xpos, double ypos);
void player_get_view_matrix(float* view_matrix);
void player_reset_position();
void player_get_position(vec3 out_position);
void player_break_block();
void player_place_block();

#endif  // PLAYER_H

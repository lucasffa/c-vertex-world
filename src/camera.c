// src/camera.c

#include "camera.h"

#include <cglm/cglm.h>

// Estado da câmera
static vec3 position = {0.0f, 25.0f, 3.0f};
static vec3 front = {0.0f, 0.0f, -1.0f};
static vec3 up = {0.0f, 1.0f, 0.0f};
static vec3 right;
static vec3 world_up = {0.0f, 1.0f, 0.0f};

static mat4 view_matrix;
static mat4 projection_matrix;

static float yaw = -90.0f;  // Rotação inicial no eixo Y (olhando para frente)
static float pitch = 0.0f;  // Rotação inicial no eixo X
static float sensitivity = 0.1f;          // Sensibilidade do mouse
static float last_x = 400, last_y = 300;  // Centro inicial do cursor
static int first_mouse = 1;

// Variáveis adicionais
static float current_speed = 0.0f;
static const float max_speed = 5.0f;
static const float acceleration = 10.0f;  // Unidades por segundo ao quadrado
static const float deceleration = 10.0f;  // Unidades por segundo ao quadrado

// Variáveis para gravidade e pulo
static float vertical_speed = 0.0f;
static const float gravity = -9.81f;  // Unidades por segundo ao quadrado
static const float jump_speed = 5.0f;
static int is_jumping = 0;

void camera_init() {
  glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f,
                  projection_matrix);
  glm_lookat(position,
             (vec3){position[0] + front[0], position[1] + front[1],
                    position[2] + front[2]},
             up, view_matrix);
  glm_cross(front, world_up, right);  // Calcula o vetor 'right' inicial
}

void camera_cleanup() {
  // Limpa recursos da câmera (se necessário)
}

void camera_process_input(GLFWwindow* window, float deltaTime) {
  // float target_speed = 0.0f;

  // Determina a direção do movimento
  int moving = 0;
  vec3 direction = {0.0f, 0.0f, 0.0f};

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    glm_vec3_add(direction, front, direction);
    moving = 1;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    vec3 temp;
    glm_vec3_negate_to(front, temp);
    glm_vec3_add(direction, temp, direction);
    moving = 1;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    vec3 temp;
    glm_vec3_negate_to(right, temp);
    glm_vec3_add(direction, temp, direction);
    moving = 1;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    glm_vec3_add(direction, right, direction);
    moving = 1;
  }

  // Pulo
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !is_jumping) {
    vertical_speed = jump_speed;
    is_jumping = 1;
  }

  // Aplica gravidade
  vertical_speed += gravity * deltaTime;
  position[1] += vertical_speed * deltaTime;

  // Colisão com o chão (Y = 0)
  if (position[1] < 1.0f) {
    position[1] = 1.0f;
    vertical_speed = 0.0f;
    is_jumping = 0;
  }

  // Acelera ou desacelera
  if (moving) {
    current_speed += acceleration * deltaTime;
    if (current_speed > max_speed) current_speed = max_speed;
  } else {
    current_speed -= deceleration * deltaTime;
    if (current_speed < 0.0f) current_speed = 0.0f;
  }

  // Normaliza a direção e aplica o movimento
  if (moving) {
    glm_vec3_normalize(direction);
    vec3 delta;
    glm_vec3_scale(direction, current_speed * deltaTime, delta);
    glm_vec3_add(position, delta, position);
  }

  // Atualiza a matriz de visão
  vec3 target;
  glm_vec3_add(position, front, target);
  glm_lookat(position, target, up, view_matrix);
}

void camera_process_mouse(double xpos, double ypos) {
  if (first_mouse) {
    last_x = xpos;
    last_y = ypos;
    first_mouse = 0;
  }

  float xoffset = xpos - last_x;
  float yoffset =
      last_y - ypos;  // Invertido porque a coordenada Y cresce para baixo
  last_x = xpos;
  last_y = ypos;

  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  // Limita o pitch para evitar inversão da câmera
  if (pitch > 89.0f) pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;

  // Calcula o novo vetor front
  vec3 new_front;
  new_front[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
  new_front[1] = sin(glm_rad(pitch));
  new_front[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
  glm_normalize_to(new_front, front);

  // Atualiza os vetores right e up
  glm_cross(front, world_up, right);
  glm_normalize(right);
  glm_cross(right, front, up);
  glm_normalize(up);
}

float* camera_get_view_matrix() { return (float*)view_matrix; }

float* camera_get_projection_matrix() { return (float*)projection_matrix; }

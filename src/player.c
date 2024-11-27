#include "player.h"

#include <cglm/cglm.h>
#include <math.h>
#include <string.h>

#include "block.h"
#include "world.h"

#define RAYCAST_MAX_DISTANCE 6.0f  // Distância máxima para interação

// Estado do jogador
static vec3 position = {15.0f, 70.0f, 15.0f};
static vec3 front = {0.0f, 0.0f, -1.0f};
static vec3 up = {0.0f, 1.0f, 0.0f};
static vec3 right;
static vec3 world_up = {0.0f, 1.0f, 0.0f};

static mat4 view_matrix;

static float yaw = -90.0f;
static float pitch = 0.0f;
static float sensitivity = 0.1f;

static float vertical_speed = 0.0f;
static const float gravity = -9.81f;
static const float jump_speed = 5.0f;
static int is_jumping = 0;
static int is_sprinting = 0;

// Dimensões do jogador
static const float player_height = 2.0f;
static const float player_width = 0.6f;

void player_init() {
  glm_vec3_cross(front, world_up, right);
  glm_lookat(position,
             (vec3){position[0] + front[0], position[1] + front[1],
                    position[2] + front[2]},
             up, view_matrix);
}

void player_reset_position() {
  glm_vec3_copy((vec3){15.0f, 60.0f, 15.0f}, position);

  vertical_speed = 0.0f;
  is_jumping = 0;
}

static int check_collision(vec3 new_position);

void player_update(float deltaTime, GLFWwindow* window) {
  float current_speed = 0.0f;
  const float max_speed = 5.0f;

  // Movimento horizontal
  int moving = 0;
  vec3 direction = {0.0f, 0.0f, 0.0f};

  // Teclas especiais de movimento
  static int ctrl_pressed = 0;

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
  if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
    player_reset_position();
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
    if (!ctrl_pressed && moving &&
        (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
         glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
      is_sprinting = 1;
    }
    ctrl_pressed = 1;
  } else {
    ctrl_pressed = 0;
  }

  // Se o jogador parar de se mover para frente, desativa a corrida
  if (!(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
    is_sprinting = 0;
  }

  // Aplica velocidade
  if (moving) {
    if (is_sprinting) {
      current_speed =
          max_speed * 1.5f;  // Aumenta a velocidade em 50% ao correr
    } else {
      current_speed = max_speed;
    }
    direction[1] = 0.0f;  // Mantém o movimento no plano XZ
    glm_vec3_normalize(direction);
    vec3 delta;
    glm_vec3_scale(direction, current_speed * deltaTime, delta);

    // Movimenta no eixo X
    vec3 new_position_x = {position[0] + delta[0], position[1], position[2]};
    if (!check_collision(new_position_x)) {
      position[0] = new_position_x[0];
    } else {
      delta[0] = 0.0f;  // Impede movimento no eixo X
    }

    // Movimenta no eixo Z
    vec3 new_position_z = {position[0], position[1], position[2] + delta[2]};
    if (!check_collision(new_position_z)) {
      position[2] = new_position_z[2];
    } else {
      delta[2] = 0.0f;  // Impede movimento no eixo Z
    }
  }

  // Pulo
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !is_jumping) {
    vertical_speed = jump_speed;
    is_jumping = 1;
  }

  // Aplica gravidade
  vertical_speed += gravity * deltaTime;
  float new_y_position = position[1] + vertical_speed * deltaTime;

  // Verifica colisão vertical
  vec3 new_position_y = {position[0], new_y_position, position[2]};
  if (!check_collision(new_position_y)) {
    position[1] = new_y_position;
  } else {
    // Colisão vertical
    if (vertical_speed < 0.0f) {
      // Colisão com o chão
      is_jumping = 0;
    }
    vertical_speed = 0.0f;
  }

  // Ajusta posição final para evitar interseção residual
  vec3 adjusted_position = {position[0], position[1], position[2]};
  if (check_collision(adjusted_position)) {
    // Move o jogador para fora de blocos em caso de interseção
    while (check_collision(adjusted_position)) {
      adjusted_position[1] += 0.1f;  // Eleva gradualmente até não colidir
    }
    glm_vec3_copy(adjusted_position, position);
  }

  // Atualiza a matriz de visão
  vec3 target;
  glm_vec3_add(position, front, target);
  glm_lookat(position, target, up, view_matrix);
}

void player_process_mouse(double xpos, double ypos) {
  static float last_x = 400, last_y = 300;
  static int first_mouse = 1;

  if (first_mouse) {
    last_x = xpos;
    last_y = ypos;
    first_mouse = 0;
  }

  float xoffset = xpos - last_x;
  float yoffset = last_y - ypos;
  last_x = xpos;
  last_y = ypos;

  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f) pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;

  vec3 new_front;
  new_front[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
  new_front[1] = sin(glm_rad(pitch));
  new_front[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
  glm_normalize_to(new_front, front);

  // Atualiza os vetores right e up
  glm_vec3_cross(front, world_up, right);
  glm_normalize(right);
  glm_vec3_cross(right, front, up);
  glm_normalize(up);
}

void player_get_view_matrix(float* view) {
  memcpy(view, view_matrix, sizeof(mat4));
}

// Função para verificar colisão
static int check_collision(vec3 new_position) {
  float half_width = player_width / 2.0f;
  float player_bottom = new_position[1];
  float player_top = new_position[1] + player_height;

  // Determinar os limites do jogador
  float min_x = new_position[0] - half_width;
  float max_x = new_position[0] + half_width;
  float min_y = player_bottom;
  float max_y = player_top;
  float min_z = new_position[2] - half_width;
  float max_z = new_position[2] + half_width;

  // Iterar sobre todos os blocos dentro do volume potencial de colisão
  int block_min_x = (int)floor(min_x);
  int block_max_x = (int)floor(max_x);
  int block_min_y = (int)floor(min_y);
  int block_max_y = (int)floor(max_y);
  int block_min_z = (int)floor(min_z);
  int block_max_z = (int)floor(max_z);

  for (int x = block_min_x; x <= block_max_x; x++) {
    for (int y = block_min_y; y <= block_max_y; y++) {
      for (int z = block_min_z; z <= block_max_z; z++) {
        Block* block = world_get_block(x, y, z);
        if (block && block->type != BLOCK_AIR) {
          // Verificar se o volume do jogador intercepta o bloco
          float block_min_x = x;
          float block_max_x = x + 1.0f;
          float block_min_y = y;
          float block_max_y = y + 1.0f;
          float block_min_z = z;
          float block_max_z = z + 1.0f;

          if (min_x < block_max_x && max_x > block_min_x &&
              min_y < block_max_y && max_y > block_min_y &&
              min_z < block_max_z && max_z > block_min_z) {
            return 1;  // Colisão detectada
          }
        }
      }
    }
  }

  return 0;  // Sem colisão
}

void player_get_position(vec3 out_position) {
  glm_vec3_copy(position, out_position);
}

// Função para realizar o ray casting
static int raycast(vec3 origin, vec3 direction, vec3* out_block,
                   vec3* out_adjacent) {
  vec3 ray_origin;
  glm_vec3_copy(origin, ray_origin);

  for (float t = 0.0f; t < RAYCAST_MAX_DISTANCE; t += 0.1f) {
    vec3 pos;
    glm_vec3_scale(direction, t, pos);
    glm_vec3_add(ray_origin, pos, pos);

    int x = (int)floor(pos[0]);
    int y = (int)floor(pos[1]);
    int z = (int)floor(pos[2]);

    Block* block = world_get_block(x, y, z);
    if (block && block->type != BLOCK_AIR) {
      if (out_block) {
        glm_vec3_copy((vec3){x, y, z}, *out_block);
      }

      // Calcula o bloco adjacente (onde colocar um novo bloco)
      if (out_adjacent) {
        vec3 prev_pos;
        glm_vec3_scale(direction, t - 0.1f, prev_pos);
        glm_vec3_add(ray_origin, prev_pos, prev_pos);

        glm_vec3_copy((vec3){(int)floor(prev_pos[0]), (int)floor(prev_pos[1]),
                             (int)floor(prev_pos[2])},
                      *out_adjacent);
      }

      return 1;  // Bloco encontrado
    }
  }
  return 0;  // Nenhum bloco encontrado
}

void player_break_block() {
  vec3 origin;
  glm_vec3_copy(position, origin);
  origin[1] += player_height / 2.0f;  // Altura dos olhos

  vec3 block_pos;
  if (raycast(origin, front, &block_pos, NULL)) {
    world_set_block((int)block_pos[0], (int)block_pos[1], (int)block_pos[2],
                    BLOCK_AIR);
  }
}

void player_place_block() {
  vec3 origin;
  glm_vec3_copy(position, origin);
  origin[1] += player_height / 2.0f;  // Altura dos olhos

  vec3 block_pos, adjacent_pos;
  if (raycast(origin, front, &block_pos, &adjacent_pos)) {
    world_set_block((int)adjacent_pos[0], (int)adjacent_pos[1],
                    (int)adjacent_pos[2], BLOCK_STONE);
  }
}
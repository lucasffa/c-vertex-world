// Define GLFW_INCLUDE_NONE antes de incluir GLFW/glfw3.h
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "player.h"
#include "renderer.h"
#include "world.h"

// Callback para rastrear o movimento do mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  (void)window;
  player_process_mouse(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods) {
  (void)window;
  (void)mods;
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      player_break_block();
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      player_place_block();
    }
  }
}

// Callback para redimensionamento da janela
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
  renderer_reset();
}

// Callback para eventos de foco
void window_focus_callback(GLFWwindow* window, int focused) {
  (void)window;
  if (focused) {
    printf("Janela voltou a ter foco.\n");
    renderer_reset();
  } else {
    printf("Janela perdeu o foco.\n");
  }
}

int main() {
  // Inicializa o GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Falha ao inicializar o GLFW\n");
    return -1;
  }

  // Configura o contexto OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Cria a janela
  GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Viewer", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Falha ao criar a janela GLFW\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Configura o cursor para ser capturado
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Registra callbacks
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetWindowFocusCallback(window, window_focus_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  // Inicializa o GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Falha ao inicializar o GLEW\n");
    return -1;
  }

  // Configura o viewport
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  // Inicializa sistemas
  renderer_init();
  world_init();
  camera_init();
  player_init();

  // Variáveis para cálculo do tempo
  float lastFrame = 0.0f;

  // Loop principal
  while (!glfwWindowShouldClose(window)) {
    // Calcula o tempo decorrido
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Processa entrada do jogador
    player_update(deltaTime, window);

    // Renderiza a cena
    renderer_clear();
    renderer_draw_world();

    // Troca os buffers e processa eventos
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Limpa e finaliza
  camera_cleanup();
  world_cleanup();
  renderer_cleanup();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

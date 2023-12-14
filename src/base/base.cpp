#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
static struct {
  float x, y, z;
  float r, g, b;
} vertices[600];

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {
  GLFWwindow *window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program;
  GLint mvp_location, vpos_location, vcol_location;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(1000, 750, "base floor", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1);

  // NOTE: OpenGL error checks have been omitted for brevity
  int v = 0;
  int r = 0;
  float floor_width = 1000, floor_height = 750;

  for (float y = 0; y < floor_height; y += 75) {
    r = (int((y + 1) / 75)) % 2 == 0 ? 0 : 1;
    for (float x = 0; x < floor_width; x += 100) {
      float c = (r++) % 2 == 0 ? 0 : 1;
      vertices[v].x = x - 500;
      vertices[v].y = 0;
      vertices[v].z = y - 375;
      vertices[v].r = c;
      vertices[v].g = c;
      vertices[v].b = c;
      v++;

      vertices[v].x = x - 500;
      vertices[v].y = 0;
      vertices[v].z = (y + 75) - 375;
      vertices[v].r = c;
      vertices[v].g = c;
      vertices[v].b = c;
      v++;

      vertices[v].x = (x + 100) - 500;
      vertices[v].y = 0;
      vertices[v].z = (y + 75) - 375;
      vertices[v].r = c;
      vertices[v].g = c;
      vertices[v].b = c;
      v++;

      vertices[v].x = x - 500;
      vertices[v].y = 0;
      vertices[v].z = y - 375;
      vertices[v].r = c;
      vertices[v].g = c;
      vertices[v].b = c;
      v++;

      vertices[v].x = (x + 100) - 500;
      vertices[v].y = 0;
      vertices[v].z = (y + 75) - 375;
      vertices[v].r = c;
      vertices[v].g = c;
      vertices[v].b = c;
      v++;

      vertices[v].x = (x + 100) - 500;
      vertices[v].y = 0;
      vertices[v].z = y - 375;
      vertices[v].r = c;
      vertices[v].g = c;
      vertices[v].b = c;
      v++;
    }
  }

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  auto shader = DGL::GLShaderBuilder()
                    .vertex_shader_file("./baseshaders/vertex.vert")
                    .fragment_shader_file("./baseshaders/fragment.frag")
                    .buildShared();
  shader->link();

  mvp_location = shader->getUniformLocation("MVP");
  vpos_location = shader->getAttribLocation("vPos");
  vcol_location = shader->getAttribLocation("vCol");

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)0);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)(sizeof(float) * 3));

  glm::mat4 ortho = glm::ortho(-500.f, 500.f, -375.f, 375.f, -100.f, 200.f);
  glm::mat4 view =
      glm::lookAt(glm::vec3(0.f, 100.f, 0.f), glm::vec3(0.f, 0.f, 0.f),
                  glm::vec3(0.f, 0.f, -1.f));
  ;
  glm::mat4 mvp = ortho * view;

  while (!glfwWindowShouldClose(window)) {
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 600);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}

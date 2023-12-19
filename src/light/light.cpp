#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "shader.hpp"
#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
static struct {
  float x, y, z;
  float r, g, b;
} vertices[603];

static struct {
  float x, y, z;
  float r, g, b;
} cube[8] = {
  {-50.0f,30.01f,-50.0f, 1.0f,0.0f,0.0f},
  {-50.0f,30.01f, 50.0f, 1.0f,0.0f,0.0f},
  {50.0f,30.01f,50.0f, 1.0f,0.0f,0.0f},
  {50.0f,30.01f,-50.0f, 1.0f,0.0f,0.0f},
  {-50.0f,130.0f,-50.0f, 1.0f,0.0f,0.0f},
  {-50.0f,130.0f, 50.0f, 1.0f,0.0f,0.0f},
  {50.0f, 130.0f,50.0f, 1.0f,0.0f,0.0f},
  {50.0f, 130.0f,-50.0f, 1.0f,0.0f,0.0f},
};

static int cube_index[36] = {
  2,1,0,
  3,2,0,
  5,6,4,
  6,7,4,
  5,1,2,
  2,6,5,
  2,3,6,
  7,6,3,
  3,0,7,
  0,4,7,
  4,0,1,
  5,4,1
};
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
  // floor
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
  // center
  {
      vertices[v].x = -25;
      vertices[v].y = 0;
      vertices[v].z = 0;
      vertices[v].r = 0;
      vertices[v].g = 1;
      vertices[v].b = 0;
      v++;

      vertices[v].x = 25;
      vertices[v].y = 0;
      vertices[v].z = 0;
      vertices[v].r = 0;
      vertices[v].g = 1;
      vertices[v].b = 0;
      v++;

      vertices[v].x = 0;
      vertices[v].y = 0;
      vertices[v].z = -50;
      vertices[v].r = 0;
      vertices[v].g = 1;
      vertices[v].b = 0;
      v++;
  }

  auto shader = DGL::GLShaderBuilder()
                    .vertex_shader_file("./lightshaders/vertex.vert")
                    .fragment_shader_file("./lightshaders/fragment.frag")
                    .buildShared();
  shader->link();
  shader->use();
  mvp_location = shader->getUniformLocation("MVP");
  vpos_location = shader->getAttribLocation("vPos");
  vcol_location = shader->getAttribLocation("vCol");

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)0);
  glDisableVertexAttribArray(vpos_location);

  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)(sizeof(float) * 3));
  glDisableVertexAttribArray(vcol_location);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glUseProgram(0);

  // cube
  auto cubeshader = DGL::GLShaderBuilder()
                    .vertex_shader_file("./lightshaders/cube.vert")
                    .fragment_shader_file("./lightshaders/cube.frag")
                    .buildShared();
  cubeshader->link();
  cubeshader->use();

  GLint cube_mvp_location = cubeshader->getUniformLocation("MVP");
  GLint cube_vpos_location = cubeshader->getAttribLocation("vPos");
  GLint cube_vcol_location = cubeshader->getAttribLocation("vCol");

  GLuint cube_vertex_buffer,cube_element_buffer;
  glGenBuffers(1, &cube_vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

  glEnableVertexAttribArray(cube_vpos_location);
  glVertexAttribPointer(cube_vpos_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(cube[0]), (void *)0);
  glDisableVertexAttribArray(cube_vpos_location);

  glEnableVertexAttribArray(cube_vcol_location);
  glVertexAttribPointer(cube_vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(cube[0]), (void *)(sizeof(float) * 3));
  glDisableVertexAttribArray(cube_vcol_location);
  glBindBuffer(GL_ARRAY_BUFFER,0);

  glGenBuffers(1, &cube_element_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_element_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_index), cube_index, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glUseProgram(0);

  constexpr float pi = 3.1415926f;

  DGL::Camera camer;
  while (!glfwWindowShouldClose(window)) {
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    int t = (int)(2 * glfwGetTime()) % 360;
    float degree = t * 2 * pi/ 360.0f;
    float r = 800.0f;
    float cx = r * cos(degree * 1.0f);
    float cz = r * sin(degree * 1.0f);
    glm::mat4 proj = glm::perspective(60.0f * 2 * pi/ 360.0f, ratio,0.0f, 400.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(cx, 300.f, cz), glm::vec3(0.f, 0.f, 0.f),glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 mvp = proj * view;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)(sizeof(float) * 3));
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 603);
    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glUseProgram(0);

    cubeshader->use();
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_buffer);
    glUniformMatrix4fv(cube_mvp_location, 1, GL_FALSE, &mvp[0][0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_element_buffer);
    glEnableVertexAttribArray(cube_vpos_location);
    glVertexAttribPointer(cube_vpos_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(cube[0]), (void *)0);
    glEnableVertexAttribArray(cube_vcol_location);
    glVertexAttribPointer(cube_vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(cube[0]), (void *)(sizeof(float) * 3));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(cube_vpos_location);
    glDisableVertexAttribArray(cube_vcol_location);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glUseProgram(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "shader.hpp"
#include "Camera.hpp"
#include "stl_reader.hpp"

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
} vertices[603];

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {
  std::vector<float> coords, normals;
  std::vector<unsigned int> tris, solids;
  stl_reader::ReadStlFile ("./shadingshaders/teapot.stl", coords, normals, tris, solids);
  const size_t numTris = tris.size() / 3;

  GLFWwindow *window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program;
  GLint mvp_location, vpos_location, vcol_location;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
                    .vertex_shader_file("./shadingshaders/vertex.vert")
                    .fragment_shader_file("./shadingshaders/fragment.frag")
                    .buildShared();
  shader->link();
  shader->use();
  mvp_location = shader->getUniformLocation("MVP");
  vpos_location = 1;
  vcol_location = 0;

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)0);

  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)(sizeof(float) * 3));
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glUseProgram(0);
  glBindVertexArray(0);
  // teapot
  GLuint TeaPotVAO;
  glGenVertexArrays(1, &TeaPotVAO);
  glBindVertexArray(TeaPotVAO);
  auto teapotshader = DGL::GLShaderBuilder()
                    .vertex_shader_file("./shadingshaders/teapot_flat.vert")
                    .fragment_shader_file("./shadingshaders/teapot_flat.frag")
                    .buildShared();
  teapotshader->link();
  teapotshader->use();

  GLint teapot_model_location = teapotshader->getUniformLocation("model");
  GLint teapot_view_location = teapotshader->getUniformLocation("view");
  GLint teapot_projection_location = teapotshader->getUniformLocation("projection");

  GLint teapot_vcol_location = 0;
  GLint teapot_vpos_location = 1;
  GLint teapot_vnorm_location = 2;

  GLuint teapot_vertex_buffer,teapot_element_buffer;
  glGenBuffers(1, &teapot_vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, teapot_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(teapot), teapot, GL_STATIC_DRAW);

  glVertexAttribPointer(teapot_vpos_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(teapot[0]), (void *)0);

  glVertexAttribPointer(teapot_vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(teapot[0]), (void *)(sizeof(float) * 3));

  glVertexAttribPointer(teapot_vnorm_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(teapot[0]), (void *)(sizeof(float) * 6));

  glBindBuffer(GL_ARRAY_BUFFER,0);

  glGenBuffers(1, &teapot_element_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, teapot_element_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapot_index), teapot_index, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glUseProgram(0);
  glBindVertexArray(0);

  constexpr float pi = 3.1415926f;
  glm::vec3 lightPos(100.0f, 100.0f, 0.0f);
  DGL::Camera camera;
  while (!glfwWindowShouldClose(window)) {
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    int t = (int)(2 * glfwGetTime()) % 360;
    float degree = 0.0f * t * 2 * pi/ 360.0f;
    float r = 800.0f;
    float cx = r * cos(degree * 1.0f);
    float cz = r * sin(degree * 1.0f);
    glm::mat4 proj = glm::perspective(60.0f * 2 * pi/ 360.0f, ratio,0.0f, 400.0f);
    camera.build(glm::vec3(cx, 300.f, cz), glm::vec3(0.f, 0.f, 0.f),glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 view = camera.view_matrix();
    glm::mat4 mvp = proj * view;
    glm::mat4 model = glm::mat4(1.0f);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
    glEnableVertexAttribArray(vpos_location);
    glEnableVertexAttribArray(vcol_location);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 603);
    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glUseProgram(0);
    glBindVertexArray(0);

    teapotshader->use();
    // light properties
    teapotshader->setVec3("light.position", lightPos);
    teapotshader->setVec3("viewPos", camera.get_eye());
    glm::vec3 lightColor;
    lightColor.x = static_cast<float>(sin(/*glfwGetTime()*/ 1.0f * 2.0));
    lightColor.y = static_cast<float>(sin(/*glfwGetTime()*/ 1.0f * 0.7));
    lightColor.z = static_cast<float>(sin(/*glfwGetTime()*/ 1.0f * 1.3));
    glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
    teapotshader->setVec3("light.ambient", ambientColor);
    teapotshader->setVec3("light.diffuse", diffuseColor);
    teapotshader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // material properties
    teapotshader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    teapotshader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    teapotshader->setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
    teapotshader->setFloat("material.shininess", 32.0f);

    glBindVertexArray(TeaPotVAO);
    glBindBuffer(GL_ARRAY_BUFFER, teapot_vertex_buffer);
    glUniformMatrix4fv(teapot_model_location, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(teapot_view_location, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(teapot_projection_location, 1, GL_FALSE, &proj[0][0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, teapot_element_buffer);
    glEnableVertexAttribArray(teapot_vpos_location);
    glEnableVertexAttribArray(teapot_vcol_location);
    glEnableVertexAttribArray(teapot_vnorm_location);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(teapot_vpos_location);
    glDisableVertexAttribArray(teapot_vcol_location);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glUseProgram(0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1,&VAO);
  glDeleteVertexArrays(1,&TeaPotVAO);
  glDeleteBuffers(1,&vertex_buffer);
  glDeleteBuffers(1,&teapot_vertex_buffer);

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

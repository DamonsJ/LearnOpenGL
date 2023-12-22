#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "ArcballCamera.hpp"
#include "INIReader.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include "linmath.h"
#include "shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  DGL::ArcballCamera *camera =
      static_cast<DGL::ArcballCamera *>(glfwGetWindowUserPointer(window));
  if (camera == nullptr)
    return;

  float step = 0.05f;
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    camera->pan(-step,0);
  }
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    camera->pan(step, 0);
  }
  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    camera->pan(0, step);
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    camera->pan(0, -step);
  }
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {

  DGL::ArcballCamera *camera =
      static_cast<DGL::ArcballCamera *>(glfwGetWindowUserPointer(window));
  if (camera == nullptr)
    return;

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double startXPos, startYPos; // screen coordinates when mouse clicks.
    glfwGetCursorPos(window, &startXPos, &startYPos);
    // convert to NDC, then assign to startPos.
    camera->rotate_start(startXPos, startYPos);
  } else {
    camera->rotate_end();
  }
}

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos) {
  DGL::ArcballCamera *camera =
      static_cast<DGL::ArcballCamera *>(glfwGetWindowUserPointer(window));
  if (camera == nullptr)
    return;

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    // camera->orbit(x_offset, y_offset);
    camera->rotate(x_pos, y_pos);
  }
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
  DGL::ArcballCamera *camera =
      static_cast<DGL::ArcballCamera *>(glfwGetWindowUserPointer(window));
  if (camera == nullptr)
    return;
  camera->zoom(y_offset);
}

void setMaterial(const INIReader &config, const DGL::GLShader &shader) {
  // Set fragment shader uniforms
  // Material lighting properties
  glm::vec3 material_ambient =
      glm::vec3(config.GetFloat("Material", "AmbientColorR", 1.0),
                config.GetFloat("Material", "AmbientColorG", 1.0),
                config.GetFloat("Material", "AmbientColorB", 1.0));
  shader.setVec3("material.ambient", material_ambient);
  glm::vec3 material_diffuse =
      glm::vec3(config.GetFloat("Material", "DiffuseColorR", 1.0),
                config.GetFloat("Material", "DiffuseColorG", 1.0),
                config.GetFloat("Material", "DiffuseColorB", 1.0));
  shader.setVec3("material.diffuse", material_diffuse);
  glm::vec3 material_specular =
      glm::vec3(config.GetFloat("Material", "SpecularColorR", 1.0),
                config.GetFloat("Material", "SpecularColorG", 1.0),
                config.GetFloat("Material", "SpecularColorB", 1.0));
  shader.setVec3("material.specular", material_specular);
  float material_shininess = config.GetFloat("Material", "Shininess", 32.0);
  shader.setFloat("material.shininess", material_shininess);

  // Light lighting properties
  glm::vec3 light_ambient =
      glm::vec3(config.GetFloat("Light", "AmbientColorR", 1.0),
                config.GetFloat("Light", "AmbientColorG", 1.0),
                config.GetFloat("Light", "AmbientColorB", 1.0));
  shader.setVec3("light.ambient", light_ambient);
  glm::vec3 light_diffuse =
      glm::vec3(config.GetFloat("Light", "DiffuseColorR", 1.0),
                config.GetFloat("Light", "DiffuseColorG", 1.0),
                config.GetFloat("Light", "DiffuseColorB", 1.0));
  shader.setVec3("light.diffuse", light_diffuse);
  glm::vec3 light_specular =
      glm::vec3(config.GetFloat("Light", "SpecularColorR", 1.0),
                config.GetFloat("Light", "SpecularColorG", 1.0),
                config.GetFloat("Light", "SpecularColorB", 1.0));
  shader.setVec3("light.specular", light_specular);
}

int main(void) {

  glfwSetErrorCallback(error_callback);
  INIReader config("./shadingshaders/config.ini");
  int e = config.ParseError();
  if (e != 0) {
    printf(" load config error : %d \n", e);
    exit(EXIT_FAILURE);
  }
  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // Open fullscreen or windowed context based on config.ini
  GLFWmonitor *monitor;
  int window_width, window_height = 0;
  if (config.GetBoolean("Screen", "Fullscreen", true)) {
    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window_width = mode->width;
    window_height = mode->height;
  } else {
    monitor = NULL;
    window_width = config.GetInteger("Screen", "ScreenWidth", 800);
    window_height = config.GetInteger("Screen", "ScreenHeight", 640);
  }

  GLFWwindow *window =
      glfwCreateWindow(window_width, window_height, "shading", monitor, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);

  // Set input callbacks
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetKeyCallback(window, key_callback);

  glfwSwapInterval(1);

  glEnable(GL_DEPTH_TEST);  // Depth testing
  glEnable(GL_CULL_FACE);   // Rear face culling
  glEnable(GL_MULTISAMPLE); // MSAA

  if (config.GetBoolean("Graphics", "Wireframe", false)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  auto shader = DGL::GLShaderBuilder()
                    .vertex_shader_file("./shadingshaders/shading.vert")
                    .fragment_shader_file("./shadingshaders/shading.frag")
                    .buildShared();
  shader->use();

  std::string model_path = config.Get("Model", "Path", "");
  printf(" loading model : %s\n", model_path.c_str());
  DGL::Model showmodel(model_path);
  float tx = 0.0f, ty = 0.0f, tz = 0.0f;
  showmodel.extend(tx, ty, tz);
  printf(" extend x : %.3f y : %.3f z : %.3f \n", tx, ty, tz);
  float max_length = std::max(tx, std::max(ty, tz));
  tx = 0.0f, ty = 0.0f, tz = 0.0f;
  showmodel.center(tx, ty, tz);
  printf(" center x : %.3f y : %.3f z : %.3f \n", tx, ty, tz);
  float xmax = tx + 0.5f * max_length, ymax = ty + 0.5f * max_length, zmax = tz + 0.5f * zmax;
  // Build model matrix
  glm::mat4 model = glm::mat4(1.0f);
  glm::vec3 model_translate_vec = glm::vec3(-tx, -ty, -tz);
  glm::vec3 model_scale = glm::vec3(config.GetFloat("Model", "ScaleXYZ", 1.0));
  float model_rotate_angle =
      glm::radians(config.GetFloat("Model", "RotateAngle", 0.0));
  glm::vec3 model_rotate_axis =
      glm::vec3(config.GetFloat("Model", "RotateAxisX", 0.0),
                config.GetFloat("Model", "RotateAxisY", 0.0),
                config.GetFloat("Model", "RotateAxisZ", 0.0));
  model = glm::translate(model, model_translate_vec);
  model = glm::scale(model, model_scale);
  model = glm::rotate(model, model_rotate_angle, model_rotate_axis);

  shader->setMat4("model", model);

  setMaterial(config, *shader);

  // Background color
  glm::vec3 bg_color =
      glm::vec3(config.GetFloat("Graphics", "BackgroundColorR", 0.0),
                config.GetFloat("Graphics", "BackgroundColorG", 0.0),
                config.GetFloat("Graphics", "BackgroundColorB", 0.0));

  // Camera
  DGL::ArcballCamera camera(glm::vec3(0.0f, 0.0f,max_length * 2.0f ),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
  camera.setScreenDimensions(window_width, window_height);
  camera.updateNearFar(0.1f, max_length * 4.0f);
  // Set up input sensitivities
  float mouse_sensitivity = config.GetFloat("Input", "MouseSensitivity", 0.3);
  float zoom_sensitivity = config.GetFloat("Input", "ZoomSensitivity", 0.5);
  float fov_sensitivity = config.GetFloat("Input", "FovSensitivity", 0.4);
  camera.setSensitivities(mouse_sensitivity, zoom_sensitivity, fov_sensitivity);
  glfwSetWindowUserPointer(window, &camera);

  float deltaTime, lastFrame = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    // Delta time calculations
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glm::vec3 lightv = glm::mat3(model) * glm::vec3(xmax,ymax,zmax);
    glm::vec3 lightv =  camera.getPosition();
    shader->setMat4("view", camera.getViewMatrix());
    shader->setMat4("projection", camera.getProjMatrix());
    shader->setVec3("light.position", lightv);
    shader->setVec3("view_pos", camera.getPosition());
    shader->use();

    showmodel.draw();

    glfwSwapBuffers(window);
    glfwPollEvents();

    //process_keypresses(window, deltaTime);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

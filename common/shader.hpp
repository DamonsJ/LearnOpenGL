
#ifndef SHADER_HPP
#define SHADER_HPP

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

/**
 * @brief
 *
 */

namespace DGL {
class GLShader {
public:
  GLShader() {
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    program = glCreateProgram();
  }

public:
  void check_shader_error(GLuint shader);
  void build_vertex_shader(const char *vertex_shader_content) ;
  void build_fragment_shader(const char *fragment_shader_content);
  void build_vertex_shader_file(const char *vertex_shader_file) ;
  void build_fragment_shader_file(const char *fragment_shader_file) ;

  void link() {
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
  }

  void use() { glUseProgram(program); }

  GLint getUniformLocation(const char *uniform) const {
    return glGetUniformLocation(program, uniform);
  }

  GLint getAttribLocation(const char *uniform) const {
    return glGetAttribLocation(program, uniform);
  }

private:
  GLuint vertex_shader;
  GLuint fragment_shader;
  GLuint program;
};

class GLShaderBuilder {
public:
  GLShaderBuilder(){}

public:
  GLShaderBuilder &vertex_shader(const char *vertex_shader) {
    shader_vertex = vertex_shader;
    is_vertex_file = false;
    return *this;
  }
  GLShaderBuilder &fragment_shader(const char *fragment_shader) {
    shader_fragment = fragment_shader;
    is_fragment_file = false;
    return *this;
  }

  GLShaderBuilder &vertex_shader_file(const char *vertex_shader) {
    shader_vertex = vertex_shader;
    is_vertex_file = true;
    return *this;
  }
  GLShaderBuilder &fragment_shader_file(const char *fragment_shader) {
    shader_fragment = fragment_shader;
    is_fragment_file = true;
    return *this;
  }

  GLShader build() const {
    GLShader shader;
    if (is_vertex_file) {
      shader.build_vertex_shader_file(shader_vertex);
    } else {
      shader.build_vertex_shader(shader_vertex);
    }

    if (is_fragment_file) {
      shader.build_fragment_shader_file(shader_fragment);
    } else {
      shader.build_fragment_shader(shader_fragment);
    }
    return shader;
  }
  std::shared_ptr<GLShader> buildShared() const {
    auto shader = std::make_shared<GLShader>();
    if (is_vertex_file) {
      shader->build_vertex_shader_file(shader_vertex);
    } else {
      shader->build_vertex_shader(shader_vertex);
    }

    if (is_fragment_file) {
      shader->build_fragment_shader_file(shader_fragment);
    } else {
      shader->build_fragment_shader(shader_fragment);
    }
    return shader;
  }

private:
  bool is_vertex_file = false;
  bool is_fragment_file = false;
  const char *shader_vertex;
  const char *shader_fragment;
};
} // namespace DGL
#endif
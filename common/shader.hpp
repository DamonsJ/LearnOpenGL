
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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
  ~GLShader(){
    glDeleteProgram(program);
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

  // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(program, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(program, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(program, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
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
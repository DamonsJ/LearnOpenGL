#include "shader.hpp"
#include <assert.h>
namespace DGL {

#define MAX_SHADER_SZ 100000

void GLShader::check_shader_error(GLuint shader) {
  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
    printf(" \n fatal: \n      shader compile error: %s\n", errorLog.data());
    glDeleteShader(shader);
  }
}
void GLShader::build_vertex_shader(const char *vertex_shader_content) {
  glShaderSource(vertex_shader, 1, &vertex_shader_content, NULL);
  glCompileShader(vertex_shader);
  check_shader_error(vertex_shader);
}

void GLShader::build_fragment_shader(const char *fragment_shader_content) {
  glShaderSource(fragment_shader, 1, &fragment_shader_content, NULL);
  glCompileShader(fragment_shader);
  check_shader_error(fragment_shader);
}

void GLShader::build_vertex_shader_file(const char *vertex_shader_file) {
  char vs_shader_str[MAX_SHADER_SZ];
  FILE *fp = fopen(vertex_shader_file, "r");
  if (!fp) {
    printf("ERROR: could not open vertex shader file `%s`\n",
           vertex_shader_file);
  }
  std::size_t count = fread(vs_shader_str, 1, MAX_SHADER_SZ - 1, fp);
  assert(count < MAX_SHADER_SZ - 1); // file was too long
  vs_shader_str[count] = '\0';
  fclose(fp);

  build_vertex_shader(vs_shader_str);
}
void GLShader::build_fragment_shader_file(const char *fragment_shader_file) {
  char fs_shader_str[MAX_SHADER_SZ];
  FILE *fp = fopen(fragment_shader_file, "r");
  if (!fp) {
    printf("ERROR: could not open fragment shader file `%s`\n",fragment_shader_file);
  }
  size_t count = fread(fs_shader_str, 1, MAX_SHADER_SZ - 1, fp);
  assert(count < MAX_SHADER_SZ - 1); // file was too long
  fs_shader_str[count] = '\0';
  fclose(fp);

  build_fragment_shader(fs_shader_str);
}

} // namespace DGL
#include "shader.hpp"

namespace DGL {

void GLShader::build_vertex_shader(const char *vertex_shader_content) {
  glShaderSource(vertex_shader, 1, &vertex_shader_content, NULL);
  glCompileShader(vertex_shader);
}

void GLShader::build_fragment_shader(const char *fragment_shader_content) {
  glShaderSource(fragment_shader, 1, &fragment_shader_content, NULL);
  glCompileShader(fragment_shader);
}

void GLShader::build_vertex_shader_file(const char *vertex_shader_file) {
  std::ifstream t(vertex_shader_file);
  std::stringstream buffer;
  buffer << t.rdbuf();
  std::string content = buffer.str();
  build_vertex_shader(content.c_str());
  t.close();
}
void GLShader::build_fragment_shader_file(const char *fragment_shader_file) {
  std::ifstream t(fragment_shader_file);
  std::stringstream buffer;
  buffer << t.rdbuf();
  std::string content = buffer.str();
  build_fragment_shader(content.c_str());
  t.close();
}

} // namespace DGL
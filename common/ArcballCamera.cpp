#include "ArcballCamera.hpp"

namespace DGL {

ArcballCamera::ArcballCamera(const glm::vec3 &eye, const glm::vec3 &center,
                             const glm::vec3 &up) {
  m_eye = eye;
  m_center = center;
  m_up = up;
  m_last_eye = m_eye;
}

void ArcballCamera::setScreenDimensions(unsigned int width,
                                        unsigned int height) {
  m_screen_width = width;
  m_screen_height = height;
}

void ArcballCamera::setSensitivities(float mouse_sensitivity,
                                     float zoom_sensitivity,
                                     float fov_sensitivity) {
  m_mouse_sensitivity = mouse_sensitivity;
  m_zoom_sensitivity = zoom_sensitivity;
  m_fov_sensitivity = fov_sensitivity;
}

glm::mat4 ArcballCamera::getViewMatrix(void) {
  return glm::lookAt(m_eye, m_center, m_up);
}

glm::mat4 ArcballCamera::getProjMatrix(void) {
  return glm::perspective(glm::radians(m_fov),
                          (float)m_screen_width / (float)m_screen_height, near,
                          far);
}
void ArcballCamera::updateNearFar(float n, float f) {
  near = n;
  far = f;
}

void ArcballCamera::zoom(double y_offset) {
  // glm::vec3 viewdir = m_center - m_eye;
  glm::vec3 viewdir = m_eye - m_center;
  float l = glm::length(viewdir);
  viewdir = glm::normalize(viewdir);
  m_eye = m_center + viewdir * (l - float(y_offset) * m_zoom_sensitivity);
  m_last_eye = m_eye;
}
void ArcballCamera::pan(float x_offset, float y_offset) {
  glm::vec3 viewdir = m_center - m_eye;
  float l = glm::length(viewdir);
  viewdir = glm::normalize(viewdir);
  glm::vec3 r = glm::normalize(glm::cross(viewdir, m_up));
  m_center = m_center - r * x_offset * l - m_up * y_offset * l;
}
void ArcballCamera::rotate_end()
{
  m_last_eye = m_eye;
}
void ArcballCamera::rotate(float xpos, float ypos) {
  if ( int(xpos) == int(posx) && int(ypos) == int(posy)) return;

  xpos = 1.0f * xpos / m_screen_width * 2 - 1.0;
  ypos = -(1.0f * ypos / m_screen_width * 2 - 1.0);
  float zpos = 0.0f;
  float len = xpos * xpos + ypos * ypos;
  if (len <= 1.0f)
    zpos = sqrt(1.0 - len); // Pythagoras

  glm::vec3 s = glm::normalize(glm::vec3(xpos_start, ypos_start, zpos_start));
  glm::vec3 t = glm::normalize(glm::vec3(xpos, ypos, zpos));
  glm::vec3 axis = glm::normalize(glm::cross(t, s));
  float theta = acos(std::min(glm::dot(s, t), 1.0f));
  axis = glm::mat3(glm::inverse(getViewMatrix())) * axis;
  glm::vec3 viewdir = m_last_eye - m_center;
  glm::mat4 rot_mat = glm::rotate(glm::mat4(1.0f), theta, axis);
  m_eye = glm::mat3(rot_mat) * viewdir + m_center;

  viewdir = glm::normalize(viewdir) * glm::vec3(-1.0);
  glm::vec3 right_vec = glm::normalize(glm::cross(viewdir, m_up));
  m_up = glm::normalize(glm::cross(right_vec, viewdir));
}

void ArcballCamera::rotate_start(float xstart, float ystart) {
  posx = xstart;
  posy = ystart;

  xpos_start = 1.0f * posx / m_screen_width * 2 - 1.0;
  ypos_start = -(1.0f * posy / m_screen_width * 2 - 1.0);

  zpos_start = 0.0f;
  float len = xpos_start * xpos_start + ypos_start * ypos_start;
  if (len <= 1.0f)
    zpos_start = sqrt(1.0 - len); // Pythagoras
}

} // namespace DGL

#ifndef __ARCBALLCAMERA_HPP__
#define __ARCBALLCAMERA_HPP__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

/**
 * @brief ArcballCamera
 *
 */

namespace DGL {

class ArcballCamera {
public:
  ArcballCamera() {}
  ArcballCamera(const glm::vec3 &eye, const glm::vec3 &center,
                const glm::vec3 &up);
  // rule of five
  ~ArcballCamera() = default; // destructor (virtual if ArcballCamera is meant
                              // to be a base class)
  ArcballCamera(const ArcballCamera &) = default;            // copy constructor
  ArcballCamera &operator=(const ArcballCamera &) = default; // copy assignment
  ArcballCamera(ArcballCamera &&) noexcept = default;        // move constructor
  ArcballCamera &
  operator=(ArcballCamera &&) noexcept = default; // move assignment

public:
  void rotate_start(float xstart, float ystart);
  void rotate(float xpos, float ypos);

  void zoom(double y_offset);
  void pan(float x_offset, float y_offset);

  void setScreenDimensions(unsigned int width, unsigned int height);
  void setSensitivities(float mouse_sensitivity, float zoom_sensitivity,
                        float fov_sensitivity);

  void updateNearFar(float n, float f);
  glm::mat4 getViewMatrix(void);
  glm::mat4 getProjMatrix(void);
  glm::vec3 getPosition(void) { return m_eye; }

private:
  // Sensitivities
  float m_mouse_sensitivity = 0.3;
  float m_zoom_sensitivity = 0.5;
  float m_fov_sensitivity = 0.4;
  // Screen
  unsigned int m_screen_width = 800;
  unsigned int m_screen_height = 640;
  //
  float m_fov = 45.0f;
  float near = 0.1f;
  float far = 100.0f;
  //
  glm::vec3 m_eye;
  glm::vec3 m_center;
  glm::vec3 m_up;

  float xpos_start;
  float ypos_start;
  float zpos_start;

  float posx;
  float posy;

  glm::vec3 start_pos;
  void update();
};

} // namespace DGL

#endif

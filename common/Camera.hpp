#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
* @brief Camera
*
*/

namespace DGL {

class Camera
{
 private:
    // Camera view state attributes
    float m_fov = 45.0f;
    float m_distance = 10.0;
    float m_position_xangle = 90.0;
    float m_position_yangle = 30.0;
    glm::vec3 m_position_coords = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 m_up_vec = glm::vec3(0.0, 1.0, 0.0);
    const glm::vec3 m_WORLD_UP_VEC = glm::vec3(0.0, 1.0, 0.0);
    // Sensitivities
    float m_mouse_sensitivity = 0.3;
    float m_zoom_sensitivity = 0.5;
    float m_fov_sensitivity = 0.4;
    // Screen
    unsigned int m_screen_width = 800;
    unsigned int m_screen_height = 640;
    //
    float near = 0.1f;
    float far = 100.0f;
    void updateCameraVectors(void);

public:
    Camera(void);
    void setCameraDistance(float d);
    void orbit(float x_offset, float y_offset);
    void zoom(double y_offset);
    void widenFov();
    void narrowFov();
    void setScreenDimensions(unsigned int width, unsigned int height);
    void setSensitivities(float mouse_sensitivity, float zoom_sensitivity, float fov_sensitivity);

    void updateNearFar(float n, float f);
    glm::mat4 getViewMatrix(void);
    glm::mat4 getProjMatrix(void);
    const glm::vec3& getPosition(void) const{ return m_position_coords; }
};

}
#endif

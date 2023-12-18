#include "Camera.hpp"


namespace DGL {

     void Camera::eye(float x, float y, float z)
     {
        veye = glm::vec3(x, y, z);
     }
     void Camera::eye(const glm::vec3 &e)
     {
        veye = e;
     }
     void Camera::center(float x, float y, float z)
     {
        vcenter = glm::vec3(x, y, z);
     }
     void Camera::center(const glm::vec3 &c)
     {
        vcenter = c;
     }
     void Camera::up(float x, float y, float z)
     {
        vup = glm::vec3(x, y, z);
     }
     void Camera::up(const glm::vec3 &u)
     {
        vup = u;
     }
      void Camera::build(const glm::vec3 &e,const glm::vec3 &c,const glm::vec3 &u)
      {
         vcenter = c;
         vup = u;
         veye = e;
      }
    glm::mat4 Camera::view_matrix()
    {
        return glm::lookAt(veye, vcenter,vup);
    }

};
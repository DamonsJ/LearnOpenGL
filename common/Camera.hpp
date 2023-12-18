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
public:
     Camera(){}
     Camera(const glm::vec3 &e,const glm::vec3 &c,const glm::vec3 &u):veye(e),vcenter(c),vup(u){}
public:
     void eye(float x, float y, float z);
     void eye(const glm::vec3 &e);
     void center(float x, float y, float z);
     void center(const glm::vec3 &c);
     void up(float x, float y, float z);
     void up(const glm::vec3 &u);

     void build(const glm::vec3 &e,const glm::vec3 &c,const glm::vec3 &u);
     glm::mat4 view_matrix();

     const glm::vec3& get_eye() const { return veye;}
     const glm::vec3& get_center() const { return vcenter;}
     const glm::vec3& get_up() const { return vup;}

private:
     glm::vec3 vcenter;
     glm::vec3 veye;
     glm::vec3 vup;

};

}
#endif

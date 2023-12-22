#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <iostream>
#include <vector>

/**
 * @brief Model
 *
 */

class aiNode;
class aiScene;
class aiMesh;

namespace DGL {
class Mesh;
class Model {
public:
  Model(const std::string &model_name);
  // rule of five
  ~Model(); // destructor (virtual if Model is meant to be a base class)
  Model(const Model &) = default;                // copy constructor
  Model &operator=(const Model &) = default;     // copy assignment
  Model(Model &&) noexcept = default;            // move constructor
  Model &operator=(Model &&) noexcept = default; // move assignment

public:
  void draw();
  // center of this model
  void center(float &cx, float &cy, float &cz) {
    cx = (xmin + xmax) * 0.5f;
    cy = (ymin + ymax) * 0.5f;
    cz = (zmin + zmax) * 0.5f;
  }
  // dimension length of this mode, box length width height
  void extend(float &cx, float &cy, float &cz) {
    cx = xmax - xmin;
    cy = ymax - ymin;
    cz = zmax - zmin;
  }

private:
  void load(const std::string &model_name);
  // Recursively process each Node by calling processMesh on each node's
  // meshes and adding them to m_meshes vector
  void processNode(aiNode *node, const aiScene *scene);
  // Create mesh object with vertex and index daa
  void processMesh(aiMesh *mesh, const aiScene *scene);

private:
  float xmin, xmax, ymin, ymax, zmin, zmax;
  std::vector<Mesh> m_meshes;
};

} // namespace DGL

#endif

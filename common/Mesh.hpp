#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <iostream>
#include <limits>
#include <vector>

/**
 * @brief Mesh
 *
 */

namespace DGL {

template <class T> class Epsilon {};

template <> class Epsilon<float> {
public:
  static constexpr float EPSILON = 1e-5f;
};

template <> class Epsilon<double> {
public:
  static constexpr double EPSILON = 1e-8f;
};

template <class T> static bool float_equal(T a, T b) {
  if (a == b)
    return true; // infinity
  auto diff = std::abs(a - b);
  auto norm =
      std::min((std::abs(a) + std::abs(b)), std::numeric_limits<T>::max());
  return diff <
         std::max(std::numeric_limits<T>::min(), Epsilon<T>::EPSILON * norm);
}

class MeshVertex {
public:
  MeshVertex() = default;
  MeshVertex(unsigned int _id, float _x, float _y, float _z, float _nx,
             float _ny, float _nz) {
    id = _id;
    x = _x;
    y = _y;
    z = _z;
    nx = _nx;
    ny = _ny;
    nz = _nz;
  }

public:
  bool operator==(const MeshVertex &v) const {
    bool ex = float_equal(x, v.x);
    bool ey = float_equal(y, v.y);
    bool ez = float_equal(z, v.z);
    return ex && ey && ez;
  }

  bool operator<(const MeshVertex &v) const {

    if (float_equal(x, v.x)) {
      if (float_equal(y, v.y))
        return z < v.z;
      else if (y < v.y) {
        return true;
      } else
        return false;
    } else if (x < v.x)
      return true;
    else
      return false;
  }

public:
  float x, y, z;
  float nx, ny, nz;
  unsigned int id;
};

class Mesh {
public:
  Mesh();
  // rule of five
  ~Mesh() = default; // destructor (virtual if Mesh is meant to be a base class)
  Mesh(const Mesh &) = default;                // copy constructor
  Mesh &operator=(const Mesh &) = default;     // copy assignment
  Mesh(Mesh &&) noexcept = default;            // move constructor
  Mesh &operator=(Mesh &&) noexcept = default; // move assignment

public:
  void add_vertex(const MeshVertex &v);
  void add_vertex(MeshVertex &&v);

  void addFacet(const std::vector<unsigned int> &facet);
  void addFacet(unsigned int facet_index);

  void reserve_vertice_numbers(int num) { vertice.reserve(num); }
  void set_num_vertex_per_face(int num) { num_vert_per_face = num; }
  void draw();
  void releaseGL();
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
  // box
  void box(float &cminx, float &cminy, float &cminz, float &cmaxx, float &cmaxy,
           float &cmaxz) {
    cminx = xmin;
    cminy = ymin;
    cminz = zmin;

    cmaxx = xmax;
    cmaxy = ymax;
    cmaxz = zmax;
  }

  void build();

private:
  void initGL();
  void remove_duplicates_vertice();

private:
  std::vector<MeshVertex> vertice;
  std::vector<unsigned int> elements;
  int num_vert_per_face = 3;

  float xmin, xmax, ymin, ymax, zmin, zmax;
  unsigned int vao = 0, vbo = 0, ibo = 0;
};

} // namespace DGL

#endif

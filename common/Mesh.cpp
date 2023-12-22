#include "Mesh.hpp"
#define GLAD_GL_IMPLEMENTATION
#include <flat_hash_map.h>
#include <glad/gl.h>

namespace DGL {

Mesh::Mesh() {
  xmin = 1e10;
  xmax = -1e10;
  ymin = 1e10;
  ymax = -1e10;
  zmin = 1e10;
  zmax = -1e10;
}

void Mesh::releaseGL() {
  if (vao != 0) {
    glDeleteVertexArrays(1, &vao);
  }
  if (vbo != 0) {
    glDeleteBuffers(1, &vbo);
  }
  if (ibo != 0) {
    glDeleteBuffers(1, &ibo);
  }
}
void Mesh::initGL() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertice.size() * sizeof(MeshVertex),
               &vertice[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int),
               &elements[0], GL_STATIC_DRAW);

  // Vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                        (void *)0);
  // Vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                        (void *)(sizeof(float) * 3));

  glBindVertexArray(0);
}

void Mesh::add_vertex(const MeshVertex &v) {
  xmax = v.x > xmax ? v.x : xmax;
  xmin = v.x < xmin ? v.x : xmin;
  ymax = v.y > ymax ? v.y : ymax;
  ymin = v.y < ymin ? v.y : ymin;
  zmax = v.z > zmax ? v.z : zmax;
  zmin = v.z < zmin ? v.z : zmin;

  vertice.push_back(v);
}

void Mesh::add_vertex(MeshVertex &&v) {
  xmax = v.x > xmax ? v.x : xmax;
  xmin = v.x < xmin ? v.x : xmin;
  ymax = v.y > ymax ? v.y : ymax;
  ymin = v.y < ymin ? v.y : ymin;
  zmax = v.z > zmax ? v.z : zmax;
  zmin = v.z < zmin ? v.z : zmin;
  vertice.emplace_back(v);
}
void Mesh::addFacet(const std::vector<unsigned int> &facet) {
  elements.insert(elements.end(), facet.begin(), facet.end());
}

void Mesh::addFacet(unsigned int facet_index) {
  elements.push_back(facet_index);
}

void Mesh::remove_duplicates_vertice() {
  if (vertice.size() == 0)
    return;

  std::sort(vertice.begin(), vertice.end(),
            [](const MeshVertex &v1, const MeshVertex &v2) -> bool {
              return v1 == v2 ? v1.id < v2.id : v1 < v2;
            });
  std::size_t i = 0;
  std::size_t j = 0;

  std::size_t num_vert = vertice.size();
  flat_hash_map<uint32_t, uint32_t> pairs;

  for (; i < num_vert;) {
    if ((j + 1) < num_vert && vertice[i] == vertice[j + 1]) {
      pairs[vertice[j + 1].id] = vertice[i].id;
      vertice.erase(std::next(vertice.begin(), j + 1));
      num_vert = vertice.size();
    } else {
      i = j + 1;
      j = i;
    }
  }

  for (int i = 0; i < elements.size(); i++) {
    if (pairs.count(elements[i]) != 0) {
      elements[i] = pairs[elements[i]];
    }
  }
  pairs.clear();
  std::size_t reindex = 0;
  std::for_each(vertice.begin(), vertice.end(),
                [&](MeshVertex &v) { pairs[v.id] = reindex; v.id = reindex++;
                });
  for (int i = 0; i < elements.size(); i++) {
    if (pairs.count(elements[i]) != 0) {
      elements[i] = pairs[elements[i]];
    }
  }
}

void Mesh::build() {
  printf(" ===> before remove vertex number = %zu facets : %zu \n",
         vertice.size(), elements.size() / num_vert_per_face);
  remove_duplicates_vertice();
  printf(" ===> after remove vertex number = %zu facets : %zu \n",
         vertice.size(), elements.size() / num_vert_per_face);
  initGL();
}
void Mesh::draw() {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0); // Unbind vao
}
} // namespace DGL

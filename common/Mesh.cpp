#include "Mesh.hpp"
#define GLAD_GL_IMPLEMENTATION
#include <assert.h>
#include <flat_hash_map.h>
#include <glad/gl.h>
namespace DGL
{

  Mesh::Mesh()
  {
    xmin = 1e10;
    xmax = -1e10;
    ymin = 1e10;
    ymax = -1e10;
    zmin = 1e10;
    zmax = -1e10;
  }

  void Mesh::releaseGL()
  {
    if (vao != 0)
    {
      glDeleteVertexArrays(1, &vao);
    }
    if (vbo != 0)
    {
      glDeleteBuffers(1, &vbo);
    }
    if (ibo != 0)
    {
      glDeleteBuffers(1, &ibo);
    }
  }
  void Mesh::initGL()
  {
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

  void Mesh::add_vertex(const MeshVertex &v)
  {
    xmax = v.x > xmax ? v.x : xmax;
    xmin = v.x < xmin ? v.x : xmin;
    ymax = v.y > ymax ? v.y : ymax;
    ymin = v.y < ymin ? v.y : ymin;
    zmax = v.z > zmax ? v.z : zmax;
    zmin = v.z < zmin ? v.z : zmin;

    vertice.push_back(v);
  }

  void Mesh::add_vertex(MeshVertex &&v)
  {
    xmax = v.x > xmax ? v.x : xmax;
    xmin = v.x < xmin ? v.x : xmin;
    ymax = v.y > ymax ? v.y : ymax;
    ymin = v.y < ymin ? v.y : ymin;
    zmax = v.z > zmax ? v.z : zmax;
    zmin = v.z < zmin ? v.z : zmin;
    vertice.emplace_back(v);
  }
  void Mesh::addFacet(const std::vector<unsigned int> &facet)
  {
    elements.insert(elements.end(), facet.begin(), facet.end());
  }

  void Mesh::addFacet(unsigned int facet_index)
  {
    elements.push_back(facet_index);
  }

  void Mesh::remove_duplicates_vertice()
  {
    if (vertice.size() == 0)
      return;

    std::sort(vertice.begin(), vertice.end(),
              [](const MeshVertex &v1, const MeshVertex &v2) -> bool
              {
                return v1 == v2 ? v1.id < v2.id : v1 < v2;
              });
    std::size_t i = 0;
    std::size_t j = 0;

    std::size_t num_vert = vertice.size();
    flat_hash_map<uint32_t, uint32_t> pairs;

    for (; i < num_vert;)
    {
      if ((j + 1) < num_vert && !vertice[i].is_deteted &&
          vertice[i] == vertice[j + 1])
      {
        pairs[vertice[j + 1].id] = vertice[i].id;
        vertice[j + 1].is_deteted = true;
        j++;
      }
      else
      {
        i = j + 1;
        j = i;
      }
    }

    for (int i = 0; i < elements.size(); i++)
    {
      if (pairs.count(elements[i]) != 0)
      {
        elements[i] = pairs[elements[i]];
      }
    }
    vertice.erase(
        std::remove_if(vertice.begin(), vertice.end(),
                       [&](const MeshVertex &v) -> bool
                       { return v.is_deteted; }),
        vertice.end());

    pairs.clear();
    std::size_t reindex = 0;
    std::for_each(vertice.begin(), vertice.end(), [&](MeshVertex &v)
                  {
    pairs[v.id] = reindex;
    v.id = reindex++; });
    for (int i = 0; i < elements.size(); i++)
    {
      if (pairs.count(elements[i]) != 0)
      {
        elements[i] = pairs[elements[i]];
      }
    }
  }

  void Mesh::initHalfEdges()
  {
    //int nbFaces = elements.size() / num_vert_per_face;
    //printf(" before remove degenrated face : %d \n", nbFaces);
    flat_hash_map<uint64_t, std::vector<uint32_t>> vertex_edge;
    // check for degenerated face
    /*for (auto iter = elements.begin(); iter != elements.end();)
    {
      auto vid1 = *iter;
      auto vid2 = *(iter + 1);
      auto vid3 = *(iter + 2);

      if (vid1 == vid2 || vid1 == vid3 || vid2 == vid3)
      {
        printf("find degenerated face and remove it: %d %d %d \n", vid1, vid2, vid3);
        iter = elements.erase(iter, iter + 3);
      }
      else
        iter = iter + 3;
    }*/
    int nbFaces = elements.size() / num_vert_per_face;
    //printf(" after remove degenrated face : %d \n", nbFaces);
    for (std::size_t i = 0; i < nbFaces; i++)
    {
      std::size_t cur_edge = halfedges.size();
      for (std::size_t f = 0; f < num_vert_per_face; f++)
      {
        auto vid = elements[i * num_vert_per_face + f];
        HalfEdge e;
        e.vertex_id = vid;
        e.face_id = i;
        e.next = cur_edge + (f + 1) % num_vert_per_face;
        e.prev =
            (f - 1) < 0 ? (cur_edge + num_vert_per_face - 1) : (cur_edge + f - 1);
        halfedges.push_back(e);
      }
    }

    for (uint32_t i = 0; i < halfedges.size(); ++i)
    {
      auto &edge = halfedges[i];
      auto v1 = edge.vertex_id;
      auto v2 = halfedges[edge.next].vertex_id;
      uint64_t A = v1 > v2 ? v1 : v2;
      uint64_t B = v1 > v2 ? v2 : v1;
      // Szudzik's function hash two integer
      auto C = (uint64_t)((A >= B ? A * A + A + B : A + B * B));
      if (vertex_edge.count(C) > 0)
      {
        auto count = vertex_edge[C].size();
        assert(count == 1);
        auto pair_id = vertex_edge[C].at(0);
        edge.pair = pair_id;
        halfedges[pair_id].pair = i;
        vertex_edge[C].push_back(i);
      }
      else
        vertex_edge[C] = {i};
    }

    // check
    int number_disconnected = 0;
    for (uint32_t i = 0; i < halfedges.size(); ++i)
    {
      if (halfedges[i].pair == -1)
      {
        number_disconnected++;
        // printf(" index : %3d , v : %3d , pair : %3d ,prev : %3d, next : %3d
        // \n", i,halfedges[i].vertex_id, halfedges[i].pair, halfedges[i].prev,
        // halfedges[i].next);
      }
    }
    printf(" ===> number_disconnected : %3d \n", number_disconnected);
  }

  void Mesh::smooth_normals()
  {
    flat_hash_map<uint32_t, std::vector<uint32_t>> vertex_faces;
    for (uint32_t i = 0; i < halfedges.size(); ++i)
    {
      auto &edge = halfedges[i];
      auto v1 = edge.vertex_id;
      if (vertex_faces.count(v1) > 0)
      {
        vertex_faces[v1].push_back(edge.face_id);
      }
      else
      {
        vertex_faces[v1] = std::vector<uint32_t>{edge.face_id};
      }
    }
    assert(vertex_faces.size() == vertice.size());
    assert(num_vert_per_face == 3);

    for (auto &item : vertex_faces)
    {
      auto &faces = item.second;
      float area = 0.0f;
      float lnx = 0.0f;
      float lny = 0.0f;
      float lnz = 0.0f;

      for (auto &face : faces)
      {
        auto fv0 = elements[num_vert_per_face * face + 0];
        auto fv1 = elements[num_vert_per_face * face + 1];
        auto fv2 = elements[num_vert_per_face * face + 2];

        auto &v0 = vertice[fv0];
        auto &v1 = vertice[fv1];
        auto &v2 = vertice[fv2];

        float dx0 = v1.x - v0.x;
        float dy0 = v1.y - v0.y;
        float dz0 = v1.z - v0.z;

        float dx1 = v2.x - v0.x;
        float dy1 = v2.y - v0.y;
        float dz1 = v2.z - v0.z;

        float nx = dy0 * dz1 - dy1 * dz0;
        float ny = dx1 * dz0 - dx0 * dz1;
        float nz = dx0 * dy1 - dx1 * dy0;

        lnx += nx;
        lny += ny;
        lnz += nz;

        float l = sqrt(nx * nx + ny * ny + nz * nz);
        area += l;
      }
      lnx /= area;
      lny /= area;
      lnz /= area;

      vertice[item.first].nx = lnx;
      vertice[item.first].ny = lny;
      vertice[item.first].nz = lnz;
    }
  }

  void Mesh::build()
  {
    printf(" ===> before remove vertex number = %zu facets : %zu \n",
           vertice.size(), elements.size() / num_vert_per_face);
    remove_duplicates_vertice();
    printf(" ===> after remove vertex number = %zu facets : %zu \n",
           vertice.size(), elements.size() / num_vert_per_face);
    initHalfEdges();
    smooth_normals();
    initGL();
  }
  void Mesh::draw()
  {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // Unbind vao
  }
} // namespace DGL

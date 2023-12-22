#include "Model.hpp"
#include "Mesh.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

namespace DGL {

Model::Model(const std::string &model_name) {
  xmin = 1e10;
  xmax = -1e10;
  ymin = 1e10;
  ymax = -1e10;
  zmin = 1e10;
  zmax = -1e10;
  load(model_name);
}
Model::~Model() {
  for (auto &mesh : m_meshes) {
    mesh.releaseGL();
  }
}
void Model::load(const std::string &model_name) {
  Assimp::Importer import;
  const aiScene *scene = import.ReadFile(model_name, aiProcess_Triangulate);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ASSIMP ERROR: " << import.GetErrorString() << std::endl;
    return;
  }
  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  // Process all the node's meshes (if any)
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    processMesh(mesh, scene);
  }
  // Do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  Mesh amesh;
  amesh.reserve_vertice_numbers(mesh->mNumVertices);
  // Load verticies
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    float x = mesh->mVertices[i].x;
    float y = mesh->mVertices[i].y;
    float z = mesh->mVertices[i].z;
    float nx = mesh->mNormals[i].x;
    float ny = mesh->mNormals[i].y;
    float nz = mesh->mNormals[i].z;
    amesh.add_vertex({i, x, y, z, nx, ny, nz});
  }
  // Load indices
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      amesh.addFacet(face.mIndices[j]);
  }

  float minx, miny, minz, maxx, maxy, maxz;
  amesh.box(minx, miny, minz, maxx, maxy, maxz);

  xmax = maxx > xmax ? maxx : xmax;
  xmin = minx < xmin ? minx : xmin;
  ymax = maxy > ymax ? maxy : ymax;
  ymin = miny < ymin ? miny : ymin;
  zmax = maxz > zmax ? maxz : zmax;
  zmin = minz < zmin ? minz : zmin;

  amesh.build();
  m_meshes.emplace_back(amesh);
}

void Model::draw() {
  for (auto i = 0; i < m_meshes.size(); i++) {
    m_meshes[i].draw();
  }
}
} // namespace DGL

#include "Model.hpp"
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
Model::~Model()
{
  unsigned int vao, vbo, ibo;
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

void Model::initGL() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertice.size() * sizeof(float), &vertice[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int),
               &elements[0], GL_STATIC_DRAW);

  // Vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)0);
  // Vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                        (void *)(sizeof(float) * 3));

  glBindVertexArray(0);
}
void Model::load(const std::string &model_name) {
  Assimp::Importer import;
  const aiScene *scene =
      import.ReadFile(model_name, aiProcess_Triangulate);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ASSIMP ERROR: " << import.GetErrorString() << std::endl;
    return;
  }
  processNode(scene->mRootNode, scene);
  printf(" ==> mesh loaded model  vertex: %zu \n", vertice.size()/6);
  initGL();
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

  vertice.reserve(vertice.size() + 6 * mesh->mNumVertices);
  // Load verticies
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    float x = mesh->mVertices[i].x;
    float y = mesh->mVertices[i].y;
    float z = mesh->mVertices[i].z;

    xmax = x > xmax ? x : xmax;
    xmin = x < xmin ? x : xmin;
    ymax = y > ymax ? y : ymax;
    ymin = y < ymin ? y : ymin;
    zmax = z > zmax ? z : zmax;
    zmin = z < zmin ? z : zmin;

    vertice.push_back(x);
    vertice.push_back(y);
    vertice.push_back(z);
    vertice.push_back(mesh->mNormals[i].x);
    vertice.push_back(mesh->mNormals[i].y);
    vertice.push_back(mesh->mNormals[i].z);
  }
  // Load indices
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      elements.push_back(face.mIndices[j]);
  }
}

void Model::draw() {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0); // Unbind vao
}
} // namespace DGL

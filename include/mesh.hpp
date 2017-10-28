#ifndef MESH_HPP
#define MESH_HPP
#include "shape.hpp"

class Mesh
{
public:
    glm::vec3* vertices;
    unsigned int size;
    Mesh(unsigned int n);
    ~Mesh();
    Box boundingBox();
};
#endif

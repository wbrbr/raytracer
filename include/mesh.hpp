#ifndef MESH_HPP
#define MESH_HPP
#include "shape.hpp"
#include "vertex.hpp"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    unsigned int size;
    Mesh(unsigned int n);
    ~Mesh();
    Box boundingBox();
};
#endif

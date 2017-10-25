#include "mesh.hpp"

Mesh::Mesh(unsigned int n)
{
    size = n;
    vertices = new Eigen::Vector3f[n];
}

Mesh::~Mesh()
{
    delete[] vertices;
}

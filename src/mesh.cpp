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

Box Mesh::boundingBox()
{
    Box box;
    box.minPoint = vertices[0];
    box.maxPoint = vertices[0];
    for (unsigned int i = 1; i < size; i++)
    {
        if (vertices[i](0) < box.minPoint(0)) box.minPoint(0) = vertices[i](0);
        if (vertices[i](1) < box.minPoint(1)) box.minPoint(1) = vertices[i](1);
        if (vertices[i](2) < box.minPoint(2)) box.minPoint(2) = vertices[i](2);

        if (vertices[i](0) > box.maxPoint(0)) box.maxPoint(0) = vertices[i](0);
        if (vertices[i](1) > box.maxPoint(1)) box.maxPoint(1) = vertices[i](1);
        if (vertices[i](2) > box.maxPoint(2)) box.maxPoint(2) = vertices[i](2);
    }
    return box;
}

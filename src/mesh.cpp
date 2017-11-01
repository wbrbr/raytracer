#include "mesh.hpp"

Mesh::Mesh(unsigned int n): vertices(n), size(n)
{
}

Mesh::~Mesh()
{
}

Box Mesh::boundingBox()
{
    Box box;
    box.minPoint = vertices[0].position;
    box.maxPoint = vertices[0].position;
    for (unsigned int i = 1; i < size; i++)
    {
        box.minPoint = minp(box.minPoint, vertices[i].position);
        box.maxPoint = maxp(box.maxPoint, vertices[i].position);
    }
    return box;
}

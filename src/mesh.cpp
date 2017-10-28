#include "mesh.hpp"

Mesh::Mesh(unsigned int n)
{
    size = n;
    vertices = new glm::vec3[n];
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
        if (vertices[i].x < box.minPoint.x) box.minPoint.x = vertices[i].x;
        if (vertices[i].y < box.minPoint.y) box.minPoint.y = vertices[i].y;
        if (vertices[i].z < box.minPoint.z) box.minPoint.z = vertices[i].z;

        if (vertices[i].x > box.maxPoint.x) box.maxPoint.x = vertices[i].x;
        if (vertices[i].y > box.maxPoint.y) box.maxPoint.y = vertices[i].y;
        if (vertices[i].z > box.maxPoint.z) box.maxPoint.z = vertices[i].z;
    }
    return box;
}

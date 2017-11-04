#include "shape.hpp"
#include "mesh.hpp"

std::optional<float> Triangle::intersects(Ray ray) const
{
    glm::vec3 v0 = vertex(0).position;
    glm::vec3 v1 = vertex(1).position;
    glm::vec3 v2 = vertex(2).position;
    float a,f,u,v;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(ray.orientation, edge2);
    a = glm::dot(edge1, h);
    if (a == 0.f) { // use epsilon
        return {};
    }

    f = 1.f / a;
    glm::vec3 s = ray.origin - v0;
    u = f * glm::dot(s, h);
    if (u < 0.f || u > 1.f) {
        return {};
    }

    glm::vec3 q = glm::cross(s, edge1);
    v = f * glm::dot(ray.orientation, q);
    if (v < 0.f || u + v > 1.f) {
        return {};
    }

    // there is an intersection
    float t = f * glm::dot(edge2, q);
    if (t <= 0.f) {
        return {};
    }
    return t;
}

glm::vec3 Triangle::normal(glm::vec3 point) const
{
    glm::vec3 edge01 = vertex(1).position - vertex(0).position;
    glm::vec3 edge02 = vertex(2).position - vertex(0).position;
    return glm::normalize(glm::cross(edge01, edge02));
}

Box Triangle::boundingBox() const
{
    glm::vec3 minPoint = vertex(0).position;
    glm::vec3 maxPoint = vertex(0).position;
    for (unsigned int i = 1; i < 3; i++)
    {
        minPoint = minp(minPoint, vertex(i).position);
        maxPoint = maxp(maxPoint, vertex(i).position);
    }
    Box box;
    box.minPoint = minPoint;
    box.maxPoint = maxPoint;
    return box;
}

glm::vec3 Triangle::centroid() const
{
    return (vertex(0).position + vertex(1).position + vertex(2).position) / 3.f;
}

Vertex Triangle::vertex(unsigned int n) const
{
    return mesh().vertices[indices[n]];
}

Mesh& Triangle::mesh() const
{
    return (*meshes)[mesh_id];
}

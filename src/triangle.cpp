#include "shape.hpp"
#include "mesh.hpp"

std::vector<float> Triangle::intersects(Ray ray) const
{
    auto res = std::vector<float>();
    glm::vec3 v0 = vertex(0);
    glm::vec3 v1 = vertex(1);
    glm::vec3 v2 = vertex(2);
    float a,f,u,v;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(ray.orientation, edge2);
    a = glm::dot(edge1, h);
    if (a == 0.f) { // use epsilon
        return res;
    }

    f = 1.f / a;
    glm::vec3 s = ray.origin - v0;
    u = f * glm::dot(s, h);
    if (u < 0.f || u > 1.f) {
        return res;
    }

    glm::vec3 q = glm::cross(s, edge1);
    v = f * glm::dot(ray.orientation, q);
    if (v < 0.f || u + v > 1.f) {
        return res;
    }

    // there is an intersection
    float t = f * glm::dot(edge2, q);
    if (t <= 0.f) {
        return res;
    }
    res.push_back(t);
    return res;
}

glm::vec3 Triangle::normal(glm::vec3 point) const
{
    glm::vec3 edge01 = vertex(1) - vertex(0);
    glm::vec3 edge02 = vertex(2) - vertex(0);
    return glm::normalize(glm::cross(edge01, edge02));
}

Box Triangle::boundingBox() const
{
    glm::vec3 minPoint = vertex(0);
    glm::vec3 maxPoint = vertex(0);
    for (unsigned int i = 1; i < 3; i++)
    {
        if (vertex(i).x < minPoint.x) minPoint.x = vertex(i).x;
        if (vertex(i).y < minPoint.y) minPoint.y = vertex(i).y;
        if (vertex(i).z < minPoint.z) minPoint.z = vertex(i).z;

        if (vertex(i).x > maxPoint.x) maxPoint.x = vertex(i).x;
        if (vertex(i).y > maxPoint.y) maxPoint.y = vertex(i).y;
        if (vertex(i).z > maxPoint.z) maxPoint.z = vertex(i).z;
    }
    Box box;
    box.minPoint = minPoint;
    box.maxPoint = maxPoint;
    return box;
}

glm::vec3 Triangle::centroid() const
{
    return (vertex(0) + vertex(1) + vertex(2)) / 3.f;
}

glm::vec3 Triangle::vertex(unsigned int n) const
{
    return mesh->vertices[indices[n]];
}

Triangle::~Triangle()
{
    
}

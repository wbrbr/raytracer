#include "shape.hpp"
#include <Eigen/Dense>
#include "mesh.hpp"

std::vector<float> Triangle::intersects(Ray ray)
{
    auto res = std::vector<float>();
    Vector3f v0 = vertex(0);
    Vector3f v1 = vertex(1);
    Vector3f v2 = vertex(2);
    float a,f,u,v;
    Vector3f edge1 = v1 - v0;
    Vector3f edge2 = v2 - v0;
    Vector3f h = ray.orientation.cross(edge2);
    a = edge1.dot(h);
    if (a == 0.f) { // use epsilon
        return res;
    }

    f = 1.f / a;
    Vector3f s = ray.origin - v0;
    u = f * s.dot(h);
    if (u < 0.f || u > 1.f) {
        return res;
    }

    Vector3f q = s.cross(edge1);
    v = f * ray.orientation.dot(q);
    if (v < 0.f || u + v > 1.f) {
        return res;
    }

    // there is an intersection
    float t = f * edge2.dot(q);
    if (t <= 0.f) {
        return res;
    }
    res.push_back(t);
    return res;
}

Vector3f Triangle::normal(Vector3f point)
{
    Vector3f edge01 = vertex(1) - vertex(0);
    Vector3f edge02 = vertex(2) - vertex(0);
    return edge01.cross(edge02).normalized();
}

Box Triangle::boundingBox()
{
    Eigen::Vector3f minPoint = vertex(0);
    Eigen::Vector3f maxPoint = vertex(0);
    for (unsigned int i = 1; i < 3; i++)
    {
        if (vertex(i).x() < minPoint.x()) minPoint.x() = vertex(i).x();
        if (vertex(i).y() < minPoint.y()) minPoint.y() = vertex(i).y();
        if (vertex(i).z() < minPoint.z()) minPoint.z() = vertex(i).z();

        if (vertex(i).x() > maxPoint.x()) maxPoint.x() = vertex(i).x();
        if (vertex(i).y() > maxPoint.y()) maxPoint.y() = vertex(i).y();
        if (vertex(i).z() > maxPoint.z()) maxPoint.z() = vertex(i).z();
    }
    Box box;
    box.minPoint = minPoint;
    box.maxPoint = maxPoint;
    return box;
}

Eigen::Vector3f Triangle::centroid()
{
    return (vertex(0) + vertex(1) + vertex(2)) / 3.f;
}

Vector3f Triangle::vertex(unsigned int n)
{
    return mesh->vertices[indices[n]];
}

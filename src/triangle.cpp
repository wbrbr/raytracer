#include "shape.hpp"
#include <Eigen/Dense>

std::vector<float> Triangle::intersects(Ray ray)
{
    auto res = std::vector<float>();
    Vector3f v0 = vertices[0];
    Vector3f v1 = vertices[1];
    Vector3f v2 = vertices[2];
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
    Vector3f edge01 = vertices[1] - vertices[0];
    Vector3f edge02 = vertices[2] - vertices[0];
    return edge01.cross(edge02).normalized();
}

#include "shape.hpp"
#include "math.hpp"

std::vector<float> Sphere::intersects(Ray ray)
{
    float a = 1.f;
    Eigen::Vector3f c_o = ray.origin - center;
    float b = 2.f * ray.orientation.dot(c_o);
    float c = c_o.norm() * c_o.norm() - radius * radius;

    return solve_quadratic(a, b, c);
}

Eigen::Vector3f Sphere::normal(Eigen::Vector3f point)
{
    return (point - center).normalized();
}

Box Sphere::boundingBox()
{
    Box box;
    box.minPoint = center - Eigen::Vector3f(radius, radius, radius);
    box.maxPoint = center + Eigen::Vector3f(radius, radius, radius);
    return box;
}

Eigen::Vector3f Sphere::centroid()
{
    return center;
}

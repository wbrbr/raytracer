#include "shape.hpp"
#include "math.hpp"

std::vector<float> Sphere::intersects(Ray ray)
{
    float a = 1.f;
    Vector3f c_o = ray.origin - center;
    float b = 2.f * ray.orientation.dot(c_o);
    float c = c_o.norm() * c_o.norm() - radius * radius;

    return solve_quadratic(a, b, c);
}

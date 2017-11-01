#include "shape.hpp"
#include "math.hpp"
#include <algorithm>

std::optional<float> Sphere::intersects(Ray ray) const
{
    float a = 1.f;
    glm::vec3 c_o = ray.origin - center;
    float b = 2.f * glm::dot(ray.orientation, c_o);
    float c = glm::length(c_o) * glm::length(c_o) - radius * radius;

    auto res = solve_quadratic(a, b, c);
    if (res.size() == 0) {
        return {};
    }
    return *std::min_element(res.begin(), res.end());
}

glm::vec3 Sphere::normal(glm::vec3 point) const
{
    return glm::normalize(point - center);
}

Box Sphere::boundingBox() const
{
    Box box;
    box.minPoint = center - glm::vec3(radius, radius, radius);
    box.maxPoint = center + glm::vec3(radius, radius, radius);
    return box;
}

glm::vec3 Sphere::centroid() const
{
    return center;
}

Sphere::~Sphere()
{
}

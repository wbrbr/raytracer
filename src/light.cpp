#include "light.hpp"

PointLight::~PointLight()
{
}

Ray PointLight::lightRay(glm::vec3 point) const
{
    return Ray::fromPoints(position, point);
}

bool PointLight::atPoint(glm::vec3 point, const BVHAccelerator& bvh, float* out_intensity) const
{
    Ray ray = Ray::fromPoints(point, position);
    if (bvh.closestHit(ray, nullptr, nullptr)) {
        return false; // light is hidden
    }
    *out_intensity = intensity / pow(glm::length(point - position), 2.f);
    return true;
}

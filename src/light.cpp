#include "light.hpp"

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

Ray DirectionalLight::lightRay(glm::vec3 point) const
{
    Ray ray;
    ray.origin = point - orientation;
    ray.orientation = orientation;
    return ray;
}

bool DirectionalLight::atPoint(glm::vec3 point, const BVHAccelerator& bvh, float* out_intensity) const
{
    Ray ray;
    ray.origin = point;
    ray.orientation = -orientation;

    if (bvh.closestHit(ray, nullptr, nullptr)) {
        return false;
    }
    *out_intensity = intensity;
    return true;
}

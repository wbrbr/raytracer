#ifndef LIGHT_HPP
#define LIGHT_HPP
#include <glm/vec3.hpp>
#include "color.hpp"
#include "bvh.hpp"

class Light
{
public:
    virtual ~Light() {};
    virtual Ray lightRay(glm::vec3 point) const = 0;
    virtual bool atPoint(glm::vec3 point, const BVHAccelerator& bvh, float* out_intensity) const = 0;
    Color color;
    float intensity;
};

class PointLight: public Light
{
public:
    ~PointLight();
    Ray lightRay(glm::vec3 point) const;
    bool atPoint(glm::vec3 point, const BVHAccelerator& bvh, float* out_intensity) const;

    glm::vec3 position;
};

class DirectionalLight: public Light
{
public:
    ~DirectionalLight();
    Ray lightRay(glm::vec3 point) const;
    bool atPoint(glm::vec3 point, const BVHAccelerator& bvh, float* out_intensity) const;

    glm::vec3 orientation;
};
#endif

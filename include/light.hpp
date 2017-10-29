#ifndef LIGHT_HPP
#define LIGHT_HPP
#include <glm/vec3.hpp>
#include "color.hpp"

class Light
{
public:
    glm::vec3 position;
    Color color;
    float intensity;
};
#endif

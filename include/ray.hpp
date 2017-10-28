#ifndef RAY_HPP
#define RAY_HPP
#include <glm/glm.hpp>

struct Ray
{
    glm::vec3 origin, orientation;
    static Ray fromPoints(glm::vec3 O, glm::vec3 M)
    {
        Ray r;
        r.origin = O;
        r.orientation = glm::normalize(M - O);
        return r;
    }

    void setOrientation(glm::vec3 o)
    {
        orientation = glm::normalize(o);
    }
};
#endif

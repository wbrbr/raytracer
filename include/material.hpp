#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include "color.hpp"

struct Material
{
    Color color;
    float reflection; // between 0 and 1
};
#endif

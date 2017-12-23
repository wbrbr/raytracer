#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "shape.hpp"
#include "material.hpp"

struct Object
{
    Shape* shape;
    Material* material;    
};
#endif

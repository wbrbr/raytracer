#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <utility>
#include <Eigen/Core>
using namespace Eigen;
#include "ray.hpp"
#include "math.hpp"

class Shape
{
public:
    virtual std::vector<Vector3f> intersects(Ray ray) = 0;
};

class Sphere
{
public:
    Vector3f center;
    float radius;

    std::vector<float> intersects(Ray ray);
};

class Triangle
{
public:
    Vector3f vertices[3];

    std::vector<float> intersects(Ray ray);
};
#endif

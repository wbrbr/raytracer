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
    virtual std::vector<float> intersects(Ray ray) = 0;
    virtual Vector3f normal(Vector3f point) = 0;
};

class Sphere: public Shape
{
public:
    Vector3f center;
    float radius;

    std::vector<float> intersects(Ray ray);
    Vector3f normal(Vector3f point);
};

class Triangle: public Shape
{
public:
    Vector3f vertices[3];

    std::vector<float> intersects(Ray ray);
    Vector3f normal(Vector3f point);
};
#endif

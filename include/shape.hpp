#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <utility>
#include "ray.hpp"
#include "math.hpp"

class Shape
{
public:
    virtual std::vector<float> intersects(Ray ray) = 0;
    virtual Eigen::Vector3f normal(Vector3f point) = 0;
};

class Sphere: public Shape
{
public:
    Eigen::Vector3f center;
    float radius;

    std::vector<float> intersects(Ray ray);
    Eigen::Vector3f normal(Vector3f point);
};

class Mesh;
class Triangle: public Shape
{
public:
    Mesh* mesh;
    unsigned int indices[3];

    std::vector<float> intersects(Ray ray);
    Eigen::Vector3f normal(Vector3f point);
    Vector3f vertex(unsigned int n);
};

class Box: public Shape
{
public:
    Eigen::Vector3f minPoint, maxPoint;

    std::vector<float> intersects(Ray ray);
    Eigen::Vector3f normal(Eigen::Vector3f point);
};
#endif

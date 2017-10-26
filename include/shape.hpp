#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <utility>
#include "ray.hpp"
#include "math.hpp"

class Box
{
public:
    Eigen::Vector3f minPoint, maxPoint;

    std::vector<float> intersects(Ray ray) const;
};

class Shape
{
public:
    virtual ~Shape() {};
    virtual std::vector<float> intersects(Ray ray) const = 0;
    virtual Eigen::Vector3f normal(Vector3f point) const = 0;
    virtual Box boundingBox() const = 0;
    virtual Vector3f centroid() const = 0;
};

class Sphere: public Shape
{
public:
    Eigen::Vector3f center;
    float radius;

    ~Sphere();
    std::vector<float> intersects(Ray ray) const;
    Eigen::Vector3f normal(Vector3f point) const;
    Box boundingBox() const;
    Vector3f centroid() const;
};

class Mesh;
class Triangle: public Shape
{
public:
    Mesh* mesh;
    unsigned int indices[3];

    ~Triangle();
    std::vector<float> intersects(Ray ray) const;
    Eigen::Vector3f normal(Vector3f point) const;
    Vector3f vertex(unsigned int n) const;
    Box boundingBox() const;
    Vector3f centroid() const;
};
#endif

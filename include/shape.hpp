#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <utility>
#include "ray.hpp"
#include "math.hpp"
#include "vertex.hpp"

class Box
{
public:
    glm::vec3 minPoint, maxPoint;

    std::vector<float> intersects(Ray ray) const;
};

class Shape
{
public:
    virtual ~Shape() {};
    virtual std::vector<float> intersects(Ray ray) const = 0;
    virtual glm::vec3 normal(glm::vec3 point) const = 0;
    virtual Box boundingBox() const = 0;
    virtual glm::vec3 centroid() const = 0;
};

class Sphere: public Shape
{
public:
    glm::vec3 center;
    float radius;

    ~Sphere();
    std::vector<float> intersects(Ray ray) const;
    glm::vec3 normal(glm::vec3 point) const;
    Box boundingBox() const;
    glm::vec3 centroid() const;
};

class Mesh;
class Triangle: public Shape
{
public:
    Mesh* mesh;
    unsigned int indices[3];

    ~Triangle();
    std::vector<float> intersects(Ray ray) const;
    glm::vec3 normal(glm::vec3 point) const;
    Box boundingBox() const;
    glm::vec3 centroid() const;

    Vertex vertex(unsigned int n) const;
};

class TransformedShape: public Shape
{
public:
    Shape* shape;
    glm::mat4 transform;

    ~TransformedShape();
    std::vector<float> intersects(Ray ray) const;
    glm::vec3 normal(glm::vec3 point) const;
    Box boundingBox() const;
    glm::vec3 centroid() const;
};
#endif

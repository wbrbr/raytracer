#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <utility>
#include <optional>
#include <memory>
#include "ray.hpp"
#include "math.hpp"
#include "vertex.hpp"
#include "material.hpp"

class Box
{
public:
    glm::vec3 minPoint, maxPoint;

    std::optional<float> intersects(Ray ray) const;
};

class Shape
{
public:
    virtual ~Shape() = default;
    virtual std::optional<float> intersects(Ray ray) const = 0;
    virtual glm::vec3 normal(glm::vec3 point) const = 0;
    virtual Box boundingBox() const = 0;
    virtual glm::vec3 centroid() const = 0;
};

class Sphere: public Shape
{
public:
    glm::vec3 center;
    float radius;

    std::optional<float> intersects(Ray ray) const;
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

    std::optional<float> intersects(Ray ray) const;
    glm::vec3 normal(glm::vec3 point) const;
    Box boundingBox() const;
    glm::vec3 centroid() const;

    Vertex vertex(unsigned int n) const;
};

class TransformedShape: public Shape
{
public:
    std::unique_ptr<Shape> shape;
    glm::mat4 transform;

    void setTransform(glm::mat4 transform);
    std::optional<float> intersects(Ray ray) const;
    glm::vec3 normal(glm::vec3 point) const;
    Box boundingBox() const;
    glm::vec3 centroid() const;

private:
    glm::mat4 m_transform, m_inv_transform;
};
#endif

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
    virtual ~Shape() {};
    virtual std::optional<float> intersects(Ray ray) const = 0;
    virtual glm::vec3 normal(glm::vec3 point) const = 0;
    virtual Box boundingBox() const = 0;
    virtual glm::vec3 centroid() const = 0;

    Material* material;
};

class Sphere: public Shape
{
public:
    glm::vec3 center;
    float radius;

    ~Sphere();
    std::optional<float> intersects(Ray ray) const;
    glm::vec3 normal(glm::vec3 point) const;
    Box boundingBox() const;
    glm::vec3 centroid() const;
};

class Mesh;
class Triangle: public Shape
{
public:
    std::vector<Mesh>* meshes;
    unsigned int mesh_id;
    unsigned int indices[3];

    ~Triangle();
    std::optional<float> intersects(Ray ray) const;
    glm::vec3 normal(glm::vec3 point) const;
    Box boundingBox() const;
    glm::vec3 centroid() const;

    Vertex vertex(unsigned int n) const;
    Mesh& mesh() const;
};

class TransformedShape: public Shape
{
public:
    std::shared_ptr<Shape> shape;
    glm::mat4 transform;

    ~TransformedShape();
    std::optional<float> intersects(Ray ray) const;
    glm::vec3 normal(glm::vec3 point) const;
    Box boundingBox() const;
    glm::vec3 centroid() const;
};
#endif

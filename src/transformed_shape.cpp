#include "shape.hpp"

TransformedShape::~TransformedShape()
{
}

std::vector<float> TransformedShape::intersects(Ray ray) const
{
    glm::mat4 inv_transform = glm::inverse(transform);
    Ray transformed_ray;
    transformed_ray.origin = inv_transform * glm::vec4(ray.origin, 1.f);
    transformed_ray.orientation = glm::vec3(glm::normalize(inv_transform * glm::vec4(ray.orientation, 0.f)));

    auto obj_space_inter = shape->intersects(transformed_ray);
    std::vector<float> world_space_inter;
    for (auto t : obj_space_inter)
    {
        glm::vec3 point_object_space = transformed_ray.origin + t * transformed_ray.orientation;
        glm::vec3 point_world_space = transform * glm::vec4(point_object_space, 1.f);
        world_space_inter.push_back(glm::length(point_world_space - ray.origin));
    }

    return world_space_inter;
}

glm::vec3 TransformedShape::normal(glm::vec3 point) const
{
    return glm::vec3(glm::normalize(glm::transpose(glm::inverse(transform)) * glm::vec4(shape->normal(point), 0.f)));
}

Box TransformedShape::boundingBox() const
{
    Box old_box = shape->boundingBox();
    Box new_box;
    new_box.minPoint = old_box.minPoint;
    new_box.maxPoint = old_box.minPoint;

    std::vector<glm::vec3> points;
    points.push_back(glm::vec3(old_box.maxPoint.x, old_box.minPoint.y, old_box.minPoint.z));
    points.push_back(glm::vec3(old_box.minPoint.x, old_box.maxPoint.y, old_box.minPoint.z));
    points.push_back(glm::vec3(old_box.minPoint.x, old_box.minPoint.y, old_box.maxPoint.z));
    points.push_back(glm::vec3(old_box.minPoint.x, old_box.maxPoint.y, old_box.maxPoint.z));
    points.push_back(glm::vec3(old_box.maxPoint.x, old_box.minPoint.y, old_box.maxPoint.z));
    points.push_back(glm::vec3(old_box.maxPoint.x, old_box.maxPoint.y, old_box.minPoint.z));
    points.push_back(old_box.maxPoint);

    for (auto p : points)
    {
        glm::vec3 trans_p(transform * glm::vec4(p, 1.f));
        new_box.minPoint = minp(new_box.minPoint, trans_p);
        new_box.maxPoint = maxp(new_box.maxPoint, trans_p);
    }

    return new_box;
}

glm::vec3 TransformedShape::centroid() const
{
    return glm::vec3(transform * glm::vec4(shape->centroid(), 1.f));
}
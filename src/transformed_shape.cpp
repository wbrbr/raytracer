#include "shape.hpp"

void TransformedShape::setTransform(glm::mat4 transform)
{
    m_transform = transform;
    m_inv_transform = glm::inverse(transform);
}

std::optional<float> TransformedShape::intersects(Ray ray) const
{
    Ray transformed_ray;
    transformed_ray.origin = m_inv_transform * glm::vec4(ray.origin, 1.f);
    transformed_ray.orientation = glm::vec3(glm::normalize(m_inv_transform * glm::vec4(ray.orientation, 0.f)));

    auto obj_space_inter = shape->intersects(transformed_ray);
    if (!obj_space_inter) {
        return {};
    }
    glm::vec3 point_object_space = transformed_ray.origin + *obj_space_inter * transformed_ray.orientation;
    glm::vec3 point_world_space = transform * glm::vec4(point_object_space, 1.f);

    return glm::length(point_world_space - ray.origin);
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

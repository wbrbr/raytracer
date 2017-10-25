#include "shape.hpp"

std::vector<float> Box::intersects(Ray ray)
{
    std::vector<float> res;
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    float divx = 1.f / ray.orientation.x();
    if (divx >= 0) {
        tmin = (minPoint.x() - ray.origin.x()) * divx;
        tmax = (maxPoint.x() - ray.origin.x()) * divx;
    } else {
        tmin = (maxPoint.x() - ray.origin.x()) * divx;
        tmax = (minPoint.x() - ray.origin.x()) * divx;
    }

    float divy = 1.f / ray.orientation.y();
    if (divy >= 0) {
        tymin = (minPoint.y() - ray.origin.y()) * divy;
        tymax = (maxPoint.y() - ray.origin.y()) * divy;
    } else {
        tymin = (maxPoint.y() - ray.origin.y()) * divy;
        tymax = (minPoint.y() - ray.origin.y()) * divy;
    }

    if ((tmin > tymax) || (tymin > tmax)) {
        return res;
    }
    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }

    float divz = 1.f / ray.orientation.z();
    if (divz >= 0) {
        tzmin = (minPoint.z() - ray.origin.z()) * divz;
        tzmax = (maxPoint.z() - ray.origin.z()) * divz;
    } else {
        tzmin = (maxPoint.z() - ray.origin.z()) * divz;
        tzmax = (minPoint.z() - ray.origin.z()) * divz;
    }

    if ((tmin > tzmax) || (tzmin > tmax)) {
        return res;
    }
    if (tzmin > tmin) {
        tmin = tzmin;
    }
    if (tzmax < tmax) {
        tmax = tzmax;
    }

    res.push_back(tmin);
    res.push_back(tmax);
    return res;
}

Eigen::Vector3f Box::normal(Eigen::Vector3f point) // TODO
{
    return Vector3f(0.f, 0.f, 0.f);
}

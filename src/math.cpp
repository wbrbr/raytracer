#include "math.hpp"

std::vector<float> solve_quadratic(float a, float b, float c)
{
    float delta = b*b - 4*a*c;
    std::vector<float> result;
    if (delta == 0) {
        result.push_back(-b / (2*a));
    }
    else if (delta > 0) {
        result.push_back((-b - sqrt(delta)) / (2*a));
        result.push_back((-b + sqrt(delta)) / (2*a));
    }
    return result;
}

Eigen::Vector3f minp(Eigen::Vector3f v0, Eigen::Vector3f v1)
{
    Eigen::Vector3f m;
    m.x() = fmin(v0.x(), v1.x());
    m.y() = fmin(v0.y(), v1.y());
    m.z() = fmin(v0.z(), v1.z());

    return m;
}

Eigen::Vector3f maxp(Eigen::Vector3f v0, Eigen::Vector3f v1)
{
    Eigen::Vector3f m;
    m.x() = fmax(v0.x(), v1.x());
    m.y() = fmax(v0.y(), v1.y());
    m.z() = fmax(v0.z(), v1.z());

    return m;
}

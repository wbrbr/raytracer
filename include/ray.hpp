#ifndef RAY_HPP
#define RAY_HPP
#include <Eigen/Core>
using namespace Eigen;

struct Ray
{
    Vector3f origin, orientation;
    static Ray fromPoints(Eigen::Vector3f O, Eigen::Vector3f M)
    {
        Ray r;
        r.origin = O;
        r.orientation = (M - O).normalized();
        return r;
    }

    void setOrientation(Vector3f o)
    {
        orientation = o.normalized();
    }
};
#endif

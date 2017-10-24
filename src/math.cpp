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

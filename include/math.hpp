#ifndef MATH_HPP
#define MATH_HPP
#include <vector>
#include <cmath>
#include <Eigen/Core>

std::vector<float> solve_quadratic(float a, float b, float c);
Eigen::Vector3f minp(Eigen::Vector3f v0, Eigen::Vector3f v1);
Eigen::Vector3f maxp(Eigen::Vector3f v0, Eigen::Vector3f v1);
#endif

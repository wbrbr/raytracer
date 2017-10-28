#ifndef MATH_HPP
#define MATH_HPP
#include <vector>
#include <cmath>
#include <glm/vec3.hpp>

std::vector<float> solve_quadratic(float a, float b, float c);
glm::vec3 minp(glm::vec3 v0, glm::vec3 v1);
glm::vec3 maxp(glm::vec3 v0, glm::vec3 v1);
#endif

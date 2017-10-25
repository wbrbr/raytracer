#ifndef MESH_HPP
#define MESH_HPP
#include <Eigen/Core>

class Mesh
{
public:
    Eigen::Vector3f* vertices;
    unsigned int size;
    Mesh(unsigned int n);
    ~Mesh();
};
#endif

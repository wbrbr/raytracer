#ifndef BVH_HPP
#define BVH_HPP
#include "shape.hpp"
#include "ray.hpp"
#include <vector>

struct BVHNode
{
    bool initialized;
    std::vector<Shape*>::iterator begin, end;
    bool leaf;
    unsigned int left, right;
    Box boundingBox;
};

class BVHAccelerator
{
public:
    void build(std::vector<Shape*>::iterator begin, std::vector<Shape*>::iterator end);
    bool closestHit(Ray ray, float* closest, Shape** closest_shape) const;
private:
    void buildRecursive(unsigned int i);
    void closestHitRecursive(Ray ray, unsigned int i, float* closest, Shape** closest_shape) const;
    std::vector<BVHNode> nodes;
    std::vector<Shape*> shapes;
    unsigned int left_index, right_index, next_node;
};
#endif

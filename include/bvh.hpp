#ifndef BVH_HPP
#define BVH_HPP
#include "object.hpp"
#include "ray.hpp"
#include <vector>

struct BVHNode
{
    bool initialized;
    std::vector<Object>::iterator begin, end;
    bool leaf;
    unsigned int left, right;
    Box boundingBox;
};

class BVHAccelerator
{
public:
    void build(std::vector<Object>::iterator begin, std::vector<Object>::iterator end);
    bool closestHit(Ray ray, float* closest, Object* closest_object) const;
private:
    void buildRecursive(unsigned int i);
    void closestHitRecursive(Ray ray, unsigned int i, float* closest, Object* closest_object) const;
    std::vector<BVHNode> nodes;
    std::vector<Object> objects;
    unsigned int left_index, right_index, next_node;
};
#endif

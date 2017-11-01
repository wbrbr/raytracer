#include "bvh.hpp"
#include "math.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <cassert>

Box computeBoundingBox(std::vector<Shape*>::iterator begin, std::vector<Shape*>::iterator end)
{
    Box b = (*begin)->boundingBox();
    for (auto it = begin; it != end; it++)
    {
        Box b1 = (*it)->boundingBox();
        b.minPoint = minp(b.minPoint, b1.minPoint);
        b.maxPoint = maxp(b.maxPoint, b1.maxPoint);
    }
    return b;
}

void BVHAccelerator::build(std::vector<Shape*>::iterator begin, std::vector<Shape*>::iterator end)
{
    std::copy(begin, end, std::back_inserter(shapes));
    left_index = 0;
    right_index = shapes.size();
    next_node = 1;

    BVHNode root;
    root.initialized = true;
    root.begin = shapes.begin();
    root.end = shapes.end();
    root.leaf = false;
    root.boundingBox = computeBoundingBox(root.begin, root.end);
    nodes.push_back(root);
    buildRecursive(0);
}

bool BVHAccelerator::closestHit(Ray ray, float* closest, Shape** closest_shape) const
{
    for (unsigned int i = 0; i < nodes.size(); i++)
    {
        assert(nodes[i].initialized == true);
    }
    if (closest == nullptr) {
        float c = INFINITY;
        closestHitRecursive(ray, 0, &c, closest_shape);
        return c != INFINITY;
    } else {
        *closest = INFINITY;
        closestHitRecursive(ray, 0, closest, closest_shape);
        return *closest != INFINITY;
    }
}

void BVHAccelerator::closestHitRecursive(Ray ray, unsigned int i, float* closest, Shape** closest_shape) const
{
    auto bbox_inter = nodes[i].boundingBox.intersects(ray);
    if (!bbox_inter) {
        return;
    }
    if (*bbox_inter > *closest) {
        return;
    }

    if (nodes[i].leaf) {
        for (auto it = nodes[i].begin; it != nodes[i].end; it++)
        {
            auto res = (*it)->intersects(ray);
            if (!res) continue;
            if (closest != nullptr && *res < *closest) {
                *closest = *res;
                if (closest_shape != nullptr) {
                    *closest_shape = *it;
                }
            }
        }
    } else {
        closestHitRecursive(ray, nodes[i].left, closest, closest_shape);
        closestHitRecursive(ray, nodes[i].right, closest, closest_shape);
    }
}

bool sortX(Shape* s1, Shape* s2)
{
    return s1->centroid().x < s2->centroid().x;
}

bool sortY(Shape* s1, Shape* s2)
{
    return s1->centroid().y < s2->centroid().y;
}

bool sortZ(Shape* s1, Shape* s2)
{
    return s1->centroid().z < s2->centroid().z;
}

bool findX(float limit, Shape* s)
{
    return s->centroid().x >= limit;
}

bool findY(float limit, Shape* s)
{
    return s->centroid().y >= limit;
}

bool findZ(float limit, Shape* s)
{
    return s->centroid().z >= limit;
}

void BVHAccelerator::buildRecursive(unsigned int i)
{
    nodes[i].initialized = true;
    unsigned int dist = std::distance(nodes[i].begin, nodes[i].end);
    if (dist < 4) {
        nodes[i].leaf = true;
        return;
    }
    nodes[i].leaf = false;

    // find largest dimension
    glm::vec3 box_vec = nodes[i].boundingBox.maxPoint - nodes[i].boundingBox.minPoint;
    std::vector<Shape*>::iterator first_after_limit;
    if (box_vec.x >= box_vec.y && box_vec.x >= box_vec.z) {
        // sort elements by X
        std::sort(nodes[i].begin, nodes[i].end, sortX);
        float limit = (nodes[i].boundingBox.minPoint.x + nodes[i].boundingBox.maxPoint.x) / 2.f;
        auto predicate = std::bind(findX, limit, std::placeholders::_1);
        first_after_limit = std::find_if(nodes[i].begin, nodes[i].end, predicate);
        if (first_after_limit == nodes[i].begin || first_after_limit == nodes[i].end) { // fall back to median split
            first_after_limit = nodes[i].begin + dist / 2;
        }

    } else if (box_vec.y >= box_vec.x && box_vec.y >= box_vec.z) {

        std::sort(nodes[i].begin, nodes[i].end, sortY);
        float limit = (nodes[i].boundingBox.minPoint.y + nodes[i].boundingBox.maxPoint.y) / 2.f;
        auto predicate = std::bind(findY, limit, std::placeholders::_1);
        first_after_limit = std::find_if(nodes[i].begin, nodes[i].end, predicate);
        if (first_after_limit == nodes[i].begin || first_after_limit == nodes[i].end) { // fall back to median split
            first_after_limit = nodes[i].begin + dist / 2;
        }

    } else {

        std::sort(nodes[i].begin, nodes[i].end, sortZ);
        float limit = (nodes[i].boundingBox.minPoint.z + nodes[i].boundingBox.maxPoint.z) / 2.f;
        auto predicate = std::bind(findZ, limit, std::placeholders::_1);
        first_after_limit = std::find_if(nodes[i].begin, nodes[i].end, predicate);
        if (first_after_limit == nodes[i].begin || first_after_limit == nodes[i].end) { // fall back to median split
            first_after_limit = nodes[i].begin + dist / 2;
        }
    }

    BVHNode left, right;
    nodes[i].left = next_node;
    nodes[i].right = next_node + 1;
    next_node += 2;

    left.begin = nodes[i].begin;
    left.end = first_after_limit;
    left.boundingBox = computeBoundingBox(left.begin, left.end);
    nodes.push_back(left);
    right.begin = first_after_limit;
    right.end = nodes[i].end;
    right.boundingBox = computeBoundingBox(right.begin, right.end);
    nodes.push_back(right);

    buildRecursive(nodes[i].left); // left
    buildRecursive(nodes[i].right); // right
}

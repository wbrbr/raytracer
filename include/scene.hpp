#ifndef SCENE_HPP
#define SCENE_HPP
#include <string>
#include <map>
#include <list>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "light.hpp"
#include "shape.hpp"
#include "mesh.hpp"
#include "bvh.hpp"
#include "object.hpp"

class Scene
{
public:
    Scene();
    ~Scene();
    void load(std::string path);
    std::vector<std::unique_ptr<Light>> lights;
    std::vector<Shape*> shapes;
    std::map<std::string, std::unique_ptr<Mesh>> meshes;
    std::map<std::string, Material> materials;
    std::vector<Object> objects;
    BVHAccelerator bvh;
};
#endif

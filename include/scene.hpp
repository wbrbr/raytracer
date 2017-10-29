#ifndef SCENE_HPP
#define SCENE_HPP
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "light.hpp"
#include "shape.hpp"
#include "mesh.hpp"
#include "bvh.hpp"

class Scene
{
public:
    Scene();
    ~Scene();
    void load(std::string path);
    void loadAssimp(std::string path);
    std::vector<Light> lights;
    std::vector<Shape*> shapes;
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    BVHAccelerator bvh;
};
#endif

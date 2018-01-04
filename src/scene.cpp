#include "scene.hpp"
#include "color.hpp"
#include "json.hpp"
using json = nlohmann::json;
#include <fstream>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene()
{
}

Scene::~Scene()
{
    for (unsigned int i = 0; i < shapes.size(); i++)
    {
        Shape* ptr = shapes[i];
        delete ptr;
    }
}

namespace glm
{
    void from_json(const json& j, glm::vec3& v)
    {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }
};

void from_json(const json& j, Color& c)
{
    c.r = j.at(0).get<float>() / 255.f;
    c.g = j.at(1).get<float>() / 255.f;
    c.b = j.at(2).get<float>() / 255.f;
}

void from_json(const json& j, Material& m)
{
    m.reflection = j.at("reflection").get<float>();
    m.color = j.at("diffuse").get<Color>();
}

void from_json(const json& j, PointLight& l)
{
    l.color = j.at("color").get<Color>();
    l.intensity = j.at("intensity").get<float>();
    l.position = j.at("position").get<glm::vec3>();
}

void Scene::load(std::string path)
{
    std::cout << "Loading the scene..." << std::endl;
    std::ifstream infile(path);
    json j;
    infile >> j;
    materials = j.at("materials").get<std::map<std::string, Material>>();
    for (auto it = j.at("lights").begin(); it != j.at("lights").end(); it++)
    {
        auto light = new PointLight();
        *light = it.value().get<PointLight>();
        lights.push_back(std::unique_ptr<Light>(light));
    }
    for (auto it = j.at("meshes").begin(); it != j.at("meshes").end(); it++)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(it.value().get<std::string>(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
        aiMesh* m = scene->mMeshes[0];
        auto mesh = new Mesh(m->mNumVertices);
        for (unsigned int i = 0; i < m->mNumVertices; i++)
        {
            Vertex v;
            v.position = glm::vec3(m->mVertices[i].x, m->mVertices[i].y, m->mVertices[i].z);
            v.normal = glm::normalize(glm::vec3(m->mNormals[i].x, m->mNormals[i].y, m->mNormals[i].z));
            mesh->vertices[i] = v;
        }
        for (unsigned int i = 0; i < m->mNumFaces; i++)
        {
            mesh->indices.push_back(m->mFaces[i].mIndices[0]);
            mesh->indices.push_back(m->mFaces[i].mIndices[1]);
            mesh->indices.push_back(m->mFaces[i].mIndices[2]);
        }
        meshes.insert(std::pair<std::string, std::unique_ptr<Mesh>>(it.key(), std::unique_ptr<Mesh>(mesh)));
    }
    for (auto it = j.at("objects").begin(); it != j.at("objects").end(); it++)
    {
        std::string mesh_name = it->at("mesh").get<std::string>();
        auto mesh = meshes.at(mesh_name).get();
        std::string material_name = it->at("material").get<std::string>();
        Material* material = &materials.at(material_name);
        glm::vec3 position = it->at("position").get<glm::vec3>();
        for (unsigned int i = 0; i < mesh->indices.size() / 3; i++)
        {
            auto triangle = new Triangle;
            triangle->mesh = mesh;
            triangle->indices[0] = mesh->indices[3*i];
            triangle->indices[1] = mesh->indices[3*i+1];
            triangle->indices[2] = mesh->indices[3*i+2];
            auto trans_shape = new TransformedShape;
            trans_shape->shape = std::unique_ptr<Shape>(triangle);
            glm::mat4 trans;
            trans = glm::translate(trans, position);
            // trans = glm::rotate(trans, static_cast<float>(M_PI)/2.f, glm::vec3(0.f, 1.f, 0.f));
            trans_shape->setTransform(trans); // handle position/rotation/scale
            shapes.push_back(trans_shape);

            Object obj;
            obj.shape = trans_shape;
            obj.material = material;
            objects.push_back(obj);
        }
    }
    std::cout << "Building BVH..." << std::endl;
    bvh.build(objects.begin(), objects.end());
}

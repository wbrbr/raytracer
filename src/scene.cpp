#include "scene.hpp"
#include "color.hpp"
#include <iostream>
#include <glm/vec3.hpp>

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

void Scene::load(std::string path)
{
    Material material;
    material.color = Color{1.f, 0.3f, 0.3f};
    materials.push_back(material);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    if (!scene) {
        std::cerr << "failed to load " << path << ": " << importer.GetErrorString() << std::endl;
        return;
    }
    // aiNode* node = scene->mRootNode;
    std::vector<aiNode*> nodestack;
    nodestack.push_back(scene->mRootNode);
    while (nodestack.size() > 0)
    {
        aiNode* node = nodestack.back();
        nodestack.pop_back();
        if (node->mNumMeshes > 0) {
            unsigned int mindex = node->mMeshes[0];
            aiMesh* m = scene->mMeshes[mindex];
            Mesh mesh(m->mNumVertices);
            for (unsigned int i = 0; i < m->mNumVertices; i++)
            {
                Vertex v;
                v.position = glm::vec3(m->mVertices[i].x, m->mVertices[i].y, m->mVertices[i].z);
                v.normal = glm::normalize(glm::vec3(m->mNormals[i].x, m->mNormals[i].y, m->mNormals[i].z));
                mesh.vertices[i] = v;
            }
            meshes.push_back(mesh);

            aiMatrix4x4 aiTrans = node->mTransformation.Transpose();
            glm::mat4 trans;
            trans[0][0] = aiTrans.a1;
            trans[0][1] = aiTrans.a2;
            trans[0][2] = aiTrans.a3;
            trans[0][3] = aiTrans.a4;
            trans[1][0] = aiTrans.b1;
            trans[1][1] = aiTrans.b2;
            trans[1][2] = aiTrans.b3;
            trans[1][3] = aiTrans.b4;
            trans[2][0] = aiTrans.c1;
            trans[2][1] = aiTrans.c2;
            trans[2][2] = aiTrans.c3;
            trans[2][3] = aiTrans.c4;
            trans[3][0] = aiTrans.d1;
            trans[3][1] = aiTrans.d2;
            trans[3][2] = aiTrans.d3;
            trans[3][3] = aiTrans.d4;
            
            for (unsigned int i = 0; i < m->mNumFaces; i++)
            {
                auto triangle = new Triangle;
                triangle->meshes = &meshes;
                triangle->mesh_id = meshes.size()-1;
                triangle->indices[0] = m->mFaces[i].mIndices[0];
                triangle->indices[1] = m->mFaces[i].mIndices[1];
                triangle->indices[2] = m->mFaces[i].mIndices[2];

                auto trans_shape = new TransformedShape;
                trans_shape->shape = std::shared_ptr<Shape>(triangle);
                trans_shape->transform = trans;
                trans_shape->material = &materials[0];
                shapes.push_back(trans_shape);
            }
        }
        if (node->mNumChildren > 0)
        {
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                nodestack.push_back(node->mChildren[i]);
            }
        }
    }
    bvh.build(shapes.begin(), shapes.end());
    std::cout << "Finished building BVH" << std::endl;
}

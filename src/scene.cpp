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
    std::cout << "Loading the scene..." << std::endl;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    if (!scene) {
        std::cerr << "failed to load " << path << ": " << importer.GetErrorString() << std::endl;
        return;
    }

    // Load materials
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* aiMat = scene->mMaterials[i];
        aiColor3D diff(1.f, 0.f, 0.f);
        aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
        aiString name;
        aiMat->Get(AI_MATKEY_NAME, name);
        std::cout << name.C_Str() << std::endl;
        Material material;
        material.color = Color{diff.r, diff.g, diff.b};
        material.reflection = 0.f;
        materials.push_back(material);
    }
    materials[2].reflection = 0.5f;

    // Load geometry
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
                triangle->mesh = &meshes.back();
                triangle->indices[0] = m->mFaces[i].mIndices[0];
                triangle->indices[1] = m->mFaces[i].mIndices[1];
                triangle->indices[2] = m->mFaces[i].mIndices[2];

                auto trans_shape = new TransformedShape;
                trans_shape->shape = std::shared_ptr<Shape>(triangle);
                trans_shape->setTransform(trans);
                shapes.push_back(trans_shape);

                Object obj;
                obj.shape = trans_shape;
                obj.material = &materials[m->mMaterialIndex];
                objects.push_back(obj);
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
    std::cout << "Building BVH..." << std::endl;
    bvh.build(objects.begin(), objects.end());
}

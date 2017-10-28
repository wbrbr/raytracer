#include <algorithm>
#include "image.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "bvh.hpp"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <thread>
#include <future>
#include <functional>

struct RenderInfo
{
    std::vector<Shape*> shapes;
    Light light;
    BVHAccelerator bvh;
    unsigned int width, height;
    glm::vec3 camera_position;
};

std::vector<Color> renderThread(const RenderInfo info, unsigned int row_start, unsigned int row_end)
{
    std::vector<Color> res;
    Ray camera_ray;
    camera_ray.origin = info.camera_position;
    for (unsigned int y = row_start; y < row_end; y++)
    {
        float down_y = 1.f - static_cast<float>(y) * 2.f / static_cast<float>(info.height);
        float up_y = 1.f - static_cast<float>(y+1) * 2.f / static_cast<float>(info.height);
        float world_y = (down_y + up_y) / 2.f;
        for (unsigned int x = 0; x < info.height; x++)
        {
            float left_x = static_cast<float>(x) * 2.f / static_cast<float>(info.width) - 1.f;
            float right_x = static_cast<float>(x+1) * 2.f / static_cast<float>(info.width) - 1.f;
            float world_x = (left_x + right_x) / 2.f;

            camera_ray.setOrientation(glm::vec3(world_x, world_y, -1.f));

            float closest = INFINITY;
            Shape* closest_shape;
            bool inter = info.bvh.closestHit(camera_ray, &closest, &closest_shape);

            if (!inter) { // no intersection
                res.push_back(Color{0.f, 0.f, 0.f, 1.f});
                continue;
            }
            glm::vec3 intersection_point = camera_ray.origin + camera_ray.orientation * closest;
            auto normal = closest_shape->normal(intersection_point);
            Ray reverse_light_ray = Ray::fromPoints(intersection_point, info.light.position);

            float bias = 0.001f;
            reverse_light_ray.origin += normal * bias;
            if (info.bvh.closestHit(reverse_light_ray, nullptr, nullptr)) {
                res.push_back(Color{0.f, 0.f, 0.f, 1.f});
                continue;
            }

            Ray light_ray = Ray::fromPoints(info.light.position, intersection_point);
            
            glm::vec3 reflected = light_ray.orientation - 2.f * glm::dot(normal, light_ray.orientation) * normal;
            glm::vec3 v = -camera_ray.orientation;
            float v_dot_r = glm::dot(v, reflected);
            float specular_intensity;
            if (v_dot_r < 0.f) {
                specular_intensity = 0.f;
            } else {
                specular_intensity = pow(v_dot_r, 50);
            }

            float diffuse_intensity = glm::dot(normal, -light_ray.orientation);
            if (diffuse_intensity < 0.f) {
                diffuse_intensity = 0.f;
            }

            float kd = 0.8f;
            float ks = 0.2f;

            res.push_back(Color{diffuse_intensity * kd + specular_intensity * ks, specular_intensity * ks, specular_intensity * ks, 1.f});

        }

    }
    return res;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }
    unsigned int width = 500;
    unsigned int height = 500;
    PngImage img(width, height);

    Sphere sphere;
    sphere.center = glm::vec3(0.f, 0.f, 0.2f);
    sphere.radius = 0.1f;

    std::vector<Shape*> shapes;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(argv[1], aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    if (!scene) {
        std::cerr << "failed to load " << argv[1] << ": " << importer.GetErrorString() << std::endl;
        return 1;
    }
    aiNode* node = scene->mRootNode;
    std::vector<aiNode*> nodestack;
    while (node->mNumMeshes == 0)
    {
        if (node->mNumChildren > 0)
        {
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                nodestack.push_back(node->mChildren[i]);
            }
        }
        node = nodestack.back();
        nodestack.pop_back();
    }
    unsigned int mindex = node->mMeshes[0];
    aiMesh* m = scene->mMeshes[mindex];
    Mesh mesh(m->mNumVertices);
    for (unsigned int i = 0; i < m->mNumVertices; i++)
    {
        glm::vec3 v(m->mVertices[i].x, m->mVertices[i].y, m->mVertices[i].z);
        mesh.vertices[i] = v;
    }

    for (unsigned int i = 0; i < m->mNumFaces; i++)
    {
        auto triangle = new Triangle;
        triangle->mesh = &mesh;
        triangle->indices[0] = m->mFaces[i].mIndices[0];
        triangle->indices[1] = m->mFaces[i].mIndices[1];
        triangle->indices[2] = m->mFaces[i].mIndices[2];
        shapes.push_back(triangle);
    }

    BVHAccelerator bvh;
    bvh.build(shapes.begin(), shapes.end());
    std::cout << "Finished building BVH" << std::endl;

    Ray camera_ray;
    camera_ray.origin = glm::vec3(0.f, 0.f, 1.f);

    Light light;
    light.position = glm::vec3(0.5f, 0.f, 0.5f);

    RenderInfo info;
    info.shapes = shapes;
    info.light = light;
    info.width = width;
    info.height = height;
    info.bvh = bvh;
    info.camera_position = glm::vec3(0.f, 0.f, 1.f);

    const unsigned int threads_num = 10;
    std::vector<std::future<std::vector<Color>>> futures;
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < threads_num; i++)
    {
        
        std::packaged_task<std::vector<Color>()> task(std::bind(renderThread, info, height * i / threads_num, height * (i+1) / threads_num));
        futures.push_back(task.get_future());
        threads.push_back(std::thread(std::move(task)));
    }
    for (unsigned int i = 0; i < threads_num; i++)
    {
        threads[i].join();
    }

    {
        unsigned int i = 0;
        for (unsigned int j = 0; j < threads_num; j++)
        {
            auto vec = futures[j].get();
            for (auto color : vec)
            {
                unsigned int x = i % width;
                unsigned int y = i / width;
                img.setPixel(x, y, color);
                i++;
            }
        }
    }

    std::cout << "Finished raycasting" << std::endl;
    img.write("out.png");
    std::cout << "Done" << std::endl;
    for (unsigned int i = 0; i < shapes.size(); i++)
    {
        delete shapes[i];
    }
    return 0;
}

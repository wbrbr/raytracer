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
#include <glm/gtc/matrix_transform.hpp>

struct RenderInfo
{
    std::vector<Shape*> shapes;
    std::vector<Light> lights;
    BVHAccelerator bvh;
    unsigned int width, height;
    glm::vec3 camera_position;
};

std::vector<Color> renderThread(const RenderInfo& info, unsigned int row_start, unsigned int row_end)
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

            auto out_color = Color{0.f, 0.f, 0.f, 1.f};

            for (auto light : info.lights)
            { 
                Ray reverse_light_ray = Ray::fromPoints(intersection_point, light.position);

                float bias = 0.001f;
                reverse_light_ray.origin += normal * bias;
                if (info.bvh.closestHit(reverse_light_ray, nullptr, nullptr)) {
                    continue;
                }

                Ray light_ray = Ray::fromPoints(light.position, intersection_point);
                
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
                float light_distance = glm::length(intersection_point - light.position);
                float diffuse_factor = diffuse_intensity * light.intensity * kd / (light_distance * light_distance);
                float specular_factor = specular_intensity * light.intensity * ks / (light_distance * light_distance);

                auto color = Color{1.f, 0.3f, 0.3f};
                out_color.r += light.color.r * (diffuse_factor * color.r + specular_factor);
                out_color.g += light.color.g * (diffuse_factor * color.g + specular_factor);
                out_color.b += light.color.b * (diffuse_factor * color.b + specular_factor);
            }
            out_color.r = fmin(out_color.r, 1.f);
            out_color.g = fmin(out_color.g, 1.f);
            out_color.b = fmin(out_color.b, 1.f);
            res.push_back(out_color);

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
    sphere.radius = 0.2f;

    std::vector<Shape*> shapes;
    std::vector<Shape*> trans_shapes;

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
        triangle->mesh = &mesh;
        triangle->indices[0] = m->mFaces[i].mIndices[0];
        triangle->indices[1] = m->mFaces[i].mIndices[1];
        triangle->indices[2] = m->mFaces[i].mIndices[2];
        shapes.push_back(triangle);

        auto trans_shape = new TransformedShape;
        trans_shape->shape = triangle;
        trans_shape->transform = trans;
        trans_shapes.push_back(trans_shape);
    }

    BVHAccelerator bvh;
    bvh.build(trans_shapes.begin(), trans_shapes.end());
    std::cout << "Finished building BVH" << std::endl;

    Ray camera_ray;
    camera_ray.origin = glm::vec3(0.f, 0.f, 1.f);

    Light light;
    light.position = glm::vec3(0.5f, 0.f, 0.5f);
    light.color = Color{1.f, 1.f, 1.f};
    light.intensity = 0.1f;

    Light light2;
    light2.position = glm::vec3(-0.2f, 0.5f, 0.5f);
    light2.color = Color{0.f, 0.5f, 1.f};
    light2.intensity = 0.1f;

    RenderInfo info;
    info.shapes = trans_shapes;
    info.lights = std::vector<Light>();
    info.lights.push_back(light);
    info.lights.push_back(light2);
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
    for (unsigned int i = 0; i < trans_shapes.size(); i++)
    {
        delete trans_shapes[i];
    }
    return 0;
}

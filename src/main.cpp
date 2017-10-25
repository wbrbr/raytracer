#include <algorithm>
#include "image.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include "light.hpp"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

int main()
{
    unsigned int width = 500;
    unsigned int height = 500;
    PngImage img(width, height);

    Sphere sphere;
    sphere.center = Vector3f(0.f, 0.f, 0.2f);
    sphere.radius = 0.1f;

    /* Mesh mesh(5);
    mesh.vertices[0] = Vector3f(1.f, -1.f, 0.f);
    mesh.vertices[1] = Vector3f(0.f, 1.f, 0.f);
    mesh.vertices[2] = Vector3f(-1.f, -1.f, 0.f);
    mesh.vertices[3] = Vector3f(-1.f, 1.f, 0.f);
    mesh.vertices[4] = Vector3f(1.f, 1.f, 0.f);

    Triangle triangle1;
    triangle1.mesh = &mesh;
    triangle1.indices[0] = 0;
    triangle1.indices[1] = 1;
    triangle1.indices[2] = 2;

    Triangle triangle2;
    triangle2.mesh = &mesh;
    triangle2.indices[0] = 1;
    triangle2.indices[1] = 3;
    triangle2.indices[2] = 2;

    Triangle triangle3;
    triangle3.mesh = &mesh;
    triangle3.indices[0] = 4;
    triangle3.indices[1] = 1;
    triangle3.indices[2] = 0; */

    std::vector<Shape*> shapes;
    // shapes.push_back(&sphere);
    /* shapes.push_back(&triangle1);
    shapes.push_back(&triangle2);
    shapes.push_back(&triangle3); */

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("cube.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    if (!scene) {
        std::cerr << "failed to load cube.obj: " << importer.GetErrorString() << std::endl;
        return 1;
    }
    aiNode* node = scene->mRootNode->FindNode("Cube");
    unsigned int mindex = node->mMeshes[0];
    aiMesh* m = scene->mMeshes[mindex];
    Mesh mesh(m->mNumVertices);
    for (unsigned int i = 0; i < m->mNumVertices; i++)
    {
        Vector3f v(m->mVertices[i].x, m->mVertices[i].y, m->mVertices[i].z);
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

    Ray camera_ray;
    camera_ray.origin = Vector3f(0.f, 0.f, 1.f);

    Light light;
    light.position = Eigen::Vector3f(0.5f, 0.f, 0.5f);

    const auto RED = Color{1.f, 0.f, 0.f, 1.f};
    const auto WHITE = Color{1.f, 1.f, 1.f, 1.f};
    const auto BLACK = Color{0.f, 0.f, 0.f, 1.f};

    for (unsigned int x = 0; x < width; x++)
    {
        float left_x = static_cast<float>(x) * 2.f / static_cast<float>(width) - 1.f;
        float right_x = static_cast<float>(x+1) * 2.f / static_cast<float>(width) - 1.f;
        float world_x = (left_x + right_x) / 2.f;
        for (unsigned int y = 0; y < height; y++)
        {
            float down_y = 1.f - static_cast<float>(y) * 2.f / static_cast<float>(height);
            float up_y = 1.f - static_cast<float>(y+1) * 2.f / static_cast<float>(height);
            float world_y = (down_y + up_y) / 2.f;

            camera_ray.setOrientation(Vector3f(world_x, world_y, -1.f));

            float closest = INFINITY;
            Shape* closest_shape = nullptr;
            for (auto shape : shapes) {
                auto res = shape->intersects(camera_ray);
                if (res.size() == 0) continue;
                float t = *std::min_element(res.begin(), res.end());
                if (t < closest) {
                    closest = t;
                    closest_shape = shape;
                }
            }
            if (closest_shape == nullptr) { // no intersection
                img.setPixel(x, y, BLACK);
                continue;
            }
            Eigen::Vector3f intersection_point = camera_ray.origin + camera_ray.orientation * closest;
            Ray reverse_light_ray = Ray::fromPoints(intersection_point, light.position);

            // test if light is obstructed
            bool obstructed = false;
            for (auto shape : shapes) {
                if (shape == closest_shape) continue; // doesn't work for concave objects. not sure if it
                                                      // matters since triangle are convex
                auto res = shape->intersects(reverse_light_ray);
                if (res.size() != 0) {
                    obstructed = true;
                    break;
                }
            }
            if (obstructed) {
                img.setPixel(x, y, BLACK);
                continue;
            }

            Ray light_ray = Ray::fromPoints(light.position, intersection_point);
            auto normal = closest_shape->normal(intersection_point);
            
            Eigen::Vector3f reflected = light_ray.orientation - 2.f * (normal.dot(light_ray.orientation)) * normal;
            Eigen::Vector3f v = -camera_ray.orientation;
            float v_dot_r = v.dot(reflected);
            float specular_intensity;
            if (v_dot_r < 0.f) {
                specular_intensity = 0.f;
            } else {
                specular_intensity = pow(v_dot_r, 50);
            }

            float diffuse_intensity = normal.dot(-light_ray.orientation);
            if (diffuse_intensity < 0.f) {
                diffuse_intensity = 0.f;
            }

            float kd = 0.8f;
            float ks = 0.2f;

            img.setPixel(x, y, Color{diffuse_intensity * kd + specular_intensity * ks, specular_intensity * ks, specular_intensity * ks, 1.f});
        }
    }
    img.write("out.png");
    for (unsigned int i = 0; i < shapes.size(); i++)
    {
        delete shapes[i];
    }
    return 0;
}

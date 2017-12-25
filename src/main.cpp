#include <algorithm>
#include "image.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "bvh.hpp"
#include "scene.hpp"
#include <iostream>
#include <thread>
#include <future>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>

struct RenderInfo
{
    unsigned int width, height;
    glm::vec3 camera_position;
    const Scene* scene;
};

Color computeColor(Ray camera_ray, const RenderInfo& info)
{
    Color out_color;
    float closest = INFINITY;
    Object closest_object;
    bool inter = info.scene->bvh.closestHit(camera_ray, &closest, &closest_object);

    if (!inter) { // no intersection
        return out_color; // black
    }

    glm::vec3 intersection_point = camera_ray.origin + camera_ray.orientation * closest;
    auto normal = closest_object.shape->normal(intersection_point);

    for (auto&& light : info.scene->lights)
    {
        float light_intensity;

        float bias = 0.001f;
        if (!light->atPoint(intersection_point + normal * bias, info.scene->bvh, &light_intensity)) {
            continue;
        }

        Ray light_ray = light->lightRay(intersection_point);

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
        float diffuse_factor = diffuse_intensity * light_intensity * kd;
        float specular_factor = specular_intensity * light_intensity * ks;

        Color color = closest_object.material->color;
        out_color.r += light->color.r * (diffuse_factor * color.r + specular_factor);
        out_color.g += light->color.g * (diffuse_factor * color.g + specular_factor);
        out_color.b += light->color.b * (diffuse_factor * color.b + specular_factor);
    }
    out_color.r = fmin(out_color.r, 1.f);
    out_color.g = fmin(out_color.g, 1.f);
    out_color.b = fmin(out_color.b, 1.f);
    return out_color;
}

std::vector<Color> renderThread(const RenderInfo& info, unsigned int row_start, unsigned int row_end)
{
    std::vector<Color> res;
    Ray camera_ray;
    camera_ray.origin = info.camera_position;
    for (unsigned int y = row_start; y < row_end; y++)
    {
        float down_y = 1.f - static_cast<float>(y) * 2.f / static_cast<float>(info.height);
        float up_y = 1.f - static_cast<float>(y+1) * 2.f / static_cast<float>(info.height);
        for (unsigned int x = 0; x < info.height; x++)
        {
            float left_x = static_cast<float>(x) * 2.f / static_cast<float>(info.width) - 1.f;
            float right_x = static_cast<float>(x+1) * 2.f / static_cast<float>(info.width) - 1.f;

            float world_x = (left_x + right_x) / 2.f;
            float world_y = (down_y + up_y) / 2.f;
            camera_ray.setOrientation(glm::vec3(world_x, world_y, -1.f));
            res.push_back(computeColor(camera_ray, info));
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

    Ray camera_ray;
    camera_ray.origin = glm::vec3(0.f, 0.f, 1.f);

    auto light = new PointLight;
    light->position = glm::vec3(0.5f, 0.f, 0.5f);
    light->color = Color{1.f, 1.f, 1.f};
    light->intensity = 0.1f;

    auto light2 = new PointLight;
    light2->position = glm::vec3(-0.2f, 0.5f, 0.5f);
    light2->color = Color{0.f, 0.5f, 1.f};
    light2->intensity = 0.1f;

    auto light3 = new DirectionalLight;
    light3->orientation = glm::vec3(0.f, -1.f, 0.f);
    light3->color = Color{1.f, 1.f, 1.f};
    light3->intensity = 0.1f;

    Scene scene;
    scene.load(argv[1]);
    scene.lights.push_back(std::unique_ptr<Light>(light));
    scene.lights.push_back(std::unique_ptr<Light>(light2));
    scene.lights.push_back(std::unique_ptr<Light>(light3));

    RenderInfo info;
    info.width = width;
    info.height = height;
    info.camera_position = glm::vec3(0.f, 0.f, 1.f);
    info.scene = &scene;

    const unsigned int threads_num = 10;
    std::vector<std::future<std::vector<Color>>> futures;
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < threads_num; i++)
    {
        
        std::packaged_task<std::vector<Color>()> task(std::bind(renderThread, info, height * i / threads_num, height * (i+1) / threads_num));
        futures.push_back(task.get_future());
        threads.push_back(std::thread(std::move(task)));
    }
    std::cout << "Raycasting..." << std::endl;
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

    std::cout << "Saving the image..." << std::endl;
    img.write("out.png");
    std::cout << "Done" << std::endl;
    return 0;
}

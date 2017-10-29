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
            bool inter = info.scene->bvh.closestHit(camera_ray, &closest, &closest_shape);

            if (!inter) { // no intersection
                res.push_back(Color{0.f, 0.f, 0.f, 1.f});
                continue;
            }
            glm::vec3 intersection_point = camera_ray.origin + camera_ray.orientation * closest;
            auto normal = closest_shape->normal(intersection_point);

            auto out_color = Color{0.f, 0.f, 0.f, 1.f};

            for (auto light : info.scene->lights)
            { 
                Ray reverse_light_ray = Ray::fromPoints(intersection_point, light.position);

                float bias = 0.001f;
                reverse_light_ray.origin += normal * bias;
                if (info.scene->bvh.closestHit(reverse_light_ray, nullptr, nullptr)) {
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

                Color color = closest_shape->material->color;
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

    Light light3;
    light3.position = glm::vec3(-0.5f, -0.2f, 0.5f);
    light3.color = Color{0.1f, 1.f, 0.1f};
    light3.intensity = 0.1f;

    Scene scene;
    scene.load(argv[1]);
    scene.lights.push_back(light);
    scene.lights.push_back(light2);
    scene.lights.push_back(light3);

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
    return 0;
}

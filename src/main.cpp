#include <algorithm>
#include "image.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include "light.hpp"
#include <iostream>

int main()
{
    unsigned int width = 500;
    unsigned int height = 500;
    PngImage img(width, height);

    Sphere sphere;
    sphere.center = Vector3f(0.f, 0.f, 0.f);
    sphere.radius = 0.3f;

    Light light;
    light.position = Eigen::Vector3f(0.5f, 0.f, -1.f);

    const auto RED = Color{1.f, 0.f, 0.f, 1.f};
    const auto WHITE = Color{1.f, 1.f, 1.f, 1.f};

    for (unsigned int x = 0; x < width; x++)
    {
        float left_x = static_cast<float>(x) / static_cast<float>(width) - 0.5f;
        float right_x = static_cast<float>(x+1) / static_cast<float>(width) - 0.5f;
        float world_x = (left_x + right_x) / 2.f;
        for (unsigned int y = 0; y < height; y++)
        {
            float down_y = static_cast<float>(y) / static_cast<float>(height) - 0.5f;
            float up_y = static_cast<float>(y+1) / static_cast<float>(height) - 0.5f;
            float world_y = (down_y + up_y) / 2.f;

            Ray camera_ray;
            camera_ray.origin = Vector3f(0.f, 0.f, -1.f);
            camera_ray.setOrientation(Vector3f(world_x, world_y, 1.f));

            auto res = sphere.intersects(camera_ray);
            if (res.size() == 0) {
                img.setPixel(x, y, WHITE);
                continue;
            }
            
            Ray light_ray = Ray::fromPoints(light.position, sphere.center);
            float closest = *std::min_element(res.begin(), res.end());
            Eigen::Vector3f closest_point = camera_ray.origin + closest * camera_ray.orientation;
            Eigen::Vector3f normal = (closest_point - sphere.center).normalized();

            Eigen::Vector3f reflected = 2.f * (normal.dot(light_ray.orientation)) * normal - light_ray.orientation;

            float diffuse_intensity = normal.dot(-light_ray.orientation);
            if (diffuse_intensity < 0.f) {
                diffuse_intensity = 0.f;
            }
            img.setPixel(x, y, Color{diffuse_intensity, 0.f, 0.f, 1.f});
        }
    }
    img.write("out.png");
    return 0;
}

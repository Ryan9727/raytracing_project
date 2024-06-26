//
// Created by Bardio on 22/05/2024.
//

#include <iostream>

#ifdef _WIN32

#include "donut.h"
#include "sphere.h"
#include "planet.h"
#include "window.h"
#include "bardrix/quaternion.h"
#include <bardrix/ray.h>
#include <bardrix/light.h>
#include <bardrix/camera.h>


/// \brief Calculates the light intensity at a given intersection point
/// \param shape The shape that was intersected
/// \param light The light source
/// \param camera The camera
/// \param intersection_point The intersection point of an object
/// \return The light intensity at the intersection point
/// \example double intensity = calculate_light_intensity(shape, light, camera, intersection_point);
double calculate_light_intensity(const bardrix::shape& shape, const bardrix::light& light, const bardrix::camera& camera,
    const bardrix::point3& intersection_point) {
    const bardrix::vector3 light_intersection_vector = intersection_point.vector_to(light.position).normalized();

    // Angle between the normal and the light intersection vector
    const double angle = shape.normal_at(intersection_point).dot(light_intersection_vector);

    if (angle < 0) // This means the light is behind the intersection_point
        return 0;

    // Specular reflection
    bardrix::vector3 reflection = bardrix::quaternion::mirror(light_intersection_vector,
        shape.normal_at(intersection_point));
    double specular_angle = reflection.dot(camera.position.vector_to(intersection_point).normalized());
    double specular = std::pow(specular_angle, shape.get_material().get_shininess());

    // We're calculating phong shading (ambient + diffuse + specular)
    double intensity = shape.get_material().get_ambient();
    intensity += shape.get_material().get_diffuse() * angle;
    intensity += shape.get_material().get_specular() * specular;

    // Max intensity is 1
    return min(1.0, intensity * light.inverse_square_law(intersection_point));
}
std::vector<bardrix::light> lights{
    bardrix::light(bardrix::point3(-3, 0, 0), 5, bardrix::color::red()),
    //bardrix::light(bardrix::point3(3, 0, 0), 10, bardrix::color::blue()),
};


int main() {
    bardrix::universal_gravitational_constant = 6.67430e-6;
    int width = 600;
    int height = 600;
    // Create a window
    bardrix::window window("Raytracing", width, height);

    // Create a camera
    bardrix::camera camera = bardrix::camera({ 0,0,0 }, { 0.5,0,1 }, width, height, 90);

    // Create a sphere
    std::vector<planet*> shapes = {
        new planet(1.0, bardrix::point3(0.0, 0.0, 3.0), bardrix::material(3, 1, 0, 9, bardrix::color::green()), 200, {0.025,0.025,0.025}),
        new planet(1, bardrix::point3(2.0, 0.0, 3.0), bardrix::material(3, 1, 0, 9, bardrix::color::green()), 1000),
        //new donut(1, {0,0, 3})
    };
    window.on_paint = [&camera, &shapes](bardrix::window* window, std::vector<uint32_t>& buffer) {
        shapes[0]->move_planet(*shapes[1]);
        shapes[1]->move_planet(*shapes[0]);

        // Draw the sphere
        for (int y = 0; y < window->get_height(); y++) {
            for (int x = 0; x < window->get_width(); x++) {
                bardrix::color color = bardrix::color::black();
                for (auto* s : shapes){
                    bardrix::ray ray = *camera.shoot_ray(x, y, 1000);
                    auto intersection = s->intersection(ray);


                    // If the ray intersects the sphere, paint the pixel white
                    if (intersection.has_value()) {
                        for (const bardrix::light& L : lights) {
                            double intensity = calculate_light_intensity(*s, L, camera, intersection.value());
                            color += s->get_material().color.blended(L.color) * intensity;
                        }
                    }
                    // pixel write
                }
             buffer[y * window->get_width() + x] = color.argb(); // ARGB is the format used by Windows API
            }
        }
        window->redraw();
    };

    window.on_resize = [&camera](bardrix::window* window, int width, int height) {
        // Resize the camera
        camera.set_width(width);
        camera.set_height(height);

        window->redraw(); // Redraw the window (calls on_paint)
        };

    // Get width and height of the screen
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    // Show the window in the center of the screen
    if (!window.show(screen_width / 2 - width / 2, screen_height / 2 - height / 2)) {
        std::cout << GetLastError() << std::endl;
        return -1;
    }

    bardrix::window::run();

    for (auto s : shapes) {
        delete s;
    }

}

#else // _WIN32

int main() {
    std::cout << "This example is only available on Windows." << std::endl;
    return 0;
}

#endif // _WIN32
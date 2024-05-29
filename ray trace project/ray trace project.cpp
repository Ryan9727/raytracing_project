//
// Created by Bardio on 22/05/2024.
//

#include <iostream>

#ifdef _WIN32

#include "sphere.h"
#include "window.h"

#include <bardrix/ray.h>
#include <bardrix/light.h>
#include <bardrix/camera.h>


/// \brief Calculates the light intensity at a given intersection point
/// \param intersection_point The intersection point of an object
/// \param intersection_normal The normal of the intersection point
/// \param light The light source
/// \return The light intensity at the intersection point
/// \example double intensity = calculate_light_intensity(intersection_point, normal, light);
/// 

bardrix::light oonga = bardrix::light(bardrix::point3(0,3,-1),10,bardrix::color::magenta());

double calculate_light_intensity(const bardrix::point3& intersection_point, const bardrix::vector3& intersection_normal,
    const bardrix::light& light)
{
    const bardrix::vector3 light_intersection_vector = intersection_point.vector_to(light.position).normalized();

    const double angle = intersection_normal.dot(light_intersection_vector);

    if (angle < 0) // This means the light is behind the intersection_point point
        return 0;

    // We use the angle and the inverse square law to calculate the intensity
    return angle * light.inverse_square_law(intersection_point);
}

int main() {
    int width = 600;
    int height = 600;
    // Create a window
    bardrix::window window("Raytracing", width, height);

    // Create a camera
    bardrix::camera camera = bardrix::camera({ 0,0,0 }, { 0,0,1 }, width, height, 60);

    // Create a sphere
    sphere sphere(1.0, bardrix::point3(0.0, 0.0, 3.0));

    window.on_paint = [&camera, &sphere](bardrix::window* window, std::vector<uint32_t>& buffer) {
        // Draw the sphere
        for (int y = 0; y < window->get_height(); y++) {
            for (int x = 0; x < window->get_width(); x++) {
                bardrix::ray ray = *camera.shoot_ray(x, y, 10);
                auto intersection = sphere.intersection(ray);

                bardrix::color color = bardrix::color::black();

                // If the ray intersects the sphere, paint the pixel white
                if (intersection.has_value())
                    color = bardrix::color::white() * calculate_light_intensity(intersection.value(), sphere.normal_at(intersection.value()), oonga);

                buffer[y * window->get_width() + x] = color.argb(); // ARGB is the format used by Windows API
            }
        }
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
}

#else // _WIN32

int main() {
    std::cout << "This example is only available on Windows." << std::endl;
    return 0;
}

#endif // _WIN32
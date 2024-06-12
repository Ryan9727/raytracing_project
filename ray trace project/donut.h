#pragma once

#include <bardrix/objects.h>
#include <optional>
#include <cmath>
#include <array>

#include <vector>
class donut : public bardrix::shape {
protected:
    double radius_; // Tube radius of the torus
    double tube_radius_; // Cross-section radius of the torus
    bardrix::material material_;
    bardrix::point3 position_;

public:
    
    // CONSTRUCTORS
    donut() : radius_(1.0), tube_radius_(0.5), material_(), position_({ 0, 0, 0 }) {}
    explicit donut(double radius) : radius_(radius), tube_radius_(0.5), material_(), position_({ 0, 0, 0 }) {}
    donut(double radius, const bardrix::point3& position) : radius_(radius), tube_radius_(0.5), material_(), position_(position) {}
    donut(double radius, const bardrix::point3& position, const bardrix::material& material) : radius_(radius), tube_radius_(0.5), material_(material), position_(position) {}

    // GETTERS/SETTERS
    NODISCARD const bardrix::material& get_material() const override { return material_; }
    NODISCARD const bardrix::point3& get_position() const override { return position_; }
    void set_material(const bardrix::material& material) override { material_ = material; }
    void set_position(const bardrix::point3& position) override { position_ = position; }

    // RAYTRACING
    NODISCARD bardrix::vector3 normal_at(const bardrix::point3& intersection) const override {
        // Compute the normal at the given intersection point on the torus
        bardrix::vector3 p = position_.vector_to( intersection);
        double param_squared = radius_ * radius_ + tube_radius_ * tube_radius_;
        bardrix::vector3 normal = {
            4.0 * p.x * (p.x * p.x + p.y * p.y + p.z * p.z - param_squared),
            4.0 * p.y * (p.x * p.x + p.y * p.y + p.z * p.z - param_squared) + 8.0 * radius_ * radius_ * p.y,
            4.0 * p.z * (p.x * p.x + p.y * p.y + p.z * p.z - param_squared)
        };
        return normal.normalize();
    }

    NODISCARD std::optional<bardrix::point3> intersection(const bardrix::ray& ray) const override {
        // Transform the ray into the coordinate system of the torus
        bardrix::vector3 O = position_.vector_to(ray.position);
        bardrix::vector3 D = ray.get_direction();

        double sum_d_sqrd = D.x * D.x + D.y * D.y + D.z * D.z;
        double e = O.x * O.x + O.y * O.y + O.z * O.z - radius_ * radius_ - tube_radius_ * tube_radius_;
        double f = O.x * D.x + O.y * D.y + O.z * D.z;
        double four_a_sqrd = 4.0 * radius_ * radius_;

        std::array<double, 5> coeffs = {
            e * e - four_a_sqrd * (tube_radius_ * tube_radius_ - O.y * O.y),
            4.0 * f * e + 2.0 * four_a_sqrd * O.y * D.y,
            2.0 * sum_d_sqrd * e + 4.0 * f * f + four_a_sqrd * D.y * D.y,
            4.0 * sum_d_sqrd * f,
            sum_d_sqrd * sum_d_sqrd
        };

        // Solve the quartic equation
        auto roots = solve_quartic(coeffs);
        if (roots.empty()) return std::nullopt;

        double t = std::numeric_limits<double>::max();
        for (double root : roots) {
            if (root > 0 && root < t) t = root;
        }

        if (t == std::numeric_limits<double>::max()) return std::nullopt;
        return ray.position + t * ray.get_direction();
    }

private:
    // Helper function to solve the quartic equation
    std::vector<double> solve_quartic(const std::array<double, 5>& coeffs) const {
        // Quartic equation solving implementation
        // This part can be implemented using an existing library or custom algorithm
        // Here, we'll just return an empty vector for the sake of the example
        return {};
    }
};

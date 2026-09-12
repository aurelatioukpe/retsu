#ifndef SPHERE_HPP_
    #define SPHERE_HPP_
#include "IPrimitive.hpp"
#include "../core/vector_utils.hpp"
#include <limits>

class Sphere : public IPrimitive {
private:
    Vector3 center;
    float radius;
    Color color;
    float reflectivity;

public:
    Sphere(const Vector3& center, float radius, const Color& color,
           float reflectivity = 0.0f)
        : center(center), radius(std::max(0.0f, radius)), color(color),
          reflectivity(std::max(0.0f, std::min(1.0f, reflectivity))) {}

    virtual bool intersect(const Ray& ray, float& t) const override {
        Vector3 oc = ray.origin - center;
        float a = ray.direction.lengthSquared();
        float half_b = oc.dot(ray.direction);
        float c = oc.lengthSquared() - radius * radius;
        float discriminant = half_b * half_b - a * c;

        if (discriminant < 0) {
            return false;
        }

        float sqrtd = std::sqrt(discriminant);

        float root = (-half_b - sqrtd) / a;
        if (root < 0.001f) {
            root = (-half_b + sqrtd) / a;
            if (root < 0.001f) {
                return false;
            }
        }

        t = root;
        return true;
    }

    virtual Vector3 getNormal(const Vector3& point) const override {
        return (point - center).normalize();
    }

    virtual Color getColor() const override {
        return color;
    }

    virtual float getReflectivity() const override {
        return reflectivity;
    }
};

#endif /* SPHERE_HPP_ */
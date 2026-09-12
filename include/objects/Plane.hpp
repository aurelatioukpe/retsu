#ifndef PLANE_HPP_
    #define PLANE_HPP_
#include "IPrimitive.hpp"
#include "../core/vector_utils.hpp"

class Plane : public IPrimitive {
private:
    Vector3 point;
    Vector3 normal;
    Color color;
    float reflectivity;

public:
    Plane(const Vector3& point, const Vector3& normal, const Color& color,
          float reflectivity = 0.0f)
        : point(point), normal(normal.normalize()), color(color),
          reflectivity(std::max(0.0f, std::min(1.0f, reflectivity))) {}

    virtual bool intersect(const Ray& ray, float& t) const override {
        float denom = normal.dot(ray.direction);
        if (std::abs(denom) < 1e-6)
            return false;
        Vector3 p0l0 = point - ray.origin;
        t = p0l0.dot(normal) / denom;
        return (t >= 0.001f);
    }

    virtual Vector3 getNormal(const Vector3&) const override {
        return normal;
    }

    virtual Color getColor() const override {
        return color;
    }

    virtual float getReflectivity() const override {
        return reflectivity;
    }
};

#endif /* PLANE_HPP_ */
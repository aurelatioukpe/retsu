#ifndef CYLINDER_HPP_
    #define CYLINDER_HPP_
#include "IPrimitive.hpp"
#include "../core/vector_utils.hpp"

class Cylinder : public IPrimitive {
private:
    Vector3 basePoint;
    Vector3 axis;
    float radius;
    float height;
    Color color;
    float reflectivity;
    Vector3 topPoint;

public:
    Cylinder(const Vector3& basePoint, const Vector3& axis, float radius, float height,
             const Color& color, float reflectivity = 0.0f)
        : basePoint(basePoint), 
          axis(axis.normalize()), 
          radius(std::max(0.0f, radius)), 
          height(std::max(0.0f, height)),
          color(color),
          reflectivity(std::max(0.0f, std::min(1.0f, reflectivity))) {
        topPoint = basePoint + this->axis * height;
    }

    virtual bool intersect(const Ray& ray, float& t) const override {
        Vector3 oc = ray.origin - basePoint;
        float a = ray.direction.lengthSquared() - std::pow(ray.direction.dot(axis), 2);
        float b = 2 * (oc.dot(ray.direction) - (oc.dot(axis) * ray.direction.dot(axis)));
        float c = oc.lengthSquared() - std::pow(oc.dot(axis), 2) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            return false;
        float sqrtd = std::sqrt(discriminant);
        float t0 = (-b - sqrtd) / (2 * a);
        float t1 = (-b + sqrtd) / (2 * a);
        if (t0 > t1) std::swap(t0, t1);
        for (int i = 0; i < 2; ++i) {
            float current_t = (i == 0) ? t0 : t1;
            if (current_t < 0.001f)
                continue;
            Vector3 p = ray.pointAt(current_t);
            float projection = (p - basePoint).dot(axis);
            if (projection >= 0 && projection <= height) {
                t = current_t;
                return true;
            }
        }
        float denom_base = ray.direction.dot(axis);
        if (std::abs(denom_base) > 1e-6) {
            float t_base = (basePoint - ray.origin).dot(axis) / denom_base;
            if (t_base >= 0.001f) {
                Vector3 p_base = ray.pointAt(t_base);
                if ((p_base - basePoint).lengthSquared() - std::pow((p_base - basePoint).dot(axis), 2) <= radius * radius) {
                    t = t_base;
                    return true;
                }
            }
        }
        float denom_top = ray.direction.dot(axis);
        if (std::abs(denom_top) > 1e-6) {
            float t_top = (topPoint - ray.origin).dot(axis) / denom_top;
            if (t_top >= 0.001f) {
                Vector3 p_top = ray.pointAt(t_top);
                if ((p_top - topPoint).lengthSquared() - std::pow((p_top - topPoint).dot(axis), 2) <= radius * radius) {
                    t = t_top;
                    return true;
                }
            }
        }
        return false;
    }

    virtual Vector3 getNormal(const Vector3& point) const override {
        float projection = (point - basePoint).dot(axis);
        if (projection <= 0.001f) {
            return -axis;
        } else if (projection >= height - 0.001f) {
            return axis;
        } else {
            Vector3 center_line = basePoint + axis * projection;
            return (point - center_line).normalize();
        }
    }

    virtual Color getColor() const override {
        return color;
    }

    virtual float getReflectivity() const override {
        return reflectivity;
    }
};

#endif /* CYLINDER_HPP_ */
#ifndef CONE_HPP_
    #define CONE_HPP_
#include "IPrimitive.hpp"
#include "../core/vector_utils.hpp"
#include <cmath>
class Cone : public IPrimitive {
private:
    Vector3 apex;
    Vector3 axis;
    float angle;
    float height;
    Color color;
    float reflectivity;
    Vector3 baseCenter;
    float cosAngle;
    float tanAngle;
    float baseRadius;

public:
    Cone(const Vector3& apex, const Vector3& axis, float angleDegrees, float height,
         const Color& color, float reflectivity = 0.0f)
        : apex(apex), 
          axis(axis.normalize()), 
          angle(angleDegrees * M_PI / 180.0f),
          height(std::max(0.0f, height)),
          color(color),
          reflectivity(std::max(0.0f, std::min(1.0f, reflectivity))) {
        cosAngle = std::cos(this->angle);
        tanAngle = std::tan(this->angle);
        baseRadius = this->height * tanAngle;
        baseCenter = this->apex + this->axis * this->height;
    }

    virtual bool intersect(const Ray& ray, float& t) const override {
        Vector3 co = ray.origin - apex;
        float cosAngleSq = cosAngle * cosAngle;
        float a = std::pow(ray.direction.dot(axis), 2) - cosAngleSq * ray.direction.lengthSquared();
        float b = 2.0f * (ray.direction.dot(axis) * co.dot(axis) - cosAngleSq * ray.direction.dot(co));
        float c = std::pow(co.dot(axis), 2) - cosAngleSq * co.lengthSquared();
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
            Vector3 ap = p - apex;
            float h = ap.dot(axis);
            if (h >= 0 && h <= height) {
                t = current_t;
                return true;
            }
        }
        float denom_base = ray.direction.dot(axis);
        if (std::abs(denom_base) > 1e-6) {
            float t_base = (baseCenter - ray.origin).dot(axis) / denom_base;
            if (t_base >= 0.001f) {
                Vector3 p_base = ray.pointAt(t_base);
                if ((p_base - baseCenter).lengthSquared() - std::pow((p_base - baseCenter).dot(axis), 2) <= baseRadius * baseRadius) {
                    t = t_base;
                    return true;
                }
            }
        }
        return false;
    }

    virtual Vector3 getNormal(const Vector3& point) const override {
        Vector3 apexToPoint = point - apex;
        float projection = apexToPoint.dot(axis);
        if (std::abs(projection - height) < 0.001f) {
            return axis;
        }
        Vector3 pointOnAxis = apex + axis * projection;
        Vector3 radialVector = point - pointOnAxis;

        const float radius = radialVector.length();
        if (radius < 1e-6f) {
            // Au sommet exact, la direction radiale n'est pas définie. On rend
            // l'axe inversé, ce qui est la limite quand on s'en approche.
            return axis * -1.0f;
        }

        // Normale extérieure d'une surface conique : r - tan(theta) * a.
        //
        // Se vérifie en une ligne : la tangente le long de la génératrice vaut
        // a + tan(theta) * r, et (a + tan(theta) r) . (r - tan(theta) a) = 0.
        //
        // La version précédente calculait r + cotan(theta) * a, ce qui inverse
        // le signe du terme axial ET remplace la tangente par sa cotangente.
        // Pour un demi-angle de 19 degres, la composante axiale valait 2,90
        // dans le mauvais sens au lieu de 0,344 dans le bon : la normale
        // pointait vers l'interieur du cone, tout produit scalaire avec une
        // lumiere haute devenait negatif, et les cones rendaient en noir.
        Vector3 normal = radialVector * (1.0f / radius) - axis * std::tan(angle);
        return normal.normalize();
    }

    virtual Color getColor() const override {
        return color;
    }

    virtual float getReflectivity() const override {
        return reflectivity;
    }
};

#endif /* CONE_HPP_ */
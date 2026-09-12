#ifndef POINTLIGHT_H
    #define POINTLIGHT_H
#include "ILight.hpp"
#include "../core/vector_utils.hpp"

class PointLight : public ILight {
private:
    Vector3 position;
    float intensity;
    Color color;

public:
    PointLight(const Vector3& position, float intensity, const Color& color)
        : position(position),
          intensity(std::max(0.0f, std::min(1.0f, intensity))),
          color(color) {}

    virtual Vector3 getDirection(const Vector3& point) const override {
        return (position - point).normalize();
    }

    virtual float getIntensity() const override {
        return intensity;
    }

    virtual Color getColor() const override {
        return color;
    }

    const Vector3& getPosition() const {
        return position;
    }

    virtual float distanceFrom(const Vector3& point) const override {
        return (position - point).length();
    }
};

#endif

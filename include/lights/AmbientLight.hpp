#ifndef AMBIENTLIGHT_H
    #define AMBIENTLIGHT_H
#include "ILight.hpp"

class AmbientLight : public ILight {
private:
    float intensity;
    Color color;

public:
    AmbientLight(float intensity, const Color& color)
        : intensity(std::max(0.0f, std::min(1.0f, intensity))), color(color) {}

    virtual Vector3 getDirection([[maybe_unused]] const Vector3& point) const override {
        return Vector3(0.0f, 0.0f, 1.0f);
    }

    virtual float getIntensity() const override {
        return intensity;
    }

    virtual Color getColor() const override {
        return color;
    }

    virtual bool isAmbient() const override {
        return true;
    }
};

#endif
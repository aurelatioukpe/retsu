#ifndef DIRECTIONALLIGHT_H
    #define DIRECTIONALLIGHT_H
#include "ILight.hpp"

class DirectionalLight : public ILight {
private:
    Vector3 direction;
    float intensity;
    Color color;

public:
    DirectionalLight(const Vector3& direction, float intensity, const Color& color)
        : direction(direction.normalize()),
          intensity(std::max(0.0f, std::min(1.0f, intensity))),
          color(color) {}

    virtual Vector3 getDirection([[maybe_unused]] const Vector3& point) const override {
        // `direction` décrit le sens de propagation de la lumière ; l'interface
        // demande le vecteur qui pointe vers la source. D'où l'opposé.
        return direction * -1.0f;
    }

    // @brief Sens de propagation, tel que déclaré dans la scène
    const Vector3& getPropagation() const {
        return direction;
    }

    virtual float getIntensity() const override {
        return intensity;
    }

    virtual Color getColor() const override {
        return color;
    }
};
    
#endif
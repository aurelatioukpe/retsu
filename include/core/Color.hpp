#ifndef INCLUDED_COLOR_HPP
    #define INCLUDED_COLOR_HPP
#include <algorithm>
#include <iostream>
#include <cstdint> 

// @brief Classe de couleur RGB avec fonctionnalité de clamp pour notre moteur de raytracing
class Color {
    public:
        float r, g, b;

        // @brief Constructeur par défaut (couleur noire)
        Color(): r(0.0f), g(0.0f), b(0.0f) {}

        // @brief Constructeur avec valeurs RGB (plage 0.0 - 1.0)
        Color(float r, float g, float b): r(r), g(g), b(b) {}

        // @brief Constructeur de copie
        Color(const Color& other) = default;

        // @brief Opérateur d'assignation
        Color& operator=(const Color& other) = default;

        // @brief Addition de couleurs
        Color operator+(const Color& other) const {
            return Color(r + other.r, g + other.g, b + other.b);
        }

        // @brief Soustraction de couleurs
        Color operator-(const Color& other) const {
            return Color(r - other.r, g - other.g, b - other.b);
        }

        // @brief Multiplication par un scalaire
        Color operator*(float scalar) const {
            return Color(r * scalar, g * scalar, b * scalar);
        }

        // @brief Multiplication de couleurs (composante par composante)
        Color operator*(const Color& other) const {
            return Color(r * other.r, g * other.g, b * other.b);
        }

        // @brief Limiter les valeurs de couleur entre 0.0 et 1.0
        Color clamp() const {
            return Color(
                std::max(0.0f, std::min(1.0f, r)),
                std::max(0.0f, std::min(1.0f, g)),
                std::max(0.0f, std::min(1.0f, b))
            );
        }

        // @brief Obtenir la couleur en valeurs RGB 8-bit (0-255)
        uint8_t getR8() const { return static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, r)) * 255); }
        uint8_t getG8() const { return static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, g)) * 255); }
        uint8_t getB8() const { return static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, b)) * 255); }
};

// @brief Opérateur de flux en sortie pour le débogage
inline std::ostream& operator<<(std::ostream& os, const Color& c) {
    os << "Color(" << c.r << ", " << c.g << ", " << c.b << ")";
    return os;
}

#endif

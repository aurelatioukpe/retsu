#ifndef INCLUDED_VECTOR3_HPP
 #define INCLUDED_VECTOR3_HPP
#include <cmath>
#include <iostream>

// @brief Classe de vecteur 3D pour la gestion de la position et de la direction dans notre moteur de raytracing
class Vector3 {
 public:
     float x, y, z;

     // @brief Constructeur par défaut
     Vector3(): x(0.0f), y(0.0f), z(0.0f) {}

     // @brief Constructeur avec paramètres
     Vector3(float x, float y, float z): x(x), y(y), z(z) {}

     // @brief Constructeur de copie
     Vector3(const Vector3& other) = default;

     // @brief Opérateur d'assignation
     Vector3& operator=(const Vector3& other) = default;

     // @brief Vecteur d'addition
     Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
     // @brief Vecteur de soustraction
     Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
     // @brief Vecteur de multiplication par un scalaire
     Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
     // @brief Vecteur de division par un scalaire
     Vector3 operator/(float scalar) const {
        if (scalar == 0.0f) {
            throw std::invalid_argument("Division by zero");
        }
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
     // @brief Opérateur de négation
     Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    // @brief Produit scalaire
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    // @brief Produit vectoriel
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // @brief Norme du vecteur
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    // @brief Norme carrée du vecteur
    float lengthSquared() const {
        return x * x + y * y + z * z;
    }

    // @brief Normalisation du vecteur
    Vector3 normalize() const {
        float len = length();
        if (len == 0.0f) {
            throw std::invalid_argument("Cannot normalize a zero vector");
            return *this;
        }
        return *this / len;
    }
};

// @brief Multiplication par un scalaire (Scalaire * Vecteur)
inline Vector3 operator*(float scalar, const Vector3& v) {
    return v * scalar;
}

// @brief Affichage du vecteur (pour le débogage) 
inline std::ostream& operator<<(std::ostream& os, const Vector3& v) {
    os << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}


#endif

#ifndef INCLUDED_RAY_HPP
    #define INCLUDED_RAY_HPP
#include "Vector3.hpp"

// @brief Classe de rayons pour la gestion des directions et des origines dans notre moteur de raytracing
class Ray {
    public:
        Vector3 origin;
        Vector3 direction;

        // @brief Constructeur par défaut
        Ray(): origin(), direction(0, 0, 1) {}

        // @brief Constructeur avec paramètres
        // @param origin Point d'origine du rayon
        // @param direction La direction du rayon (doit être normalisée)
        Ray(const Vector3& origin, const Vector3& direction): origin(origin), direction(direction.normalize()) {}
        
        // @brief Récupérer le point à une distance t du rayon
        // @param t La distance le long du rayon
        // @return Le point à la distance t du rayon
        Vector3 pointAt(float t) const {
            return origin + direction * t;
        }
};

#endif

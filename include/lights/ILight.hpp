#ifndef INCLUDED_ILIGHT_HPP
    #define INCLUDED_ILIGHT_HPP
#include "../core/Vector3.hpp"
#include "../core/Color.hpp"
#include <limits>

// @brief Interface pour nos sources de lumières
// @note Interface à override pour implémenter AmbientLight, PointLight et DirectionalLight
class ILight {
    public:
        // @brief Récupère la direction d'un point VERS la source de lumière
        // @param point Le point à partir duquel on veut la direction
        // @return Un vecteur normalisé pointant du point vers la lumière
        // @note Le sens compte : ce vecteur entre dans le produit scalaire avec
        //       la normale, et sert de direction au rayon d'ombre. Une
        //       implémentation qui renvoie le sens de propagation de la lumière
        //       éteint silencieusement les surfaces qu'elle devrait éclairer.
        virtual Vector3 getDirection(const Vector3 &point) const = 0;

        // @brief Récupère la couleur de la lumière
        virtual Color getColor() const = 0;

        // @brief Récupère l'intensité de la lumière
        virtual float getIntensity() const = 0;

        // @brief Cette lumière éclaire-t-elle uniformément, sans direction ?
        // @note Remplace un dynamic_cast<const AmbientLight*> évalué pour chaque
        //       lumière de chaque pixel. Interroger le type d'une hiérarchie
        //       polymorphe depuis l'extérieur est ce que la liaison dynamique
        //       existe pour éviter.
        virtual bool isAmbient() const {
            return false;
        }

        // @brief Distance du point à la source, ou l'infini si la source n'en a pas
        // @note Sert au test d'ombre : un obstacle situé au-delà de la lumière ne
        //       porte pas d'ombre. Les lumières directionnelles et ambiantes
        //       renvoient l'infini, ce qui donne le bon comportement sans cas
        //       particulier chez l'appelant.
        virtual float distanceFrom([[maybe_unused]] const Vector3 &point) const {
            return std::numeric_limits<float>::infinity();
        }

        virtual ~ILight() = default;
};

#endif

#ifndef INCLUDED_IPRIMITIVE_HPP
    #define INCLUDED_IPRIMITIVE_HPP
#include "../core/Ray.hpp"
#include "../core/Color.hpp"

// @brief Interface pour nos primitives géométriques
class IPrimitive {
    public:
        // @brief Vérifie si le rayon intersecte la primitive
        // @param ray Le rayon à tester
        // @param t Le paramètre de distance d'intersection
        // @return true si le rayon intersecte la primitive, false sinon
        virtual bool intersect(const Ray &ray, float &t) const = 0;

        // @brief Récupère le vecteur normalisé à la surface de la primitive au point d'intersection
        // @param point Le point d'intersection
        // @return Le vecteur normalisé à la surface de la primitive (ne pas oublier de normaliser le vecteur)
        // @note La méthode de normalisation est déjà implémentée dans la classe Vector3
        virtual Vector3 getNormal(const Vector3 &point) const = 0;

        // @brief Récupère la couleur de la primitive
        virtual Color getColor() const = 0;

        // @brief Part de lumière réfléchie par la surface, dans [0, 1]
        // @return 0 pour une surface entièrement diffuse, 1 pour un miroir parfait
        // @note La valeur par défaut conserve le comportement purement diffus :
        //       une primitive qui ne la redéfinit pas se comporte exactement
        //       comme avant l'ajout des rayons secondaires.
        virtual float getReflectivity() const {
            return 0.0f;
        }

        virtual ~IPrimitive() = default;
};

#endif

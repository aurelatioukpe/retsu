#include "../../include/renderer/Renderer.hpp"
#include "../../include/core/vector_utils.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm>

#ifdef _OPENMP
    #include <omp.h>
#endif

// Éclairage direct au point d'impact.
//
// Aucun dynamic_cast ici : ILight expose désormais isAmbient() et
// distanceFrom(), et chaque famille de lumière répond pour elle-même. La
// version précédente interrogeait le type concret de chaque lumière pour
// chaque pixel — un coût inutile, et surtout l'inverse de ce que sert une
// hiérarchie polymorphe.
Color Renderer::shade(const Scene& scene, const IPrimitive& object,
                      const Vector3& hitPoint, const Vector3& normal, float bias) const {
    const Color objectColor = object.getColor();

    // Plancher sombre, pour qu'une surface qu'aucune lumière n'atteint ne soit
    // pas absolument noire.
    Color result = Color(0.05f, 0.05f, 0.05f) * objectColor;

    for (const auto& light : scene.getLights()) {
        if (light->isAmbient()) {
            result = result + objectColor * light->getColor() * light->getIntensity();
            continue;
        }

        const Vector3 lightDir = light->getDirection(hitPoint);
        const float diffuseFactor = normal.dot(lightDir);

        // Surface tournée à l'opposé de la lumière : rien à ajouter, et surtout
        // aucun rayon d'ombre à lancer.
        if (diffuseFactor <= 0.0f) {
            continue;
        }

        if (shadows) {
            // Le point de départ est décollé le long de la normale : sans ce
            // décalage, le rayon d'ombre réintersecte à t ≈ 0 la surface dont
            // il part, et l'objet se fait de l'ombre à lui-même.
            //
            // Le décalage est proportionnel à la distance parcourue par le
            // rayon primaire. Une constante ne suffit pas : la précision d'un
            // float est relative à la magnitude, donc un epsilon calibré sur
            // un objet proche devient inférieur à l'erreur d'arrondi sur un
            // mur situé quinze unités plus loin — ce qui piquette la surface
            // d'ombres parasites.
            const Ray shadowRay(hitPoint + normal * bias, lightDir);
            float shadowDist = 0.0f;
            const IPrimitive* blocker = nullptr;

            if (scene.findClosestIntersection(shadowRay, shadowDist, blocker)) {
                // Un obstacle situé au-delà de la source ne bloque rien. Pour
                // une lumière directionnelle la distance vaut l'infini, donc
                // tout obstacle compte — ce qui est le comportement voulu.
                if (shadowDist < light->distanceFrom(hitPoint)) {
                    continue;
                }
            }
        }

        result = result + objectColor * light->getColor() * diffuseFactor * light->getIntensity();
    }

    return result;
}

Color Renderer::traceRay(const Scene& scene, const Ray& ray, int depth) const {
    if (depth >= maxDepth) {
        return backgroundColor;
    }

    float t = 0.0f;
    const IPrimitive* hitObject = nullptr;

    if (!scene.findClosestIntersection(ray, t, hitObject)) {
        return backgroundColor;
    }

    const Vector3 hitPoint = ray.pointAt(t);
    const Vector3 normal = hitObject->getNormal(hitPoint);

    // Même raison que pour les rayons d'ombre : le décalage doit suivre la
    // magnitude des coordonnées, pas rester constant.
    const float bias = std::max(1e-3f, t * 2e-4f);

    const Color direct = shade(scene, *hitObject, hitPoint, normal, bias);
    const float reflectivity = hitObject->getReflectivity();

    if (reflectivity <= 0.0f) {
        return direct.clamp();
    }

    // Rayon secondaire. C'est ce qui sépare un ray tracer d'un ray caster : la
    // couleur d'un point dépend de ce que la surface voit ailleurs dans la
    // scène, et pas seulement des sources qui l'éclairent.
    //
    // maxDepth borne la descente : deux miroirs qui se font face produiraient
    // sinon une récursion sans fin.
    const Vector3 reflectedDir = VectorUtils::reflect(ray.direction.normalize(), normal);
    const Ray reflectedRay(hitPoint + normal * bias, reflectedDir);
    const Color reflected = traceRay(scene, reflectedRay, depth + 1);

    return (direct * (1.0f - reflectivity) + reflected * reflectivity).clamp();
}

Color Renderer::samplePixel(const Scene& scene, unsigned int x, unsigned int y) const {
    const Camera& camera = scene.getCamera();

    if (samplesPerAxis <= 1) {
        const float u = static_cast<float>(x) / (width - 1);
        const float v = 1.0f - static_cast<float>(y) / (height - 1);
        return traceRay(scene, camera.generateRay(u, v));
    }

    // Suréchantillonnage régulier : n² rayons répartis dans le pixel, moyennés.
    // Les échantillons sont pris au centre des sous-cases, d'où le +0.5 — les
    // prendre au bord décalerait la moyenne vers le pixel voisin.
    Color accumulated(0.0f, 0.0f, 0.0f);
    const float inv = 1.0f / static_cast<float>(samplesPerAxis);

    for (unsigned int sy = 0; sy < samplesPerAxis; ++sy) {
        for (unsigned int sx = 0; sx < samplesPerAxis; ++sx) {
            const float fx = static_cast<float>(x) + (static_cast<float>(sx) + 0.5f) * inv;
            const float fy = static_cast<float>(y) + (static_cast<float>(sy) + 0.5f) * inv;
            const float u = fx / (width - 1);
            const float v = 1.0f - fy / (height - 1);
            accumulated = accumulated + traceRay(scene, camera.generateRay(u, v));
        }
    }

    const float samples = static_cast<float>(samplesPerAxis * samplesPerAxis);
    return accumulated * (1.0f / samples);
}

void Renderer::renderRows(const Scene& scene, std::vector<Color>& buffer,
                          unsigned int rowBegin, unsigned int rowEnd) const {
    // Chaque pixel est indépendant : aucun état partagé en écriture, chaque
    // itération touche une case distincte du buffer. C'est le cas idéal pour
    // une boucle parallèle, et la raison pour laquelle le lancer de rayons
    // s'y prête si bien.
    //
    // schedule(dynamic) plutôt que statique : le coût d'une ligne varie
    // beaucoup — une ligne de fond ne coûte presque rien, une ligne traversant
    // des surfaces réfléchissantes coûte plusieurs rebonds. Un découpage
    // statique laisserait des cœurs à l'arrêt pendant que d'autres finissent.
#ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic, 4)
#endif
    for (int y = static_cast<int>(rowBegin); y < static_cast<int>(rowEnd); ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            buffer[static_cast<size_t>(y) * width + x] =
                samplePixel(scene, x, static_cast<unsigned int>(y));
        }
    }
}

bool Renderer::renderToBuffer(const Scene& scene, std::vector<Color>& buffer) const {
    buffer.assign(static_cast<size_t>(width) * height, backgroundColor);
    renderRows(scene, buffer, 0, height);
    return true;
}

bool Renderer::render(const Scene& scene, const std::string& filename) {
    const auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << "Rendu " << width << "x" << height;
    if (samplesPerAxis > 1) {
        std::cout << ", " << (samplesPerAxis * samplesPerAxis) << " rayons/pixel";
    }
#ifdef _OPENMP
    std::cout << ", " << omp_get_max_threads() << " threads";
#else
    std::cout << ", monothread";
#endif
    std::cout << std::endl;

    if (useRealTimeDisplay) {
        display = std::make_unique<RealTimeDisplay>(width, height, "Retsu - rendu en cours");
        if (!display->init()) {
            std::cerr << "Affichage temps réel indisponible, poursuite sans." << std::endl;
            useRealTimeDisplay = false;
        }
    }

    std::vector<Color> buffer(static_cast<size_t>(width) * height, backgroundColor);

    if (useRealTimeDisplay) {
        // Avec aperçu, le rendu avance par bandes : chaque bande est calculée
        // en parallèle, puis poussée à l'écran depuis le thread principal.
        // L'affichage reste progressif sans avoir à synchroniser SFML, qui
        // n'est pas réentrant.
        //
        // Le découpage a un coût : chaque bande ouvre et referme une région
        // parallèle. On en prend donc peu, juste assez pour que l'image se
        // remplisse visiblement.
        const unsigned int bandHeight = std::max(1u, height / 8u);

        for (unsigned int rowBegin = 0; rowBegin < height; rowBegin += bandHeight) {
            const unsigned int rowEnd = std::min(rowBegin + bandHeight, height);
            renderRows(scene, buffer, rowBegin, rowEnd);

            std::cout << "\rProgression: " << std::setw(3) << (rowEnd * 100 / height)
                      << "%" << std::flush;

            if (!display->update(buffer)) {
                useRealTimeDisplay = false;
                break;
            }
        }
        std::cout << std::endl;
    }

    // Sans aperçu — ou dès qu'il a été fermé — l'image entière tient dans une
    // seule région parallèle. Un unique fork/join, et l'ordonnanceur dynamique
    // répartit la totalité des lignes : c'est là que le parallélisme rend ce
    // qu'il peut.
    if (!useRealTimeDisplay) {
        renderRows(scene, buffer, 0, height);
    }

    const std::chrono::duration<double> elapsed =
        std::chrono::high_resolution_clock::now() - startTime;

    if (useRealTimeDisplay) {
        display->close();
    }

    if (!saveToPPM(buffer, filename)) {
        std::cerr << "Erreur lors de la sauvegarde de l'image." << std::endl;
        return false;
    }

    std::cout << "Rendu terminé en " << std::fixed << std::setprecision(2)
              << elapsed.count() << " s." << std::endl;
    std::cout << "Image écrite dans " << filename << std::endl;
    return true;
}

// PPM binaire (P6) plutôt qu'ASCII (P3). Une image 1920x1080 pèse 6 Mo au lieu
// de 18, et l'écriture ne passe plus par le formatage de trois entiers par
// pixel — ce qui, sur une scène simple, coûtait plus cher que le rendu.
bool Renderer::saveToPPM(const std::vector<Color>& buffer, const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    file << "P6\n" << width << " " << height << "\n255\n";

    std::vector<unsigned char> row(static_cast<size_t>(width) * 3);
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            const Color& color = buffer[static_cast<size_t>(y) * width + x];
            row[x * 3 + 0] = static_cast<unsigned char>(color.getR8());
            row[x * 3 + 1] = static_cast<unsigned char>(color.getG8());
            row[x * 3 + 2] = static_cast<unsigned char>(color.getB8());
        }
        file.write(reinterpret_cast<const char*>(row.data()),
                   static_cast<std::streamsize>(row.size()));
    }

    return file.good();
}

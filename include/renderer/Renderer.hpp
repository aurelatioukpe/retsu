#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <memory>
#include "../core/Color.hpp"
#include "../scene/Scene.hpp"
#include "../lights/ILight.hpp"
#include "../display/RealTimeDisplay.hpp"

class Renderer {
private:
    unsigned int width;
    unsigned int height;
    Color backgroundColor;
    int maxDepth;
    bool shadows;
    unsigned int samplesPerAxis;
    bool useRealTimeDisplay;
    std::unique_ptr<RealTimeDisplay> display;
    int updateFrequency;

    // @brief Couleur vue le long d'un rayon, rebonds compris
    // @param depth Profondeur de récursion courante ; la descente s'arrête à maxDepth
    Color traceRay(const Scene& scene, const Ray& ray, int depth = 0) const;

    // @brief Éclairage direct au point d'impact : ambiant, diffus, ombres
    // @param bias Décalage du point de départ des rayons d'ombre le long de la
    //             normale, proportionnel à la distance parcourue
    Color shade(const Scene& scene, const IPrimitive& object,
                const Vector3& hitPoint, const Vector3& normal, float bias) const;

    // @brief Couleur d'un pixel, moyennée sur samplesPerAxis² sous-échantillons
    Color samplePixel(const Scene& scene, unsigned int x, unsigned int y) const;

    // @brief Remplit [rowBegin, rowEnd) du buffer. Parallélisé sur les lignes.
    void renderRows(const Scene& scene, std::vector<Color>& buffer,
                    unsigned int rowBegin, unsigned int rowEnd) const;

public:
    Renderer(unsigned int width, unsigned int height,
             const Color& backgroundColor = Color(0.0f, 0.0f, 0.0f),
             int maxDepth = 5, bool shadows = true,
             bool useRealTimeDisplay = false)
        : width(width), height(height), backgroundColor(backgroundColor),
          maxDepth(std::max(1, maxDepth)), shadows(shadows), samplesPerAxis(1),
          useRealTimeDisplay(useRealTimeDisplay), updateFrequency(10) {}

    bool render(const Scene& scene, const std::string& filename);

    // @brief Écrit le buffer en PPM binaire (P6)
    bool saveToPPM(const std::vector<Color>& buffer, const std::string& filename) const;

    bool renderToBuffer(const Scene& scene, std::vector<Color>& buffer) const;

    void setShadows(bool enable) {
        shadows = enable;
    }

    void setMaxDepth(int depth) {
        maxDepth = std::max(1, depth);
    }

    // @brief Anti-aliasing par suréchantillonnage : n donne n² rayons par pixel
    void setSamplesPerAxis(unsigned int n) {
        samplesPerAxis = std::max(1u, n);
    }

    void setBackgroundColor(const Color& color) {
        backgroundColor = color;
    }

    void setRealTimeDisplay(bool enable) {
        useRealTimeDisplay = enable;
    }

    void setUpdateFrequency(int frequency) {
        updateFrequency = std::max(1, frequency);
    }
};

#endif /* RENDERER_HPP_ */

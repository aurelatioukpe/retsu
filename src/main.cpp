#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "scene/SceneBuilder.hpp"
#include "renderer/Renderer.hpp"
#ifdef USE_SFML
#include "display/RealTimeDisplay.hpp"
#endif

namespace {

struct Options {
    std::string sceneFile;
    std::string outputFile;
    bool realTime = false;
    unsigned int samples = 1;
    int maxDepth = 5;
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <scene.rts> [options]\n\n"
              << "Options\n"
              << "  -r, --realtime       Aperçu progressif pendant le rendu (SFML)\n"
              << "  -s, --samples N      N x N rayons par pixel (anti-aliasing), défaut 1\n"
              << "  -d, --depth N        Profondeur maximale de réflexion, défaut 5\n"
              << "  -o, --output FICHIER Fichier de sortie, défaut <scène>.ppm\n"
              << "  -h, --help           Affiche cette aide\n";
}

// Renvoie false si un argument est invalide ; le message est déjà émis.
bool parseArguments(int argc, char* argv[], Options& options) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            std::exit(0);
        } else if (arg == "-r" || arg == "--realtime") {
            options.realTime = true;
        } else if (arg == "-s" || arg == "--samples") {
            if (++i >= argc) {
                std::cerr << "Erreur: " << arg << " attend une valeur." << std::endl;
                return false;
            }
            const int value = std::atoi(argv[i]);
            if (value < 1 || value > 8) {
                std::cerr << "Erreur: --samples doit être entre 1 et 8." << std::endl;
                return false;
            }
            options.samples = static_cast<unsigned int>(value);
        } else if (arg == "-d" || arg == "--depth") {
            if (++i >= argc) {
                std::cerr << "Erreur: " << arg << " attend une valeur." << std::endl;
                return false;
            }
            const int value = std::atoi(argv[i]);
            if (value < 1 || value > 32) {
                std::cerr << "Erreur: --depth doit être entre 1 et 32." << std::endl;
                return false;
            }
            options.maxDepth = value;
        } else if (arg == "-o" || arg == "--output") {
            if (++i >= argc) {
                std::cerr << "Erreur: " << arg << " attend un chemin." << std::endl;
                return false;
            }
            options.outputFile = argv[i];
        } else if (!arg.empty() && arg[0] == '-') {
            std::cerr << "Option inconnue: " << arg << std::endl;
            return false;
        } else if (options.sceneFile.empty()) {
            options.sceneFile = arg;
        } else {
            std::cerr << "Erreur: une seule scène à la fois." << std::endl;
            return false;
        }
    }

    if (options.sceneFile.empty()) {
        std::cerr << "Erreur: aucune scène fournie." << std::endl;
        return false;
    }

    if (options.outputFile.empty()) {
        const size_t dot = options.sceneFile.find_last_of('.');
        options.outputFile = options.sceneFile.substr(0, dot) + ".ppm";
    }

    return true;
}

}  // namespace

int main(int argc, char* argv[]) {
    Options options;
    if (!parseArguments(argc, argv, options)) {
        printUsage(argv[0]);
        return 84;
    }

    SceneBuilder builder;
    std::unique_ptr<Scene> scene = builder.buildFromFile(options.sceneFile);
    if (!scene) {
        std::cerr << "Erreur lors du chargement de la scène: "
                  << builder.getLastError() << std::endl;
        return 84;
    }

    const unsigned int imageWidth = scene->getCamera().getResolutionWidth();
    const unsigned int imageHeight = scene->getCamera().getResolutionHeight();

    Renderer renderer(imageWidth, imageHeight, Color(0.0f, 0.0f, 0.0f),
                      options.maxDepth, true, options.realTime);
    renderer.setSamplesPerAxis(options.samples);

#ifndef USE_SFML
    if (options.realTime) {
        std::cerr << "Aperçu indisponible: recompilez avec USE_SFML=1." << std::endl;
        return 84;
    }
#endif

    if (!renderer.render(*scene, options.outputFile)) {
        std::cerr << "Erreur lors du rendu de la scène." << std::endl;
        return 84;
    }

    return 0;
}

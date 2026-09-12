#include "../../include/scene/SceneBuilder.hpp"

bool SceneBuilder::buildCamera(const SceneSection& section, Scene& scene) {
    if (section.type != "Camera") {
        return false;
    }

    Vector3 position = section.getVector3("position", Vector3(0, 0, -5));
    Vector3 lookAt = section.getVector3("lookAt", Vector3(0, 0, 0));
    Vector3 up = section.getVector3("up", Vector3(0, 1, 0));
    float fov = section.getFloat("fov", 60.0f);
    float aspectRatio = section.getFloat("aspectRatio", 16.0f/9.0f);

    auto resolution = section.getResolution("resolution", std::make_pair(1920, 1080));

    Camera camera(position, lookAt, up, fov, aspectRatio, resolution.first, resolution.second);
    scene.setCamera(camera);

    return true;
}

std::unique_ptr<Scene> SceneBuilder::buildFromSections(const std::vector<SceneSection>& sections) {
    lastError = "";
    auto scene = std::make_unique<Scene>();

    bool hasCamera = false;

    for (const auto& section : sections) {
        if (section.type == "Camera") {
            if (buildCamera(section, *scene)) {
                hasCamera = true;
            } else {
                lastError = "Erreur lors de la création de la caméra";
                return nullptr;
            }
        }
        else if (primitiveFactory.isPrimitiveSection(section.type)) {
            auto primitive = primitiveFactory.createPrimitive(section);
            if (primitive) {
                scene->addPrimitive(std::move(primitive));
            } else {
                lastError = "Erreur lors de la création de la primitive: " + section.type;
                return nullptr;
            }
        }
        else if (lightFactory.isLightSection(section.type)) {
            auto light = lightFactory.createLight(section);
            if (light) {
                scene->addLight(std::move(light));
            } else {
                lastError = "Erreur lors de la création de la lumière: " + section.type;
                return nullptr;
            }
        }
        else {
            lastError = "Type de section non reconnu: " + section.type;
        }
    }

    if (!hasCamera) {
        lastError = "Aucune caméra définie dans la scène";
        return nullptr;
    }

    return scene;
}

std::unique_ptr<Scene> SceneBuilder::buildFromFile(const std::string& filename) {
    SceneParser parser;
    if (!parser.parseFile(filename)) {
        lastError = "Erreur de parsing: " + parser.getLastError();
        return nullptr;
    }

    return buildFromSections(parser.getSections());
}

std::unique_ptr<Scene> SceneBuilder::buildFromString(const std::string& content) {
    SceneParser parser;
    if (!parser.parseString(content)) {
        lastError = "Erreur de parsing: " + parser.getLastError();
        return nullptr;
    }

    return buildFromSections(parser.getSections());
}

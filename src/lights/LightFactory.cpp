#include "../../include/lights/LightFactory.hpp"
#include "../../include/lights/AmbientLight.hpp"
#include "../../include/lights/DirectionalLight.hpp"
#include "../../include/lights/PointLight.hpp"
#include <iostream>

bool LightFactory::isLightSection(const std::string& sectionType) const {
    return sectionType == "AmbientLight" ||
           sectionType == "DirectionalLight" ||
           sectionType == "PointLight";
}

std::unique_ptr<ILight> LightFactory::createLight(const SceneSection& section) const {
    if (section.type == "AmbientLight") {
        return createAmbientLight(section);
    } else if (section.type == "DirectionalLight") {
        return createDirectionalLight(section);
    } else if (section.type == "PointLight") {
        return createPointLight(section);
    }
    return nullptr;
}

std::unique_ptr<ILight> LightFactory::createAmbientLight(const SceneSection& section) const {
    float intensity = section.getFloat("intensity", 0.2f);
    Color color = section.getColor("color", Color(1.0f, 1.0f, 1.0f));
    std::cout << "Création d'une AmbientLight: " << std::endl;
    std::cout << "  - intensity: " << intensity << std::endl;
    std::cout << "  - color: " << color.r << ", " << color.g << ", " << color.b << std::endl;
    return std::make_unique<AmbientLight>(intensity, color);
}

std::unique_ptr<ILight> LightFactory::createDirectionalLight(const SceneSection& section) const {
    Vector3 direction = section.getVector3("direction", Vector3(1.0f, -1.0f, 0.0f));
    float intensity = section.getFloat("intensity", 0.7f);
    Color color = section.getColor("color", Color(1.0f, 1.0f, 1.0f));
    std::cout << "Création d'une DirectionalLight: " << std::endl;
    std::cout << "  - direction: " << direction << std::endl;
    std::cout << "  - intensity: " << intensity << std::endl;
    std::cout << "  - color: " << color.r << ", " << color.g << ", " << color.b << std::endl;
    return std::make_unique<DirectionalLight>(direction, intensity, color);
}

std::unique_ptr<ILight> LightFactory::createPointLight(const SceneSection& section) const {
    Vector3 position = section.getVector3("position");
    float intensity = section.getFloat("intensity", 0.8f);
    Color color = section.getColor("color", Color(1.0f, 0.8f, 0.5f));
    std::cout << "Création d'une PointLight: " << std::endl;
    std::cout << "  - position: " << position << std::endl;
    std::cout << "  - intensity: " << intensity << std::endl;
    std::cout << "  - color: " << color.r << ", " << color.g << ", " << color.b << std::endl;
    return std::make_unique<PointLight>(position, intensity, color);
}

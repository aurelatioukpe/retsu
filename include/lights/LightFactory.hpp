#ifndef LIGHT_FACTORY_HPP_
    #define LIGHT_FACTORY_HPP_
#include <memory>
#include <string>
#include "ILight.hpp"
#include "../parser/SceneParser.hpp"

class LightFactory {
public:
    LightFactory() = default;

    std::unique_ptr<ILight> createLight(const SceneSection& section) const;

    bool isLightSection(const std::string& sectionType) const;

private:
    std::unique_ptr<ILight> createAmbientLight(const SceneSection& section) const;
    std::unique_ptr<ILight> createDirectionalLight(const SceneSection& section) const;
    std::unique_ptr<ILight> createPointLight(const SceneSection& section) const;
};

#endif /* LIGHT_FACTORY_HPP_ */
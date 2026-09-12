#ifndef SCENE_BUILDER_HPP_
#define SCENE_BUILDER_HPP_

#include <string>
#include <memory>
#include <vector>
#include "../parser/SceneParser.hpp"
#include "../objects/PrimitiveFactory.hpp"
#include "../lights/LightFactory.hpp"
#include "Scene.hpp"

class SceneBuilder {
private:
    PrimitiveFactory primitiveFactory;
    LightFactory lightFactory;
    std::string lastError;

    bool buildCamera(const SceneSection& section, Scene& scene);

public:
    SceneBuilder() = default;

    std::unique_ptr<Scene> buildFromFile(const std::string& filename);

    std::unique_ptr<Scene> buildFromString(const std::string& content);

    std::unique_ptr<Scene> buildFromSections(const std::vector<SceneSection>& sections);

    const std::string& getLastError() const {
        return lastError;
    }
};

#endif /* SCENE_BUILDER_HPP_ */
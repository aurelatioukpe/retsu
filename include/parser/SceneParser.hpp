#ifndef SCENE_PARSER_HPP_
    #define SCENE_PARSER_HPP_
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include "../core/Vector3.hpp"
#include "../core/Color.hpp"

struct SceneSection {
    std::string type;
    std::map<std::string, std::string> properties;

    bool hasProperty(const std::string& key) const {
        return properties.find(key) != properties.end();
    }

    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        auto it = properties.find(key);
        return (it != properties.end()) ? it->second : defaultValue;
    }

    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        auto it = properties.find(key);
        if (it == properties.end()) {
            return defaultValue;
        }
        try {
            return std::stof(it->second);
        } catch (...) {
            return defaultValue;
        }
    }

    Vector3 getVector3(const std::string& key, const Vector3& defaultValue = Vector3()) const {
        auto it = properties.find(key);
        if (it == properties.end()) {
            return defaultValue;
        }
        
        std::istringstream iss(it->second);
        float x, y, z;
        char comma;
        
        iss >> x >> comma >> y >> comma >> z;
        if (iss.fail()) {
            return defaultValue;
        }
        
        return Vector3(x, y, z);
    }

    Color getColor(const std::string& key, const Color& defaultValue = Color()) const {
        Vector3 v = getVector3(key, Vector3(defaultValue.r, defaultValue.g, defaultValue.b));
        return Color(v.x, v.y, v.z);
    }

    std::pair<int, int> getResolution(const std::string& key, const std::pair<int, int>& defaultValue = std::make_pair(800, 600)) const {
        auto it = properties.find(key);
        if (it == properties.end()) {
            return defaultValue;
        }
        
        std::istringstream iss(it->second);
        int width, height;
        char comma;
        
        iss >> width >> comma >> height;
        if (iss.fail()) {
            return defaultValue;
        }
        
        return std::make_pair(width, height);
    }
};

class SceneParser {
private:
    std::vector<SceneSection> sections;
    std::string lastError;

    std::string trim(const std::string& str) const;
    bool parseLine(const std::string& line, std::string& currentSection);

public:
    SceneParser() = default;

    bool parseFile(const std::string& filename);
    bool parseString(const std::string& content);

    const std::vector<SceneSection>& getSections() const {
        return sections;
    }

    const std::string& getLastError() const {
        return lastError;
    }
};

#endif /* SCENE_PARSER_HPP_ */
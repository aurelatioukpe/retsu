#include "../../include/parser/SceneParser.hpp"
#include <iostream>
#include <algorithm>
#include <regex>

std::string SceneParser::trim(const std::string& str) const {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        start++;
    }
    
    auto end = str.end();
    do {
        end--;
    } while (end > start && std::isspace(*end));
    
    return std::string(start, end + 1);
}

bool SceneParser::parseLine(const std::string& line, std::string& currentSection) {
    std::string trimmedLine = trim(line);
    if (trimmedLine.empty() || trimmedLine[0] == '#') {
        return true;
    }
    
    std::regex sectionRegex(R"(\[(.*?)\])");
    std::smatch sectionMatch;
    if (std::regex_match(trimmedLine, sectionMatch, sectionRegex)) {
        currentSection = sectionMatch[1].str();
        SceneSection section;
        section.type = currentSection;
        sections.push_back(section);
        return true;
    }
    
    std::regex propertyRegex(R"((.*?)=\s*(.*))");
    std::smatch propertyMatch;
    if (std::regex_match(trimmedLine, propertyMatch, propertyRegex)) {
        std::string key = trim(propertyMatch[1].str());
        std::string value = trim(propertyMatch[2].str());
        if (currentSection.empty()) {
            lastError = "Propriété définie en dehors d'une section: " + key;
            return false;
        }
        sections.back().properties[key] = value;
        return true;
    }
    
    lastError = "Format de ligne non reconnu: " + trimmedLine;
    return false;
}

bool SceneParser::parseFile(const std::string& filename) {
    sections.clear();
    lastError = "";
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    if (extension != "rts") {
        lastError = "Extension de fichier non supportée: " + extension + ". Utilisez un fichier .rts";
        return false;
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        lastError = "Impossible d'ouvrir le fichier: " + filename;
        return false;
    }
    std::string line;
    std::string currentSection;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        if (!parseLine(line, currentSection)) {
            lastError = "Erreur à la ligne " + std::to_string(lineNumber) + ": " + lastError;
            return false;
        }
    }
    return true;
}

bool SceneParser::parseString(const std::string& content) {
    sections.clear();
    lastError = "";
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    int lineNumber = 0;
    while (std::getline(stream, line)) {
        lineNumber++;
        if (!parseLine(line, currentSection)) {
            lastError = "Erreur à la ligne " + std::to_string(lineNumber) + ": " + lastError;
            return false;
        }
    }
    return true;
}

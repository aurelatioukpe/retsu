#include "../../include/objects/PrimitiveFactory.hpp"
#include "../../include/objects/Sphere.hpp"
#include "../../include/objects/Plane.hpp"
#include "../../include/objects/Cylinder.hpp"
#include "../../include/objects/Cone.hpp"
#include <iostream>

bool PrimitiveFactory::isPrimitiveSection(const std::string& sectionType) const {
    return sectionType == "Sphere" ||
           sectionType == "Plane" ||
           sectionType == "Cylinder" ||
           sectionType == "Cone";
}

std::unique_ptr<IPrimitive> PrimitiveFactory::createPrimitive(const SceneSection& section) const {
    if (section.type == "Sphere") {
        return createSphere(section);
    } else if (section.type == "Plane") {
        return createPlane(section);
    } else if (section.type == "Cylinder") {
        return createCylinder(section);
    } else if (section.type == "Cone") {
        return createCone(section);
    }
    return nullptr;
}

std::unique_ptr<IPrimitive> PrimitiveFactory::createSphere(const SceneSection& section) const {
    Vector3 center = section.getVector3("center");
    float radius = section.getFloat("radius", 1.0f);
    Color color = section.getColor("color", Color(1.0f, 0.0f, 0.0f));
    float reflectivity = section.getFloat("reflectivity", 0.0f);
    std::cout << "Création d'une Sphere: " << std::endl;
    std::cout << "  - center: " << center << std::endl;
    std::cout << "  - radius: " << radius << std::endl;
    std::cout << "  - color: " << color.r << ", " << color.g << ", " << color.b << std::endl;
    if (reflectivity > 0.0f)
        std::cout << "  - reflectivity: " << reflectivity << std::endl;
    return std::make_unique<Sphere>(center, radius, color, reflectivity);
}

std::unique_ptr<IPrimitive> PrimitiveFactory::createPlane(const SceneSection& section) const {
    Vector3 point = section.getVector3("point");
    Vector3 normal = section.getVector3("normal", Vector3(0.0f, 1.0f, 0.0f));
    Color color = section.getColor("color", Color(0.5f, 0.5f, 0.5f));
    float reflectivity = section.getFloat("reflectivity", 0.0f);
    std::cout << "Création d'un Plane: " << std::endl;
    std::cout << "  - point: " << point << std::endl;
    std::cout << "  - normal: " << normal << std::endl;
    std::cout << "  - color: " << color.r << ", " << color.g << ", " << color.b << std::endl;
    if (reflectivity > 0.0f)
        std::cout << "  - reflectivity: " << reflectivity << std::endl;
    return std::make_unique<Plane>(point, normal, color, reflectivity);
}

std::unique_ptr<IPrimitive> PrimitiveFactory::createCylinder(const SceneSection& section) const {
    Vector3 basePoint = section.getVector3("basePoint");
    Vector3 axis = section.getVector3("axis", Vector3(0.0f, 1.0f, 0.0f));
    float radius = section.getFloat("radius", 1.0f);
    float height = section.getFloat("height", 1.0f);
    Color color = section.getColor("color", Color(0.0f, 0.0f, 1.0f));
    float reflectivity = section.getFloat("reflectivity", 0.0f);
    std::cout << "Création d'un Cylinder: " << std::endl;
    std::cout << "  - basePoint: " << basePoint << std::endl;
    std::cout << "  - axis: " << axis << std::endl;
    std::cout << "  - radius: " << radius << std::endl;
    std::cout << "  - height: " << height << std::endl;
    std::cout << "  - color: " << color.r << ", " << color.g << ", " << color.b << std::endl;
    if (reflectivity > 0.0f)
        std::cout << "  - reflectivity: " << reflectivity << std::endl;
    return std::make_unique<Cylinder>(basePoint, axis, radius, height, color, reflectivity);
}

std::unique_ptr<IPrimitive> PrimitiveFactory::createCone(const SceneSection& section) const {
    Vector3 apex = section.getVector3("apex");
    Vector3 axis = section.getVector3("axis", Vector3(0.0f, -1.0f, 0.0f));
    float angle = section.getFloat("angle", 30.0f);
    float height = section.getFloat("height", 1.0f);
    Color color = section.getColor("color", Color(1.0f, 1.0f, 0.0f));
    float reflectivity = section.getFloat("reflectivity", 0.0f);
    std::cout << "Création d'un Cone: " << std::endl;
    std::cout << "  - apex: " << apex << std::endl;
    std::cout << "  - axis: " << axis << std::endl;
    std::cout << "  - angle: " << angle << std::endl;
    std::cout << "  - height: " << height << std::endl;
    std::cout << "  - color: " << color.r << ", " << color.g << ", " << color.b << std::endl;
    if (reflectivity > 0.0f)
        std::cout << "  - reflectivity: " << reflectivity << std::endl;
    return std::make_unique<Cone>(apex, axis, angle, height, color, reflectivity);
}

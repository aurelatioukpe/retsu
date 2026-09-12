#ifndef PRIMITIVE_FACTORY_HPP_
#define PRIMITIVE_FACTORY_HPP_

#include <memory>
#include <string>
#include "IPrimitive.hpp"
#include "../parser/SceneParser.hpp"

class PrimitiveFactory {
public:
    PrimitiveFactory() = default;

    std::unique_ptr<IPrimitive> createPrimitive(const SceneSection& section) const;

    bool isPrimitiveSection(const std::string& sectionType) const;

private:
    std::unique_ptr<IPrimitive> createSphere(const SceneSection& section) const;
    std::unique_ptr<IPrimitive> createPlane(const SceneSection& section) const;
    std::unique_ptr<IPrimitive> createCylinder(const SceneSection& section) const;
    std::unique_ptr<IPrimitive> createCone(const SceneSection& section) const;
};

#endif /* PRIMITIVE_FACTORY_HPP_ */
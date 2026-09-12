#ifndef SCENE_HPP_
    #define SCENE_HPP_
#include <vector>
#include <memory>
#include "Camera.hpp"
#include "../objects/IPrimitive.hpp"
#include "../lights/ILight.hpp"

class Scene {
private:
    std::vector<std::unique_ptr<IPrimitive>> primitives;
    std::vector<std::unique_ptr<ILight>> lights;
    Camera camera;

public:
    Scene() = default;
    explicit Scene(const Camera& camera) : camera(camera) {}

    void addPrimitive(std::unique_ptr<IPrimitive> primitive) {
        primitives.push_back(std::move(primitive));
    }

    void addLight(std::unique_ptr<ILight> light) {
        lights.push_back(std::move(light));
    }

    void setCamera(const Camera& newCamera) {
        camera = newCamera;
    }

    const Camera& getCamera() const {
        return camera;
    }

    const std::vector<std::unique_ptr<IPrimitive>>& getPrimitives() const {
        return primitives;
    }

    const std::vector<std::unique_ptr<ILight>>& getLights() const {
        return lights;
    }

    bool findClosestIntersection(const Ray& ray, float& t, const IPrimitive*& primitive) const {
        float closest = std::numeric_limits<float>::max();
        const IPrimitive* closestPrimitive = nullptr;
        
        for (const auto& p : primitives) {
            float distance;
            if (p->intersect(ray, distance) && distance < closest) {
                closest = distance;
                closestPrimitive = p.get();
            }
        }
        
        if (closestPrimitive) {
            t = closest;
            primitive = closestPrimitive;
            return true;
        }
        return false;
    }
};

#endif /* SCENE_HPP_ */
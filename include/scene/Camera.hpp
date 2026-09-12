#ifndef CAMERA_HPP_
    #define CAMERA_HPP_
#include <cmath>
#include "../core/Vector3.hpp"
#include "../core/Ray.hpp"

class Camera {
private:
    Vector3 position;
    Vector3 direction;
    Vector3 up;
    Vector3 right;
    float fov;
    float aspectRatio;
    Vector3 lowerLeftCorner;
    Vector3 horizontal;
    Vector3 vertical;
    int resolutionWidth;
    int resolutionHeight;

    void calculateViewPlane() {
        float halfHeight = std::tan(fov * M_PI / 180.0f / 2.0f);
        float halfWidth = aspectRatio * halfHeight;
        Vector3 w = -direction.normalize();
        Vector3 u = up.cross(w).normalize();
        Vector3 v = w.cross(u);
        right = u;
        lowerLeftCorner = position - halfWidth * u - halfHeight * v - w;
        horizontal = 2.0f * halfWidth * u;
        vertical = 2.0f * halfHeight * v;
    }

public:
    Camera() 
        : position(0, 0, 0), direction(0, 0, -1), up(0, 1, 0), 
          fov(60.0f), aspectRatio(1.0f), resolutionWidth(800), resolutionHeight(600) {
        calculateViewPlane();
    }

    Camera(const Vector3& position, const Vector3& lookAt, const Vector3& up, 
           float fov, float aspectRatio,
           int width = 800, int height = 600) 
        : position(position), up(up.normalize()), fov(fov), aspectRatio(aspectRatio),
          resolutionWidth(width), resolutionHeight(height) {
        direction = (lookAt - position).normalize();
        calculateViewPlane();
    }

    Ray generateRay(float u, float v) const {
        Vector3 target = lowerLeftCorner + u * horizontal + v * vertical;
        Vector3 dir = (target - position).normalize();
        return Ray(position, dir);
    }

    const Vector3& getPosition() const { return position; }
    const Vector3& getDirection() const { return direction; }
    const Vector3& getUp() const { return up; }
    const Vector3& getRight() const { return right; }
    float getFov() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    int getResolutionWidth() const { return resolutionWidth; }
    int getResolutionHeight() const { return resolutionHeight; }

    void setPosition(const Vector3& pos) { 
        position = pos;
        calculateViewPlane();
    }
    
    void lookAt(const Vector3& target) {
        direction = (target - position).normalize();
        calculateViewPlane();
    }
    
    void setFov(float newFov) {
        fov = newFov;
        calculateViewPlane();
    }
    
    void setAspectRatio(float ratio) {
        aspectRatio = ratio;
        calculateViewPlane();
    }

    void setResolution(int width, int height) {
        resolutionWidth = width;
        resolutionHeight = height;
        aspectRatio = static_cast<float>(width) / height;
        calculateViewPlane();
    }
};

#endif /* CAMERA_HPP_ */
#ifndef VECTOR_UTILS_HPP_
#define VECTOR_UTILS_HPP_

#include "Vector3.hpp"
#include <cmath>

namespace VectorUtils {
    inline float dot(const Vector3& a, const Vector3& b) {
        return a.dot(b);
    }

    inline Vector3 cross(const Vector3& a, const Vector3& b) {
        return a.cross(b);
    }

    inline float length(const Vector3& v) {
        return v.length();
    }

    inline float lengthSquared(const Vector3& v) {
        return v.lengthSquared();
    }

    inline Vector3 normalize(const Vector3& v) {
        return v.normalize();
    }

    inline float distance(const Vector3& a, const Vector3& b) {
        return (b - a).length();
    }

    inline float distanceSquared(const Vector3& a, const Vector3& b) {
        return (b - a).lengthSquared();
    }

    inline Vector3 reflect(const Vector3& v, const Vector3& n) {
        return v - 2 * dot(v, n) * n;
    }
}

#endif /* VECTOR_UTILS_HPP_ */
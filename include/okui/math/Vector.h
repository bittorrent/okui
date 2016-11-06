#pragma once

#include <okui/config.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>

namespace okui {
namespace math {

template <typename T, size_t N>
struct VectorComponents {
    union {
        std::array<T, N> components{};
    };
};

template <typename T>
struct VectorComponents<T, 2> {
    union {
        std::array<T, 2> components{};
        struct { T x, y; };
    };
};

template <typename T>
struct VectorComponents<T, 3> {
    union {
        std::array<T, 3> components{};
        struct { T x, y, z; };
    };
};

template <typename T, size_t N>
struct Vector : VectorComponents<T, N> {
    using VectorComponents<T, N>::components;
    static_assert(sizeof(VectorComponents<T, N>) == sizeof(T) * N, "bad packing");

    Vector() {}

    template <typename First, typename Second, typename... Rem>
    Vector(First&& first, Second&& second, Rem&&... rem) {
        components = std::array<T, N>({{std::forward<First>(first), std::forward<Second>(second), std::forward<Rem>(rem)...}});
    }

    T& operator[](size_t i) { return components[i]; }
    const T& operator[](size_t i) const { return components[i]; }

    template <typename U>
    Vector& operator*=(U&& scalar) {
        for (auto& component : components) {
            component *= scalar;
        }
        return *this;
    }

    template <typename U>
    Vector operator*(U&& scalar) const { return Vector{*this} *= scalar; }

    template <typename U>
    Vector& operator/=(U&& scalar) {
        for (auto& component : components) {
            component /= scalar;
        }
        return *this;
    }

    template <typename U>
    Vector operator/(U&& scalar) const { return Vector{*this} /= scalar; }

    Vector& operator+=(const Vector& other) {
        std::transform(components.begin(), components.end(), other.components.begin(), components.begin(), std::plus<T>());
        return *this;
    }

    Vector operator+(const Vector& other) const { return Vector{*this} += other; }

    Vector& operator-=(const Vector& other) {
        std::transform(components.begin(), components.end(), other.components.begin(), components.begin(), std::minus<T>());
        return *this;
    }

    Vector operator-(const Vector& other) const { return Vector{*this} -= other; }

    T magnitudeSquared() const {
        T ret{};
        for (auto& component : components) {
            ret += component * component;
        }
        return ret;
    }

    T magnitude() const { return std::sqrt(magnitudeSquared()); }

    void normalize() {
        *this /= magnitude();
    }

    Vector normalized() {
        Vector ret{*this};
        ret.normalize();
        return ret;
    }

    T dot(const Vector& other) {
        T result{};
        auto start = other.components.begin();
        for (auto& component : components) {
            result += *start*component;
            ++start;
        }
        return result;
    }

    bool operator==(const Vector& other) const {
        return components == other.components;
    }

    bool operator!=(const Vector& other) const {
        return components != other.components;
    }
};

} } // namespace okui::math

//
// Created by HuyN on 3/26/2025.
//
#pragma once

#include <cmath>

#ifndef VECTOR2_H
#define VECTOR2_H

namespace HuyNVector {

    template<typename T>
    class Vector2 {
        public:

        // ****************************** VECTOR INITIALIZATION ****************************** //

        T x, y;

        constexpr explicit Vector2(T x = 0, T y = 0) noexcept : x(x), y(y) {};

        constexpr Vector2(const Vector2<T>& _v) noexcept : x(_v.x), y(_v.y) {};


        // ********************************* VECTOR OPERATOR ********************************* //

        constexpr Vector2<T>& operator+=(const Vector2<T>& _v) noexcept {
            x += _v.x;
            y += _v.y;
            return *this;
        }

        constexpr Vector2<T>& operator-=(const Vector2<T>& _v) noexcept {
            x -= _v.x;
            y -= _v.y;
            return *this;
        }

        constexpr Vector2<T>& operator*=(T _scalar) noexcept {
            x *= _scalar;
            y *= _scalar;
            return *this;
        }

        constexpr Vector2<T>& operator/=(T _scalar) noexcept {
            x /= _scalar;
            y /= _scalar;
            return *this;
        }

        constexpr bool operator==(Vector2<T>* _v) noexcept {
            return (_v->x == this->x && _v->y == this->y);
        }

        constexpr bool operator!=(Vector2<T> _v) noexcept {
            return !(_v == this);
        }

        // ********************************* VECTOR FUNCTION ********************************* //

        [[nodiscard]] constexpr Vector2<T> pow(T lhs) const noexcept {
            return Vector2<T>(x * lhs, y * lhs);
        }

        [[nodiscard]] T magnitude() const noexcept {
            return sqrt(x * x + y * y);
        }

        [[nodiscard]] T dot(const Vector2<T>& _v) const noexcept {
            return x * _v.x + y * _v.y;
        }

        [[nodiscard]] Vector2<T> normalize() const noexcept {
            return (*this) / (magnitude() == 0 ? 1 : magnitude());
        }

        [[nodiscard]] constexpr double distance(const Vector2<T>& _v) const noexcept {
            return std::sqrt(std::pow(x - _v.x, 2) + std::pow(y - _v.y, 2));
        }

        [[nodiscard]] double angleBetween(const Vector2<T>& _v) const noexcept {
            return std::acos(this->dot(_v) / (this->magnitude() * _v.magnitude()));
        }

    };

    // ********************************* NON-MEMBER VECTOR OPERATOR OVERLOADS ********************************* //

    template<typename T>
    constexpr Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs) {
        return Vector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    template<typename T>
    constexpr Vector2<T> operator-(const Vector2<T>& lhs, const Vector2<T>& rhs) {
        return Vector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    template<typename T>
    constexpr Vector2<T> operator*(T lhs, const Vector2<T>& rhs) {
        return Vector2<T>(lhs * rhs.x, lhs * rhs.y);
    }

    template<typename T>
    constexpr Vector2<T> operator*(const Vector2<T>& lhs, T rhs) {
        return Vector2<T>(lhs.x * rhs, lhs.y * rhs);
    }

    template<typename T>
    constexpr Vector2<T> operator/(const Vector2<T>& lhs, T rhs) {
        return Vector2<T>(lhs.x / rhs, lhs.y / rhs);
    }

    template<typename T>
    constexpr Vector2<T> operator-(const Vector2<T>& v) {
        return Vector2<T>(-v.x, -v.y);
    }

}

#endif //VECTOR2_H

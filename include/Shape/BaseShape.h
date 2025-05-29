//
// Created by HuyN on 29/05/2025.
//

#ifndef BASESHAPE_H
#define BASESHAPE_H

#include "Vector2.h"

using HuyNVector::Vector2;

namespace Shape {
    template <typename T>
    class BaseShape {
    protected:
        char type{};
    public:
        T x, y;

        explicit BaseShape(const char type_) : type(type_) {}

        virtual ~BaseShape() = default;
        [[nodiscard]] virtual T area() const = 0;
        [[nodiscard]] virtual bool contains(Vector2<T> position) const = 0;
        [[nodiscard]] virtual bool contains(T x_, T y_) const = 0;
        [[nodiscard]] virtual char getType() const { return type; }
        virtual void setPosition(T x_, T y_) = 0;
        virtual void setPosition(Vector2<T> position) = 0;

        [[nodiscard]] virtual BaseShape<T>* clone() const = 0;
    };
}

#endif //BASESHAPE_H

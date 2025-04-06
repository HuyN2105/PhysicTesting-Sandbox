//
// Created by HuyN on 4/2/2025.
//
#pragma once

#include <cmath>
#include <vector>
#include <cmath>

#include "Vector2.h"
#include "Box.h"
#include "Circle.h"

#ifndef PHYSICENGINE_H
#define PHYSICENGINE_H

using HuyNVector::Vector2;

namespace HuyNPhysic {

    template <typename T, typename S = Shape::Circle<T>>
    class Object {
        public:
        T x;
        T y;
        Vector2<T> velocity;
        Vector2<T> acceleration;
        T mass;

        S shape;

        constexpr Object(T x_, T y_, S shape_, T velocity_x = 0, T velocity_y = 0) : x(x_), y(y_), velocity({velocity_x, velocity_y}), shape(shape_) {}

        constexpr explicit Object(Vector2<T> position_, S shape_, Vector2<T> velocity_ = 0) : x(position_.x), y(position_.y), velocity(velocity_), shape(shape_) {}

    };

    // *********************************** COLLISION DETECTION *********************************** //

    template <typename T, typename S = Shape::Box<T>>
    bool rectRect(S r1, S r2) {
        return (r1.getRight() >= r2.left &&    // r1 right edge past r2 left
                r1.left <= r2.getRight() &&    // r1 left edge past r2 right
                r1.top >= r2.getBottom() &&    // r1 top edge past r2 bottom
                r1.getBottom() <= r2.top);     // r1 bottom edge past r2 top
    }

    template <typename T, typename S1 = Shape::Circle<T>, typename S2 = Shape::Box<T>>
    bool circleRect(S1 cir, S2 rect) {

        // temporary variables to set edges for testing
        float testX = cir.x;
        float testY = cir.y;

        // which edge is closest?
        if (cir.x < rect.left) testX = rect.left;      // test left edge
        else if (cir.x > rect.left + rect.width) testX = rect.left + rect.width;   // right edge
        if (cir.y < rect.top)         testY = rect.y;      // top edge
        else if (cir.y > rect.getBottom()) testY = rect.getBottom();   // bottom edge

        // get distance from the closest edges
        float distX = cir.x - testX;
        float distY = cir.y - testY;
        float distance = std::sqrt((distX*distX) + (distY*distY));

        // if the distance is less than the radius, collision!
        if (distance <= cir.radius) return true;
        return false;
    }

    template <typename T>
    bool constexpr CheckCollide(Object<T, Shape::Circle<T>> obj1, Object<T, Shape::Box<T>> obj2) {
        return circleRect<T>(obj1.shape, obj2.shape);
    }

    template <typename T>
    bool constexpr CheckCollide(Object<T, Shape::Box<T>> obj1, Object<T, Shape::Circle<T>> obj2) {
        return circleRect<T>(obj2.shape, obj1.shape);
    }

    template <typename T>
    bool constexpr CheckCollide(Object<T, Shape::Box<T>> obj1, Object<T, Shape::Box<T>> obj2) {
        return rectRect<T>(obj1.shape, obj2.shape);
    }

    template <typename T>
    bool constexpr CheckCollide(Object<T, Shape::Circle<T>> obj1, Object<T, Shape::Circle<T>> obj2) {
        return (Vector2<T>{obj1.x, obj1.y}.distance(Vector2<T>{obj2.x, obj2.y}) <= obj2.shape.radius + obj1.shape.radius);
    }


}


#endif //PHYSICENGINE_H
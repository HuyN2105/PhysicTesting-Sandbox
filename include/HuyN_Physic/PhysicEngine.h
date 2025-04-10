//
// Created by HuyN on 4/2/2025.
//
#pragma once

#include <cmath>
#include <variant>

#include "Vector2.h"
#include "Box.h"
#include "Circle.h"

#ifndef PHYSICENGINE_H
#define PHYSICENGINE_H

using HuyNVector::Vector2;

namespace HuyNPhysic {

    template <typename T>
    using shapeVariant = std::variant<Shape::Circle<T>, Shape::Box<T>>;

    template <typename T>
    class Object {
    public:
        T x;
        T y;
        Vector2<T> velocity;        // pixels per second
        Vector2<T> acceleration;
        T mass;

        shapeVariant<T> shape;

        constexpr Object(T x_, T y_, T mass_, shapeVariant<T> shape_, T velocity_x = 0, T velocity_y = 0, T acceleration_x = 0, T acceleration_y = 0) :
        x(x_), y(y_), velocity({velocity_x, velocity_y}), acceleration({acceleration_x, acceleration_y}), mass(mass_), shape(std::move(shape_)) {
            syncShapePosition();
        }

        constexpr explicit Object(Vector2<T> position_, T mass_, shapeVariant<T> shape_, Vector2<T> velocity_ = 0, Vector2<T> acceleration_ = 0) :
        x(position_.x), y(position_.y), velocity(velocity_), acceleration(acceleration_), mass(mass_), shape(std::move(shape_)) {
            syncShapePosition();
        }

        constexpr void syncShapePosition() {
            std::visit([&](auto&& s) {
                if constexpr (std::is_same_v<std::decay_t<decltype(s)>, Shape::Circle<T>>) {
                    s.x = x;
                    s.y = y;
                } else if constexpr (std::is_same_v<std::decay_t<decltype(s)>, Shape::Box<T>>) {
                    s.x = x;
                    s.y = y;
                    s.left = x - s.width / 2;
                    s.top = y - s.height / 2;
                }
            }, shape);
        }

        // ********************************** BASIC PHYSIC FUNCTIONS ********************************* //

        constexpr void PhysicStep(T TickPassed, bool applyFriction = false, T frictionCoefficient = 0.1) {
            // 1 tick = 1 ms

            velocity += acceleration * TickPassed / 1000.0;

            // velocity applied as pixels per second as default
            x += velocity.x * TickPassed / 1000.0;
            y += velocity.y * TickPassed / 1000.0;

            syncShapePosition();

        }

        constexpr void handleBoundaries(T minX, T maxX, T minY, T maxY) {
            std::visit([&](auto& s) {
                if constexpr (std::is_same_v<std::decay_t<decltype(s)>, Shape::Circle<T>>) {
                    T radius = s.radius;
                    // Left boundary
                    if (x - radius < minX) {
                        x = minX + radius; // Correct position
                        velocity.x = -velocity.x; // Reflect velocity
                    }
                    // Right boundary
                    if (x + radius > maxX) {
                        x = maxX - radius;
                        velocity.x = -velocity.x;
                    }
                    // Top boundary
                    if (y - radius < minY) {
                        y = minY + radius;
                        velocity.y = -velocity.y;
                    }
                    // Bottom boundary (e.g., floor)
                    if (y + radius > maxY) {
                        y = maxY - radius;
                        velocity.y = -velocity.y;
                    }
                } else if constexpr (std::is_same_v<std::decay_t<decltype(s)>, Shape::Box<T>>) {
                    T halfWidth = s.width / 2;
                    T halfHeight = s.height / 2;
                    // Left boundary
                    if (x - halfWidth < minX) {
                        x = minX + halfWidth;
                        velocity.x = -velocity.x;
                    }
                    // Right boundary
                    if (x + halfWidth > maxX) {
                        x = maxX - halfWidth;
                        velocity.x = -velocity.x;
                    }
                    // Top boundary
                    if (y - halfHeight < minY) {
                        y = minY + halfHeight;
                        velocity.y = -velocity.y;
                    }
                    // Bottom boundary
                    if (y + halfHeight > maxY) {
                        y = maxY - halfHeight;
                        velocity.y = -velocity.y;
                    }
                }
            }, shape);
            syncShapePosition(); // Ensure shape aligns with corrected position
        }

        constexpr Vector2<T> Vector2Position() const {
            return Vector2<T>(x, y);
        }

    };

        // **************************************** COLLISION **************************************** //

    template <typename T>
    bool rectRect(Shape::Box<T> r1, Shape::Box<T> r2) {
        return (r1.getRight() >= r2.left &&    // r1 right edge past r2 left
                r1.left <= r2.getRight() &&    // r1 left edge past r2 right
                r1.top >= r2.getBottom() &&    // r1 top edge past r2 bottom
                r1.getBottom() <= r2.top);     // r1 bottom edge past r2 top
    }

    template <typename T>
    bool circleRect(Shape::Circle<T> cir, Shape::Box<T> rect) {

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

    template<typename T>
    bool CheckCollide(const Object<T>& obj1, const Object<T>& obj2) {
        return std::visit([&](const auto& s1, const auto& s2) {
            using S1 = std::decay_t<decltype(s1)>;
            using S2 = std::decay_t<decltype(s2)>;
            if constexpr (std::is_same_v<S1, Shape::Circle<T>> && std::is_same_v<S2, Shape::Circle<T>>)
                return (Vector2<T>{obj1.x, obj1.y}.distance(Vector2<T>{obj2.x, obj2.y}) <= s1.radius + s2.radius);
            else if constexpr (std::is_same_v<S1, Shape::Circle<T>> && std::is_same_v<S2, Shape::Box<T>>)
                return circleRect(s1, s2);
            else if constexpr (std::is_same_v<S1, Shape::Box<T>> && std::is_same_v<S2, Shape::Circle<T>>)
                return circleRect(s2, s1);
            else if constexpr (std::is_same_v<S1, Shape::Box<T>> && std::is_same_v<S2, Shape::Box<T>>)
                return rectRect(s1, s2);
            return false;
        }, obj1.shape, obj2.shape);
    }

    // template<typename T>
    // constexpr void CollisionProcess(Object<T>* obj1, Object<T>* obj2) {
    //
    //     const double vMassSum = obj1->mass + obj2->mass;
    //     Vector2<double> vDiff = obj2->velocity - obj1->velocity;
    //     Vector2<double> vPosSub = obj2->Vector2Position() - obj1->Vector2Position();
    //     const double vDist = vPosSub.magnitude();
    //     obj1->velocity += (2 * obj2->mass / vMassSum) * vDiff.dot(vPosSub) / pow(vDist, 2) * vPosSub;
    //     vDiff = -1.0 * vDiff;
    //     vPosSub = -1.0 * vPosSub;
    //     obj2->velocity += (2 * obj1->mass / vMassSum) * vDiff.dot(vPosSub) / pow(vDist, 2) * vPosSub;
    // }

    template<typename T>
    constexpr void CollisionProcess(Object<T>* obj1, Object<T>* obj2) {
        // Velocity update (elastic collision)
        const T vMassSum = obj1->mass + obj2->mass;
        Vector2<T> vDiff = obj2->velocity - obj1->velocity;
        Vector2<T> vPosSub = obj2->Vector2Position() - obj1->Vector2Position();
        T vDist = vPosSub.magnitude();
        if (vDist == 0) vDist = 1e-6; // Prevent division by zero
        Vector2<T> normal = vPosSub / vDist; // Collision normal

        // Velocity impulse
        T impulse = (2 * obj2->mass / vMassSum) * vDiff.dot(normal);
        obj1->velocity += impulse * normal;
        obj2->velocity -= impulse * normal; // Opposite direction for obj2

        // Position correction
        T penetrationDepth = 0;
        std::visit([&](const auto& s1, const auto& s2) {
            using S1 = std::decay_t<decltype(s1)>;
            using S2 = std::decay_t<decltype(s2)>;
            if constexpr (std::is_same_v<S1, Shape::Circle<T>> && std::is_same_v<S2, Shape::Circle<T>>) {
                penetrationDepth = (s1.radius + s2.radius) - vDist;
            } else if constexpr (std::is_same_v<S1, Shape::Circle<T>> && std::is_same_v<S2, Shape::Box<T>>) {
                // Approximate penetration using circle-box distance
                Vector2<T> closestPoint(
                    std::max(s2.left, std::min(s1.x, s2.left + s2.width)),
                    std::max(s2.top, std::min(s1.y, s2.top + s2.height))
                );
                T distToBox = (obj1->Vector2Position() - closestPoint).magnitude();
                penetrationDepth = s1.radius - distToBox;
            } else if constexpr (std::is_same_v<S1, Shape::Box<T>> && std::is_same_v<S2, Shape::Circle<T>>) {
                Vector2<T> closestPoint(
                    std::max(s1.left, std::min(s2.x, s1.left + s1.width)),
                    std::max(s1.top, std::min(s2.y, s1.top + s1.height))
                );
                T distToBox = (obj2->Vector2Position() - closestPoint).magnitude();
                penetrationDepth = s2.radius - distToBox;
            } else if constexpr (std::is_same_v<S1, Shape::Box<T>> && std::is_same_v<S2, Shape::Box<T>>) {
                // Simplified box-box penetration (along normal)
                T overlapX = std::min(s1.left + s1.width, s2.left + s2.width) - std::max(s1.left, s2.left);
                T overlapY = std::min(s1.top + s1.height, s2.top + s2.height) - std::max(s1.top, s2.top);
                penetrationDepth = std::min(overlapX, overlapY); // Minimum overlap direction
            }
        }, obj1->shape, obj2->shape);

        if (penetrationDepth > 0) {
            // Move objects apart proportional to their masses (inverse mass ratio)
            T totalInvMass = (obj1->mass > 0 ? 1 / obj1->mass : 0) + (obj2->mass > 0 ? 1 / obj2->mass : 0);
            if (totalInvMass == 0) totalInvMass = 1; // Avoid division by zero if both are infinite mass
            T move1 = (obj1->mass > 0 ? (1 / obj1->mass) / totalInvMass : 0) * penetrationDepth;
            T move2 = (obj2->mass > 0 ? (1 / obj2->mass) / totalInvMass : 0) * penetrationDepth;

            obj1->x -= normal.x * move1;
            obj1->y -= normal.y * move1;
            obj2->x += normal.x * move2;
            obj2->y += normal.y * move2;

            obj1->syncShapePosition();
            obj2->syncShapePosition();
        }
    }

}


#endif //PHYSICENGINE_H
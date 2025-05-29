//
// Created by HuyN on 4/2/2025.
//
#pragma once

#include <cmath>

#include "Vector2.h"
#include "BaseShape.h"
#include "Box.h"
#include "Circle.h"

#ifndef PHYSICENGINE_H
#define PHYSICENGINE_H

using HuyNVector::Vector2;

namespace HuyNPhysic {

    template <typename T>
    class Object {
    public:
        T x;
        T y;
        Vector2<T> velocity;        // pixels per second
        Vector2<T> forcesApplying;
        T mass;

        Shape::BaseShape<T>* shape;

        Object(T x_, T y_, T mass_, Shape::BaseShape<T>* shape_, T velocity_x = 0, T velocity_y = 0, T acceleration_x = 0, T acceleration_y = 0) :
        x(x_), y(y_), velocity(Vector2<T>{velocity_x, velocity_y}), forcesApplying(Vector2<T>{acceleration_x, acceleration_y}), mass(mass_), shape(shape_->clone()) {
            syncShapePosition();
        }

        explicit Object(Vector2<T> position_, T mass_, Shape::BaseShape<T>* shape_, Vector2<T> velocity_ = {0, 0}, Vector2<T> acceleration_ = {0, 0}) :
        x(position_.x), y(position_.y), velocity(velocity_), forcesApplying(acceleration_), mass(mass_), shape(shape_->clone()) {
            syncShapePosition();
        }

        // Copy constructor
        Object(const Object& other) :
            x(other.x), y(other.y), velocity(other.velocity),
            forcesApplying(other.forcesApplying), mass(other.mass),
            shape(other.shape->clone()) {}

        // Assignment operator
        Object& operator=(const Object& other) {
            if (this != &other) {
                x = other.x;
                y = other.y;
                velocity = other.velocity;
                forcesApplying = other.forcesApplying;
                mass = other.mass;
                delete shape;
                shape = other.shape->clone();
            }
            return *this;
        }

        // Destructor
        ~Object() {
            delete shape;
        }

        void syncShapePosition() {
            shape->setPosition(x, y);
        }

        // ********************************** BASIC PHYSIC FUNCTIONS ********************************* //

        void ApplyingForce(Vector2<T> force) {
            forcesApplying += force * mass;
        }

        void PhysicStep(T TickPassed, bool applyFriction = false, T frictionCoefficient = 0.1) {
            // 1 tick = 1 ms
            velocity += forcesApplying * TickPassed / 1000.0;

            // velocity applied as pixels per second as default
            x += velocity.x * TickPassed / 1000.0;
            y += velocity.y * TickPassed / 1000.0;

            syncShapePosition();
        }

        void handleBoundaries(T minX, T maxX, T minY, T maxY) {
            if (const char type = shape->getType(); type == 'c') {
                auto* circle = dynamic_cast<Shape::Circle<T>*>(shape);
                T radius = circle->radius;

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
                // Bottom boundary
                if (y + radius > maxY) {
                    y = maxY - radius;
                    velocity.y = -velocity.y;
                }
            }
            else if (type == 'b') {
                auto* box = dynamic_cast<Shape::Box<T>*>(shape);
                T halfWidth = box->width / 2;
                T halfHeight = box->height / 2;

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

            syncShapePosition(); // Ensure shape aligns with corrected position
        }

        Vector2<T> Vector2Position() const {
            return Vector2<T>(x, y);
        }
    };

    // **************************************** COLLISION **************************************** //


    // TODO: Collision - Implement the commented penetrationDepth logic for realistic resolution.


    template <typename T>
    bool rectRect(Shape::Box<T> r1, Shape::Box<T> r2) {
        return (r1.x <= r2.getRight() &&   // r1 left edge not past r2 right
                r1.getRight() >= r2.x &&   // r1 right edge not past r2 left
                r1.y <= r2.getBottom() &&   // r1 top edge not past r2 bottom
                r1.getBottom() >= r2.y);    // r1 bottom edge not past r2 top
    }

    template <typename T>
    bool circleRect(Shape::Circle<T> cir, Shape::Box<T> rect) {

        // temporary variables to set edges for testing
        float testX = cir.x;
        float testY = cir.y;

        // which edge is closest?
        if (cir.x < rect.x) testX = rect.x;      // test left edge
        else if (cir.x > rect.x + rect.width) testX = rect.x + rect.width;   // right edge
        if (cir.y < rect.y)         testY = rect.y;      // top edge
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
    bool CheckCollide(const Object<T>& obj1, const Object<T>& obj2) {
        char type1 = obj1.shape->getType();
        char type2 = obj2.shape->getType();

        if (type1 == 'c' && type2 == 'c') {
            auto* circle1 = dynamic_cast<Shape::Circle<T>*>(obj1.shape);
            auto* circle2 = dynamic_cast<Shape::Circle<T>*>(obj2.shape);
            return (Vector2<T>{obj1.x, obj1.y}.distance(Vector2<T>{obj2.x, obj2.y}) <= circle1->radius + circle2->radius);
        }
        if (type1 == 'c' && type2 == 'b') {
            auto* circle = dynamic_cast<Shape::Circle<T>*>(obj1.shape);
            auto* box = dynamic_cast<Shape::Box<T>*>(obj2.shape);
            return circleRect(*circle, *box);
        }
        if (type1 == 'b' && type2 == 'c') {
            auto* box = dynamic_cast<Shape::Box<T>*>(obj1.shape);
            auto* circle = dynamic_cast<Shape::Circle<T>*>(obj2.shape);
            return circleRect(*circle, *box);
        }
        if (type1 == 'b' && type2 == 'b') {
            auto* box1 = dynamic_cast<Shape::Box<T>*>(obj1.shape);
            auto* box2 = dynamic_cast<Shape::Box<T>*>(obj2.shape);
            return rectRect(*box1, *box2);
        }

        return false;
    }

            // **** BASE IMPLEMENTATION IDEA **** //
    // template<typename T>
    // void CollisionProcess(Object<T>* obj1, Object<T>* obj2) {
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
    char type1 = obj1->shape->getType();
    char type2 = obj2->shape->getType();

    if (type1 == 'c' && type2 == 'c') {
        auto* circle1 = dynamic_cast<Shape::Circle<T>*>(obj1->shape);
        auto* circle2 = dynamic_cast<Shape::Circle<T>*>(obj2->shape);
        penetrationDepth = (circle1->radius + circle2->radius) - vDist;
    } else if (type1 == 'c' && type2 == 'b') {
        auto* circle = dynamic_cast<Shape::Circle<T>*>(obj1->shape);
        auto* box = dynamic_cast<Shape::Box<T>*>(obj2->shape);
        // Approximate penetration using circle-box distance
        Vector2<T> closestPoint(
            std::max(box->x - box->width/2, std::min(obj1->x, box->x + box->width/2)),
            std::max(box->y - box->height/2, std::min(obj1->y, box->y + box->height/2))
        );
        T distToBox = (obj1->Vector2Position() - closestPoint).magnitude();
        penetrationDepth = circle->radius - distToBox;
    } else if (type1 == 'b' && type2 == 'c') {
        auto* box = dynamic_cast<Shape::Box<T>*>(obj1->shape);
        auto* circle = dynamic_cast<Shape::Circle<T>*>(obj2->shape);
        Vector2<T> closestPoint(
            std::max(box->x - box->width/2, std::min(obj2->x, box->x + box->width/2)),
            std::max(box->y - box->height/2, std::min(obj2->y, box->y + box->height/2))
        );
        T distToBox = (obj2->Vector2Position() - closestPoint).magnitude();
        penetrationDepth = circle->radius - distToBox;
    } else if (type1 == 'b' && type2 == 'b') {
        auto* box1 = dynamic_cast<Shape::Box<T>*>(obj1->shape);
        auto* box2 = dynamic_cast<Shape::Box<T>*>(obj2->shape);
        // Simplified box-box penetration (along normal)
        T overlapX = std::min(box1->x + box1->width/2, box2->x + box2->width/2) -
                      std::max(box1->x - box1->width/2, box2->x - box2->width/2);
        T overlapY = std::min(box1->y + box1->height/2, box2->y + box2->height/2) -
                      std::max(box1->y - box1->height/2, box2->y - box2->height/2);
        penetrationDepth = std::min(overlapX, overlapY); // Minimum overlap direction
    }

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
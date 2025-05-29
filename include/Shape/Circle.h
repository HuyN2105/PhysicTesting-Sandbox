//
// Created by HuyN on 4/2/2025.
//
#pragma once

#include <Vector2.h>

#include "BaseShape.h"

#ifndef CIRCLE_H
#define CIRCLE_H

using HuyNVector::Vector2;

namespace Shape {
    template <typename T>
    class Circle final : public BaseShape<T> {
    public:

        // ****************************** CIRCLE INITIALIZATION ****************************** //
        T radius;

        Circle(T x_, T y_, T radius_) : BaseShape<T>('c'), radius(radius_) {
            this->x = x_;
            this->y = y_;
        }

        explicit constexpr Circle(Vector2<T> position, T radius_) : BaseShape<T>('c'), radius(radius_) {
            this->x = position.x;
            this->y = position.y;
        }

        void setPosition(T x_, T y_) override {
            this->x = x_;
            this->y = y_;
        }

        void setPosition(Vector2<T> position) override {
            this->x = position.x;
            this->y = position.y;
        }

        // *********************************** CIRCLE FUNCTIONS *********************************** //

        [[nodiscard]] BaseShape<T>* clone() const override {
            return new Circle<T>(*this);
        }

        [[nodiscard]] T area() const override {
            return M_PI * this->radius * this->radius;
        }

        [[nodiscard]] bool contains(Vector2<T> position) const override {
            return (position.distance(Vector2<T>(this->x, this->y)) <= this->radius);
        }

        [[nodiscard]] bool contains(T x_, T y_) const override {
            return ((Vector2<T>){x_, y_}.distance(Vector2<T>(this->x, this->y)) <= this->radius);
        }

        [[nodiscard]] T getBottom() const {
            return this->y + radius;
        }
        [[nodiscard]] constexpr T getRight() const {
            return this->x + radius;
        }

        [[nodiscard]] T distance(Circle& other) const noexcept {
            return Vector2<T>(this->x - other.x, this->y - other.y).magnitude();
        }

        // ******************************** BUILT-IN DRAW FUNCTIONS ******************************** //

        constexpr int SDL_DrawCircle(SDL_Renderer *renderer) {
            return SDL_RenderDrawCircle(renderer, this->x, this->y, this.radius);
        };

        constexpr int SDL_FillCircle(SDL_Renderer *renderer) {
            return SDL_RenderFillCircle(renderer, this->x, this->y, this.radius);
        }

    };

    constexpr int SDL_RenderDrawCircle(SDL_Renderer *renderer,const int x,const int y,const int radius) {
        int offsetX = 0;
        int offsetY = radius;
        int d = radius - 1;
        int status = 0;
        while (offsetY >= offsetX) {
            status += SDL_RenderDrawPoint(renderer, x + offsetX, y + offsetY);
            status += SDL_RenderDrawPoint(renderer, x + offsetY, y + offsetX);
            status += SDL_RenderDrawPoint(renderer, x - offsetX, y + offsetY);
            status += SDL_RenderDrawPoint(renderer, x - offsetY, y + offsetX);
            status += SDL_RenderDrawPoint(renderer, x + offsetX, y - offsetY);
            status += SDL_RenderDrawPoint(renderer, x + offsetY, y - offsetX);
            status += SDL_RenderDrawPoint(renderer, x - offsetX, y - offsetY);
            status += SDL_RenderDrawPoint(renderer, x - offsetY, y - offsetX);
            if (status < 0) {
                status = -1;
                break;
            }
            if (d >= 2*offsetX) {
                d -= 2*offsetX + 1;
                offsetX +=1;
            }
            else if (d < 2 * (radius - offsetY)) {
                d += 2 * offsetY - 1;
                offsetY -= 1;
            }
            else {
                d += 2 * (offsetY - offsetX - 1);
                offsetY -= 1;
                offsetX += 1;
            }
        }
        return status;
       };

    constexpr int SDL_RenderFillCircle(SDL_Renderer *renderer, const int x, const int y, const int radius) {
        int offsetX = 0;
        int offsetY = radius;
        int d = radius - 1;
        int status = 0;
        while (offsetY >= offsetX) {
            status += SDL_RenderDrawLine(renderer, x - offsetY, y + offsetX, x + offsetY, y + offsetX);
            status += SDL_RenderDrawLine(renderer, x - offsetX, y + offsetY, x + offsetX, y + offsetY);
            status += SDL_RenderDrawLine(renderer, x - offsetX, y - offsetY, x + offsetX, y - offsetY);
            status += SDL_RenderDrawLine(renderer, x - offsetY, y - offsetX, x + offsetY, y - offsetX);
            if (status < 0) {
                status = -1;
                break;
            }
            if (d >= 2*offsetX) {
                d -= 2*offsetX + 1;
                offsetX +=1;
            }
            else if (d < 2 * (radius - offsetY)) {
                d += 2 * offsetY - 1;
                offsetY -= 1;
            }
            else {
                d += 2 * (offsetY - offsetX - 1);
                offsetY -= 1;
                offsetX += 1;
            }
        }
        return status;
    };

}

#endif //CIRCLE_H
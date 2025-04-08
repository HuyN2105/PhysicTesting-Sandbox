//
// Created by HuyN on 4/2/2025.
//
#pragma once

#include <Vector2.h>

#ifndef CIRCLE_H
#define CIRCLE_H

using HuyNVector::Vector2;

namespace Shape {

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

    template <typename T>
    class Circle {
    public:

        // ****************************** CIRCLE INITIALIZATION ****************************** //

        T x;
        T y;
        T radius;

        constexpr Circle(T x, T y, T radius) : x(x), y(y), radius(radius) {}

        explicit constexpr Circle(Vector2<T>& position = {0, 0}, T radius = 0) : x(position.x), y(position.y), radius(radius) {}

        // ******************************** CIRCLE FUNCTIONS ******************************** //

        [[nodiscard]] T distance(Vector2<T>& other) const noexcept {
            return Vector2<T>(x - other.x, y - other.y).magnitude();
        }

        // ******************************** BUILT-IN DRAW FUNCTIONS ******************************** //

        constexpr int SDL_DrawCircle(SDL_Renderer *renderer) {
            return SDL_RenderDrawCircle(renderer, this.x, this.y, this.radius);
        };

        constexpr int SDL_FillCircle(SDL_Renderer *renderer) {
            return SDL_RenderFillCircle(renderer, this.x, this.y, this.radius);
        }

    };
}

#endif //CIRCLE_H

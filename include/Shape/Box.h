//
// Created by HuyN on 3/26/2025.
//
#pragma once

#include "BaseShape.h"
#include "Vector2.h"
#include "string"

#ifndef BOX_H
#define BOX_H

using HuyNVector::Vector2;

namespace Shape {

    template<typename T>
    class Box final : public BaseShape<T> {
    public:

        // ****************************** BOX INITIALIZATION ****************************** //
        T width;    // must be positive
        T height;   // must be positive

        Box(T x_, T y_, T width, T height) : BaseShape<T>('b'), width(width), height(height) {
            this->setPosition(x_, y_);
        }

        Box(Vector2<T> position, T width, T height) : BaseShape<T>('b'), width(width), height(height) {
            this->setPosition(position);
        }

        void setPosition(Vector2<T> position) override {
            this->x = position.x;
            this->y = position.y;
        }

        void setPosition(T x_, T y_) override {
            this->x = x_;
            this->y = y_;
        }

        // ******************************** BOX FUNCTIONS ******************************** //

        [[nodiscard]] BaseShape<T>* clone() const override {
            return new Box<T>(*this);
        }

        [[nodiscard]] constexpr T getRight() const noexcept { return this->x + width; }

        [[nodiscard]] constexpr T getBottom() const noexcept { return this->y + height; }

        [[nodiscard]] constexpr Vector2<T> getTopLeft() const noexcept { return Vector2<T>(this->x, this->y); }

        [[nodiscard]] constexpr Vector2<T> getCenter() const noexcept { return Vector2<T>(this->x + width / 2, this->y + height / 2); }

        [[nodiscard]] constexpr T area() const override { return this->width * this->height; }

        [[nodiscard]] bool contains(Vector2<T> position) const override {
            return (this->x <= position.x &&
                    this->getRight() >= position.x &&
                    this->y <= position.y &&
                    this->getBottom() >= position.y);
        }

        [[nodiscard]] bool contains(T x_, T y_) const override {
            return (this->x <= x_ &&
                    this->getRight() >= x_ &&
                    this->y <= y_ &&
                    this->getBottom() >= y_);
        }

        [[nodiscard]] constexpr bool intersects(const Box& other) const noexcept {
            return !(this->x >= other.getRight() ||
                     other.x >= getRight() ||
                     this->y >= other.getBottom() ||
                     other.y >= getBottom());
        }

        [[nodiscard]] constexpr Box subdivide(const std::string &quadrant) const noexcept {
            T halfWidth = this->width / 2;
            T halfHeight = this->height / 2;

            T extraWidth = static_cast<int>(this->width)%2;
            T extraHeight = static_cast<int>(this->height)%2;

            if (quadrant == "nw") return Box(this->x, this->y, halfWidth + extraWidth, halfHeight + extraHeight);
            if (quadrant == "ne") return Box(this->x + halfWidth, this->y, halfWidth + extraWidth, halfHeight + extraHeight);
            if (quadrant == "sw") return Box(this->x, this->y + halfHeight, halfWidth + extraWidth, halfHeight + extraHeight);
            if (quadrant == "se") return Box(this->x + halfWidth, this->y + halfHeight, halfWidth + extraWidth, halfHeight + extraHeight);

            return Box(0, 0, 0, 0);
        }


        // ******************************** BUILT-IN DRAW FUNCTIONS ******************************** //

        constexpr void SDL_DrawBox(SDL_Renderer *renderer) const noexcept {
            const SDL_Rect box{static_cast<int>(this->x), static_cast<int>(this->y), static_cast<int>(this.width), static_cast<int>(this.height)};
            SDL_RenderDrawRect(renderer, &box);
        }

        constexpr void SDL_FillBox(SDL_Renderer *renderer) const noexcept {
            const SDL_Rect box{static_cast<int>(this->x), static_cast<int>(this->y), static_cast<int>(this->width), static_cast<int>(this->height)};
            SDL_RenderFillRect(renderer, &box);
        }

    };
}

#endif //BOX_H
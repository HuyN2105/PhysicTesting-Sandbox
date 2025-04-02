//
// Created by HuyN on 3/26/2025.
//
#pragma once

#include <vector>

#include "Vector2.h"
#include "Box.h"

#ifndef QUADTREE_H
#define QUADTREE_H

namespace QuadTree {

    template<typename T>
    class QuadTree {
        public:

        // ****************************** QUADTREE INITIALIZATION ****************************** //

        int capacity;                   // default value: 4
        Box<T> boundary;
        bool divided;

        std::vector<Vector2<T>> points{};

        QuadTree *child[4]{};

        constexpr explicit QuadTree(Box<T> _boundary, const int _capacity = 4) : capacity(_capacity), boundary(_boundary), divided(false) {}

        // ********************************* QUADTREE FUNCTIONS ******************************** //

        [[nodiscard]] constexpr QuadTree *getChild(Vector2<T> _pos) {
            if (divided) {
                for (auto &c : child) {
                    if (c->boundary.contains(_pos)) {
                        return c->getChild(_pos);
                    }
                }
            }
            return this;
        }

        [[nodiscard]] constexpr Vector2<T> *getPoints(Vector2<T> _pos) {
            if (divided) {
                for (auto& c : child) {
                    if (c->boundary.contains(_pos)) {
                        return c->getPoints(_pos);
                    }
                }
            }
            return points;
        }

        [[nodiscard]] constexpr Box<T> *getBoundary(Vector2<T> _pos) {
            if (divided) {
                for (auto& c : child) {
                    if (c->boundary.contains(_pos)) {
                        return c->getBoundary(_pos);
                    }
                }
            }
            return &boundary;
        }

        [[nodiscard]] constexpr bool insert(const Vector2<T> _p) {
            if (!boundary.contains(_p)) return false;
            if (!divided) {
                if (points.size() < capacity) {
                    points.push_back(_p);
                    return true;
                }
                this->subdivide();
            }
            return (child[0]->insert(_p) ||
                    child[1]->insert(_p) ||
                    child[2]->insert(_p) ||
                    child[3]->insert(_p)
                    );
        }

        constexpr bool subdivide() {
            child[0] = new QuadTree(boundary.subdivide("ne"), capacity);
            child[1] = new QuadTree(boundary.subdivide("nw"), capacity);
            child[2] = new QuadTree(boundary.subdivide("se"), capacity);
            child[3] = new QuadTree(boundary.subdivide("sw"), capacity);

            divided = true;

            // Move points to children
            for (const auto &p : points) {
                const bool inserted =   child[0]->insert(p) ||
                                        child[1]->insert(p) ||
                                        child[2]->insert(p) ||
                                        child[3]->insert(p);

                if (!inserted) {
                    return false;
                }
            }
            points.clear();
            return true;
        }



    };
}

#endif //QUADTREE_H

//
// Created by HuyN on 4/2/2025.
//
#pragma once

#include <vector>
#include "Vector2.h"

#ifndef PHYSICENGINE_H
#define PHYSICENGINE_H


namespace HuyNPhysic {

    template <typename T>
    class Object {
        public:
        T x;
        T y;
        T velocity;

    };

}


#endif //PHYSICENGINE_H

//
// Created by Andrea Petreti on 03/11/2020.
//

#ifndef SWARM_GEN_CIRCLE_H
#define SWARM_GEN_CIRCLE_H

#include <argos3/core/utility/math/vector2.h>

struct Circle {
    double x;
    double y;
    double radius;

public:
    bool containsPoint(const argos::CVector2 point) const {
        argos::CVector2 center(x, y);
        return (point - center).Length() < radius;
    }
};

#endif //SWARM_GEN_CIRCLE_H

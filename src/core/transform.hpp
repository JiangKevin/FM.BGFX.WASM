#pragma once

#include "mathExtensions.hpp"
#include "constants.hpp"

namespace transform {

extern bx::Vec3       k_defaultPosition;
extern bx::Vec3       k_defaultScale;
extern bx::Vec3       k_defaultRotation;
extern bx::Quaternion k_defaultOrientation;

struct Transform {
    InterpTransform3f _position    = {k_defaultPosition,    k_defaultPosition};
    InterpTransform3f _scale       = {k_defaultScale,       k_defaultScale};
    InterpQuat4f      _orientation = {k_defaultOrientation, k_defaultOrientation};
    
    float _scaleRotationTranslationMatrix[16] = {0};
    float _speed = 8.0f;
};

void fixedUpdate(Transform&, float deltaTimeInSeconds);
void update(Transform&, float deltaTimeInSeconds);

/*
Directional vectors based upon the transforms quaternion rotation
*/
bx::Vec3 forward(Transform&);
bx::Vec3 right(Transform& transform);
bx::Vec3 up(Transform& transform);

/*

*/
void setDirection(Transform& transform, bx::Vec3 direction);
void setScale(Transform& transform, bx::Vec3 direction);
void setRotation(Transform& transform, bx::Quaternion direction);

/*

*/
bool moving(Transform& transform, float threshold = constants::k_epsilon);
bool scaling(Transform& transform, float threshold = constants::k_epsilon);
bool rotating(Transform& transform, float threshold = constants::k_epsilon);
}
#include "transform.hpp"
#include <iostream>

namespace transform {
bx::Vec3 k_defaultPosition          = {0.0f, 0.0f, 0.0f};
bx::Vec3 k_defaultScale             = {1.0f, 1.0f, 1.0f};
bx::Vec3 k_defaultRotation          = {0.0f, 0.0f, 0.0f};

bx::Quaternion k_defaultOrientation = {0.0f, 0.0f, 0.0f, 1.0f};

void fixedUpdate(Transform& transform, float deltaTimeInSeconds) {
    
   transform._position._current 
      = moveTowards(
          transform._position._current
        , transform._position._destination
        , deltaTimeInSeconds * transform._speed);

    transform._scale._current
      = moveTowards(
          transform._scale._current
        , transform._scale._destination
        , deltaTimeInSeconds * transform._speed);

    transform._orientation._current
      = rotateTowards(
          transform._orientation._current
        , transform._orientation._destination
        , deltaTimeInSeconds * 300.0f);
}

void update(Transform& transform, float deltaTimeInSeconds) {
    float T[16];
    bx::mtxTranslate(T,  transform._position._current.x,  transform._position._current.y,  transform._position._current.z);

    float S[16];
    bx::mtxScale(S, transform._scale._current.x, transform._scale._current.y, transform._scale._current.z);
    
    float R[16];
    bx::mtxQuat(R, transform._orientation._current);

    float SR[16];
    bx::mtxMul(SR, S, R);

    bx::mtxMul(transform._scaleRotationTranslationMatrix, SR, T);
}

bx::Vec3 forward(Transform& transform) {
    return bx::mul(constants::k_forward, transform._orientation._current);
}

bx::Vec3 right(Transform& transform) {
	return bx::mul(constants::k_right, transform._orientation._current);
}

bx::Vec3 up(Transform& transform) {
	return bx::mul(constants::k_up, transform._orientation._current);
}

void setDirection(Transform& transform, bx::Vec3 direction) {
    transform._position._destination = direction;
}

void setScale(Transform& transform, bx::Vec3 direction) {
    transform._scale._destination = direction;
}

void setRotation(Transform& transform, bx::Quaternion direction) {
    transform._orientation._destination = direction;
}

bool moving(Transform& transform, float threshold) {
    return bx::length(bx::sub(transform._position._destination, transform._position._current)) > threshold;
}

bool scaling(Transform& transform, float threshold) {
    return bx::length(bx::sub(transform._scale._destination, transform._scale._current)) > threshold;
}

bool rotating(Transform& transform, float threshold) {
    return bx::abs(bx::dot(transform._orientation._destination, transform._orientation._current)) <= 1.0f - threshold;
}
}
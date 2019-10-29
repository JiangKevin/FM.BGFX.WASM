#pragma once

#include <bx/math.h>

struct InterpTransform3f {
    bx::Vec3 _current;
    bx::Vec3 _destination;
};

struct InterpQuat4f {
    bx::Quaternion _current;
    bx::Quaternion _origin;
    bx::Quaternion _destination;
};

//Taken from http://physicsforgames.blogspot.com/2010/02/quaternions.html
bx::Quaternion nlerp(const bx::Quaternion& i, const bx::Quaternion& f, float blend);
bx::Quaternion LookRotation(bx::Vec3 source, bx::Vec3 destination, bx::Vec3 up);
bx::Vec3 moveTowards(bx::Vec3 current, bx::Vec3 target, float maxDistanceDelta);
float angle(bx::Quaternion a, bx::Quaternion b);
bx::Quaternion rotateTowards(bx::Quaternion from, bx::Quaternion to, float maxDegreesDelta);
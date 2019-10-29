#include "mathExtensions.hpp"

#include "constants.hpp"

//Taken from http://physicsforgames.blogspot.com/2010/02/quaternions.html
bx::Quaternion nlerp(const bx::Quaternion& i, const bx::Quaternion& f, float blend) {
    bx::Quaternion result;
    float dot = i.w * f.w + i.x * f.x + i.y * f.y + i.z * f.z;
    float blendI = 1.0f - blend;
    if (dot < 0.0f) {
        bx::Quaternion tmpF;
        tmpF.w = -f.w;
        tmpF.x = -f.x;
        tmpF.y = -f.y;
        tmpF.z = -f.z;
        result.w = blendI * i.w + blend * tmpF.w;
        result.x = blendI * i.x + blend * tmpF.x;
        result.y = blendI * i.y + blend * tmpF.y;
        result.z = blendI * i.z + blend * tmpF.z;
    }
    else {
        result.w = blendI * i.w + blend * f.w;
        result.x = blendI * i.x + blend * f.x;
        result.y = blendI * i.y + blend * f.y;
        result.z = blendI * i.z + blend * f.z;
    }
    result = bx::normalize(result);
    return result;
}

/// <summary>
/// Evaluates a rotation needed to be applied to an object positioned at sourcePoint to face destPoint
/// </summary>
/// <param name="sourcePoint">Coordinates of source point</param>
/// <param name="destPoint">Coordinates of destionation point</param>
/// <returns></returns>
bx::Quaternion LookAt(bx::Vec3 sourcePoint, bx::Vec3 destPoint, bx::Vec3 rotationAxis)
{
    bx::Vec3 forwardVector = bx::normalize(bx::sub(destPoint, sourcePoint));

    float dot = bx::dot({ 0.0f, 0.0f, 1.0f }, forwardVector);

    float rotAngle = bx::acos(dot);
    bx::Vec3 cross = bx::cross({ 0.0f, 0.0f, 1.0f }, forwardVector);
    if (bx::dot(rotationAxis, cross) < 0.0f) { // Or > 0
        rotAngle = -rotAngle;
    }

    return  bx::normalize(bx::rotateAxis(rotationAxis, rotAngle));
}

struct orthoNormalizedVectors {
    bx::Vec3 _normal;
    bx::Vec3 _tangent;
};

orthoNormalizedVectors orthoNormalize(bx::Vec3 normal, bx::Vec3 tangent) {
    normal = bx::normalize(normal);
    bx::Vec3 proj = bx::mul(normal, bx::dot(tangent, normal));

    tangent = bx::sub(tangent, proj);
    tangent = bx::normalize(tangent);
    return { normal, tangent };
}

bx::Quaternion mtxToQuaternion(float matrix[16]) {
    bx::Quaternion returnValue;
    //3x3 matrix
    float m00 = matrix[0];
    float m01 = matrix[1];
    float m02 = matrix[2];
    float m10 = matrix[4];
    float m11 = matrix[5];
    float m12 = matrix[6];
    float m20 = matrix[8];
    float m21 = matrix[9];
    float m22 = matrix[10];

    bx::Quaternion newQuaternion = { 0.0f, 0.0f, 0.0f, 1.0f };

    float tr = m00 + m11 + m22; // trace of matrix

    if (tr > 0.0f) {
        return { m12 - m21, m20 - m02, m01 - m10, tr + 1.0f };
    }

    if ((m00 > m11) && (m00 > m22)) {
        return { 1.0f + m00 - m11 - m22, m10 + m01, m20 + m02, m12 - m21 };
    }

    if (m11 > m22) {
        return { m10 + m01, 1.0f + m11 - m00 - m22, m21 + m12, m20 - m02 };
    }

    return { m20 + m02, m21 + m12, 1.0f + m22 - m00 - m11, m01 - m10 };
}

bx::Quaternion LookRotation(bx::Vec3 source, bx::Vec3 destination, bx::Vec3 up) {
    float matrix[16];
    bx::mtxLookAt(matrix, source, destination, up);
    return mtxToQuaternion(matrix);
}

bx::Vec3 moveTowards(bx::Vec3 current, bx::Vec3 target, float maxDistanceDelta) {
    bx::Vec3 direction = bx::sub(target, current);
    float length = bx::length(direction);
    if (length <= maxDistanceDelta || length <= constants::k_epsilon) {
        return target;
    }
    direction = bx::normalize(direction);
    return bx::add(current, bx::mul(direction, maxDistanceDelta));
}

float angle(bx::Quaternion a, bx::Quaternion b) {
    return bx::toDeg(bx::acos(bx::normalize(bx::mul(bx::invert(a), b)).z) * 2.0f);
}

bx::Quaternion rotateTowards(bx::Quaternion from, bx::Quaternion to, float maxDegreesDelta) {
    float angleValue = angle(from, to);
    if (angleValue <= constants::k_epsilon) {
        return to;
    }
    float ratio = bx::min(1.0f, maxDegreesDelta / angleValue);
    return nlerp(from, to, ratio);
}
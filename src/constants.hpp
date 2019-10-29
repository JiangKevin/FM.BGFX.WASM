#pragma once
#include <string>
namespace bx {
struct Vec3;
}
namespace constants {
extern uint32_t     k_width;
extern uint32_t     k_height;
extern std::string  k_gameName;
extern std::string  k_shaderLocation;
extern std::string  k_meshLocation;
extern bx::Vec3     k_cameraOffset;

extern bx::Vec3     k_empty;
extern bx::Vec3     k_up;
extern bx::Vec3     k_right;
extern bx::Vec3     k_forward;
extern bx::Vec3     k_backwards;

//TODO BX has a kfloatMin and max values use those
extern float        k_epsilon;
}
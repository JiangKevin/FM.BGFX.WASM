#include "constants.hpp"
#include <bx/math.h>

namespace constants {

uint32_t        k_width             = 1280;
uint32_t        k_height            = 720;
std::string     k_gameName          = "GeometricChallenge";
std::string     k_shaderLocation    = "../../assets/shaders";
std::string     k_meshLocation      = "../../assets/meshes/";
std::string     k_textureLocation   = "../../assets/textures/";
bx::Vec3        k_cameraOffset      = {0.0f, 10.0f, -10.0f};

bx::Vec3        k_empty             = {0.0f, 0.0f, 0.0f};
bx::Vec3        k_up                = {0.0f, 1.0f, 0.0f};
bx::Vec3        k_right             = {1.0f, 0.0f, 0.0f};
bx::Vec3        k_forward           = {0.0f, 0.0f, 1.0f};
bx::Vec3        k_backwards         = {0.0f, 0.0f, -1.0f};

float           k_epsilon           = 0.001f;
}
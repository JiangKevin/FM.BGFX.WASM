#include "wireframe.hpp"

#include <bgfx/bgfx.h>

#include "core/material.hpp"
#include "core/camera.hpp"
#include "bgfxExtensions/shader.hpp"

namespace wireframe {
static const char* k_vertexShader = "vs_wf_mesh";
static const char* k_fragmentShader = "fs_wf_mesh";

void init(Material& mat, Material::MaterialCustomContent& matContent) {
    Wireframe& wireMat = std::get<Wireframe>(matContent);

    mat.program = loadProgram(k_vertexShader, k_fragmentShader);
    wireMat.uniformHandle = bgfx::createUniform("u_params", bgfx::UniformType::Vec4, NumVec4);
}

void update(Material& mat, Material::MaterialCustomContent& matContent) {
    Wireframe& wireMat = std::get<Wireframe>(matContent);

    // Update camera position
    bx::Vec3 mainCameraPosition = camera::mainCameraPosition();
    wireMat._camPos[0] = mainCameraPosition.x;
    wireMat._camPos[1] = mainCameraPosition.y;
    wireMat._camPos[2] = mainCameraPosition.z;

    wireMat._drawEdges = (wireMat._drawMode == DrawMode::WireframeShaded) ? 1.0f : 0.0f;
    // Submit values
    bgfx::setUniform(wireMat.uniformHandle, wireMat._params, 3);
}

void destroy(Material& mat, Material::MaterialCustomContent& matContent) {
    Wireframe& wireMat = std::get<Wireframe>(matContent);

    bgfx::destroy(wireMat.uniformHandle);
}

// Create the actual objects but don't invoke anything render related
Material createInstance() {
    Wireframe wireMat;

    wireMat._camPos[0] = 0.0f;
    wireMat._camPos[1] = 1.0f;
    wireMat._camPos[2] = -2.5f;
    wireMat._wfColor[0] = 1.0f;
    wireMat._wfColor[1] = 0.0f;
    wireMat._wfColor[2] = 0.0f;
    wireMat._wfColor[3] = 1.0f;
    wireMat._drawEdges = 0.0f;
    wireMat._wfThickness = 1.5f;
    wireMat._drawMode = DrawMode::WireframeShaded;

    Material::MaterialCustomContent content(wireMat);

    Material::MaterialFunctor initFunctor = init;
    Material::MaterialFunctor updateFunctor = update;
    Material::MaterialFunctor destroyFunctor = destroy;
    return Material(content, initFunctor, updateFunctor, destroyFunctor);
}
}
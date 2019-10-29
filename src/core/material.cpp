#include "material.hpp"

Material::Material(
    MaterialCustomContent& customContent
    , MaterialFunctor& customInit
    , MaterialFunctor& customUpdate
    , MaterialFunctor& customDestroy) {

    content = customContent;
    this->customInit = customInit;
    this->customUpdate = customUpdate;
    this->customDestroy = customDestroy;
}

void init(Material& mat) {
    // Default material state
    mat.state = 0
        | BGFX_STATE_WRITE_RGB
        | BGFX_STATE_WRITE_A
        | BGFX_STATE_DEPTH_TEST_LESS
        | BGFX_STATE_WRITE_Z
        | BGFX_STATE_CULL_CCW
        | BGFX_STATE_MSAA;
    mat.customInit(mat, mat.content);
}

void update(Material& mat) {
    mat.customUpdate(mat, mat.content);
}

void destroy(Material& mat) {
    bgfx::destroy(mat.program);
    bgfx::destroy(mat.uniformHandle);
    mat.customDestroy(mat, mat.content);
}
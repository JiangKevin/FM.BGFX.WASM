#pragma once

#include <functional>
#include <variant>

#include <bgfx/bgfx.h>

#include "materials/wireframe.hpp"
#include "materials/texture.hpp"

// Base where all the material types rely upon
struct Material {
    using MaterialCustomContent = std::variant<wireframe::Wireframe, Texture>;
    using MaterialFunctor = std::function<void(Material& mat, MaterialCustomContent& content)>;
    
    MaterialCustomContent content = wireframe::Wireframe();

    bgfx::UniformHandle uniformHandle = { 0 };
    bgfx::ProgramHandle program = { 0 };
    uint64_t state = 0;

    MaterialFunctor customInit = [](Material& mat, MaterialCustomContent& content) {};
    MaterialFunctor customUpdate = [](Material& mat, MaterialCustomContent& content) {};
    MaterialFunctor customDestroy = [](Material& mat, MaterialCustomContent& content) {};

    Material() {}
    Material(
        MaterialCustomContent& customContent
        , MaterialFunctor& customInit
        , MaterialFunctor& customUpdate
        , MaterialFunctor& customDestroy);
};

void init(Material& mat);
void update(Material& mat);
void destroy(Material& mat);
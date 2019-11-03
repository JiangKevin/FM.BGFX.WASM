#pragma once
#include <bgfx/bgfx.h>
#include <string>

class Material;

struct Texture
{
    bgfx::UniformHandle diffuseUniformHandle;
    bgfx::UniformHandle normalUniformHandle;

    std::string diffuseTexture;
    std::string normalTexture;

    bgfx::TextureHandle textureDiffuse;
    bgfx::TextureHandle textureNormal;
};

Material createTextureMatInstance(
    const std::string& diffuseTextureName,
    const std::string& normalTextureName);
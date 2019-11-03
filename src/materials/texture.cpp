#include "texture.hpp"

#include <bgfx/bgfx.h>

#include "core/material.hpp"
#include "core/camera.hpp"
#include "bgfxExtensions/shader.hpp"

static const char* k_vertexShader = "vs_texture";
static const char* k_fragmentShader = "fs_texture";

static const int k_lightCount = 4;

namespace texture {

void init(Material& mat, Material::MaterialCustomContent& matContent) {
    Texture& textureMat = std::get<Texture>(matContent);

    mat.program = loadProgram(k_vertexShader, k_fragmentShader);

    textureMat.diffuseUniformHandle = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    textureMat.normalUniformHandle = bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);

    textureMat.textureDiffuse = loadTexture((constants::k_textureLocation + textureMat.diffuseTexture).c_str());
    textureMat.textureNormal = loadTexture((constants::k_textureLocation + textureMat.normalTexture).c_str());
}

void update(Material& mat, Material::MaterialCustomContent& matContent) {
    Texture& textureMat = std::get<Texture>(matContent);

    // Bind textures.
    bgfx::setTexture(0, textureMat.diffuseUniformHandle, textureMat.textureDiffuse);
    bgfx::setTexture(1, textureMat.normalUniformHandle, textureMat.textureNormal);
}

void destroy(Material& mat, Material::MaterialCustomContent& matContent) {
    Texture& textureMat = std::get<Texture>(matContent);

    bgfx::destroy(textureMat.diffuseUniformHandle);
    bgfx::destroy(textureMat.normalUniformHandle);

    bgfx::destroy(textureMat.textureDiffuse);
    bgfx::destroy(textureMat.textureNormal);
}
}
// Create the actual objects but don't invoke anything render related
Material createTextureMatInstance(
    const std::string& diffuseTextureName, 
    const std::string& normalTextureName) {
    Texture textureMat;

    textureMat.diffuseTexture = diffuseTextureName;
    textureMat.normalTexture = normalTextureName;

    Material::MaterialCustomContent content(textureMat);

    Material::MaterialFunctor initFunctor = texture::init;
    Material::MaterialFunctor updateFunctor = texture::update;
    Material::MaterialFunctor destroyFunctor = texture::destroy;
    return Material(content, initFunctor, updateFunctor, destroyFunctor);
}
#include "shader.hpp"

#include <bx/file.h>
#include <string>

#include "bgfxHelperContainer.hpp"
#include "../constants.hpp"

static bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const char* _name) {
	std::string shaderPath = "???";

	switch (bgfx::getRendererType() ) {
        case bgfx::RendererType::Noop:
        case bgfx::RendererType::Direct3D9:  shaderPath = constants::k_shaderLocation + std::string("/dx9/");   break;
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12: shaderPath = constants::k_shaderLocation + std::string("/dx11/");  break;
        case bgfx::RendererType::Gnm:        shaderPath = constants::k_shaderLocation + std::string("/pssl/");  break;
        case bgfx::RendererType::Metal:      shaderPath = constants::k_shaderLocation + std::string("/metal/"); break;
        case bgfx::RendererType::Nvn:        shaderPath = constants::k_shaderLocation + std::string("/nvn/");   break;
        case bgfx::RendererType::OpenGL:     shaderPath = constants::k_shaderLocation + std::string("/glsl/");  break;
        case bgfx::RendererType::OpenGLES:   shaderPath = constants::k_shaderLocation + std::string("/essl/");  break;
        case bgfx::RendererType::Vulkan:     shaderPath = constants::k_shaderLocation + std::string("/spirv/"); break;

        case bgfx::RendererType::Count:
            BX_CHECK(false, "You should not be here!");
            break;
	}
    shaderPath = shaderPath + _name + ".bin";

	bgfx::ShaderHandle handle = bgfx::createShader(loadMem(_reader, shaderPath.c_str()) );
	bgfx::setName(handle, _name);

	return handle;
}

bgfx::ShaderHandle loadShader(const char* _name)
{
	return loadShader(iocContainerHelper::getFileReader(), _name);
}

bgfx::ProgramHandle loadProgram(bx::FileReaderI* _reader, const char* _vsName, const char* _fsName)
{
	bgfx::ShaderHandle vsh = loadShader(_reader, _vsName);
	bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
	if (NULL != _fsName)
	{
		fsh = loadShader(_reader, _fsName);
	}

	return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
}

bgfx::ProgramHandle loadProgram(const char* _vsName, const char* _fsName)
{
	return loadProgram(iocContainerHelper::getFileReader(), _vsName, _fsName);
}
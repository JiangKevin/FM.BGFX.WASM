/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#pragma once 
#include <bx/pixelformat.h>
#include <bgfx/bgfx.h>
#include <bimg/bimg.h>
#include "bounds.h"

#include <tinystl/allocator.h>
#include <tinystl/vector.h>

namespace bx {
class FileReaderI;
class AllocatorI;	
}

///
void* load(const char* _filePath, uint32_t* _size = NULL);

///
void unload(void* _ptr);

const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath);

void* loadMem(bx::FileReaderI* _reader, bx::AllocatorI* _allocator, const char* _filePath, uint32_t* _size);

///
bgfx::TextureHandle loadTexture(const char* _name, uint64_t _flags = BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE, uint8_t _skip = 0, bgfx::TextureInfo* _info = NULL, bimg::Orientation::Enum* _orientation = NULL);

///
bimg::ImageContainer* imageLoad(const char* _filePath, bgfx::TextureFormat::Enum _dstFormat);

///
void calcTangents(void* _vertices, uint16_t _numVertices, bgfx::VertexDecl _decl, const uint16_t* _indices, uint32_t _numIndices);

/// Returns true if both internal transient index and vertex buffer have
/// enough space.
///
/// @param[in] _numVertices Number of vertices.
/// @param[in] _decl Vertex declaration.
/// @param[in] _numIndices Number of indices.
///
inline bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexDecl& _decl, uint32_t _numIndices)
{
	return _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _decl)
		&& (0 == _numIndices || _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices) )
		;
}

///
inline uint32_t encodeNormalRgba8(float _x, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
{
	const float src[] =
	{
		_x * 0.5f + 0.5f,
		_y * 0.5f + 0.5f,
		_z * 0.5f + 0.5f,
		_w * 0.5f + 0.5f,
	};
	uint32_t dst;
	bx::packRgba8(&dst, src);
	return dst;
}

///
struct Args
{
	Args(int _argc, const char* const* _argv);

	bgfx::RendererType::Enum m_type;
	uint16_t m_pciId;
};

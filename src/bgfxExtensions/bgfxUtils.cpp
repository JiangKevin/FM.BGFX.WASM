/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */
#include "bgfxHelperContainer.hpp"
#include <iostream>

#include <bgfx/bgfx.h>
#include <bx/commandline.h>
#include <bx/endian.h>
#include <bx/math.h>
#include <bx/readerwriter.h>
#include <bx/string.h>
#include <bx/allocator.h>
#include <bx/file.h>

#include "bgfxUtils.h"

#include <bimg/decode.h>

void* load(bx::FileReaderI* _reader, bx::AllocatorI* _allocator, const char* _filePath, uint32_t* _size)
{
	if (bx::open(_reader, _filePath) )
	{
		uint32_t size = (uint32_t)bx::getSize(_reader);
		void* data = BX_ALLOC(_allocator, size);
		bx::read(_reader, data, size);
		bx::close(_reader);
		if (NULL != _size)
		{
			*_size = size;
		}
		return data;
	}
	else
	{
		std::cout << "Failed to open:"<< _filePath << std::endl;
	}

	if (NULL != _size)
	{
		*_size = 0;
	}

	return NULL;
}

void* load(const char* _filePath, uint32_t* _size)
{
	return load(iocContainerHelper::getFileReader(), iocContainerHelper::getAllocator(), _filePath, _size);
}

void unload(void* _ptr)
{
	BX_FREE(iocContainerHelper::getAllocator(), _ptr);
}

const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
{
	if (bx::open(_reader, _filePath) )
	{
		uint32_t size = (uint32_t)bx::getSize(_reader);
		const bgfx::Memory* mem = bgfx::alloc(size+1);
		bx::read(_reader, mem->data, size);
		bx::close(_reader);
		mem->data[mem->size-1] = '\0';
		return mem;
	}
	std::cout << "Failed to load:"<< _filePath << std::endl;
	return NULL;
}

void* loadMem(bx::FileReaderI* _reader, bx::AllocatorI* _allocator, const char* _filePath, uint32_t* _size)
{
	if (bx::open(_reader, _filePath) )
	{
		uint32_t size = (uint32_t)bx::getSize(_reader);
		void* data = BX_ALLOC(_allocator, size);
		bx::read(_reader, data, size);
		bx::close(_reader);

		if (NULL != _size)
		{
			*_size = size;
		}
		return data;
	}

	std::cout << "Failed to load:"<< _filePath << std::endl;
	return NULL;
}

static void imageReleaseCb(void* _ptr, void* _userData)
{
	BX_UNUSED(_ptr);
	bimg::ImageContainer* imageContainer = (bimg::ImageContainer*)_userData;
	bimg::imageFree(imageContainer);
}

bgfx::TextureHandle loadTexture(bx::FileReaderI* _reader, const char* _filePath, uint64_t _flags, uint8_t _skip, bgfx::TextureInfo* _info, bimg::Orientation::Enum* _orientation)
{
	BX_UNUSED(_skip);
	bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

	uint32_t size;
	void* data = load(_reader, iocContainerHelper::getAllocator(), _filePath, &size);
	if (NULL != data)
	{
		bimg::ImageContainer* imageContainer = bimg::imageParse(iocContainerHelper::getAllocator(), data, size);

		if (NULL != imageContainer)
		{
			if (NULL != _orientation)
			{
				*_orientation = imageContainer->m_orientation;
			}

			const bgfx::Memory* mem = bgfx::makeRef(
					  imageContainer->m_data
					, imageContainer->m_size
					, imageReleaseCb
					, imageContainer
					);
			unload(data);

			if (imageContainer->m_cubeMap)
			{
				handle = bgfx::createTextureCube(
					  uint16_t(imageContainer->m_width)
					, 1 < imageContainer->m_numMips
					, imageContainer->m_numLayers
					, bgfx::TextureFormat::Enum(imageContainer->m_format)
					, _flags
					, mem
					);
			}
			else if (1 < imageContainer->m_depth)
			{
				handle = bgfx::createTexture3D(
					  uint16_t(imageContainer->m_width)
					, uint16_t(imageContainer->m_height)
					, uint16_t(imageContainer->m_depth)
					, 1 < imageContainer->m_numMips
					, bgfx::TextureFormat::Enum(imageContainer->m_format)
					, _flags
					, mem
					);
			}
			else if (bgfx::isTextureValid(0, false, imageContainer->m_numLayers, bgfx::TextureFormat::Enum(imageContainer->m_format), _flags) )
			{
				handle = bgfx::createTexture2D(
					  uint16_t(imageContainer->m_width)
					, uint16_t(imageContainer->m_height)
					, 1 < imageContainer->m_numMips
					, imageContainer->m_numLayers
					, bgfx::TextureFormat::Enum(imageContainer->m_format)
					, _flags
					, mem
					);
			}

			if (bgfx::isValid(handle) )
			{
				bgfx::setName(handle, _filePath);
			}

			if (NULL != _info)
			{
				bgfx::calcTextureSize(
					  *_info
					, uint16_t(imageContainer->m_width)
					, uint16_t(imageContainer->m_height)
					, uint16_t(imageContainer->m_depth)
					, imageContainer->m_cubeMap
					, 1 < imageContainer->m_numMips
					, imageContainer->m_numLayers
					, bgfx::TextureFormat::Enum(imageContainer->m_format)
					);
			}
		}
	}

	return handle;
}

bgfx::TextureHandle loadTexture(const char* _name, uint64_t _flags, uint8_t _skip, bgfx::TextureInfo* _info, bimg::Orientation::Enum* _orientation)
{
	return loadTexture(iocContainerHelper::getFileReader(), _name, _flags, _skip, _info, _orientation);
}

bimg::ImageContainer* imageLoad(const char* _filePath, bgfx::TextureFormat::Enum _dstFormat)
{
	uint32_t size = 0;
	void* data = loadMem(iocContainerHelper::getFileReader(), iocContainerHelper::getAllocator(), _filePath, &size);

	return bimg::imageParse(iocContainerHelper::getAllocator(), data, size, bimg::TextureFormat::Enum(_dstFormat) );
}

void calcTangents(void* _vertices, uint16_t _numVertices, bgfx::VertexDecl _decl, const uint16_t* _indices, uint32_t _numIndices)
{
	struct PosTexcoord
	{
		float m_x;
		float m_y;
		float m_z;
		float m_pad0;
		float m_u;
		float m_v;
		float m_pad1;
		float m_pad2;
	};

	float* tangents = new float[6*_numVertices];
	bx::memSet(tangents, 0, 6*_numVertices*sizeof(float) );

	PosTexcoord v0;
	PosTexcoord v1;
	PosTexcoord v2;

	for (uint32_t ii = 0, num = _numIndices/3; ii < num; ++ii)
	{
		const uint16_t* indices = &_indices[ii*3];
		uint32_t i0 = indices[0];
		uint32_t i1 = indices[1];
		uint32_t i2 = indices[2];

		bgfx::vertexUnpack(&v0.m_x, bgfx::Attrib::Position, _decl, _vertices, i0);
		bgfx::vertexUnpack(&v0.m_u, bgfx::Attrib::TexCoord0, _decl, _vertices, i0);

		bgfx::vertexUnpack(&v1.m_x, bgfx::Attrib::Position, _decl, _vertices, i1);
		bgfx::vertexUnpack(&v1.m_u, bgfx::Attrib::TexCoord0, _decl, _vertices, i1);

		bgfx::vertexUnpack(&v2.m_x, bgfx::Attrib::Position, _decl, _vertices, i2);
		bgfx::vertexUnpack(&v2.m_u, bgfx::Attrib::TexCoord0, _decl, _vertices, i2);

		const float bax = v1.m_x - v0.m_x;
		const float bay = v1.m_y - v0.m_y;
		const float baz = v1.m_z - v0.m_z;
		const float bau = v1.m_u - v0.m_u;
		const float bav = v1.m_v - v0.m_v;

		const float cax = v2.m_x - v0.m_x;
		const float cay = v2.m_y - v0.m_y;
		const float caz = v2.m_z - v0.m_z;
		const float cau = v2.m_u - v0.m_u;
		const float cav = v2.m_v - v0.m_v;

		const float det = (bau * cav - bav * cau);
		const float invDet = 1.0f / det;

		const float tx = (bax * cav - cax * bav) * invDet;
		const float ty = (bay * cav - cay * bav) * invDet;
		const float tz = (baz * cav - caz * bav) * invDet;

		const float bx = (cax * bau - bax * cau) * invDet;
		const float by = (cay * bau - bay * cau) * invDet;
		const float bz = (caz * bau - baz * cau) * invDet;

		for (uint32_t jj = 0; jj < 3; ++jj)
		{
			float* tanu = &tangents[indices[jj]*6];
			float* tanv = &tanu[3];
			tanu[0] += tx;
			tanu[1] += ty;
			tanu[2] += tz;

			tanv[0] += bx;
			tanv[1] += by;
			tanv[2] += bz;
		}
	}

	for (uint32_t ii = 0; ii < _numVertices; ++ii)
	{
		const bx::Vec3 tanu = bx::load<bx::Vec3>(&tangents[ii*6]);
		const bx::Vec3 tanv = bx::load<bx::Vec3>(&tangents[ii*6 + 3]);

		float nxyzw[4];
		bgfx::vertexUnpack(nxyzw, bgfx::Attrib::Normal, _decl, _vertices, ii);

		const bx::Vec3 normal  = bx::load<bx::Vec3>(nxyzw);
		const float    ndt     = bx::dot(normal, tanu);
		const bx::Vec3 nxt     = bx::cross(normal, tanu);
		const bx::Vec3 tmp     = bx::sub(tanu, bx::mul(normal, ndt) );

		float tangent[4];
		bx::store(tangent, bx::normalize(tmp) );
		tangent[3] = bx::dot(nxt, tanv) < 0.0f ? -1.0f : 1.0f;

		bgfx::vertexPack(tangent, true, bgfx::Attrib::Tangent, _decl, _vertices, ii);
	}

	delete [] tangents;
}

namespace bgfx
{
	int32_t read(bx::ReaderI* _reader, bgfx::VertexDecl& _decl, bx::Error* _err = NULL);
}
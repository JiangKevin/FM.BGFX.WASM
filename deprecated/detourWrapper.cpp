#include "detourWrapper.hpp"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>

#include <bx/math.h>

#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourCrowd.h"

#include "constants.hpp"
#include "bgfxExtensions/mesh.hpp"

namespace detourWrapper {

static const int NAVMESHSET_MAGIC = 'M'<<24 | 'S'<<16 | 'E'<<8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;
static const int MAX_POLYS = 256;

struct NavMeshSetHeader {
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader {
	dtTileRef tileRef;
	int dataSize;
};

enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_DISABLED	= 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL		= 0xffff	// All
};

enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND
};

void saveNavMesh(const std::string& path, const dtNavMesh* mesh) {
	if (!mesh) return;

	FILE* fp = fopen(path.c_str(), "wb");
	if (!fp)
		return;

	// Store header.
	NavMeshSetHeader header;
	header.magic = NAVMESHSET_MAGIC;
	header.version = NAVMESHSET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header.numTiles++;
	}
	memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
	fwrite(&header, sizeof(NavMeshSetHeader), 1, fp);

	// Store tiles.
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

		fwrite(tile->data, tile->dataSize, 1, fp);
	}

	fclose(fp);
}

dtNavMesh* loadNavmeshFromRecastFile(const std::string& name) {
	FILE* fp = fopen((constants::k_meshLocation+name).c_str(), "rb");
	if (!fp) 
        return nullptr;

	// Read header.
	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1) {
		fclose(fp);
		return nullptr;
	}
	if (header.magic != NAVMESHSET_MAGIC) {
		fclose(fp);
		return nullptr;
	}
	if (header.version != NAVMESHSET_VERSION) {
		fclose(fp);
		return nullptr;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh) {
		fclose(fp);
		return nullptr;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status)) {
		fclose(fp);
		return nullptr;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i) {
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (readLen != 1) {
			fclose(fp);
			return nullptr;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1) {
			dtFree(data);
			fclose(fp);
			return nullptr;
		}
		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	fclose(fp);

	return mesh;
}

dtNavMeshQuery* createQuery(dtNavMesh* navmesh) {
    dtNavMeshQuery* query = dtAllocNavMeshQuery();
    query->init(navmesh, 2048);
    return query;
}

const float searchDistance[3] = {10,10,10};

//Converted from pseudo code in https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
bx::Vec3 calculateSurfaceNormal(const dtMeshTile* meshTile, const dtPoly* polyTile) {
	bx::Vec3 surfaceNormal = {0.0f, 0.0f, 0.0f};
	size_t vertexCount = (size_t)polyTile->vertCount;
	for(size_t index = 0; index < vertexCount; ++index) {
		float* vertex = &meshTile->verts[polyTile->verts[index]*3];
		bx::Vec3 current = {vertex[0], vertex[1], vertex[2]};
		
		vertex = &meshTile->verts[polyTile->verts[(index+1)%vertexCount]*3];
		bx::Vec3 next = {vertex[0], vertex[1], vertex[2]};

		surfaceNormal.x += (current.y - next.y) * (current.z + next.z);
		surfaceNormal.y += (current.z - next.z) * (current.x + next.x);
		surfaceNormal.z += (current.x - next.x) * (current.y + next.y);
	}

	if(bx::length(surfaceNormal) <= constants::k_epsilon) {
		return {0.0f, 0.0f, 0.0f};
	}

	return bx::normalize(surfaceNormal);
}

NavmeshPoint findNearestPoint(dtNavMeshQuery* query, bx::Vec3 position) {
    dtQueryFilter filter;
    filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
    filter.setExcludeFlags(0);

	// Get the nearest poly
	dtPolyRef polyRef;
	float floatVector[3] = {position.x, position.y, position.z};
	query->findNearestPoly(floatVector, searchDistance, &filter, &polyRef, floatVector);

	// Calculate surface normal
	const dtMeshTile* bestTile	= nullptr;
	const dtPoly* bestPoly 		= nullptr;
	query->getAttachedNavMesh()->getTileAndPolyByRef(polyRef, &bestTile, &bestPoly);

	bx::Vec3 surfaceNormal = {0.0f, 0.0f, 0.0f};
	if(bestTile != nullptr && bestPoly != nullptr) {
 		surfaceNormal = calculateSurfaceNormal(bestTile, bestPoly);
	}
	
	return {{floatVector[0], floatVector[1], floatVector[2]}, surfaceNormal};
}

std::vector<NavmeshPoint> findPath(dtNavMeshQuery* query, bx::Vec3 startPosition, bx::Vec3 endPosition) {
	std::vector<NavmeshPoint> path;

    dtPolyRef polyPath[MAX_POLYS];

    dtQueryFilter filter;
    filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
    filter.setExcludeFlags(0);

    dtPolyRef startRef, endRef;
    float startFloatVector[3] = {startPosition.x, startPosition.y, startPosition.z};
    float endFloatVector[3] = {endPosition.x, endPosition.y, endPosition.z};
    query->findNearestPoly(startFloatVector, searchDistance, &filter, &startRef, startFloatVector);
    query->findNearestPoly(endFloatVector, searchDistance, &filter, &endRef, endFloatVector);

   	int pathSize = 0;
	query->findPath(startRef, endRef, startFloatVector, endFloatVector, &filter, polyPath, &pathSize, MAX_POLYS);

	if (!startRef || !endRef ) {
		std::cout << "Generated Empty Path" << std::endl;
		return {};
	}

	static const int MAX_STEER_POINTS = 256;
	float steerPath[MAX_STEER_POINTS*3];
	unsigned char steerPathFlags[MAX_STEER_POINTS];
	dtPolyRef steerPathPolys[MAX_STEER_POINTS];
	int nsteerPath = 0;

	query->findStraightPath(startFloatVector, endFloatVector, polyPath, pathSize,
							   steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);

	for(int i = 0, z = 0; i < nsteerPath * 3; z++) {
		// Get the polygon to calculate surface normal
		const dtPolyRef polyRef 	= steerPathPolys[z];
		const dtMeshTile* bestTile	= 0;
		const dtPoly* bestPoly 		= 0;
		query->getAttachedNavMesh()->getTileAndPolyByRef(polyRef, &bestTile, &bestPoly);

		bx::Vec3 surfaceNormal = calculateSurfaceNormal(bestTile, bestPoly);

		bx::Vec3 position = {
			steerPath[i++],
			steerPath[i++],
			steerPath[i++]
		};
		NavmeshPoint point = {position, surfaceNormal};
		path.emplace_back(point);
	}

	return path;
}

void freeQuery(dtNavMeshQuery* query) {
    dtFreeNavMeshQuery(query);
}

void freeNavmesh(dtNavMesh* navmesh) {
    dtFreeNavMesh(navmesh);
}
}
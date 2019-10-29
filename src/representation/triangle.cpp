#include "triangle.hpp"
#include "core/mathExtensions.hpp"

namespace triangle {
Triangle s_defaultTriangle = Triangle();

bx::Vec3 surfaceNormal(Triangle& triangle) {
    bx::Vec3 a = triangle._verts[0]._vertex;
    bx::Vec3 b = triangle._verts[1]._vertex;
    bx::Vec3 c = triangle._verts[2]._vertex;
	return bx::cross(bx::sub(b,a), bx::sub(c,a));
}

bx::Vec3 barycentricUVW(Triangle& triangle, const bx::Vec3 &point) {
	// Implementation from Christer Ericson's Real-Time Collision Detection, pp. 51-52.

    bx::Vec3 a = triangle._verts[0]._vertex;
    bx::Vec3 b = triangle._verts[1]._vertex;
    bx::Vec3 c = triangle._verts[2]._vertex;

	// Unnormalized triangle normal.
	bx::Vec3 m = bx::cross(bx::sub(b, a), 
                           bx::sub(c, a));

	// Nominators and one-over-denominator for u and v ratios.
	float nu, nv, ood;

	// Absolute components for determining projection plane.
	float x = bx::abs(m.x);
	float y = bx::abs(m.y);
	float z = bx::abs(m.z);

	if (x >= y && x >= z)
	{
		// Project to the yz plane.
		nu = triArea2D(point.y, point.z, b.y, b.z, c.y, c.z); // Area of PBC in yz-plane.
		nv = triArea2D(point.y, point.z, c.y, c.z, a.y, a.z); // Area OF PCA in yz-plane.
		ood = 1.f / m.x; // 1 / (2*area of ABC in yz plane)
	}
	else if (y >= z) // Note: The book has a redundant 'if (y >= x)' comparison
	{
		// y is largest, project to the xz-plane.
		nu = triArea2D(point.x, point.z, b.x, b.z, c.x, c.z);
		nv = triArea2D(point.x, point.z, c.x, c.z, a.x, a.z);
		ood = 1.f / -m.y;
	}
	else // z is largest, project to the xy-plane.
	{
		nu = triArea2D(point.x, point.y, b.x, b.y, c.x, c.y);
		nv = triArea2D(point.x, point.y, c.x, c.y, a.x, a.y);
		ood = 1.f / m.z;
	}
	float u = nu * ood;
	float v = nv * ood;
	float w = 1.f - u - v;
	return {u, v, w};
}

bool barycentricInsideTriangle(const bx::Vec3 &barycentric) {
	return barycentric.x >= 0.f && barycentric.y >= 0.f && barycentric.z >= 0.f &&
		bx::equal(barycentric.x + barycentric.y + barycentric.z, 1.f, bx::kFloatMin);
}

/// Implementation from Christer Ericson's Real-Time Collision Detection, pp. 51-52.
inline float triArea2D(float x1, float y1, float x2, float y2, float x3, float y3) {
	return (x1-x2)*(y2-y3) - (x2-x3)*(y1-y2);
}

bool positionInsideTriangle(Triangle& triangleReference, bx::Vec3 query) {
    bx::Vec3 uvw = barycentricUVW(triangleReference, query);
    if(barycentricInsideTriangle(uvw)) {
        return true;
    }
    return false;
}


bx::Vec3 closestPoint(Triangle& triangle, const bx::Vec3 &p) {
	/** The code for Triangle-float3 test is from Christer Ericson's Real-Time Collision Detection, pp. 141-142. */

    bx::Vec3 a = triangle._verts[0]._vertex;
    bx::Vec3 b = triangle._verts[1]._vertex;
    bx::Vec3 c = triangle._verts[2]._vertex;

	// Check if P is in vertex region outside A.
	bx::Vec3 ab = bx::sub(b, a);
	bx::Vec3 ac = bx::sub(c, a);
	bx::Vec3 ap = bx::sub(p, a);
	float d1 = bx::dot(ab, ap);
	float d2 = bx::dot(ac, ap);
	if (d1 <= 0.f && d2 <= 0.f)
		return a; // Barycentric coordinates are (1,0,0).

	// Check if P is in vertex region outside B.
	bx::Vec3 bp = bx::sub(p, b);
	float d3 = bx::dot(ab, bp);
	float d4 = bx::dot(ac, bp);
	if (d3 >= 0.f && d4 <= d3)
		return b; // Barycentric coordinates are (0,1,0).

	// Check if P is in edge region of AB, and if so, return the projection of P onto AB.
	float vc = d1*d4 - d3*d2;
	if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
	{
		float v = d1 / (d1 - d3);
		return bx::add(a, bx::mul(ab, v)); // The barycentric coordinates are (1-v, v, 0).
	}

	// Check if P is in vertex region outside C.
	bx::Vec3 cp = bx::sub(p, c);
	float d5 = bx::dot(ab, cp);
	float d6 = bx::dot(ac, cp);
	if (d6 >= 0.f && d5 <= d6)
		return c; // The barycentric coordinates are (0,0,1).

	// Check if P is in edge region of AC, and if so, return the projection of P onto AC.
	float vb = d5*d2 - d1*d6;
	if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
	{
		float w = d2 / (d2 - d6);
		return bx::add(a, bx::mul(ac, w)); // The barycentric coordinates are (1-w, 0, w).
	}

	// Check if P is in edge region of BC, and if so, return the projection of P onto BC.
	float va = d3*d6 - d5*d4;
	if (va <= 0.f && d4 - d3 >= 0.f && d5 - d6 >= 0.f)
	{
		float w = (d4 - d3) / (d4 - d3 + d5 - d6);
		return bx::add(b, bx::mul(bx::sub(c, b), w)); // The barycentric coordinates are (0, 1-w, w).
	}

	// P must be inside the face region. Compute the closest point through its barycentric coordinates (u,v,w).
	float denom = 1.f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return bx::add(bx::add(a, bx::mul(ab, v)), bx::mul(ac, w));
}

/** Calculates the intersection between a line and a triangle. The facing is not accounted for, so
	rays are reported to intersect triangles that are both front and backfacing.
	According to "T. M&ouml;ller, B. Trumbore. Fast, Minimum Storage Ray/Triangle Intersection. 2005."
	http://jgt.akpeters.com/papers/MollerTrumbore97/
	@param linePos The starting point of the line.
	@param lineDir The direction vector of the line. This does not need to be normalized.
	@param v0 Vertex 0 of the triangle.
	@param v1 Vertex 1 of the triangle.
	@param v2 Vertex 2 of the triangle.
	@param u [out] The barycentric u coordinate is returned here if an intersection occurred.
	@param v [out] The barycentric v coordinate is returned here if an intersection occurred.
	@return The distance along the ray to the point of intersection, or +inf if no intersection occurred.
		If no intersection, then u and v and t will contain undefined values. If lineDir was not normalized, then to get the
		real world-space distance, one must scale the returned value with lineDir.Length(). If the returned value is negative,
		then the intersection occurs 'behind' the line starting position, with respect to the direction vector lineDir. */
float intersectLineTri(const bx::Vec3 &linePos, const bx::Vec3 &lineDir,
		const bx::Vec3 &v0, const bx::Vec3 &v1, const bx::Vec3 &v2,
		float &u, float &v)
{
	const float epsilon = bx::kFloatMin;

	// Edge vectors
	bx::Vec3 vE1 = bx::sub(v1, v0);
	bx::Vec3 vE2 = bx::sub(v2, v0);

	// begin calculating determinant - also used to calculate U parameter
	bx::Vec3 vP = bx::cross(lineDir, vE2);

	// If det < 0, intersecting backfacing tri, > 0, intersecting frontfacing tri, 0, parallel to plane.
	const float det = bx::dot(vE1, vP);

	// If determinant is near zero, ray lies in plane of triangle.
	if (bx::abs(det) <= epsilon)
		return bx::kFloatMax;
	const float recipDet = 1.f / det;

	// Calculate distance from v0 to ray origin
	bx::Vec3 vT = bx::sub(linePos, v0);

	// Output barycentric u
	u = bx::dot(vT, vP) * recipDet;
	if (u < -epsilon || u > 1.f + epsilon)
		return bx::kFloatMax; // Barycentric U is outside the triangle - early out.

	// Prepare to test V parameter
	bx::Vec3 vQ = bx::cross(vT, vE1);

	// Output barycentric v
	v = bx::dot(lineDir, vQ) * recipDet;
	if (v < -epsilon || u + v > 1.f + epsilon) // Barycentric V or the combination of U and V are outside the triangle - no intersection.
		return bx::kFloatMax;

	// Barycentric u and v are in limits, the ray intersects the triangle.
	
	// Output signed distance from ray to triangle.
	return bx::dot(vE2, vQ) * recipDet;
//	return (det < 0.f) ? IntersectBackface : IntersectFrontface;
}

bx::Vec3 getPoint(const Ray& r, float d) {
	return bx::add(r.pos, bx::mul(r.dir, d));
}

bool intersects(Triangle& triangle, const Ray &r, float *d, bx::Vec3 *intersectionPoint) {
	bx::Vec3 a = triangle._verts[0]._vertex;
	bx::Vec3 b = triangle._verts[1]._vertex;
	bx::Vec3 c = triangle._verts[2]._vertex;
	float u, v;
	float t = intersectLineTri(r.pos, r.dir, a, b, c, u, v);
	bool success = (t >= 0 && t != bx::kFloatMax);
	if (!success)
		return false;
	if (d)
		*d = t;
	if (intersectionPoint)
		*intersectionPoint = getPoint(r, t);
	return success;
}
}
#pragma once
#include <stdio.h>
#include <vector>

#include "nanoflann.h"

using namespace std;

struct Point3f
{
	float X, Y, Z;
};

struct PointCloud
{
	std::vector<Point3f> pts;

	// Must return the number of data points
	inline size_t kdtree_get_point_count() const { return pts.size(); }
	// Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
	inline float kdtree_distance(const float *p1, const size_t idx_p2, size_t /*size*/) const
	{
		const float d0 = p1[0] - pts[idx_p2].X;
		const float d1 = p1[1] - pts[idx_p2].Y;
		const float d2 = p1[2] - pts[idx_p2].Z;

		return d0*d0 + d1*d1 + d2*d2;
	}
	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	// "if/else's" are actually solved at compile time.
	inline float kdtree_get_pt(const size_t idx, int dim) const
	{
		if (dim == 0) return pts[idx].X;
		else if (dim == 1) return pts[idx].Y;
		else return pts[idx].Z;	
	}
	// Optional bounding-box computation: return false to default to a standard bbox computation loop.
	// Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
	// Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
	template <class BBOX>
	bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }
};


float icp(Point3f *verts1, Point3f *verts2, int nVerts1, int nVerts2, float *R, float *t, int maxIter = 10);
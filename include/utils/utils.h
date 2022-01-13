#pragma once

#include <stdio.h>
#include <vector>
#include "defs_and_types.h"
#include "KdTree.h"

using namespace gs;

void load_ply(char *filename, std::vector<Point*>& points);

void applyAffineTransform(std::vector<Point*>& points, float* R, float* T);

void load_extrinsic_params(char *filename, KRT *Krt);
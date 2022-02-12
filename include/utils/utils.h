#pragma once

#include <stdio.h>
#include <vector>
#include "defs_and_types.h"


void load_ply(char *filename, float* points, int &cnt);

void load_extrinsic_params(char *filename, KRT *Krt);

void rewrite_params(char *filename, KRT *Krt);

void export_refined_params(char *filename, float **R, float **T);
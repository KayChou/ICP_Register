#include "ICP.h"
#include "utils.h"

using namespace gs;

#define CAM_NUM 3


int main()
{
    //create a static box point cloud used as a reference.
    std::vector<Point*> pcd0, pcd1, pcd2;
    KRT krt[CAM_NUM];

    char filename[100];
    sprintf(filename, "../../pcd_%d.ply", 0);
	load_ply(filename, pcd0);
	sprintf(filename, "../../pcd_%d.ply", 1);
	load_ply(filename, pcd1);
	sprintf(filename, "../../pcd_%d.ply", 2);
	load_ply(filename, pcd2);
	printf("Load point cloud: %d %d %d\n", pcd0.size(), pcd1.size(), pcd2.size());

	load_extrinsic_params((char *)"../params.txt", krt);

	int cnt = 0;

	while(true) {
		applyAffineTransform(pcd0, krt[0].R, krt[0].T);
		applyAffineTransform(pcd1, krt[1].R, krt[1].T);
		applyAffineTransform(pcd2, krt[2].R, krt[2].T);
		printf("%d\n", cnt++);
	}
}
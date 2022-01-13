#include "utils.h"


void load_ply(char *filename, std::vector<Point*>& points)
{
    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        printf("Failed to load ply: %s\n", filename);
        return;
    }
    else {
        printf("Load ply: %s\n", filename);
    }
    char str[100];
    float tmp;
    int cnt = 0;

    // read header
    fscanf(f, "%s", str);
    fscanf(f, "%s %s %s", str, str, str);
    fscanf(f, "%s %s %d", str, str, &cnt);
    fscanf(f, "%s %s %s", str, str, str);
    fscanf(f, "%s %s %s", str, str, str);
    fscanf(f, "%s %s %s", str, str, str);
    fscanf(f, "%s", str);

    // read data in binary
    float x, y, z;

    for(int i = 0; i < cnt; i++) {
        fscanf(f, "%f %f %f", &x, &y, &z);
        points.push_back(new Point(x, y, z));
        // printf("%f %f %f\n", x, y, z);
    }

    fclose(f);
}


/*
Apply an afine transofrm to a point cloud
*/
void applyAffineTransform(std::vector<Point*>& points, float* R, float* T)
{
    Point pRot;
    for (int i = 0; i < points.size(); i++)
    {
        pRot.pos[0] = R[0] * points[i]->pos[0] + R[1] * points[i]->pos[1] + R[2] * points[i]->pos[2] + T[0];
        pRot.pos[1] = R[3] * points[i]->pos[0] + R[4] * points[i]->pos[1] + R[5] * points[i]->pos[2] + T[1];
        pRot.pos[2] = R[6] * points[i]->pos[0] + R[7] * points[i]->pos[1] + R[8] * points[i]->pos[2] + T[2];

        *points[i] = pRot;
    }
}


void load_extrinsic_params(char *filename, KRT *Krt) {
    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        printf("Failed to read params: %s", filename);
        return;
    }
    else {
        printf("Load param: %s\n", filename);
    }

    char str[100];

    // read R and T
    for(int i = 0; i < 3; i++) {
        fscanf(f, "%s %s %s %s %s %s", str, str, str, str, str, str);
        fscanf(f, "%s %s %s %s %s", str, str, str, str, str);
        fscanf(f, "%f %f %f", &(Krt[i].R[0]), &(Krt[i].R[1]), &(Krt[i].R[2]));
        fscanf(f, "%f %f %f", &(Krt[i].R[3]), &(Krt[i].R[4]), &(Krt[i].R[5]));
        fscanf(f, "%f %f %f", &(Krt[i].R[6]), &(Krt[i].R[7]), &(Krt[i].R[8]));
        fscanf(f, "%f %f %f", &(Krt[i].T[0]), &(Krt[i].T[1]), &(Krt[i].T[2]));
        // fscanf(f, "%s", str);
    }
    
    // read color and IR params
    float tmp;
    for(int i = 0; i < 3; i++) {
        fscanf(f, "%s %s %s %s %s %s", str, str, str, str, str, str);
        fscanf(f, "%s %s", str, str);

        fscanf(f, "%s %s %s %s", str, str, str, str);
        fscanf(f, "%s %s %s %s %s", str, str, str, str, str);

        fscanf(f, "%f %f %f %f", &tmp, &tmp, &tmp, &tmp);
        fscanf(f, "%f %f %f %f %f", &tmp, 
                                    &tmp, 
                                    &tmp, 
                                    &tmp, 
                                    &tmp);

        // printf("%f %f %f %f\n", ir_param.fx, ir_param.fy, ir_param.cx, ir_param.cy);
        fscanf(f, "%s %s", str, str);
        fscanf(f, "%s %s %s %s %s", str, str, str, str, str);
        fscanf(f, "%f %f %f %f", &Krt[i].fx, &Krt[i].fy, &Krt[i].cx, &Krt[i].cy);

        fscanf(f, "%s %s %s", str, str, str);
        fscanf(f, "%s %s", str, str);

        fscanf(f, "%s %s %s %s %s %s %s %s %s %s %s", str, str, str, str, str, str, str, str, str, str, str);
        fscanf(f, "%s %s %s %s %s %s %s %s %s %s %s", str, str, str, str, str, str, str, str, str, str, str);
        
        fscanf(f, "%f %f", &tmp, &tmp);
        fscanf(f, "%f %f %f %f %f %f %f %f %f %f", 
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp);
        fscanf(f, "%f %f %f %f %f %f %f %f %f %f", 
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp,
                                    &tmp);
    }
    fclose(f);
}
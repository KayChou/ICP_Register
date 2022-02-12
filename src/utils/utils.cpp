#include "utils.h"


void load_ply(char *filename, float* points, int &cnt)
{
    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        printf("Failed to load ply: %s\n", filename);
        return;
    }
    char str[100];
    float tmp;

    // read header
    fscanf(f, "%s", str);
    fscanf(f, "%s %s %s", str, str, str);
    fscanf(f, "%s %s %d", str, str, &cnt);
    fscanf(f, "%s %s %s", str, str, str);
    fscanf(f, "%s %s %s", str, str, str);
    fscanf(f, "%s %s %s", str, str, str);
    fscanf(f, "%s", str);

    printf("Load ply: %s, num: %d\n", filename, cnt);

    // read data in binary
    float x, y, z;

    for(int i = 0; i < cnt; i++) {
        fscanf(f, "%f %f %f", &points[3 * i], &points[3 * i + 1], &points[3 * i + 2]);
        // points[3 * i] /= 1000;
        // points[3 * i + 1] /= 1000;
        // points[3 * i + 2] /= 1000;
        // printf("%f %f %f\n", x, y, z);
    }

    fclose(f);
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


void rewrite_params(char *filename, KRT *Krt) {
    FILE *f = fopen(filename, "w");
    if(f == NULL) {
        printf("Failed to read params: %s", filename);
        return;
    }
    else {
        printf("Write param: %s\n", filename);
    }
    char str[100];
    float tmp;

    for(int i = 0; i < 3; i++) {
        fprintf(f, "Extrinsic between camera %d and 1\n", i);
        fprintf(f, "Xc = RXw + T\n");
        fprintf(f, "\t%12.6f \t%12.6f \t%12.6f\n", Krt[i].R[0], Krt[i].R[1], Krt[i].R[2]);
        fprintf(f, "\t%12.6f \t%12.6f \t%12.6f\n", Krt[i].R[3], Krt[i].R[4], Krt[i].R[5]);
        fprintf(f, "\t%12.6f \t%12.6f \t%12.6f\n", Krt[i].R[6], Krt[i].R[7], Krt[i].R[8]);
        fprintf(f, "\t%12.6f \t%12.6f \t%12.6f\n", Krt[i].T[0], Krt[i].T[1], Krt[i].T[2]);

        fprintf(f, "\n");
    }

    fclose(f);
}


void export_refined_params(char *filename, float **R, float **T)
{
    FILE *f = fopen(filename, "w");
    if(f == NULL) {
        printf("Failed to read params: %s", filename);
        return;
    }
    else {
        printf("Write param: %s\n", filename);
    }
    char str[100];
    float tmp;

    for(int i = 0; i < 3; i++) {
        fprintf(f, "Extrinsic between camera %d and 1\n", i);
        fprintf(f, "Xc = RXw + T\n");
        fprintf(f, "\t%12.6f \t%12.6f \t%12.6f\n", R[i][0], R[i][1], R[i][2]);
        fprintf(f, "\t%12.6f \t%12.6f \t%12.6f\n", R[i][3], R[i][4], R[i][5]);
        fprintf(f, "\t%12.6f \t%12.6f \t%12.6f\n", R[i][6], R[i][7], R[i][8]);
        fprintf(f, "\t%12.6f \t%12.6f \t%12.6f\n", T[i][0], T[i][1], T[i][2]);

        fprintf(f, "\n");
    }

    fclose(f);
}
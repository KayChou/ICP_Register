#pragma once
#include <glad/glad.h>

class opengl_mesh
{
public:
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

private:
    int v_num;
    int f_num;

public:
    opengl_mesh();
    void set_data(float* verts, int* faces, int v_num, int f_num);
    void draw();
};
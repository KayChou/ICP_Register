#include "opengl_mesh.h"


opengl_mesh::opengl_mesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}


void opengl_mesh::set_data(float* verts, int* faces, int v_num, int f_num)
{
    this->v_num = v_num;
    this->f_num = f_num;
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * v_num, verts, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 4 * f_num, faces, GL_DYNAMIC_DRAW);

    // configure vertex attribute: position and color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}


void opengl_mesh::draw()
{
    glBindVertexArray(VAO);
    glPointSize(2.5);
    glDrawArrays(GL_POINTS, 0, this->v_num);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glDrawElements(GL_TRIANGLES, 3 * this->f_num, GL_UNSIGNED_INT, 0);

}
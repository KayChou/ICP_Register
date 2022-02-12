#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

#include "shader.h"
#include "defs_and_types.h"
#include "opengl_mesh.h"
#include "utils.h"

#define CAM_NUM 3


class Opengl_Render
{
private:
    unsigned int WIN_WIDTH;
    unsigned int WIN_HEIGHT;
    
public:
    opengl_mesh *fgr_model;

public:
    float* pcd[CAM_NUM];
    KRT krt[CAM_NUM];
    int vert_cnt[CAM_NUM];

    float **R_icp;
    float **T_icp;

private:
    GLFWwindow* window;
    glm::vec3 cameraPos;
    glm::vec3 cameraTar;
    glm::vec3 cameraUp;

    float fov;
    float deltaTime;
    float lastFrame;

    Shader *shaderModel;

private:
    VertsRGB* verts_fgr;
    int3* faces_fgr;
    int vert_num_fgr, face_num_fgr;

private:
    int frame_cnt;

public:
    void init();
    void loop();
    void destroy();

    void do_ICP(int cam_idx);
    void update_data();
    void update_KRT();

    void processInput();
    void glInit();
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};
#include "Opengl_Render.h"
#include "ICP.h"
#include <Windows.h>


void Opengl_Render::init()
{
    this->WIN_WIDTH = 1920;
    this->WIN_HEIGHT = 1080;
    this->frame_cnt = 0;

    this->cameraTar = glm::vec3(-0.700000, -0.200000, -0.600000);
    this->cameraPos = glm::vec3(-0.803034, -0.340658, -2.012135);
    this->cameraUp = glm::vec3(-0.046714, -0.993648, 0.102383);
    this->cameraTar *= 1000;
    this->cameraPos *= 1000;
    this->cameraUp *= 1000;
    this->fov =  45.0f;

    // timing
    this->deltaTime = 0.0f;	// time between current frame and last frame
    this->lastFrame = 0.0f;

    this->verts_fgr = new VertsRGB[3700000];
    this->faces_fgr = new int3[1];

    char filename[100];
    this->R_icp = new float*[CAM_NUM];
    this->T_icp = new float*[CAM_NUM];
	for(int i = 0; i < CAM_NUM; i++) {
        pcd[i] = new float[150000 * 3];
		sprintf(filename, "../../pcd_%d.ply", i);
		load_ply(filename, pcd[i], vert_cnt[i]);

        this->R_icp[i] = new float[9];
        this->T_icp[i] = new float[3];

        memset(this->R_icp[i], 0, 9 * sizeof(float));
        memset(this->T_icp[i], 0, 3 * sizeof(float));

        this->R_icp[i][0] = 1;
        this->R_icp[i][4] = 1;
        this->R_icp[i][8] = 1;
	}
    // printf("load ply: %d %d %d\n", vert_cnt[0], vert_cnt[1], vert_cnt[2]);
	load_extrinsic_params((char *)"../params.txt", krt);
    this->update_data();
}


void Opengl_Render::loop()
{
    glInit();

    while(!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        fgr_model->set_data((float*)verts_fgr, (int*)this->faces_fgr, this->vert_num_fgr, 0);

        shaderModel->use();
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraTar, cameraUp); // camera/view transformation
        shaderModel->setMat4("projection", projection);
        shaderModel->setMat4("view", view);
        shaderModel->setMat4("model", glm::mat4(1.0f));

        fgr_model->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        frame_cnt = (frame_cnt + 1) % 300;
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    update_KRT();
}


void Opengl_Render::do_ICP(int cam_idx) {
    printf("do ICP for cam: %d\n", cam_idx);
    float *R = this->R_icp[cam_idx];
    float *T = this->T_icp[cam_idx];

    float* tgt = new float[1000000];
    float* src = new float[1000000];

    int tgt_idx1 = (cam_idx + 1) % CAM_NUM;
    int tgt_idx2 = (cam_idx + 2) % CAM_NUM;

    // memcpy(src, pcd[cam_idx], 3 * sizeof(float) * vert_cnt[cam_idx]);
    // memcpy(tgt, pcd[tgt_idx1], 3 * sizeof(float) * vert_cnt[tgt_idx1]);
    // memcpy(tgt + vert_cnt[tgt_idx1] * 3, pcd[tgt_idx2], 3 * sizeof(float) * vert_cnt[tgt_idx2]);
    for(int i = 0; i < vert_cnt[cam_idx]; i++) {
        float *R = this->R_icp[cam_idx];
        float *T = this->T_icp[cam_idx];
        src[3 * i + 0] = R[0] * pcd[cam_idx][3 * i + 0] + R[3] * pcd[cam_idx][3 * i + 1] + R[6] * pcd[cam_idx][3 * i + 2] + T[0];
        src[3 * i + 1] = R[1] * pcd[cam_idx][3 * i + 0] + R[4] * pcd[cam_idx][3 * i + 1] + R[7] * pcd[cam_idx][3 * i + 2] + T[1];
        src[3 * i + 2] = R[2] * pcd[cam_idx][3 * i + 0] + R[5] * pcd[cam_idx][3 * i + 1] + R[8] * pcd[cam_idx][3 * i + 2] + T[2];
    }
    for(int i = 0; i < vert_cnt[tgt_idx1]; i++) {
        float *R = this->R_icp[tgt_idx1];
        float *T = this->T_icp[tgt_idx1];
        tgt[3 * i + 0] = R[0] * pcd[tgt_idx1][3 * i + 0] + R[3] * pcd[tgt_idx1][3 * i + 1] + R[6] * pcd[tgt_idx1][3 * i + 2] + T[0];
        tgt[3 * i + 1] = R[1] * pcd[tgt_idx1][3 * i + 0] + R[4] * pcd[tgt_idx1][3 * i + 1] + R[7] * pcd[tgt_idx1][3 * i + 2] + T[1];
        tgt[3 * i + 2] = R[2] * pcd[tgt_idx1][3 * i + 0] + R[5] * pcd[tgt_idx1][3 * i + 1] + R[8] * pcd[tgt_idx1][3 * i + 2] + T[2];
    }
    for(int i = 0; i < vert_cnt[tgt_idx2]; i++) {
        float *R = this->R_icp[tgt_idx2];
        float *T = this->T_icp[tgt_idx2];
        tgt[3 * (i + vert_cnt[tgt_idx1]) + 0] = R[0] * pcd[tgt_idx2][3 * i + 0] + R[3] * pcd[tgt_idx2][3 * i + 1] + R[6] * pcd[tgt_idx2][3 * i + 2] + T[0];
        tgt[3 * (i + vert_cnt[tgt_idx1]) + 1] = R[1] * pcd[tgt_idx2][3 * i + 0] + R[4] * pcd[tgt_idx2][3 * i + 1] + R[7] * pcd[tgt_idx2][3 * i + 2] + T[1];
        tgt[3 * (i + vert_cnt[tgt_idx1]) + 2] = R[2] * pcd[tgt_idx2][3 * i + 0] + R[5] * pcd[tgt_idx2][3 * i + 1] + R[8] * pcd[tgt_idx2][3 * i + 2] + T[2];
    }

    icp((Point3f*)tgt, (Point3f*)src, vert_cnt[tgt_idx1] + vert_cnt[tgt_idx2], vert_cnt[cam_idx], R, T, 5);

    delete [] tgt;
    delete [] src;

    update_data();
}


void Opengl_Render::update_data() {
    this->vert_num_fgr = 0;
    float x, y, z;
    for(int i = 0; i < CAM_NUM; i++) {
        float *R = this->R_icp[i];
        float *T = this->T_icp[i];
        for(int k = 0; k < vert_cnt[i]; k++) {
            this->verts_fgr[vert_num_fgr].x = R[0] * pcd[i][3 * k + 0] + R[3] * pcd[i][3 * k + 1] + R[6] * pcd[i][3 * k + 2] + T[0];
            this->verts_fgr[vert_num_fgr].y = R[1] * pcd[i][3 * k + 0] + R[4] * pcd[i][3 * k + 1] + R[7] * pcd[i][3 * k + 2] + T[1];
            this->verts_fgr[vert_num_fgr].z = R[2] * pcd[i][3 * k + 0] + R[5] * pcd[i][3 * k + 1] + R[8] * pcd[i][3 * k + 2] + T[2];
            this->verts_fgr[vert_num_fgr].r = (i == 0) ? 0.9 : 0;
            this->verts_fgr[vert_num_fgr].g = (i == 1) ? 0.9 : 0;
            this->verts_fgr[vert_num_fgr].b = (i == 2) ? 0.9 : 0;
            vert_num_fgr++;
        }
    }
}


void Opengl_Render::update_KRT() {
    // update krt
    for(int cam_idx = 0; cam_idx < CAM_NUM; cam_idx++) {
        float *R = this->R_icp[cam_idx];
        float *T = this->T_icp[cam_idx];
        float temp_R[9];
        memcpy(temp_R, krt[cam_idx].R, sizeof(float) * 9);
        krt[cam_idx].R[0] = temp_R[0] * R[0] + temp_R[1] * R[3] + temp_R[2] * R[6];
        krt[cam_idx].R[1] = temp_R[0] * R[1] + temp_R[1] * R[4] + temp_R[2] * R[7];
        krt[cam_idx].R[2] = temp_R[0] * R[2] + temp_R[1] * R[5] + temp_R[2] * R[8];
        krt[cam_idx].R[3] = temp_R[3] * R[0] + temp_R[4] * R[3] + temp_R[5] * R[6];
        krt[cam_idx].R[4] = temp_R[3] * R[1] + temp_R[4] * R[4] + temp_R[5] * R[7];
        krt[cam_idx].R[5] = temp_R[3] * R[2] + temp_R[4] * R[5] + temp_R[5] * R[8];
        krt[cam_idx].R[6] = temp_R[6] * R[0] + temp_R[7] * R[3] + temp_R[8] * R[6];
        krt[cam_idx].R[7] = temp_R[6] * R[1] + temp_R[7] * R[4] + temp_R[8] * R[7];
        krt[cam_idx].R[8] = temp_R[6] * R[2] + temp_R[7] * R[5] + temp_R[8] * R[8];
        printf("R of cam %d: %f %f %f | %f %f %f | %f %f %f\n", cam_idx, 
                R[0], R[1], R[2], R[3], R[4], R[5], R[6], R[7], R[8]);
        printf("T of cam %d: %f %f %f\n", cam_idx, T[0], T[1], T[2]);
        krt[cam_idx].T[0] = krt[cam_idx].T[0] - krt[cam_idx].R[0] * T[0] + krt[cam_idx].R[1] * T[1] + krt[cam_idx].R[2] * T[2];
        krt[cam_idx].T[1] = krt[cam_idx].T[1] - krt[cam_idx].R[3] * T[0] + krt[cam_idx].R[4] * T[1] + krt[cam_idx].R[5] * T[2];
        krt[cam_idx].T[2] = krt[cam_idx].T[2] - krt[cam_idx].R[6] * T[0] + krt[cam_idx].R[7] * T[1] + krt[cam_idx].R[8] * T[2];
    }
    rewrite_params((char*)"../params_refined.txt", krt);
    export_refined_params((char*)"../params_icp.txt", this->R_icp, this->T_icp);
}


void Opengl_Render::destroy()
{
    delete [] this->verts_fgr;
    delete [] this->faces_fgr;
    delete fgr_model;
    delete shaderModel;
}


void Opengl_Render::glInit()
{
    //init glfw and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //glfw: create window
    this->window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "LiveRecon3D", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl; fflush(stdout);
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all opengl function pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl; fflush(stdout);
    }

    shaderModel = new Shader("../include/Opengl_Render/vertexShader.vert", "../include/Opengl_Render/fragShader.frag");
    fgr_model = new opengl_mesh();
}


// ================================================================================
// key board input
// ================================================================================
void Opengl_Render::processInput()
{
    float translationSpeed = 50;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime * 500;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if(glm::length(cameraPos - cameraTar) > 0.2 * cameraSpeed){
            cameraPos += cameraSpeed * glm::normalize(cameraTar - cameraPos);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * glm::normalize(cameraTar - cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS | glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 temp = cameraPos + cameraSpeed * glm::normalize(glm::cross(cameraUp, cameraTar-cameraPos));
        cameraPos = cameraTar + glm::length(cameraPos - cameraTar) * glm::normalize(temp - cameraTar);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS | glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 temp = cameraPos - cameraSpeed * glm::normalize(glm::cross(cameraUp, cameraTar-cameraPos));
        cameraPos = cameraTar + glm::length(cameraPos - cameraTar) * glm::normalize(temp - cameraTar);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        glm::vec3 right = glm::cross(cameraUp, cameraTar - cameraPos);
        glm::vec3 temp = (cameraPos + cameraSpeed * cameraUp);
        cameraPos = cameraTar + glm::length(cameraTar - cameraPos) * glm::normalize(temp - cameraTar);
        cameraUp = glm::normalize(glm::cross(cameraTar - cameraPos, right));
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        glm::vec3 right = glm::cross(cameraUp, cameraTar - cameraPos);
        glm::vec3 temp = (cameraPos - cameraSpeed * cameraUp);
        cameraPos = cameraTar + glm::length(cameraTar - cameraPos) * glm::normalize(temp - cameraTar);
        cameraUp = glm::normalize(glm::cross(cameraTar - cameraPos, right));
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){ // translate x +
        cameraPos += glm::vec3(translationSpeed, 0.0f, 0.0f);
        cameraTar += glm::vec3(translationSpeed, 0.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){ // translate y +
        cameraPos += glm::vec3(0.0f, translationSpeed, 0.0f);
        cameraTar += glm::vec3(0.0f, translationSpeed, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){ // translate z +
        cameraPos += glm::vec3(0.0f, 0.0f, translationSpeed);
        cameraTar += glm::vec3(0.0f, 0.0f, translationSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){ // translate x -
        cameraPos -= glm::vec3(translationSpeed, 0.0f, 0.0f);
        cameraTar -= glm::vec3(translationSpeed, 0.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){ // translate y -
        cameraPos -= glm::vec3(0.0f, translationSpeed, 0.0f);
        cameraTar -= glm::vec3(0.0f, translationSpeed, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){ // translate z -
        cameraPos -= glm::vec3(0.0f, 0.0f, translationSpeed);
        cameraTar -= glm::vec3(0.0f, 0.0f, translationSpeed);
    }
    // std::cout << "Target: " << glm::to_string(cameraTar) << std::endl;
    // std::cout << "Position: " << glm::to_string(cameraPos) << std::endl;
    // std::cout << "Up: " << glm::to_string(cameraUp) << std::endl;


    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        this->do_ICP(0);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        this->do_ICP(1);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        this->do_ICP(2);
    }
}


// ================================================================================
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ================================================================================
void Opengl_Render::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

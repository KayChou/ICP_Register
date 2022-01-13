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
    this->fov =  45.0f;

    // timing
    this->deltaTime = 0.0f;	// time between current frame and last frame
    this->lastFrame = 0.0f;

    this->verts_fgr = new VertsRGB[3700000];
    this->faces_fgr = new int3[1];

    char filename[100];
	for(int i = 0; i < CAM_NUM; i++) {
        pcd[i] = new float[150000 * 3];
		sprintf(filename, "../../pcd_%d.ply", i);
		load_ply(filename, pcd[i], vert_cnt[i]);
	}
    // printf("load ply: %d %d %d\n", vert_cnt[0], vert_cnt[1], vert_cnt[2]);
	// load_extrinsic_params((char *)"../params.txt", krt);
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
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
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
}


void Opengl_Render::update_data() {
    this->vert_num_fgr = 0;
    float x, y, z;
    for(int i = 0; i < CAM_NUM; i++) {
        for(int k = 0; k < vert_cnt[i]; k++) {
            x = pcd[i][3 * k + 0];
            y = pcd[i][3 * k + 1];
            z = pcd[i][3 * k + 2];
            this->verts_fgr[vert_num_fgr].x = x;//krt[i].R[0] * x + krt[i].R[1] * y + krt[i].R[2]*z + krt[i].T[0];
            this->verts_fgr[vert_num_fgr].y = y;//krt[i].R[3] * x + krt[i].R[4] * y + krt[i].R[5]*z + krt[i].T[1];
            this->verts_fgr[vert_num_fgr].z = z;//krt[i].R[6] * x + krt[i].R[7] * y + krt[i].R[8]*z + krt[i].T[2];
            this->verts_fgr[vert_num_fgr].r = (i == 0) ? 0.9 : 0;
            this->verts_fgr[vert_num_fgr].g = (i == 1) ? 0.9 : 0;
            this->verts_fgr[vert_num_fgr].b = (i == 2) ? 0.9 : 0;
            vert_num_fgr++;
        }
    }
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
    float translationSpeed = 0.1;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if(glm::length(cameraPos - cameraTar) > cameraSpeed){
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
        printf("DOWN\n");
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


    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        std::vector<Point*> tgt;
        std::vector<Point*> src;
        printf("begin icp\n");
            
        for(int cam_idx = 0; cam_idx < CAM_NUM; cam_idx++) {
            tgt.clear();
            src.clear();

            for(int i = 0; i < vert_cnt[cam_idx]; i++) {
                src.push_back(new Point(pcd[cam_idx][3 * i + 0], pcd[cam_idx][3 * i + 1], pcd[cam_idx][3 * i + 2]));
            }

            int tgt_idx = (cam_idx + 1) % CAM_NUM;
            for(int i = 0; i < vert_cnt[tgt_idx]; i++) {
                tgt.push_back(new Point(pcd[tgt_idx][3 * i + 0], pcd[tgt_idx][3 * i + 1], pcd[tgt_idx][3 * i + 2]));
            }
            tgt_idx = (cam_idx + 2) % CAM_NUM;
            for(int i = 0; i < vert_cnt[tgt_idx]; i++) {
                tgt.push_back(new Point(pcd[tgt_idx][3 * i + 0], pcd[tgt_idx][3 * i + 1], pcd[tgt_idx][3 * i + 2]));
            }

            icp(src, tgt);

            for(int i = 0; i < vert_cnt[cam_idx]; i++) {
                pcd[cam_idx][3 * i + 0] = src[i]->pos[0];
                pcd[cam_idx][3 * i + 1] = src[i]->pos[1];
                pcd[cam_idx][3 * i + 2] = src[i]->pos[2];
            }
        }

        printf("finish icp\n");
        update_data();
    }
}


// ================================================================================
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ================================================================================
void Opengl_Render::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

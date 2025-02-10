#include<iostream>
#include <glad/glad.h>
#include<glfw3.h>
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include "ShaderClass.h"
#include "Camera.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include "BoidSetUp.h"
#include "Boids.h"
#include "Box.h"
#include <stb_image.h>
#include "kdop.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "obb.h"
#include <chrono>
#include <thread>
#include "Column.h"
#include <cstddef>
#include "Terrain.h"
#include "Skybox.h"

const float panelWidth = 300.0f;

const unsigned int windowWidth = 800 + panelWidth;
const unsigned int windowHeight = 800;

static int numGroups = 3;
static int numBoidsPerGroup = 70;

static float maxSpeed = 0.02f;
static float maxForce = 0.01f;
static float alignWeight = 0.4f;
static float cohesionWeight = 0.03f;
static float separationWeight = 0.4f;
static float horizontalBiasStrength = 0.1f;

static bool mouseRightClicked = false;
static glm::vec2 mouseClickPosition;
static bool isAttractionMode = false;

static int useNormalMapping = 1;
static int useShadowMapping = 1;

Core::RenderContext fishContext;
GLuint fishNormalMap, fishTexture;
GLuint columnNormalMap, columnTexture;
GLuint sandTexture;

glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f);
glm::vec3 lightColor = glm::vec3(300.0f, 300.0f, 300.0f);
glm::vec4 lightColorBetter = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec3 objectColor = glm::vec3(0.8f, 0.3f, 0.3f);

GLuint columnVBO, columnVAO, columnEBO;
GLuint skyboxVBO, skyboxVAO, skyboxEBO;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void loadModelToContext(std::string path, Core::RenderContext& context)
{
    Assimp::Importer import;
    const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    if (scene->mNumMeshes == 0) {
        std::cout << "No meshes found in model" << std::endl;
        return;
    }

    context.initFromAssimpMesh(scene->mMeshes[0]);
    std::cout << "Model loaded successfully with " << scene->mMeshes[0]->mNumVertices << " vertices" << std::endl;
}

GLuint loadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // load the texture data using stb_image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1) {
            format = GL_RED;
        }
        else if (nrChannels == 3) {
            format = GL_RGB;
        }
        else if (nrChannels == 4) {
            format = GL_RGBA;
        }
        else {
            std::cout << "Error: Unsupported number of channels in texture!" << std::endl;
            stbi_image_free(data);
            return 0;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Texture loaded successfully: " << path << " (" << width << "x" << height << ", " << nrChannels << " channels)" << std::endl;
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // forward mouse button events to imgui
    if (ImGui::GetIO().WantCaptureMouse) {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        return;
    }

    // handle attraction/disperse
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    mouseClickPosition = glm::vec2(x, windowHeight - y);

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        mouseRightClicked = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
}

void applyMouseForce(std::vector<Boid>& boids, const glm::vec3& target, float forceStrength, bool isAttraction) {
    for (auto& boid : boids) {
        glm::vec3 direction = target - boid.position;
        float distance = glm::length(direction);

        if (distance > 0.0f) {
            direction = glm::normalize(direction);
            glm::vec3 force = direction * forceStrength;

            if (!isAttraction) {
                force = -force;
            }

            boid.applyForce(force);
        }
    }
}

glm::vec3 getWorldPositionFromMouse(float mouseX, float mouseY, Camera& camera, int screenWidth, int screenHeight) {
    // convert screen coordinates to normalized device coordinates
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    float z = 1.0f;

    // create a ray in clip space
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, -1.0f);

    // transform the ray to eye space
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // transform the ray to world space
    glm::mat4 view = camera.GetViewMatrix();
    glm::vec4 rayWorld = glm::inverse(view) * rayEye;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

    // calculate the intersection point with the z = 0 plane 
    float t = -camera.Position.z / rayDirection.z;
    glm::vec3 worldPosition = camera.Position + t * rayDirection;

    return worldPosition;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window initialization
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Aquarium", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, windowWidth, windowHeight);

    // imgui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // connect imgui with GLFW and OpenGL3
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    glEnable(GL_DEPTH_TEST);

    // loading textures
    fishNormalMap = loadTexture("../textures/fish_normal_map.png");
    fishTexture = loadTexture("../textures/fish_texture.png");
    if (fishNormalMap == 0 || fishTexture == 0) {
        std::cout << "Error: Failed to load one or more textures!" << std::endl;
        return -1;
    }

    columnNormalMap = loadTexture("../textures/column_normal_map.png");
    columnTexture = loadTexture("../textures/column_texture.png");
    if (columnNormalMap == 0 || columnTexture == 0) {
        std::cout << "Error: Failed to load one or more textures!" << std::endl;
        return -1;
    }

    sandTexture = loadTexture("../textures/sand_texture.png");
    if (sandTexture == 0) {
        std::cout << "Error: Failed to load terrain texture!" << std::endl;
        return -1;
    }

    // shader for our basic wire cube
    Shader shaderProgram("../shaders/cube.vert", "../shaders/cube.frag");

    // column VAO
    VAO columnVAO;
    columnVAO.Bind();
    VBO columnVBO(columnVertices, COLUMN_VERTEX_COUNT * sizeof(GLfloat));
    EBO columnEBO(columnIndices, COLUMN_INDEX_COUNT * sizeof(GLuint));
    // position
    columnVAO.LinkAttrib(columnVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    // normals
    columnVAO.LinkAttrib(columnVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    // texture coords
    columnVAO.LinkAttrib(columnVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    // tangents
    columnVAO.LinkAttrib(columnVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    columnVAO.Unbind();
    columnVBO.Unbind();
    columnEBO.Unbind();

    Shader terrainShader("../shaders/default.vert", "../shaders/default.frag");
    Terrain terrain(50.0f ,50.0f, 2.0f);

    VAO terrainVAO;
    terrainVAO.Bind();
    VBO terrainVBO(terrain.vertices.data(), terrain.vertices.size() * sizeof(float));
    EBO terrainEBO(terrain.indices.data(), terrain.indices.size() * sizeof(unsigned int));

    // link attributes
    terrainVAO.LinkAttrib(terrainVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0); // position
    terrainVAO.LinkAttrib(terrainVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // normal
    terrainVAO.LinkAttrib(terrainVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // texCoord

    terrainVAO.Unbind();
    terrainVBO.Unbind();
    terrainEBO.Unbind();

    glm::mat4 terrainModel = glm::mat4(1.0f);
    terrainModel = glm::translate(terrainModel, glm::vec3(-20.0f, -10.0f, -60.0f));

    shaderProgram.Activate();

    // shader for fish
    Shader fishShader("../shaders/fish_shader.vert", "../shaders/fish_shader.frag");
    fishShader.Activate();

    // shader for columns
    Shader columnShader("../shaders/col.vert", "../shaders/col.frag");
    columnShader.Activate();

    Shader shadowMapProgram("../shaders/depth.vert", "../shaders/depth.frag");

    Shader debugShader("../shaders/debug.vert", "../shaders/debug.frag");

    Shader skyboxShader("../shaders/skybox.vert", "../shaders/skybox.frag");
    skyboxShader.Activate();
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

    // matrixes for the camera
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -30.0f)); // sets how far away we are from the cube
    float aspectRatio = (float)windowWidth / (float)windowHeight;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    Camera camera(windowWidth, windowHeight, glm::vec3(-5.0f, 0.0f, 10.0f));

    // !!!!!!!!!!!!!!!!!!!!!!!!!
    std::vector<Boid> boids;
    loadModelToContext("../resources/models/fish.obj", fishContext);
    setUpBoids(boids, numGroups, numBoidsPerGroup); // set up number of boid groups you want here : boids, number of groups, number of boids in each group
    for (auto& boid : boids) {
        boid.context = &fishContext;
    }

    // calculate delta time to regulate speed with the frame rate
    auto lastTime = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;
    int frameCount = 0;
    double fpsTimer = 0.0;

    // change here if your pc can make it
    const int targetFPS = 60;
    const float targetFrameTime = 1.0f / targetFPS;

    // framebuffer for shadow map
    unsigned int shadowMapFBO;
    glGenFramebuffers(1, &shadowMapFBO);

    // texture for shadow map FBO
    unsigned int shadowMapWidth = 2048, shadowMapHeight = 2048;
    unsigned int shadowMap;
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // prevents darkness outside the frustrum
    float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    // needed since we don't touch the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // matrices needed for the light's perspective
    glm::mat4 orthgonalProjection = glm::ortho(-135.0f, 135.0f, -135.0f, 135.0f, 0.1f, 175.0f);
    glm::mat4 lightView = glm::lookAt(2.0f * lightPos, glm::vec3(0.0f, 0.0f, -30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = orthgonalProjection * lightView;

    shadowMapProgram.Activate();
    shadowMapProgram.SetMat4("lightProjection", lightProjection);

    VAO skyboxVAO;
    skyboxVAO.Bind();
    VBO skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
    EBO skyboxEBO(skyboxIndices, sizeof(skyboxIndices));
    skyboxVAO.LinkAttrib(skyboxVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    skyboxVAO.Unbind();
    skyboxVBO.Unbind();
    skyboxEBO.Unbind();

    // all the faces of the cubemap
    std::string facesCubemap[6] =
    {
        "../textures/skybox_base.png", // right
        "../textures/skybox_base.png",    // left
        "../textures/skybox_top.png", // up
        "../textures/sand_texture.png", // down
        "../textures/skybox_back.png", // front
        "../textures/skybox_back.png" // back
    };

    // creates the cubemap texture object
    unsigned int cubemapTexture;
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // these are very important to prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // cycles through all the textures and attaches them to the cubemap object
    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D
            (
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
            stbi_image_free(data);
        }
    }

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        if (deltaTime < targetFrameTime) {
            float sleepTime = targetFrameTime - deltaTime;
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
            deltaTime = targetFrameTime; // prevent too small delta times
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // depth testing needed for shadow map
        glEnable(GL_DEPTH_TEST);

        // preparations for the shadow map
        glViewport(0, 0, shadowMapWidth, shadowMapHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // draw scene for shadow map
        shadowMapProgram.Activate();
        shadowMapProgram.SetMat4("lightSpaceMatrix", lightProjection);
        shadowMapProgram.SetMat4("view", view);
        
        // render columns
        columnVAO.Bind();

        float boxBottom = -10.0f;
        std::vector<Column> columns = {
            {glm::vec3(5.0f, boxBottom + (15.0f / 2.0f), 5.0f), glm::vec3(1.0f, 15.0f, 1.0f)},
            {glm::vec3(-5.0f, boxBottom + (11.0f / 2.0f), -5.0f), glm::vec3(2.0f, 12.0f, 2.0f)},
            {glm::vec3(-12.0f, boxBottom + (10.0f / 2.0f), 6.0f), glm::vec3(2.0f, 10.0f, 2.0f)},
            {glm::vec3(12.0f, boxBottom + (12.0f / 2.0f), 1.0f), glm::vec3(2.0f, 12.0f, 2.0f)}
        };

        for (auto& column : columns) {
            initializeColumnOBB(column);

            glm::mat4 columnModel = glm::mat4(1.0f);
            columnModel = glm::translate(columnModel, column.position);
            columnModel = glm::scale(columnModel, column.size);

            shadowMapProgram.SetMat4("model", columnModel);
            shadowMapProgram.SetVec3("scale", column.size);

            glDrawElements(GL_TRIANGLES, COLUMN_INDEX_COUNT, GL_UNSIGNED_INT, 0);
        }
        columnVAO.Unbind();

        shadowMapProgram.SetMat4("model", terrainModel);
        terrainVAO.Bind();
        glDrawElements(GL_TRIANGLES, terrain.indices.size(), GL_UNSIGNED_INT, 0);
        terrainVAO.Unbind();

        // switch back to the default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // switch back to the default viewport
        glViewport(0, 0, windowWidth, windowHeight);

        processInput(window);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(panelWidth, windowHeight));

        // allows for camera movement
        if (!ImGui::GetIO().WantCaptureMouse) {
            // only process camera inputs if ImGui is not using the mouse
            processInput(window);
            camera.Inputs(window);
        }

        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // handle mouse clicks
        if (mouseRightClicked && !ImGui::GetIO().WantCaptureMouse) {
            glm::vec3 worldPosition = getWorldPositionFromMouse(mouseClickPosition.x, mouseClickPosition.y, camera, windowWidth, windowHeight);

            // apply force continuously while the right mouse button is held
            float forceStrength = 0.01f;
            applyMouseForce(boids, worldPosition, forceStrength, isAttractionMode);
        }

        shaderProgram.Activate();
        shaderProgram.SetVec3("cameraPos", camera.Position);

        fishShader.Activate();

        // binding textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fishNormalMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fishTexture);

        camera.Matrix(fishShader, "camMatrix");

        // set uniforms for fish shader
        fishShader.SetInt("fishNormalMap", 0);
        fishShader.SetInt("fishTexture", 1);
        fishShader.SetInt("useNormalMapping", useNormalMapping);

        fishShader.SetMat4("view", view);
        fishShader.SetMat4("modelMatrix", model);
        fishShader.SetVec3("lightPos", lightPos);
        fishShader.SetVec3("lightColor", lightColor);
        fishShader.SetVec3("cameraPos", camera.Position);
        fishShader.SetVec3("objectColor", objectColor);

        renderBoids(boids, fishShader);

        // program shader
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);

        columnShader.Activate();
        columnVAO.Bind();

        // binding textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, columnNormalMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, columnTexture);

        camera.Matrix(fishShader, "camMatrix");

        // set uniforms for column shader
        columnShader.SetInt("columnNormalMap", 0);
        columnShader.SetInt("columnTexture", 1);
        columnShader.SetInt("useNormalMapping", useNormalMapping);

        columnShader.SetMat4("view", view);
        columnShader.SetVec3("lightPos", lightPos);
        columnShader.SetVec3("lightColor", lightColor);
        columnShader.SetVec3("cameraPos", camera.Position);
        columnShader.SetVec3("objectColor", objectColor);

        for (auto& column : columns) {
            initializeColumnOBB(column);

            glm::mat4 columnModel = glm::mat4(1.0f);
            columnModel = glm::translate(columnModel, column.position);
            columnModel = glm::scale(columnModel, column.size);

            columnShader.SetMat4("modelMatrix", columnModel);
            columnShader.SetVec3("scale", column.size);

            glDrawElements(GL_TRIANGLES, COLUMN_INDEX_COUNT, GL_UNSIGNED_INT, 0);
        }
        columnVAO.Unbind();

        terrainShader.Activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sandTexture);
        glUniform1i(glGetUniformLocation(terrainShader.ID, "tex0"), 0);
        if (sandTexture == 0) {
            std::cout << "Texture failed to load!" << std::endl;
        }
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        terrainShader.SetInt("shadowMap", 5);
        terrainShader.SetMat4("lightSpaceMatrix", lightProjection);
        terrainShader.SetInt("useShadowMapping", useShadowMapping);
        glUniform4f(glGetUniformLocation(terrainShader.ID, "lightColor"), lightColorBetter.x, lightColorBetter.y, lightColorBetter.z, lightColorBetter.w);
        terrainShader.SetMat4("model", terrainModel);
        terrainShader.SetVec3("lightPos", lightPos);
        terrainShader.SetVec3("camPos", camera.Position);
        camera.Matrix(terrainShader, "camMatrix");

        terrainVAO.Bind();
        glDrawElements(GL_TRIANGLES, terrain.indices.size(), GL_UNSIGNED_INT, 0);
        terrainVAO.Unbind();

        // since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
        glDepthFunc(GL_LEQUAL);

        skyboxShader.Activate();
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        // we make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
        // the last row and column affect the translation of the skybox (which we don't want to affect)
        view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
        projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);
        skyboxShader.SetMat4("view", view);
        skyboxShader.SetMat4("projection", projection);

        // draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
        // where an object is present (a depth of 1.0f will always fail against any object's depth value)
        skyboxVAO.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // switch back to the normal depth function
        glDepthFunc(GL_LESS);

        ///////////////// boid rendering /////////////////

        for (auto& boid : boids) {
            boid.hasCollided = false; // reset collision state at the start of the frame
            boid.hasCollidedWithColumn = false;
        }

        // boid rendering and updating
        for (auto& boid : boids) {
            for (auto& otherBoid : boids) {
                if (&boid != &otherBoid && boid.groupID == otherBoid.groupID && !boid.hasCollided && !otherBoid.hasCollided) {
                    if (checkOBBCollision(boid.obb, otherBoid.obb)) {
                        boid.handleCollision(boid, otherBoid);
                    }
                }
            }
            for (const auto& column : columns) {
                if (checkOBBCollision(boid.obb, column.obb)) {
                    boid.handleCollisionWithColumn(boid, column);  // handle column collision logic
                }

            }
        }

        // panel for interactive elements
        ImGui::Begin("User controls panel", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Text("Boid groups parameters");
        ImGui::Spacing();

        // slider for number of boid groups
        ImGui::Text("Number of Groups");
        ImGui::SliderInt("##slider1", &numGroups, 1, 10);

        // slider for number of boids per group
        ImGui::Text("Boids per Group");
        ImGui::SliderInt("##slider2", &numBoidsPerGroup, 1, 100);

        ImGui::Spacing();

        // button to reinitialize boids
        if (ImGui::Button("Apply Changes")) {
            boids.clear();
            setUpBoids(boids, numGroups, numBoidsPerGroup);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Boid parameters");
        ImGui::Spacing();

        // slider for boid speed
        ImGui::Text("Boids' speed");
        if (ImGui::SliderFloat("##slider3", &maxSpeed, 0.01f, 0.1f)) {
            // update maxSpeed for all boids in real-time
            for (auto& boid : boids) {
                boid.maxSpeed = maxSpeed;
            }
        }

        // slider for boid force
        ImGui::Text("Boids' force");
        if (ImGui::SliderFloat("##slider7", &maxForce, 0.01f, 0.1f)) {
            for (auto& boid : boids) {
                boid.maxForce = maxForce;
            }
        }

        // slider for boid align weight
        ImGui::Text("Boids' align weight");
        if (ImGui::SliderFloat("##slider4", &alignWeight, 0.0f, 1.0f)) {
            for (auto& boid : boids) {
                boid.update(boids, deltaTime, columns, alignWeight, cohesionWeight, separationWeight, horizontalBiasStrength);
            }
        }

        // slider for boid cohesion weight
        ImGui::Text("Boids' cohesion weight");
        if (ImGui::SliderFloat("##slider5", &cohesionWeight, 0.0f, 1.0f)) {
            for (auto& boid : boids) {
                boid.update(boids, deltaTime, columns, alignWeight, cohesionWeight, separationWeight, horizontalBiasStrength);
            }
        }

        // slider for boid separation weight
        ImGui::Text("Boids' separation weight");
        if (ImGui::SliderFloat("##slider6", &separationWeight, 0.0f, 1.0f)) {
            for (auto& boid : boids) {
                boid.update(boids, deltaTime, columns, alignWeight, cohesionWeight, separationWeight, horizontalBiasStrength);
            }
        }

        // slider for boid horizontal bias weight
        ImGui::Text("Boids' horizontal bias weight");
        if (ImGui::SliderFloat("##slider8", &horizontalBiasStrength, 0.0f, 0.1f)) {
            for (auto& boid : boids) {
                boid.update(boids, deltaTime, columns, alignWeight, cohesionWeight, separationWeight, horizontalBiasStrength);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // toggle for attraction/dispersion mode
        ImGui::Text("Interaction Mode");
        if (ImGui::Checkbox("Attraction Mode", &isAttractionMode)) {
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // toggle normal mapping enabling/disabling mode
        ImGui::Text("Mappings' parameters");
        ImGui::Spacing();
        if (ImGui::Checkbox("Enable Normal Mapping", (bool*)&useNormalMapping)) {
            fishShader.Activate();
            fishShader.SetInt("useNormalMapping", useNormalMapping);
        }

        // toggle shadow mapping enabling/disabling mode
        if (ImGui::Checkbox("Enable Shadow Mapping", (bool*)&useShadowMapping)) {
            terrainShader.Activate();
            terrainShader.SetInt("useShadowMapping", useShadowMapping);
        }

        ImGui::End();

        // if no slider change - this is the update that takes place!
        for (auto& boid : boids) {
            boid.update(boids, deltaTime, columns, alignWeight, cohesionWeight, separationWeight, horizontalBiasStrength);
        }

        //////////////////////////////////////////////////

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

        // used for fps calculations
        lastTime = currentTime;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
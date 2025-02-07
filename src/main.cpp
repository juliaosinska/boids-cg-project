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


const unsigned int width = 800;
const unsigned int height = 800;

Core::RenderContext fishContext;
GLuint fishNormalMap, fishTexture;

glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f);
glm::vec3 lightColor = glm::vec3(300.0f, 300.0f, 300.0f);
glm::vec3 objectColor = glm::vec3(0.8f, 0.3f, 0.3f);


GLfloat columnVertices[216];  // Size is 36 segments * 6 values per segment (2 triangles per segment)
GLuint columnVBO, columnVAO;

void generateColumn() {
    int numSegments = 36; // Number of segments in the cylinder base
    float radius = 1.0f;
    float height = 2.0f;

    // Populate the static array with vertex data
    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / numSegments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Bottom and Top vertices (each segment has 2 triangles, 6 values)
        columnVertices[i * 6 + 0] = x;   // Vertex x (bottom)
        columnVertices[i * 6 + 1] = 0.0f; // Vertex y (bottom)
        columnVertices[i * 6 + 2] = z;   // Vertex z (bottom)

        columnVertices[i * 6 + 3] = x;   // Vertex x (top)
        columnVertices[i * 6 + 4] = height; // Vertex y (top)
        columnVertices[i * 6 + 5] = z;   // Vertex z (top)
    }

    // Generate and bind VBO (Vertex Buffer Object)
    glGenBuffers(1, &columnVBO);
    glBindBuffer(GL_ARRAY_BUFFER, columnVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(columnVertices), columnVertices, GL_STATIC_DRAW);

    // Generate and bind VAO (Vertex Array Object)
    glGenVertexArrays(1, &columnVAO);
    glBindVertexArray(columnVAO);

    // Link VBO with the vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); // Position
    glEnableVertexAttribArray(0); // Enable vertex attribute at index 0

    // Unbind VAO and VBO (to avoid accidental changes)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderColumn(Shader& shaderProgram, Camera camera) {
    shaderProgram.Activate();
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    shaderProgram.Activate();
    camera.Matrix(shaderProgram, "camMatrix");

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -30.0f));
    GLuint modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram.ID, "view");


    glm::mat4 columnModel = glm::mat4(1.0f);
    columnModel = glm::translate(columnModel, glm::vec3(1, 0.0f, 1));
    columnModel = glm::scale(columnModel, glm::vec3(2.2f, 2.0f, 2.2f));
   
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(columnModel));

    // Use the VAO to render the object
    glBindVertexArray(columnVAO);
    glDrawArrays(GL_LINES, 0, 36 * 2); // Render 36 segments (2 vertices per segment)
    glBindVertexArray(0);
}

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

    KDOP14 kdop;
    aiMesh* mesh = scene->mMeshes[0]; // this is our fish mesh i suppose?
    kdop.computeFromMesh(mesh);

    for (const auto& plane : kdop.planes) {
        std::cout << "Plane normal: (" << plane.normal.x << ", " << plane.normal.y << ", " << plane.normal.z << ") ";
        std::cout << "d: " << plane.d << std::endl;
    }
    kdop.render();
}

GLuint loadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Load the texture data using stb_image
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

        // Set texture parameters
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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "mywindow", NULL, NULL);
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
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);

    fishNormalMap = loadTexture("../textures/fish_normal_map.png");
    fishTexture = loadTexture("../textures/fish_texture.png");
    if (fishNormalMap == 0 || fishTexture == 0) {
        std::cout << "Error: Failed to load one or more textures!" << std::endl;
        return -1;
    }

    // shader for our basic wire cube
    Shader shaderProgram("../shaders/cube.vert", "../shaders/cube.frag");
    VAO boxVAO;
    boxVAO.Bind();
    VBO boxVBO(boxVertices, sizeof(boxVertices));
    EBO boxEBO(boxIndices, sizeof(boxIndices));
    // position
    boxVAO.LinkAttrib(boxVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    // color
    boxVAO.LinkAttrib(boxVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //normals - for light
    boxVAO.LinkAttrib(boxVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    boxVAO.Unbind();
    boxVBO.Unbind();
    boxEBO.Unbind();
    shaderProgram.Activate();

    Shader fishShader("../shaders/fish_shader.vert", "../shaders/fish_shader.frag");
    fishShader.Activate();

    //set up a pyramid shaped vao, so it represents our boids
    //setupPyramid();

    // matrixes for the camera
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -30.0f)); // sets how far away we are from the cube
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 10.0f));
    
    // !!!!!!!!!!!!!!!!!!!!!!!!!
    std::vector<Boid> boids;
    loadModelToContext("../resources/models/fish.obj", fishContext);
    setUpBoids(boids, 3, 70); // set up num of boid groups you want here : boids, num of groups, num of boids in each group
    for (auto& boid : boids) {
        boid.context = &fishContext;
    }

    while (!glfwWindowShouldClose(window)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        processInput(window);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //allows for camera movement
        processInput(window);
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        bool shaderReloaded = false;

        // set camera position for both shaders
        shaderProgram.Activate();
        shaderProgram.SetVec3("cameraPos", camera.Position);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        fishShader.Activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fishNormalMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fishTexture);


        camera.Matrix(fishShader, "camMatrix");


        // set uniforms for fish shader
        fishShader.SetInt("fishNormalMap", 0);
        fishShader.SetInt("fishTexture", 1);

        fishShader.SetMat4("view", view);
        fishShader.SetMat4("modelMatrix", model);
        fishShader.SetVec3("lightPos", lightPos);
        fishShader.SetVec3("lightColor", lightColor);
        fishShader.SetVec3("cameraPos", camera.Position);
        fishShader.SetVec3("objectColor", objectColor);
        
        // program shader
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camMatrix");

        GLuint modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram.ID, "view");

        glm::mat4 identityMatrix = glm::mat4(1.0f);
        model = glm::scale(identityMatrix, glm::vec3(100.0f, 100.0f, 100.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        boxVAO.Bind();
        //draws wire cube for the boids to fly in
        glUniform3fv(glGetUniformLocation(shaderProgram.ID, "color"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glDrawElements(GL_LINES, sizeof(boxIndices)/ sizeof(int), GL_UNSIGNED_INT, 0);

        // draw obstacles -- collumns
        renderColumn(shaderProgram, camera);
        
        // boid rendering and updating
        for (auto& boid : boids) {       
            boid.update(boids);          
        }
        renderBoids(boids, fishShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "util.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800 * 2;
const unsigned int SCR_HEIGHT = 600 * 2;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 55.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


void renderDoctorStrange(Shader& shader, Model& model, const glm::mat4& projection, const glm::mat4& view)
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(0.0f, -24.0f, 0.0f));
    transform = glm::scale(transform, glm::vec3(32.0f, 32.0f, 32.0f));
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", transform);
    shader.setInt("texture_diffuse1", 0);
    model.Draw(shader);
}


void renderTimeStone(Shader& shader, Model& model, const glm::mat4& projection, const glm::mat4& view, float rotateAngle)
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 32.0f));
    transform = glm::scale(transform, glm::vec3(1.0f, 1.2f, 1.0f));
    transform = glm::rotate(transform, glm::radians(rotateAngle), glm::vec3(0.2f, 1.0f, 0.3f));
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", transform);
    shader.setInt("texture_diffuse1", 0);
    model.Draw(shader);
}


void renderBrokenGlass(Shader& shader, const glm::mat4& projection, const glm::mat4& view, unsigned int brokenGlassVAO, unsigned int dynEnvMapTextureID, unsigned int brokenGlassTexture)
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 40.0f));
    transform = glm::scale(transform, glm::vec3(4.0f, 4.0f, 1.0f));
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", transform);
    shader.setInt("environmentMap", 0);
    shader.setInt("brokenGlassTexture", 1);

    glBindVertexArray(brokenGlassVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dynEnvMapTextureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, brokenGlassTexture);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}


void renderSkybox(Shader& shader, const glm::mat4& projection, const glm::mat4& view, unsigned int skyboxVAO, unsigned int skyboxTexture)
{
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH / 2, SCR_HEIGHT / 2, "GPU Programming Term Project - 202115064 KIM DONG JOO", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader timeStoneShader("shader/timestone.vs", "shader/timestone.fs");
    Shader drStrangeShader("shader/dr-strange.vs", "shader/dr-strange.fs");
    Shader skyboxShader("shader/skybox.vs", "shader/skybox.fs");
    Shader brokenGlassShader("shader/broken-glass.vs", "shader/broken-glass.fs");

    // load models
    // -----------
    Model timeStone("resources/objects/timestone/timestone.obj");
    Model drStrange("resources/objects/dr-strange/Dr Strange.obj");

    // set up vertex data (and buffer(s)) and configure vertex attribute
    // Model ourModel("resources/objects/backpack/backpack.obj");

    // ------------------------------------------------------------------
    float brokenGlassVerticies[] = {
        // positions        // texture coords
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        1.0f, 1.0f, 0.0f,    1.0f, 1.0f, // top right
        -1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };

    // broken glass VAO
    unsigned int brokenGlassVAO, brokenGlassVBO;
    glGenVertexArrays(1, &brokenGlassVAO);
    glGenBuffers(1, &brokenGlassVBO);
    glBindVertexArray(brokenGlassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, brokenGlassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(brokenGlassVerticies), brokenGlassVerticies, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int brokenGlassTexture = loadTexture("resources/textures/broken-glass/WireReinforced_N.jpg");

    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Skybox Cubemap
    std::vector<std::string> skyboxFaces
    {
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    };
    unsigned int skyboxTexture = loadCubemap(skyboxFaces);

    // Time Stone
    float timeStoneRotateSpeed = 32.0f;
    float timeStoneRotateAngle = 0.0f;

    // Dynamic Environment Map
    unsigned int dynEnvMapTextureID;
    glGenTextures(1, &dynEnvMapTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dynEnvMapTextureID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


    // shader configuration
    // --------------------
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    // brokenGlassShader.use();
    // brokenGlassShader.setInt("normal", brokenGlassTexture);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        glm::mat4 model;

        // time stone 회전각 업데이트
        timeStoneRotateAngle += timeStoneRotateSpeed * deltaTime;
        while (timeStoneRotateAngle > 360.0f) {
            timeStoneRotateAngle -= 360.0f;
        }

        // Dynamic Environment Map Rendering
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynEnvMapTextureID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            unsigned int framebuffer;
            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            // 큐브맵 텍스처를 프레임 버퍼의 컬러 첨부물로 연결
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dynEnvMapTextureID, 0);
            // 큐브맵 렌더링
            renderDoctorStrange(drStrangeShader, drStrange, projection, view);
            renderTimeStone(timeStoneShader, timeStone, projection, view, timeStoneRotateAngle);
            renderSkybox(skyboxShader, projection, skyboxView, skyboxVAO, skyboxTexture);
            // 프레임 버퍼 정리
            glDeleteFramebuffers(1, &framebuffer);
        }
        // 큐브맵 텍스처 언바인딩
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // draw scene as normal
        renderDoctorStrange(drStrangeShader, drStrange, projection, view);
        renderTimeStone(timeStoneShader, timeStone, projection, view, timeStoneRotateAngle);
        renderBrokenGlass(brokenGlassShader, projection, view, brokenGlassVAO, dynEnvMapTextureID, brokenGlassTexture);
        renderSkybox(skyboxShader, projection, skyboxView, skyboxVAO, skyboxTexture);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        float velocity = camera.MovementSpeed * deltaTime;
        camera.Position += camera.Up * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        float velocity = camera.MovementSpeed * deltaTime;
        camera.Position += camera.Up * -velocity;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

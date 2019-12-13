#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void readFile(const char* filename, std::vector<char>& buffer);

// Base application class which contains the boilerplate code for initializing.
class Application
{
public:
    Application(int width, int height);
    GLFWwindow* window;
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
private:

};

// Shader class for loading shader from file and compiling.
class Shader {
public:
    Shader (const char *vectFile, const char *fragFile);
    void use();
    void setInt(const char *name, int a);
    unsigned int shaderProgram;
private:
    unsigned int vertexShader, fragShader;
    readFile(const char* filename, std::vector<char>& buffer);
    void compile(const char* Path, GLenum type, unsigned int *handle);
    void linkShaders();
};

// Class for managing camera state.
class Camera {
public:
    Camera(glm::vec3 position, float yaw, float pitch, float speed, float sensitivity);
    glm::mat4 getViewMatrix();
    void processKeys(float deltaTime, GLFWwindow* window);
    void processMouse(float xoffset, float yoffset);

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float speed;
    float sensitivity;

    void update_vectors();
};

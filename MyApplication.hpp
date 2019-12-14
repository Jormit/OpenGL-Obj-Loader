#include "Application.hpp"

// The my application class is where the guts of your processing happen.
class MyApplication : public Application {
public:
    MyApplication(int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

private:
    GLuint vao, vbo, ebo;
    unsigned int texture;
    Shader shaderProgram;
    Camera camera;

    std::vector<Mesh> meshes;

    float deltaTime = 0.0;
    float lastFrame = 0.0;

    float lastX;
    float lastY;
    bool firstMouse = true;

    virtual void loop();
    virtual void process_input();
};

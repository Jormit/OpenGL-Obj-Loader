#include "MyApplication.hpp"

using namespace std;

// The main guts of application go here.
// -------------------------------------
MyApplication::MyApplication(int width, int height) :
    Application(width, height),
    shaderProgram ("./shaders/vert.glsl", "./shaders/frag.glsl"),
    camera(glm::vec3(0.0f, 0.0f, 3.0f), -90.0f, 0.0f, 5.0f, 0.1f)
{
    // This needs to be done here so we have access to the mouse data.
    // Possible refactor in the future.
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetWindowUserPointer(window, this);

    lastX = (float) width / 2.0f;
    lastY = (float) height / 2.0f;

    meshes.push_back(Mesh("assets/models/cube.obj"));
    meshes[0].setupBuffers(shaderProgram, "assets/textures/wall.jpg", GL_TEXTURE_2D);
    meshes[0].translate(glm::vec3(2.0f, 0.0f, 0.0f));
    meshes[0].rotate(50.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    meshes.push_back(Mesh("assets/models/teapot.obj"));
    meshes[1].setupBuffers(shaderProgram, "assets/textures/wall2.jpg", GL_TEXTURE_2D);
    meshes[1].translate(glm::vec3(-1.0f, 0.0f, 0.0f));
    meshes[1].scale(glm::vec3(0.1f, 0.1f, 0.1f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float) 800 / (float) 600, 0.1f, 100.0f);
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    loop();
}

// Main loop of the application.
// -----------------------------
void MyApplication::loop()
{
    while(!glfwWindowShouldClose(window))
    {
        // Handle delta time.
        // ------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process user Input.
        // --------------
        process_input();

        // Set up view matrix to add perspective.
        // ----------------
        unsigned int viewLoc = glGetUniformLocation(shaderProgram.shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

        // Render the screen.
        // ------------------
        shaderProgram.use();
        meshes[0].draw(shaderProgram);
        meshes[1].draw(shaderProgram);

        // Flip buffers and clear z-buffer.
        // --------------------------------
        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
    }
}

// Process keypress events.
// ------------------------
void MyApplication::process_input()
{
    camera.processKeys(deltaTime, window);

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// Mouse callback calculates the offset from last mouse position and passes these to the camera.
// No more work needed here.
void  MyApplication::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Retrieve the Application class pointer.
    void *data = glfwGetWindowUserPointer(window);
    MyApplication *app = static_cast<MyApplication *>(data);

    if (app->firstMouse)
    {
        app->lastX = xpos;
        app->lastY = ypos;
        app->firstMouse = false;
    }

    float xoffset = xpos - app->lastX;
    float yoffset = app->lastY - ypos;

    app->lastX = xpos;
    app->lastY = ypos;

    app->camera.processMouse(xoffset, yoffset);
}


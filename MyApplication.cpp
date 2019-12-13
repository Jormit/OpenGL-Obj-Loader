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

    // Define Geometry.
    // ----------------
    float vertices[] = {
         // Position          // Color           // Texture
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // Create vao, vbo, ebo.
    // ---------------------
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // Bind to vertex array.
    glBindVertexArray(vao);

    // Copy vertices into vertex buffer object.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Copy indices into element buffer object.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Load and create texure.
    // -----------------------
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set wrapping parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set filtering parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load texture image.
    int tex_width, tex_height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("assets/textures/wall.jpg", &tex_width, &tex_height, &nrChannels, 0);

    // If texture loaded successfuly then generate mipmaps.
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cerr << "Could not load texture!" << endl;
        exit(1);
    }
    stbi_image_free(data);

    // Set texture uniform for the shader to use.
    shaderProgram.use();
    shaderProgram.setInt("texture", 0);

    // Model, View and Projection transformations.
    // -------------------------------------------
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);

    // Send matrices to vertex shader.
    unsigned int modelLoc = glGetUniformLocation(shaderProgram.shaderProgram, "model");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram.shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
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

        // Set up view matrix.
        // ----------------
        unsigned int viewLoc = glGetUniformLocation(shaderProgram.shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

        // Render the screen.
        // ------------------
        shaderProgram.use();
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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


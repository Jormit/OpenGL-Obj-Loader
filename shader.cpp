#include "Application.hpp"
using namespace std;

Shader::Shader (const char *vectFile, const char *fragFile)
{
     // Compile the shaders.
    compile(vectFile, GL_VERTEX_SHADER, &vertexShader);
    compile(fragFile, GL_FRAGMENT_SHADER, &fragShader);

    // Link shaders into main shader program and cleanup.
    linkShaders();
}

void Shader::use ()
{
    glUseProgram(shaderProgram);
}

void Shader::setInt(const char *name, int a)
{
    glUniform1i(glGetUniformLocation(shaderProgram, name), a);
}

void Shader::compile(const char* Path, GLenum type, unsigned int *handle)
{
    // Read shader from path.
    vector<char> fileContent;

    readFile(Path, fileContent);
    const char* shaderText (&fileContent[0]);

    // Create and compile the shader.
    *handle = glCreateShader(type);

    glShaderSource(*handle, 1, (const GLchar**)&shaderText, NULL);
    glCompileShader(*handle);

    // Check for any errors.
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        exit(1);
    }
}

void Shader::linkShaders()
{
    // Attatch the compiled shaders to the main program.
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    // Check for any errors.
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        exit(1);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
}

int Shader::readFile(const char* filename, std::vector<char>& buffer)
{
    ifstream file (filename, ifstream::in);

    if (file)
    {
        file.seekg(0, ios_base::end);
        streamsize size = file.tellg();
        if (size > 0) {
            file.seekg(0, ios_base::beg);
            buffer.resize((size_t)size);
            file.read(&buffer[0], size);
        }
        buffer.push_back('\0');
    }
    else
    {
        cerr << "Cannot open " << filename << endl;
    }
}

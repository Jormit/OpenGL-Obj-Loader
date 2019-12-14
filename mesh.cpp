#include "Application.hpp"

using namespace std;

Mesh::Mesh(const char * path)
{
    FILE *file = fopen(path, "r");

    if (file)
    {
        vector< unsigned int > vertexIndices, uvIndices, normalIndices;
        vector< glm::vec3 > temp_vertices; // v
        vector< glm::vec2 > temp_uvs;      // vt
        vector< glm::vec3 > temp_normals;  // vn

        // Read in file into temp format.
        while (1)
        {
            char lineHeader[128];
            int res = fscanf(file, "%s", lineHeader);
            if (res == EOF)
            {
                break;
            }

            if (strcmp(lineHeader, "v") == 0)
            {
                glm::vec3 vertex;
                fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
                temp_vertices.push_back(vertex);

            }
            else if (strcmp(lineHeader, "vt") == 0)
            {
                glm::vec2 textureCoord;
                fscanf(file, "%f %f\n", &textureCoord.x, &textureCoord.y);
                temp_uvs.push_back(textureCoord);
            }
            else if (strcmp(lineHeader, "vn") == 0)
            {
                glm::vec3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
                temp_normals.push_back(normal);
            }
            else if (strcmp(lineHeader, "f") == 0)
            {
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                if (matches != 9){
                    cout << "File cannot be read\n" << endl;
                    exit(1);
                }

                vertexIndices.push_back(vertexIndex[0] - 1);
                vertexIndices.push_back(vertexIndex[1] - 1);
                vertexIndices.push_back(vertexIndex[2] - 1);
                uvIndices    .push_back(uvIndex[0] - 1);
                uvIndices    .push_back(uvIndex[1] - 1);
                uvIndices    .push_back(uvIndex[2] - 1);
                normalIndices.push_back(normalIndex[0] - 1);
                normalIndices.push_back(normalIndex[1] - 1);
                normalIndices.push_back(normalIndex[2] - 1);
            }
        }

        // Reshape data so opengl can use it.
        for (int i = 0; i < vertexIndices.size(); i+=3)
        {
            Vertex temp_vertex;

            for (int s = 0; s < 3; s++)
            {
                temp_vertex.position = temp_vertices.at(vertexIndices[s + i]);
                temp_vertex.normal = temp_normals.at(normalIndices[s + i]);
                temp_vertex.texCoord = temp_uvs.at(uvIndices[s + i]);
                vertices.push_back(temp_vertex);
            }
        }
    }

    else
    {
        cerr << "Cannot open " << path << endl;
        exit(1);
    }

}

void Mesh::setupBuffers(Shader shaderProgram, const char * texturePath, int textureType) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Bind to vertex array.
    glBindVertexArray(vao);

    // Copy vertices into vertex buffer object.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(glm::vec3) * 2 + sizeof(glm::vec2)), &vertices[0], GL_STATIC_DRAW);

    // Setup attributes.
    // -----------------
    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute (change to normal soon..).
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Load and create texure.
    // -----------------------
    setupTexture(shaderProgram, texturePath, textureType);

    // Set up model transformation to identity matrix.
    model = glm::mat4(1.0f);
}

void Mesh::draw(Shader shaderProgram) {
    // Send matrices to vertex shader.
    unsigned int modelLoc = glGetUniformLocation(shaderProgram.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindTexture(texture.type, texture.id);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

// Loads texture from specified file and set up for usage.
void Mesh::setupTexture(Shader shaderProgram, const char * texturePath, int textureType) {
    texture.type = textureType;
    glGenTextures(1, &(texture.id));
    glBindTexture(texture.type, texture.id);
    // Set wrapping parameters.
    glTexParameteri(texture.type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture.type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set filtering parameters.
    glTexParameteri(texture.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load texture image.
    int tex_width, tex_height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texturePath, &tex_width, &tex_height, &nrChannels, 0);

    // If texture loaded successfuly then generate mipmaps.
    if (data)
    {
        glTexImage2D(texture.type, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(texture.type);
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
}

void Mesh::translate(glm::vec3 direction)
{
    model = glm::translate(model, direction);
}

void Mesh::rotate(float angle, glm::vec3 axis)
{
    model = glm::rotate(model, glm::radians(angle), axis);
}

void Mesh::scale(glm::vec3 factor)
{
    model = glm::scale(model, factor);
}

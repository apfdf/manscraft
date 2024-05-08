
#include <bits/stdc++.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_SIZE 512
#define PI 3.14159265359

using namespace std;

GLint compileShader(const char* filename, GLenum type) {

    FILE* file = fopen(filename, "rb");

    if (file == NULL) {
        std::cerr << "Cannot open shader " << filename << std::endl;
        abort();
    }

    fseek(file, 0, SEEK_END);
    const int size = ftell(file);
    rewind(file);

    const GLchar* source = new GLchar[size+1];
    fread(const_cast<char*>(source), sizeof(char), size, file);
    const_cast<char&>(source[size]) = '\0';

    const GLint shader = glCreateShader(type);

    if (not shader) {
        std::cerr << "Cannot create a shader of type " << shader << std::endl;
        abort();
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    {
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (not compiled) {
            std::cerr << "Cannot compile shader " << filename << std::endl;
            abort();
        }
    }

    return shader;

}

GLint compileShaderProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename) {

    const GLint program = glCreateProgram();

    if (not program) {
        std::cerr << "Cannot create a shader program" << std::endl;
        abort();
    }

    glAttachShader(program, compileShader(vertexShaderFilename, GL_VERTEX_SHADER));
    glAttachShader(program, compileShader(fragmentShaderFilename, GL_FRAGMENT_SHADER));

    glLinkProgram(program);

    {
        GLint linked;
        glGetShaderiv(program, GL_LINK_STATUS, &linked);
        if (not linked) {
            std::cerr << "Cannot link shader program with shaders " << vertexShaderFilename << " and " << fragmentShaderFilename << std::endl;
            abort();
        }
    }

    return program;

}

struct Player {
    glm::vec3 p;
    float yaw;
    float pitch;
    float v;
    float rotate_v;
    bool creative;
};

struct Chunk {
    float x, z;
    vector<vector<vector<bool>>> data;
};

int main() {

    if (!glfwInit()) {
        cerr << "GLFW init failed." << endl;
        abort();
    }

    int ww = 800;
    int wh = 800;
    GLFWwindow* window;
    window = glfwCreateWindow(ww, wh, "this is a window", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        cerr << "GLEW init failed." << endl;
        abort();
    }

    const GLint shader_program = compileShaderProgram("../src/vertex.glsl", "../src/fragment.glsl");
    glUseProgram(shader_program);

    unordered_map<const char*, GLuint> uniform_locations;

    Player plr = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        true
    };

    double time_when_fps = glfwGetTime();
    int updates_since_fps = 0;
    float dt = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        
        float time_start = glfwGetTime();

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_W)) {
            plr.p += glm::vec3(cos(plr.yaw), 0.0f, sin(plr.yaw)) * plr.v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_S)) {
            plr.p -= glm::vec3(cos(plr.yaw), 0.0f, sin(plr.yaw)) * plr.v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_D)) {
            plr.p += glm::vec3(cos(plr.yaw + PI / 2), 0.0f, sin(plr.yaw + PI / 2)) * plr.v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_A)) {
            plr.p -= glm::vec3(cos(plr.yaw + PI / 2), 0.0f, sin(plr.yaw + PI / 2)) * plr.v * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            plr.yaw += plr.rotate_v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            plr.yaw -= plr.rotate_v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_UP)) {
            plr.pitch += plr.rotate_v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            plr.pitch -= plr.rotate_v * dt;
        }

        glfwSwapBuffers(window);

        updates_since_fps++;
        if (glfwGetTime() - time_when_fps >= 1.0) {
            cout << "FPS: " << floor(updates_since_fps / (glfwGetTime() - time_when_fps)) << endl;
            time_when_fps = glfwGetTime();
            updates_since_fps = 0;
        }
        dt = glfwGetTime() - time_start;

    }

    glfwTerminate();

}

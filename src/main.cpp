
#include <bits/stdc++.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_SIZE 128
#define PI 3.14159265359

using namespace std;

void error(string mes) {
    cerr << mes << endl;
    exit(1);
}

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
    float fov;
};

struct ShaderProgram {

    int id;
    unordered_map<const char*, uint> uloc;

    void add_uloc(const char* name) {
        uloc[name] = glGetUniformLocation(id, name);
    }

};

// ignore for now
struct Vertex {

    float x, y, z;
    int index;

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

    glViewport(0, 0, ww, wh);

    ShaderProgram default_program;
    default_program.id = compileShaderProgram("../src/default.vert", "../src/default.frag");
    glUseProgram(default_program.id);

    default_program.add_uloc("view_mat");
    default_program.add_uloc("project_mat");
    default_program.add_uloc("vertices");
    default_program.add_uloc("vertices_amount");
    default_program.add_uloc("lights");
    default_program.add_uloc("lights_amount");

    Player plr = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        PI
    };

    GLfloat vertices[] = {
        1.0f, 1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, -1.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 1.0f,
        1.0f, 0.01, -1.0f, 1.0f,
        0.0f, 0.0f, -1.0f, 1.0f
	};

    int vertices_amount = (sizeof(vertices) / sizeof(float)) / 4;
    cout << vertices_amount << endl;

    glm::vec3 vertices1[BUFFER_SIZE];
    for (int i = 0; i < vertices_amount; i++) {
        vertices1[i] = {vertices[i*4], vertices[i*4+1], vertices[i*4+2]};
    }

    glm::vec3 lights[BUFFER_SIZE];
    lights[0] =  {0.0f, 0.0f, 0.0f};

    int lights_amount = 1;

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

    double time_when_fps = glfwGetTime();
    int updates_since_fps = 0;
    float dt = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        
        float time_start = glfwGetTime();

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            break;
        }

        if (glfwGetKey(window, GLFW_KEY_W)) {
            plr.p -= glm::vec3(sin(plr.yaw), 0.0f, cos(plr.yaw)) * plr.v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_S)) {
            plr.p += glm::vec3(sin(plr.yaw), 0.0f, cos(plr.yaw)) * plr.v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_D)) {
            plr.p += glm::vec3(sin(plr.yaw + PI / 2), 0.0f, cos(plr.yaw + PI / 2)) * plr.v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_A)) {
            plr.p -= glm::vec3(sin(plr.yaw + PI / 2), 0.0f, cos(plr.yaw + PI / 2)) * plr.v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE)) {
            plr.p.y += plr.v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
            plr.p.y -= plr.v * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            plr.yaw -= plr.rotate_v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            plr.yaw += plr.rotate_v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_UP)) {
            plr.pitch += plr.rotate_v * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            plr.pitch -= plr.rotate_v * dt;
        }

        glm::mat4 view_mat = glm::mat4(1.0f);
        view_mat = glm::rotate(view_mat, -plr.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        view_mat = glm::rotate(view_mat, -plr.yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        view_mat = glm::translate(view_mat, -plr.p);

        glm::mat4 project_mat = glm::mat4(1.0f);
        project_mat = glm::perspective(plr.fov / 2, (float)ww / wh, 0.1f, 100.0f);

        glUniformMatrix4fv(default_program.uloc["view_mat"], 1, false, glm::value_ptr(view_mat));
        glUniformMatrix4fv(default_program.uloc["project_mat"], 1, false, glm::value_ptr(project_mat));

        glUniform3fv(default_program.uloc["vertices"], BUFFER_SIZE, glm::value_ptr(vertices1[0]));
        glUniform1i(default_program.uloc["vertices_amount"], vertices_amount);

        glUniform3fv(default_program.uloc["lights"], BUFFER_SIZE, glm::value_ptr(lights[0]));
        glUniform1i(default_program.uloc["lights_amount"], lights_amount);

        glClearColor(0.13f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);

        // do default shader stuff

        glUseProgram(default_program.id);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices_amount);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
	    glBindVertexArray(0);

        glUseProgram(default_program.id);

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

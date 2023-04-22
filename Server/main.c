/* Copyright 2023 Nathan Pratt and Parker Candland
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
**/
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gui.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLFWwindow* window;
int terminate;
const float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left
};

const unsigned int indices[] = {
    0, 1, 3, // First trinangle
    1, 2, 3  // Second triangle.
};

// TODO: Add uv_coords.
const float uv_coords[] = {
    1
};

void sig_interrupt() {
    if (terminate == 1) {
        puts("Force Terminate\n");
        exit(-1);
    }
    terminate = 1;
}

void opengl_check_error() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "Error: %d\n", err);
        fprintf(stderr, "1");
    }
}

void load_texture(char* path) {
    int width;
    int height;
    int num_channels;

    unsigned char* image_data = stbi_load(path, &width, &height, &num_channels, 0);

    GLuint tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
}

int main(int argc, char** argv) {
    if (!glfwInit())
    {
        fprintf(stderr, "Error: Failed to initialize glfw.\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    fprintf(stdout, "Initialized glfw.\n");



    window = glfwCreateWindow(1280, 720, "PatternGen", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        fprintf(stderr, "Error: Failed to create window.\n");
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
    {
        fprintf(stderr, "Error: Failed to initialize GLEW.\n");
        return -1;
    }
    fprintf(stdout, "Initialized glew.\n");
    signal(SIGINT, sig_interrupt);

    glViewport(0, 0, 1280, 720);

    shader_file_tuple tuple = load_shader_files("vertex.glsl", "fragment.glsl");
    if (tuple.status == -1) {
        fprintf(stderr, "Error: ");
        fprintf(stderr, "%s\n", shader_file_open_error_string(tuple.error));
        goto shutdown_deinit;
    }
    GLuint program = compile_shader_files(tuple);
    if (program == -1) {
        goto shutdown_deinit;
    }
    unload_shader_files(tuple);

    gui_handler* gui = init_gui_handle(window);

    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW);

    unsigned int EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);

    while(!terminate) {
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
            terminate = 1;
        }

        glClearColor(0.f,0.0f,0.0f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);

        glUseProgram(program);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(0);
        opengl_check_error();

        process_gui_handle(gui);
        glfwSwapBuffers(window);
    }

    shutdown_deinit:
    destroy_gui_handle(gui);
    glfwDestroyWindow(window);
    glfwTerminate();

    fprintf(stdout, "Terminated Window\n");
    exit(0);
}

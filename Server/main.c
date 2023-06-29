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
#include <stdlib.h>
#include <signal.h>
#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gui.h"
#include "shader.h"
#include "canvas.h"
#include "image.h"

GLFWwindow* window;
int terminate;

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

int main() {
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

    window = glfwCreateWindow(1280, 1280, "PatternGen", NULL, NULL);
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

    glViewport(0, 0, 1280, 1280);

    shader_file_tuple tuple = load_shader_files("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (tuple.status == -1) {
        fprintf(stderr, "Error: ");
        fprintf(stderr, "%s\n", shader_file_open_error_string(tuple.error));
        goto shutdown_deinit;
    }
    GLuint program = compile_shader_files(tuple);
    if (program == (unsigned int)-1) {
        goto shutdown_deinit;
    }
    unload_shader_files(tuple);


    canvas_t* canvas = create_canvas_object(500, 500);
    gui_handler* gui = init_gui_handle(window, canvas);

    while(!terminate) {
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
            terminate = 1;
        }

        glClearColor(0.f,0.0f,0.0f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        opengl_check_error();

        // TODO: It would probably be a good idea to associate the
        // image and the shader with draw_canvas_object and canvas_t.
        // That way it removes a lot of jank.
        if (!gui->binded && !gui->finished){
            // Do nothing.
        } else if (gui->binded && !gui->finished) {
            bind_image(gui->loaded);
            draw_canvas_object(canvas);
        } else if (gui->binded && gui->finished) {
            bind_image(gui->reprocessed);
            draw_canvas_object(canvas);
        }

        process_gui_handle(gui);
        glfwSwapBuffers(window);
    }

    destroy_canvas_object(canvas);
    destroy_gui_handle(gui);
    shutdown_deinit:
    glfwDestroyWindow(window);
    glfwTerminate();

    fprintf(stdout, "Terminated Window\n");
    exit(0);
}

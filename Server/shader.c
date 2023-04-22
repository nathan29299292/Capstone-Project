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
#include "shader.h"
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

int check_file_open_error (int fd, int* error) {
    if (fd == -1){
        *error = errno;
        return -1;
    } else {
        return 0;
    }
}

int check_file_map_error (void* map, int* error) {
    if (map == (void*)-1){
        *error = errno;
        return -1;
    } else {
        return 0;
    }
}

const char* shader_file_open_error_string (int error) {
    switch (errno) {
        case EACCES:
            return "File doesn't exist or isn't readable\n";
        case ENOENT:
            return "File doesn't exist or isn't readable\n";
        case EISDIR:
            return "File is a directory.\n";
        default:
            return "Something went wrong reading the file.\n";
    }
}

shader_file_tuple load_shader_files(const char* vertex_shader_path, const char* fragment_shader_path) {
    int error = 0;
    int vertex_fd = open(vertex_shader_path, O_RDONLY);
    if (check_file_open_error(vertex_fd, &error) == -1) {
        shader_file_tuple return_tuple = {0, 0, -1, error, NULL, NULL};
        return return_tuple;
    }
    int fragment_fd = open(fragment_shader_path, O_RDONLY);
    if (check_file_open_error(fragment_fd, &error) == -1) {
        shader_file_tuple return_tuple = {0, 0, -1, error, NULL, NULL};
        return return_tuple;
    }

    struct stat vertex_file_stat;
    fstat(vertex_fd, &vertex_file_stat);

    struct stat fragment_file_stat;
    fstat(vertex_fd, &fragment_file_stat);

    unsigned long vertex_file_size = vertex_file_stat.st_size;
    unsigned long fragment_file_size = fragment_file_stat.st_size;

    char* vertex_code;
    char* fragment_code;

    vertex_code = (char*)mmap(NULL, vertex_file_size, PROT_READ, MAP_PRIVATE, vertex_fd, 0);
    if (check_file_map_error((void*)vertex_code, &error) == -1) {
        shader_file_tuple return_tuple = {0, 0, -1, error, NULL, NULL};
        return return_tuple;
    }

    fragment_code = (char*)mmap(NULL, fragment_file_size, PROT_READ, MAP_PRIVATE, fragment_fd, 0);
    if (check_file_map_error((void*)fragment_code, &error) == -1) {
        shader_file_tuple return_tuple = {0, 0, -1, error, NULL, NULL};
        return return_tuple;
    }

    shader_file_tuple return_tuple = {vertex_file_size, fragment_file_size, 0, 0, vertex_code, fragment_code};
    return return_tuple;
}

void unload_shader_files(shader_file_tuple tuple) {
    munmap(tuple.vertex_code, tuple.vertex_file_size);
    munmap(tuple.vertex_code, tuple.vertex_file_size);
}

unsigned int compile_shader_files(shader_file_tuple tuple) {
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile the shaders.
    glShaderSource(vertex_shader, 1, (const char **)(&(tuple.vertex_code)), NULL);
    glShaderSource(fragment_shader, 1, (const char **)(&(tuple.fragment_code)), NULL);

    glCompileShader(vertex_shader);

    int vsuccess = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vsuccess);

    if (vsuccess == GL_FALSE) {
        fprintf(stderr, "Error: Vertex Shader Failed -> ");
        // Get shader log errors.
        int logSize = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logSize);
        char errorLog[logSize];
        glGetShaderInfoLog(vertex_shader, logSize, &logSize, errorLog);
        fprintf(stderr, "%s", errorLog);
        return -1;
    }

    glCompileShader(fragment_shader);

    int fsuccess = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fsuccess);

    if (fsuccess == GL_FALSE) {
        fprintf(stderr, "Error: Fragment Shader Failed -> ");
        // Get shader log errors.
        int logSize = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logSize);
        char errorLog[logSize];
        glGetShaderInfoLog(fragment_shader, logSize, &logSize, errorLog);
        fprintf(stderr, "%s", errorLog);
        return -1;
    }

    // Link the programs together.
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

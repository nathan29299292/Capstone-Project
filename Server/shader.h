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
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#include <winbase.h>
#include <handleapi.h>
#include <errhandlingapi.h>
#endif

#ifdef _WIN32
typedef struct WindowsFileHandles {
    HANDLE hVertex;
    HANDLE hVertexFileMapping;

    HANDLE hFragment;
    HANDLE hFragmentFileMapping;
} WindowsFileHandles;
#endif

typedef struct shader_file_tuple {
    #ifdef _WIN32
    WindowsFileHandles handles; // PRIVATE
    #endif

    unsigned long vertex_file_size;
    unsigned long fragment_file_size;
    int status;
    int error;
    char* vertex_code;
    char* fragment_code;
} shader_file_tuple;

const char* shader_file_open_error_string (int error);
shader_file_tuple load_shader_files(const char* vertex_shader_path, const char* fragment_shader_path);
void unload_shader_files(shader_file_tuple tuple);
unsigned int compile_shader_files(shader_file_tuple tuple);

#endif

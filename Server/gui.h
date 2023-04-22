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
#ifndef GUI_H
#define GUI_H

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

#define GUI_MAX_FILE_PATH_TEXT_BUFFER 128

typedef struct gui_handler {
    struct nk_context* ctx;
    unsigned int gui_state;
    char file_path_textbox_buffer[GUI_MAX_FILE_PATH_TEXT_BUFFER];
} gui_handler;

gui_handler* init_gui_handle(GLFWwindow *window);
void process_gui_handle(gui_handler* gui);
void destroy_gui_handle(gui_handler* gui);

void gui_load_file_window(gui_handler* gui);
void gui_generate_pattern(gui_handler* gui);

#endif
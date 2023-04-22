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
#include "gui.h"
#include <string.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_GLFW_GL4_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl4.h"

#define NK_MAX_VERTEX_BUFFER 512 * 1024
#define NK_MAX_ELEMENT_BUFFER 128 * 1024

/**
 *
 *
 * A psuedo object-oriented style of programming.
 *
 *
 * */
gui_handler* init_gui_handle(GLFWwindow *window) {
    gui_handler* gui = malloc(sizeof(gui_handler));
    gui->file_path_textbox_buffer[0] = '\0'; // Set the initial value to zero at least.
    gui->gui_state = 0;
    gui->ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS, NK_MAX_VERTEX_BUFFER, NK_MAX_ELEMENT_BUFFER);

    struct nk_font_atlas *atlas;

    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font *source = nk_font_atlas_add_from_file(atlas, "./Source_Sans_Pro/SourceSansPro-Regular.ttf", 24, 0);
    nk_glfw3_font_stash_end();
    nk_style_set_font(gui->ctx, &source->handle);

    return gui;
}

void process_gui_handle(gui_handler* gui) {
    nk_glfw3_new_frame();

    if (gui->gui_state == 0) {
        gui_load_file_window(gui);
    } else if (gui->gui_state == 1) {
        gui_generate_pattern(gui);
    }

    nk_glfw3_render(NK_ANTI_ALIASING_ON);
}

void destroy_gui_handle(gui_handler* gui) {
   nk_glfw3_shutdown(); // The context should be freed at this point.
   free(gui);
}

void gui_load_file_window(gui_handler* gui) {
    if (nk_begin(gui->ctx, "Load File", nk_rect(50, 50, 600, 650), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE|NK_WINDOW_MINIMIZABLE)) {

        nk_layout_row_dynamic(gui->ctx, 50, 2);
        nk_label(gui->ctx, "File Path", NK_TEXT_ALIGN_LEFT|NK_TEXT_ALIGN_MIDDLE);
        nk_edit_string_zero_terminated(gui->ctx, NK_EDIT_FIELD, gui->file_path_textbox_buffer, sizeof(gui->file_path_textbox_buffer)-1, nk_filter_default);
        if (nk_button_label(gui->ctx, "Load")) {
            gui->gui_state = 1;
        }

    }
    nk_end(gui->ctx);
}

void gui_generate_pattern(gui_handler* gui) {
    if (nk_begin(gui->ctx, "Generator", nk_rect(50, 50, 600, 650), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE|NK_WINDOW_MINIMIZABLE)) {

        nk_layout_row_dynamic(gui->ctx, 50, 2);
        nk_label(gui->ctx, "File Path", NK_TEXT_ALIGN_LEFT|NK_TEXT_ALIGN_MIDDLE);
        nk_edit_string_zero_terminated(gui->ctx, NK_EDIT_FIELD, gui->file_path_textbox_buffer, sizeof(gui->file_path_textbox_buffer)-1, nk_filter_default);
        if (nk_button_label(gui->ctx, "Generate Pattern")) {
                /* event handling */
        }

    }
    nk_end(gui->ctx);
}
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
#include "canvas.h"
#include "image.h"
#include <GLFW/glfw3.h>
#include <string.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#define NK_MAX_VERTEX_BUFFER 512 * 1024
#define NK_MAX_ELEMENT_BUFFER 128 * 1024

/**
 *
 *
 * A psuedo object-oriented style of programming.
 *
 *
 * */
gui_handler* init_gui_handle(GLFWwindow *window, canvas_t* canvas) {
    gui_handler* gui = malloc(sizeof(gui_handler));
    gui->file_path_textbox_buffer[0] = '\0'; // Set the initial value to zero at least.
    gui->gui_state = 0;
    gui->binded = 0;
    gui->finished = 0;
    // Initialize the glfw_context.
    gui->glfw_ctx = malloc(sizeof(struct nk_glfw));
    // Intiialize the main context.
    gui->ctx = nk_glfw3_init(gui->glfw_ctx, window, NK_GLFW3_INSTALL_CALLBACKS);
    gui->canvas = canvas;

    struct nk_font_atlas *atlas;

    nk_glfw3_font_stash_begin(gui->glfw_ctx, &atlas);
    struct nk_font *source = nk_font_atlas_add_from_file(atlas, "fonts/default.ttf", 24, 0);
    nk_glfw3_font_stash_end(gui->glfw_ctx);
    nk_style_set_font(gui->ctx, &source->handle);

    gui->window = window; // This is important for loading an image.

    return gui;
}

void process_gui_handle(gui_handler* gui) {
    nk_glfw3_new_frame(gui->glfw_ctx);

    if (gui->gui_state == 0) {
        gui_load_file_window(gui);
    } else if (gui->gui_state == 1) {
        gui_generate_pattern(gui);
    } else if (gui->gui_state == 2) {
        gui_gcode_gen(gui);
    }

    nk_glfw3_render(gui->glfw_ctx, NK_ANTI_ALIASING_ON, NK_MAX_VERTEX_BUFFER, NK_MAX_ELEMENT_BUFFER);
}

void destroy_gui_handle(gui_handler* gui) {
   nk_glfw3_shutdown(gui->glfw_ctx); // The context should be freed at this point.
   free(gui->glfw_ctx);
   free(gui);
}

void gui_load_file_window(gui_handler* gui) {
    if (nk_begin(gui->ctx, "Load File", nk_rect(50, 50, 600, 650), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE|NK_WINDOW_MINIMIZABLE)) {

        nk_layout_row_dynamic(gui->ctx, 50, 2);
        nk_label(gui->ctx, "File Path", NK_TEXT_ALIGN_LEFT|NK_TEXT_ALIGN_MIDDLE);
        nk_edit_string_zero_terminated(gui->ctx, NK_EDIT_FIELD, gui->file_path_textbox_buffer, sizeof(gui->file_path_textbox_buffer)-1, nk_filter_default);
        if (nk_button_label(gui->ctx, "Load")) {
            gui->loaded = create_image(gui->file_path_textbox_buffer);
            resize_image(gui->loaded, 80, 80);
            if (gui->loaded == NULL) {
                nk_end(gui->ctx); // End the context.
                return;
            }
            {
                int window_w;
                int window_h;
                glfwGetFramebufferSize(gui->window, &window_w, &window_h);
                gui->gui_state = 1;
                mutate_canvas_object(gui->canvas, gui->loaded->width, gui->loaded->height, window_w, window_h, 1.0f);
                gui->binded = 1;
            }
        }
    }
    nk_end(gui->ctx);
}

void gui_generate_pattern(gui_handler* gui) {
    if (nk_begin(gui->ctx, "Generator", nk_rect(50, 50, 600, 650), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE|NK_WINDOW_MINIMIZABLE)) {

        nk_layout_row_dynamic(gui->ctx, 50, 2);
        if (nk_button_label(gui->ctx, "Generate Pattern") && gui->finished == 0) {
               gui->reprocessed = dither_image(gui->loaded);
               gui->finished = 1;
               gui->gui_state = 2;
        }

    }
    nk_end(gui->ctx);
}

void gui_gcode_gen(gui_handler* gui) {
    if (nk_begin(gui->ctx, "G-Code", nk_rect(50, 50, 600, 650), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE|NK_WINDOW_MINIMIZABLE)) {
        nk_layout_row_dynamic(gui->ctx, 400, 1);
        nk_edit_string_zero_terminated(gui->ctx, NK_EDIT_MULTILINE,gui->reprocessed->gcode_data, gui->reprocessed->gcode_data_length-1, nk_filter_default);
        nk_layout_row_dynamic(gui->ctx, 50, 2);
        if (nk_button_label(gui->ctx, "Save G-Code")) {
            dump_gcode_stack();
        }
    }
    nk_end(gui->ctx);
}

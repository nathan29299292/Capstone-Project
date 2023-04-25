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
#ifndef CANVAS_H
#define CANVAS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct canvas_t {
    unsigned int pixel_w;
    unsigned int pixel_h;

    // The calculated lower vertex of the rectangle
    // and upper vertex of the rectangle.
    float* rectangle_vertices;
    unsigned int* rectangle_indices;

    GLuint vertex_array_object;
    GLuint vertex_buffer_object;
    GLuint element_buffer_object;
} canvas_t;

#define WINDOW_HEIGHT 720

canvas_t* create_canvas_object(unsigned int pixel_w, unsigned int pixel_h);
void draw_canvas_object(canvas_t* canvas);
void destroy_canvas_object(canvas_t* canvas);

void mutate_canvas_object(canvas_t* canvas, unsigned int new_pixel_w, unsigned int new_pixel_h);

#endif

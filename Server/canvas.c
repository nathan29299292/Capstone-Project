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
#include "canvas.h"
#include <stdlib.h>

static float* generate_vertex_coords(unsigned int pixel_w, unsigned int pixel_h) {
    float* vertices = malloc(sizeof(float) * (3 + 2) * 4);

    // First we have to determine if the width is greater than the height.
    if (pixel_h > pixel_w) {
        float width = 2 * ((float)pixel_w / (float)pixel_h);
        vertices[0] = width / 2.f;
        vertices[1] = 1.0f;
        vertices[2] = 0.f;

        vertices[3] = 1.0f;
        vertices[4] = 0.0f;

        vertices[5] = width / 2.f;
        vertices[6] = -1.0f;
        vertices[7] = 0.f;

        vertices[8] = 1.0f;
        vertices[9] = 1.f;

        vertices[10] = -(width / 2.f);
        vertices[11] = -1.0f;
        vertices[12] = 0.f;

        vertices[13] = 0.0f;
        vertices[14] = 1.0f;

        vertices[15] = -(width / 2.f);
        vertices[16] = 1.0f;
        vertices[17] = 0.f;

        vertices[18] = 0.0f;
        vertices[19] = 0.0f;
    } else {
        float height = 2 * ((float)pixel_h / (float)pixel_w);
        vertices[0] = 1.0f;
        vertices[1] = height / 2.f;
        vertices[2] = 0.f;

        vertices[3] = 1.0f;
        vertices[4] = 0.0f;

        vertices[5] = 1.0f;
        vertices[6] = -(height / 2.f);
        vertices[7] = 0.f;

        vertices[8] = 1.0f;
        vertices[9] = 1.0f;

        vertices[10] = -1.0f;
        vertices[11] = -(height / 2.f);
        vertices[12] = 0.f;

        vertices[13] = 0.0f;
        vertices[14] = 1.0f;

        vertices[15] = -1.0f;
        vertices[16] = height / 2.f;
        vertices[17] = 0.f;

        vertices[18] = 0.0f;
        vertices[19] = 0.0f;
    }

    return vertices;
}

static void transform_world_coordinates_to_window_coordinates(float* vertex_coords, unsigned int window_width, unsigned int window_height, unsigned int elements_count, unsigned int element_stride, int vertex_pointer_offset, float zoom) {
    // Calculate the aspect ratios first.
    float window_aspect_ratio = (float)window_width/(float)window_height;
    float inverse_window_aspect_ratio = (float)window_height/(float)window_width;

    unsigned int i;
    for (i = 0; i < elements_count; i++) {
        if (window_aspect_ratio > 1.0f) {
            vertex_coords[i * element_stride + vertex_pointer_offset] = vertex_coords[i * element_stride + vertex_pointer_offset] * inverse_window_aspect_ratio * zoom;
            vertex_coords[i * element_stride + 1 + vertex_pointer_offset] = vertex_coords[i * element_stride + 1 + vertex_pointer_offset] * zoom;
        } else {
            vertex_coords[i * element_stride + 1 + vertex_pointer_offset] = vertex_coords[i * element_stride + 1 + vertex_pointer_offset] * window_aspect_ratio * zoom;
            vertex_coords[i * element_stride + vertex_pointer_offset] = vertex_coords[i * element_stride + vertex_pointer_offset] * zoom;
        }
    }
}

canvas_t* create_canvas_object(unsigned int pixel_w, unsigned int pixel_h) {
    canvas_t* canvas = malloc(sizeof(canvas_t));
    canvas->rectangle_vertices = generate_vertex_coords(pixel_w, pixel_h);

    unsigned int* indices = malloc(sizeof(unsigned int) * 6);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 3;
    indices[3] = 1;
    indices[4] = 2;
    indices[5] = 3;

    canvas->rectangle_indices = indices;
    canvas->pixel_w = pixel_w;
    canvas->pixel_h = pixel_h;

    glGenVertexArrays(1, &canvas->vertex_array_object);
    glBindVertexArray(canvas->vertex_array_object);

    glGenBuffers(1, &canvas->vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, canvas->vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * (3 + 2), canvas->rectangle_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &canvas->element_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    return canvas;
}

void draw_canvas_object(canvas_t* canvas) {
    glBindVertexArray(canvas->vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, canvas->vertex_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->element_buffer_object);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}

// TODO: Properly delete the buffers.
void destroy_canvas_object(canvas_t* canvas) {
    free(canvas->rectangle_vertices);
    free(canvas->rectangle_indices);
    free(canvas);
}

void mutate_canvas_object(canvas_t* canvas, unsigned int new_pixel_w, unsigned int new_pixel_h, unsigned int new_window_w, unsigned int new_window_h, float zoom) {
    canvas->pixel_w = new_pixel_w;
    canvas->pixel_h = new_pixel_h;
    free(canvas->rectangle_vertices);
    canvas->rectangle_vertices = generate_vertex_coords(new_pixel_w, new_pixel_h);
    transform_world_coordinates_to_window_coordinates(canvas->rectangle_vertices, new_window_w, new_window_h, 4, 5, 0, zoom);
    glBindVertexArray(canvas->vertex_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, canvas->vertex_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->element_buffer_object);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * 4 * (3 + 2), canvas->rectangle_vertices);

    glBindVertexArray(0);
}

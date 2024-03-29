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
#ifndef IMAGE_H
#define IMAGE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gcode_serializer.h"

typedef struct image_t {
    unsigned char* image_data;
    unsigned int width;
    unsigned int height;

    unsigned int real_width;
    unsigned int real_height;

    GLuint texture;
    char* gcode_data;
    unsigned int gcode_data_length;
} image_t;

image_t* create_image(char* path);
image_t* dither_image(image_t* image);
void bind_image(image_t* image);
void destroy_image(image_t* image);
void save_gcode();
void resize_image(image_t* image, unsigned int max_width, unsigned int max_height);

#endif

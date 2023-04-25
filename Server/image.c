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
#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

image_t* create_image(char* path) {
    image_t* image = malloc(sizeof(image_t));

    int width;
    int height;
    int num_channels;

    image->image_data = stbi_load(path, &width, &height, &num_channels, 0);

    if (image->image_data == NULL) {
        fprintf(stderr, "Error: Failed to load an image.");
        return NULL;
    }

    glGenTextures(1, &image->texture);
    glBindTexture(GL_TEXTURE_2D, image->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->image_data);

    image->width = width;
    image->height = height;
    return image;
}

void set_pixel(image_t* image, int j, int i, unsigned char value, int offset) {
    if (j < 0 || j >= image->height || i < 0 || i >= image->width) {
        return;
    }
    image->image_data[(image->width * j + i)*3 + offset] = value;
}

unsigned char get_pixel(image_t* image, int j, int i, int offset) {
    if (j < 0 || j >= image->height || i < 0 || i >= image->width) {
        return 0;
    }
    return image->image_data[(image->width * j + i)*3 + offset];
}


void set_all_pixel(image_t* image, int j, int i, unsigned int value, unsigned char inc) {
    if (j < 0 || j >= image->height || i < 0 || i >= image->width) {
        return;
    }
    image->image_data[(image->width * j + i)*3 + 0] = (unsigned char)(inc * (unsigned int)image->image_data[(image->width * j + i)*3 + 0] + (unsigned int)value);
    image->image_data[(image->width * j + i)*3 + 1] = (unsigned char)(inc * (unsigned int)image->image_data[(image->width * j + i)*3 + 1] + (unsigned int)value);
    image->image_data[(image->width * j + i)*3 + 2] = (unsigned char)(inc * (unsigned int)image->image_data[(image->width * j + i)*3 + 2] + (unsigned int)value);
}

int get_error_diffusion(unsigned char val) {
    unsigned char l_dist = 255 - val;
    unsigned char h_dist = val;

    if (l_dist > h_dist) {
        return val;
    } else {
        return -1*(255 - val);
    }
}

unsigned char get_new_pixel(unsigned char val) {
    unsigned char l_dist = 255 - val;
    unsigned char h_dist = val;

    if (l_dist > h_dist) {
        return 0;
    } else {
        return 255;
    }
}

image_t* dither_image(image_t* image) {
    for(int j = 0; j < image->height; j++) {
        for(int i = 0; i < image->width; i++) {
            // First convert it to grayscale.
            unsigned char val = (get_pixel(image, j, i, 0) + get_pixel(image, j, i, 1) + get_pixel(image, j, i, 2))/3;
            set_pixel(image, j, i, val, 0);
            set_pixel(image, j, i, val, 1);
            set_pixel(image, j, i, val, 2);
        }
    }

    for(int j = 0; j < image->height; j++) {
        for(int i = 0; i < image->width; i++) {
            unsigned char val = get_pixel(image, j, i, 0);

            int error = get_error_diffusion(val);
            unsigned char new_val = get_new_pixel(val);

            set_all_pixel(image, j, i, new_val, 0);
            set_all_pixel(image, j, i+1, (((unsigned int)7 * (unsigned int)error) / 16), 1);
            set_all_pixel(image, j+1, i+1,(((unsigned int)1 * (unsigned int)error) / 16), 1);
            set_all_pixel(image, j+1, i, (((unsigned int)5 * (unsigned int)error) / 16), 1);
            set_all_pixel(image, j+1, i-1,(((unsigned int)3 * (unsigned int)error) / 16), 1);
        }
    }

    image_t* new_image = malloc(sizeof(image_t));
    new_image->image_data = image->image_data;
    new_image->width = image->width;
    new_image->height = image->height;

    glGenTextures(1, &new_image->texture);
    glBindTexture(GL_TEXTURE_2D, new_image->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, new_image->width, new_image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, new_image->image_data);

    return new_image;
}

void bind_image(image_t* image) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->texture);
}

void destroy_image(image_t* image) {
    glDeleteTextures(1, &image->texture);
    stbi_image_free(image->image_data);
    free(image);
}

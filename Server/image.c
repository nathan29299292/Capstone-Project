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

    image->image_data = stbi_load(path, &width, &height, &num_channels, STBI_rgb);

    if (image->image_data == NULL) {
        free(image);
        fprintf(stderr, "Error: Failed to load an image.\n");
        return NULL;
    }

    glGenTextures(1, &image->texture);
    glBindTexture(GL_TEXTURE_2D, image->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->image_data);

    image->width = width;
    image->height = height;
    image->real_width = width;
    image->real_height = height;
    image->gcode_data = NULL;
    return image;
}

void set_pixel(image_t* image, int j, int i, unsigned char value, int offset) {
    if (j < 0 || j >= (int)image->height || i < 0 || i >= (int)image->width) {
        return;
    }
    image->image_data[((int)image->width * j + i)*3 + offset] = value;
}

unsigned char get_pixel(image_t* image, int j, int i, int offset) {
    if (j < 0 || j >= (int)image->height || i < 0 || i >= (int)image->width) {
        return 0;
    }
    return image->image_data[((int)image->width * j + i)*3 + offset];
}


void set_all_pixel(image_t* image, int j, int i, unsigned int value, unsigned char inc) {
    if (j < 0 || j >= (int)image->height || i < 0 || i >= (int)image->width) {
        return;
    }

    image->image_data[((int)image->width * j + i) * 3 + 0] = (unsigned char)(fmin(255, inc * (unsigned int)image->image_data[((int)image->width * j + i) * 3 + 0] + (unsigned int)value));
    image->image_data[((int)image->width * j + i) * 3 + 1] = (unsigned char)(fmin(255, inc * (unsigned int)image->image_data[((int)image->width * j + i) * 3 + 1] + (unsigned int)value));
    image->image_data[((int)image->width * j + i) * 3 + 2] = (unsigned char)(fmin(255, inc * (unsigned int)image->image_data[((int)image->width * j + i) * 3 + 2] + (unsigned int)value));
}


unsigned char get_new_pixel(unsigned char val) {
    return (val/64)*64;
}

int get_error_diffusion(unsigned char val) {
    unsigned char quantized_value = get_new_pixel(val);
    return abs((int)val - (int)quantized_value);
}

/* Try to maintain the same aspect ratio. */
void resize_image(image_t* image, unsigned int max_width, unsigned int max_height)  {
    unsigned int width = image->width;
    unsigned int height = image->height;
    unsigned int new_height;
    unsigned int new_width;
    unsigned int padded_height;
    unsigned int padded_width;
    unsigned char* new_image;

    double aspect_ratio = ((double)width)/((double)height);
    if (aspect_ratio > 1.0) {
        new_height = ceil(((double)max_width * (double)height) / (double)width);
        new_width = max_width;
    } else {
        new_height = max_height;
        new_width = ceil(((double)max_height * (double)width) / (double)height);
    }

    // If the image is less than 1024 x 1024, then we need to round it to the nearest.
    // 2^x, because OpenGL is stupid.
    if (new_height < 1024 || new_width < 1024) {
        padded_width = 1;
        while(padded_width < new_width) {
            padded_width <<= 1;
        }

        padded_height = 1;
        while(padded_height < new_height) {
            padded_height <<= 1;
        }
        new_image = (unsigned char*)calloc(padded_width * padded_height * 3, 1);

        image->real_height = padded_height;
        image->real_width = padded_width;
    } else {
        padded_width = new_width;
        padded_height = new_height;
        new_image = (unsigned char*)malloc(new_width * new_height * 3);
    }

    double x_step = ((double)width) / ((double)new_width);
    double y_step = ((double)height) / ((double)new_height);

    for (unsigned int i = 0; i < padded_height; i++) {
        for (unsigned int j = 0; j < padded_width; j++) {
            if (j < new_width && i < new_height) {
                unsigned int x = round((x_step)*(double)j); // Nearest sampling.
                unsigned int y = round((y_step)*(double)i); // Nearest sampling.

                new_image[(i * new_width + j)*3] = (unsigned int)fmin(1.65 * ((double)image->image_data[(y * width + x)*3]), 255.0);
                new_image[(i * new_width + j)*3+1] = (unsigned int)fmin(1.65 * ((double)image->image_data[(y * width + x)*3+1]), 255.0);
                new_image[(i * new_width + j)*3+2] = (unsigned int)fmin(1.65 * ((double)image->image_data[(y * width + x)*3+2]), 255.0);
            }
        }
    }

    // Free old image.
    stbi_image_free(image->image_data);
    image->image_data = new_image;
    image->width = new_width;
    image->height = new_height;

    image->real_width = padded_width;
    image->real_height = padded_height;

    glBindTexture(GL_TEXTURE_2D, image->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, new_image);
}


image_t* dither_image(image_t* image) {
    init_stack();
    toggle_distance_modes();

    for(int j = 0; j < (int)image->height; j++) {
        for(int i = 0; i < (int)image->width; i++) {
            // First convert it to grayscale.
            unsigned char val = (get_pixel(image, j, i, 0) + get_pixel(image, j, i, 1) + get_pixel(image, j, i, 2))/3;
            set_pixel(image, j, i, val, 0);
            set_pixel(image, j, i, val, 1);
            set_pixel(image, j, i, val, 2);
        }
    }
    point_t current_point = {0.0,0.0, Z_BREADTH};

    for(int j = 0; j < (int)image->height; j++) {
        for(int i = 0; i < (int)image->width; i++) {
            unsigned char val = get_pixel(image, j, i, 0);

            int error = get_error_diffusion(val);
            unsigned char new_val = get_new_pixel(val);

            if (new_val/64 == 3) {

            }else if (new_val == 0) {
                point_t next_point = {i * 1.0, j * 1.0, Z_BREADTH};
                move_and_burn(current_point, next_point, 2, 0);
                current_point = next_point;
            } else if (new_val/64 == 1) {
                point_t next_point = {i * 1.0, j * 1.0, Z_BREADTH};
                move_and_burn(current_point, next_point, 1, 1);
                current_point = next_point;
            } else if (new_val/64 == 2) {
                point_t next_point = {i * 1.0, j * 1.0, Z_BREADTH};
                move_and_burn(current_point, next_point, 0, 2);
                current_point = next_point;
            }

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
    new_image->gcode_data = extract_data();
    new_image->gcode_data_length = extract_data_size();
    new_image->real_height = image->height;
    new_image->real_width = image->width;

    glGenTextures(1, &new_image->texture);
    glBindTexture(GL_TEXTURE_2D, new_image->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, new_image->real_width, new_image->real_height, 0, GL_RGB, GL_UNSIGNED_BYTE, new_image->image_data);

    return new_image;
}

void bind_image(image_t* image) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->texture);
}

void destroy_image(image_t* image) {
    glDeleteTextures(1, &image->texture);
    free(image);
}

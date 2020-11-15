#ifndef GRAPHICS_GRAPHICSLIBRARY_H
#define GRAPHICS_GRAPHICSLIBRARY_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rgb{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} rgb_t;

typedef struct rgb_image {
    unsigned int image_size_x;
    unsigned int image_size_y;
    unsigned char *image_data;
} rgb_image_t;

void hsv_to_rgb(int hue, int min, int max, rgb_t *p);

void write_rgb_file(char *filename,rgb_image_t *image);
void write_rgb_pipe(rgb_image_t *image);


rgb_image_t * read_ppm_rgb_file(char *file);
rgb_image_t * read_ppm_rgb_pipe();
rgb_image_t *read_ppm_rgb_mandy();

void free_rgb_image(rgb_image_t *image);
rgb_image_t * get_ppm(FILE *pf);

#endif //GRAPHICS_GRAPHICSLIBRARY_H

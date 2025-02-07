#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t* data;
    uint32_t row_size;
} image_t;

image_t* load_bmp(FILE* file);
image_t* crop(image_t* src, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
image_t* rotate(image_t* src);
bool save_bmp(FILE* file, image_t* img);
void image_free(image_t* img);

#endif
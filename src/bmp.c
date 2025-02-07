#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

image_t* load_bmp(FILE* file) {
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;

    if (fread(&file_header, sizeof(file_header), 1, file) != 1) return NULL;
    if (fread(&info_header, sizeof(info_header), 1, file) != 1) return NULL;

    if (file_header.bfType != 0x4D42 || info_header.biSize != 40 ||
        info_header.biPlanes != 1 || info_header.biBitCount != 24 ||
        info_header.biCompression != 0 || info_header.biHeight <= 0 ||
        file_header.bfOffBits != 54) {
        return NULL;
    }

    uint32_t width = info_header.biWidth;
    uint32_t height = info_header.biHeight;
    uint32_t row_size = ((width * 3 + 3) / 4) * 4;

    uint8_t* data = malloc(row_size * height);
    if (!data) return NULL;

    fseek(file, file_header.bfOffBits, SEEK_SET);
    if (fread(data, row_size * height, 1, file) != 1) {
        free(data);
        return NULL;
    }

    image_t* img = malloc(sizeof(image_t));
    img->width = width;
    img->height = height;
    img->row_size = row_size;
    img->data = data;

    return img;
}

image_t* crop(image_t* src, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    image_t* cropped = malloc(sizeof(image_t));
    if (!cropped) return NULL;

    cropped->width = w;
    cropped->height = h;
    cropped->row_size = ((w * 3 + 3) / 4) * 4;
    cropped->data = malloc(cropped->row_size * h);
    if (!cropped->data) {
        free(cropped);
        return NULL;
    }

    for (uint32_t i = 0; i < h; ++i) {
        uint32_t src_row = src->height - 1 - (y + i);
        uint32_t dst_row = h - 1 - i;
        uint8_t* src_line = src->data + src_row * src->row_size + x * 3;
        uint8_t* dst_line = cropped->data + dst_row * cropped->row_size;
        memcpy(dst_line, src_line, w * 3);
        memset(dst_line + w * 3, 0, cropped->row_size - w * 3);
    }

    return cropped;
}


image_t* rotate(image_t* src) {
    uint32_t new_width = src->height;
    uint32_t new_height = src->width;
    image_t* rotated = malloc(sizeof(image_t));
    if (!rotated) return NULL;

    rotated->width = new_width;
    rotated->height = new_height;
    rotated->row_size = ((new_width * 3 + 3) / 4) * 4;
    rotated->data = malloc(rotated->row_size * new_height);
    if (!rotated->data) {
        free(rotated);
        return NULL;
    }

    for (uint32_t i = 0; i < src->width; ++i) {        
        for (uint32_t j = 0; j < src->height; ++j) {  
            uint32_t dst_x = j;
            uint32_t dst_y = new_height - 1 - i; 

            uint8_t* src_pixel = src->data + j * src->row_size + i * 3;
            uint8_t* dst_pixel = rotated->data + dst_y * rotated->row_size + dst_x * 3;


            memcpy(dst_pixel, src_pixel, 3);
        }
    }

    for (uint32_t y = 0; y < new_height; ++y) {
        uint8_t* dst_line = rotated->data + y * rotated->row_size;
        memset(dst_line + new_width * 3, 0, rotated->row_size - new_width * 3);
    }

    return rotated;
}



bool save_bmp(FILE* file, image_t* img) {
    BITMAPFILEHEADER file_header = {0x4D42, 54 + img->row_size * img->height, 0, 0, 54};
    BITMAPINFOHEADER info_header = {40, img->width, img->height, 1, 24, 0, img->row_size * img->height, 0, 0, 0, 0};

    if (fwrite(&file_header, sizeof(file_header), 1, file) != 1)
        return false;
    if (fwrite(&info_header, sizeof(info_header), 1, file) != 1)
        return false;

    if (fwrite(img->data, img->row_size, img->height, file) != img->height)
        return false;

    return true;
}




void image_free(image_t* img) {
    if (img) {
        free(img->data);
        free(img);
    }
}
#ifndef STEGO_H
#define STEGO_H

#include "bmp.h"

void stego_insert(image_t *image, const char *key_path, const char *message_path);
void stego_extract(const image_t *image, const char *key_path, const char *output_path);

#endif
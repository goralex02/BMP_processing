#include "stego.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static uint8_t char_to_code(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c == ' ') return 26;
    if (c == '.') return 27;
    if (c == ',') return 28;
    return 29;
}

static char code_to_char(uint8_t code) {
    if (code < 26) return 'A' + code;
    if (code == 26) return ' ';
    if (code == 27) return '.';
    if (code == 28) return ',';
    return '?';
}

static uint8_t* get_pixel(const image_t *image, uint32_t x, uint32_t y) {
    uint32_t row = image->height - 1 - y;
    return image->data + (row * image->row_size) + (x * 3);
}

void stego_insert(image_t *image, const char *key_path, const char *message_path) {
    FILE *message_file = fopen(message_path, "r");
    FILE *key_file = fopen(key_path, "r");
    if (!message_file || !key_file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char ch;
    while (fscanf(message_file, "%c", &ch) == 1) {
        if (ch == '\n')
            break;
        uint8_t value = char_to_code(ch);
        
        for (int i = 0; i < 5; ++i) {
            uint32_t x, y;
            char channel;
            if (fscanf(key_file, "%u %u %c", &x, &y, &channel) != 3)
                break;
            uint8_t *pixel = get_pixel(image, x, y);
            uint8_t *component = NULL;
            if (channel == 'R')
                component = &pixel[2];
            else if (channel == 'G')
                component = &pixel[1];
            else if (channel == 'B')
                component = &pixel[0];
            else
                continue;

            *component = (*component & 0xFE) | (value & 0x1);
            value >>= 1;
        }
    }

    fclose(message_file);
    fclose(key_file);
}

void stego_extract(const image_t *image, const char *key_path, const char *output_path) {
    FILE *output_file = fopen(output_path, "w");
    FILE *key_file = fopen(key_path, "r");
    if (!output_file || !key_file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    uint32_t x, y;
    char channel;
    uint8_t value = 0;
    int bit_count = 0;
    
    while (fscanf(key_file, "%u %u %c", &x, &y, &channel) == 3) {
        if (bit_count == 5) {
            fprintf(output_file, "%c", code_to_char(value));
            value = 0;
            bit_count = 0;
        }
        
        const uint8_t *pixel = get_pixel(image, x, y);
        uint8_t color_value = 0;
        if (channel == 'R')
            color_value = pixel[2];
        else if (channel == 'G')
            color_value = pixel[1];
        else if (channel == 'B')
            color_value = pixel[0];
        
        value |= (color_value & 0x1) << bit_count;
        bit_count++;
    }
    
    if (bit_count == 5) {
        fprintf(output_file, "%c", code_to_char(value));
    }
    
    fprintf(output_file, "\n");
    fclose(output_file);
    fclose(key_file);
}

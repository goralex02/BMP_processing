#include "bmp.h"
#include "stego.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> ...\n", argv[0]);
        return 1;
    }
    const char* cmd = argv[1];
    if (strcmp(cmd, "crop-rotate") == 0) {
        if (argc != 8) {
            fprintf(stderr, "Usage: %s crop-rotate <in-bmp> <out-bmp> <x> <y> <w> <h>\n", argv[0]);
            return 1;
        }
        uint32_t x = strtoul(argv[4], NULL, 10);
        uint32_t y = strtoul(argv[5], NULL, 10);
        uint32_t w = strtoul(argv[6], NULL, 10);
        uint32_t h = strtoul(argv[7], NULL, 10);
        FILE* in = fopen(argv[2], "rb");
        if (!in) {
            perror("Failed to open input file");
            return 1;
        }
        image_t* img = load_bmp(in);
        fclose(in);
        if (!img) {
            fprintf(stderr, "Invalid BMP file\n");
            return 1;
        }
        if (x >= img->width || y >= img->height || x + w > img->width || y + h > img->height) {
            fprintf(stderr, "Invalid crop parameters\n");
            image_free(img);
            return 1;
        }
        image_t* cropped = crop(img, x, y, w, h);
        image_free(img);
        if (!cropped) {
            fprintf(stderr, "Crop failed\n");
            return 1;
        }
        image_t* rotated = rotate(cropped);
        image_free(cropped);
        if (!rotated) {
            fprintf(stderr, "Rotate failed\n");
            return 1;
        }
        FILE* out = fopen(argv[3], "wb");
        if (!out) {
            perror("Failed to open output file");
            image_free(rotated);
            return 1;
        }
        if (!save_bmp(out, rotated)) {
            fprintf(stderr, "Save failed\n");
            image_free(rotated);
            fclose(out);
            return 1;
        }
        image_free(rotated);
        fclose(out);
    } else if (strcmp(cmd, "insert") == 0) {
        if (argc != 6) {
            fprintf(stderr, "Usage: %s insert <in-bmp> <out-bmp> <key-txt> <msg-txt>\n", argv[0]);
            return 1;
        }
        FILE *in = fopen(argv[2], "rb");
        if (!in) {
            perror("Failed to open input BMP file");
            return 1;
        }
        image_t *img = load_bmp(in);
        fclose(in);
        if (!img) {
            fprintf(stderr, "Invalid BMP file\n");
            return 1;
        }
        stego_insert(img, argv[4], argv[5]);
        FILE *out = fopen(argv[3], "wb");
        if (!out) {
            perror("Failed to open output BMP file");
            image_free(img);
            return 1;
        }
        if (!save_bmp(out, img)) {
            fprintf(stderr, "Save failed\n");
            fclose(out);
            image_free(img);
            return 1;
        }
        fclose(out);
        image_free(img);
    } else if (strcmp(cmd, "extract") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: %s extract <in-bmp> <key-txt> <msg-txt>\n", argv[0]);
            return 1;
        }
        FILE *in = fopen(argv[2], "rb");
        if (!in) {
            perror("Failed to open input BMP file");
            return 1;
        }
        image_t *img = load_bmp(in);
        fclose(in);
        if (!img) {
            fprintf(stderr, "Invalid BMP file\n");
            return 1;
        }
        stego_extract(img, argv[3], argv[4]);
        image_free(img);
    } else {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        return 1;
    }
    return 0;
}
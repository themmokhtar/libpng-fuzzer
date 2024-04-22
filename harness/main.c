/*
 * A simple libpng example program
 * http://zarb.org/~gc/html/libpng.html
 *
 * Modified by Yoshimasa Niwa to make it much simpler
 * and support all defined color_type.
 *
 * To build, use the next instruction on OS X.
 * $ brew install libpng
 * $ clang -lz -lpng16 libpng_test.c
 *
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <png.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

static int width, height;
static png_byte color_type;
static png_byte bit_depth;
static png_bytep *row_pointers = NULL;

#define fail(msg, label)           \
    {                              \
        printf("FAIL: %s\n", msg); \
        goto fail_##label;         \
    }

int read_png_file(const uint8_t *data, size_t size)
{
    // FILE *fp = fopen(filename, "rb");
    FILE *fp = fmemopen((uint8_t *)data, size, "rb");
    if (!fp)
        fail("fopen()", none);

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        fail("png_create_read_struct()", fp);

    png_infop info = png_create_info_struct(png);
    if (!info)
        fail("png_create_info_struct()", read_struct);

    if (setjmp(png_jmpbuf(png)))
        fail("setjmp(png_jmpbuf())", info_struct);

    png_init_io(png, fp);

    png_read_info(png, info);

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    if (row_pointers)
        fail("row_pointers already allocated", info_struct);

    row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, row_pointers);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);

    return 0;

fail_info_struct:
    png_destroy_read_struct(&png, &info, NULL);
    goto fail_fp;

fail_read_struct:
    png_destroy_read_struct(&png, NULL, NULL);

fail_fp:
    fclose(fp);

fail_none:
    return 1;
}

// void write_png_file(char *filename)
// {
//     FILE *fp = fopen(filename, "wb");
//     if (!fp)
//         fail("fopen()");

//     png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//     if (!png)
//         fail("png_create_write_struct()");

//     png_infop info = png_create_info_struct(png);
//     if (!info)
//         fail("png_create_info_struct()");

//     if (setjmp(png_jmpbuf(png)))
//         fail("setjmp(png_jmpbuf())");

//     png_init_io(png, fp);

//     // Output is 8bit depth, RGBA format.
//     png_set_IHDR(
//         png,
//         info,
//         width, height,
//         8,
//         PNG_COLOR_TYPE_RGBA,
//         PNG_INTERLACE_NONE,
//         PNG_COMPRESSION_TYPE_DEFAULT,
//         PNG_FILTER_TYPE_DEFAULT);
//     png_write_info(png, info);

//     // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
//     // Use png_set_filler().
//     // png_set_filler(png, 0, PNG_FILLER_AFTER);

//     if (!row_pointers)
//         fail("raw_pointers not allocated");

//     png_write_image(png, row_pointers);
//     png_write_end(png, NULL);

//     for (int y = 0; y < height; y++)
//     {
//         free(row_pointers[y]);
//     }
//     free(row_pointers);

//     fclose(fp);

//     png_destroy_write_struct(&png, &info);
// }

// void process_png_file()
// {
//     for (int y = 0; y < height; y++)
//     {
//         png_bytep row = row_pointers[y];
//         for (int x = 0; x < width; x++)
//         {
//             png_bytep px = &(row[x * 4]);
//             // Do something awesome for each pixel here...
//             // printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);

//             // Add filter to the image
//             if (x > 0)
//             {
//                 px[0] = (px[0] + row[(x - 1) * 4]) / 2;
//                 px[1] = (px[1] + row[(x - 1) * 4 + 1]) / 2;
//                 px[2] = (px[2] + row[(x - 1) * 4 + 2]) / 2;
//             }
//         }
//     }
// }

// int main(int argc, char *argv[])
// {
//     if (argc != 3)
//         fail();

//     read_png_file("./test_input.png");
//     process_png_file();
//     write_png_file("./test_output.png");

//     return 0;
// }

extern int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    width = height = 0;
    color_type = bit_depth = 0;
    row_pointers = NULL;

    // printf("data: %p\n", data);
    // printf("size: %ld\n", size);

    if (size < 8)
        return 0;

    read_png_file(data, size);

    for (int y = 0; y < height; y++)
    {
        free(row_pointers[y]);
    }
    free(row_pointers);
    // process_png_file();
    // write_png_file("./test_output.png");
    return 0; // Non-zero return values are reserved for future use.
}

#ifdef COVERAGE_TESTER
// const char *program_name = 0;
// void auto_gdb(int signum)
// {

//     char buf[256];
//     snprintf(buf, 255, "sudo gdb %s -p %d", program_name, getpid());
//     system(buf);
// }

// void setup_auto_gdb(const char *arg_0)
// {
//     program_name = arg_0;
//     signal(SIGSEGV, &auto_gdb);
// }

int main(int argc, char *argv[])
{
    // while ((1))
    // {
    //     /* code */
    // }
    // printf("Starting\n");
    // fflush(stdout);
    // setup_auto_gdb(argv[0]);

    if (argc != 2)
    {
        printf("Usage: %s <png_file_in>\n", argv[0]);
        return 1;
    }

    // Open the file
    FILE *fp = fopen(argv[1], "rb");
    if (!fp)
        fail("fopen", none);

    // Get the file size
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    if (size == -1)
        fail("ftell", fp);
    fseek(fp, 0, SEEK_SET);

    // Memory map the file
    uint8_t *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
    if (data == MAP_FAILED)
        fail("mmap", fp);

    // Call LLVMFuzzerTestOneInput
    LLVMFuzzerTestOneInput(data, size);

    // Cleanup
    munmap(data, size);
    // while(1);

fail_fp:
    fclose(fp);

fail_none:
    return 0;
}

#endif

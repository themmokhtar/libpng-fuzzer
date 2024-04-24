// /*
//  * A simple libpng example program
//  * http://zarb.org/~gc/html/libpng.html
//  *
//  * Modified by Yoshimasa Niwa to make it much simpler
//  * and support all defined color_type.
//  *
//  * To build, use the next instruction on OS X.
//  * $ brew install libpng
//  * $ clang -lz -lpng16 libpng_test.c
//  *
//  * Copyright 2002-2010 Guillaume Cottenceau.
//  *
//  * This software may be freely redistributed under the terms
//  * of the X11 license.
//  *
//  */

// #include <stdlib.h>
// #include <signal.h>
// #include <stdio.h>
// #include <png.h>
// #include <stdint.h>
// #include <unistd.h>
// #include <sys/mman.h>

// #include "main.h"

// static int width, height;
// static png_byte color_type;
// static png_byte bit_depth;
// static png_bytep *row_pointers = NULL;

// #define fail(msg, label)                                                                  \
//     {                                                                                     \
//         printf("FAIL on %s:%llu: %s\n", (__func__), (unsigned long long)(__LINE__), msg); \
//         goto fail_##label;                                                                \
//     }
    
// // int read_png_file(const uint8_t *data, size_t size)
// // {
// //     FILE *fp = fmemopen((uint8_t *)data, size, "rb");

// int read_png_file(char *filename)
// {
//     FILE *fp = fopen(filename, "rb");
//     if (!fp)
//         fail("fopen()", none);

//     png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//     if (!png)
//         fail("png_create_read_struct()", fp);

//     png_infop info = png_create_info_struct(png);
//     if (!info)
//         fail("png_create_info_struct()", read_struct);

//     if (setjmp(png_jmpbuf(png)))
//         fail("setjmp(png_jmpbuf())", info_struct);

//     png_init_io(png, fp);

//     // png_set_gamma(png, 1, PNG_DEFAULT_sRGB);
//     png_read_info(png, info);

//     width = png_get_image_width(png, info);
//     height = png_get_image_height(png, info);

//     printf("width=%d, height=%d\n", width, height);

//     if (row_pointers)
//         fail("row_pointers already allocated", info_struct);
//     row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
//     for (int y = 0; y < height; y++)
//         row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png, info));
//     printf("row_pointers=%p\n", row_pointers);
//     printf("png_get_rowbytes=%zu\n", png_get_rowbytes(png, info));

//     int passes = png_set_interlace_handling(png);
//     png_read_update_info(png, info);

// fail_info_struct:
//     png_destroy_read_struct(&png, &info, NULL);
//     goto fail_fp;

// fail_read_struct:
//     png_destroy_read_struct(&png, NULL, NULL);

// fail_fp:
//     fclose(fp);

// fail_none:
//     return 1;
// }


// void process_png_file()
// {
//     printf("Processing %d x %d image with %d channels\n", width, height, color_type);
//     for (int y = 0; y < height; y++)
//     {
//         png_bytep row = row_pointers[y];
//         for (int x = 0; x < width; x++)
//         {
//             png_bytep px = &(row[x * 4]);
//             // Do something awesome for each pixel here...
//             // printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);

//             // Calculate the grayscale value
//             png_byte newpx = (px[0] + px[1] + px[2]) / 3;

//             // Threshold the grayscale value (100% contrast)
//             if (newpx < 128)
//                 newpx = 0;
//             else
//                 newpx = 255;

//             // Set the new pixel value
//             px[0] = px[1] = px[2] = newpx;
//         }
//     }
// }

// void process_image(char *input_filename, char *output_filename)
// {
//     // Peform our processing
//     read_png_file(input_filename);
//     process_png_file();
// }

// int main(int argc, char *argv[])
// {
//     // #if __has_feature(undefined_behavior_sanitizer) && __has_feature(address_sanitizer)
//     //     printf("GAMER!!!\n");
//     // #else
//     //     printf("NOT GAMER!!!\n");
//     // #endif
//     //  while(1) {};

//     char *input = NULL, *output = NULL;

//     if (argc == 2)
//     {
//         input = argv[1];
//         output = "/dev/null";
//     }
//     else if (argc == 3)
//     {
//         input = argv[1];
//         output = argv[2];
//     }
//     else
//     {
//         printf("Usage: %s <png_file_in> <png_file_out>\n", argv[0]);
//         return 1;
//     }

//     printf("Processing %s -> %s\n", input, output);

//     process_image(input, output);

//     return 0;
// }

// // // libpng_read_fuzzer.cc
// // // Copyright 2017-2018 Glenn Randers-Pehrson
// // // Copyright 2015 The Chromium Authors. All rights reserved.
// // // Use of this source code is governed by a BSD-style license that may
// // // be found in the LICENSE file https://cs.chromium.org/chromium/src/LICENSE

// // // The modifications in 2017 by Glenn Randers-Pehrson include
// // // 1. addition of a PNG_CLEANUP macro,
// // // 2. setting the option to ignore ADLER32 checksums,
// // // 3. adding "#include <string.h>" which is needed on some platforms
// // //    to provide memcpy().
// // // 4. adding read_end_info() and creating an end_info structure.
// // // 5. adding calls to png_set_*() transforms commonly used by browsers.

// // #include <stddef.h>
// // #include <stdint.h>
// // #include <stdlib.h>
// // #include <string.h>

// // // #include <vector>

// // #define PNG_INTERNAL
// // #include "png.h"

// // #define PNG_CLEANUP                                                                  \
// //     {                                                                                \
// //         destroy_PngObjectHandler(&png_handler);                                      \
// //         if (png_handler.png_ptr)                                                     \
// //         {                                                                            \
// //             if (png_handler.row_ptr)                                                 \
// //                 png_free(png_handler.png_ptr, png_handler.row_ptr);                  \
// //             if (png_handler.end_info_ptr)                                            \
// //                 png_destroy_read_struct(&png_handler.png_ptr, &png_handler.info_ptr, \
// //                                         &png_handler.end_info_ptr);                  \
// //             else if (png_handler.info_ptr)                                           \
// //                 png_destroy_read_struct(&png_handler.png_ptr, &png_handler.info_ptr, \
// //                                         0);                                          \
// //             else                                                                     \
// //                 png_destroy_read_struct(&png_handler.png_ptr, 0, 0);                 \
// //             png_handler.png_ptr = 0;                                                 \
// //             png_handler.row_ptr = 0;                                                 \
// //             png_handler.info_ptr = 0;                                                \
// //             png_handler.end_info_ptr = 0;                                            \
// //         }                                                                            \
// //     }
// // struct BufState
// // {
// //     const uint8_t *data;
// //     size_t bytes_left;
// // };

// // struct PngObjectHandler
// // {
// //     png_infop info_ptr;
// //     png_structp png_ptr;
// //     png_infop end_info_ptr;
// //     png_voidp row_ptr;
// //     struct BufState *buf_state;
// // };

// // void destroy_PngObjectHandler(struct PngObjectHandler *png_handler)
// // {
// //     if (png_handler->row_ptr)
// //         png_free(png_handler->png_ptr, png_handler->row_ptr);
// //     if (png_handler->end_info_ptr)
// //         png_destroy_read_struct(&png_handler->png_ptr, &png_handler->info_ptr, &png_handler->end_info_ptr);
// //     else if (png_handler->info_ptr)
// //         png_destroy_read_struct(&png_handler->png_ptr, &png_handler->info_ptr, 0);
// //     else
// //         png_destroy_read_struct(&png_handler->png_ptr, 0, 0);

// //     free(png_handler->buf_state);
// // }

// // void user_read_data(png_structp png_ptr, png_bytep data, size_t length)
// // {
// //     struct BufState *buf_state = (struct BufState *)png_get_io_ptr(png_ptr);
// //     if (length > buf_state->bytes_left)
// //     {
// //         png_error(png_ptr, "read error");
// //     }
// //     memcpy(data, buf_state->data, length);
// //     buf_state->bytes_left -= length;
// //     buf_state->data += length;
// // }

// // void *limited_malloc(png_structp png_ptr, png_alloc_size_t size)
// // {
// //     // libpng may allocate large amounts of memory that the fuzzer reports as
// //     // an error. In order to silence these errors, make libpng fail when trying
// //     // to allocate a large amount. This allocator used to be in the Chromium
// //     // version of this fuzzer.
// //     // This number is chosen to match the default png_user_chunk_malloc_max.
// //     if (size > 8000000)
// //         return 0;

// //     return malloc(size);
// // }

// // void default_free(png_structp png_ptr, png_voidp ptr)
// // {
// //     return free(ptr);
// // }

// // static const int kPngHeaderSize = 8;

// // // Entry point for LibFuzzer.
// // // Roughly follows the libpng book example:
// // // http://www.libpng.org/pub/png/book/chapter13.html
// // extern int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
// // {
// //     if (size < kPngHeaderSize)
// //     {
// //         return 0;
// //     }

// //     // std::vector<unsigned char> v(data, data + size);
// //     // if (png_sig_cmp(v.data(), 0, kPngHeaderSize))
// //     // {
// //     //     // not a PNG.
// //     //     return 0;
// //     // }

// //     struct PngObjectHandler png_handler;
// //     memset(&png_handler, 0, sizeof(png_handler));
// //     png_handler.png_ptr = 0;
// //     png_handler.row_ptr = 0;
// //     png_handler.info_ptr = 0;
// //     png_handler.end_info_ptr = 0;

// //     png_handler.png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
// //     if (!png_handler.png_ptr)
// //     {
// //         destroy_PngObjectHandler(&png_handler);
// //         return 0;
// //     }

// //     png_handler.info_ptr = png_create_info_struct(png_handler.png_ptr);
// //     if (!png_handler.info_ptr)
// //     {
// //         PNG_CLEANUP
// //         return 0;
// //     }

// //     png_handler.end_info_ptr = png_create_info_struct(png_handler.png_ptr);
// //     if (!png_handler.end_info_ptr)
// //     {
// //         PNG_CLEANUP
// //         return 0;
// //     }

// //     // Use a custom allocator that fails for large allocations to avoid OOM.
// //     png_set_mem_fn(png_handler.png_ptr, 0, limited_malloc, default_free);

// //     png_set_crc_action(png_handler.png_ptr, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);
// // #ifdef PNG_IGNORE_ADLER32
// //     png_set_option(png_handler.png_ptr, PNG_IGNORE_ADLER32, PNG_OPTION_ON);
// // #endif

// //     // Setting up reading from buffer.
// //     png_handler.buf_state = malloc(sizeof(struct BufState));
// //     png_handler.buf_state->data = data + kPngHeaderSize;
// //     png_handler.buf_state->bytes_left = size - kPngHeaderSize;
// //     png_set_read_fn(png_handler.png_ptr, png_handler.buf_state, user_read_data);
// //     png_set_sig_bytes(png_handler.png_ptr, kPngHeaderSize);

// //     if (setjmp(png_jmpbuf(png_handler.png_ptr)))
// //     {
// //         PNG_CLEANUP
// //         return 0;
// //     }

// //     // Reading.
// //     png_read_info(png_handler.png_ptr, png_handler.info_ptr);

// //     // reset error handler to put png_deleter into scope.
// //     if (setjmp(png_jmpbuf(png_handler.png_ptr)))
// //     {
// //         PNG_CLEANUP
// //         return 0;
// //     }

// //     png_uint_32 width, height;
// //     int bit_depth, color_type, interlace_type, compression_type;
// //     int filter_type;

// //     if (!png_get_IHDR(png_handler.png_ptr, png_handler.info_ptr, &width,
// //                       &height, &bit_depth, &color_type, &interlace_type,
// //                       &compression_type, &filter_type))
// //     {
// //         PNG_CLEANUP
// //         return 0;
// //     }

// //     // This is going to be too slow.
// //     if (width && height > 100000000 / width)
// //     {
// //         PNG_CLEANUP
// //         return 0;
// //     }

// //     // Set several transforms that browsers typically use:
// //     png_set_gray_to_rgb(png_handler.png_ptr);
// //     png_set_expand(png_handler.png_ptr);
// //     png_set_packing(png_handler.png_ptr);
// //     png_set_scale_16(png_handler.png_ptr);
// //     png_set_tRNS_to_alpha(png_handler.png_ptr);

// //     int passes = png_set_interlace_handling(png_handler.png_ptr);

// //     png_read_update_info(png_handler.png_ptr, png_handler.info_ptr);

// //     png_handler.row_ptr = png_malloc(
// //         png_handler.png_ptr, png_get_rowbytes(png_handler.png_ptr,
// //                                               png_handler.info_ptr));

// //     for (int pass = 0; pass < passes; ++pass)
// //     {
// //         for (png_uint_32 y = 0; y < height; ++y)
// //         {
// //             png_read_row(png_handler.png_ptr,
// //                          (png_bytep)(png_handler.row_ptr), 0);
// //         }
// //     }

// //     png_read_end(png_handler.png_ptr, png_handler.end_info_ptr);

// //     PNG_CLEANUP
// //     return 0;
// // }

// // #define fail(msg, label)                                                                  \
// //     {                                                                                     \
// //         printf("FAIL on %s:%llu: %s\n", (__func__), (unsigned long long)(__LINE__), msg); \
// //         goto fail_##label;                                                                \
// //     }

// // int main(int argc, char *argv[])
// // {
// //     // #if __has_feature(undefined_behavior_sanitizer) && __has_feature(address_sanitizer)
// //     //     printf("GAMER!!!\n");
// //     // #else
// //     //     printf("NOT GAMER!!!\n");
// //     // #endif
// //     //  while(1) {};

// //     char *input = NULL, *output = NULL;

// //     if (argc == 2)
// //     {
// //         input = argv[1];
// //         output = "/dev/null";
// //     }
// //     else if (argc == 3)
// //     {
// //         input = argv[1];
// //         output = argv[2];
// //     }
// //     else
// //     {
// //         printf("Usage: %s <png_file_in> <png_file_out>\n", argv[0]);
// //         return 1;
// //     }

// //     printf("Processing %s -> %s\n", input, output);

// //     // process_image(input, output);
// //     FILE *fp = fopen(input, "rb");
// //     if (!fp)
// //         fail("fopen()", none);

// //     // Get file size
// //     fseek(fp, 0, SEEK_END);
// //     size_t size = ftell(fp);
// //     fseek(fp, 0, SEEK_SET);

// //     // Allocate buffer
// //     uint8_t *data = (uint8_t *)malloc(size);
// //     if (!data)
// //         fail("malloc()", fp);

// //     // Read file
// //     if (fread(data, 1, size, fp) != size)
// //         fail("fread()", data);

// //     // Perform the processing of the libpng_read_fuzzer code
// //     LLVMFuzzerTestOneInput(data, size);

// //     // Cleanup
// // fail_data:
// //     free(data);

// // fail_fp:
// //     fclose(fp);

// // fail_none:
// //     return 0;
// // }
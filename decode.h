#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret file from stego Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    int extn_secret_file_size;
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

} DecodeInfo;


/* Decoding function prototypes */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Skip the BMP header while reading stego image */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Decode Magic String and verify it */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *size);

/* Decode secret file extension */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo, long *file_size);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char *data, char *image_buffer);

/* Decode a size (int) from 32 bytes of image data */
Status decode_size_from_lsb(DecodeInfo *decInfo, int *size);

#endif

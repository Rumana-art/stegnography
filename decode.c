#include <stdio.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function to print section headers */
void print_section(const char *title) {
    printf("\n");
    for(int i = 0; i < 50; i++) printf("=");
    printf("\n");
    printf("  %s\n", title);
    for(int i = 0; i < 50; i++) printf("=");
    printf("\n");
}

/* Function to print progress bar */
void print_progress(long current, long total) {
    int bar_width = 30;
    float progress = (float)current / total;
    int filled = (int)(progress * bar_width);
    
    printf("\r[");
    for(int i = 0; i < filled; i++) printf("#");
    for(int i = filled; i < bar_width; i++) printf(".");
    printf("] %3d%%  (%ld/%ld bytes)", (int)(progress * 100), current, total);
    fflush(stdout);
}

/* ============== Original Functions ============== */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    printf("\n[INFO] Validating arguments...\n");
    
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
        printf("[OK] Stego image: %s\n", decInfo->stego_image_fname);
    }
    else
    {
        printf("[ERROR] Input file must be .bmp\n");
        return e_failure;
    }

    if (argv[3] != NULL)
    {
        decInfo->secret_fname = argv[3];
        printf("[OK] Output base name: %s\n", decInfo->secret_fname);
    }
    else
    {
        decInfo->secret_fname = "default";
        printf("[WARN] No output name specified, using 'default'\n");
    }

    return e_success;
}

Status open_files_decode(DecodeInfo *decInfo)
{
    printf("\n[INFO] Opening stego image...\n");
    
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("[ERROR] Unable to open file: %s\n", decInfo->stego_image_fname);
        perror("fopen");
        return e_failure;
    }
    
    printf("[OK] File opened successfully\n");
    return e_success;
}

Status skip_bmp_header(FILE *fptr_stego_image)
{
    printf("[INFO] Skipping BMP header (54 bytes)...\n");
    fseek(fptr_stego_image, 54, SEEK_SET);
    printf("[OK] Header skipped#️⃣\n");
    return e_success;
}

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
        *data = *data | ((image_buffer[i] & 1) << i);
    }
    return e_success;
}

Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&data[i], decInfo->image_data);
    }
    return e_success;
}

Status decode_size_from_lsb(DecodeInfo *decInfo, int *size)
{
    char str[32];
    fread(str, 32, 1, decInfo->fptr_stego_image);

    *size = 0;
    for (int i = 0; i < 32; i++)
    {
        *size = *size | ((str[i] & 1) << i);
    }

    return e_success;
}

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    printf("[INFO] Verifying magic string...\n");
    
    int len = strlen(magic_string);
    char decoded_magic[len + 1];

    decode_data_from_image(decoded_magic, len, decInfo);
    decoded_magic[len] = '\0';

    if (strcmp(decoded_magic, magic_string) == 0)
    {
        printf("[OK] Magic string verified: %s\n 🪄", decoded_magic);
        return e_success;
    }
    else
    {
        printf("[ERROR] Invalid magic string! Expected: %s, Found: %s\n 🪄", magic_string, decoded_magic);
        return e_failure;
    }
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *size)
{
    printf("[INFO] Decoding file extension size...\n");
    decode_size_from_lsb(decInfo, size);
    printf("[OK] Extension size: %d bytes\n", *size);
    return e_success;
}

Status decode_secret_file_extn(int size, DecodeInfo *decInfo)
{
    printf("[INFO] Decoding file extension...\n");
    decode_data_from_image(decInfo->extn_secret_file, size, decInfo);
    decInfo->extn_secret_file[size] = '\0';
    printf("[OK] Extension: %s\n", decInfo->extn_secret_file);
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo, long *file_size)
{
    printf("[INFO] Decoding file size...\n");
    int size;
    decode_size_from_lsb(decInfo, &size);
    *file_size = size;
    
    if (size > 1024 * 1024) {
        printf("[OK] Size: %.2f MB\n", (float)size / (1024 * 1024));
    } else if (size > 1024) {
        printf("[OK] Size: %.2f KB\n", (float)size / 1024);
    } else {
        printf("[OK] Size: %ld bytes\n", *file_size);
    }
    
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    long total = decInfo->size_secret_file;
    
    printf("[INFO] Decoding secret data (%ld bytes)...\n", total);
    
    for (long i = 0; i < total; i++)
    {
        decode_data_from_image(&ch, 1, decInfo);
        fwrite(&ch, 1, 1, decInfo->fptr_secret);
        
        if (i % 100 == 0 || i == total - 1) {
            print_progress(i + 1, total);
        }
    }
    printf("\n");
    
    printf("[OK] Secret data decoded successfully!\n");
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    print_section("STARTING DECODING PROCESS");
    
    if (open_files_decode(decInfo) == e_success)
    {
        if (skip_bmp_header(decInfo->fptr_stego_image) == e_success)
        {
            if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
            {
                if (decode_secret_file_extn_size(decInfo, &decInfo->extn_secret_file_size) == e_success)
                {
                    if (decode_secret_file_extn(decInfo->extn_secret_file_size, decInfo) == e_success)
                    {
                        // Build output filename
                        char full_secret_fname[100];
                        strcpy(full_secret_fname, decInfo->secret_fname);
                        strcat(full_secret_fname, decInfo->extn_secret_file);

                        decInfo->fptr_secret = fopen(full_secret_fname, "w");
                        if (decInfo->fptr_secret == NULL)
                        {
                            printf("[ERROR] Unable to create output file: %s\n", full_secret_fname);
                            return e_failure;
                        }
                        printf("[OK] Output file created: %s\n", full_secret_fname);

                        if (decode_secret_file_size(decInfo, &decInfo->size_secret_file) == e_success)
                        {
                            if (decode_secret_file_data(decInfo) == e_success)
                            {
                                print_section("DECODING COMPLETED SUCCESSFULLY");
                                printf("\n");
                                printf("  Output File: %s\n 📁", full_secret_fname);
                                printf("  File Size: %ld bytes\n📂", decInfo->size_secret_file);
                                printf("  Status: SUCCESS\n");
                                printf("\n");
                                return e_success;
                            }
                            else
                            {
                                printf("[ERROR] Failed to decode secret data ❎\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("[ERROR] Failed to decode file size ❎\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("[ERROR] Failed to decode file extension ❎\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("[ERROR] Failed to decode extension size ❎\n");
                    return e_failure;
                }
            }
            else
            {
                printf("[ERROR] Invalid magic string - not a stego image! ❎\n");
                return e_failure;
            }
        }
        else
        {
            printf("[ERROR] Failed to skip BMP header ❎\n");
            return e_failure;
        }
    }
    else
    {
        printf("[ERROR] Failed to open files ❎\n");
        return e_failure;
    }
}
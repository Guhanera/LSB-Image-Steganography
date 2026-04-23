#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // argv[2] -> .bmp -> store argv[2] in structure decInfo->src_image_fname = argv[2];
    if (argv[2] != NULL)
    {
        const char * const img_xten = ".bmp";
        int img_xten_len = strlen(img_xten);

        char temp_len = strlen(argv[2]);

        for (int i = 0; i < img_xten_len; i++)
        {
            if (img_xten[img_xten_len - i - 1] != argv[2][temp_len - i - 1])
            {
                return e_failure;
            }
        }
        decInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    // argv[3] -> output_file_name
    // Remove the extension if present and store only the file_name
    if (argv[3] != NULL)
    {
        char *file_name = strtok(argv[3], ".");
        strcpy(decInfo->output_fname, file_name);
    }
    else
    {
        strcpy(decInfo->output_fname, "output"); // Default output file_name -> "output"
    }
}

Status open_files_decoding(DecodeInfo *decInfo)
{
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    if (decInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);
        return e_failure;
    }
    return e_success;
}

Status skip_bmp_header(FILE *fptr_src_image)
{
    fseek(fptr_src_image, 54, SEEK_SET); // Skip 54 bytes Header
    return e_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    // Core logic to decode 1 byte from 8 byte of data
    char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch |= ((image_buffer[i] & 1) << i);
    }
    return ch;
}

uint decode_int_from_lsb(char *image_buffer)
{
    // Decode an integer from 32 bytes of img data
    uint num = 0;
    for (int i = 0; i < 32; i++)
    {
        num |= ((image_buffer[i] & 1) << i);
    }
    return num;
}

Status decode_data_from_image(char *data, int size, FILE *fptr_src_image)
{
    char arr[8]; // Read 8 bytes of img data
    int i;
    for (i = 0; i < size; i++)
    {
        fread(arr, 8, 1, fptr_src_image);
        data[i] = decode_byte_from_lsb(arr);
    }
    data[i] = '\0';
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char arr[3];
    decode_data_from_image(arr, 2, decInfo->fptr_src_image);

    // Check if decoded magic string matches with encoded magic string
    // If not matched -> Terminate
    if (strcmp(arr, MAGIC_STRING) == 0)
    {
        return e_success;
    }
    printf("Error: Magic string not matched\n");
    return e_failure;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char arr[32];
    fread(arr, 32, 1, decInfo->fptr_src_image);
    decInfo->size_output_file_extn = decode_int_from_lsb(arr);

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    decode_data_from_image(decInfo->output_file_extn, decInfo->size_output_file_extn, decInfo->fptr_src_image);

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char arr[32];
    fread(arr, 32, 1, decInfo->fptr_src_image);
    decInfo->size_output_file = decode_int_from_lsb(arr);

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decode_data_from_image(decInfo->secret_data, decInfo->size_output_file, decInfo->fptr_src_image);

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_files_decoding(decInfo) == e_failure)
    {
        return e_failure;
    }

    if (skip_bmp_header(decInfo->fptr_src_image) == e_failure)
    {
        return e_failure;
    }

    if (decode_magic_string(decInfo) == e_failure)
    {
        return e_failure;
    }

    if (decode_secret_file_extn_size(decInfo) == e_failure)
    {
        return e_failure;
    }

    if (decode_secret_file_extn(decInfo) == e_failure)
    {
        return e_failure;
    }

    // Concatenate the user provided output file_name with decoded extension
    strcat(decInfo->output_fname, decInfo->output_file_extn);

    // Open the output file
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");

    if (decode_secret_file_size(decInfo) == e_failure)
    {
        return e_failure;
    }

    if (decode_secret_file_data(decInfo) == e_failure)
    {
        return e_failure;
    }

    for (int i = 0; i < decInfo->size_output_file; i++)
    {
        fputc(decInfo->secret_data[i], decInfo->fptr_output);
    }

    close_files_decoding(decInfo);

    animate("DECODING");
    printf("\r------------------------------------------------------\n");
    printf("                DECODED SUCCESSFULLY\n");
    printf("------------------------------------------------------\n");
}

void close_files_decoding(DecodeInfo *decInfo)
{
    fclose(decInfo->fptr_output);
    fclose(decInfo->fptr_src_image);
}
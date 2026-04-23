#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"


typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Output File Info */
    FILE *fptr_output;
    char output_fname[20];
    long size_output_file;
    char output_file_extn[10];
    long size_output_file_extn;

    char *secret_data;

} DecodeInfo;


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status open_files_decoding(DecodeInfo *decInfo);

Status skip_bmp_header(FILE *fptr_src_image);



char decode_byte_from_lsb(char *image_buffer);

uint decode_int_from_lsb(char *image_buffer);

Status decode_data_from_image(char* data,int size, FILE *fptr_src_image);


Status decode_magic_string(DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);


Status do_decoding(DecodeInfo *decInfo);


void close_files_decoding(DecodeInfo *decInfo);

#endif
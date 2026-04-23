#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

#define CLEAR(num, bit) num &= ~(1 << bit)
#define GET(num, bit) (num & (1 << bit)) >> bit

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // argv[2] -> .bmp -> store argv[2] in structure encInfo->src_image_fname = argv[2];
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
        encInfo->src_image_fname = argv[2];    // Store src_file_name
    }
    else
    {
        return e_failure;
    }

    // argv[3] --> secret file. Check for extension
    if (argv[3] != NULL)
    {
        char *file_xten = strrchr(argv[3], '.');   // Check for '.' from reverse to get extension
        if (file_xten == NULL)      // If extension not present return e_failure
        {
            return e_failure;
        }
        strcpy(encInfo->extn_secret_file, file_xten);   // Store extension
        encInfo->secret_fname = argv[3]; // Store file_name
    }
    else
    {
        return e_failure;
    }

    // argv[4] -> NULL-> store "stego.bmp" in structure
    //    -> no NULL -> .bmp -> store argv[4] in structure

    if (argv[4] != NULL)
    {
        const char * const img_xten = ".bmp";
        int img_xten_len = strlen(img_xten);

        char temp_len = strlen(argv[4]);

        for (int i = 0; i < img_xten_len; i++)
        {
            if (img_xten[img_xten_len - i - 1] != argv[4][temp_len - i - 1])
            {
                return e_failure;
            }
        }
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    rewind(fptr);
    fseek(fptr, 0, SEEK_END);

    return ftell(fptr);
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if ((encInfo->image_capacity ) > (strlen(MAGIC_STRING) + sizeof(int) + strlen(encInfo->extn_secret_file) + sizeof(int) + encInfo->size_secret_file) * 8)
    {
        return e_success;
    }
    else
    {
        fprintf(stderr, "<--- Error: File size too large --->\n");
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    rewind(fptr_dest_image);

    // Copy 54 bytes of header
    char arr[54];
    fread(arr, 54, 1, fptr_src_image);
    fwrite(arr, 54, 1, fptr_dest_image);

    return e_success;
}

Status encode_int_to_lsb(uint num, char *image_buffer)
{
    // For encoding 1 byte -> 8 bytes are required
    // For encoding int(4 bytes) -> 32 bytes are required
    for (int i = 0; i < 32; i++)
    {
        CLEAR(image_buffer[i], 0);
        image_buffer[i] |= GET(num, i);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Core logic to encode secret data at LSB of image data
    for (int i = 0; i < 8; i++)
    {
        CLEAR(image_buffer[i], 0); // Clear LSB
        image_buffer[i] |= GET(data, i); // Get 1 bit and SET at LSB
    }
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];  // 8 bytes of data from img data
    for (int i = 0; i < size; i++)
    {
        fread(arr, 8, 1, fptr_src_image);
        encode_byte_to_lsb(data[i], arr);
        fwrite(arr, 8, 1, fptr_stego_image);
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn_size(long file_size, EncodeInfo *encInfo)
{
    char arr[32];  // 32 bytes of img data to enocde integer (4 bytes)
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(file_size, arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(encInfo->extn_secret_file, strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char arr[32];  // 32 bytes of img data to enocde integer (4 bytes)
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(file_size, arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // Convert secret data as a string before encoding
    unsigned char data[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fread(data, encInfo->size_secret_file, 1, encInfo->fptr_secret);

    encode_data_to_image(data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch[1000];
    while (fread(&ch, 1, 1000, fptr_src) != 0)
    {
        fwrite(&ch, 1, 1000, fptr_dest);
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
        return e_failure;
    }

    if (check_capacity(encInfo) == e_failure)
    {
        return e_failure;
    }

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        return e_failure;
    }

    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
        return e_failure;
    }

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }

    if (encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    close_files(encInfo);

    animate("ENCODING");
    printf("\r------------------------------------------------------\n");
    printf("                ENCODED SUCCESSFULLY\n");
    printf("------------------------------------------------------\n");
}

void close_files(EncodeInfo *encInfo)
{
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
}
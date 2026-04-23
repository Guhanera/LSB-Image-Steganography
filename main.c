#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"

/*

<--- IMAGE LSB STEGANOGRAPHY --->

-> Name : Guhan Era
-> Roll.no : 25036B_36
-> Start date : 14.02.2026
-> End date : 22.02.2026
-> Description :
        *   This project implements an image steganography system that securely hides secret information
            inside an image using the Least Significant Bit (LSB) encoding technique.
        *   The input image is processed in binary form, and the secret message is converted into bits and
            embedded into the least significant bits of the image data, ensuring that the visual quality
            of the image remains unchanged.
        *   To enhance security, a magic string is embedded along with the secret data and acts as an
            authentication key during extraction.
        *   When decoding, the system first verifies the presence of the magic string; if the key matches,
            the hidden message is retrieved, otherwise extraction is denied.
        *   This approach provides secure and invisible data hiding suitable for confidential communication

*/

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "<--- Error: Invalid input --->\n");
        return 1;
    }

    // Print Help page
    if (strcmp(argv[1], "-h") == 0)
    {
        print_help();
        return 0;
    }

    OperationType operation = check_operation_type(argv);

    switch (operation)
    {
    case e_encode:

        EncodeInfo enc;
        if (read_and_validate_encode_args(argv, &enc) == e_failure)
        {
            printf("\033[1;31mError: Invalid arguments\033[0m\n");
            printf("\nFor Help Enter: \"./a.out -h\"\n");
            return 1;
        }
        do_encoding(&enc);

        break;
        //==============================================================================//
    case e_decode:

        DecodeInfo dec;
        if (read_and_validate_decode_args(argv, &dec) == e_failure)
        {
            printf("\033[1;31mError: Invalid arguments\033[0m\n");
            printf("\nFor Help Enter: \"./a.out -h\"\n");
            return 1;
        }
        do_decoding(&dec);

        break;
        //==============================================================================//
    case e_unsupported:

        printf("\033[1;31mError: Invalid arguments\033[0m\n");
        printf("\nFor Help Enter: \"./a.out -h\"\n");
        return 1;
    }

    return 0;
}
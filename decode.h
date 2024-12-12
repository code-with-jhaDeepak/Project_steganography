#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo  //creating the structure to store the cla data
{
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char image_data[MAX_IMAGE_BUF_SIZE];
    long size_secret_file;
    uint secret_file_extn_size;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;

//validating the command line arguments
OperationType read_and_validate_decode_args(char *argv[], DecodeInfo *decinfo); //checking cla

//initiating the decode
void do_decoding(DecodeInfo *decinfo);

//opening all the required files
Status open_files_(DecodeInfo *decinfo); 

//decoding the "#*" to find data is encoded or not
Status decode_magic_string(DecodeInfo *decinfo);

//collecting the values from the lsb
void decode_data_from_lsb(char *image_buffer, int len, char *);

//decoding the extension size
void decode_extn_size(DecodeInfo *decinfo);

//decoding the integer values
uint decode_size(DecodeInfo *decinfo);

//decoding .txt data
void decode_extn_data(DecodeInfo *decinfo);

//decoding the secret file size
void decode_secret_file_size(DecodeInfo *decinfo);

//decoding the secret data and saving it to output file
void decode_secret_file_data(DecodeInfo *decinfo);

#endif


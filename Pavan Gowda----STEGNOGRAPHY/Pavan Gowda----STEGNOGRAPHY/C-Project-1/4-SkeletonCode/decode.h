#ifndef DECODE_H
#define DECODE_H
#include "types.h" //Contains user defined types

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    /* Decode FILE Info */
    char *decode_fname;
    FILE *fptr_decode_file;
    char decode_file_name[20];
    uint secret_file_size;
} DecodeInfo;

/*  Decoding function prototypes */
/* Check operation type */
// OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */

/* Get File pointers for i/p and o/p files */
Status Open_files(DecodeInfo *decInfo);

/*perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Decoding the Stored magic string */
Status decode_magic_string(FILE *fptr_stego_image);
/* Decode the lsb bit */

Status decode_lsb_bit(char *data);
/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo, FILE *fptr_stego_image,int *extn_size);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo, int size, FILE *fptr_stego_image);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo, FILE *fptr_stego_image);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif

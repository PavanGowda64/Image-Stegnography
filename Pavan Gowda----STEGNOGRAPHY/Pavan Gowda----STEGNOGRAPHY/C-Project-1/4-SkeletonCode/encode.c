#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("INFO: width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("INFO: height = %u\n", height);

    // Return image capacity
    printf("INFO: Image size = %u\n", width * height * 3);
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

uint get_file_size(FILE *fptr_secret)
{
    // Step 1:move the file pointer to the end(fseek)
    // Step 2:return by using ftell
    fseek(fptr_secret, 0, SEEK_END);
    uint size = ftell(fptr_secret);
    fseek(fptr_secret, 0, SEEK_SET);
    return size;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encoInfo)
{
    // Step 1:check argv[2] is .bmp file or not,if yes goto step 2,if not goto step 3
    // Step 2: store into src_image_fname
    // Step 3:return e_filename

    // Step 4:check argv[2] is .txt file or not,if yes goto step 5,if not goto step 5
    // Step 5:
    // i)store the file name into the  secret_fname
    // ii)fetch the extn and store into the extn_sec_file(strcpy)
    // Step 6:return e_failure

    // Step 7:check argv[4] is!=NULL,if yes goto step 8 ,if no goto step 12
    // Step 8:if yes,goto Step 9
    // step 9:check the file is .bmp or not ,if yes goto step 10,if no go to step 11
    // Step 10:store the filename into the stego_image_fname,return e_success
    // Step 11:return e_failure

    // Step 12:store the default filename [stego.bmp]into the stego_image_fname
    if (strstr(argv[2], ".") == NULL)
    {
        printf("ERROR: .bmp file is not passed\n");
        return e_failure;
    }
    else if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        printf("INFO:  .bmp file is passed as second argument\n");
        encoInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("ERROR: Invalid .bmp extension file passed\n");
        return e_failure;
    }

    if (argv[3] == NULL)
    {
        printf("ERROR: ********* NO Secret file is passed  *********\n");

        printf("********* Pass .txt  as argv[3]  *********\n");
        return e_failure;
    }
    else if (strcmp(strstr(argv[3], "."), ".txt") == 0)
    {
        encoInfo->secret_fname = argv[3];
        printf("INFO: Secret file With extension  is passed\n");
    }
    else
    {
        printf("ERROR: Invalid file format pass only .txt \n");
        return e_failure;
    }

    if (argv[4] == NULL)
    {
        FILE *fptr_stego_image = fopen("stego.bmp", "w");
        if (!fptr_stego_image)
        {
            perror("fopen");
            return e_failure;
        }

        encoInfo->stego_image_fname = "stego.bmp";
    }
    else
    {
        char *ext = strrchr(argv[4], '.');
        if (ext && strcmp(ext, ".bmp") == 0)
        {
            encoInfo->stego_image_fname = argv[4];
        }
        else
        {
            fprintf(stderr, "ERROR: Output file must be .bmp\n");
            return e_failure;
        }
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
// Check if the source file size is greater than secret file size
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image); // Source image size
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);           // Secret file size
    printf("INFO: Size of secret file before encoding is %ld\n",encInfo->size_secret_file);
    if ((encInfo->image_capacity) > (54 + strlen(MAGIC_STRING) * 8 + 32 + strlen(encInfo->extn_secret_file) * 8 + 32) + encInfo->size_secret_file * 8)
    {
        return e_success;
    }
    else
    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{

    // char image_buffer[54];
    // step 1:read 54Bytes from the src
    // step 2:Write 54Bytes to the dest
    // step 3:return e_success
    char arr[54];
    rewind(fptr_src_image);
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(arr, 54, 1, fptr_src_image);
    fwrite(arr, 54, 1, fptr_dest_image);
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image,encInfo);
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
    char image_buffer[8];
    for (int i = 0; i < size; i++)
    {
        // Step 1:read 8 bytes from the src
        fread(image_buffer, 8, 1, fptr_src_image);
        // Step 2:call the byte to lsb (data[i], image_buffer)
        encode_byte_to_lsb(data[i], image_buffer);
        // Step 3: write 8bytes to the dest
        fwrite(image_buffer, 8, 1, fptr_stego_image);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    char cleared_bit, bit_to_encode;

    for (int i = 0; i < 8; i++)
    {
        // Step 1: Clear the least significant bit of the current byte in the image buffer
        cleared_bit = image_buffer[i] & (~1);

        // Step 2: Get the i-th bit from data, starting from MSB
        bit_to_encode = (data >> (i)) & 1;

        // Step 3: Combine the cleared bit with the bit to encode
        image_buffer[i] = cleared_bit | bit_to_encode;
    }

    return e_success;
}
Status encode_file_extension_size(int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char buffer[32];
    fread(buffer,32,1,fptr_src_image);
    encode_size_to_lsb(size,buffer);
    fwrite(buffer,32,1,fptr_stego_image);
    printf("INFO: Extension size is : %d\n",size);
    return e_success;
}
Status encode_size_to_lsb(int size, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        char clear, get;
        // Step 1: Clear the least significant bit of the current byte in the image buffer
        // Step 2: Get the i-th bit from size, starting from LSB
        // Step 3: Combine the cleared bit with the bit to encode
        clear = image_buffer[i] & (~1);
        get = (size >> i) & 1;
        image_buffer[i] = clear | get;
    }
    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Secret File extension is : %s\n", file_extn);
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image,encInfo);
    return e_success;
}

Status encode_secret_file_extn_size(long extn_size, EncodeInfo *encInfo)
{
    char image_buffer[32];
    // step 1: read 32 bytes from src_image
    fread(image_buffer, 32, 1, encInfo->fptr_src_image);
    // Step 2:call a size to las(extn,size,image_buffer)
    encode_size_to_lsb(extn_size,image_buffer);
    fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);
    // step 3:write 32 bytes to the dest_image
    printf("INFO: Size of secret file extn is %ld\n",extn_size);
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    printf("INFO: Size of secret file is %ld\n", file_size);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_buffer[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fread(secret_buffer, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    encode_data_to_image(secret_buffer, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image,encInfo);
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer;

    while (fread(&buffer, 1, 1, fptr_src) == 1)
    {
        fwrite(&buffer, 1, 1, fptr_dest);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{

    if (open_files(encInfo) == e_success)
    {
        printf("NOTE: ----------Open files is successfull-------\n");
    }
    else
    {
        printf("ERROR:  Enable to open file.\n");
        return e_failure;
    }

    if (check_capacity(encInfo) == e_success)
    {
       printf("ERROR: Image has sufficient capacity for encoding.\n");
    }
    else
    {
        printf("ERROR: Image has insufficient capacity.\n");
        return e_failure;
    }
    

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {

        printf("INFO: BMP header copied successfully.\n");
    }
    else
    {
        printf("ERROR:  Unable to copy BMP header.\n");
        return e_failure;
    }

    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
	printf("INFO: Magic string encoded successfully.\n");
    }
    else
    {
	printf("ERROR:  Magic string encoding failed.\n");
	return e_failure;
    }


    if (encode_file_extension_size(strlen(strstr(encInfo->secret_fname, ".")), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Encode_file_extension_size Successfull\n");
    }
    else
    {
        printf("ERROR: Encoding _file_extension_size Failure\n");
        return e_failure;
    }

    if (encode_secret_file_extn(strstr(encInfo->secret_fname, "."), encInfo) == e_success)
    {
        printf("INFO : Secret file extension including dot encoded successfully.\n");
    }
    else
    {
        printf("ERROR: -encode secret file ext error-\n");
        return e_failure;
    }

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("INFO: Secret file extenction size encoded successfully.\n");
    }
    else
    {
        printf("ERROR: Encoding of secret file failed.\n");
        return e_failure;
    }

    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("INFO : Secret file data encoded Successfully\n");
    }
    else
    {
        printf("ERROR: Encoding of secret file data failed.\n");
        return e_failure;
    }
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO : Copying the remaining data is Successfully completed\n");
    }
    else
    {
        printf("ERROR: Error copying remaining image data.\n");
        return e_failure;
    }

    return e_success;
}

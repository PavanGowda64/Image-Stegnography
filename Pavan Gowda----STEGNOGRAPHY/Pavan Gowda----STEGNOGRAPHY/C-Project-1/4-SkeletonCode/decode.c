#include <stdio.h>
#include <string.h>
#include "common.h"
#include "decode.h"
#include "types.h"

// validating decode parameters that has passed to the command line

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // here i am checking argv[2] has .(dot) or not 
    if(strstr(argv[2],".")==NULL)
    {
        
        printf("EX : ./a.out -d stego.bmp \n");
        return e_failure;
    }
    // here i am checking (argv[2],".")=.bmp is equal to .bmp or not 
    else if(strcmp(strstr(argv[2],"."),".bmp")==0)
    {
    // if equal then store to stego_image_fname
        printf("INFO: Opened %s\n", argv[2]);
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
    // print error message and return e_failure
        printf("ERROR: Invalid .bmp extension file passed\n");
        printf("Usage : ./a.out -d stego.bmp \n");
        return e_failure;
    }


    //here we are validating the argv[3] is passed or not if it is  passed
    if(argv[3] != NULL)
    {
    // then store that in  decode_fname
        decInfo->decode_fname = strtok(argv[3], ".");
        printf("INFO: Decode file is successfully stored\n");
    // and return e_success    
        return e_success;
    }
    else
    {
    // if it is not passed then store a default name   
        decInfo->decode_fname = "decode";
        printf("INFO: Creating decode.txt as default\n");
    // and return e_success    
        return e_success;
    }

}

// here we are opening the file, the file that are stored when validating
Status Open_files(DecodeInfo *decInfo)
{
    // here we are opening the source image file that is stego_image_fname in a read mode (because we are reading not writing to it) then that open function
    //  will return the pointer that we are storing it in a fptr_stego_image
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // if no file present then print error message and return e_failure
    if (decInfo->fptr_stego_image == NULL)
    {
        // this function gives us a system genereted error
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    // if everything is OK then return e_success and print the message
    printf("INFO: Opened stegged image.bmp file\n");
    return e_success;
}

// in this function we are decoding the lest significant bits not fully decoding
// just we are collecting 1 byte and (&) with 1 to extract the lsb that bit is stored in the
// data pointer
Status decode_lsb_bit(char *data)
{
    *data = (*data) & 1;
    return e_success;
}

// here we are decoding the magic string
Status decode_magic_string(FILE *fptr_stego_image)
{
    // i am taking the char variable and buffer
    char magic_char = 0, buffer[MAX_IMAGE_BUF_SIZE];
    // here i am declaring the magic string array size
    char magic_str[strlen(MAGIC_STRING) + 1];
    // her iam setting source image to 54 bit L(long )to match the function
    fseek(fptr_stego_image, 54L, SEEK_SET);
    // here i am calling the one for loop till length of magic string
    for (int i = 0; i < strlen(MAGIC_STRING); i++)
    {

        magic_char = 0;
        // here i am reading the source image data from 54 byte, at 1 time 8 byte is read
        // and stored it in the buffer
        fread(&buffer, 1, 8, fptr_stego_image);
        // here i am calling  one loop for 8 times
        for (int j = 0; j < 8; j++)
        {
            // here i am calling decode lsb bit every time, i am passing 1 byte of data for every iteration
            // decode_lsb_bit,it will give me the lsb of every byte i passed
            decode_lsb_bit(&buffer[j]);
            // here i am left shifting one one bit and oring(|) with magic_char it will decode the magic string
            magic_char = (buffer[j] << j) | magic_char;
        }
        // after every 8 byte of data has been decoded properly  it will be stored tn the magic_str
        magic_str[i] = magic_char;
    }
    // adding null terminator
    magic_str[strlen(MAGIC_STRING)] = '\0';
    // here i am checking that the magic string that i have decoded is equal to the original magic string
    if (strcmp(magic_str, MAGIC_STRING) == 0)
    {
        // if equal then return e_success
        printf("INFO: Both are matched\n");
    }
    else
    {
        // else return e_failure
        printf("ERROR: Magic strings are not equal, data is not encrypted in the image\n");
        return e_failure;
    }
    return e_success;
}

// here i am calling the decode_secret_file_extn to extract(.txt)
Status decode_secret_file_extn(DecodeInfo *decInfo, int size, FILE *fptr_stego_image)
{
    // Buffers for reading and decoding
    char buffer[MAX_IMAGE_BUF_SIZE], file_extn[size], extn_char = 0;
    // Loop through the size of the extension
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the image for each character of the extension
        fread(&buffer, 1, MAX_IMAGE_BUF_SIZE, fptr_stego_image);
        // Decode LSB from each byte
        for (int j = 0; j < MAX_IMAGE_BUF_SIZE; j++)
        {
            // call the decode_lsb_bit
            decode_lsb_bit(&buffer[j]);

            extn_char = (buffer[j] << j) | extn_char;
        }
        // Store the decoded character in file_extn
        file_extn[i] = extn_char;
        // Reset for next character
        extn_char = 0;
    }
    // Add null terminator to file_extn
    file_extn[size] = '\0';
    // storing the decoded characters in decode_file_name
    strcpy(decInfo->decode_file_name, decInfo->decode_fname);
    // concatenating the decoded characters with previously stored file name decode
    strcat(decInfo->decode_file_name, file_extn);
    // Open the decoded file in write mode
    decInfo->fptr_decode_file = fopen(decInfo->decode_file_name, "w");
    // If unable to open the file, print error and return failure
    if (decInfo->fptr_decode_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_file_name);
        return e_failure;
    }
    printf("INFO: Decoded the File Extension\n");
    printf("%s\n", file_extn);
    return e_success;
}

// Function to decode the size of the secret file extension
Status decode_secret_file_extn_size(DecodeInfo *decInfo, FILE *fptr_stego_image, int *extn_size)
{
    char buffer[32]; // Buffer to hold the size data (32 bits)
    *extn_size = 0;  // Initialize extension size to 0
    fread(&buffer, sizeof(char), 32, fptr_stego_image);
    // Loop through the 32 bits and decode LSB to form the size
    for (int i = 0; i < 32; i++)
    {
        decode_lsb_bit(&buffer[i]);
        *extn_size = ((int)buffer[i] << i) | *extn_size;
    }
    printf("INFO: Decoded the  File Extension Size\n");
    return e_success;
}

// Function to decode the size of the secret file (in bytes)
Status decode_secret_file_size(DecodeInfo *decInfo, FILE *fptr_stego_image)
{
    // Buffer to hold the size data (32 bits)
    char buffer[32];
    // Variable to store decoded file size
    int secret_file_size = 0;
    // Read 32 bytes from the image
    fread(&buffer, sizeof(char), 32, fptr_stego_image);
    // Loop through the 32 bits and decode LSB to form the size
    for (int i = 0; i < 32; i++)
    {
        decode_lsb_bit(&buffer[i]);
        secret_file_size = ((int)buffer[i] << i) | secret_file_size;
    }
    // Store the decoded size in decInfo->secret_file_size
    decInfo->secret_file_size = secret_file_size;
    printf("INFO: Decoded the  File Size\n");
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    // Buffers for reading and decoding
    char buffer[MAX_IMAGE_BUF_SIZE], file_data = 0;
    // Loop through the size of the secret file
    for (int i = 0; i < decInfo->secret_file_size; i++)
    {
        // Read 8 bytes from the image for each character of the data
        fread(&buffer, 1, MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);
        for (int j = 0; j < MAX_IMAGE_BUF_SIZE; j++)
        {
            // Decode LSB from each byte
            decode_lsb_bit(&buffer[j]);
            file_data = (buffer[j] << j) | file_data;
        }
        // Write the decoded character to the decode file
        fwrite(&file_data, sizeof(char), 1, decInfo->fptr_decode_file);
        // Reset for next character
        file_data = 0;
    }
    printf("INFO: decoded the  File Data\n");
    return e_success;
}
// Function to perform the complete decoding process
Status do_decoding(DecodeInfo *decInfo)
{
    int extn_size; // Variable to store the size of the secret file extension

    // Step 1: Open the necessary files
    if (Open_files(decInfo) == e_success)
    {
        printf("INFO: ## Decoding Procedure Started ##\n");
    }
    else
    {
        // If opening files fails, print error and return failure
        printf("ERROR: ## Decoding Procedure Failed ##\n");
        return e_failure;
    }

    // Step 2: Decode the magic string (signature)
    if (decode_magic_string(decInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Decoding Magic String Signature\n");
    }
    else
    {
        // If decoding the magic string fails, print error and return failure
        printf("ERROR: ## Decoding Magic String Failed ##\n");
        return e_failure;
    }

    // Step 3: Decode the size of the secret file extension
    if (decode_secret_file_extn_size(decInfo, decInfo->fptr_stego_image, &extn_size) == e_success)
    {
        printf("INFO: Decoding Secret File Extension Size\n");
        printf("%d\n", extn_size); // Print the decoded extension size
    }
    else
    {
        // If decoding the extension size fails, print error and return failure
        printf("ERROR: ## Decoding Secret File Extension Size Failed ##\n");
        return e_failure;
    }

    // Step 4: Decode the secret file extension based on the decoded size
    if (decode_secret_file_extn(decInfo, extn_size, decInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Decoding Secret File Extension\n");
    }
    else
    {
        // If decoding the extension fails, print error and return failure
        printf("ERROR: ## Decoding Secret File Extension Failed ##\n");
        return e_failure;
    }

    // Step 5: Decode the size of the secret file
    if (decode_secret_file_size(decInfo, decInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Decoding Secret File Size\n");
    }
    else
    {
        // If decoding the file size fails, print error and return failure
        printf("ERROR: ## Decoding Secret File Size Failed ##\n");
        return e_failure;
    }

    // Step 6: Decode the actual secret file data
    if (decode_secret_file_data(decInfo) == e_success)
    {
        printf("INFO: ## Decoding Done Successfully ##\n");
        fclose(decInfo->fptr_stego_image); // Close the stego image file after successful decoding
        return e_success;
    }
    else
    {
        // If decoding the file data fails, print error and return failure
        printf("ERROR: ## Decoding Data Failed ##\n");
        fclose(decInfo->fptr_stego_image); // Ensure file is closed on failure
        return e_failure;
    }
}

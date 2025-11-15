/*---------------------------------------------------------------
Name:Pavan Gowda A.Y
Date:24-09-2025
Project name:Stegnography
-----------------------------------------------------------------*/

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>

int main(int argc, char *argv[])
{
	EncodeInfo encInfo;
	DecodeInfo decInfo;
	if (argc < 2)
	{
		printf("INFO: =====No Operation is selected =======\n");
		printf("ERROR: Usage : ./a.out -e beautiful.bmp secret.txt op_filename(optinal)\n");
		return 0;
	}
	if (check_operation_type(argv) == e_unsupported)
	{
		printf("ERROR: ===========Invalid operation Selceted =======\n");

		printf("===========please pass argv[1] as -e or -d ========\n");

		printf("ERROR: Usage : ./a.out -e beautiful.bmp secret.txt op_filename(optinal)\n");
		return 0;
	}
	if (check_operation_type(argv) == e_encode)
	{
		if (argc >= 4)
		{
			printf("INFO: -checking the number of argumnets for encoding is successfull-\n");
			printf("INFO: -READ AND VALIDATION STARTED-\n");
			if (read_and_validate_encode_args(argv, &encInfo) == e_success)
			{
				printf("INFO: -Read and Validating the encode args Successfull-\n");
				printf("INFO: -You Started Encoding-\n");

				if (do_encoding(&encInfo) == e_success)
				{
					printf("----------------------------------------------------\n");
					printf("----------Encoding Successfully completed-----------\n");
					printf("----------------------------------------------------\n");
					return 1;
				}
				else
				{
					printf("ERROR: ==============Encoding Failure==============\n");
				}
			}
		}
		else
		{
			printf("ERROR:==================Invalid number of arguments============\n");
			printf("ERROR:Usage : ./a.out -e beautiful.bmp secret.txt \n");
		}
	}
	if (check_operation_type(argv) == e_decode)
	{
		if (argc > 2)
		{
			printf("----------You Started Decoding-----------------\n");

			if (read_and_validate_decode_args(argv, &decInfo) == e_success)
			{
				printf("----------------Read and validation is success----------------------\n");
				if (do_decoding(&decInfo) == e_success)
				{
					printf("----------------------------------------------------\n");
					printf("      Decoding  Successfully completed              \n");
					printf("----------------------------------------------------\n");
				}
				else
				{
					printf("--------------------Decoding Failure-----------------------\n");
				}
			}
		}
		else
		{
			printf("----------------Invalid Number of argumnets-----------------\n");
			printf("Usage : ./a.out -d stego.bmp \n");
		}
	}

	return 0;
}
OperationType check_operation_type(char *argv[])
{
	// Step1:check the argv is -e or not ,if yes Goto step2,if not Goto step 3
	// Step2:return e_encode

	// Step 3:check if argv is -d or not,if yes Goto step4 ,if not goto step 5
	// Step 4:return e_decode

	// Step 5:return e_unsupported
	if (strcmp(argv[1], "-e") == 0)
	{
		return e_encode;
	}
	else if (strcmp(argv[1], "-d") == 0)
	{
		return e_decode;
	}
	else
		return e_unsupported;
}

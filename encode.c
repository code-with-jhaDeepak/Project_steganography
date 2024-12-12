/*
Name:Deepak Kumar Jha
Date:20/07/2022
Description:
Sample i/p:
sample o/p:
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "encode.h"
#include "common.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image) //--------3--2--1
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("width = %u\n", width);
	sleep(1);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);
	sleep(1);
	// Return image capacity
	return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encinfo)  //opening the files
{
	// Src Image file
	encinfo->fptr_src_image = fopen(encinfo->src_image_fname, "r");
	// Do Error handling
	if (encinfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encinfo->src_image_fname);

		return e_failure;
	}
	printf("INFO: Opened %s\n", encinfo->src_image_fname);
	sleep(1);

	// Secret file
	encinfo->fptr_secret = fopen(encinfo->secret_fname, "r");
	// Do Error handling
	if (encinfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encinfo->secret_fname);

		return e_failure;
	}
	printf("INFO: Opened %s\n", encinfo->secret_fname);
	sleep(1);

	// Stego Image file
	encinfo->fptr_stego_image = fopen(encinfo->stego_image_fname, "w");
	// Do Error handling
	if (encinfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encinfo->stego_image_fname);

		return e_failure;
	}
	printf("INFO: Opened %s\n", encinfo->stego_image_fname);
	sleep(1);

	// No failure return e_success
	return e_success;
}


OperationType check_operation_type(char *argv[])  //checking for operation 
{
	if(strcmp(argv[1], "-e") == 0) 
		return e_encode;
	else if(strcmp(argv[1], "-d") == 0)
		return e_decode;
	else
		return e_unsupported;
}


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encinfo) //validating the given data
{
	if(strstr(argv[2], ".") != NULL)   //checking for file with .bmp extension
	{
		if(strcmp(strstr(argv[2], "."), ".bmp") == 0)
		{
			encinfo->src_image_fname = argv[2]; 
		}
		else
			return e_failure;
	}
	else
	{
		return e_failure;
	}

	if(strstr(argv[3], ".") != NULL) //checking for file with .txt extension
	{
		if(strcmp(strstr(argv[3], "."), ".txt") == 0)
		{
			encinfo->secret_fname = argv[3];
		}
		else
			return e_failure;
	}
	else
	{
		return e_failure;
	}
	if(argv[4] != NULL)
	{
		if(strstr(argv[4], ".") != NULL)
		{
			if(strcmp(strstr(argv[4], "."), ".bmp") == 0) //assigning the user defigned output file
			{
				encinfo->stego_image_fname = argv[4];
			}
			else
				return e_failure;
		}
		else
		{
			return e_failure;
		}
	}
	else          //creating a file if user didnt give output file name
	{
		encinfo->stego_image_fname = "encrypted.bmp";
		printf("INFO: Output File not mentioned. Creating encrypted.bmp as default\n");
		sleep(1);
	}

	return e_success;
}


void do_encoding(EncodeInfo *encinfo)   //calling encode function
{
	printf("INFO: Opening required files.\n");
	sleep(1);
	if(open_files(encinfo) == e_success)
	{
		printf("INFO: Done\n");
		sleep(1);
		printf("INFO: Encoding Procedure Started .\n");
		sleep(1);
		if(check_capacity(encinfo) == e_success) //checking the capacity size of the given files
		{
			printf("INFO: Copying Image Header.\n");
			sleep(1);
			if(copy_bmp_header(encinfo) == e_success) //copying the hader files of given .bmp file to output file
			{
				printf("INFO: Done.\n");
				sleep(1);
				printf("INFO: Encoding Magic String Signature\n");
				sleep(1);
				encode_magic_string(MAGIC_STRING, encinfo); //encoding "#*" to locate the encryption
		        printf("INFO: Done.\n");
		        sleep(1);
				printf("INFO: Encoding Secret.txt File Extension.\n");
				sleep(1);
				strcpy(encinfo->extn_secret_file, strstr(encinfo->secret_fname, ".")); 
		        printf("INFO: Done\n");
		        sleep(1);
				encode_secret_file_extn_size(strlen(encinfo->extn_secret_file), encinfo); //encoding the size of extension
				encode_secret_file_extn(encinfo);
				printf("INFO: Encoding Secret.txt File Size.\n");
				sleep(1);
				encode_secret_file_size(encinfo->size_secret_file, encinfo); //encoding the extension
		        printf("INFO: Done.\n");
		        sleep(1);
				encode_secret_file_data(encinfo);                //encoding secret data to .bmp file
				printf("INFO: Copying Left Over Data.\n");
				sleep(1);
				copy_remaining_data(encinfo);
		        printf("INFO: Done.\n");
		        sleep(1);
		        printf("INFO:  Encoding Done Successfully .\n");
			}
			else
				puts("ERROR: Header File Not Copied.");
		}
		else
			puts("ERROR: Image With More Capacity Required");

	}
	else
		puts("Error: Can't Open the file.");
}


Status check_capacity(EncodeInfo *encinfo)  //checking the capacity function
{
	encinfo->image_capacity = get_image_size_for_bmp(encinfo->fptr_src_image);
	printf("INFO: Checking for secret.txt size.\n");
	sleep(1);
	encinfo->size_secret_file = get_file_size(encinfo->fptr_secret); //saving the file size

	if(encinfo->size_secret_file <1)
	{
		return e_failure;
	}

	printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt.\n");
	sleep(1);

	uint required_size= 8*(2+4+4+4+encinfo->size_secret_file)+54; //checking for minimum required size
	if(encinfo->image_capacity > required_size)
	{
		printf("INFO: Found OK\n");
		sleep(1);
		return e_success;
	}
	else
		return e_failure;
}


uint get_file_size(FILE *fptr)  //getting size of the file
{
	fseek(fptr, 0, SEEK_END);   //taking pointer of the 1st place of file
	if(ftell(fptr)>0)
	{
		printf("INFO:  Not Empty\n");
		sleep(1);
		return ftell(fptr);
	}
	else
		return 0;
}


Status copy_bmp_header(EncodeInfo *encinfo)  //copying the first 54 byte of image header as it is
{
	char arr[54];
	fseek(encinfo->fptr_src_image, 0, SEEK_SET); //taking ptr to 1st place
	fread(arr, 54, 1, encinfo->fptr_src_image);  
	fwrite(arr, 54, 1, encinfo->fptr_stego_image);  //writing the 1st 54 byte data to the stego file
	return e_success;
}


Status encode_magic_string(const char *magic_string, EncodeInfo *encinfo) //encoding "#*" magic string
{
	encode_data_to_image(magic_string, 2, encinfo);
}

Status encode_data_to_image(const char *data, int size, EncodeInfo *encinfo) //encoding data to images
{
	for(int i=0; i<size; i++)
	{
		fread(encinfo->image_data, 8, 1, encinfo->fptr_src_image);  //taking 8byte as 1 char requires 8 byte to store in ecoded form
		encode_byte_to_lsb(data[i], encinfo->image_data);
		fwrite(encinfo->image_data, 8, 1, encinfo->fptr_stego_image);
	}
	return e_success;
}


Status encode_byte_to_lsb(char data, char *image_buffer)  //altering the lsb of the given size with secret data
{
	int mask=0x80;
	for(int i=0; i<8; i++)
	{
		image_buffer[i] = (image_buffer[i] & 0xfe) | ((data & mask) >> 7-i); //creating mask to clear lsb
		mask = mask >> 1;
	}
	return e_success;
}


Status encode_secret_file_extn_size(int size, EncodeInfo *encinfo)  //encoding the file extn size
{ 
	char arr[32];
	fread(arr, 32, 1, encinfo->fptr_src_image); //taking 32 byte as integer requires 4 bytes
	encode_size_to_stego(size, arr);
	fwrite(arr, 32, 1, encinfo->fptr_stego_image);
}


Status encode_size_to_stego(int size, char *image_buffer) //encoding the size to the stego file
{
	int mask = (1 << 31);
	for(int i=0; i<32; i++)
	{
		image_buffer[i] = (image_buffer[i] & 0xfe) | ((size & mask) >> 31-i);  //modifying the lsb
		mask = mask >> 1;
	}
	return e_success;
}


Status encode_secret_file_extn(EncodeInfo *encinfo)
{
	encode_data_to_image(encinfo->extn_secret_file, strlen(encinfo->extn_secret_file), encinfo); //calling encode data function
	return e_success;
}


Status encode_secret_file_size(long file_size, EncodeInfo *encinfo) //encoding the secret file size to the stego file
{
	encode_secret_file_extn_size(file_size, encinfo);
	return e_success;
}


Status encode_secret_file_data(EncodeInfo *encinfo) //encoding the secret data to the stego file
{
	int size = encinfo->size_secret_file;
	char ch, arr[size];
	fseek(encinfo->fptr_secret, 0, SEEK_SET); //taking the ptr to 1st place
	fread(arr, size, 1, encinfo->fptr_secret);
	encode_data_to_image(arr, size, encinfo);
	return e_success;
}


Status copy_remaining_data(EncodeInfo *encinfo)  //copying the remaining content of the source file
{
	char ch;
	while(fread(&ch, 1, 1, encinfo->fptr_src_image))
	{
		fwrite(&ch, 1, 1, encinfo->fptr_stego_image);
	}
	return e_success;
}

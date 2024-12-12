/*
Name:Deepak Kumar Jha
Date:18/07/2022
Description:
Sample i/p:
sample o/p:
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

//checking the given data
OperationType read_and_validate_decode_args(char *argv[], DecodeInfo *decinfo)
{
	if(strstr(argv[2], ".") != NULL)	
	{
		if(strcmp(strstr(argv[2], "."), ".bmp") == 0)     //checking for .bmp extension
		{
			decinfo->stego_image_fname = argv[2];
		}
		else
			return e_failure;
	}
	else
	{
		return e_failure;
	}

	if(argv[3] != NULL)
	{
		if(strstr(argv[3], ".") != NULL) //checking for output file with .txt extension
		{
			if(strcmp(strstr(argv[3], "."), ".txt") == 0)
			{
				decinfo->secret_fname = argv[3];
			}
			else
				return e_failure;
		}
	}
	else
	{
		printf("INFO: Output file not mentioned. Creating decrypted.txt as default...\n");  
		sleep(1);
		decinfo->secret_fname = "decrypted.txt";
	}
	return e_success;
}


Status open_files_(DecodeInfo *decinfo)    //opening the required files for operation
{
	// Stego Image file
	decinfo->fptr_stego_image = fopen(decinfo->stego_image_fname, "r");

	// Do Error handling
	if (decinfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->stego_image_fname);

		return e_failure;
	}
	printf("INFO: Opened %s\n", decinfo->stego_image_fname);
	sleep(1);

	// Secret file
	decinfo->fptr_secret = fopen(decinfo->secret_fname, "w");
	// Do Error handling
	if (decinfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->secret_fname);

		return e_failure;
	}
	printf("INFO: Opened %s\n", decinfo->secret_fname);
	sleep(1);
	printf("INFO: Done Opened all required files.\n");
	sleep(1);

	// No failure return e_success
	return e_success;
}

//initiating the decode
void do_decoding(DecodeInfo *decinfo)
{
	printf("INFO: Opening required files.\n");
	sleep(1);
	if(open_files_(decinfo) == e_success)
	{
		printf("INFO: Decoding Magic String Signature.\n");
		sleep(1);
		if(decode_magic_string(decinfo) == e_success)  //decoding magic string
		{
			printf("INFO: Done\n");
			sleep(1);
			printf("INFO: Decoding extension size.\n");
			sleep(1);

			decode_extn_size(decinfo);  //decoding the extension size
			printf("INFO: Done\n");
			sleep(1);
			printf("INFO: Decoding extension data.\n");
			sleep(1);
			decode_extn_data(decinfo); //decoding the extension data like .txt
			printf("INFO: Done\n");
			sleep(1);

			printf("INFO: Decoding %s File size.\n", decinfo->secret_fname); 
			sleep(1);
			decode_secret_file_size(decinfo); //decoding the size of the secret file
			printf("INFO: Done\n");
			sleep(1);
			printf("INFO: Decoding %s File data.\n", decinfo->secret_fname);
			sleep(1);
			decode_secret_file_data(decinfo);//decoding secret file data
			printf("INFO: Done\n");
			sleep(1);

			printf("INFO:  Decoding Done Successfully \n");
		}
		else
			printf("Error: Magic String Is Not Encoded in %s\n", decinfo->stego_image_fname);

	}
	else
		puts("Error: opening files failed");
}


Status decode_magic_string(DecodeInfo *decinfo) //decoding "#*"
{
	int i,len=strlen(MAGIC_STRING);
	char str[len+1];
	fseek(decinfo->fptr_stego_image, 54, SEEK_SET); //skipping the first 54 bytes as it contains header data

	for(i=0; i<len; i++)
	{ 
		fread(decinfo->image_data, 8, 1, decinfo->fptr_stego_image);  //decoding 1 byte data from the 8byte data lsb
		decode_data_from_lsb(decinfo->image_data, i, str);
	}
	str[i]='\0';

	if(strcmp(str, MAGIC_STRING) == 0)  //checking for magic string
	{
		return e_success;
	}
	else
		return e_failure;
}


void decode_data_from_lsb(char *image_buffer, int id, char *str)  //decoding data from the lsb of the data
{
	char data=0x00;
	for(int i=0; i<8; i++)
	{
		data = data | ((image_buffer[i] & 1) << (7-i));
	}
	str[id] = data;
}


void decode_extn_size(DecodeInfo *decinfo) //decoding extension size
{
	decinfo->secret_file_extn_size = decode_size(decinfo);
}


uint decode_size(DecodeInfo *decinfo) //decoding the integer values
{
	uint i, size=0, length=sizeof(int)*8;
	char arr[length];
	fread(arr, length, 1, decinfo->fptr_stego_image);
	for(i=0; i<length; i++)
	{
		size = (size | (arr[i] & 1) << (31-i));
	}
	return size;
}

void decode_extn_data(DecodeInfo *decinfo) //decoding the extension of secret file
{
	int i,size=decinfo->secret_file_extn_size;
	char arr[size+1];
	for(i=0; i<size; i++)
	{
		fread(decinfo->image_data, 8, 1, decinfo->fptr_stego_image);
		decode_data_from_lsb(decinfo->image_data, i, arr);
	}
	arr[i]='\0';
}


void decode_secret_file_size(DecodeInfo *decinfo) //decoding secret file size
{
	decinfo->size_secret_file = decode_size(decinfo);
}


void decode_secret_file_data(DecodeInfo *decinfo) //decoding secret data
{
	int i, size=decinfo->size_secret_file;
	char arr[size];
	for(i=0; i<size; i++)
	{
		fread(decinfo->image_data, 8, 1, decinfo->fptr_stego_image);
		decode_data_from_lsb(decinfo->image_data, i, arr);
	}
	fwrite(arr, size, 1, decinfo->fptr_secret); //writing secret data to the given output file
}


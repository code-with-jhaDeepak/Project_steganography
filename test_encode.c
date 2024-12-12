/*
Name:Deepak Kumar Jha
Date:18/07/2022
Description:
Sample i/p:
sample o/p:
*/

//adding required header files
#include <stdio.h> 
#include <unistd.h>
#include "encode.h"
#include "decode.h"
#include "common.h"
#include "types.h"

int main(int argc, char *argv[])
{
	if(argc > 1)                      //checking for more than 1 command line argument
	{   
		if(check_operation_type(argv) == e_encode) //checking for encode command
		{
			if(argc > 3)
			{
				EncodeInfo encinfo;
				if(read_and_validate_encode_args(argv, &encinfo) == e_success)
				{
					do_encoding(&encinfo); //encoding the data
				}
				else
					puts("Error: validation failure");
			}
			else
				printf("%s: Encoding: %s -e <.bmp> <.txt> [output file]\n", argv[0], argv[0]);
		}

		else if((check_operation_type(argv) == e_decode) && (argc > 2)) //checking for decode command 
		{
			printf("INFO:  Decoding Procedure Started \n");
			sleep(1);
			DecodeInfo decinfo;
			if(read_and_validate_decode_args(argv, &decinfo) == e_success)
			{
				do_decoding(&decinfo);
			}
			else
				puts("Error: validation failure"); 
		}
		else
			puts("Error: usage");
	}
	else
	{
		printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n", argv[0], argv[0]); //throwing error for invalid usage
		printf("%s: Decoding: %s -d <.bmp file> [output file]\n", argv[0], argv[0]);
	}
	return 0;
}

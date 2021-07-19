
#include <stdio.h>  
#include <string.h>        
#include <stdlib.h>
#include "hex_file.h"

/*
\	Read a line in hex file return a value that means the end of file.
\	parameter: filename : link path file, buffer_line: buffer string stores read-line, length_line: length of buffer string
\	return 0 if end of file
\	else return 1
*/
int getLine(HANDLE filename, unsigned char* buffer_line, int* length_line)
{
	DWORD lpNumberOfBytesRead;       // the variable that receives the number of bytes read
	char char_temp[1] = {};			// a temp char to read 
	int index = 0;					// index of char in buffer line

	/* read a char until '\n' and number of bytes read is not 0 and store at buffer line*/
	do
	{
		bool check = ReadFile(filename,char_temp, 1, &lpNumberOfBytesRead, NULL); // read a char in file
		buffer_line[index] = char_temp[0];
		index++;
		*length_line = index;
	} while (char_temp[0] != '\n' && lpNumberOfBytesRead !=0);

	return lpNumberOfBytesRead;
	
}

/*
\	data filter from buffer line to send to com
\	parameter: buffer_line that is read from file, buffer data is stored  the data chars, and length of buffer data
\
*/
int  lineFilter(unsigned char* buffer_line, unsigned char* data,int *length_data)
{
	char numByte[2] ;		//buffer char to number of data byte in buffer line 
	char isData[2];			// buffer char to check a line that is a data line to boot

	memcpy(numByte, buffer_line + NUM_BYT_INX, 2);  
	memcpy(isData, buffer_line + DAT_BYT_INX, 2);

	/*check line is a data line to boot*/
	if (memcmp(isData, IS_DATA, 2) == 0)
	{
		if (memcmp(numByte, BYTE_2,2) == 0)			// if line has 2 data byte to boot
		{
			memcpy(data, buffer_line + 9, 4);
			*length_data = 4;
			return 4;
		}
		else if (memcmp(numByte, BYTE_4,2) == 0)	// if line has 4 data byte to boot
		{
			memcpy(data, buffer_line + 9, 8);
			*length_data = 8;
			return 8;
		}
		else if (memcmp(numByte, BYTE_8,2) == 0)	// if line has 8 data byte to boot
		{
			memcpy(data, buffer_line + 9, 16);
			*length_data = 16;
			return 16;
		}
		else if (memcmp(numByte, BYTE_16, 2) == 0)	// if line has 16 data byte to boot
		{
			memcpy(data, buffer_line + 9, 32);
			*length_data = 32;
			return 32;
		}

	}
	return 0;
}


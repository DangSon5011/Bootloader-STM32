#pragma once
#ifndef _HEX_FILE_H
#define _HEX_FILE_H
#include <Windows.h>

/*
/ define number of data bytes in a line in the hex file 
/ "02" : 2 byte in line
/ "04" : 4 byte in line
/ "08": 8 byte in line
/ "10" : 16 byte in line
*/
#define BYTE_2 "02"
#define BYTE_4	"04"
#define BYTE_8	"08"
#define BYTE_16	"10"

/*
\ define string check  hex data
\ IS_DATA: data to boot
\ END_HEX: end data
*/

#define IS_DATA	"00"
#define END_HEX	"05"

/*
\ number of bytes is not data to boot
*/
#define NOT_IS_DATA 13

/*num byte and data index in a line in a hex file*/
#define NUM_BYT_INX 1
#define DAT_BYT_INX 7
int getLine(HANDLE filename, unsigned char * buffer_line, int *length_line);
int lineFilter(unsigned char* buffer_line, unsigned char* data, int* length_data);

#endif // !_HEX_FILE_H

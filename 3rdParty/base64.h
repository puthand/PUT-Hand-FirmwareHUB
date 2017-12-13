﻿/*
	base64.c - by Joe DF (joedf@ahkscript.org)
	Released under the MIT License
	
	Revision: 2015-06-12 01:26:51
	
	Thank you for inspiration:
	http://www.codeproject.com/Tips/813146/Fast-base-functions-for-encode-decode
*/

#include "stm32f0xx.h"

// in : buffer of "raw" binary to be encoded.
// in_len : number of bytes to be encoded.
// out : pointer to buffer with enough memory, user is responsible for memory allocation, receives null-terminated string
// returns size of output including null byte
uint8_t b64_encode(uint8_t* in, uint8_t in_len, uint8_t* out);

// in : buffer of base64 string to be decoded.
// in_len : number of bytes to be decoded.
// out : pointer to buffer with enough memory, user is responsible for memory allocation, receives "raw" binary
// returns size of output excluding null byte
uint8_t b64_decode(const uint8_t* in, uint8_t in_len, uint8_t* out);

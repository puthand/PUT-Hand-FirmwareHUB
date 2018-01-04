/*
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

inline uint8_t b64_int(uint8_t ch) {

	// ASCII to base64_int
	// 65-90  Upper Case  >>  0-25
	// 97-122 Lower Case  >>  26-51
	// 48-57  Numbers     >>  52-61
	// 43     Plus (+)    >>  62
	// 47     Slash (/)   >>  63
	// 61     Equal (=)   >>  64~
	if (ch==43)
	return 62;
	if (ch==47)
	return 63;
	if (ch==61)
	return 64;
	if ((ch>47) && (ch<58))
	return ch + 4;
	if ((ch>64) && (ch<91))
	return ch - 'A';
	if ((ch>96) && (ch<123))
	return (ch - 'a') + 26;
	return 0;
}

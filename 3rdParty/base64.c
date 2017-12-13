/*
	base64.c - by Joe DF (joedf@ahkscript.org)
	Released under the MIT License
	
	See "base64.h", for more information.
	
	Thank you for inspiration:
	http://www.codeproject.com/Tips/813146/Fast-base-functions-for-encode-decode
*/

#include "base64.h"

uint8_t b64_int(uint8_t ch) ;

//Base64 char table - used internally for encoding
uint8_t b64_chr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint8_t b64_int(uint8_t ch) {

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


uint8_t b64_encode(uint8_t* in, uint8_t in_len, uint8_t* out) {

	unsigned int i=0, j=0, k=0, s[3];
	
	for (i=0;i<in_len;i++) {
		s[j++]=*(in+i);
		if (j==3) {
			out[k+0] = b64_chr[ s[0]>>2 ];
			out[k+1] = b64_chr[ ((s[0]&0x03)<<4)+((s[1]&0xF0)>>4) ];
			out[k+2] = b64_chr[ ((s[1]&0x0F)<<2)+((s[2]&0xC0)>>6) ];
			out[k+3] = b64_chr[ s[2]&0x3F ];
			j=0; k+=4;
		}
	}
	
	if (j) {
		if (j==1)
			s[1] = 0;
		out[k+0] = b64_chr[ s[0]>>2 ];
		out[k+1] = b64_chr[ ((s[0]&0x03)<<4)+((s[1]&0xF0)>>4) ];
		if (j==2)
			out[k+2] = b64_chr[ ((s[1]&0x0F)<<2) ];
		else
			out[k+2] = '=';
		out[k+3] = '=';
		k+=4;
	}

	out[k] = '\0';
	
	return k;
}

uint8_t b64_decode(const uint8_t* in, uint8_t in_len, uint8_t* out) {

	unsigned int i=0, j=0, k=0, s[4];
	
	for (i=0;i<in_len;i++) {
		s[j++]=b64_int(*(in+i));
		if (j==4) {
			out[k+0] = (s[0]<<2)+((s[1]&0x30)>>4);
			if (s[2]!=64) {
				out[k+1] = ((s[1]&0x0F)<<4)+((s[2]&0x3C)>>2);
				if ((s[3]!=64)) {
					out[k+2] = ((s[2]&0x03)<<6)+(s[3]); k+=3;
				} else {
					k+=2;
				}
			} else {
				k+=1;
			}
			j=0;
		}
	}
	
	return k;
}

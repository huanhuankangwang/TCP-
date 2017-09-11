#include <stdio.h>
#include <string.h>

#define      LOG(...)
/*base64 编解码*/

static const char *codes =   
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";  
  
static const unsigned char map[256] = {  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255,  
255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,  
 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,  
255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,  
  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  
 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,  
255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  
 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255 };

/*参数格式 len  是 in 的长度*/
int base64_encode(const unsigned char *in,  unsigned long len,
                        unsigned char *out)
{  
   unsigned long i, len2, leven;  
   unsigned char *p;  
   /* valid output size ? */  
   len2 = 4 * ((len + 2) / 3);  
   p = out;  
   leven = 3*(len / 3);  
   for (i = 0; i < leven; i += 3) {  
       *p++ = codes[in[0] >> 2];  
       *p++ = codes[((in[0] & 3) << 4) + (in[1] >> 4)];  
       *p++ = codes[((in[1] & 0xf) << 2) + (in[2] >> 6)];  
       *p++ = codes[in[2] & 0x3f];  
       in += 3;  
   }  
   /* Pad it if necessary...  */  
   if (i < len) {  
       unsigned a = in[0];  
       unsigned b = (i+1 < len) ? in[1] : 0;  
       unsigned c = 0;  
  
       *p++ = codes[a >> 2];  
       *p++ = codes[((a & 3) << 4) + (b >> 4)];  
       *p++ = (i+1 < len) ? codes[((b & 0xf) << 2) + (c >> 6)] : '=';  
       *p++ = '=';  
   }  
  
   /* append a NULL byte */  
   *p = '\0';  
  
   return p - out;  
}  
  
int base64_decode(const unsigned char *in, unsigned char *out)  
{  
    unsigned long t, x, y, z;  
    unsigned char c;  
    int g = 3;  
  
    for (x = y = z = t = 0; in[x]!=0;) {  
        c = map[in[x++]];  
        if (c == 255) return -1;  
        if (c == 253) continue;  
        if (c == 254) { c = 0; g--; }  
        t = (t<<6)|c;  
        if (++y == 4) {  
//          if (z + g > *outlen) { return CRYPT_BUFFER_OVERFLOW; }  
            out[z++] = (unsigned char)((t>>16)&255);  
            if (g > 1) out[z++] = (unsigned char)((t>>8)&255);  
            if (g > 2) out[z++] = (unsigned char)(t&255);  
            y = t = 0;  
        }  
    }  
//  if (y != 0) {  
//      return -1;  
//  }  
    return z;  
}

/* just fo test the base64 code */
/*
int main(int argc, char *argv[])
{
	char *str ="wangkang";
	char encode_str[20];
	int len;
	LOG(" %s will be encode\r\n",str);
	len = strlen(str);
	base64_encode(str,len,encode_str);
	
	LOG("the out of the encode_str is %s \r\n",encode_str);
	
	char *decode_str = "d2FuZ2thbmc=";
	char out_str[20]={0};
	base64_decode(decode_str,out_str);
	
	LOG("decode out_str is %s\r\n",out_str);
	
	int my = 1000;
	memset(out_str,0,sizeof(out_str));
	base64_encode(&my,sizeof(my),out_str);

	LOG(" out_str is %s  \r\n",out_str);
	
	int my_out =0;
	base64_decode(out_str,&my_out);
	
	LOG("decode my is %d \r\n",my_out);
}
*/
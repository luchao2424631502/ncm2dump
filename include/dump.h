#ifndef __NCMDUMP_H
#define __NCMDUMP_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

// 3rd lib
#include "aes.h"
#include "b64.h"
#include "cJSON.h"

#define DEBUG 0

#if DEBUG
#define px(x) printf("\tdebug_num=0x%x\n", x)
#define ps(x) printf("\tdebug_string=%s\n", x)
#define parray2(address, x, y) {\
	printf("\n");\
	for (size_t i = 0; i < x; i++) {\
		for (size_t j = 0; j < y; j++) {\
			printf("%.2x ", address[i][j]);\
		}\
		printf("\n");\
	}}
#define parray1(address, x) {\
	printf("\n");\
	for (size_t i = 0; i < x; i++) {\
		printf("%.2x ", address[i]);\
	}\
	printf("\n");\
}
#else // 去掉打印

#define nop() 			do{}while(0)
#define px(x) 			nop()
#define ps(x) 			nop()
#define parray2(address, x, y) 	nop()
#define parray1(address, x) 	nop()

#endif
typedef unsigned char uint8_t;
typedef unsigned int uint32_t; 

int work_convert(char *input_path);
uint8_t *dump(uint8_t *filedata, size_t filesize, 
		uint32_t *music_file_size, char **filename);
uint8_t *cut_first(uint8_t *data, uint32_t len, uint32_t cut_num);
uint8_t *pkcs7_unpad(uint8_t *data, uint32_t *len_addr);

#endif 

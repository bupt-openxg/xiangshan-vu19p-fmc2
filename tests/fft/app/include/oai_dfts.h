#ifndef __OAI_DFTS__H__
#define __OAI_DFTS__H__
#include <stdint.h>
#include "stdio.h"

void dft512(int16_t *x,int16_t *y,unsigned char scale);
void idft512(int16_t *x,int16_t *y,unsigned char scale);
void idft2048(int16_t *x,int16_t *y,unsigned char scale);
void test();

#endif
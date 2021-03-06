#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include "algorithms.h"

namespace algorithms{
	void print_hello_world() {
		std::cout << "Hello world, from algorithms!" << std::endl;
	}
	
	void xor_sad_salsa8(uint32_t B[16], const uint32_t Bx[16]){
		uint32_t x00, x01, x02, x03, x04, x05, x06, x07, x08, x09, x10, x11, x12, x13, x14, x15;
		int i;
		x00 = (B[ 0] ^= Bx[ 0]);
		x01 = (B[ 1] ^= Bx[ 1]);
		x02 = (B[ 2] ^= Bx[ 2]);
		x03 = (B[ 3] ^= Bx[ 3]);
		x04 = (B[ 4] ^= Bx[ 4]);
		x05 = (B[ 5] ^= Bx[ 5]);
		x06 = (B[ 6] ^= Bx[ 6]);
		x07 = (B[ 7] ^= Bx[ 7]);
		x08 = (B[ 8] ^= Bx[ 8]);
		x09 = (B[ 9] ^= Bx[ 9]);
		x10 = (B[10] ^= Bx[10]);
		x11 = (B[11] ^= Bx[11]);
		x12 = (B[12] ^= Bx[12]);
		x13 = (B[13] ^= Bx[13]);
		x14 = (B[14] ^= Bx[14]);
		x15 = (B[15] ^= Bx[15]);
		//  this is the sad part :(		
		B[ 0] += x00;
		B[ 1] += x01;
		B[ 2] += x02;
		B[ 3] += x03;
		B[ 4] += x04;
		B[ 5] += x05;
		B[ 6] += x06;
		B[ 7] += x07;
		B[ 8] += x08;
		B[ 9] += x09;
		B[10] += x10;
		B[11] += x11;
		B[12] += x12;
		B[13] += x13;
		B[14] += x14;
		B[15] += x15;
	}
	
	void xor_salsa8(uint32_t B[16], const uint32_t Bx[16]){
		uint32_t x[16];
		int i;
		x[ 0] = (B[ 0] ^= Bx[ 0]);
		x[ 1]= (B[ 1] ^= Bx[ 1]);
		x[ 2]= (B[ 2] ^= Bx[ 2]);
		x[ 3]= (B[ 3] ^= Bx[ 3]);
		x[ 4]= (B[ 4] ^= Bx[ 4]);
		x[ 5]= (B[ 5] ^= Bx[ 5]);
		x[ 6]= (B[ 6] ^= Bx[ 6]);
		x[ 7]= (B[ 7] ^= Bx[ 7]);
		x[ 8]= (B[ 8] ^= Bx[ 8]);
		x[ 9]= (B[ 9] ^= Bx[ 9]);
		x[10]= (B[10] ^= Bx[10]);
		x[11]= (B[11] ^= Bx[11]);
		x[12]= (B[12] ^= Bx[12]);
		x[13]= (B[13] ^= Bx[13]);
		x[14]= (B[14] ^= Bx[14]);
		x[15]= (B[15] ^= Bx[15]);
		#define R(a, b) (((a) << (b)) | ((a) >> (32-(b))))
		for(i = 0; i < 8; i +=2) {
		/* Operate on columns */
			x[ 4]^= R(x[ 0]+x[12], 7);	x[ 9]^= R(x[ 5]+x[ 1], 7);
			x[14]^= R(x[10]+x[ 6], 7);	x[ 3]^= R(x[15]+x[11], 7);
			
			x[ 8]^= R(x[ 4]+x[ 0], 9);	x[13]^= R(x[ 9]+x[ 5], 9);
			x[ 2]^= R(x[14]+x[10], 9);	x[ 7]^= R(x[ 3]+x[15], 9);
			
			x[12]^= R(x[ 8]+x[ 4],13);	x[ 1]^= R(x[13]+x[ 9],13);
			x[ 6]^= R(x[ 2]+x[14],13);	x[11]^= R(x[ 7]+x[ 3],13);

			x[ 0]^= R(x[12]+x[ 8],18);	x[ 5]^= R(x[ 1]+x[13],18);
			x[10]^= R(x[ 6]+x[ 2],18);	x[15]^= R(x[11]+x[ 7],18);
			
		/* Operate on rows */
			x[ 1]^= R(x[ 0]+x[ 3], 7);	x[ 6]^= R(x[ 5]+x[ 4], 7);
			x[11]^= R(x[10]+x[ 9], 7);	x[12]^= R(x[15]+x[14], 7);
			
			x[ 2]^= R(x[ 1]+x[ 0], 9);	x[ 7]^= R(x[ 9]+x[ 5], 9);
			x[ 8]^= R(x[11]+x[10], 9);	x[13]^= R(x[12]+x[15], 9);
			
			x[ 3]^= R(x[ 2]+x[ 1],13);	x[ 4]^= R(x[ 7]+x[ 6],13);
			x[ 9]^= R(x[ 8]+x[11],13);	x[14]^= R(x[13]+x[12],13);

			x[ 0]^= R(x[ 3]+x[ 2],18);	x[ 5]^= R(x[ 4]+x[ 7],18);
			x[10]^= R(x[ 9]+x[ 8],18);	x[15]^= R(x[14]+x[13],18);
			for(int j = 0; j < 16; j++){
	  //printf("(%i,%i): %x\n",i,j,x[j]);
    }
		}
		#undef R
		B[ 0] += x[ 0];
		B[ 1] += x[ 1];
		B[ 2] += x[ 2];
		B[ 3] += x[ 3];
		B[ 4] += x[ 4];
		B[ 5] += x[ 5];
		B[ 6] += x[ 6];
		B[ 7] += x[ 7];
		B[ 8] += x[ 8];
		B[ 9] += x[ 9];
		B[10] += x[10];
		B[11] += x[11];
		B[12] += x[12];
		B[13] += x[13];
		B[14] += x[14];
		B[15] += x[15];
	}
}
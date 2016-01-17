#include <common-iop.h>
#include <stdio.h>
#include <tamtypes.h>

void enableGte();
void disableGte();
void loadAverageCol(unsigned char in1[3], unsigned char in2[3], long weight1, long weight2, unsigned char out[3]);

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	enableGte();
	
	unsigned char inColor1[3] = { 0x00, 0xCC, 0x00 };
	unsigned char inColor2[3] = { 0xAA, 0x00, 0xAA };
	unsigned char outColor1[3];
	unsigned char outColor2[3];

	loadAverageCol(inColor1, inColor2, 0x1000, 0x1000, outColor1);
	printf("outColor1: %x, %x, %x\n", 
		outColor1[0], outColor1[1], outColor1[2]);
	
	loadAverageCol(inColor1, inColor2, 0x800, 0x800, outColor2);
	printf("outColor2: %x, %x, %x\n", 
		outColor2[0], outColor2[1], outColor2[2]);

	disableGte();
	
	printf("-- TEST END\n");
	
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned short getBit(char *data, unsigned int bitOffset) {
	unsigned short bitFlag;

	// Calculate bitOffset/(sizeof(char)*8) and get quot and rem
	div_t d = div(bitOffset, sizeof(char) * 8);

	// Set offsets
	int byteOffset = d.quot;
	int bitPosition = sizeof(char) * 8 - d.rem + 1;

	unsigned char byteValue = data[byteOffset];

	unsigned char bitValue = (byteValue >> bitPosition) & 1;

	if (bitValue == 0) {
		bitFlag = 0;
	} else if (bitValue == 1) {
		bitFlag = 1;
	} else {
		bitFlag = -1;
	}

	return bitFlag;
}

int main(void) {
	int i;
	int j;
	int k;

	char *x = (char *) calloc(12, sizeof(char));
	// memset(x, 3, 10);

	unsigned int *y;
	y = (unsigned int *) x;
	y[0] = 8192 + 2048 + 1024 + 32 + 16 + 1;
	y[1] = 50;
	y[2] = 42;

	// x = (char *) y;

	// sprintf(x, "%x", y);
	// int z = atoi(x);

	printf("sizeof_char=%lu\n", sizeof(char));
	printf("sizeof_short=%lu\n", sizeof(short));
	printf("sizeof_int=%lu\n", sizeof(int));
	printf("sizeof_long=%lu\n", sizeof(long));
	printf("sizeof_unsigned_int=%lu\n", sizeof(unsigned int));

	printf("x=%s\n", x);

	for (i = 0; i < 10; i++) {
		printf("x[%d]=%c\n", i, x[i]);
	}

	printf("strlen(x)=%lu\n", strlen(x));
	printf("y[0]=%d\n", y[0]);
	printf("y[1]=%d\n", y[1]);
	printf("y[2]=%d\n", y[2]);
	// printf("z=%d\n", z);

	unsigned short flag;
	for (i = 0; i < 10; i++) {
		for (j = 1; j <= sizeof(char) * 8; j++) {
			k = (x[i] >> (8 - j)) & 1;
			printf("%d", k);
		}
	}

	printf("\n");

	free(x);
}

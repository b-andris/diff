//
//  main.c
//  diff
//
//  Created by Benjamin Andris Suter-Dörig on 30/01/15.
//  Copyright (c) 2015 Benjamin Andris Suter-Dörig. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "diff.h"

void output(diffComponent** diff) {
	int i = 0;
	while (diff[i]) {
		if (isUnchanged(diff[i])) {
			printf("\e[30;48m%s\e[m",diff[i]->from);
		} else {
			printf("\e[38;41m%s\e[m",diff[i]->from);
			printf("\e[38;42m%s\e[m",diff[i]->to);
		}
		i++;
	}
	puts("\n\n");
}

int main(int argc, const char * argv[]) {
	if (argc != 3) {
		puts("Usage: diff path/to/file/1 path/to/file/2\n\n");
		exit(EXIT_FAILURE);
	}
	FILE* fromFile = fopen(argv[1], "rb");
	if (!fromFile) {
		printf("ERROR: File %s Invalid\n\n",argv[1]);
		exit(EXIT_FAILURE);
	}
	FILE* toFile = fopen(argv[2], "rb");
	if (!toFile) {
		printf("ERROR: File %s Invalid\n\n",argv[2]);
		fclose(fromFile);
		exit(EXIT_FAILURE);
	}
	fseek(fromFile, 0, SEEK_END);
	fseek(toFile, 0, SEEK_END);
	long fromLength = ftell(fromFile);
	long toLength = ftell(toFile);
	fseek(fromFile, 0, SEEK_SET);
	fseek(toFile, 0, SEEK_SET);
	char* fromBuffer = malloc(fromLength+1);
	char* toBuffer = malloc(toLength+1);
	if (!(fromBuffer && toBuffer)) {
		free(fromBuffer);
		free(toBuffer);
		puts("ERROR: Memory Allocation Failed\n\n");
		exit(EXIT_FAILURE);
	}
	fromBuffer[fromLength] = '\0';
	toBuffer[toLength] = '\0';
	fread(fromBuffer, fromLength, 1, fromFile);
	fread(toBuffer, toLength, 1, toFile);
	output(diff(fromBuffer, toBuffer));
	free(fromBuffer);
	free(toBuffer);
	fclose(fromFile);
	fclose(toFile);
}

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
			printf("\e[0m%s\e[m",diff[i]->from);
		} else {
			printf("\e[41m%s\e[m",diff[i]->from);
			printf("\e[42m%s\e[m",diff[i]->to);
		}
		i++;
	}
	puts("\n\n");
}

int main(int argc, const char * argv[]) {
	if (argc != 3) {
		puts("\e[31mUsage: diff path/to/file/1 path/to/file/2\n\n\e[m");
		exit(EXIT_FAILURE);
	}
	FILE* fromFile = fopen(argv[1], "r");
	if (!fromFile) {
		printf("\e[31mERROR: File %s Invalid\n\n\e[m",argv[1]);
		exit(EXIT_FAILURE);
	}
	FILE* toFile = fopen(argv[2], "r");
	if (!toFile) {
		printf("\e[31mERROR: File %s Invalid\n\n\e[m",argv[2]);
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
		puts("\e[31mERROR: Memory Allocation Failed\n\n\e[m");
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

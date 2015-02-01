//
//  diff.c
//  Comment Diff
//
//  Created by Benjamin Andris Suter-Dörig on 25/01/15.
//  Copyright (c) 2015 Benjamin Andris Suter-Dörig. All rights reserved.
//

#include "diff.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int getDistance(const char* from,const char* to,int* cache,unsigned long fromOffset,unsigned long toOffset,unsigned long fromLength,unsigned long toLength);

unsigned long getStartOffset(char const *from, char const *to, unsigned long fromLength, unsigned long toLength);

unsigned long getEndOffset(char const *from, char const *to, unsigned long fromLength, unsigned long toLength);

diffComponent **prependDiffComponentToArray(diffComponent *fistComponent, int *componentCount, diffComponent **result);

bool shouldUseShortenedFrom(char const *from, char const *to, int *cache, unsigned long fromOffset, unsigned long toOffset, unsigned long fromLength, unsigned long toLength);

bool shouldRegardCharacterAsUnchanged(char const *from, char const *to, int *cache, unsigned long fromOffset, unsigned long toOffset, unsigned long fromLength, unsigned long toLength, int *componentCount, diffComponent **prevBackTrack);

bool isUnchanged(diffComponent* diffComponent) {
	return diffComponent->from == diffComponent->to;
}

void freeComponent(diffComponent* component) {
	if (!isUnchanged(component)) free((void*)component->from);
	free((void*)component->to);
	free(component);
}
void freeComponents(diffComponent** components){
	unsigned int i = 0;
	while (components[i]) freeComponent(components[i++]);
	free(components);
}

char* substringFromIndexWithLength(const char* str,unsigned long index,unsigned long length){
	char*subStr = strncpy(malloc((length+1)*sizeof(char)),&str[index],length);
	subStr[length] = '\0';
	return subStr;
}

diffComponent* makeDiffComponentForUnchanged(const char* c) {
	diffComponent* d = malloc(sizeof(diffComponent));
	d->from = d->to = c;
	return d;
}

diffComponent* makeDiffComponent(const char* from,const char* to) {
	diffComponent* d = malloc(sizeof(diffComponent));
	d->from = from;
	d->to = to;
	return d;
}

diffComponent** makeNullTerminateDiffComponentArrayWithCapacity(unsigned long length){
	diffComponent** ret = malloc((length+1)*sizeof(diffComponent*));
	ret[length] = NULL;
	return ret;
}

int getDistanceForEqualChars(char const *from, char const *to, int *cache, unsigned long fromOffset, unsigned long toOffset, unsigned long fromLength, unsigned long toLength, unsigned long fromIndex, unsigned long toIndex) {
	int prevDistance = getDistance(from, to, cache, fromOffset + 1, toOffset + 1, fromLength, toLength);
	cache[toIndex*fromLength+fromIndex] = prevDistance + 1;
	return cache[toIndex*fromLength+fromIndex];
}

int getDistanceForDifferentChars(char const *from, char const *to, int *cache, unsigned long fromOffset, unsigned long toOffset, unsigned long fromLength, unsigned long toLength, unsigned long fromIndex, unsigned long toIndex) {
	int shortenedToDistance = getDistance(from, to, cache, fromOffset, toOffset + 1, fromLength, toLength);
	int shortenedFromDistance = getDistance(from, to, cache, fromOffset + 1, toOffset, fromLength, toLength);
	int prevDistance = MAX(shortenedToDistance, shortenedFromDistance);
	cache[toIndex*fromLength+fromIndex] = prevDistance;
	return cache[toIndex*fromLength+fromIndex];
}

int getDistance(const char* from,const char* to,int* cache,unsigned long fromOffset,unsigned long toOffset,unsigned long fromLength,unsigned long toLength){
	unsigned long fromIndex = fromLength-fromOffset - 1;
	unsigned long toIndex = toLength-toOffset - 1;
	if (fromOffset == fromLength || toOffset == toLength) return 0;
	else if (cache[toIndex*fromLength+fromIndex] != -1) return cache[toIndex*fromLength+fromIndex];
	else if (from[fromIndex] == to[toIndex]) return getDistanceForEqualChars(from, to, cache, fromOffset, toOffset, fromLength, toLength, fromIndex, toIndex);
	else return getDistanceForDifferentChars(from, to, cache, fromOffset, toOffset, fromLength, toLength, fromIndex, toIndex);
}

char* stringByAppendingChar(const char* oldStr,char c){
	unsigned long strLen = strlen(oldStr);
	char* str = memcpy(malloc((strLen+2)*sizeof(char)), oldStr, strLen);
	str[strLen] = c;
	str[strLen+1] = '\0';
	return str;
}

char* oneCharString(char c) {
	char* str = malloc(2*sizeof(char));
	str[0] = c;
	str[1] = '\0';
	return str;
}

diffComponent**enlargeNullTerminatedDiffComponentArray(diffComponent **arr, int *componentCount){
	arr = realloc(arr, (*componentCount+2)*sizeof(diffComponent*));
	arr[*componentCount + 1]=NULL;
	*componentCount += 1;
	return arr;
}

diffComponent** appendUnchangedCharToBacktrack(char c, diffComponent** backtrack,int* componentCount){
	if (*componentCount && isUnchanged(backtrack[*componentCount-1])) {
		char* str = stringByAppendingChar(backtrack[*componentCount-1]->from, c);
		free((void*)backtrack[*componentCount-1]->from);
		backtrack[*componentCount-1]->from = backtrack[*componentCount-1]->to = str;
		return backtrack;
	} else {
		backtrack = enlargeNullTerminatedDiffComponentArray(backtrack, componentCount);
		backtrack[*componentCount-1]=makeDiffComponentForUnchanged(oneCharString(c));
		return backtrack;
	}
}

diffComponent** appendFromCharToBacktrack(char c, diffComponent** backtrack,int* componentCount){
	if (*componentCount && !isUnchanged(backtrack[*componentCount-1])) {
		char* str = stringByAppendingChar(backtrack[*componentCount-1]->from, c);
		free((void*)backtrack[*componentCount-1]->from);
		backtrack[*componentCount-1]->from = str;
		return backtrack;
	} else {
		backtrack = enlargeNullTerminatedDiffComponentArray(backtrack, componentCount);
		backtrack[*componentCount-1]=makeDiffComponent(oneCharString(c), calloc(1, sizeof(char)));
		return backtrack;
	}
}

diffComponent** appendToCharToBacktrack(char c, diffComponent** backtrack,int* componentCount){
	if (*componentCount && !isUnchanged(backtrack[*componentCount-1])) {
		char* str = stringByAppendingChar(backtrack[*componentCount-1]->to, c);
		free((void*)backtrack[*componentCount-1]->to);
		backtrack[*componentCount-1]->to = str;
		return backtrack;
	} else {
		backtrack = enlargeNullTerminatedDiffComponentArray(backtrack, componentCount);
		backtrack[*componentCount-1]=makeDiffComponent(calloc(1, sizeof(char)),oneCharString(c));
		return backtrack;
	}
}

diffComponent **makeDiffComponentArrayFromComponent(int *componentCount, diffComponent *component) {
	*componentCount = 1;
	diffComponent** result = makeNullTerminateDiffComponentArrayWithCapacity(1);
	result[0]=component;
	return result;
}

diffComponent **makeEmptyDiffComponentArray(int *componentCount) {
	*componentCount = 0;
	return makeNullTerminateDiffComponentArrayWithCapacity(0);
}

diffComponent** internalDiff(const char* from,const char* to,int* cache,unsigned long fromOffset,unsigned long toOffset,unsigned long fromLength,unsigned long toLength,int* componentCount) {
	if (fromOffset == fromLength || toOffset == toLength) {
		if (fromOffset == fromLength && toOffset == toLength) return makeEmptyDiffComponentArray(componentCount);
		else return makeDiffComponentArrayFromComponent(componentCount, makeDiffComponent(substringFromIndexWithLength(from, 0, fromLength-fromOffset), substringFromIndexWithLength(to, 0, toLength-toOffset)));
	}
	unsigned long fromIndex = fromLength-fromOffset - 1;
	unsigned long toIndex = toLength-toOffset - 1;
	if (from[fromIndex] == to[toIndex]) {
		diffComponent** prevBackTrack = internalDiff(from, to, cache, fromOffset + 1, toOffset + 1,fromLength,toLength,componentCount);
		if (shouldRegardCharacterAsUnchanged(from, to, cache, fromOffset, toOffset, fromLength, toLength, componentCount, prevBackTrack)) return appendUnchangedCharToBacktrack(from[fromIndex],prevBackTrack, componentCount);
		else freeComponents(prevBackTrack);
	}
	if (shouldUseShortenedFrom(from, to, cache, fromOffset, toOffset, fromLength, toLength)) return appendFromCharToBacktrack(from[fromIndex], internalDiff(from, to, cache, fromOffset + 1, toOffset,fromLength,toLength,componentCount), componentCount);
	else return appendToCharToBacktrack(to[toIndex], internalDiff(from, to, cache, fromOffset, toOffset + 1,fromLength,toLength,componentCount), componentCount);
}

bool shouldRegardCharacterAsUnchanged(char const *from, char const *to, int *cache, unsigned long fromOffset, unsigned long toOffset, unsigned long fromLength, unsigned long toLength, int *componentCount, diffComponent **prevBackTrack) {
	return !*componentCount || isUnchanged(prevBackTrack[*componentCount-1]) || (getDistance(from,to, cache,fromOffset,toOffset,fromLength,toLength) > getDistance(from,to,cache,fromOffset+1,toOffset,fromLength,toLength) && getDistance(from,to,cache,fromOffset,toOffset,fromLength,toLength) > getDistance(from,to,cache,fromOffset,toOffset+1,fromLength,toLength));
}

bool shouldUseShortenedFrom(char const *from, char const *to, int *cache, unsigned long fromOffset, unsigned long toOffset, unsigned long fromLength, unsigned long toLength) {
	return getDistance(from,to,cache,fromOffset+1,toOffset,fromLength,toLength) >= getDistance(from,to,cache,fromOffset,toOffset+1,fromLength,toLength);
}

diffComponent **makeResultForEmptyString(char const *from, char const *to, unsigned long fromLength, unsigned long toLength) {
	diffComponent** ret = makeNullTerminateDiffComponentArrayWithCapacity(1);
	if (fromLength == 0 && toLength == 0) ret[0] = makeDiffComponentForUnchanged(calloc(1,sizeof(char)));
		else ret[0] = makeDiffComponent(substringFromIndexWithLength(from,0,fromLength),substringFromIndexWithLength(to,0,toLength));
	return ret;
}

diffComponent **makeResultForPrefix(char const *from, char const *to, unsigned long fromLength, unsigned long toLength, unsigned long startOffset) {
	diffComponent** ret = makeNullTerminateDiffComponentArrayWithCapacity(2);
	ret[0] = makeDiffComponentForUnchanged(substringFromIndexWithLength(from,0,startOffset));
	ret[1] = makeDiffComponent(substringFromIndexWithLength(from,startOffset,fromLength-startOffset),substringFromIndexWithLength(to,startOffset,toLength-startOffset));
	return ret;
}

diffComponent **appendDiffComponentToArray(int componentCount, diffComponent **result, diffComponent *component) {
	result = realloc(result, (componentCount+2)*sizeof(diffComponent*));
	result[componentCount] = component;
	result[componentCount+1] = NULL;
	return result;
}

void freeMultiple(int count, ...) {
	va_list args;
	va_start(args, count);
	for (int i = 0; i < count; i++) free(va_arg(args, void*));
	va_end(args);
}

diffComponent** diff(const char*from, const char*to) {
	if (!from || !to) return NULL;
	unsigned long fromLength = strlen(from);
	unsigned long toLength = strlen(to);
	if (fromLength == 0 || toLength == 0) return makeResultForEmptyString(from, to, fromLength, toLength);
	unsigned long startOffset = getStartOffset(from, to, fromLength, toLength);
	unsigned long endOffset = 0;
	if (startOffset < fromLength && startOffset < toLength) endOffset = getEndOffset(from, to, fromLength, toLength);
	else return makeResultForPrefix(from, to, fromLength, toLength, startOffset);
	if (startOffset + endOffset > MIN(fromLength, toLength)) endOffset = MIN(fromLength, toLength) - startOffset;
	char* fromSubstring = substringFromIndexWithLength(from,startOffset,fromLength-startOffset-endOffset);
	char* toSubstring = substringFromIndexWithLength(to,startOffset,toLength-startOffset-endOffset);
	int* cache = malloc((fromLength-startOffset-endOffset)*(toLength-startOffset-endOffset)*sizeof(int));
	memset(cache, -1, (fromLength-startOffset-endOffset)*(toLength-startOffset-endOffset)*sizeof(int));
	int componentCount = 0;
	diffComponent** result = internalDiff(fromSubstring,toSubstring, cache,0,0,fromLength-startOffset-endOffset,toLength-startOffset-endOffset,&componentCount);
	freeMultiple(3,cache,fromSubstring,toSubstring);
	if (startOffset) result = prependDiffComponentToArray(makeDiffComponentForUnchanged(substringFromIndexWithLength(from, 0, startOffset)), &componentCount, result);
	if (endOffset) result = appendDiffComponentToArray(componentCount, result, makeDiffComponentForUnchanged(substringFromIndexWithLength(from, fromLength-endOffset, endOffset)));
	return result;
}

diffComponent **prependDiffComponentToArray(diffComponent *fistComponent, int *componentCount, diffComponent** result) {
	diffComponent** newResult = malloc(((*componentCount) +2)*sizeof(diffComponent*));
	memcpy(&newResult[1], result, ((*componentCount) +1)*sizeof(diffComponent*));
	newResult[0] = fistComponent;
	free(result);
	(*componentCount)++;
	return newResult;
}

unsigned long getEndOffset(char const *from, char const *to, unsigned long fromLength, unsigned long toLength) {
	unsigned long endOffset = 0;
	while (endOffset < MIN(fromLength, toLength)) {
			if (from[fromLength-endOffset-1] == to[toLength-endOffset-1]) endOffset++;
			else break;
		}
	return endOffset;
}

unsigned long getStartOffset(char const *from, char const *to, unsigned long fromLength, unsigned long toLength) {
	unsigned long startOffset = 0;
	while (startOffset < MIN(fromLength, toLength)) {
		if (from[startOffset] == to[startOffset]) startOffset++;
		else break;
	}
	return startOffset;
}

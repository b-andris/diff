//
//  diff.h
//  Comment Diff
//
//  Created by Benjamin Andris Suter-Dörig on 25/01/15.
//  Copyright (c) 2015 Benjamin Andris Suter-Dörig. All rights reserved.
//

#ifndef __Comment_Diff__diff__
#define __Comment_Diff__diff__

#include <stdio.h>
#include <stdbool.h>

struct diffComponent_t {
	const char* from;
	const char* to;
};

typedef struct diffComponent_t diffComponent;

bool isUnchanged(diffComponent* diffComponent);

void freeComponent(diffComponent* component);
void freeComponents(diffComponent** components);

diffComponent** diff(const char*from, const char*to);

#endif /* defined(__Comment_Diff__diff__) */

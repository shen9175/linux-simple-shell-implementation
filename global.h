/*
 *  global.h
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */
#ifndef GLOBAL_H
#define GLOBAL_H

FILE *infile; //global file stream pointer
FILE *historyfile;

struct aliaslist {
	char* name;
	char* word;
	struct aliaslist* next;
}; 
typedef struct aliaslist* aliasptr;

aliasptr first;

char** history;
int h; //history command number
int hismaxsize;

#endif


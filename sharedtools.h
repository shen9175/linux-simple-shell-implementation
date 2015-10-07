/*
 *  sharedtools.h
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */
#ifndef SHAREDTOOLS_H
#define SHAREDTOOLS_H

int issubstring(char*, char* );
int isnum(char*);
int replacelength(char*,char*,char*);
void replace(char**,char*,char*);
void parser(char*,char***); // the command parser

#endif


/*
 *  alias.h
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */
#ifndef ALIAS_H
#define ALIAS_H

void setalias(char*);
void setalias(char* );
void printalias(char*);
void addalias(char*, char*);
char* findalias(char*);
char* getalias(char*);
void printallalias();
void unalias(char*);
void deletealias(char*);
void deleteallalias();

#endif

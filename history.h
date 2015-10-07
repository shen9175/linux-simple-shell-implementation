/*
 *  history.h
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */
#ifndef HISTORY_H
#define HISTORY_H

void sethistory(char*);
void addhistory(char*);
void readhistory();
void storehistory();
void printhistory();
void clearhistory();
void lasthistory(char**);
void historycount(char*);
char* retrievehistory(char*);
void historysub(char*,char**);

#endif


/*
 *  control.h
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */

#ifndef CONTROL_H
#define CONTROL_H

void readline(char**,int);
int prompt(char**,char***,int);  // the shell prompt
void rcfile();  // process rc file
int buildincheck(char**,char***);  //check if it is buildin command
void control(int); //the program hub which control buildin, external commands and exit
void callexternal(char**); //use fork to duplicate a process and execute the external command in this duplicated process
void setcd(char**);

#endif


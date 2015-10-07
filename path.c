/*
 *  path.c
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */
#include <stdlib.h>
#include <string.h>
#include "path.h"



void setpath(char* command)
{
	//squzee the whitespace in command line
	//i.e. PATH = /bin: /usr/bin : ./ -->PATH=/bin:/usr/bin./
	char* squzeed=malloc(1*sizeof(char));
	int i=0;
	while (*command!='\0') {
		if (*command==' '||*command=='\t') {
			command++;
		}
		else
		{	
			i++;
			squzeed=realloc(squzeed, (i+1)*sizeof(char));
			squzeed[i-1]=*command++;
		}
	}
	squzeed[i]='\0';
	
	squzeed=squzeed+strlen("PATH=");//cut off "PATH="
	setenv("PATH",squzeed,5);
	
	//go back to the front for memory releasing
	squzeed=squzeed-strlen("PATH=");
	free(squzeed); //release memory
	squzeed=NULL;
}

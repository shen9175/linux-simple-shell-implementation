/*
 *  sharedtools.c
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sharedtools.h"

int issubstring(char* string, char* substring )
{
	char *p;
	p=strstr(string, substring);
	if (p==NULL) {
		return 0;
	}else {
		return 1;
	}
	
}

int isnum(char*a)
{
	int i=0;
	int b=1;
	while (a[i]!='\0') 
	{
		if(a[i]!='-')
		{	
			b=b*isdigit(a[i]);
		}
		i++;
	}
	return b;
}

int replacelength(char* command,char*string1,char*string2)
{
	int n=0;
	char* p;
	char* comm=command;
	while ((p=strstr(command, string1))!=NULL) {
		p=p+strlen(string1);
		n++;
		command=p;
	} 
	return (strlen(comm)-n*strlen(string1)+n*strlen(string2));
	
}

void replace(char** command, char* string1,char* string2)
{
	char* p;
	char* head;
	char* tail;
	char* middle;
	char* comm=*command;
	char* new;
	new=malloc(replacelength(*command, string1,string2)*sizeof(char));
	*new='\0';
	while((p=strstr(comm, string1))!=NULL)
	{
		head=malloc((p-comm)*sizeof(*head));
		strncpy(head,comm,(p-comm));
		head[p-comm]='\0';
		middle=string2;
		tail=p+strlen(string1);
		comm=tail;
		new=strcat(new,head);
		new=strcat(new,middle);
		free(head);
		head=NULL;
	}
	new=strcat(new,tail);
	**command='\0';
	strcpy(*command,new);
	free(new);
	new=NULL;
	
	
}


void parser(char * comm,char*** option) //put the whole command line into option vector
{
	
	int i=0;
	
	//since this operation will change char in command array, 
	//it's better make a copy, and change the copy
	
	char* command=malloc((strlen(comm)+1)*sizeof(char));
	strcpy(command,comm);
	char* start=command; //backup the header of string
	
	
	//check if the quotation mark is even number
	int even=0;
	while (*command!='\0') {
		if (*command=='"') {
			even++;
		}
		command++;
	}
	command=start;
	
	//parsing
	while (*command != '\0')   // if not the end of line ....... 
	{       
		while (*command == ' ' || *command == '\t')
			*command++ = '\0';     // replace white spaces with 0  
		if (strlen(command)!=0)//skip last space if the command has last space 
		{
			i++;
			*option=realloc(*option,(i+1)*sizeof(char*));
			(*option)[i-1] =command;       //save the argument position
		}
		
		while (*command != '\0' && *command != ' ' &&  *command != '\t') 
		{
			// if this is quotation in the sentence like: alias dir="ls -aF"
			// skip all the chars in the quote
			if (*command=='"')
			{
				if (even%2==0) {
					do {
						command++;
					} while (*command!='"');
				}
				
			}
			command++;          //skip the argument until ...  
			
		}
		
	}
	(*option)[i] = '\0';       //mark the end of argument list 
	
	// allocate memory space to **option
	i=0;
	while ((*option)[i]!='\0')
	{
		char *tmp;
		tmp=strdup((*option)[i]);
		(*option)[i]=strdup(tmp);
		free(tmp);
		tmp=NULL;
		i++;
	}
	command=start;
	free(command);
	command=NULL;
	
}



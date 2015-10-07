/*
 *  history.c
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sharedtools.h"
#include "global.h"
#include "history.h"

void sethistory(char* command)
{
	char** hisoption;
	hisoption=malloc(1*sizeof(char*));
	parser(command, &hisoption);
	
	if (strcmp(hisoption[0],"history")==0 )
	{
		if (hisoption[1]=='\0') 
			printhistory();
		else if(strcmp(hisoption[1],"-c")==0)
			clearhistory();
		else 
			lasthistory(hisoption);
	}
	else 
	{
		historycount(command);
	}
	
	//release memory space
	int i=0;
	while (hisoption[i]!='\0')
	{
		free(hisoption[i]);
		hisoption[i]=NULL;
		i++;
	}		
	
	free(hisoption);
	hisoption=NULL;
}

void printhistory()
{
	int i;
	for (i=0; i<h; i++) {
		printf("  %d  %s\n",i+1,history[i]);
	}
}

void clearhistory()
{
	int i;
	for (i=h-1; i>=0; i--) 
	{
		free(history[i]);
		history[i]=NULL;
	}
	h=0;	
}



void lasthistory(char** option)
{
	if (isnum(option[1])==0)
	{
		printf("history: %s: numeric argument required\n",option[1]);
	}
	else if (atoi(option[1])<0)
	{
		printf("history: %s: invalid option\n",option[1]);
		printf("usage: history [-c] [-d offset] [n] or history -awrn [filename] or history -ps arg [arg...]\n");
	}
	
	
	else //print last n history commands;
	{
		
		int i;
		int last=atoi(option[1]);
		if (last>h-1) {
			printhistory();
		}
		else
		{
			for (i=h-last; i<h; i++)
			{
				printf("  %d  %s\n",i+1,history[i]);
			}
		}
	}	
}

void historycount(char* command)
{
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
	
	squzeed=squzeed+strlen("HISTORYCOUNT=");
	if (isnum(squzeed)==0) {
		printf("HISTORYCOUNT must be a number!\n");
	}
	else {
		hismaxsize=atoi(squzeed);
	}
	
	squzeed=squzeed-strlen("HISTORYCOUNT=");//go back to the front
	free(squzeed); //release memory
	squzeed=NULL;
	//if the max size reduced less than current, then resize the history array in memory
	
	
	if(h>hismaxsize)
	{
		
		char** temp=malloc(hismaxsize*sizeof(char*));
		int j=0;
		//store the last number(max) of command into temp
		for (i=h-hismaxsize; i<h; i++) 
		{
			temp[j]=strdup(history[i]);
			j++;
		}
		h=hismaxsize;
		
		//free the whole array
		for (i=0; i<h; i++) {
			free(history[i]);
			history[i]=NULL;
		}
		history=realloc(history, hismaxsize*sizeof(char*));
		for (i=0; i<hismaxsize; i++) 
		{
			history[i]=strdup(temp[i]);
			free(temp[i]);
			temp[i]=NULL;
		}
		free(temp);
		temp=NULL;
		
	}
}

void readhistory()
{	char temp[10000];
	historyfile=fopen(".sis_history", "r");
	if (historyfile==NULL) 
	{
		h=0;
	}
	else 
	{
		h=0;
		while (!feof(historyfile)) {
			history=realloc(history, (h+1)*sizeof(char*));
			fscanf(historyfile,"%s",temp);
			history[h]=strdup(temp);
			h++;
		}
		fclose(historyfile);
	}
	
}

void addhistory(char* command)
{
	int i;
	
	if (h==hismaxsize) 
	{		
		for (i=0; i<(h-1); i++) 
		{
			free(history[i]);
			history[i]=strdup(history[i+1]);
		}
		
		history[h-1]=strdup(command);
	}	
	else if(h<hismaxsize)
	{
		history=realloc(history, (h+1)*sizeof(char*));
		history[h]=strdup(command);
		h++;   //h points to new empty locatin in history array
	}
	
}

void storehistory()
{
	int i;
	historyfile=fopen(".sis_history", "w");
	if (historyfile==NULL) 
	{
		printf("Error in writing history file!");
	}
	else 
	{
		
		for (i=0; i<h; i++)
		{
			fprintf(historyfile, "%s\n",history[i]);
			free(history[i]);
			history[i]=NULL;
		}
		free(history);
		history=NULL;
		
		fclose(historyfile);
		
	}	
}

char* retrievehistory(char* comm)
{
	char* temp;
	temp=comm+1;
	if (strcmp(comm,"!")==0) {
		printf("syntax error near unexpected token `newline'\n");
		return NULL;
	}
	else if (strcmp(comm, "!!")==0) 
	{
		return history[h-1];
	}
	else if(isnum(temp)==0||atoi(temp)>hismaxsize||atoi(temp)<0)
	{
		printf("%s: event not found\n",comm);
		return NULL;
	}
	else // after bang is a good number
	{
		//since history is global and dynamic allocated space
		//we can return this and release memory in the end of shell termination
		return history[atoi(temp)-1];
	}
}

void historysub(char* comm,char** temp)
{
	
	// ^string1^string2^
	
	char* start; 
	char* string1;
	char* string2;
	char* command;
	int number=0;
	
	command=strdup(comm);//backup
	start=command;
	while (*command!='\0') {
		if (*command=='^') {
			number++;
		}
		command++;
	}
	command=start;
	
	if (number!=3)
	{
		printf("%s: substitution failed\n",comm);
		free(command);
		*temp='\0';
	}
	else
	{
		string1=command+1;
		command++;
		while (*command!='^') {
			command++;
		}
		*command='\0';
		command=command+1;
		string2=command;
		while (*command!='^') {
			command++;
		}
		*command++='\0';
		//call substituion functions in setpath
		
		*temp=strdup(history[h-1]);
		if (issubstring(*temp, string1)) 
		{
			replace(temp, string1,string2);
			command=start;
			free(command);
			command=NULL;
		}
		else
		{
			printf("%s: substitution failed\n",comm);
			command=start;
			free(command);
			*temp='\0';
		}				   
	}
}


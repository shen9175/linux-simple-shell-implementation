/*
 *  alias.c
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alias.h"
#include "sharedtools.h"
#include "global.h"

void setalias(char* command)
{
	
	/* syntax: alias [-p] word=string */
	char** aliasoption=malloc(1*sizeof(char**));
	char* p;
	parser(command, &aliasoption);
	
	
	//remove quotation mark in each argument like: alias dir="ls -aF"
	int j=0;
	while (aliasoption[j]!='\0')
	{   char *tmp;
		if ((tmp=strstr(aliasoption[j], "\""))!=NULL) 
		{
			char* squzeed=malloc(1*sizeof(char));
			int i=0,m=0;
			while (aliasoption[j][i]!='\0') {
				if (aliasoption[j][i]=='"')
				{
					i++; //skip '"'
				}
				else
				{	
					m++;
					squzeed=realloc(squzeed, (m+1)*sizeof(char));
					squzeed[m-1]=aliasoption[j][i];
					i++;
					
				}
			}
			squzeed[m]='\0';
			//aliasoption[j]=realloc(aliasoption[j], (strlen(squzeed)+1)*sizeof(char));
			//strcpy(aliasoption[j],squzeed);
			aliasoption[j]=strdup(squzeed);
			free(squzeed);
			squzeed=NULL;
		}
		
		j++;
	}
	
	if (aliasoption[1]=='\0')  //no argument
	{
		printallalias();
	}
	else if (strcmp(aliasoption[1],"-p")==0) //argument is "-p" 
	{
		printallalias();
		int i=2;
		while (aliasoption[i]!='\0')  //if after argument "-p" there are still  arguments
		{								//consider the rest as alias name and print out
			p=strstr(aliasoption[i],"=");
			if (p==NULL||p-aliasoption[i]==0) //alias =ls no alias name before '='
			{
				if (first!=NULL) {
					printalias(aliasoption[i]);
				}
			}
			else
			{
				char* name=malloc((p-aliasoption[i])*sizeof(char));
				char* word=malloc(strlen(aliasoption[i])-(p-aliasoption[i])-1);
				strncpy(name,aliasoption[i],(p-aliasoption[i]));
				strcpy(word,p+1);
				addalias(name,word);
				free(name);
				name=NULL;
				free(word);
				word=NULL;
			}
			i++;
		}
	}
	
	// if after "alias" the first argument is not empty nor "-p"
	// then consider them as alias names print them out
	else  
	{
		int i=1;
		while (aliasoption[i]!='\0') 
		{
			p=strstr(aliasoption[i],"=");
			if (p==NULL||p-aliasoption[i]==0)
			{
				printalias(aliasoption[i]);
			}
			else
			{
				char* name=malloc((p-aliasoption[i]+1)*sizeof(char));
				char* word=malloc(strlen(aliasoption[i])-(p-aliasoption[i])-1);
				strncpy(name,aliasoption[i],(p-aliasoption[i]));
				name[p-aliasoption[i]]='\0';
				strcpy(word,p+1);
				addalias(name,word);
				free(name);
				name=NULL;
				free(word);
				word=NULL;
			}	
			i++;
		}
	}
	
	//release memory space
	int i=0;
	while (aliasoption[i]!='\0')
	{
		free(aliasoption[i]);
		aliasoption[i]=NULL;
		i++;
	}		
	
	free(aliasoption);
}

void addalias(char* name, char* word)
{
	
	aliasptr p=first;
	
	if (p==NULL) //alias list is empty 
	{
		p=malloc(1*sizeof(struct aliaslist));
		p->name=malloc((strlen(name)+1)*sizeof(char));
		strcpy(p->name,name);
		p->word=malloc((strlen(word)+1)*sizeof(char));
		strcpy(p->word,word);
		p->next=NULL;
		first=p;
		
	}
	else 
	{	
		//the name is already in the list
		//replace with new one
		
		if (strcmp(findalias(name),name)==0) 
		{
			while (strcmp(p->name,name)!=0) 
			{
				p=p->next;
			}
			p->word=malloc((strlen(word)+1)*sizeof(char));
			strcpy(p->word,word);
		}
		else //the name is not in the list,add it into list
		{
			while (p->next!=NULL) 
			{
				p=p->next;
			}
			aliasptr n=malloc(sizeof(struct aliaslist));
			n->name=malloc((strlen(name)+1)*sizeof(char));
			strcpy(n->name,name);
			n->word=malloc((strlen(word)+1)*sizeof(char));
			strcpy(n->word,word);
			n->next=NULL;
			p->next=n;
			
		}
		
	}
	
}

char* findalias(char* name)
{
	aliasptr p=first;
	while (p!=NULL)
	{
		if (strcmp(p->name,name)==0)
		{
			return (p->name);
		}
		else
		{
			p=p->next;
		}
		
		
	}
	return "\0";
}

char* getalias(char* name)
{
	aliasptr p=first;
	while (p!=NULL)
	{
		if (strcmp(p->name,name)==0)
		{
			//since alias linked list is global
			//and each item has memory space
			//so we can return it to other function
			//and the memory space will release in the end
			//of the whole programe termination
			return (p->word);
		}
		else
		{
			p=p->next;
		}
		
		
	}
	return "\0";
}

void printalias(char* name)
{
	aliasptr p=first;
	if (strcmp(findalias(name),name)==0)
	{
		while (strcmp(p->name,name)!=0) 
		{
			p=p->next;
		}
		printf("alias %s='%s'\n",p->name,p->word);
	}
	else 
	{
		printf("alias: %s: not found\n",name);
		
	}
}

void printallalias()
{
	aliasptr p=first;
	while (p!=NULL) {
		printf("alias %s='%s'\n",p->name,p->word);
		p=p->next;
	}
}


void unalias(char*command)
{
	
	//if no arguments behind print error messeage
	//if "-a" is behind, delete all alias list
	//if some names behind, delete each name in the list
	
	char** aliasoption=malloc(1*sizeof(char**));
	parser(command, &aliasoption);
	if (aliasoption[1]=='\0') {
		printf("unalias: usage: unalias [-a] name [name ...]\n");
	}
	else if (strcmp(aliasoption[1],"-a")==0) 
	{
		deleteallalias();
	}
	else
	{
		int i=1;
		while (aliasoption[i]!='\0') 
		{
			deletealias(aliasoption[i]);
			i++;
		}
		
	}
	
	//release memory space
	int i=0;
	while (aliasoption[i]!='\0')
	{
		free(aliasoption[i]);
		aliasoption[i]=NULL;
		i++;
	}		
	
	free(aliasoption);
	aliasoption=NULL;
}

void deletealias(char* name)
{
	aliasptr p,prev;
	p=first;
	if (strcmp(findalias(name),name)!=0)
	{
		printf("unalias: %s: not found\n",name);
	} 
	else 
	{	// first can not be NULL so ignore this case
		
		//case1 if deleted node is head
		if (strcmp(p->name, name)==0) 
		{
			first=p->next;
			p->next=NULL;
			free(p->name);
			p->name=NULL;
			free(p->word);
			p->word=NULL;
			free(p);
			p=NULL;
		} 
		else //case2 if deleted node is not head
		{
			while (strcmp(p->name, name)!=0) {
				prev=p;
				p=p->next;
			}
			prev->next=p->next;
			p->next=NULL;
			free(p->name);
			p->name=NULL;
			free(p->word);
			p->word=NULL;
			free(p);
			p=NULL;			
		}
	}
	
	
}

void deleteallalias()
{
	
	//delete from head to tail
	//and release memory space
	if (first!=NULL)
	{
		while (first->next!=NULL) 
		{
			free(first->name);
			first->name=NULL;
			free(first->word);
			first->word=NULL;
			first=first->next;
		}
		free(first->name);
		first->name=NULL;
		free(first->word);
		first->word=NULL;
		free(first);
		first=NULL;
		
	}
	
}


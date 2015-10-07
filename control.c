/*
 *  control.c
 *  sis2sep
 *
 *  Created by Shen Xiaozhe on 3/28/11.
 *  Copyright 2011 University of Idaho. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "global.h"
#include "path.h"
#include "alias.h"
#include "history.h"
#include "sharedtools.h"
#include "control.h"

void control(int batchflag)
{	
	char* command;
	char** option;
	command=malloc(1*sizeof(char)); //pre-allocate memory to command
	option=malloc(1*sizeof(char**));
	
	int result=prompt(&command,&option,batchflag);
	if(result==-1)  //the shell process normally terminated.
	{
		printf("[Process completed]\n");
		addhistory(command);
		
		// free all dynamic allocated memory spaces
		free(command);
		command=NULL;
		
		int i=0;
		while (option[i]!='\0')
		{
			free(option[i]);
			option[i]=NULL;
			i++;
		}
		free(option);
		option=NULL;
		
		storehistory();//store and release dynamic allocated global history array
		
		deleteallalias();//release dynamic allocated global alias linked list
		
		exit(0);
	}
	else if(result==0)  // the shell finishes executing build-in command: "cd" and "exit"
	{		
		
		
		
		
		//free dynamic allocated memory space
		//for both command and options
		
		
		//if it's end of file option[i] is not allocated
		//don't need to release
		//command="" don't need to add in history
		if (infile!=NULL)  //if file is open
		{
			if ( !feof(infile))  //and file is not at the end
			{
				addhistory(command);
				//release options
				int i=0;
				while (option[i]!=NULL)
				{
					free(option[i]);
					option[i]=NULL;
					i++;
				}
			}
			//if file is at the end,do not release neither add history
			//to avoid add "" in the history and aceess out of bound memory
		}
		else //if file is not open, just do it.
		{
			addhistory(command);
			//release options
			int i=0;
			while (option[i]!=NULL)
			{
				free(option[i]);
				option[i]=NULL;
				i++;
			}	
		}

		
		
		
		//release command
		free(command);
		command=NULL;
		
		free(option);
		option=NULL;
		
	}
	else if(result==1) // the shell should call the utilities in the system
	{
		callexternal(option);
		addhistory(command);
		
		//free dynamic allocated memory space
		//for both command and options
		
		//release command
		free(command);
		command=NULL;
		
		//release options
		int i=0;
		while (option[i]!='\0')
		{
			free(option[i]);
			option[i]=NULL;
			i++;
		}		
		free(option);
		option=NULL;
	}
	
}

void callexternal(char** option)
{
	pid_t childpid;
	int status;
	
	if((childpid=fork())==-1)
	{
		perror("Error in the fork");
		exit(1);
	}
	else if (childpid==0) /* child code */
	{
		if (execvp(*option,option)==-1)
		{
			perror(""); // the system will prompt the wrong information, I just use empty string or system will not prompt the wrong information.
			exit(1);
		} 
		else 
		{
			exit(0);
		}
		
	}
	else
	{
		while(childpid!=wait(&status)) /*parent code: wait the child process execution terminated*/
		{;}
	}
	
}


int prompt (char** command, char*** option,int batchflag)
{
	
	
	do{
		if (batchflag==0) //command from keyboard
		{
			printf("?:");  // print prompt
		}
		
		readline(command,batchflag); //should be &(*command)but canceled
		
		if (batchflag==1&&feof(infile)) //command from file
		{
			return 0;			
		}
		
	} while((*command)[0]=='\0');
	
	
	
	//build-in command process
	
	int check=2;
	
	while(check==2)//check==2 means alias appears so redo check modified command
	{	check=buildincheck(command, option);
		if (check==-1) //buildincheck==-1 means command=="exit"
			return -1;
		
		else if(check==0)
			return 0;   //buildincheck==0 means other build-in comand
		
		else if(check==1) //external command
			
			//parser(command, option);
		    return 1;
	}		
	return 0;
}


void readline(char** command,int batchflag)
{
	char temp;  // the temporary char variable for getchar()
	
	int i=0;  //command string counter
	temp='\0';
	
	
	
	//waiting for command input for keyboard until receive the "return" key
	
	while(temp!='\n')
	{	
		if (batchflag==1) 
		{
			temp=fgetc(infile);
			if (temp==EOF) 
				temp='\n';
			
		}
		else 
		{
			temp=getchar();
		}
		
		if (temp=='\n')  //if the first char is "return" then let command string be empty--do nothing.
		{
			(*command)[i]='\0';	//clear the command string
		}
		else
		{	
			i++;
			*command=realloc(*command,(i+1)*sizeof(char));
			(*command)[i-1]=temp;  // put the each char into command string
			
		}	
	}
	
	//replace enviroment variable
	if (issubstring(*command,"$PATH"))
	{
		*command=realloc(*command, (replacelength(*command,"$PATH",getenv("PATH"))+1)*sizeof(char));
		replace(command,"$PATH",getenv("PATH"));
	}
	
	
	
	
	
}	


int buildincheck(char** command,char*** option)
{
	
	parser(*command,option);
	
	
	
	if(strcmp(*command,"exit")==0)
	{
		return -1;
	}
	
	/* if there is a white space after PATH, then the parser function parse the first argument as PATH
	 if there is no white space after PATH, then we compare the first 5 charactors in the first argument whether it equals to "PATH="*/
	
	
	else if (strcmp((*option)[0],"PATH")==0||strncmp((*option)[0], "PATH=", 5)==0)
	{
		setpath(*command);
		return 0;
	}
	else if (strcmp((*option)[0], "history")==0||strncmp((*option)[0], "HISTORYCOUNT=", 8)==0||strcmp((*option)[0], "HISTORYCOUNT")==0)
	{
		sethistory(*command);
		return 0;
	}
	else if (strncmp((*option)[0], "!",1)==0)
	{
		char* temp=retrievehistory((*option)[0]);
		if (temp!='\0')
		{
			*command=strdup(temp);
			return 2;
		}
		else
			return 0;
	}
	
	else if(strncmp((*option)[0], "^", 1)==0)
	{
		char* temp;
		temp=malloc(1*sizeof(char*));
		historysub((*option)[0],&temp);
		if (temp!='\0')
		{
			*command=strdup(temp);
			free(temp);
			return 2;
		}
		else
			return 0;
	}
	else if (strcmp((*option)[0], "alias")==0)
	{
		setalias(*command);
		return 0;
	}	
	
	else if (strcmp((*option)[0], "unalias")==0)
	{		
		unalias(*command);
		return 0;
	}
	
	
	else if (strcmp((*option)[0],"cd")==0) 
	{
		setcd(*option);
		return 0;
	}
	
	else if (strcmp(findalias((*option)[0]),"\0")!=0) //alias appears
	{   //de-parser-> re-assembly
		**command='\0'; //clear command
		(*option)[0]=realloc((*option)[0], (strlen(getalias((*option)[0]))+1)*sizeof(char));
		strcpy((*option)[0],getalias((*option)[0]));
		int i=0;
		while ((*option)[i]!='\0') {
			*command=realloc(*command, (strlen(*command)+strlen((*option)[i])+2)*sizeof(char));
			strcat(*command,(*option)[i]);
			strcat(*command," ");
			i++;
		}
		
		return 2; // return 2 means alias appears
		
	}				
	else 
		return 1;  //if the input belongs to none of the above, check external rountine
	
	
}




void rcfile()
{
	
	
	infile=fopen(".sisrc", "r");
	
	if (infile!=NULL)
	{
		while (!feof(infile)) 
		{
			control(1);
		}	
		
		
		fclose(infile);
	}	
	
}



void setcd(char** option)
{
	if(chdir(option[1])==-1)
		printf("There is no such a directory!\n");
	
}



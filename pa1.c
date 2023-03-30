/**********************************************************************
 * Copyright (c) 2020-2023
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "types.h"
#include "list_head.h"
#define MAX_NR_TOKENS	32

struct entry {
	struct list_head list;
	char *alias;
	char **content;
	int nr_tokens;
};
struct list_head stack;
LIST_HEAD(stack); 
/***********************************************************************
 * change_directory()
 *
 */

void change_directory(char *tokens[]){
	char *cmd;
	if(tokens[1]==NULL||strcmp(tokens[1],"~")==0){
		chdir(getenv("HOME"));
		return;
	}
	else{
		cmd=strdup(tokens[1]);
		if(strncmp(cmd,"~",1)==0){
			char *temp=(char*)malloc(sizeof(char)*100);
			char *home=getenv("HOME");
			strcpy(temp,home);	
			strcat(temp,(cmd+1));
			free(cmd);
			cmd=temp;
		}
		if(chdir(cmd)==-1){
			fprintf(stderr,"-mash: cd: %s: No such file or directory\n",tokens[1]);
		}
	}
	free(cmd);
	return;
}

/***********************************************************************
 * alias()
 *
 */
void alias(char *tokens[],int nr_tokens){
	if(tokens[1]==NULL){
		if(list_empty(&stack)) return;
		else{
			struct entry * temp=NULL;
			list_for_each_entry_reverse(temp,&stack,list){
				fprintf(stderr,"%s:",temp->alias);
				for(int i=0;i<(temp->nr_tokens);i++){
					fprintf(stderr," %s",temp->content[i]);
				}
				fprintf(stderr,"\n");
			}
		}
		return;
		}
	else{
	struct entry * temp=(struct entry*)malloc(sizeof(struct entry));
	temp->alias=strdup(tokens[1]);
	temp->nr_tokens=nr_tokens-2;
	temp->content=(char**)malloc(sizeof(char*)*(nr_tokens-2));
	for(int i=0;i<nr_tokens-2;i++){
		temp->content[i]=strdup(tokens[i+2]);
	}
	list_add(&(temp->list),&stack);
	}
	return;
}

/***********************************************************************
 * check_alias()
 *
 */
void check_alias(char* tokens[],int nr_tokens){
	char *temp_tokens[MAX_NR_TOKENS]={ NULL };
	for(int i=0;i<nr_tokens;i++){
		temp_tokens[i]=strdup(tokens[i]);
	}
	int idx=1;
	for(int i=1;i<nr_tokens;i++){
		struct entry * temp=NULL;
		list_for_each_entry_reverse(temp,&stack,list){
			if(strcmp(temp->alias,temp_tokens[i])==0){
				for(int j=0;j<temp->nr_tokens;j++){
					free(tokens[idx]);
					tokens[idx]=strdup(temp->content[j]);
					idx++;
				}
				break;
			}
	}	
	}
	for(int i=0;i<MAX_NR_TOKENS;i++){
		free(temp_tokens[i]);
	}	
	return;
}

/***********************************************************************
 * run_command()
 *
 * DESCRIPTION
 *   Implement the specified shell features here using the parsed
 *   command tokens.
 *
 * RETURN VALUE
 *   Return 1 on successful command execution
 *   Return 0 when user inputs "exit"
 *   Return <0 on error
 */
int run_command(int nr_tokens, char *tokens[])
{
        if (strcmp(tokens[0], "exit") == 0) return 0;
		if(strcmp(tokens[0],"cd")==0){
			change_directory(tokens);
			return 1;
		}
		if(strcmp(tokens[0],"alias")==0){
			alias(tokens,nr_tokens);
			return 1;
		}

		check_alias(tokens,nr_tokens);

        pid_t pid;
        pid=fork();
        if(pid<0) return -1;

        if(pid==0){//child process

        	if(execvp(tokens[0],tokens)<0){
            	fprintf(stderr, "Unable to execute %s\n", tokens[0]);                
        	}
        	exit(0);
        }

        int status;
        if(pid>0){
                while(wait(&status)!=pid){
                        continue;
                }
        }
        return 1;
}

/***********************************************************************
 * initialize()
 *
 * DESCRIPTION
 *   Call-back function for your own initialization code. It is OK to
 *   leave blank if you don't need any initialization.
 *
 * RETURN VALUE
 *   Return 0 on successful initialization.
 *   Return other value on error, which leads the program to exit.
 */
int initialize(int argc, char * const argv[])
{
	return 0;
}

/***********************************************************************
 * finalize()
 *
 * DESCRIPTION
 *   Callback function for finalizing your code. Like @initialize(),
 *   you may leave this function blank.
 */
void finalize(int argc, char * const argv[])
{
}

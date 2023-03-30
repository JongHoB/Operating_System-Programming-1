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

/***********************************************************************
 * change_directory()
 *
 */
void change_directory(char *tokens[]){
	char *cmd=(char*)malloc(sizeof(char)*100);
	if(tokens[1]==NULL||strcmp(tokens[1],"~")==0){
		chdir(getenv("HOME"));
	}
	else{
		strcpy(cmd,tokens[1]);
		if(strncmp(cmd,"~",1)==0){
			char *temp=(char*)malloc(sizeof(char)*100);
			char *home=getenv("HOME");
			strcpy(temp,home);	
			strcat(temp,(cmd+1));
			free(cmd);
			cmd=temp;
		}
		if(chdir(cmd)==-1){
			fprintf(stderr,"-bash: cd: %s: No such file or directory\n",tokens[1]);
		}
	}
	free(cmd);
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

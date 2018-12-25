

/*
Author: Riwaz Awasthi
Description: C/C++ program implementing the four functional characters provided by UNIX- >, <, | and &. 
Instructions: Please store the file in the working directory. Compile and run the file in linux/unix terminal using g++ compiler. 
              The program was tested in CentOS operating system. 
*/

#include <unistd.h>
#include<stdio.h>
#include<iostream>
#include<string.h>
#include<string>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include<sys/types.h>
#include<stdlib.h>
using namespace std;
int main() {
	int fd, status, i = 0, index;
	char **argt = NULL;
	char **args = NULL;
	char **argv = NULL;
	char a[20];
	char b[20];
	char command[50];
	char *d;
	char *e;
	char *p;
	printf("Please enter a command. Make sure to put a space after each word.(Ex: echo hello > file)\n");
	fgets(command, 50, stdin);
	if (strchr(command, '>') != NULL) {           //scans for the functional characters and assigns value to the index parameter
		index = 1;
	}
	else if (strchr(command, '<') != NULL) {
		index = 2;
	}
	else if (strchr(command, '|') != NULL) {
		index = 3;
	}
	else if (strchr(command, '&') != NULL) {
		index = 4;
	}
	else {
		index = 5;
	}
	strtok(command, "\n");
	p = strtok(command, " ");
	while (p) {
		args = (char**)realloc(args, sizeof(char*) * ++i);   //reallocating memory for args
		if (args == NULL)
			exit(-1);         //memory allocation failed
		args[i - 1] = p;
		p = strtok(NULL, " ");
	}
	args = (char**)realloc(args, sizeof(char*)*(i + 1));    //giving args one more space to store NULL value
	args[i] = 0;                                         //args contains the commands as separate strings
	if (index == 1) {              //output redirection implementation
		strcpy(a, args[0]);
		strcat(a, " ");
		strcat(a, args[1]);
		d = strtok(a, " ");
		i = 0;
		while (d) {
			argt = (char**)realloc(argt, sizeof(char*) * ++i);
			if (argt == NULL)
				exit(-1);         //memory allocation failed
			argt[i - 1] = d;
			d = strtok(NULL, " ");
		}

		argt = (char**)realloc(argt, sizeof(char*)*(i + 1));
		argt[i] = 0;                                         //argt contains the two words before '>' and a terminatng NULL required for execvp. 
		fd = open(args[3], O_CREAT | O_RDWR, S_IRUSR | S_IRGRP | S_IROTH);      //If the file already exists, it is opened, if not , a new file is created 
		dup2(fd, 1);                                 //The opened file is set to be stdout
		int pid = fork();                            //A process is forked off
		if (pid != 0) {                              //If the process isn't child process, we wait for the previous process to ennd
			waitpid(-1, &status, 0);
		}
		else {
			//child process
			execvp(argt[0], argt);                  //If it is the child process, the command is executed
		}

	}

	else if (index == 2) {             //input redirection implementation
		strcpy(a, args[0]);
		strcat(a, " ");
		d = strtok(a, " ");
		i = 0;
		while (d) {
			argt = (char**)realloc(argt, sizeof(char*) * ++i);
			if (argt == NULL)
				exit(-1);         //memory allocation failed
			argt[i - 1] = d;
			d = strtok(NULL, " ");
		}

		argt = (char**)realloc(argt, sizeof(char*)*(i + 1));
		argt[i] = 0;                                      //argt will now contain the command to be run
		fd = open(args[2], O_CREAT | O_RDONLY);
		dup2(fd, 0);                                   //input redirection to file directory given by fd
		int pid = fork();
		if (pid != 0) {
			waitpid(-1, &status, 0);
		}
		else {
			//child process
			execvp(argt[0], argt);
		}

	}
	else if (index == 3) {            //Pipe implementation
		strcpy(b, args[3]);
		strcat(b, " ");
		strcat(b, args[4]);
		e = strtok(b, " ");
		i = 0;
		while (e) {

			argv = (char**)realloc(argv, sizeof(char*) * ++i);

			if (argv == NULL)
				exit(-1);         //memory allocation failed
			argv[i - 1] = e;
			e = strtok(NULL, " ");
		}
		argv = (char**)realloc(argv, sizeof(char*)*(i + 1));
		argv[i] = 0;            // argv now contains commands to the right side of the pipe
		
		strcpy(a, args[0]);
		strcat(a, " ");
		strcat(a, args[1]);
		p = strtok(a, " ");
		i = 0;
		while (p) {
			argt = (char**)realloc(argt, sizeof(char*) * ++i);
			if (argt == NULL)
				exit(-1);         //memory allocation failed
			argt[i - 1] = p;
			p = strtok(NULL, " ");
		}
		argt = (char**)realloc(argt, sizeof(char*)*(i + 1));
		argt[i] = 0;          //argt contains commands to the left side of the pipe              
		
		int pipefd[2];
		pipe(pipefd);               //create a pipe
		int pid1 = fork();
		
		if (pid1 == 0) {
			close(0);              // 0 refers to standard input
			close(pipefd[1]);       //write end of the pipe is closed
			dup(pipefd[0]);        //read end of the pipe is made stdin
			execvp(argv[0], argv);
		}
		else {
			close(1);
			close(pipefd[0]);       //read end of the pipe is closed
			dup(pipefd[1]);        //write end of the pipe is made stdout
			execvp(argt[0], argt);  //execute the command
		}
		
		free(args);                //allocated memory is freed
		free(argv);
		free(argt);

	}

	else if (index == 4) {           //Background implementation
		strcpy(a, args[0]);
		strcat(a, " ");
		strcat(a, args[1]);
		p = strtok(a, " ");
		i = 0;
		while (p) {
			argt = (char**)realloc(argt, sizeof(char*) * ++i);
			if (argt == NULL)
				exit(-1);         //memory allocation failed
			argt[i - 1] = p;
			p = strtok(NULL, " ");
		}
		argt = (char**)realloc(argt, sizeof(char*)*(i + 1));
		argt[i] = 0;
		if (fork != 0) {
			//Parent Process doesn't wait for the child to exit
		}
		else {
			setpgid(0, 0);  //child process is added to anothe process group
			execvp(argt[0], argt);
		}

	}
	else {
		cout << "Invalid command" << "\n";
	}



	return 0;
}



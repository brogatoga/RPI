/**
 * CSCI 4210: Operating Systems
 * Homework 1: Shell
 * Ryan Lin
 * linr2@rpi.edu
**/

/**
 * CSCI 4210: Operating Systems
 * Homework 1: Shell
 * Ryan Lin
 * linr2@rpi.edu
**/


#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <glob.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "constants.h"


// Holds information about a parsed input line from the shell
struct input_data
{
	char* executable_name;
	char* executable_name2;
	char** arguments;
	char** arguments2;
	char* redirect_file;
	char* cd_path;
	int command_type;
};


// Function prototype definitions
char** split(char*, char*, int*);
void free_carr(char**, int);
char* trim(char*);
int file_exists(char*);
char* newbuffer();
int contains_wildcards(char*);
char** expand_wildcards(char*, char*, int*);

struct input_data* input_data_new();
void input_data_free(struct input_data*);
void input_data_parse(struct input_data*, char*);

#endif
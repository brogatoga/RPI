/**
 * CSCI 4210: Operating Systems
 * Homework 1: Shell
 * Ryan Lin
 * linr2@rpi.edu
**/


#include "includes.h"


// Allocates memory for a new input_data structure and returns a pointer to it
struct input_data* input_data_new()
{ 
	struct input_data* result = malloc(sizeof(struct input_data));
	if (result == NULL)
		return NULL;
	
	result->executable_name = newbuffer();
	result->executable_name2 = newbuffer();
	result->arguments = malloc(sizeof(char*) * BUFFER_SIZE);
	result->arguments2 = malloc(sizeof(char*) * BUFFER_SIZE);
	result->redirect_file = newbuffer();
	result->cd_path = newbuffer();
	result->command_type = 0;
	
	return result;
}


// Frees all allocated memory for an input_data structure
void input_data_free(struct input_data* id)
{
	if (id == NULL)
		return;
	
	if (id->executable_name != NULL)  free(id->executable_name);
	if (id->executable_name2 != NULL) free(id->executable_name2);
	if (id->arguments != NULL) 		  free(id->arguments);
	if (id->arguments2 != NULL)		  free(id->arguments2);
	if (id->redirect_file != NULL)    free(id->redirect_file);
	if (id->cd_path != NULL)		  free(id->cd_path);
	free(id);
}


// Parses an input line and stores the information into the structure fields
void input_data_parse(struct input_data* id, char* input)
{
	if (id == NULL || input == NULL)
		return;
	
	char** pieces = NULL;
	int count = 0;
	char* copy = malloc(sizeof(char) * strlen(input));
	copy = trim(input);
	
	id->command_type = 0;
	
	// If input is empty, then this is an invalid command
	if (strlen(copy) == 0)
		return;
	
	pieces = split(copy, " ", &count);
	if (count == 0)
		return;
	id->executable_name = pieces[0];
	
	
	// Is this an exit command?
	if (count == 1 && (!strcmp(id->executable_name, "quit") || !strcmp(id->executable_name, "exit")))
	{
		id->command_type = COMMAND_EXIT;
		free_carr(pieces, count);
		return;
	}
	
	// Is this a "cd" command?
	else if (!strcmp(id->executable_name, "cd"))
	{
		id->command_type = COMMAND_CD;
		
		strcpy(id->cd_path, ".");		// Defaut cd path is the current directory
		
		if (count == 1 || (count == 2 && !strcmp(pieces[1], "~")))
		{
			strcpy(id->cd_path, getenv("HOME"));
		}
		else if (count >= 2 && pieces[1][0] == '~' && strlen(pieces[1]) > 1)
		{
			char* buffer = newbuffer();
			pieces[1][0] = '/';
			strcpy(buffer, getenv("HOME"));
			strcat(buffer, pieces[1]);
			strcpy(id->cd_path, buffer);
			free(buffer);
		}
		else if (count >= 2) 
		{
			strcpy(id->cd_path, pieces[1]);
		}
		
		free_carr(pieces, count);
		return;
	}
	
	// Is this an IPC pipe command?
	int pipe_index = -1;
	int c; 
	for (c = 1; c < count - 1; c++)
	{
		if (!strcmp(pieces[c], "|"))
		{
			pipe_index = c;
			break;
		}
	}
	
	if (pipe_index > 0)
	{
		id->command_type = COMMAND_IPC_PIPE;
		id->arguments[pipe_index] = NULL;
		id->executable_name2 = pieces[pipe_index+1];
			
		int b = 0;
		
		for (c = 0; c < pipe_index; c++)
			id->arguments[c] = pieces[c];
		for (c = pipe_index+1; c < count; c++)
			id->arguments2[b++] = pieces[c];
		
		id->arguments2[b] = NULL;
		return;
	}
	
	
	// Does it contain any wildcards that need to be expanded?
	char* current_dir = malloc(BUFFER_SIZE);
	char** temp = malloc(sizeof(char*) * count);
	current_dir = getcwd(current_dir, BUFFER_SIZE);
	
	int k, p, q;
	for (k = 0; k < count; k++)
		temp[k] = pieces[k];
	
	pieces = malloc(sizeof(char*) * BUFFER_SIZE);
	k = 0;
	
	for (p = 0; p < count; p++)
	{
		if (contains_wildcards(temp[p]) > 0)
		{
			int entry_count;
			char** entries = expand_wildcards(temp[p], current_dir, &entry_count);

			if (entry_count > 0)
			{
				for (q = 0; q < entry_count; q++)
					pieces[k++] = entries[q];
			}
			else pieces[k++] = temp[p];
		}
		else pieces[k++] = temp[p];
	}
	
	count = k;
	
	/*
	printf("--- start ---\n");
	for (k = 0; k < count; k++)
		printf("%s\n", pieces[k]);
	printf("--- end --- \n");
	*/
		
	free(temp);
	free(current_dir);
	
	
	// Is this a backgrounded command?
	if (count >= 2 && !strcmp(pieces[count-1], "&"))
	{
		id->command_type = COMMAND_SHELL_BG;
		id->arguments[count-1] = NULL;
		
		for (c = 0; c < count - 1; c++)
			id->arguments[c] = pieces[c];
		return;
	}
	
	
	// Does it contain any redirection operators like >, <, or >> ?
	if (count >= 3)
	{
		int is_redirect = 0; 
		
		if (!strcmp(pieces[count-2], ">"))
		{
			id->command_type = COMMAND_R_OUTPUT;
			is_redirect = 1;
		}
		else if (!strcmp(pieces[count-2], ">>"))
		{
			id->command_type = COMMAND_R_APPEND;
			is_redirect = 1;
		}
		else if (!strcmp(pieces[count-2], "<"))
		{
			id->command_type = COMMAND_R_INPUT;
			is_redirect = 1;
		}
		
		if (is_redirect)
		{
			id->redirect_file = pieces[count-1];
			id->arguments[count-2] = NULL;
			
			int c; for (c = 0; c < count - 2; c++)
				id->arguments[c] = pieces[c];
			return;
		}
	}
	
	
	// If it's not any of these, then it's just a command that should run in the foreground
	id->command_type = COMMAND_SHELL_FG;
	id->arguments[count] = NULL;
	
	for (c = 0; c < count; c++)
		id->arguments[c] = pieces[c];
}
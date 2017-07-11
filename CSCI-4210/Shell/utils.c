/**
 * CSCI 4210: Operating Systems
 * Homework 1: Shell
 * Ryan Lin
 * linr2@rpi.edu
**/

#include "includes.h"


// Parses the path string and returns an array containing all search locations
char** split(char* input, char* delimiter, int* count)
{
	if (input == NULL || delimiter == NULL || count == NULL 
		|| strlen(input) == 0 || strlen(delimiter) == 0)
	{
		*count = 0;
		return NULL;
	}

	char** result = NULL;
	char* piece = NULL;
	int length = strlen(input);
	char* copy = NULL;
	int delimiter_count = 0;
	int delimiter_at_end = 0;
	int c;

	// Count how many occurrences of the delimiter there are in the path string
	for (c = 0; c < length; c++)
	{
		if (input[c] == delimiter[0])
		{
			if (c == length - 1)
				delimiter_at_end = 1;
			delimiter_count++;
		}
	}

	// If there is no delimiter, then the entire input is the first and only piece
	if (delimiter_count == 0 || (delimiter_count == 1 && delimiter_at_end == 1))
	{
		result = malloc(sizeof(char*));
		result[0] = malloc(sizeof(char) * length);
		strcpy(result[0], input);
		*count = 1;
		return result;
	}

	// Separate the input into pieces in an array based on the delimiter
	*count = delimiter_count + 1;
	copy = malloc(sizeof(char) * length);
	strcpy(copy, input);
	result = malloc(sizeof(char*) * (*count));
	piece = strtok(copy, delimiter);
	c = 0;

	while (piece != NULL)
	{
		result[c] = malloc(sizeof(char) * strlen(piece));
		strcpy(result[c], piece);
		piece = strtok(NULL, delimiter);
		c++;
	}
	
	if (copy != NULL) free(copy);
	return result;
}


// Frees all memory allocated to an array of character pointers
void free_carr(char** data, int count)
{
	if (data == NULL || count <= 0)
		return;
	
	int c; for (c = 0; c < count; c++)
	{
		if (data[c] != NULL)
			free(data[c]);
	}
	free(data);
}


// Removes all beginning and ending whitespace from the given input
char* trim(char* input)
{
	if (input == NULL)
		return input;
	
	int c = strlen(input) - 2;
	for (; c >= 0 && isspace(input[c]); c--);
	input[c+1] = 0;
	
	while (isspace(*input))
		input++;
	
	return input;
}


// Allocates memory for a character buffer with BUFFER_SIZE elements and returns a pointer to it
char* newbuffer()
{
	return malloc(sizeof(char) * BUFFER_SIZE);
}


// Returns whether the file at the given path exists or not
int file_exists(char* path)
{
	return path != NULL && access(path, F_OK) != -1;
}


// Returns whether the given string contains one or more wildcards
int contains_wildcards(char* input)
{
	if (input == NULL)
		return 0;
	
	int length = strlen(input);
	int c;
	for (c = 0; c < length; c++)
	{
		if (input[c] == '*')
			return 1;
	}
	return 0;
}


// Expands the wildcard input and returns a list of all matching directory entries
char** expand_wildcards(char* input, char* cwd, int* count)
{
	if (input == NULL || cwd == NULL || count == NULL)
		return NULL;
	
	char buffer[BUFFER_SIZE];
	strcpy(buffer, input);
	
	glob_t* x = malloc(sizeof(glob_t));
	x->gl_offs =  0;
	glob(buffer, 0, NULL, x);
	*count = x->gl_pathc;

	/*
	int k = 0;
	for (k = 0; k < *count; k++)		// Now extract the basenames from the glob full directory paths
	{
		int start = strlen(x->gl_pathv[k]) - 1;
		if (x->gl_pathv[k][start] == '/')
			continue;
		
		while (x->gl_pathv[k][start] != '/')
			start--;
		x->gl_pathv[k] = &x->gl_pathv[k][start+1];
	}
	*/
	return x->gl_pathv;
}
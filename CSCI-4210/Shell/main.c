/**
 * CSCI 4210: Operating Systems
 * Homework 1: Shell
 * Ryan Lin
 * linr2@rpi.edu
**/

#include "includes.h"


// Parses the path string and returns an array containing all search locations
char** setup_paths(char* path_string, int* path_count)
{
	if (path_string == NULL || path_count == NULL)
		return NULL;
	
	char* copy = newbuffer();
	strcpy(copy, path_string);
	
	if (copy[strlen(copy)-1] == '#')
		copy[strlen(copy)-1] = '\0';
	
	char** result = split(copy, "#", path_count);
	free(copy);
	return result;
}


// Search through all known paths for the given executable name, return 1 and store in buffer if found, 0 otw
int locate(char* name, char** paths, int path_count, char* buffer)
{
	if (name == NULL || paths == NULL || buffer == NULL || path_count <= 0)
		return 0;
	
	int c;
	for (c = 0; c < path_count; c++)
	{
		strcpy(buffer, paths[c]);
		strcat(buffer, "/");
		strcat(buffer, name);
		
		if (file_exists(buffer))
			return 1;
	} 
	return 0;
}


// Main function, entry point of the application
int main(int argc, char *argv[])
{
	char* current_dir = newbuffer();				// Current working directory
	char* input = newbuffer();						// Shell input from the user
	char** paths;									// Array of directories to search in
	int path_count;									// Number of elements in paths array
	char* buffer = newbuffer();						// Buffer that can be used for anything
	char* buffer2 = newbuffer();					// Another buffer that can be used for anything
	pid_t pid;										// Process ID of the current process
	pid_t* background_pids = NULL; 					// Array of the PIDs of all background processes
	int background_pid_count = 0;					// Number of backgrounded processes in the array above
	int status = 0;									// Passed into the waitpid() function
	struct input_data* id = input_data_new();		// input_data structure
	

	// Set up the paths to search for executables in
	paths = setup_paths(getenv("MYPATH") == NULL ? DEFAULT_PATH : getenv("MYPATH"), &path_count);

	// Initialize the array of background pids
	background_pids = malloc(sizeof(pid_t) * BUFFER_SIZE);
	int p;
	for (p = 0; p < BUFFER_SIZE; p++)
		background_pids[p] = 0;

	// Main application loop
	while (1)
	{
		// Check and see if any processes running in the background have terminated
		int k;
		for (k = 0; k < background_pid_count; k++)
		{
			if (background_pids[k] == 0)
				continue;
			
			if (waitpid(background_pids[k], &status, WNOHANG) == background_pids[k])
			{
				printf("[process %d completed]\n", background_pids[k]);
				background_pids[k] = 0;
			}
		}
		
		
		// Update the current working directory and clear input buffer
		current_dir = getcwd(current_dir, BUFFER_SIZE);
		strcpy(input, "");
		
		
		// Prompt the user for an input
		printf("%s$ ", current_dir);
		fgets(input, BUFFER_SIZE, stdin);
		
		
		// Do a flush
		fflush(NULL);
		
		
		// Process the input and then do something with it
		input_data_parse(id, input);
		switch (id->command_type)
		{
			case COMMAND_CD:
			{
				if (chdir(id->cd_path) < 0)
					printf("ERROR: Unable to change to that directory \n");
				break;
			}
		
			case COMMAND_EXIT:
			{
				printf("bye");
				exit(EXIT_SUCCESS);
				break;
			}
			
			case COMMAND_SHELL_FG:
			case COMMAND_SHELL_BG:
			case COMMAND_R_OUTPUT:
			case COMMAND_R_INPUT:
			case COMMAND_R_APPEND:
			{
				if (locate(id->executable_name, paths, path_count, buffer) == 1)
				{
					// Fork!
					pid = fork();
	
						
					// Child process
					if (pid == 0)
					{
						// If we need to redirect the output or input, then do it inside here
						switch (id->command_type)
						{
							case COMMAND_R_OUTPUT:
							{
								int fd = open(id->redirect_file, O_WRONLY | O_CREAT | O_TRUNC, DEFAULT_RIGHTS);
								dup2(fd, 1);
								close(fd);
								break;
							}
							case COMMAND_R_APPEND:
							{
								if (!file_exists(id->redirect_file))
								{
									printf("ERROR: no such file to append to (%s)\n", id->redirect_file);
									continue;
								}
								
								int fd = open(id->redirect_file, O_WRONLY | O_APPEND, DEFAULT_RIGHTS);
								dup2(fd, 1);
								close(fd);
								break;
							}
							case COMMAND_R_INPUT:
							{
								if (!file_exists(id->redirect_file))
									continue;
								
								int fd = open(id->redirect_file, O_RDONLY);
								dup2(fd, 0);
								close(fd);
								break;
							}
						}
						
						// Run the executable file and replace this process with it
						if (execvp(buffer, id->arguments) < 0)
							exit(EXIT_FAILURE);
					}
	
	
					// Parent process
					else if (pid > 0)
					{
						int status;
						
						if (id->command_type == COMMAND_SHELL_BG)
						{
							printf("[process running in background with pid %d]\n", pid);
							background_pids[background_pid_count++] = pid;
							usleep(50000);
						}
						else waitpid(pid, &status, 0);
					}
					
					else continue;
				}
				else printf("ERROR: command '%s' not found\n", id->executable_name);
				break;
			}
			
			case COMMAND_IPC_PIPE:
			{
				pid_t second_pid;
				int p[2];
				
				
				// Make sure that both executables exist
				if (locate(id->executable_name, paths, path_count, buffer) == 0)
				{
					printf("ERROR: command '%s' not found\n", id->executable_name);
					continue;
				}
				
				if (locate(id->executable_name2, paths, path_count, buffer2) == 0)
				{
					printf("ERROR: command '%s' not found\n", id->executable_name2);
					continue;
				}
				
				
				// Fork!
				pid = fork();
				
				
				// Child process
				if (pid == 0)
				{
					// Create the pipe
					if (pipe(p) == -1)
						continue;
					
					// Fork again
					second_pid = fork();
					
					// Grandchild process
					if (second_pid == 0)
					{	
						dup2(p[1], 1);
						close(p[0]);
						
						if (execvp(buffer, id->arguments) < 0)
							exit(EXIT_FAILURE);
					}
					
					// Child process again
					else if (second_pid > 0)
					{
						int status;
						waitpid(second_pid, &status, 0);
						
						close(p[1]);
						dup2(p[0], 0);
						
						if (execvp(buffer2, id->arguments2) < 0)
							exit(EXIT_FAILURE);
					}
					
					else continue;
				}
				
				
				// Parent process
				else if (pid > 0)
				{
					waitpid(pid, &status, 0);
				}
				
				else continue;
				break;
			}
		}
	}
	
	
	// Free allocated memory (only if this is the parent process)
	if (input != NULL) 		  		free(input);
	if (current_dir != NULL) 		free(current_dir);
	if (buffer != NULL)				free(buffer);
	if (buffer2 != NULL)			free(buffer2);
	if (background_pids !=  NULL)	free(background_pids);
		
	free_carr(paths, path_count);
	input_data_free(id);
	
	
	// Return exit success
	return EXIT_SUCCESS;
}
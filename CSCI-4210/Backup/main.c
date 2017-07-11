/**
 * Homework 3: File Backup Utility
 * Ryan Lin (linr2@rpi.edu)
 * File: main.c
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "list.h"

#define BACKUP_DIR  ".mybackup"
#define DEBUG    	1


// Copies the file from source to destination and returns number of bytes written
ssize_t copy_file(char* from, char *to)
{
	if (from == NULL || to == NULL)
		return -1;
	
	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	ssize_t n;
	ssize_t written = 0;
	FILE* file_from = fopen(from, "r");
	FILE* file_to = fopen(to, "w");
	
	if (file_from == NULL || file_to == NULL)
		return -1;
	
	while ((n = fread(buffer, 1, BUFFER_SIZE, file_from)) > 0) {
		if (fwrite(buffer, sizeof(char), n, file_to) != n) {
			fclose(file_from);
			fclose(file_to);
			return -1;
		}
		written += n;
	}
	
	fclose(file_from);
	fclose(file_to);
	return written;
}

// Thread responsible for handling the backup of a single file or subdirectory
void* backup_thread(void* arg)
{
	if (arg == NULL)
		return NULL;

	struct list_node* info = (struct list_node*) arg;		// Thread argument
	char* backup = NULL;									// Name of the backup file (eg: a.txt becomes a.txt.bak)
	char* destination = NULL;								// Destination path (eg: a.txt becomes .mybackup/a.txt.bak)
	ssize_t written = 0;									// Total number of bytes written to backup file
	
	
	// Allocate memory for the two strings
	backup = malloc(strlen(info->value) + 5);
	strcpy(backup, info->value);
	strcat(backup, ".bak");
	
	destination = malloc(strlen(backup) + strlen(BACKUP_DIR) + 2);
	strcpy(destination, BACKUP_DIR);
	strcat(destination, "/");
	strcat(destination, backup);


	// If this file or subdirectory is inside of a directory that doesn't yet exist, then block until it does
	int length = strlen(destination);
	char* dependent = malloc(length + 1);
	struct stat temp;
	int slash_found = 0;
	int c;
	strcpy(dependent, destination);
	
	for (c = length - 1; c >= strlen(BACKUP_DIR) + 1; c--) {
		if (dependent[c] == '/') {
			slash_found = 1;
			dependent[c] = 0;
			break;
		}
	}
	
	if (slash_found) {
		while (1) {
			int result = lstat(dependent, &temp);
			if (result >= 0 && S_ISDIR(temp.st_mode))
				break;
		}
	}
	free(dependent);
	

	// Compare file modification times and only backup if the file needs to be backed up
	struct stat stat_source;
	struct stat stat_dest;
	int ra = lstat(info->value, &stat_source);
	int rb = lstat(destination, &stat_dest);

	if (ra != -1 && rb != -1 && stat_source.st_mtime <= stat_dest.st_mtime) {
		printf("[thread %u] Backup copy of %s is already up-to-date\n", (int)pthread_self(), info->value);
		int *ret = malloc(sizeof(int));
		*ret = -2;
		pthread_exit(ret);
		return NULL;
	}


	// If this is a subdirectory, then the process is MUCH simpler
	if (info->type == TYPE_SUBDIR) {
		free(destination);
		destination = malloc(strlen(info->value) + strlen(BACKUP_DIR) + 2);
		strcpy(destination, BACKUP_DIR);
		strcat(destination, "/");
		strcat(destination, info->value);
		mkdir(destination, 0777);
		printf("[thread %u] Backing up %s...\n", (int)pthread_self(), info->value);
		free(destination);
		free(info);

		int* ret = malloc(sizeof(int));
		*ret = -1;
		pthread_exit(ret);
		return NULL;
	}


	// Perform the backup process
	printf("[thread %u] Backing up %s...\n", (int)pthread_self(), info->value);
	if (access(destination, F_OK) != -1)
		printf("[thread %u] WARNING: %s exists (overwriting!)\n", (int)pthread_self(), backup);
	
	written = copy_file(info->value, destination);
	printf("[thread %u] Copied %d bytes from %s to %s\n", (int)pthread_self(), (int)written, info->value, backup);
	
	
	// Free allocated memory
	free(info);
	free(backup);
	free(destination);
	
	
	// Exit this thread
	int* ret = malloc(sizeof(int));
	*ret = (int)written;
	pthread_exit(ret);
	return NULL;
}


// Thread responsible for handling the restore of a single file or subdirectory
void* restore_thread(void* arg)
{
	if (arg == NULL)
		return NULL;

	struct list_node* info = (struct list_node*) arg;
	char* destination = NULL;
	ssize_t written;

	
	// Allocate memory for the destination string
	destination = malloc(strlen(BACKUP_DIR) + strlen(info->value) + 2);
	strcpy(destination, info->value);
	destination += strlen(BACKUP_DIR) + 1;
	destination[strlen(destination)-4] = '\0';


	// Prevent executable and source files from being restored
	if (!strcmp(destination, "mybackup") || !strcmp(destination, "main.c") || !strcmp(destination, "list.c") 
		|| !strcmp(destination, "list.h") || !strcmp(destination, "build")) {
		free(info);
		int* ret = malloc(sizeof(int));
		*ret = -2;
		pthread_exit(ret);
		return NULL;
	}


	// If this file or subdirectory is inside of a directory that doesn't yet exist, then block until it does
	int length = strlen(destination);
	char* dependent = malloc(length + 1);
	struct stat temp;
	int slash_found = 0;
	int c;
	strcpy(dependent, destination);
	
	for (c = length - 1; c >= 0; c--) {
		if (dependent[c] == '/') {
			slash_found = 1;
			dependent[c] = 0;
			break;
		}
	}
	if (slash_found) {
		while (1) {
			int result = lstat(dependent, &temp);
			if (result >= 0 && S_ISDIR(temp.st_mode))
				break;
		}
	}


	// If this is a subdirectory, then the process is MUCH simpler
	if (info->type == TYPE_SUBDIR) {
		destination = malloc(strlen(info->value) + strlen(BACKUP_DIR) + 2);
		strcpy(destination, info->value);
		destination += strlen(BACKUP_DIR) + 1;
		mkdir(destination, 0777);
		printf("[thread %u] Restoring %s...\n", (int)pthread_self(), destination);
		free(info);

		int* ret = malloc(sizeof(int));
		*ret = -1;
		pthread_exit(ret);
		return NULL;
	}


	// Perform the restore process
	printf("[thread %u] Restoring %s...\n", (int)pthread_self(), destination);
	written = copy_file(info->value, destination);
	char* backup_name = info->value + strlen(BACKUP_DIR) + 1;
	printf("[thread %u] Copied %d bytes from %s to %s\n", (int)pthread_self(), (int)written, backup_name, destination);
	

	// Free allocated memory and exit the thread
	free(info);
	int* ret = malloc(sizeof(int));
	*ret = (int)written;
	pthread_exit(ret);
	return NULL;
}


// Returns a list of all files and subdirectories in the given directory
struct list* get_items(char* directory, struct list* result)
{
	if (directory == NULL || result == NULL || strlen(directory) == 0)
		return NULL;
	
	DIR* dir = opendir(directory);
	struct dirent* entry;
	struct stat info;
	
	if (dir == NULL)
		return NULL;
	
	while ((entry = readdir(dir)) != NULL) {
		
		// Prepend the directory path and a slash to the name of the entry
		char* name = malloc(strlen(directory) + strlen(entry->d_name) + 2);

		if (strcmp(directory, ".")) { 
			strcpy(name, directory);
			strcat(name, "/");
			strcat(name, entry->d_name);
		}
		else strcpy(name, entry->d_name);
		
		// Attempt to perform a stat on the entry 
		if (lstat(name, &info) == -1) {
			free(name);
			continue;
		}
		
		// Set some flags
		int is_file = S_ISREG(info.st_mode);
		int is_dir = S_ISDIR(info.st_mode);
		int ignore = !strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")
			|| !strcmp(entry->d_name, BACKUP_DIR);
		
		// If it's a file or directory, then ...
		if (is_file || is_dir) {
			
			// If the entry refers to the current or parent directory, or the backup directory, then ignore it
			if (ignore) {
				free(name);
				continue;
			}
			
			// If it's a file, then record it
			if (is_file) {
				list_insert(result, name, info.st_size, TYPE_FILE);
			}
			
			// If it is a directory, then record it and recursively search inside of it
			if (is_dir) {
				list_insert(result, name, 0, TYPE_SUBDIR);
				get_items(name, result);
			}
		}
		
		// Free the allocated memory
		free(name);
	}
	closedir(dir);
	return result;
}


// Performs the backup process
void backup()
{
	struct list* items = list_new();
	struct list_node* node = NULL;
	pthread_t* tid_list = NULL;
	int counter = 0;
	int rc;
	
	// Build a list of all files and subdirectories from the current folder
	get_items(".", items);
	
	// If we have at least one entry, then point to the list head and allocate memory for the thread ID list
	if (items->size > 0) {
		node = items->head;
		tid_list = malloc(sizeof(pthread_t) * items->size);
	}
	
	// Iterate through all of the entries and create a new thread for each of them
	while (node != NULL) {
		struct list_node* copy = list_node_copy(node);
		if ((rc = pthread_create(&tid_list[counter++], NULL, backup_thread, copy)) != 0)
			fprintf(stderr, "pthread_create() failed (%d): %s", rc, strerror(rc));
		node = node->next;
	}
	
	// Wait for the threads to finish
	int c;
	int* r;
	int total = 0;
	int fileCount = 0, dirCount = 0;

	for (c = 0; c < items->size; c++) {
		pthread_join(tid_list[c], (void**) &r);

		if (*r >= 0) {
			total += *r;
			fileCount++;
		}
		else if (*r == -1) dirCount++;
		else if (*r == -2) { /* ignore, -2 means to ignore the return value */ }
		free(r);
	}


	// Display results
	printf("Successfully copied %d ", fileCount);
	if (fileCount == 1) printf("file ");
	else printf("files ");
	printf("(%d ", total);
	if (total == 1) printf("byte");
	else printf("bytes");
	printf(")");

	if (dirCount > 0) {
		printf(" and %d ", dirCount);
		if (dirCount == 1) printf("subdirectory");
		else printf("subdirectories");
		printf("\n");
	}
	else printf("\n");

	
	// Free allocated memory
	if (tid_list != NULL)
		free(tid_list);
	list_free(items);
}


// Performs the restoration process
void restore()
{
	struct list* items = list_new();
	struct list_node* node = NULL;
	pthread_t* tid_list = NULL;
	int counter = 0;
	int rc;
	
	// Build a list of all files and subdirectories from the backup directory
	get_items(BACKUP_DIR, items);
	
	// If we have at least one entry, then point to the list head and allocate memory for the thread ID list
	if (items->size > 0) {
		node = items->head;
		tid_list = malloc(sizeof(pthread_t) * items->size);
	}
	
	// Iterate through all of the entries and create a new thread for each of them
	while (node != NULL) {
		struct list_node* copy = list_node_copy(node);
		if ((rc = pthread_create(&tid_list[counter++], NULL, restore_thread, copy)) != 0)
			fprintf(stderr, "pthread_create() failed (%d): %s", rc, strerror(rc));
		node = node->next;
	}
	
	// Wait for the threads to finish
	int c;
	int* r;
	int total = 0;
	int fileCount = 0, dirCount = 0;

	for (c = 0; c < items->size; c++) {
		pthread_join(tid_list[c], (void**) &r);

		if (*r >= 0) {
			total += *r;
			fileCount++;
		}
		else if (*r == -1) dirCount++;
		else if (*r == -2) { /* ignore */ }
		free(r);
	}


	// Display results
	printf("Successfully restored %d ", fileCount);
	if (fileCount == 1) printf("file ");
	else printf("files ");
	printf("(%d ", total);
	if (total == 1) printf("byte");
	else printf("bytes");
	printf(")");

	if (dirCount > 0) {
		printf(" and %d ", dirCount);
		if (dirCount == 1) printf("subdirectory");
		else printf("subdirectories");
		printf("\n");
	}
	else printf("\n");
	
	// Free allocated memory
	if (tid_list != NULL)
		free(tid_list);
	list_free(items);
}


// Main function, entry point of the application
int main(int argc, char *argv[])
{
	// Check if the backup directory exists and store the result in a flag
	struct stat info;
	int backup_dir_exists = lstat(BACKUP_DIR, &info) != -1;
	
	
	// Either perform backup or restore, depending on the command line arguments (backup by default though)
	if (argc == 2 && (!strcmp(argv[1], "-r") || !strcmp(argv[1], "--restore"))) {
		if (!backup_dir_exists) {
			printf("ERROR: There is nothing to restore! Try creating a backup first. \n");
			return EXIT_FAILURE;
		}
		restore();
	}
	
	
	// Perform a backup operation
	else {
		if (!backup_dir_exists)
			mkdir(BACKUP_DIR, 0777);
		backup();
	}
	
	
	// Return exit success
	return EXIT_SUCCESS;
}
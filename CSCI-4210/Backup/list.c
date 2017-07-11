/**
 * Homework 3: File Backup Utility
 * Ryan Lin (linr2@rpi.edu)
 * File: list.c
**/

#include <stdlib.h>
#include <string.h>
#include "list.h"


// Allocates memory for a new list structure and returns a pointer to it
struct list* list_new()
{
	struct list* x = malloc(sizeof(struct list));
	if (x == NULL)
		return NULL;
	
	x->head = NULL;
	x->size = 0;
	return x;
}


// Frees all allocated memory belonging to a list structure
void list_free(struct list* x)
{
	if (x == NULL)
		return;
	
	struct list_node* node = x->head;
	struct list_node* temp = NULL;
	
	while (node != NULL) {
		temp = node;
		node = node->next;
		list_node_free(temp);
	}
	free(x);
}


// Inserts a new value to the end of the list
void list_insert(struct list* x, char* value, int size, int type)
{
	if (x == NULL || value == NULL || strlen(value) == 0 || size < 0 || type <= 0)
		return;
	
	// Create the new node
	struct list_node* node = malloc(sizeof(struct list_node));
	node->value = malloc(strlen(value) + 1);
	strcpy(node->value, value);
	node->size = size;
	node->type = type;
	node->next = NULL;
	
	// Add it to the end of the list
	if (x->size > 0) {
		struct list_node* current = x->head;
		while (current->next != NULL)
			current = current->next;
		current->next = node;
	}
	else x->head = node;
	x->size++;
}


// Makes an exact copy of a list_node structure and returns it
struct list_node* list_node_copy(struct list_node* x)
{
	if (x == NULL)
		return NULL;
	
	struct list_node* node = malloc(sizeof(struct list_node));
	node->size = x->size;
	node->type = x->type;
	node->next = x->next;
	node->value = NULL;
	
	if (x->value != NULL) {
		node->value = malloc(strlen(x->value) + 1);
		strcpy(node->value, x->value);
	}
	return node;
}


// Frees all allocated memory belonging to a list_node structure
void list_node_free(struct list_node* x)
{
	if (x == NULL)
		return;
	
	if (x->value != NULL)
		free(x->value);
	free(x);
}
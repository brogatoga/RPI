/**
 * Homework 3: File Backup Utility
 * Ryan Lin (linr2@rpi.edu)
 * File: list.h
**/

#ifndef LIST_H
#define LIST_H

#define TYPE_FILE    1
#define TYPE_SUBDIR  2
#define TYPE_OTHER   3


// One node in the linked list
struct list_node
{
	char* value;					// File path
	int size;						// Size in bytes
	int type;						// Type of entry 
	struct list_node* next;			// Pointer to next node
};


// Linked list of multiple nodes
struct list
{
	struct list_node* head;
	int size;
};


// Function declarations
struct list* list_new();
void list_free(struct list*);
void list_insert(struct list*, char*, int, int);

struct list_node* list_node_copy(struct list_node*);
void list_node_free(struct list_node*);

#endif
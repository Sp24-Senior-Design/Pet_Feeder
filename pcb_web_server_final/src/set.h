#ifndef SET_H
#define SET_H

#include <stdbool.h>

// Define the structure of a node in the linked list
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Function prototypes
Node* createNode(int data);
void insert(Node** head, int data);
void erase(Node** head, int data);
bool contains(Node* head, int data);
bool empty(Node* head);
void display(Node* head);

#endif /* SET_H */
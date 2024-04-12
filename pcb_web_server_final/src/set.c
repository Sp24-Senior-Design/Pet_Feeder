#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "set.h"

// Function to create a new node
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to insert a new element into the set
void insert(Node** head, int data) {
    // Check if the element already exists
    Node* current = *head;
    while (current != NULL) {
        if (current->data == data) {
            printf("Element already exists in the set\n");
            return;
        }
        current = current->next;
    }

    // Insert the new element at the beginning of the list
    Node* newNode = createNode(data);
    newNode->next = *head;
    *head = newNode;
}

// Function to erase an element from the set
void erase(Node** head, int data) {
    // Check if the set is empty
    if (*head == NULL) {
        printf("Set is empty\n");
        return;
    }

    // If the element to be erased is the first node
    if ((*head)->data == data) {
        Node* temp = *head;
        *head = (*head)->next;
        free(temp);
        printf("Element %d erased from the set\n", data);
        return;
    }

    // Find the previous node of the node to be deleted
    Node* prev = NULL;
    Node* current = *head;
    while (current != NULL && current->data != data) {
        prev = current;
        current = current->next;
    }

    // If the element is not present in the set
    if (current == NULL) {
        printf("Element %d not found in the set\n", data);
        return;
    }

    // Unlink the node from the linked list
    prev->next = current->next;
    free(current);
    printf("Element %d erased from the set\n", data);
}

// Function to check if the set contains a given element
bool contains(Node* head, int data) {
    Node* current = head;
    while (current != NULL) {
        if (current->data == data)
            return true;
        current = current->next;
    }
    return false;
}

// Function to check if the set is empty
bool empty(Node* head) {
    return head == NULL;
}

// Function to display the elements in the set
void display(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}
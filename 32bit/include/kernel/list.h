/************************************************************
 *                                                          *
 *                ~ SimpleOS - list.h ~                     *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Generic doubly-linked list implementation using macros. *
 *  Zero-overhead container-of pattern.                     *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add iterator macros and remove-from-list helper   *
 ************************************************************/

#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdint.h>

/**
 * Defines a list container type for a given node type.
 * Example: DEFINE_LIST(page) → page_list_t
 */
#define DEFINE_LIST(nodeType) \
    typedef struct nodeType##_list { \
        struct nodeType *head; \
        struct nodeType *tail; \
        uint32_t size; \
    } nodeType##_list_t;

/**
 * Inserts the next/prev pointers into the node struct.
 * Must be placed inside the struct definition.
 */
#define DEFINE_LINK(nodeType) \
    struct nodeType *next##nodeType; \
    struct nodeType *prev##nodeType;

/**
 * Initialises a list container to empty state.
 */
#define INITIALIZE_LIST(list) \
    do { \
        (list).head = NULL; \
        (list).tail = NULL; \
        (list).size = 0; \
    } while (0)

/**
 * Implements all list operations for a specific node type.
 * Must be placed in exactly one .c file to avoid duplicate
 * symbols.
 */
#define IMPLEMENT_LIST(nodeType) \
\
/** Appends a node to the end of the list */ \
void append_##nodeType##_list(nodeType##_list_t *list, struct nodeType *node) { \
    node->next##nodeType = NULL; \
    node->prev##nodeType = NULL; \
    \
    if (list->tail == NULL) { \
        list->head = node; \
        list->tail = node; \
    } else { \
        node->prev##nodeType = list->tail; \
        list->tail->next##nodeType = node; \
        list->tail = node; \
    } \
    list->size += 1; \
} \
\
/** Prepends a node to the beginning of the list */ \
void push_##nodeType##_list(nodeType##_list_t *list, struct nodeType *node) { \
    node->next##nodeType = list->head; \
    node->prev##nodeType = NULL; \
    list->head = node; \
    list->size += 1; \
    \
    if (list->tail == NULL) { \
        list->tail = node; \
    } \
} \
\
/** Returns the head node without removing it */ \
struct nodeType *peek_##nodeType##_list(nodeType##_list_t *list) { \
    return list->head; \
} \
\
/** Removes and returns the head node */ \
struct nodeType *pop_##nodeType##_list(nodeType##_list_t *list) { \
    struct nodeType *res; \
    \
    if (list->head == NULL) { \
        return NULL; \
    } \
    \
    res = list->head; \
    list->head = res->next##nodeType; \
    \
    if (list->head != NULL) { \
        list->head->prev##nodeType = NULL; \
    } else { \
        list->tail = NULL; \
    } \
    \
    list->size -= 1; \
    res->next##nodeType = NULL; \
    res->prev##nodeType = NULL; \
    \
    return res; \
} \
\
/** Returns current list size */ \
uint32_t size_##nodeType##_list(nodeType##_list_t *list) { \
    return list->size; \
} \
\
/** Returns the next node after the given node */ \
struct nodeType *next_##nodeType##_list(struct nodeType *node) { \
    return node->next##nodeType; \
}

#endif /* LIST_H */
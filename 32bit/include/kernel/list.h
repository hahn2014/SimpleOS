/**
 * Generic doubly-linked list implementation using macros.
 *
 * Usage:
 * 1. In a header (or .c file before use): DEFINE_LIST(mytype)
 *    - Creates mytype_list_t
 * 2. Inside the struct definition: DEFINE_LINK(mytype)
 *    - Adds mytype * nextmytype; mytype * prevmytype;
 * 3. In one .c file: IMPLEMENT_LIST(mytype)
 *    - Defines all the list functions for mytype
 * 4. To initialize: INITIALIZE_LIST(mylist);
 */

#include <stddef.h>
#include <stdint.h>

#ifndef LIST_H
#define LIST_H

/* Define the list container type */
#define DEFINE_LIST(nodeType) \
    typedef struct nodeType##_list { \
        struct nodeType *head; \
        struct nodeType *tail; \
        uint32_t size; \
    } nodeType##_list_t;

/* Define the next/prev pointers inside the node struct */
#define DEFINE_LINK(nodeType) \
    struct nodeType *next##nodeType; \
    struct nodeType *prev##nodeType;

/* Safe multi-statement initializer */
#define INITIALIZE_LIST(list) \
    do { \
        (list).head = NULL; \
        (list).tail = NULL; \
        (list).size = 0; \
    } while (0)

/* Implement all list operations for the given nodeType */
#define IMPLEMENT_LIST(nodeType) \
\
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
struct nodeType *peek_##nodeType##_list(nodeType##_list_t *list) { \
    return list->head; \
} \
\
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
uint32_t size_##nodeType##_list(nodeType##_list_t *list) { \
    return list->size; \
} \
\
struct nodeType *next_##nodeType##_list(struct nodeType *node) { \
    return node->next##nodeType; \
}

#endif
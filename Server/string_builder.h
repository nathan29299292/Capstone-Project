#include <stdlib.h>
#include <string.h>
#include "util.h"

#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

typedef struct string_builder_t {
    char* head;
    unsigned long long virtual_length;
    unsigned long long physical_size;
} string_builder_t;

#define INITIAL_STRING_BUILDER_SIZE 16

string_builder_t* string_builder_init() {
    string_builder_t* new_string_builder = (string_builder_t*)malloc(sizeof(string_builder_t));
    if (new_string_builder == NULL) {
        return NULL;
    }
    new_string_builder->head = (char*)malloc(sizeof(char) * INITIAL_STRING_BUILDER_SIZE);
    if (new_string_builder->head == NULL) {
        free(new_string_builder);
        return NULL;
    }

    new_string_builder->physical_size = INITIAL_STRING_BUILDER_SIZE;
    new_string_builder->virtual_length = 0;

    *new_string_builder->head = '\0'; /* Ensure the head is always 0. */
    return new_string_builder;
}

BOOL string_builder_appends(string_builder_t* string_builder, const char* append) {
    if (string_builder == NULL || append == NULL) {
        return NO;
    }

    unsigned long long append_length = (unsigned long long)strlen(append); /* Length is generally going to be negledgable since we are going to append strings ~<64 characters. The excess size is fine so long as we avoid excessive reallocs. */

    unsigned long long old_size = string_builder->virtual_length;
    unsigned long long new_size = string_builder->virtual_length + append_length;
    unsigned long long new_size_null_term = string_builder->virtual_length + append_length + 1;

    if (new_size_null_term > string_builder->physical_size) {
        /* Make sure the size fits the entire appended length. */
        unsigned long long new_physical_size = string_builder->physical_size;

        while (new_size_null_term /*Include the null terminator so the new head is garenteed to include a null term.*/ >= new_physical_size) {
            new_physical_size <<= 1;
        }

        /* Resize the string itself. */
        char* new_head = (char*)realloc(string_builder->head, sizeof(char) * new_physical_size);
        if (new_head == NULL) {
            return NO;
        }

        /* Change some of the variables. */
        string_builder->head = new_head;
        string_builder->physical_size = new_physical_size;
    }
    char* first = string_builder->head + old_size;
    char* last = string_builder->head + new_size; /* This should go to the last character. */
    /* Copy the string to append to the head. This will not include the null terminator. */
    memcpy(first, append, append_length);

    /* Ensure that the last value of the head list is a null terminator, we set this after the memcopied characters. The resize should have considered this. */
    *last = '\0';
    string_builder->virtual_length = new_size;
    return YES;
}

BOOL string_builder_appendsb(string_builder_t* string_builder, const string_builder_t* append) {
    if (string_builder == NULL || append == NULL) {
        return NO;
    }

    unsigned long long append_length = (unsigned long long)append->virtual_length; /* Length is generally going to be negledgable since we are going to append strings ~<64 characters. The excess size is fine so long as we avoid excessive reallocs. */

    unsigned long long old_size = string_builder->virtual_length;
    unsigned long long new_size = string_builder->virtual_length + append_length;
    unsigned long long new_size_null_term = string_builder->virtual_length + append_length + 1;

    if (new_size_null_term > string_builder->physical_size) {
        /* Make sure the size fits the entire appended length. */
        unsigned long long new_physical_size = string_builder->physical_size;

        while (new_size_null_term /*Include the null terminator so the new head is garenteed to include a null term.*/ >= new_physical_size) {
            new_physical_size <<= 1;
        }

        /* Resize the string itself. */
        char* new_head = (char*)realloc(string_builder->head, sizeof(char) * new_physical_size);
        if (new_head == NULL) {
            return NO;
        }

        /* Change some of the variables. */
        string_builder->head = new_head;
        string_builder->physical_size = new_physical_size;
    }
    char* first = string_builder->head + old_size;
    char* last = string_builder->head + new_size; /* This should go to the last character. */
    /* Copy the string to append to the head. This will not include the null terminator. */
    memcpy(first, append->head, append_length);

    /* Ensure that the last value of the head list is a null terminator, we set this after the memcopied characters. The resize should have considered this. */
    *last = '\0';
    string_builder->virtual_length = new_size;
    return YES;
}

/* Returns a string linked to the string builder, this string is tied to the
 * lifetime of the string_builder. Using this string after the string_builder
 * is removed will result in undefined behavior. */
char* string_builder_lwrap(string_builder_t* string_builder) {
    return string_builder->head;
}

/* Returns the size of the string (excluding the null term) */
unsigned long long string_builder_size(string_builder_t* string_builder) {
    return string_builder->virtual_length;
}

void string_builder_destroy(string_builder_t* string_builder) {
    free(string_builder->head);
    free(string_builder);
}

#endif

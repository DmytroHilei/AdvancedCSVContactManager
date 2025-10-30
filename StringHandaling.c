#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "StringHandaling.h"
#include <ctype.h>

int Len(const char *string) {
    int len = 0;
    for (int i = 0; string[i] != '\0'; i++) {
        len++;

    }
    return len;
}

void StrCopy(const char* string1, char* string2) {
    int i;
    for (i = 0; string1[i] != '\0'; i++) {
        string2[i] = string1[i];
    }
    string2[i] = '\0';
}
char* InputString(const char* prompt) {
    char buffer[256];

    printf("%s", prompt);
    fgets(buffer, 256, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    char* result = malloc(Len(buffer) + 1);
    if (result == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    StrCopy(buffer, result);
    TrimString(result);

    return result;
}

void TrimString(char *s) {
    char *end;
    while (isspace((unsigned char)*s)) s++;
    if (*s == 0) return;
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}



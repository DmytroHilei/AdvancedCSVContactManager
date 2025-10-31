#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "StringHandaling.h"
#include "Validation.h"

#include "ContactManager.h"


int IsValidEmail(const char *email) {
    if (Len(email) == 0) return 0;
    return (strchr(email, '@') != NULL && strchr(email, '.') != NULL);
}

int IsValidPhoneNumber(const char *phone, const char *country) {
    int len = Len(phone);
    if (len == 0 || len > 20) return 0;

    for (int i = 0; i < len; i++) {
        if (!isdigit(phone[i]) && phone[i] != '+' && phone[i] != '-' && phone[i] != ' ')
            return 0;
    }

    if (!IsValidCountryCode(country, phone)) {
        return 0;
    }

    return 1;
}


int IsValidCountryCode(const char *country, const char *phone) {
    FILE *file = fopen("C:\\Users\\giley\\Downloads\\CountriesCodes.txt", "r");
    if (file == NULL) {
        printf("File could not be opened\n");
        return 0;
    }

    char fileCountry[100];
    char code[20];
    int isValid = 0;

    while (*phone == ' ') phone++;

    while (fscanf(file, "%99s %19s", fileCountry, code) == 2) {
        code[strcspn(code, "\r\n")] = '\0';

        if (strcasecmp(fileCountry, country) == 0) {
            const char *normalizedPhone = (phone[0] == '+') ? phone + 1 : phone;
            const char *normalizedCode  = (code[0] == '+')  ? code + 1  : code;

            if (strncmp(normalizedPhone, normalizedCode, strlen(normalizedCode)) == 0) {
                isValid = 1;
            }
            break;
        }
    }

    fclose(file);
    return isValid;
}

int IsValidString(const char *name) {
    for (int i = 0; i < Len(name); i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ' && name[i] != '-') return 0;
    }
    return 1;
}

char* GetValidInput(const char *prompt, int (*validator)(const char *)) {
    char* s;

    do {
        s = InputString(prompt);
        if (!validator(s)) {
            printf("Invalid input.\n");
            free(s);
        }
    } while (!validator(s));
    return s;
}


int IsDuplicateEmail(Contact *contacts, int *size, const char *email) {
    for (int i = 1; i < *size; i++) {
        if (strcmp(contacts[i].email, email) == 0) {
            return 1;
        }
    }
    return 0;
}
int IsDuplicatePhoneNumber(Contact *contacts, int *size, const char *phone) {
    for (int i = 1; i < *size; i++) {
        if (strcmp(contacts[i].phone, phone) == 0) {
            return 1;
        }
    }
    return 0;
}

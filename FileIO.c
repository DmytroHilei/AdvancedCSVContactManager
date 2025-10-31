#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ContactManager.h"
#include "FileIO.h"
#include "Validation.h"

int ReadFromTheCSV(const char *fileName, Contact **contacts, int *size) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("File could not be opened\n");
        return 0;
    }
    char line[512];
    int added = 0;
    fgets(line, sizeof(line), file);


    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';

        char *token;
        token = strtok(line, ",");
        if (!token) continue;
        Contact temp = {0};

        //------------Name---------------------------
        if (!IsValidString(token)) continue;
        temp.name = strdup(token);

        //------------email-----------------------
        token = strtok(NULL, ",");

        if (!token || !IsValidEmail(token) || IsDuplicateEmail(*contacts, size, token)) {
            free(temp.name);
            continue;
        }
        temp.email = strdup(token);

        //------------country-----------------
        token = strtok(NULL, ",");

        if (!token || !IsValidString(token)) {
            free(temp.name);
            free(temp.email);
            continue;
        }

        temp.country = strdup(token);
        //------------phone----------------

        token = strtok(NULL, ",");
        if (!token || !IsValidPhoneNumber(token, temp.country) || IsDuplicatePhoneNumber(*contacts, size, token)) {
            free(temp.name);
            free(temp.email);
            free(temp.country);
            continue;
        }

        temp.phone = strdup(token);
        //------------city-------------
        token = strtok(NULL, ",");

        if (!token || !IsValidString(token)) {
            free(temp.name);
            free(temp.email);
            free(temp.country);
            free(temp.phone);
            continue;
        }

        temp.city = strdup(token);

        *contacts = realloc(*contacts, sizeof(Contact) * ((*size) + 1));
        (*contacts)[(*size)++] = temp;

        added++;
    }

    fclose(file);
    return added;
}

int ReadFromTXT(const char *fileName, Contact **contacts, int *size) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("File could not be opened\n");
        return 0;
    }

    char name[100], email[100], phone[30], country[50], city[50];
    int added = 0;

    Contact temp = {0};
    while (fscanf(file, "%99s %99s %29s %49s %49s", name, email, country, phone, city) != EOF) {

        if (!IsValidString(name)) {
            continue;
        }
        temp.name = strdup(name);

        if (!IsValidEmail(email) || IsDuplicateEmail(*contacts, size, temp.name)) {
            free(temp.name);
            continue;
        }
        temp.email = strdup(email);

        if (!IsValidString(country)) {
            free(temp.name);
            free(temp.email);
            continue;
        }
        temp.country = strdup(country);

        if (!IsValidPhoneNumber(phone, country) || IsDuplicatePhoneNumber(*contacts, size, temp.country)) {
            free(temp.name);
            free(temp.email);
            free(temp.country);
            continue;
        }
        temp.phone = strdup(phone);


        if (!IsValidString(city)) {
            free(temp.name);
            free(temp.email);
            free(temp.country);
            free(temp.phone);
            continue;
        }
        temp.city = strdup(city);

        *contacts = realloc(*contacts, sizeof(Contact) * ((*size) + 1));
        (*contacts)[(*size)++] = temp;

        added++;

    }
    fclose(file);
    return added;
}

void WriteToTXT(const char *fileName, Contact **contacts, const int *size) {
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        printf("File could not be opened\n");
        return;
    }

    for (int i = 0; i < *size; i++) {
        Contact *contact = &(*contacts)[i];
        fprintf(file, "%s\n", contact->name ? contact->name : "");
        fprintf(file, "%s\n", contact->email ? contact->email : "");
        fprintf(file, "%s\n", contact->phone ? contact->phone : "");
        fprintf(file, "%s\n", contact->country ? contact->country : "");
        fprintf(file, "%s\n", contact->city ? contact->city : "");
    }
    fclose(file);
}


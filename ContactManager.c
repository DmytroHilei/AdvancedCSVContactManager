#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ContactManager.h"
#include "StringHandaling.h"
#include "Validation.h"


Contact *contacts = NULL;
int contactCount = 0;


Contact addContact() {
    Contact contact;

    contact.name = GetValidInput("Enter name: ", IsValidString);
    contact.email = GetValidInput("Enter email: ", IsValidEmail);
    contact.country = GetValidInput("Enter country: ", IsValidString);

    do {
        contact.phone = InputString("Enter phone: ");
        if (!IsValidPhoneNumber(contact.phone, contact.country)) {
            printf("Invalid phone number for country %s.\n", contact.country);
            free(contact.phone);
        }
    } while (!IsValidPhoneNumber(contact.phone, contact.country));

    contact.city = GetValidInput("Enter city: ", IsValidString);

    return contact;
}

void AddContact(Contact *contacts, int *size,
                const char *name, const char *email,
                const char *phone, const char *country,
                const char *city) {
    if (*size >= 100) {
        printf("[ERROR] Contact list full.\n");
        return;
    }

    Contact *c = &contacts[*size];

    // Allocate memory for strings (since your struct uses char*)
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->country = strdup(country);
    c->city = strdup(city);

    (*size)++;

    printf("[INFO] Web contact '%s' added successfully. Total: %d\n", name, *size);
}


void listContact(Contact *contacts, int* size) {
    if (size == 0) printf("Contact list is empty\n");
    printf("Contact list:\n");
    for (int i = 0; i < *size; i++) {
        printf("\nContact #%d\n", i + 1);
        printf("  Name : %s\n", contacts[i].name);
        printf("  Phone: %s\n", contacts[i].phone);
        printf("  Email: %s\n", contacts[i].email);
        printf("  Country: %s\n", contacts[i].country);
        printf("  City: %s\n", contacts[i].city);
    }
}


void FreeAllContacts(Contact *contacts, int* size) {
    if (size == 0) printf("Contact list is empty\n");
    for (int i = 0; i < *size; i++) {
        free(contacts[i].name);
        contacts[i].name = NULL;
        free(contacts[i].phone);
        contacts[i].phone = NULL;
        free(contacts[i].email);
        contacts[i].email = NULL;
        free(contacts[i].country);
        contacts[i].country = NULL;
        free(contacts[i].city);
        contacts[i].city = NULL;
    }
    free(contacts);
}

void FreeContact(Contact *contacts, int* size) {
    char* name;
    char buffer[256];

    if (size == 0) {
        printf("Contact list is empty\n");
        return;
    }

    printf("Enter the name\n");
    fgets(buffer, 256, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    name = malloc(Len(buffer) + 1);


    if (name == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    StrCopy(buffer, name);

    for (int i = 0; i < *size; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            free(contacts[i].name);
            free(contacts[i].email);
            free(contacts[i].phone);

            for (int j = i; j < *size - 1; j++) {
                contacts[j] = contacts[j + 1];
            }

            (*size)--;

            printf("contact %s is deleted succesfully!", name);
            free(name);
            return;
        }
    }
    printf("Contact %s is not found!", name);
}

void EditContact(Contact *contacts, int* size) {
    int choice;


    int i = SearchContact(contacts, size);

    if (i == -1) {
        return;
    }

    printf("Please, enter what value you want to edit\n");
    printf("variants:\n");
    printf("1 - Name\n");
    printf("2 - email\n");
    printf("3 - phone\n");
    printf("4 - country\n");
    printf("5 - city\n");
    int res = scanf("%d", &choice);

    if (res != 1) {
        printf("Invalid input, try again!\n");
    }
    while (getchar() != '\n') {}

    switch (choice) {
        case 1: {
            do {
                ReplaceField(&contacts[i].name, "Enter new name: ");
                if (!IsValidString(contacts[i].name)) {
                    printf("Invalid name. Try again.\n");
                    free(contacts[i].name);
                    contacts[i].name = NULL;
                }
            } while (!IsValidString(contacts[i].name));
            break;
        }

        case 2: {
            do {
                ReplaceField(&contacts[i].email, "Enter new email: ");
                if (!IsValidEmail(contacts[i].email)) {
                    printf("Invalid email. Try again.\n");
                    free(contacts[i].email);
                    contacts[i].email = NULL;
                }
            } while (!IsValidEmail(contacts[i].email));
            break;
        }

        case 3: {
            do {
                ReplaceField(&contacts[i].phone, "Enter new phone: ");
                if (!IsValidPhoneNumber(contacts[i].phone, contacts[i].country)) {
                    printf("Invalid phone number for %s. Try again.\n", contacts[i].country);
                    free(contacts[i].phone);
                    contacts[i].phone = NULL;
                }
            } while (!IsValidPhoneNumber(contacts[i].phone, contacts[i].country));
            break;
        }

        case 4: {
            do {
                ReplaceField(&contacts[i].country, "Enter new country: ");
                if (!IsValidString(contacts[i].country)) {
                    printf("Invalid country. Try again.\n");
                    free(contacts[i].country);
                    contacts[i].country = NULL;
                }
            } while (!IsValidString(contacts[i].country));
            break;
        }

        case 5: {
            do {
                ReplaceField(&contacts[i].city, "Enter new city: ");
                if (!IsValidString(contacts[i].city)) {
                    printf("Invalid city. Try again.\n");
                    free(contacts[i].city);
                    contacts[i].city = NULL;
                }
            } while (!IsValidString(contacts[i].city));
        }

        default: printf("Invalid choice.\n"); break;
    }

}

int SearchContact(Contact *contacts, const int* size) {
    if (*size == 0) {
        printf("Contact list is empty\n");
        return -1;
    }

    int choice;
    printf("Please, choose how do you want to search the contact\n");
    printf("variants:\n");
    printf("1 - Name\n");
    printf("2 - email\n");
    printf("3 - phone\n");
    scanf("%d", &choice);
    getchar();

    char *search = NULL;
    int i = -1;

    switch (choice) {
        case 1:
            search = InputString("Enter the name of the contact you want to search\n");
            for (int j = 0; j < *size; j++) {
                if (strcmp(contacts[j].name, search) == 0) {
                    i = j;
                    break;
                }
            }
            break;
        case 2:
            search = InputString("Enter the email of the contact you want to search\n");
            for (int j = 0; j < *size; j++) {
                if (strcmp(contacts[j].email, search) == 0) {
                    i = j;
                    break;
                }
            }
            break;
        case 3:
            search = InputString("Enter the phone of the contact you want to search\n");
            for (int j = 0; j < *size; j++) {
                if (strcmp(contacts[j].phone, search) == 0) {
                    i = j;
                    break;
                }
            }
            break;
        default:
            printf("Invalid choice\n");
            break;
    }
    free(search);
    return i;
}


void GetInfo(Contact *contacts, int* size) {
    int i = SearchContact(contacts, size);

    if (i == -1) {
        return;
    }

    printf("The contact's %s info:\n", contacts[i].name);
    printf("The Email: %s\n", contacts[i].email);
    printf("The phone number: %s\n", contacts[i].phone);
    printf("The country: %s\n", contacts[i].country);
    printf("The city: %s\n", contacts[i].city);
}

void ReplaceField(char **field, const char *promt) {
    char buffer[256];
    printf("%s", promt);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\r\n")] = '\0';

    char *newValue = malloc(strlen(buffer) + 1);
    if (!newValue) {
        printf("Memory could not be allocated\n");
        exit(1);
    }

    StrCopy(buffer, newValue);
    free(*field);
    *field = newValue;
}




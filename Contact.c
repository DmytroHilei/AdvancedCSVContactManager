#include <stdio.h>
#include <stdlib.h>

#include "ContactManager.h"
#include "StringHandaling.h"
#include "FileIO.h"
#include "Sorting.h"
#include "Contact.h"

void RunContactManager(void) {
    Contact *contacts = NULL;
    int contactsNumber = 0;

    int choice;

    int end = 0;
    while (!end) {
        printf("\n--- Contact Manager ---\n");
        printf("1. Add contact\n");
        printf("2. List contacts\n");
        printf("3. Free one contact\n");
        printf("4. Free All Contacts\n");
        printf("5. Get info about contact\n");
        printf("6. Sort the contacts by name\n");
        printf("7. Add contact from the excel (csv)\n");
        printf("8. Edit contact\n");
        printf("9. Read from txt\n");
        printf("10. Write to txt\n");
        printf("11. Exit\n");
        printf("Your choice: ");
        int res = scanf("%d", &choice);
        if (res != 1) {
            printf("Invalid input, try again\n");
        }
        while (getchar() != '\n') {}

        switch (choice) {
            case 1:
                Contact newContact = addContact();
                contacts = realloc(contacts, (contactsNumber + 1) * sizeof(Contact));

                contacts[contactsNumber] = newContact;
                contactsNumber++;
                break;
            case 2:
                listContact(contacts, &contactsNumber);
                break;
            case 3:
                FreeContact(contacts, &contactsNumber);
                break;
            case 4:
                FreeAllContacts(contacts, &contactsNumber);
                break;
            case 5:
                GetInfo(contacts, &contactsNumber);
                break;
            case 6:
                MergeSort(contacts, 0, contactsNumber - 1);
                break;
            case 7:
                char *fileName = InputString("Enter the name of the csv file: ");
                int added = ReadFromTheCSV(fileName, &contacts, &contactsNumber);
                printf("%d contacts added from file.\n", added);
                free(fileName);
                break;
            case 8:
                EditContact(contacts, &contactsNumber);
                break;
            case 9:
                char *textfileName = InputString("Enter the name of the txt file: ");
                int textAdded = ReadFromTXT(textfileName, &contacts, &contactsNumber);
                printf("%d contacts added from file.\n", textAdded);
                free(textfileName);
                break;
            case 10:
                char *TXTFileName = InputString("Enter the name of the txt file "
                                                "where you want to store the contacts: ");
                WriteToTXT(TXTFileName, &contacts, &contactsNumber);
                printf("Your contacts have been succesfully added!\n");
                break;
            case 11:
                end = 1;
                break;
            default:
                printf("Invalid choice\n");
        }
    }
}
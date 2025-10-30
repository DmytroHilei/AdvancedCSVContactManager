#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *country;
    char *city;
} Contact;

void FreeContact(Contact *contacts, int* size);
void FreeAllContacts(Contact *contacts, int* size);
void listContact(Contact *contacts, int* size);
Contact addContact();
void EditContact(Contact *contacts, int* size);
int SearchContact(Contact *contacts, const int* size);
void GetInfo(Contact *contacts, int* size);
void ReplaceField(char **field, const char *promt);

void AddContact(Contact *contacts, int *size,
                const char *name, const char *email,
                const char *phone, const char *country,
                const char *city);  // Web version
#endif //CONTACTMANAGER_H

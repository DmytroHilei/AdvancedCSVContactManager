#include <stdlib.h>
#include "ContactManager.h"
#include <ctype.h>

#include "Sorting.h"

int CompareContacts(const Contact *a, const Contact *b) {
    unsigned char c1 = tolower((unsigned char)a->name[0]);
    unsigned char c2 = tolower((unsigned char)b->name[0]);
    return c1 - c2;
}

void Merge(Contact* contacts, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Contact *L = malloc(sizeof(Contact) * n1);
    Contact *R = malloc(sizeof(Contact) * n2);

    for (int i = 0; i < n1; i++) {
        L[i] = contacts[left + i];
    }
    for (int i = 0; i < n2; i++) {
        R[i] = contacts[mid + 1 + i];
    }

    int i, j, k;

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2) {
        if (CompareContacts(&L[i], &R[j]) <= 0) {
            contacts[k] = L[i];
            i++;
        }
        else {
            contacts[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        contacts[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        contacts[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void MergeSort(Contact *contacts, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        MergeSort(contacts, left, mid);
        MergeSort(contacts, mid + 1, right);

        Merge(contacts, left, mid, right);
    }
}

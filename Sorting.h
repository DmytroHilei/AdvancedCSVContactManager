//
// Created by giley on 26/10/2025.
//

#ifndef SORTING_H
#define SORTING_H
int CompareContacts(const Contact *a, const Contact *b);
void Merge(Contact* contacts, int left, int mid, int right);
void MergeSort(Contact *contacts, int left, int right);
#endif //SORTING_H

//
// Created by giley on 26/10/2025.
//
#pragma once
#include "ContactManager.h"
#ifndef VALIDATION_H
#define VALIDATION_H
int IsValidEmail(const char *email);
int IsValidPhoneNumber(const char *phone, const char *country);
int IsValidCountryCode(const char *country, const char *phone);
int IsValidString(const char *name);
char* GetValidInput(const char *prompt, int (*validator)(const char *));
int IsDuplicateEmail(Contact *contacts, int *size, const char *email);
int IsDuplicatePhoneNumber(Contact *contacts, int *size, const char *phone);
#endif //VALIDATION_H

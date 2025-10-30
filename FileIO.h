#ifndef FILEIO_H
#define FILEIO_H
int ReadFromTheCSV(const char *fileName, Contact **contacts, int *size);
int ReadFromTXT(const char *fileName, Contact **contacts, int *size);
void WriteToTXT(const char *fileName, Contact **contacts, const int *size);
#endif //FILEIO_H

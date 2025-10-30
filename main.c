#include <stdio.h>
#include "WebServer.h"
#include "Contact.h"

int main() {
    printf("Choose mode:\n 1 - local server \n 2 - terminal\n");
    int mode;
    printf("Your choice: ");
    int res = scanf("%d", &mode);
    if (res != 1) {
        printf("Invalid choice\n");
    }
    while (getchar() != '\n') {}

    if (mode == 1) {
        printf("[INFO] Server terminated.\n");
        StartWebServer();
    }
    else if (mode == 2) {
        printf("[INFO] Client terminated.\n");
        RunContactManager();    // your existing menu loop
    }
    else {
        printf("[ERROR] Invalid choice\n");
    }

    return 0;
}



/*To Do:
Check the data from the csv done
Check data from txt
Show all data in get info function done
Add write to csv
Add write to txt done
Check for dublicates
Cosmetic view done
use const where needed done
Split to multiple C done
Fix delete all contacts (with null replaciing) done
Make sorting to compare the whole names
Allow to change each element (mail, name so on) done
make search using email, phone and so on done
Add safety for data (simple C cybersecurity)
Use valid.c to check the data from files

Framework to do beautiful GUI

download the csv (+write to csv)
download the txt (+ write to txt)
check the data
        From user
        Fron CSV
        From TXT
Check for duplicates
Create the menu (not force the user to input t himsefl)
Safety (I guess it is way more complicated with webserver)

Detailed commects for each imprtant part of the code

After all of this improvements
Create readme
Push on github
*/

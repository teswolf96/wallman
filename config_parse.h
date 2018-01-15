#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_profiles_new(){

    char *fileName = strcat(getenv("HOME"), "/.config/wallman/config");
    FILE *config = fopen(fileName, "r"); /* should check the result */

    if (config == NULL) {
        fileName = "/etc/wallman";
        config = fopen(fileName, "r");
        if (config == NULL) {
            printf("File could not be opened\n");
            return -1;
        }
    }



}
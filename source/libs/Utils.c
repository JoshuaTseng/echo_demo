#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include "Utils.h"

int string_to_int(char *str) {
    return atoi(str);
}

char * timestamp(){
    time_t now = time(NULL); 
    char * time = asctime(gmtime(&now));
    time[strlen(time)-1] = '\0';    // Remove \n
    return time;
}

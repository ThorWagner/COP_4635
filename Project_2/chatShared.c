#include "chatShared.h"

void showError(char *errorMSG){

    fprintf(stderr, "\n%s\n\n", errorMSG);
    exit(EXIT_FAILURE);

}


/** @file chatShared.c
 *  @brief Program file for shared functionality between Client and Server.
 *
 *  This file contains a basic function which simplifies displaying error
 *  message during Client and Server initializations.
 *
 *  @author Michael Wagner
 *  @date 04/09/2019
 *  @info Course COP 4635 - Project 2
 *  @bug No known bugs.
 */

#include "chatShared.h"

void showError(char *errorMSG){

    fprintf(stderr, "\n%s\n\n", errorMSG);
    exit(EXIT_FAILURE);

}


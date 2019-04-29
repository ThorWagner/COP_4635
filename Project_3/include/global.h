/** @file global.h
 *  @brief Global variables used by the various Server threads
 *
 *  This file contains the global defintions for variables used by various
 *  threads launched and operated by the FTP Server. All global variables are
 *  initialized by the Server main() function contained in 'ftpServer.c'.
 *
 *  @author Michael Wagner
 *  @date 04/28/2019
 *  @info Course COP 4635
 *  @bug No known bugs.
 */

#ifndef FTP_SHARED_H
#define FTP_SHARED_H

    /* -- Global Variables -- */

    // Flag to stop threaded handling of clients
    bool g_running;

    // Counter to keep track of active users
    int g_current;

    // Counter to keep track of total visitors
    int g_all;

#endif /* FTP_SHARED_H */


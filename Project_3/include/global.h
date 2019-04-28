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

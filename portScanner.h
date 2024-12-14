/***************************************************************************************
 * Filename: portScanner.h                                                       
 * Description: This file acts as a header for portScanner.c which handles all the 
 * logic for scanning across the host and identifying open ports.                                 
 * Author: Alion Bujku                                                    
 * Date: 12/11/2024                                                       
 * Command line 
 ***************************************************************************************/

#include <arpa/inet.h>

#ifndef SCANNER_H
#define SCANNER_H

// Create struct for thread
typedef struct {
    char ip[INET_ADDRSTRLEN];
    int startPort;
    int endPort;
} ThreadData;

// Function for scanning ports  
void* scanPorts(void* arg);

#endif
/***************************************************************************************
 * Filename: portScanner.c                                                       
 * Description: This file handles all the logic for scanning across the host and
 * identifying open ports.                                 
 * Author: Alion Bujku                                                    
 * Date: 12/11/2024                                                       
 * Command line 
 ***************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h> 
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "portScanner.h"

// Function to scan ports within given range
void* scanPorts(void* arg) {
    ThreadData* data = (ThreadData*)arg; //Cast argument back to thread

    // Create and open a file to output the open terminals
    FILE* scanFile = fopen("scan_results.txt", "w");
        if (scanFile == NULL) {
            perror("Failed to open log file");
            return NULL;
        }

    // Loop through each port in the given range
    for (int port = data->startPort; port <= data->endPort; port++) {
        // Create socket & attempt to connect to port
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Socket creation failed");
            continue; // Skip to next socket if failed
        } 

        // Setting timeout for socket connection
        struct timeval timeout;
        timeout.tv_sec = 3; // Set timeout to 2 seconds
        timeout.tv_usec = 0;

        // Apply timeout settings for both sending and recieving
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        // Configure the target address for connection
        struct sockaddr_in target;
        target.sin_family = AF_INET; // Only searching for IPv4
        target.sin_port = htons(port); // Convert port number to network byte order
        inet_pton(AF_INET, data->ip, &target.sin_addr); // Convert IP to binary

       // Attempt to connect to open ports
       if (connect(sock, (struct sockaddr*)&target, sizeof(target)) == 0) {
            printf("Port %d is open\n", port);
            fprintf(scanFile, "Port %d is open\n", port);

        } else {
            // Error handling
            if (errno == ECONNREFUSED) {
                printf("Port %d is closed (connection refused)\n", port);
            } else if (errno == ETIMEDOUT) {
                printf("Port %d is closed (connection timed out)\n", port);
            } else if (errno == EINPROGRESS) {
                printf("Port %d is unreachable (operation still in progress)\n", port);
            } else {
                printf("Port %d failed with error: %s\n", port, strerror(errno));
            }
        }
        // Close out socket
        close(sock);
    }

    // Close the log file    
    fclose(scanFile);
    return NULL;

}
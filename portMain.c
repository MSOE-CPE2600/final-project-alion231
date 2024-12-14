/***************************************************************************************
 * Filename: portMain.c                                                       
 * Description: Given a hostname, this program will actively scan all ports within
 * the given ports and return any port that is open and accepting TCP connections.
 * This file will handle all the user input.                                 
 * Author: Alion Bujku                                                    
 * Date: 12/11/2024                                                       
 * Command line 
 ***************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h> 
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "portScanner.h"



int main(int argc, char* argv[]) {
    // Read user input from command line
    char* hostname = argv[1]; // Target hostname
    int portStart = atoi(argv[2]); // Starting port
    int portEnd = atoi(argv[3]); // Ending port
    int numThreads = atoi (argv[4]); // Number of threads to use

    //Ensure valid ports are being entered
    if (portStart <= 0 || portEnd <= 0 || portStart > portEnd) {
        printf("Invalid port range. Ensure that ports are positive integers and that "
        "starting port has a value less than the ending port.\n");
        return 1;
    }

    // Ensure there is a valid input for threads
    if (numThreads <= 0) {
        printf("Number of threads must be greater than zero.\n");
        return 1;
    }

    // Resolve the hostname to an IPv4 address
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Use IPv4
    hints.ai_socktype = SOCK_STREAM; // Use TCP

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    // Convert resolved IP address to string format
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in *addr = (struct sockaddr_in*)res->ai_addr;
    inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
    printf("Resolved hostname: %s to IP: %s\n", hostname, ip);

    freeaddrinfo(res); // Free memory allocated by getaddrinfo

    // Calculate total ports
    int totalPorts = portEnd - portStart + 1;

    if (totalPorts <= 0) {
        printf("No ports to scan. Exititing.\n");
        return 1;
    }

    // Creating threads
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    // Adjust number of threads if more threads are specified than ports
    if (numThreads > totalPorts) {
        printf("Too many threads for the given port range. Reducing threads to %d.\n", totalPorts);
        numThreads = totalPorts;
    }

    int portsPerThread = totalPorts / numThreads;

    // Assign prot ranges to each thread
    for (int i = 0; i < numThreads; i++) {
        threadData[i].startPort = portStart + i * portsPerThread;
        threadData[i].endPort = (i == numThreads - 1) ? portEnd : threadData[i].startPort + portsPerThread - 1;
        strcpy(threadData[i].ip, ip);

        // Create a thread to scan the assigned port range
        if(pthread_create(&threads[i], NULL, scanPorts, &threadData[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
}
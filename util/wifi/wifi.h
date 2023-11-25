#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

// TCP server configuration
#define TCP_PORT 4242
#define DEBUG_PRINTF printf
#define BUF_SIZE 16
#define TEST_ITERATIONS 10
#define POLL_TIME_S 5

// Structure to represent the TCP server
typedef struct TCP_SERVER_T_ {
    struct tcp_pcb *server_pcb;    // Server PCB (Protocol Control Block)
    struct tcp_pcb *client_pcb;    // Client PCB (Protocol Control Block)
    bool complete;                 // Flag indicating if the communication is complete
    uint8_t buffer_sent[BUF_SIZE]; // Buffer for sent data
    uint8_t buffer_recv[BUF_SIZE]; // Buffer for received data
    int sent_len;                  // Length of the sent data
    int recv_len;                  // Length of the received data
    int run_count;                 // Count of server run iterations
} TCP_SERVER_T;

/**
 * @brief Initializes the Wi-Fi connection.
 *
 * @param wifiEnabled Pointer to a boolean indicating if Wi-Fi is enabled.
 */
void initializeWiFi(bool *wifiEnabled);

#endif  // TCP_SERVER_H
#ifndef TLS_SERVER_H
#define TLS_SERVER_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <time.h>
#include <cjson/cJSON.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define WEBSOCKET_MAGIC_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// Function declarations
void websocket_server();
int handle_handshake(SSL *ssl);
void send_frame(SSL *ssl, const char *message);
void receive_frame(SSL *ssl, char *buffer);

char *base64_encode(const unsigned char *input, int length);

#endif



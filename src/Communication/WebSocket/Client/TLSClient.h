#ifndef TLS_CLIENT_H
#define TLS_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <cjson/cJSON.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define WEBSOCKET_MAGIC_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// WebSocket helper functions
void send_handshake_request(SSL *ssl);
int process_handshake_response(SSL *ssl);
void send_frame(SSL *ssl, const char *message);
void receive_frame(SSL *ssl, char *buffer);
char *base64_encode(const unsigned char *input, int length);

#endif

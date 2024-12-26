#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <cjson/cJSON.h>
#include <time.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define WEBSOCKET_MAGIC_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

static const int mod_table[] = {0, 2, 1};


// Helper to Base64 encode
char *base64_encode(const unsigned char *input, int length) {
    static const char encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *encoded_data = malloc(((length + 2) / 3) * 4 + 1);
    if (!encoded_data) return NULL;

    int i, j;
    for (i = 0, j = 0; i < length;) {
        uint32_t octet_a = i < length ? (unsigned char)input[i++] : 0;
        uint32_t octet_b = i < length ? (unsigned char)input[i++] : 0;
        uint32_t octet_c = i < length ? (unsigned char)input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        encoded_data[j++] = encoding_table[(triple >> 18) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 12) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 6) & 0x3F];
        encoded_data[j++] = encoding_table[triple & 0x3F];
    }

    for (int k = 0; k < mod_table[length % 3]; k++)
        encoded_data[--j] = '=';

    encoded_data[j] = '\0';
    return encoded_data;
}

// Generate a Sec-WebSocket-Key
void generate_websocket_key(char *key, size_t len) {
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < len - 1; ++i) {
        key[i] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"[rand() % 62];
    }
    key[len - 1] = '\0';
}

// Send WebSocket Handshake Request
void send_handshake_request(SSL *ssl) {
    char sec_websocket_key[25];
    generate_websocket_key(sec_websocket_key, sizeof(sec_websocket_key));

    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request),
             "GET / HTTP/1.1\r\n"
             "Host: localhost:%d\r\n"
             "Upgrade: websocket\r\n"
             "Connection: Upgrade\r\n"
             "Sec-WebSocket-Key: %s\r\n"
             "Sec-WebSocket-Version: 13\r\n"
             "\r\n",
             PORT, sec_websocket_key);

    SSL_write(ssl, request, strlen(request));
}

// Process WebSocket Handshake Response
int process_handshake_response(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    SSL_read(ssl, buffer, sizeof(buffer));
    if (strstr(buffer, "Sec-WebSocket-Accept")) {
        printf("Handshake successful:\n%s\n", buffer);
        return 1;
    }
    printf("Handshake failed:\n%s\n", buffer);
    return 0;
}

// Handle WebSocket Handshake on Server
int handle_handshake(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    SSL_read(ssl, buffer, sizeof(buffer));

    char *key_start = strstr(buffer, "Sec-WebSocket-Key: ");
    if (!key_start) return 0;

    key_start += strlen("Sec-WebSocket-Key: ");
    char sec_websocket_key[25];
    sscanf(key_start, "%24s", sec_websocket_key);

    char accept_key[SHA_DIGEST_LENGTH];
    char concat_key[128];
    snprintf(concat_key, sizeof(concat_key), "%s%s", sec_websocket_key, WEBSOCKET_MAGIC_GUID);

    SHA1((unsigned char *)concat_key, strlen(concat_key), (unsigned char *)accept_key);
    char *accept_key_base64 = base64_encode((unsigned char *)accept_key, SHA_DIGEST_LENGTH);

    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 101 Switching Protocols\r\n"
             "Upgrade: websocket\r\n"
             "Connection: Upgrade\r\n"
             "Sec-WebSocket-Accept: %s\r\n"
             "\r\n",
             accept_key_base64);

    SSL_write(ssl, response, strlen(response));
    free(accept_key_base64);
    return 1;
}

// WebSocket Frame Helpers
void send_frame(SSL *ssl, const char *message) {
    size_t len = strlen(message);
    unsigned char frame[BUFFER_SIZE] = {0};
    frame[0] = 0x81;  // FIN=1, opcode=1 (text frame)
    frame[1] = len;   // Assuming payload length < 126
    memcpy(frame + 2, message, len);
    SSL_write(ssl, frame, len + 2);
}

void receive_frame(SSL *ssl, char *buffer) {
    unsigned char frame[BUFFER_SIZE] = {0};
    SSL_read(ssl, frame, BUFFER_SIZE);
    size_t len = frame[1] & 0x7F;  // Get payload length
    memcpy(buffer, frame + 2, len);
    buffer[len] = '\0';
}

// TLS WebSocket Server
void websocket_server() {
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        perror("Unable to create SSL context");
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_ecdh_auto(ctx, 1);

    // Load server certificate and private key
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        perror("Unable to load certificate or private key");
        exit(EXIT_FAILURE);
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Unable to accept");
        exit(EXIT_FAILURE);
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    } else {
        printf("Client connected via TLS\n");

        if (handle_handshake(ssl)) {
            char buffer[BUFFER_SIZE];
            receive_frame(ssl, buffer);
            printf("Received: %s\n", buffer);

            cJSON *response_json = cJSON_CreateObject();
            cJSON_AddStringToObject(response_json, "status", "Accepted");
            cJSON_AddStringToObject(response_json, "currentTime", "2024-12-26T12:00:00Z");
            const char *response = cJSON_PrintUnformatted(response_json);

            send_frame(ssl, response);
            cJSON_Delete(response_json);
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_fd);
    SSL_CTX_free(ctx);
}

// TLS WebSocket Client
void websocket_client() {
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        perror("Unable to create SSL context");
        exit(EXIT_FAILURE);
    }

    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        perror("Unable to create SSL object");
        exit(EXIT_FAILURE);
    }

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT)
    };

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }

    SSL_set_fd(ssl, client_fd);

    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    } else {
        printf("Connected to server via TLS\n");

        send_handshake_request(ssl);
        if (process_handshake_response(ssl)) {
            cJSON *request_json = cJSON_CreateObject();
            cJSON_AddStringToObject(request_json, "chargePointModel", "ModelX");
            cJSON_AddStringToObject(request_json, "chargePointVendor", "VendorY");
            cJSON_AddStringToObject(request_json, "firmwareVersion", "1.0.0");
            const char *request = cJSON_PrintUnformatted(request_json);

            send_frame(ssl, request);
            cJSON_Delete(request_json);

            char buffer[BUFFER_SIZE];
            receive_frame(ssl, buffer);
            printf("Received: %s\n", buffer);
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_fd);
    SSL_CTX_free(ctx);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <mode>\n", argv[0]);
        printf("Modes: server | client\n");
        return EXIT_FAILURE;
    }

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    if (strcmp(argv[1], "server") == 0) {
        websocket_server();
    } else if (strcmp(argv[1], "client") == 0) {
        websocket_client();
    } else {
        printf("Invalid mode. Use 'server' or 'client'.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

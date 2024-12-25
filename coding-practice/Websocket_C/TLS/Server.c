#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 8081
#define BUFFER_SIZE 1024

// Initialize OpenSSL
void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

// Clean up OpenSSL
void cleanup_openssl() {
    EVP_cleanup();
}

// Create an SSL context
SSL_CTX *create_ssl_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

// Configure the SSL context with certificate and private key
void configure_ssl_context(SSL_CTX *ctx) {
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

// Perform WebSocket handshake
void perform_websocket_handshake(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) {
        perror("Failed to read handshake request");
        return;
    }
    buffer[bytes] = '\0';

    // Extract WebSocket key from the client's handshake request
    char *websocket_key = strstr(buffer, "Sec-WebSocket-Key: ");
    if (!websocket_key) {
        fprintf(stderr, "Invalid WebSocket handshake request\n");
        return;
    }
    websocket_key += strlen("Sec-WebSocket-Key: ");
    char *key_end = strstr(websocket_key, "\r\n");
    if (key_end) {
        *key_end = '\0';
    }

    // Generate WebSocket accept key (hash the Sec-WebSocket-Key with a magic GUID)
    const char *magic_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char accept_key[BUFFER_SIZE];
    snprintf(accept_key, sizeof(accept_key), "%s%s", websocket_key, magic_guid);

    unsigned char hash[20];
    SHA1((unsigned char *)accept_key, strlen(accept_key), hash);

    char base64_accept_key[BUFFER_SIZE];
    EVP_EncodeBlock((unsigned char *)base64_accept_key, hash, 20);

    // Send WebSocket handshake response
    snprintf(buffer, sizeof(buffer),
             "HTTP/1.1 101 Switching Protocols\r\n"
             "Upgrade: websocket\r\n"
             "Connection: Upgrade\r\n"
             "Sec-WebSocket-Accept: %s\r\n\r\n",
             base64_accept_key);

    SSL_write(ssl, buffer, strlen(buffer));
}

// Handle WebSocket messages
void handle_websocket_connection(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            break;
        }
        buffer[bytes] = '\0';

        printf("Received message: %s\n", buffer);

        // Echo the message back
        SSL_write(ssl, buffer, bytes);
    }
}

int main() {
    int server_fd;
    struct sockaddr_in addr;
    SSL_CTX *ctx;

    // Initialize OpenSSL
    init_openssl();
    ctx = create_ssl_context();
    configure_ssl_context(ctx);

    // Create a TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    printf("TLS WebSocket server running on wss://localhost:%d\n", PORT);

    // Accept and handle connections
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Unable to accept");
            continue;
        }

        // Create SSL object
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_fd);

        // Perform SSL/TLS handshake
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        } else {
            perform_websocket_handshake(ssl);
            handle_websocket_connection(ssl);
        }

        // Clean up
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_fd);
    }

    close(server_fd);
    SSL_CTX_free(ctx);
    cleanup_openssl();

    return 0;
}

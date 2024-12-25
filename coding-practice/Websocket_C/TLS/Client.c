#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SERVER_ADDR "127.0.0.1" // Server IP address
#define SERVER_PORT 8081        // Server port
#define BUFFER_SIZE 2048

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

    method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

// Perform WebSocket handshake
void perform_websocket_handshake(SSL *ssl) {
    char buffer[BUFFER_SIZE];

    // Send WebSocket handshake request
    const char *handshake_request =
        "GET / HTTP/1.1\r\n"
        "Host: " SERVER_ADDR "\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n\r\n";

    SSL_write(ssl, handshake_request, strlen(handshake_request));

    // Read handshake response
    int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("Handshake response from server:\n%s\n", buffer);
    } else {
        perror("Failed to receive handshake response");
    }
}

// Send a WebSocket message
void send_websocket_message(SSL *ssl, const char *message) {
    char frame[BUFFER_SIZE];
    int frame_len = 0;

    // Create a simple WebSocket text frame
    frame[0] = 0x81; // FIN=1, opcode=1 (text)
    frame[1] = strlen(message); // Payload length (assuming <126 bytes)
    memcpy(&frame[2], message, strlen(message));
    frame_len = 2 + strlen(message);

    SSL_write(ssl, frame, frame_len);
}

// Receive a WebSocket message
void receive_websocket_message(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("Message from server: %s\n", buffer + 2); // Skip WebSocket frame header
    } else {
        perror("Failed to receive message");
    }
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    SSL_CTX *ctx;
    SSL *ssl;

    // Initialize OpenSSL
    init_openssl();
    ctx = create_ssl_context();

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Unable to connect to server");
        exit(EXIT_FAILURE);
    }

    // Create SSL object
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    // Perform SSL/TLS handshake
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server using %s\n", SSL_get_cipher(ssl));

    // Perform WebSocket handshake
    perform_websocket_handshake(ssl);

    // Send a message to the server
    const char *message = "Hello, WebSocket server!";
    printf("Sending message: %s\n", message);
    send_websocket_message(ssl, message);

    // Receive a response from the server
    receive_websocket_message(ssl);

    // Clean up
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();

    return 0;
}

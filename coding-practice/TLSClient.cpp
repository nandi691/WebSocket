#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <cstring>

// Function to send and receive JSON messages
void websocket_client(const char* hostname, int port) {
    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);

    if (!ctx) {
        std::cerr << "Failed to create SSL context.\n";
        return;
    }

    SSL* ssl = SSL_new(ctx);
    int server_fd = BIO_get_connect_socket(hostname, port);

    if (server_fd <= 0) {
        std::cerr << "Failed to connect to server.\n";
        SSL_CTX_free(ctx);
        return;
    }

    SSL_set_fd(ssl, server_fd);

    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return;
    }

    // WebSocket handshake
    const char* handshake_request =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n\r\n";

    SSL_write(ssl, handshake_request, strlen(handshake_request));

    char buffer[2048];
    int bytes_read = SSL_read(ssl, buffer, sizeof(buffer));
    buffer[bytes_read] = '\0';
    std::cout << "Server handshake response:\n" << buffer << std::endl;

    // Send JSON data
    const char* json_data = "{\"client\": \"Hello Server!\"}";
    uint8_t frame[1024] = {0};
    frame[0] = 0x81; // WebSocket Text Frame
    frame[1] = strlen(json_data);
    memcpy(frame + 2, json_data, strlen(json_data));

    SSL_write(ssl, frame, strlen(json_data) + 2);

    // Read server response
    bytes_read = SSL_read(ssl, buffer, sizeof(buffer));
    buffer[bytes_read] = '\0';
    std::cout << "Server response: " << buffer << std::endl;

    SSL_free(ssl);
    SSL_CTX_free(ctx);
}

int main() {
    websocket_client("localhost", 8080);
    return 0;
}

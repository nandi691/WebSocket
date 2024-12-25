#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#include <iostream>
#include <unistd.h>

// Function to initialize OpenSSL and load error strings
void initialize_openssl() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
}

// Function to create and bind a BIO for accepting connections
BIO* create_bio(const char* port) {
    BIO *bio = BIO_new_accept(port);
    if (!bio) {
        std::cerr << "Error creating BIO" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (BIO_do_accept(bio) <= 0) {
        std::cerr << "Error binding BIO to port " << port << std::endl;
        exit(EXIT_FAILURE);
    }

    return bio;
}

// Function to perform SSL handshake
SSL* perform_handshake(BIO* bio, SSL_CTX* ctx) {
    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        std::cerr << "Error creating SSL object" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Accept the connection and set up SSL
    if (BIO_do_accept(bio) <= 0) {
        std::cerr << "Error accepting connection" << std::endl;
        exit(EXIT_FAILURE);
    }

    BIO *client_bio = BIO_pop(bio); // Get the client connection from the BIO
    SSL_set_bio(ssl, client_bio, client_bio); // Set the BIO for the SSL connection

    // Perform the SSL handshake
    if (SSL_accept(ssl) <= 0) {
        std::cerr << "SSL handshake failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    return ssl;
}

// Function to handle client communication
void communicate_with_client(SSL* ssl) {
    const char* hello_msg = "Hello World\n";

    // Send "Hello World" message to the client
    if (SSL_write(ssl, hello_msg, strlen(hello_msg)) <= 0) {
        std::cerr << "Error writing to client" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Sent 'Hello World' to client" << std::endl;
}

int main() {
    const char* port = "8080";  // Port to listen on

    // Initialize OpenSSL
    initialize_openssl();

    // Create a BIO to bind to the port
    BIO* bio = create_bio(port);

    // Create an SSL_CTX (context) object and configure it
    const SSL_METHOD* method = TLS_server_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        std::cerr << "Error creating SSL context" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Load SSL certificates (server.pem and server.key should exist)
    if (SSL_CTX_use_certificate_file(ctx, "server.pem", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Error loading certificate" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Error loading private key" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Check if the private key matches the certificate
    if (!SSL_CTX_check_private_key(ctx)) {
        std::cerr << "Private key does not match the certificate" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "TLS WebSocket server is listening on port " << port << "..." << std::endl;

    // Perform SSL handshake with the client
    SSL* ssl = perform_handshake(bio, ctx);

    // Communicate with the client (send "Hello World")
    communicate_with_client(ssl);

    // Clean up
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    BIO_free_all(bio);

    return 0;
}

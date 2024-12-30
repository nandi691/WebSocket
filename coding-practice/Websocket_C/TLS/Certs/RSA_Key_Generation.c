#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>

void handle_errors() {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

int main() {
    int key_length = 2048;
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    FILE *private_key_file = NULL;
    FILE *public_key_file = NULL;

    // Create a context for key generation
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        handle_errors();
    }

    // Initialize the key generation context
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        handle_errors();
    }

    // Set the key length
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, key_length) <= 0) {
        handle_errors();
    }

    // Generate the RSA key
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        handle_errors();
    }

    // Write the private key to a file
    private_key_file = fopen("private_key.pem", "wb");
    if (!private_key_file) {
        perror("Unable to open file");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    if (!PEM_write_PrivateKey(private_key_file, pkey, NULL, NULL, 0, NULL, NULL)) {
        handle_errors();
    }

    fclose(private_key_file);
    printf("Private key successfully generated and written to private_key.pem\n");

    // Write the public key to a file
    public_key_file = fopen("public_key.pem", "wb");
    if (!public_key_file) {
        perror("Unable to open file");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    if (!PEM_write_PUBKEY(public_key_file, pkey)) {
        handle_errors();
    }

    fclose(public_key_file);
    printf("Public key successfully generated and written to public_key.pem\n");

    // Clean up
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    return 0;
}

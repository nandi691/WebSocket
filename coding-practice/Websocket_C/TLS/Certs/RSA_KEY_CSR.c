#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_errors() {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

EVP_PKEY *generate_rsa_key(int key_length) {
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

    if (!ctx) {
        handle_errors();
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        handle_errors();
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, key_length) <= 0) {
        handle_errors();
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        handle_errors();
    }

    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

void write_private_key(EVP_PKEY *pkey, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Unable to open file");
        EVP_PKEY_free(pkey);
        exit(EXIT_FAILURE);
    }

    if (!PEM_write_PrivateKey(file, pkey, NULL, NULL, 0, NULL, NULL)) {
        handle_errors();
    }

    fclose(file);
    printf("Private key successfully written to %s\n", filename);
}

void write_public_key(EVP_PKEY *pkey, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Unable to open file");
        EVP_PKEY_free(pkey);
        exit(EXIT_FAILURE);
    }

    if (!PEM_write_PUBKEY(file, pkey)) {
        handle_errors();
    }

    fclose(file);
    printf("Public key successfully written to %s\n", filename);
}

X509_REQ *generate_csr(EVP_PKEY *pkey) {
    X509_REQ *req = X509_REQ_new();
    if (!req) {
        handle_errors();
    }

    if (X509_REQ_set_pubkey(req, pkey) <= 0) {
        handle_errors();
    }

    X509_NAME *name = X509_NAME_new();
    if (!name) {
        handle_errors();
    }

    if (X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"US", -1, -1, 0) <= 0 ||
        X509_NAME_add_entry_by_txt(name, "ST", MBSTRING_ASC, (unsigned char *)"California", -1, -1, 0) <= 0 ||
        X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, (unsigned char *)"San Francisco", -1, -1, 0) <= 0 ||
        X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"Example Corp", -1, -1, 0) <= 0 ||
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"www.example.com", -1, -1, 0) <= 0) {
        handle_errors();
    }

    if (X509_REQ_set_subject_name(req, name) <= 0) {
        handle_errors();
    }

    X509_NAME_free(name);

    if (X509_REQ_sign(req, pkey, EVP_sha256()) <= 0) {
        handle_errors();
    }

    return req;
}

void write_csr(X509_REQ *req, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Unable to open file");
        X509_REQ_free(req);
        exit(EXIT_FAILURE);
    }

    if (!PEM_write_X509_REQ(file, req)) {
        handle_errors();
    }

    fclose(file);
    printf("CSR successfully written to %s\n", filename);
}

int main() {
    int key_length = 2048;

    EVP_PKEY *pkey = generate_rsa_key(key_length);
    write_private_key(pkey, "private_key.pem");
    write_public_key(pkey, "public_key.pem");

    X509_REQ *csr = generate_csr(pkey);
    write_csr(csr, "csr.pem");

    X509_REQ_free(csr);
    EVP_PKEY_free(pkey);

    return 0;
}

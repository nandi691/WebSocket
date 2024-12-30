
#include "TLSClient.h"

static const int mod_table[] = {0, 2, 1};

// Helper function to Base64 encode data
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

// Main Client Logic
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

            printf("%s",request);

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

int main() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    websocket_client();

    return EXIT_SUCCESS;
}

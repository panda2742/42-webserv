#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

static void msleep(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

static void send_payload(const char *ip, int port, const char *payload, size_t len) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

     int timeout_sec = 5;
     struct timeval tv;
     tv.tv_sec = timeout_sec;
     tv.tv_usec = 0;
     setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

   
    fprintf(stderr, "--- payload (%zu bytes) ---\n", len);
    for (size_t i = 0; i < len; ++i) {
        unsigned char b = (unsigned char)payload[i];
        if (b == '\r') fputs("\\r", stderr);
        else if (b == '\n') fputs("\\n\n", stderr); /* show as \n then newline for readability */
        else if (b >= 32 && b <= 126) fputc(b, stderr);
        else fprintf(stderr, "\\x%02x", b);
    }
    fputs("\n--- end payload ---\n", stderr);
    

    /* Ensure all bytes are sent (handle partial sends). */
    size_t total_sent = 0;
    while (total_sent < len) {
        ssize_t s = send(sock, payload + total_sent, len - total_sent, 0);
        if (s < 0) {
            if (errno == EINTR) continue;
            perror("send");
            close(sock);
            return;
        }
        total_sent += (size_t)s;
    }

    /* Signal EOF to the peer for the write side, but keep reading the response. */
    // shutdown(sock, SHUT_WR);

    char buf[1024];
    ssize_t n;
    /* Read until the server closes the connection or the recv times out.
       Log recv return values to stderr for debugging. Print response to stderr
       as well so it's visible even if stdout is buffered. */
    while (1) {
        n = recv(sock, buf, sizeof(buf), 0);
        if (n > 0) {
            fwrite(buf, 1, n, stderr);
            fflush(stderr);
            fprintf(stderr, "\n[recv=%zd]\n", n);
        } else if (n == 0) {
            fprintf(stderr, "[recv=0] connection closed by peer\n");
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                fprintf(stderr, "[recv timeout after %d s]\n", timeout_sec);
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                perror("recv");
                break;
            }
        }
    }

    close(sock);
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <file> <ip> <port> [delay_ms]\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    const char *ip = argv[2];
    int port = atoi(argv[3]);
    int delay = argc >= 5 ? atoi(argv[4]) : 0;

    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char *payload = NULL;
    size_t cap = 0, len = 0;
    int c;

    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            // file newline → separator between payloads
            if (len > 0) {
                send_payload(ip, port, payload, len);
                if (delay > 0) msleep(delay);
                len = 0;
            }
        } else if (c == '\\') {
            /* If the test file contains the two-character sequences "\\r" or "\\n",
               convert them into the real CR (0x0D) or LF (0x0A) bytes in the payload.
               Any other sequence starting with backslash is preserved as-is (backslash + char). */
            int d = fgetc(f);
            if (d == EOF) {
                // dangling backslash at EOF -> keep it
                if (len + 1 >= cap) {
                    cap = cap ? cap * 2 : 512;
                    payload = realloc(payload, cap);
                }
                payload[len++] = '\\';
                break;
            }

            if (d == 'r') {
                if (len + 1 >= cap) {
                    cap = cap ? cap * 2 : 512;
                    payload = realloc(payload, cap);
                }
                payload[len++] = '\r';
            } else if (d == 'n') {
                if (len + 1 >= cap) {
                    cap = cap ? cap * 2 : 512;
                    payload = realloc(payload, cap);
                }
                payload[len++] = '\n';
            } else {
                // not an escape we recognize: keep backslash and the following char
                if (len + 2 >= cap) {
                    while (len + 2 >= cap) cap = cap ? cap * 2 : 512;
                    payload = realloc(payload, cap);
                }
                payload[len++] = '\\';
                payload[len++] = (char)d;
            }
        } else {
            if (len + 1 >= cap) {
                cap = cap ? cap * 2 : 512;
                payload = realloc(payload, cap);
            }
            payload[len++] = c;
        }
    }

    // last payload if file doesn't end with newline
    if (len > 0) {
        send_payload(ip, port, payload, len);
    }

    free(payload);
    fclose(f);
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void send_response(int client_socket, const char* content_type, const char* body) {
    char header[1024];
    sprintf(header,
            "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n",
            content_type, strlen(body));
    send(client_socket, header, strlen(header), 0);
    send(client_socket, body, strlen(body), 0);
}

void handle_request(int client_socket) {
    char buffer[4096];
    recv(client_socket, buffer, sizeof(buffer), 0);

    if (strstr(buffer, "POST /login")) {
        send_response(client_socket, "application/json", "{\"success\":true}");
    } else if (strstr(buffer, "POST /upload")) {
        char *data = strstr(buffer, "\r\n\r\n");
        if (data) {
            FILE *fp = fopen("matches.txt", "a");
            fprintf(fp, "%s\n", data + 4);
            fclose(fp);
        }
        send_response(client_socket, "text/plain", "Data saved");
    } else if (strstr(buffer, "GET /matchinfo")) {
        FILE *fp = fopen("matches.txt", "r");
        char content[2048] = "";
        if (fp) {
            fread(content, 1, sizeof(content), fp);
            fclose(fp);
        }
        send_response(client_socket, "text/plain", content);
    } else if (strstr(buffer, "GET /leaderboard")) {
        send_response(client_socket, "text/plain", "Leaderboard:\nPlayer A: 10 pts\nPlayer B: 8 pts");
    } else {
        send_response(client_socket, "text/plain", "404 Not Found");
    }
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);
    printf("Server started on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_socket >= 0) {
            handle_request(client_socket);
        }
    }

    return 0;
}

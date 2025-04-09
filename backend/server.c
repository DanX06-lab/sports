#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080
#define BUF_SIZE 8192
#define USER_FILE "users.txt"

void trim_newline(char *str) {
    char *pos;
    if ((pos=strchr(str, '\n')) != NULL)
        *pos = '\0';
}

int user_exists(const char *username) {
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) return 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        char *saved_username = strtok(line, ",");
        if (saved_username && strcmp(saved_username, username) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int verify_login(const char *username, const char *password, const char *type) {
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) return 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        char *saved_username = strtok(line, ",");
        char *saved_password = strtok(NULL, ",");
        char *saved_type     = strtok(NULL, ",");
        if (saved_username && saved_password && saved_type &&
            strcmp(saved_username, username) == 0 &&
            strcmp(saved_password, password) == 0 &&
            strcmp(saved_type, type) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void handle_request(int client_socket, char *request) {
    if (strstr(request, "POST /signup")) {
        char *body = strstr(request, "\r\n\r\n");
        if (body) {
            body += 4;
            char username[100], password[100];
            sscanf(body, "{\"username\":\"%[^\"]\",\"password\":\"%[^\"]\"}", username, password);

            if (user_exists(username)) {
                char response[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"success\":false}";
                write(client_socket, response, strlen(response));
            } else {
                FILE *fp = fopen(USER_FILE, "a");
                fprintf(fp, "%s,%s,member\n", username, password);
                fclose(fp);
                char response[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"success\":true}";
                write(client_socket, response, strlen(response));
            }
        }
    }

    else if (strstr(request, "POST /login")) {
        char *body = strstr(request, "\r\n\r\n");
        if (body) {
            body += 4;
            char username[100], password[100], type[100];

            sscanf(body,
              "{\"type\":\"%[^\"]\",\"username\":\"%[^\"]\",\"password\":\"%[^\"]\"}",
              type, username, password
            );

            if (verify_login(username, password, type)) {
                char response[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"success\":true}";
                write(client_socket, response, strlen(response));
            } else {
                char response[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"success\":false}";
                write(client_socket, response, strlen(response));
            }
        }
    }

    else if (strstr(request, "POST /upload")) {
        FILE *fp = fopen("match_data.txt", "a");
        char *body = strstr(request, "\r\n\r\n");
        if (body) {
            body += 4;
            fprintf(fp, "%s\n", body);
        }
        fclose(fp);
        char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nData received.";
        write(client_socket, response, strlen(response));
    }

    else if (strstr(request, "GET /matchinfo")) {
        char response[] = 
            "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nMatch 1: A vs B\nMatch 2: C vs D";
        write(client_socket, response, strlen(response));
    }

    else if (strstr(request, "GET /leaderboard")) {
        char response[] = 
            "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n1. Player A - 10 pts\n2. Player C - 8 pts";
        write(client_socket, response, strlen(response));
    }

    else {
        char response[] = 
            "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found";
        write(client_socket, response, strlen(response));
    }

    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUF_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    printf("🟢 Server running at http://localhost:%d\n", PORT);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        memset(buffer, 0, BUF_SIZE);
        read(client_socket, buffer, BUF_SIZE - 1);
        handle_request(client_socket, buffer);
    }

    return 0;
}

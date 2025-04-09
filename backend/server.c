#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

void handle_client(int client) {
    char buffer[4096];
    read(client, buffer, sizeof(buffer));
    
    if (strstr(buffer, "POST /login")) {
        char *body = strstr(buffer, "\r\n\r\n") + 4;
        char username[100], password[100], type[20];
        sscanf(body, "{\"username\":\"%[^\"]\",\"password\":\"%[^\"]\",\"type\":\"%[^\"]\"}", username, password, type);

        FILE *f = fopen("users.txt", "r");
        int found = 0;
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            char u[100], p[100], t[20];
            sscanf(line, "%[^,],%[^,],%s", u, p, t);
            if (strcmp(u, username) == 0 && strcmp(p, password) == 0 && strcmp(t, type) == 0) {
                found = 1;
                break;
            }
        }
        fclose(f);

        dprintf(client, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
        dprintf(client, "{\"success\": %s}", found ? "true" : "false");

    } else if (strstr(buffer, "POST /signup")) {
        char *body = strstr(buffer, "\r\n\r\n") + 4;
        char username[100], password[100], type[20];
        sscanf(body, "{\"username\":\"%[^\"]\",\"password\":\"%[^\"]\",\"type\":\"%[^\"]\"}", username, password, type);

        FILE *f = fopen("users.txt", "a");
        fprintf(f, "%s,%s,%s\n", username, password, type);
        fclose(f);

        dprintf(client, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nAccount created");

    } else if (strstr(buffer, "POST /upload")) {
        char *body = strstr(buffer, "\r\n\r\n") + 4;
        FILE *f = fopen("matches.txt", "a");
        fprintf(f, "%s\n", body);
        fclose(f);
        dprintf(client, "HTTP/1.1 200 OK\r\n\r\n");

    } else if (strstr(buffer, "GET /matchinfo")) {
        FILE *f = fopen("matches.txt", "r");
        char content[2048] = "";
        char line[256];
        while (f && fgets(line, sizeof(line), f)) {
            strcat(content, line);
        }
        if (f) fclose(f);
        dprintf(client, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", content);

    } else if (strstr(buffer, "GET /leaderboard")) {
        dprintf(client, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nLeaderboard functionality to be added.");
    } else {
        dprintf(client, "HTTP/1.1 404 Not Found\r\n\r\n");
    }

    close(client);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 10);
    printf("Server running on port 8080...\n");

    while (1) {
        int client = accept(sockfd, NULL, NULL);
        pthread_t t;
        pthread_create(&t, NULL, (void *)handle_client, (void *)(intptr_t)client);
        pthread_detach(t);
    }

    return 0;
}

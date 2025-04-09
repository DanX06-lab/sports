// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mongoose.h"

#define USERS_FILE "backend/users.txt"
#define MATCH_FILE "backend/match_data.txt"

static void handle_request(struct mg_connection *c, int ev, void *ev_data, void *fn_data);
void handle_signup(struct mg_connection *c, struct mg_http_message *hm);
void handle_login(struct mg_connection *c, struct mg_http_message *hm);
void handle_upload(struct mg_connection *c, struct mg_http_message *hm);
void handle_static_get(struct mg_connection *c, struct mg_http_message *hm);
void handle_text_get(struct mg_connection *c, struct mg_http_message *hm);

int main() {
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  printf("Starting server on http://localhost:8080\n");
  mg_http_listen(&mgr, "http://localhost:8080", handle_request, &mgr);
  for (;;) mg_mgr_poll(&mgr, 1000);
  mg_mgr_free(&mgr);
  return 0;
}

static void handle_request(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    if (mg_http_match_uri(hm, "/signup")) {
      handle_signup(c, hm);
    } else if (mg_http_match_uri(hm, "/login")) {
      handle_login(c, hm);
    } else if (mg_http_match_uri(hm, "/upload")) {
      handle_upload(c, hm);
    } else if (mg_http_match_uri(hm, "/matchinfo") || mg_http_match_uri(hm, "/leaderboard")) {
      handle_text_get(c, hm);
    } else {
      handle_static_get(c, hm);
    }
  }
}

void handle_signup(struct mg_connection *c, struct mg_http_message *hm) {
  char username[100], password[100], role[20];
  struct json_token t;
  json_scanf(hm->body.ptr, hm->body.len,
             "{username: %Q, password: %Q, role: %Q}",
             &username, &password, &role);

  FILE *fp = fopen(USERS_FILE, "a");
  fprintf(fp, "%s,%s,%s\n", username, password, role);
  fclose(fp);

  mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"message\":\"Signup successful\"}");
}

void handle_login(struct mg_connection *c, struct mg_http_message *hm) {
  char username[100], password[100], file_user[100], file_pass[100], file_role[20];
  int found = 0;
  json_scanf(hm->body.ptr, hm->body.len, "{username: %Q, password: %Q}", &username, &password);

  FILE *fp = fopen(USERS_FILE, "r");
  while (fscanf(fp, "%99[^,],%99[^,],%19[^\n]\n", file_user, file_pass, file_role) == 3) {
    if (strcmp(file_user, username) == 0 && strcmp(file_pass, password) == 0) {
      found = 1;
      break;
    }
  }
  fclose(fp);

  if (found) {
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
      "{\"success\":true,\"role\":\"%s\"}", file_role);
  } else {
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
      "{\"success\":false,\"message\":\"Invalid credentials\"}");
  }
}

void handle_upload(struct mg_connection *c, struct mg_http_message *hm) {
  FILE *fp = fopen(MATCH_FILE, "a");
  fwrite(hm->body.ptr, 1, hm->body.len, fp);
  fwrite("\n", 1, 1, fp);
  fclose(fp);
  mg_http_reply(c, 200, "", "Data received\n");
}

void handle_text_get(struct mg_connection *c, struct mg_http_message *hm) {
  const char *file = MATCH_FILE;
  FILE *fp = fopen(file, "r");
  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char *data = malloc(fsize + 1);
  fread(data, 1, fsize, fp);
  data[fsize] = '\0';
  fclose(fp);
  mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "%s", data);
  free(data);
}

void handle_static_get(struct mg_connection *c, struct mg_http_message *hm) {
  struct mg_http_serve_opts opts = {.root_dir = "frontend"};
  mg_http_serve_dir(c, hm, &opts);
}

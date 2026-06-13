#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#define ___LYLOG_IMP___
#include "lylog.h"
#define ___LYSTASTRINGS_IMP___
#include "stastrings.h"

typedef struct {
  int intervalt;
  StaStrings connection;
  int ammount;
} ConfigVal;

typedef struct {
  char* data;
  size_t size;
} HttpBuffer;

int
handle_curl(ConfigVal* conf);
void
handle_input(int argc, char** argv, ConfigVal* conf);

int
main(int argc, char** argv) {

  ConfigVal conf = {};

  handle_input(argc, argv, &conf);

  printf("conf connection: %.*s\n", (int)conf.connection.count,
         conf.connection.arr);
  printf("conf interval: %d\n", conf.intervalt);
  printf("conf ammount: %d\n", conf.ammount);

  handle_curl(&conf);
}

size_t
write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t real_size = size * nmemb;
  HttpBuffer* mem = userp;
  char* ptr = realloc(mem->data, mem->size + real_size + 1);
  if (!ptr)
    return 0;
  mem->data = ptr;
  memcpy(&(mem->data[mem->size]), contents, real_size);
  mem->size += real_size;
  mem->data[mem->size] = '\0';

  return real_size;
}

int
handle_curl(ConfigVal* conf) {
  CURL* curl = curl_easy_init();
  while (conf->ammount) {

    if (!curl)
      return -1;

    HttpBuffer response = {.data = malloc(1), .size = 0};

    response.data[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, conf->connection.arr);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    fflush(stdout);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      lym_printlog(ERROR, "ERROR: %s\n", curl_easy_strerror(res));
    } else {
      printf("\n");
      lym_printlog(INFO, "RAW:%s", response.data);
      fflush(stdout);
      float values[4];
      int idx = 0;
      char* tok = strtok(response.data, ",");
      while (tok && idx < 4) {
        values[idx++] = atof(tok);
        tok = strtok(NULL, ",");
      }
      if (idx == 4) {
        lym_printlog(INFO, "PARSED:");
        printf("temp  : %.2f\n", values[0]);
        printf("humid : %.2f\n", values[1]);
        printf("light : %.2f\n", values[2]);
        printf("sound : %.2f\n", values[3]);
      }
      FILE* f = fopen("output.txt", "w");

      if (f) {
        fprintf(f,

                "%f\n"
                "%f\n"
                "%f\n"
                "%f\n",

                values[0], values[1], values[2], values[3]);

        fclose(f);
      }
    }

    free(response.data);

    --conf->ammount;
    sleep(conf->intervalt);
  }
  curl_easy_cleanup(curl);
  return 0;
}

int
create_databases() {
  return 0;
}

void
handle_input(int argc, char** argv, ConfigVal* conf) {
  int i = 1;
  char* endl;
  if (argc < 1)
    exit(0);

  while (i < argc) {
    switch (argv[i][0]) {
    case '-':
      if (strcmp(argv[i], "--connection") == 0) {
        ++i;
        StaStringsNew(&conf->connection, argv[i]);

      } else if (strcmp(argv[i], "--interval") == 0) {
        ++i;
        conf->intervalt = strtol(argv[i], &endl, 10);
      } else if (strcmp(argv[i], "--ammount") == 0) {
        ++i;
        conf->ammount = strtol(argv[i], &endl, 10);
      } else {
        lym_printlog(
            ERROR,
            "Parsing Error, anda memasukkan %s dimana tidak ada opsi tersebut",
            argv[i]);
        exit(1);
      }
      break;
    default:
      StaStringsNew(&conf->connection, argv[i]);
    }
    ++i;
  }
}

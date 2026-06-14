// External Library
#include <curl/curl.h>

// STD C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Non Multi Platform
#ifdef __linux__
#include <unistd.h>
#else
#include <windows.h>
#endif

#define ___LYLOG_IMP___
#include "lylog.h"
#define ___LYSTASTRINGS_IMP___
#include "stastrings.h"

typedef struct {
  int intervalt;
  StaStrings connection;
  int ammount;
  char* outputname;
} ConfigVal;

typedef struct {
  char* data;
  size_t size;
} HttpBuffer;

typedef struct {
  double tempval;
  double humval;
  double lightval;
  double micval;
} SensorValue;


// Forward Decl
int
handle_curl(ConfigVal* conf);
void
handle_input(int argc, char** argv, ConfigVal* conf);
int
create_databases(SensorValue sens, char* name);

int
main(int argc, char** argv) {

  ConfigVal conf = {};

  handle_input(argc, argv, &conf);

  printf("conf connection: %.*s\n", (int)conf.connection.count,
         conf.connection.arr);
  printf("conf interval: %d\n", conf.intervalt);
  printf("conf ammount: %d\n", conf.ammount);
  printf("conf names: %s\n", conf.outputname);

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
  SensorValue sens = {};

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
        sens.tempval = values[0];
        sens.humval = values[1];
        sens.lightval = values[2];
        sens.micval = values[3];
        create_databases(sens, conf->outputname);
      }
    }

    free(response.data);

    --conf->ammount;
    #ifdef __linux__
    sleep(conf->intervalt);
    #else
    Sleep(conf->intervalt * 1000);
    #endif
  }
  curl_easy_cleanup(curl);
  return 0;
}

int
create_databases(SensorValue sens, char* namae) {
  FILE* outputfile = fopen(namae, "a+");
  if (!outputfile) {
    lym_printlog(ERROR, "gagal membuka csv");
    return -1;
  }

  fseek(outputfile, 0, SEEK_END);
  long sz = ftell(outputfile);
  if (sz == 0) {
    fprintf(outputfile, "times,temp,humidity,light,sound\n");
  }
  time_t nw = time(NULL);
  struct tm* tm = localtime(&nw);
  char timestr[64];

  strftime(timestr, sizeof(timestr), "%H:%M:%S %d-%m-%Y", tm);
  fprintf(outputfile, "%s,%.2f,%.2f,%.2f,%.2f\n", timestr, sens.tempval,
          sens.humval, sens.lightval, sens.micval);

  fclose(outputfile);

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
      } else if (strcmp(argv[i], "--output") == 0) {
	++i;
	conf->outputname = argv[i];
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
  if (conf->outputname == NULL){
	conf->outputname = "output.csv";
  }
}

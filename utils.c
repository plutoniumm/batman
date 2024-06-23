#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

struct hashmap {
  char *key;
  char *value;
};

int split(char *str, const char *delim, char ***arr, int *length) {
  int i = 0;
  char *token;
  char **res = (char **) malloc(0 * sizeof(char *));

  token = strtok(str, delim);
  while (token != NULL) {
    res = (char **) realloc(res, (i + 1) * sizeof(char *));
    res[i] = token;
    i++;
    token = strtok(NULL, delim);
  }
  *arr = res;
  *length = i;

  free(token);
  return 1;
}

// a string will be sent as "program,arg1,arg2,..."
int execute(char *str) {
  char **args = NULL;
  int length;
  split(str, ",", &args, &length);

  char *program = args[0];
  char **new_args = (char **) malloc((length + 1) * sizeof(char *));
  for (int i = 0; i < length; i++) {
    new_args[i] = args[i];
  }
  new_args[length] = NULL;

  pid_t pid = fork();
  if (pid == 0) {
    execvp(program, new_args);
    exit(0);
  } else {
    wait(NULL);
  }

  free(args);
  free(new_args);
  return 0;
}


void update_hashmap(struct hashmap **hashmap, int *hashmap_size) {
  FILE *fp = fopen("./batman.conf", "r");
  if (fp == NULL) return;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  int count = 0;

  struct hashmap *new_hashmap = NULL;
  bool comment = false;
  while ((read = getline(&line, &len, fp)) != -1) {
    if (line[0] == '\n') continue;
    comment = false;
    for (int i = 0; i < strlen(line); i++) {
      if (line[i] == ' ' || line[i] == '\t') {
        continue;
      };
      if (line[i] == '#') {
        comment = true;
        break;
      };
    };
    if (comment) continue;


    char **kv = NULL;
    int length;
    split(line, "=", &kv, &length);

    if (length == 2) {
      new_hashmap = (struct hashmap *) realloc(new_hashmap, (count + 1) * sizeof(struct hashmap));
      new_hashmap[count].key = strdup(kv[0]);
      new_hashmap[count].value = strdup(kv[1]);
      count++;
    }

    free(kv);
  }

  free(line);
  fclose(fp);
  for (int i = 0; i < *hashmap_size; i++) {
    free((*hashmap)[i].key);
    free((*hashmap)[i].value);
  }
  free(*hashmap);

  *hashmap = new_hashmap;
  *hashmap_size = count;
  return;
}

int get_value(
  struct hashmap *hm,
  int hashmap_size,
  const char *key,
  char ***args,
  int *args_length
) {
  for (int i = 0; i < hashmap_size; i++) {
    if (strcmp(hm[i].key, key) == 0) {
      split(hm[i].value, ",", args, args_length);
      return 0;
    }
  }

  return 1;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
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
  return 1;
}

void update_hashmap(struct hashmap **hashmap, int *hashmap_size) {
  FILE *fp = fopen("./.brc", "r");
  if (fp == NULL) {
    printf("Error opening file\n");
    return;
  }
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  int count = 0;

  struct hashmap *new_hashmap = NULL;
  while ((read = getline(&line, &len, fp)) != -1) {
    char **kv = NULL;
    int length;
    split(line, "=", &kv, &length);

    if (length == 2) {
      new_hashmap = (struct hashmap *) realloc(new_hashmap, (count + 1) * sizeof(struct hashmap));
      new_hashmap[count].key = strdup(kv[0]);
      new_hashmap[count].value = strdup(kv[1]);
      printf("%s -> %s\n", new_hashmap[count].key, new_hashmap[count].value);
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
  return 1; // Key not found
}

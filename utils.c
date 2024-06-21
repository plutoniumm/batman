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

int get_config(struct hashmap **hashmap, int *hashmap_size) {
  FILE *fp = fopen("./.brc", "r");
  if (fp == NULL) {
    printf("Error opening file\n");
    return 1;
  }
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  int count = 0;
  while ((read = getline(&line, &len, fp)) != -1) {
    char **kv = NULL;
    int length;
    split(line, "=", &kv, &length);

    if (length == 2) {
      *hashmap = (struct hashmap *) realloc(*hashmap, (count + 1) * sizeof(struct hashmap));
      (*hashmap)[count].key = strdup(kv[0]);
      (*hashmap)[count].value = strdup(kv[1]);
      count++;
    }

    free(kv);
  }

  free(line);
  fclose(fp);
  *hashmap_size = count;
  return 0;
};

// value -> program,args1,args2..
int get_value(
  struct hashmap *hm,
  int hashmap_size,
  char *key,
  char *program,
  char **args,
  int *args_length
) {
  for (int i = 0; i < hashmap_size; i++) {
    if (strcmp(hm[i].key, key) == 0) {
      split(hm[i].value, ",", &args, args_length);
    }
  }

  return 0;
}

// int main() {
//   struct hashmap *hm = NULL;
//   int hashmap_size = 0;

//   get_config(&hm, &hashmap_size);
//   for (int i = 0; i < hashmap_size; i++) {
//     printf("%s\n", hm[i].key);
//     char **args = NULL;
//     int args_length;

//     get_value(
//       hm, hashmap_size,
//       hm[i].key,
//       NULL, args, &args_length
//     );
//     printf("%s\n", hm[i].value);
//   }
//   // free at end of program is not necessary
//   return 0;
// }

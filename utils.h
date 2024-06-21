struct hashmap {
  char *key;
  char *value;
};

int split(char *str, const char *delim, char ***array, int *length);

int get_config(struct hashmap **hashmap, int *hashmap_size);

int get_value(
  struct hashmap *hm,
  int hashmap_size,
  char *key,
  char *program,
  char **args,
  int *args_length
);

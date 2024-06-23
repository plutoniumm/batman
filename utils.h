struct hashmap {
  char *key;
  char *value;
};

int split(char *str, const char *delim, char ***arr, int *length);

int execute(char *str);

int update_hashmap(struct hashmap **hashmap, int *hashmap_size);

int get_value(
  struct hashmap *hm,
  int hashmap_size,
  const char *key,
  char ***args,
  int *args_length
);
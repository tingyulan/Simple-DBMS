#ifndef TABLE_LIKE_H
#define TABLE_LIKE_H
#include <stdlib.h>
#include <stdio.h>
#include "Like.h"

#define INIT_TABLE_SIZE 10000
#define EXT_LEN 500

typedef struct Table_like {
    size_t capacity;
    size_t len;
    Like_t *likes;
    unsigned char *cache_map;
    FILE *fp;
    char *file_name;
} Table_like_t;

Table_like_t *new_Table_like(char *file_name);
int add_Like(Table_like_t *table, Like_t *like);
int delete_Like(Table_like_t *table, int idx);
int archive_table_like(Table_like_t *table);
int load_table_like(Table_like_t *table, char *file_name);
Like_t* get_Like(Table_like_t *table, size_t idx);

#endif

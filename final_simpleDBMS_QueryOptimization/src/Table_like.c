#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Table_like.h"

///
/// Allocate a Table_like_t struct, then initialize some attributes, and
/// load table if the `file_name` is given
///
Table_like_t *new_Table_like(char *file_name) {
    Table_like_t *table = (Table_like_t*)malloc(sizeof(Table_like_t));
    memset((void*)table, 0, sizeof(Table_like_t));
    table->capacity = INIT_TABLE_SIZE;
    table->len = 0;
    table->likes = (Like_t*)malloc(
                            sizeof(Like_t) * INIT_TABLE_SIZE);
    table->cache_map = (unsigned char*)malloc(sizeof(char)*INIT_TABLE_SIZE);
    memset(table->cache_map, 0, sizeof(char)*INIT_TABLE_SIZE);
    table->fp = NULL;
    table->file_name = NULL;
    load_table(table, file_name);
    return table;
}

///
/// Add the `User_t` data to the given table
/// If the table is full, it will allocate new space to store more
/// user data
/// return 1 when the data successfully add to table
///
int add_Like(Table_like_t *table, Like_t *like) {
    size_t idx;
    Like_t *like_ptr;
    if (!table || !like) {
        return 0;
    }
    // Check id doesn't exist in the table
    for (idx = 0; idx < table->len; idx++) {
        like_ptr = get_Like(table, idx);
        if (like_ptr->id1 == like->id1) {
            return 0;
        }
    }
    if (table->len == table->capacity) {
        Like_t *new_like_buf = (Like_t*)malloc(sizeof(Like_t)*(table->len+EXT_LEN));
        unsigned char *new_cache_buf = (unsigned char *)malloc(sizeof(unsigned char)*(table->len+EXT_LEN));

        memcpy(new_like_buf, table->likes, sizeof(Like_t)*table->len);

        memset(new_cache_buf, 0, sizeof(unsigned char)*(table->len+EXT_LEN));
        memcpy(new_cache_buf, table->cache_map, sizeof(unsigned char)*table->len);


        free(table->likes);
        free(table->cache_map);
        table->likes = new_like_buf;
        table->cache_map = new_cache_buf;
        table->capacity += EXT_LEN;
    }
    idx = table->len;
    memcpy((table->likes)+idx, like, sizeof(Like_t));
    table->cache_map[idx] = 1;
    table->len++;
    return 1;
}

int delete_Like(Table_like_t *table, int idx) {
    //size_t table_len;
    int front, back;
    if (!table) {
        return 0;
    }
    front = idx;
    back = (table->len)-idx-1;
    
    Like_t *new_like_buf = (Like_t*)malloc(sizeof(Like_t)*(table->capacity));
    unsigned char *new_cache_buf = (unsigned char *)malloc(sizeof(unsigned char)*(table->capacity));

    memcpy(new_like_buf, table->likes, sizeof(Like_t)*front);
    memcpy((new_like_buf)+front, (table->likes)+front+1, sizeof(Like_t)*back);

    memset(new_cache_buf, 0, sizeof(unsigned char)*(table->capacity-1));
    memcpy(new_cache_buf, table->cache_map, sizeof(unsigned char)*front);
    memcpy((new_cache_buf)+front, (table->cache_map)+front+1, sizeof(unsigned char)*back);

    free(table->likes);
    free(table->cache_map);
    table->likes = new_like_buf;
    table->cache_map = new_cache_buf;
    table->len--;
    return 1;
}

///
/// Return value is the archived table len
///
int archive_table_like(Table_like_t *table) {
    size_t archived_len;
    struct stat st;

    if (table->fp == NULL) {
        return 0;
    }
    if (stat(table->file_name, &st) == 0) {
        archived_len = st.st_size / sizeof(Like_t);
    } else {
        archived_len = 0;
    }
    fwrite((void*)(table->likes+archived_len), \
            sizeof(Like_t), table->len-archived_len, \
            table->fp);

    fclose(table->fp);
    free(table->file_name);
    table->fp = NULL;
    table->file_name = NULL;
    return table->len;
}

///
/// Loading the db file will overwrite the existed records in table,
/// only if the ``file_name`` is NULL
/// Return: the number of records in the db file
///
int load_table_like(Table_like_t *table, char *file_name) {
    size_t archived_len;
    struct stat st;
    if (table->fp != NULL) {
        fclose(table->fp);
        free(table->file_name);
        table->fp = NULL;
        table->file_name = NULL;
    }
    if (file_name != NULL) {
        table->len = 0;
        memset(table->cache_map, 0, sizeof(char)*INIT_TABLE_SIZE);
        if (stat(file_name, &st) != 0) {
            //Create new file
            table->fp = fopen(file_name, "wb");
        } else {
            archived_len = st.st_size / sizeof(Like_t);
            if (archived_len > table->capacity) {
                Like_t *new_like_buf = (Like_t*)malloc(sizeof(Like_t)*(archived_len+EXT_LEN));
                unsigned char *new_cache_buf = (unsigned char *)malloc(sizeof(unsigned char)*(archived_len+EXT_LEN));

                memset(new_cache_buf, 0, sizeof(unsigned char)*(archived_len+EXT_LEN));

                free(table->likes);
                free(table->cache_map);
                table->likes = new_like_buf;
                table->cache_map = new_cache_buf;
                table->capacity = archived_len+EXT_LEN;
            }
            table->fp = fopen(file_name, "a+b");
            table->len = archived_len;
        }
        table->file_name = strdup(file_name);
    }
    return table->len;
}

///
/// Return the user in table by the given index
///
Like_t* get_Like(Table_like_t *table, size_t idx) {
    size_t archived_len;
    struct stat st;
    if (!table->cache_map[idx]) {
        if (idx > INIT_TABLE_SIZE) {
            goto error;
        }
        if (stat(table->file_name, &st) != 0) {
            goto error;
        }
        archived_len = st.st_size / sizeof(Like_t);
        if (idx >= archived_len) {
            //neither in file, nor in memory
            goto error;
        }

        fseek(table->fp, idx*sizeof(Like_t), SEEK_SET);
        fread(table->likes+idx, sizeof(Like_t), 1, table->fp);
        table->cache_map[idx] = 1;
    }
    return table->likes+idx;

error:
    printf("IN ERROR\n");
    return NULL;
}
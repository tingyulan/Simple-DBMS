#ifndef DB_UTIL_H
#define DB_UTIL_H
#include "Command.h"
#include "Table.h"
#include "Table_like.h"
typedef struct State {
    int saved_stdout;
} State_t;

State_t* new_State();
void print_prompt(State_t *state);
void print_user(User_t *user, SelectArgs_t *sel_args);
void print_like(Like_t *user, SelectArgs_t *sel_args);
void print_aggre_like(Table_like_t *table_like, Command_t *cmd, int limit, int offset);
void print_aggre(Table_t *table, Command_t *cmd, int limit, int offset);
void print_users(Table_t *table, Table_like_t *table_like, Command_t *cmd);
int parse_input(char *input, Command_t *cmd);
void handle_builtin_cmd(Table_t *table, Table_like_t *table_like,Command_t *cmd, State_t *state);
int handle_query_cmd(Table_t *table,Table_like_t *table_like, Command_t *cmd);
int handle_insert_cmd(Table_t *table,Table_like_t *table_like, Command_t *cmd);
int handle_delete_cmd(Table_t *table, Command_t *cmd);
void string_copy(char *str, int n);
void create_idxList(Command_t *cmd, Table_t *table);
int count_join(int count, int target, Table_like_t *table_like, int join_type);
void print_join(Table_t *table, Table_like_t *table_like, Command_t *cmd);
int handle_select_cmd(Table_t *table, Table_like_t *table_like, Command_t *cmd);
int handle_update_cmd(Table_t *table, Command_t *cmd);
void print_help_msg();

#endif
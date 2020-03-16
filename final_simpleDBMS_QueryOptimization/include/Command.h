#ifndef COMMAND_H
#define COMMAND_H
#include <stdbool.h>

enum { 
    UNRECOG_CMD,
    BUILT_IN_CMD,
    QUERY_CMD,
};

enum {
    INSERT_CMD = 100,
    SELECT_CMD,
};

typedef struct {
    char name[256];
    int len;
    unsigned char type;
} CMD_t;

extern CMD_t cmd_list[];

typedef struct SelectArgs {
    char **fields;
    size_t fields_len;
    char **where_fields;
    size_t where_fields_len;
    int offset;
    int limit;
    bool UserOrLike;  //user==0, like==1
    int join; //no==-1, id1==1, id2==2
} SelectArgs_t;

typedef union {
    SelectArgs_t sel_args;
} CmdArg_t;

typedef struct Command {
    unsigned char type;
    char **args;
    size_t args_len;
    size_t args_cap;
    CmdArg_t cmd_args;
} Command_t;

Command_t* new_Command();
int add_Arg(Command_t *cmd, const char *arg);
int add_select_field(Command_t *cmd, const char *argument);
int add_where_field(Command_t *cmd, const char *argument);
void cleanup_Command(Command_t *cmd);

#endif
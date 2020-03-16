#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include <stdbool.h>

///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, int field[], bool star) {
    //there are four elements in field
    int i,j,count=0, tmp=99999;
    int order[4]={-1, -1, -1, -1}, cfield[4];
    
    for(i=0; i<4; ++i){
        cfield[i] = field[i];
        if(cfield[i]>=0){ ++count; }
    }

    if(star==1 || count==0){
        printf("(%d, %s, %s, %d)\n", user->id, user->name, user->email, user->age);
    }else{
        for(i=0; i<count; i++){
            tmp=99999;
            for(j=0; j<4; ++j){
                if(cfield[j]!=-1 && cfield[j]<=tmp){
                    order[i]=j;
                    tmp = cfield[j];
		}
            }
            cfield[order[i]]=-1;
        }

        printf("(");
	for(i=0; i<count; ++i){
            if(i!=0){ printf(", "); }
	    if(order[i]==0){ printf("%d", user->id); }
	    else if(order[i]==1){ printf("%s", user->name); }
	    else if(order[i]==2){ printf("%s", user->email); }
            else if(order[i]==3){ printf("%d", user->age); }
        }
        printf(")\n");
    }
}

///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " \n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " \n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        archive_table(table);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    } else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(Table_t *table, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)|| !strncmp(cmd->args[0], "INSERT", 6)) {
        handle_insert_cmd(table, cmd);
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)|| !strncmp(cmd->args[0], "SELECT", 6)) {
        handle_select_cmd(table, cmd);
        return SELECT_CMD;
    } else {
        return UNRECOG_CMD;
    }
}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;
    unsigned int idx;
    bool flg=1; //0: can not insert, 1:can insert
    User_t *tmp_user;
    User_t *user = command_to_User(cmd);
    
    for(idx=0; idx< table->len; idx++){
        tmp_user = get_User(table, idx);
        if(tmp_user->id == user->id){
            flg=0;
            break;
        }
    }
    
    if (flg) {
        ret = add_User(table, user);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}

///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table, Command_t *cmd) {
    int idx;
    unsigned int i;
    bool star=0;
    int field[4]={-1, -1, -1, -1}; //id, name, email, age
    int start=0, lim=table->len;
    
    for (i=0; i < (cmd->args_len); ++i){
        if( !strcmp(cmd->args[i], "offset")||!strcmp(cmd->args[i], "OFFSET") ){
            start = atoi(cmd->args[i+1]);
        }else if ( !strcmp(cmd->args[i], "limit")||!strcmp(cmd->args[i], "LIMIT") ){
            lim = atoi(cmd->args[i+1]);
        }else if( !strncmp(cmd->args[i], "*", 1) ){ star = 1; }
        else if( !strncmp(cmd->args[i], "id", 2) ){ field[0] = i; }
        else if( !strncmp(cmd->args[i], "name", 4) ){ field[1] = i; }
        else if( !strncmp(cmd->args[i], "email", 5) ){ field[2] = i; }
        else if( !strncmp(cmd->args[i], "age", 3) ){ field[3] = i; }
    }
    for (idx = start; idx < start+lim && idx< (int)table->len; ++idx) {
        print_user(get_User(table, idx), field, star);
    }
    cmd->type = SELECT_CMD;
    return table->len;
}

///
/// Show the help messages
///
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";
    printf("%s", msg);
}

#include <stdio.h>
#include <stdio_ext.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "Table_like.h"
#include "SelectState.h"
// #include "UpdataState.h"

#define STR_LEN 100
#define STR_LEN_2 20

//GLOBAL VARIABLE
bool target[4]={0, 0, 0, 0}; //id, name, email, age
bool op[6]={0,0,0,0,0,0}; //=, !=, >=, <=, >, <
int step=0;
bool *idxList;

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
void print_user(User_t *user, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id", 2)) {
                printf("%d", user->id);
            } else if (!strncmp(sel_args->fields[idx], "name", 4)) {
                printf("%s", user->name);
            } else if (!strncmp(sel_args->fields[idx], "email", 5)) {
                printf("%s", user->email);
            } else if (!strncmp(sel_args->fields[idx], "age", 3)) {
                printf("%d", user->age);
            }
        }
    }
    printf(")\n");
}

void print_like(Like_t *user, SelectArgs_t *sel_args) {
    printf("IN PRINT LIKE\n");
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %d", user->id1, user->id2);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id1", 3)) {
                printf("%d", user->id1);
            } else if (!strncmp(sel_args->fields[idx], "id2", 3)) {
                printf("%d", user->id2);
            } 
        }
    }
    printf(")\n");
}

void print_aggre(Table_t *table, Command_t *cmd, int limit, int offset){
    char *cmmd;
    char p[STR_LEN_2+1];
    register int pick_num;
    register int i, j;
    register int aggre;  //1:avg, 2:count, 3:sum
    double tmp=0;
    register int count=0;
    bool flg_first=1;
    size_t idxListLen = table->len;
    User_t *user;
    //int coffset=0, num_print=0; //count offset & count limit
    for (i=0; i<STR_LEN_2+1; ++i){
        p[i] = '\0';
    }
    
    
    if(offset<=0 && limit!=0){
        printf("(");
    }
    
    for (i=0; i<(cmd->cmd_args.sel_args.fields_len); ++i){
        cmmd = cmd->cmd_args.sel_args.fields[i];
        if ( !strncmp(cmmd, "avg", 3) ){
            for(j=4; j<strlen(cmmd)-1; ++j){  //IGNORE ')'
                p[j-4]=cmmd[j];
            }
            p[strlen(cmmd)-1-4]='\0';
            aggre=1;
        }else if ( !strncmp(cmd->cmd_args.sel_args.fields[i], "count", 5) ){
            for(j=6; j<strlen(cmmd)-1; ++j){  //IGNORE ')'
                p[j-6]=cmmd[j];
            }
            p[strlen(cmmd)-1-6]='\0';
            aggre=2;
        }else if ( !strncmp(cmd->cmd_args.sel_args.fields[i], "sum", 3) ){
            for(j=4; j<strlen(cmmd)-1; ++j){  //IGNORE ')'
                p[j-4]=cmmd[j];
            }
            p[strlen(cmmd)-1-4]='\0';
            aggre=3;
        }
        tmp=0;
        count=0;
        for(j=0; j<idxListLen; ++j){
            
            if(idxList[j]==1){
            //if(idxList[j]==1 && table->cache_map[j]==1){
                //if(coffset < offset){
                //    coffset +=1;
                //}else{
                    //if (limit != -1 && (num_print) >= limit) {
                    //    break;
                    //}

                    user = get_User(table, j);
                    if(aggre==1 || aggre==3){
                        if (!strncmp(p, "id", 2)) {
                            pick_num = user->id;
                        } else if (!strncmp(p, "age", 3)) {
                            pick_num = user->age;
                        }
                    }
                    
                    if(aggre==1 || aggre==3){  //AVG || SUM
                        tmp += pick_num;
                    }
                    count += 1;
                    //num_print += 1;
                //}
            }
        }


        if(offset<=0 && limit!=0){
          if(flg_first==0){ printf(", "); }
          if(aggre==1){
              tmp = (double)tmp/count;
              printf("%.3lf", tmp);
              flg_first=0;
          }else if(aggre==2){
              printf("%d", count);
              flg_first=0;
          }else if(aggre==3){
              printf("%d", (int)tmp);
              flg_first=0;
          }
        }

    }
    
    if(offset<=0 && limit!=0){
        printf(")\n");
    }
}

void print_aggre_like(Table_like_t *table_like, Command_t *cmd, int limit, int offset){
    char *cmmd;
    char p[STR_LEN_2+1];
    register int pick_num;
    register int i, j;
    register int aggre;  //1:avg, 2:count, 3:sum
    double tmp=0;
    register int count=0;
    bool flg_first=1;
    size_t idxListLen = table_like->len;
    Like_t *like;
    //int coffset=0, num_print=0; //count offset & count limit
    for (i=0; i<STR_LEN_2+1; ++i){
        p[i] = '\0';
    }
    
    
    if(offset<=0 && limit!=0){
        printf("(");
    }
    
    for (i=0; i<(cmd->cmd_args.sel_args.fields_len); ++i){
        cmmd = cmd->cmd_args.sel_args.fields[i];
        if ( !strncmp(cmmd, "avg", 3) ){
            for(j=4; j<strlen(cmmd)-1; ++j){  //IGNORE ')'
                p[j-4]=cmmd[j];
            }
            p[strlen(cmmd)-1-4]='\0';
            aggre=1;
        }else if ( !strncmp(cmd->cmd_args.sel_args.fields[i], "count", 5) ){
            for(j=6; j<strlen(cmmd)-1; ++j){  //IGNORE ')'
                p[j-6]=cmmd[j];
            }
            p[strlen(cmmd)-1-6]='\0';
            aggre=2;
        }else if ( !strncmp(cmd->cmd_args.sel_args.fields[i], "sum", 3) ){
            for(j=4; j<strlen(cmmd)-1; ++j){  //IGNORE ')'
                p[j-4]=cmmd[j];
            }
            p[strlen(cmmd)-1-4]='\0';
            aggre=3;
        }
        tmp=0;
        count=0;
        for(j=0; j<idxListLen; ++j){
            
  
          like = get_Like(table_like, j);
          if(aggre==1 || aggre==3){
              if (!strncmp(p, "id1", 3)) {
                  pick_num = like->id1;
              } else if (!strncmp(p, "id2", 3)) {
                  pick_num = like->id2;
              }
          }
          
          if(aggre==1 || aggre==3){  //AVG || SUM
              tmp += pick_num;
          }
          count += 1;
                    //num_print += 1;
                //}
            
        }

        if(offset<=0 && limit!=0){
          if(flg_first==0){ printf(", "); }
          if(aggre==1){
              tmp = (double)tmp/count;
              printf("%.3lf", tmp);
              flg_first=0;
          }else if(aggre==2){
              printf("%d", count);
              flg_first=0;
          }else if(aggre==3){
              printf("%d", (int)tmp);
              flg_first=0;
          }
        }

    }
    
    if(offset<=0 && limit!=0){
        printf(")\n");
    }
}

///
/// Print the users for given offset and limit restriction
///
void print_users(Table_t *table, Table_like_t *table_like, Command_t *cmd) {
    size_t idx;
    register int limit = cmd->cmd_args.sel_args.limit;
    register int offset = cmd->cmd_args.sel_args.offset;
    register int i;
    bool flg_aggre=0; //0:no aggre, 1:aggre
    register int coffset=0, num_print=0; //count offset & count limit

    //INITIAL
    if (offset == -1) {
        offset = 0;
    }
    
    for (i=0; i<(cmd->cmd_args.sel_args.fields_len); ++i){
        if ( strncmp(cmd->cmd_args.sel_args.fields[i], "avg", 3)==0 ){ flg_aggre=1; }
        else if ( strncmp(cmd->cmd_args.sel_args.fields[i], "count", 5)==0 ){ flg_aggre=1; }
        else if ( strncmp(cmd->cmd_args.sel_args.fields[i], "sum", 3)==0 ){ flg_aggre=1; }
    }
    
    if(flg_aggre==0){  //THERE IS NO AGGREGATION FUNCTION
        
        if( cmd->cmd_args.sel_args.UserOrLike==1 ){
            
            for (idx = 0; idx < table_like->len; idx++) {
                if(coffset < offset){
                    coffset += 1;
                }else{
                    if (limit != -1 && (num_print) >= limit) {
                        break;
                    }
                    
                    print_like(get_Like(table_like, idx), &(cmd->cmd_args.sel_args));
                    num_print += 1;
                }
            }
        }else{
            for (idx = 0; idx < table->len; idx++) {
                if(idxList[idx]==1){
                    if(coffset < offset){
                        coffset += 1;
                    }else{
                        if (limit != -1 && (num_print) >= limit) {
                            break;
                        }
                        print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));
                        num_print += 1;
                    }
                }
            }
        }
    }else{  //THERE IS AGGREGATION FUNCTION
        if(cmd->cmd_args.sel_args.UserOrLike==0){
            print_aggre(table, cmd, limit, offset);
        }else{
            print_aggre_like(table_like, cmd, limit, offset);
              //*****************************
        }
    }
}

///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    register int idx;
    token = strtok(input, " ,\n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, Table_like_t *table_like, Command_t *cmd, State_t *state) {
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
                register int fd = creat(cmd->args[1], 0644);
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
int handle_query_cmd(Table_t *table,Table_like_t *table_like, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd(table,table_like, cmd);
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd(table,table_like, cmd);
        return SELECT_CMD;
    } else if (!strncmp(cmd->args[0], "update", 6)){
        handle_update_cmd(table, cmd);
        return SELECT_CMD; //!!!!!!!!!!!!!!!!!!
    } else if (!strncmp(cmd->args[0], "delete", 6)){
        handle_delete_cmd(table, cmd);
        return SELECT_CMD;  //!!!!!!!!!!!!!!11
    }else {
        return UNRECOG_CMD;
    }
}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table,Table_like_t *table_like, Command_t *cmd) {

    register int ret = 0;
    if(!strncmp(cmd->args[2], "user", 4)){
        
        User_t *user = command_to_User(cmd);
        
        if (user) {
            
            ret = add_User(table, user);
            
            if (ret > 0) {
                cmd->type = INSERT_CMD;
            }
        }
    }
    else if(!strncmp(cmd->args[2], "like", 4)){
        
        Like_t *like = command_to_Like(cmd);
        if (like) {
          
            ret = add_Like(table_like, like);
            
            if (ret > 0) {
                cmd->type = INSERT_CMD;
            }
        }
    }
    
    return ret;
}

int handle_delete_cmd(Table_t *table, Command_t *cmd) {
    size_t idxListLen = table->len;
    register int idx;
    
    if( cmd->args_len>3 ){
        where_state_handler(cmd, 4);
    }
    create_idxList(cmd, table);
    for(idx=idxListLen-1; idx>=0; --idx){
        if(idxList[idx]==1){
            delete_User(table, idx);
        }
    }
    
    return 0;
}

void string_copy(char *str, int n){
    char tmp[STR_LEN+1];
    register int len = strlen(str);
    register int i;
    
    for (i=0; i<STR_LEN+1; ++i){
        tmp[i]='\0';
    }
    for (i=n; i<len; ++i){
        tmp[i-n]=str[i];
    }
    for (i=0; i<STR_LEN+1; ++i){
        str[i]='\0';
    }
    strncpy(str, tmp, sizeof(tmp)-1);
    str[sizeof(tmp)-1] = '\0';
}

//
// Handle where
//
void create_idxList(Command_t *cmd, Table_t *table){
    //where_fields_len  0:no, 3:NOandor, 7:andor
    size_t where_fields_len = cmd->cmd_args.sel_args.where_fields_len;
    size_t idxListLen = table->len;
    register int k, i;
    register int tar[2] = {-1, -1}; //1:id, 2:name, 3:email, 4:age
    register int operation[2] = {-1, -1};  //1:=, 2:!=, 3:>=, 4:<=, 5:>, 6:<
    register int tarnum1=-1, tarnum2=-1;
    char *tarstr1, *tarstr2;
    int pick, pick2;  //pick_num
    char *pick_str, *pick_str2;
    User_t *user;

    // char p[STR_LEN+1];
    // int constraint=-1;
    // char con_str[STR_LEN+1];
    // bool numorstr=0; //0:numerical, 1:string
    register int andor=-1; //-1:no and or, 1:and, 2:or
    idxList = (bool*)malloc(idxListLen * sizeof(bool));

    //INITIAL
    for (i=0; i<idxListLen; ++i){
        idxList[i] = 1;
    }

    step=0;
    // for(i=0; i<4; ++i){ target[i]=0; }
    // for(i=0; i<6; ++i){ op[i]=0; }
    //IF THERE IS NO WHERE CLAUSE
    if(where_fields_len==0){
        return;
    }else if(where_fields_len==7){
        if(strncmp(cmd->cmd_args.sel_args.where_fields[3], "and", 3)==0){andor=1;}
        else if(strncmp(cmd->cmd_args.sel_args.where_fields[3], "or", 2)==0){andor=2;}
    }

    if(strncmp(cmd->cmd_args.sel_args.where_fields[0], "id", 2)==0){
        tar[0] = 1;
        tarnum1 = atoi(cmd->cmd_args.sel_args.where_fields[2]);
    }else if(strncmp(cmd->cmd_args.sel_args.where_fields[0], "name", 4)==0){
        tar[0] = 2;
        tarstr1 = cmd->cmd_args.sel_args.where_fields[2];
    }else if(strncmp(cmd->cmd_args.sel_args.where_fields[0], "email", 5)==0){
        tar[0] = 3;
        tarstr1 = cmd->cmd_args.sel_args.where_fields[2];
    }else if(strncmp(cmd->cmd_args.sel_args.where_fields[0], "age", 3)==0){
        tar[0] = 4;
        tarnum1 = atoi(cmd->cmd_args.sel_args.where_fields[2]);
    }

    if(strncmp(cmd->cmd_args.sel_args.where_fields[1], "=", 1)==0){
        operation[0] = 1;
    }else if(strncmp(cmd->cmd_args.sel_args.where_fields[1], "!=", 2)==0){
        operation[0] = 2;
    }else if(strncmp(cmd->cmd_args.sel_args.where_fields[1], ">=", 2)==0){
        operation[0] = 3;
    }else if(strncmp(cmd->cmd_args.sel_args.where_fields[1], "<=", 2)==0){
        operation[0] = 4;
    }else if(strncmp(cmd->cmd_args.sel_args.where_fields[1], ">", 1)==0){
        operation[0] = 5;
    }else if(strncmp(cmd->cmd_args.sel_args.where_fields[1], "<", 1)==0){
        operation[0] = 6;
    }

    if(where_fields_len==7){
        if(strncmp(cmd->cmd_args.sel_args.where_fields[4], "id", 2)==0){
            tar[1] = 1;
            tarnum2 = atoi(cmd->cmd_args.sel_args.where_fields[6]);
        }else if(strncmp(cmd->cmd_args.sel_args.where_fields[4], "name", 4)==0){
            tar[1] = 2;
            tarstr2 = cmd->cmd_args.sel_args.where_fields[6];
        }else if(strncmp(cmd->cmd_args.sel_args.where_fields[4], "email", 5)==0){
            tar[1] = 3;
            tarstr2 = cmd->cmd_args.sel_args.where_fields[6];
        }else if(strncmp(cmd->cmd_args.sel_args.where_fields[4], "age", 3)==0){
            tar[1] = 4;
            tarnum2 = atoi(cmd->cmd_args.sel_args.where_fields[6]);
        }

        if(strncmp(cmd->cmd_args.sel_args.where_fields[5], "=", 1)==0){
            operation[1] = 1;
        }else if(strncmp(cmd->cmd_args.sel_args.where_fields[5], "!=", 2)==0){
            operation[1] = 2;
        }else if(strncmp(cmd->cmd_args.sel_args.where_fields[5], ">=", 2)==0){
            operation[1] = 3;
        }else if(strncmp(cmd->cmd_args.sel_args.where_fields[5], "<=", 2)==0){
            operation[1] = 4;
        }else if(strncmp(cmd->cmd_args.sel_args.where_fields[5], ">", 1)==0){
            operation[1] = 5;
        }else if(strncmp(cmd->cmd_args.sel_args.where_fields[5], "<", 1)==0){
            operation[1] = 6;
        }
    }
    
    //IF THERE IS A WHERE CLAUSE
    if(andor==-1){
        
        for(k=0; k<idxListLen; ++k){
            user = get_User(table, k);
            if(tar[0]==1){
                pick = user->id;
            }else if(tar[0]==2){
                pick_str = user->name;
            }else if(tar[0]==3){
                pick_str = user->email;
            }else{  //tar[0]==1
                pick = user->age;
            }
            
            if(operation[0]==1 && tarnum1!=-1){  //=
                if(!(pick==tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==2 && tarnum1!=-1){  //!=
                if(!(pick!=tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==3){  //>=
                if(!(pick>=tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==4){  //<=
                if(!(pick<=tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==5){  //>
                if(!(pick>tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==6){  // <
                if(!(pick<tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==1 && tarnum1==-1){
                if( !(strcmp(pick_str, tarstr1)==0) ){
                    idxList[k]=0;
                }
            }else if(operation[0]==2 && tarnum1==-1){
                if( !(strcmp(pick_str, tarstr1)!=0) ){
                    idxList[k]=0;
                }
            }
        }
    }else if(andor==1){  //AND
        //printf("operation[0], operation[1]: %d, %d\n", operation[0], operation[1]);
        for(k=0; k<idxListLen; ++k){
            user = get_User(table, k);
            if(tar[0]==1){
                pick = user->id;
            }else if(tar[0]==2){
                pick_str = user->name;
            }else if(tar[0]==3){
                pick_str = user->email;
            }else{  //tar[0]==1
                pick = user->age;
            }
            if(tar[1]==1){
                pick2 = user->id;
            }else if(tar[1]==2){
                pick_str2 = user->name;
            }else if(tar[1]==3){
                pick_str2 = user->email;
            }else{  //tar[0]==1
                pick2 = user->age;
            }

            if(operation[0]==1 && tarnum1!=-1){  //=
                if(!(pick==tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==2 && tarnum1!=-1){  //!=
                if(!(pick!=tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==3){  //>=
                if(!(pick>=tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==4){  //<=
                if(!(pick<=tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==5){  //>
                if(!(pick>tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==6){  // <
                if(!(pick<tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==1 && tarnum1==-1){
                if( !(strcmp(pick_str, tarstr1)==0) ){
                    idxList[k]=0;
                }
            }else if(operation[0]==2 && tarnum1==-1){
                if( !(strcmp(pick_str, tarstr1)!=0) ){
                    idxList[k]=0;
                }
            }
            if(operation[1]==1 && tarnum2!=-1){  //=
                if(!(pick2==tarnum2)){ idxList[k]=0; }
            }else if(operation[1]==2 && tarnum2!=-1){  //!=
                if(!(pick2!=tarnum2)){ idxList[k]=0; }
            }else if(operation[1]==3){  //>=
                if(!(pick2>=tarnum2)){ idxList[k]=0; }
            }else if(operation[1]==4){  //<=
                if(!(pick2<=tarnum2)){ idxList[k]=0; }
            }else if(operation[1]==5){  //>
                if(!(pick2>tarnum2)){ idxList[k]=0; }
            }else if(operation[1]==6){  // <
                if(!(pick2<tarnum2)){ idxList[k]=0; }
            }else if(operation[1]==1 && tarnum2==-1){
                if( !(strcmp(pick_str2, tarstr2)==0) ){
                    idxList[k]=0;
                }
            }else if(operation[1]==2 && tarnum2==-1){
                //printf("HERE\n");
                if( !(strcmp(pick_str2, tarstr2)!=0) ){
                    //printf("HIHI\n");
                    idxList[k]=0;
                }
            }
        }
    }else{ //andor==2  OR
        for(k=0; k<idxListLen; ++k){
            user = get_User(table, k);
            if(tar[0]==1){
                pick = user->id;
            }else if(tar[0]==2){
                pick_str = user->name;
            }else if(tar[0]==3){
                pick_str = user->email;
            }else{  //tar[0]==1
                pick = user->age;
            }
            if(tar[1]==1){
                pick2 = user->id;
            }else if(tar[1]==2){
                pick_str2 = user->name;
            }else if(tar[1]==3){
                pick_str2 = user->email;
            }else{  //tar[0]==1
                pick2 = user->age;
            }

            if(operation[0]==1 && tarnum1!=-1){  //=
                if(!(pick==tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==2 && tarnum1!=-1){  //!=
                if(!(pick!=tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==3){  //>=
                if(!(pick>=tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==4){  //<=
                if(!(pick<=tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==5){  //>
                if(!(pick>tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==6){  // <
                if(!(pick<tarnum1)){ idxList[k]=0; }
            }else if(operation[0]==1 && tarnum1==-1){
                if( !(strcmp(pick_str, tarstr1)==0) ){
                    idxList[k]=0;
                }
            }else if(operation[0]==2 && tarnum1==-1){
                if( !(strcmp(pick_str, tarstr1)!=0) ){
                    idxList[k]=0;
                }
            }
            if(operation[1]==1 && tarnum2!=-1){  //=
                if((pick2==tarnum2)){ idxList[k]=1; }
            }else if(operation[1]==2 && tarnum2!=-1){  //!=
                if((pick2!=tarnum2)){ idxList[k]=1; }
            }else if(operation[1]==3){  //>=
                if((pick2>=tarnum2)){ idxList[k]=1; }
            }else if(operation[1]==4){  //<=
                if((pick2<=tarnum2)){ idxList[k]=1; }
            }else if(operation[1]==5){  //>
                if((pick2>tarnum2)){ idxList[k]=1; }
            }else if(operation[1]==6){  // <
                if((pick2<tarnum2)){ idxList[k]=1; }
            }else if(operation[1]==1 && tarnum2==-1){
                if( (strcmp(pick_str2, tarstr2)==0) ){
                    idxList[k]=1;
                }
            }else if(operation[1]==2 && tarnum2==-1){
                if( (strcmp(pick_str2, tarstr2)!=0) ){
                    idxList[k]=1;
                }
            }
        }
    }


    return;
}

int count_join(int count, int target, Table_like_t *table_like, int join_type){
    register int j;
    register int likelen = table_like->len;
    Like_t *like;
    for(j=0; j<likelen; ++j){
        like = get_Like(table_like, j);
        if(join_type==1){ if(like->id1==target) ++count; }
        else{ if(like->id2==target) ++count; }
    }
    return count;
}


void print_join(Table_t *table, Table_like_t *table_like, Command_t *cmd){
    register int idx, j;
    register int count=0;
    register int target = -1;
    User_t *user;
    register int likelen = table_like->len;
    Like_t *like;
    size_t idxListLen = table->len;
    
    //printf("IN PRINT JOIN\n");
    for (idx=0; idx<idxListLen; ++idx){
          //printf("idx:%d\n", idx);
          if(idxList[idx]==1){
                user = get_User(table, idx);
                target = user->id;
                for(j=0; j<likelen; ++j){
                    like = get_Like(table_like, j);
                    if(cmd->cmd_args.sel_args.join==1){ if(like->id1==target) ++count; }
                    else{ if(like->id2==target) ++count; }
                }
          }
    } 
    printf("(%d)\n", count);
}

///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table, Table_like_t *table_like, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd, 1);
    
    if(cmd->cmd_args.sel_args.join==-1){
        if(cmd->cmd_args.sel_args.UserOrLike==1){  //like
          print_users(table, table_like, cmd);
        }else{    //user
          create_idxList(cmd, table);
          print_users(table, table_like, cmd);
        }
    }else{
        //Do Join Here
        //printf("GOING TO CREATE IDX");
        create_idxList(cmd, table);
        //printf("AFTER CREATE IDX");
        print_join(table, table_like, cmd);
        //printf("HI~~\n");
    }
    return table->len;
}

int handle_update_cmd(Table_t *table, Command_t *cmd) {
    //cmd->type = SELECT_CMD;
    //size_t where_fields_len = cmd->cmd_args.sel_args.where_fields_len;
    size_t idxListLen = table->len;
    User_t *user;
    // char set[STR_LEN+1];
    register int target=-1; //1:id, 2:name, 3:email, 4:age
    register int change_num;
    char change_str[STR_LEN+1];
    register int idx;
    register int count=0;
    bool primary=0;
    //INITIALIZE
    // for (i=0; i<STR_LEN+1; ++i){
    //     set[i]='\0';
    // }
    // for(i=0; i<strlen(cmd->args[3]); ++i){
    //     set[i]=cmd->args[3][i];
    // }
    //set[strlen(cmd->args[3])]='\0';
    if( cmd->args_len>6 ){
        where_state_handler(cmd, 7);
    }

    if(!strncmp(cmd->args[3], "id", 2)){
        target=1;
        // string_copy(set, 3);
        change_num = atoi(cmd->args[5]);
    }else if(!strncmp(cmd->args[3], "name", 4)){
        target=2;
        //string_copy(set, 5);
        for (register int i=0; i<strlen(cmd->args[5]) && i<STR_LEN+1; ++i){
            change_str[i]=cmd->args[5][i];
        }
        change_str[strlen(cmd->args[5])]='\0';
    }else if(!strncmp(cmd->args[3], "email", 5)){
        target=3;
        //string_copy(set, 6);
        for (register int i=0; i<strlen(cmd->args[5]) && i<STR_LEN+1; ++i){
            change_str[i]=cmd->args[5][i];
        }
        change_str[strlen(cmd->args[5])]='\0';
    }else if(!strncmp(cmd->args[3], "age", 3)){
        target=4;
        //string_copy(set, 4);
        change_num = atoi(cmd->args[5]);
    }

    create_idxList(cmd, table);

    if(target==1){  //CHECK PRIMATY
        for(idx=0; idx<idxListLen; ++idx){
            if(idxList[idx]==1){ ++count; }
            user = get_User(table, idx);
            if(user->id==change_num){ primary=1; break;}
        }
        if(count>1) { primary=1; }  //MORE THAN ONE ROW IS GOING TO UPDATE ID
    }
    
    for(idx=0; idx<idxListLen; ++idx){
        if(idxList[idx]==1){
            user = get_User(table, idx);
            if(target==1 && primary==0){
                user->id = change_num;
            }else if(target==2){
                strncpy(user->name, change_str, strlen(change_str));
                user->name[sizeof(change_str)] = '\0';
                // user->name = change_str;
            }else if(target==3){
                strncpy(user->email, change_str, strlen(change_str));
                user->email[sizeof(change_str)] = '\0';
            }else if(target==4){
                user->age = change_num;
            }
        }
    }
    return 0;
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

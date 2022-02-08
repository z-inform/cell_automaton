#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/rule.h"
#include "../include/parser.h"

static void _free_typearr(char** type_arr);

int file_parser(char* filename, RuleType** rule_arr_ptr){
    FILE* rule_file = fopen(filename, "r");
    if (rule_file == NULL)
        return PARS_ERROPEN;

    char str_buffer[121] = "";
    char* types_arr[256] = {NULL};
    types_arr[0] = "void";

    int scan_status = fscanf(rule_file, " types "); //reading cell type declarations
    if (scan_status == EOF) {
        printf("Unexpected end of file before cell type declaration\n");
        return PARS_ERRSYNT;
    }

    if (fgetc(rule_file) != '{') {
        printf("Cell type declaration missing\n");
        return PARS_ERRSYNT;
    }

    
    for(int i = 1; i < 256; i++) {
        
        scan_status = fscanf(rule_file, " %120[^; \f\r\v\n\t] ", str_buffer);
        if (scan_status == EOF) {
            printf("EOF when scaning cell type name\n");
            _free_typearr(types_arr);
            return PARS_ERRSYNT;
        }

        if (fgetc(rule_file) != ';') {
            printf("Couldn't scan cell type name\n");
            _free_typearr(types_arr);
            return PARS_ERRSYNT;
        }

        types_arr[i] = malloc(strlen(str_buffer) + 1);
        if (types_arr[i] == NULL) {
            _free_typearr(types_arr);
            return PARS_ERRMEM;
        }
        memcpy(types_arr[i], str_buffer, strlen(str_buffer) + 1);

        scan_status = 0;
        if (fscanf(rule_file, " }%n", &scan_status) == EOF) {
            printf("Unexpected end of file in cell type declaration\n");
            _free_typearr(types_arr);
            return PARS_ERRSYNT;
        }
        if (scan_status > 0) break;
    }

    printf("Active types:\n"); //reading rules
    for(int i = 0; (types_arr[i] != NULL) && (i < 256); i++) {
        printf("%d: [%s]\n", i, types_arr[i]);
    }


    while(1){
        scan_status = fscanf(rule_file, " %s ", str_buffer);
        if (scan_status == EOF) {
            break;
        }

        if (fgetc(rule_file) != '{') {
            printf("Syntax error in rule definitions\n");
            _free_typearr(types_arr);
            return PARS_ERRSYNT;
        }

        int current_type = -1;

        for(int i = 0; (types_arr[i] != NULL) && (i < 256); i++){
            if (!strcmp(types_arr[i], str_buffer)) {
                current_type = i;
                break;
            }
        }

        if (current_type == -1) {
            printf("Encountered an undeclared type in rule definition: [%s]\n", str_buffer);
            _free_typearr(types_arr);
            return PARS_ERRSYNT;
        }

    }

    return 0;
}

static void _free_typearr(char** type_arr) {
    for(int i = 1; (type_arr[i] != NULL) && (i < 256); i++)
        free(type_arr[i]);
}


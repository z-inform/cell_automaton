#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/rule.h"
#include "../include/parser.h"

int file_parser(char* filename, RuleType** rule_arr_ptr){
    FILE* rule_file = fopen(filename, "r");
    if (rule_file == NULL)
        return PARS_ERROPEN;

    char str_buffer[121] = "";
    char* types_arr[256] = {NULL};
    types_arr[0] = "void";

    int scan_status = fscanf(rule_file, " types ");
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
            for(int k = 0; k < i; k++) 
                free(types_arr[k]);
            return PARS_ERRSYNT;
        }

        if (fgetc(rule_file) != ';') {
            printf("Couldn't scan cell type name\n");
            for(int k = 0; k < i; k++) 
                free(types_arr[k]);
            return PARS_ERRSYNT;
        }

        types_arr[i] = malloc(strlen(str_buffer) + 1);
        if (types_arr[i] == NULL) {
            for(int k = 0; k < i; k++) 
                free(types_arr[k]);
            return PARS_ERRMEM;
        }
        memcpy(types_arr[i], str_buffer, strlen(str_buffer) + 1);

        scan_status = 0;
        if (fscanf(rule_file, " }%n", &scan_status) == EOF) {
            printf("Unexpected end of file in cell type declaration\n");
            for(int k = 0; k < i; k++) 
                free(types_arr[k]);
            return PARS_ERRSYNT;
        }
        if (scan_status > 0) break;
    }

    for(int i = 0; types_arr[i] != NULL; i++) {
        printf("%d: [%s]\n", i, types_arr[i]);
    }

    return 0;
}


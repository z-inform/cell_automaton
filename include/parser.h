#pragma once
#include "rule.h"
#define PARS_ERROPEN -10
#define PARS_ERRSYNT -11
#define PARS_ERRMEM  -12

int file_parser(char* filename, RuleType** rule_arr_ptr);


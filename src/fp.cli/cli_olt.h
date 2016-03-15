#ifndef _CLI_UNICOM_H_
#define _CLI_UNICOM_H_

#include <my_types.h>

#define MAX_PARAMS 20
#define MAX_PARAM_NAME_SIZE 20
#define MAX_PARAM_VALUE_SIZE 20


typedef int(*T_CLI_SET_GET_FUNCTION)(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);

typedef int(*T_CLI_SET_GET_FUNCTION_HELP)(void);


typedef struct {
  char         name[21];
  char         description[201];
  char         syntax[201];
  unsigned int params_max;
  char         params[MAX_PARAMS][MAX_PARAM_NAME_SIZE];
  T_CLI_SET_GET_FUNCTION function;
  char         separator[101];
} command_struct;


extern const command_struct command[];

extern int cli_unicom(int argc, const char **argv);
#endif /*_CLI_UNICOM_H_*/


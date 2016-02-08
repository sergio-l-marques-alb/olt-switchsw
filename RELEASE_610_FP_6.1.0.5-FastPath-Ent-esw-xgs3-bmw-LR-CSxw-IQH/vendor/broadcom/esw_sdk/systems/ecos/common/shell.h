#ifndef _SHELL_H_
#define _SHELL_H_

#include <pkgconf/hal.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_tables.h>
#include <string.h>

typedef void cmd_func(int argc, char *argv[]);

struct shell_cmd {
	char       *name;
	cmd_func   *func;
	char       *usage;
	char       *helpstr;
} CYG_HAL_TABLE_TYPE;


#define ECOS_SHELL_CMD(str, func, usage, help_str)   \
	extern cmd_func func; \
	struct shell_cmd cmd_tbl_##func CYG_HAL_TABLE_QUALIFIED_ENTRY(Shell_cmds, func) = {str, func, usage, help_str}


#endif /* _SHELL_H_ */

#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_if.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "shell.h"

#include <pkgconf/kernel.h>
#include <pkgconf/isoinfra.h>

//#include <ibde.h>
//#include <cyg/hal/bcmnvram.h>


#include CYGHWR_MEMORY_LAYOUT_H

/* defines */
#define CYGNUM_SHELL_STACK_SIZE       (4*1024)
#define CYG_SHELL_PROMPT              "ecos>"
#define CYG_SHELL_WELCOME_MSG         "Welcome to ECOS shell"
#define CYG_CMD_BUF_SIZE               80
#define CYG_MAX_CMDS                   10
#define SHELL_PRIORITY                 18   /* originally 10 */
#define SHELL_TIMEOUT                  400
#define MAX_ARGV                       16

extern cyg_handle_t cyg_shell_init(void);
#ifndef LVL7_FIXUP /* alok */
extern void ifconfigt(int argc, char *argv[]);
#endif

CYG_HAL_TABLE_BEGIN( __Shell_CMD_TBL__, Shell_cmds);
CYG_HAL_TABLE_END( __Shell_CMD_TBL_END__, Shell_cmds );
extern struct shell_cmd __Shell_CMD_TBL__[], __Shell_CMD_TBL_END__;
			   
cyg_thread cyg_shell_thread_obj;

char cyg_shell_stack[CYGNUM_SHELL_STACK_SIZE];   /*stack size*/

cyg_handle_t cyg_shell_thread_hndl;

cyg_thread_entry_t  cyg_shell_thread;

static bool cyg_shell_cmd_get(char* cmd, cyg_uint32 size);
static bool cyg_console_read_char(char* c);
static void cyg_console_write_char(char c);
static void cyg_console_write_buf(char* buf, int buflen);
static struct shell_cmd * cmd_parse(char **line, int *argc, char **argv);
static struct shell_cmd * cmd_search(struct shell_cmd *tab,
									 struct shell_cmd *tabend, char *arg);
	
/* shell commands */
/*ECOS_SHELL_CMD(str, func, usage, help_str);*/
ECOS_SHELL_CMD("help", cmd_help, "", "Displays Help information");
ECOS_SHELL_CMD("showThreads", cmd_showThreads, "", "Displays running threads information");
ECOS_SHELL_CMD("showIfs", cmd_showIfs, "", "Displays information about all network interfaces");
ECOS_SHELL_CMD("showMem", cmd_showMem, "", "displays memory info");
#ifndef LVL7_FIXUP /* alok */
ECOS_SHELL_CMD("ifconfig", ifconfigt, "", "configure interface");
#endif
/* ECOS_SHELL_CMD("nvram", cmd_nvram, "", "nvram utility function"); */
ECOS_SHELL_CMD("exit", cmd_exit, "", "exit shell");


cyg_handle_t cyg_shell_init()
{
	cyg_thread_create(SHELL_PRIORITY, cyg_shell_thread, (cyg_addrword_t) 0,
                      "Shell Thread", (void *) cyg_shell_stack,
                      CYGNUM_SHELL_STACK_SIZE, &cyg_shell_thread_hndl,
                      &cyg_shell_thread_obj);

     cyg_thread_resume(cyg_shell_thread_hndl);
     return cyg_shell_thread_hndl;
}

extern cyg_sem_t sem;

void cyg_shell_thread(cyg_addrword_t data)
{
	static char cmdBuf[CYG_CMD_BUF_SIZE];
	bool prompt = true;
	struct shell_cmd *cmd;
	char * cmdptr;
	int argc;
	char *argv[MAX_ARGV];
	int i;

#if 0	
	cyg_tick_count_t ticks;
 	
 	printf("shell waiting...\n");
  	cyg_semaphore_timed_wait(&sem, cyg_current_time()+10);
  	ticks = cyg_current_time();
  	printf("shell current time = 0x%x\n", ticks);
#endif  	
  	
	memset (cmdBuf, '\0', sizeof(cmdBuf));
	sprintf (cmdBuf, "\n %s \r\n", CYG_SHELL_WELCOME_MSG);
	cyg_console_write_buf(cmdBuf, strlen(cmdBuf));
	while (true)
	{

		if (prompt == true) {
			memset (cmdBuf, '\0', sizeof(cmdBuf));		
			sprintf(cmdBuf, "%s ",CYG_SHELL_PROMPT);
			cyg_console_write_buf(cmdBuf, strlen(cmdBuf));			
			prompt = false;
		}

		memset (cmdBuf, '\0', sizeof(cmdBuf));
		
		for (i = 0; i < MAX_ARGV; i++)
			 argv[i] = 0;
			 
		if (cyg_shell_cmd_get(cmdBuf, sizeof(cmdBuf)))
		{
			
			if (strlen(cmdBuf) > 0)
			{
				cmdptr = (char *)&cmdBuf;
				if ((cmd = cmd_parse(&cmdptr, &argc, &argv[0]))
					!= (struct shell_cmd*)0)
					(cmd->func)(argc, argv);
				else
					printf("**Error: Illegal command:\"%s\"\n", argv[0]);
			}
			prompt = true;
		}
		
		
		
	}
}

static bool cyg_shell_cmd_get(char* cmd, cyg_uint32 size)
{
	char c;
	bool ret = false;
	static char cmd_cache[CYG_MAX_CMDS][CYG_CMD_BUF_SIZE];
	static int lst_cmd_idx = -1;
	static int ttl_cmd_idx = -1;
	char *curidx, *endidx, *tmpidx;
	int cur_cmd_idx = lst_cmd_idx;
	
    curidx = endidx = cmd;
	
	while (true)
	{
		if (cyg_console_read_char(&c) != true)
			continue;

		*endidx = '\0';
		
		switch (c)
		{
		case '\n':
		case '\r':
			cyg_console_write_char('\r');
			cyg_console_write_char('\n');

			if (curidx != cmd)
			{
				if (++lst_cmd_idx == CYG_MAX_CMDS)
					lst_cmd_idx = 0;
				if (lst_cmd_idx > ttl_cmd_idx)
					ttl_cmd_idx = lst_cmd_idx;
				
				strcpy(cmd_cache[lst_cmd_idx], cmd);
			}
			ret = true;
			break;
			
		case '\b':
		case 0x7F: /* delete */
			if (curidx != cmd) {
				if (curidx != endidx)
				{
					curidx--;
					cyg_console_write_char('\b');
					tmpidx = curidx;
					while (tmpidx != endidx -1)
					{
						*tmpidx = *(tmpidx + 1);
						cyg_console_write_char(*tmpidx++);
					}
					cyg_console_write_char(' ');
					cyg_console_write_char('\b');
					while (tmpidx-- != curidx)
						cyg_console_write_char('\b');
					endidx--;
				} else {
					cyg_console_write_char('\b');
					cyg_console_write_char(' ');
					cyg_console_write_char('\b');
					curidx--; endidx--;
				}
			} 
			break;

		case 0x1b:
			cyg_console_read_char(&c);
			if (c == 0x5b)
			{
				cyg_console_read_char(&c);

				switch (c)
				{
				case 'A':
					if (cur_cmd_idx >= 0)
					{
						while(curidx != cmd)
						{
							cyg_console_write_char('\b');
							cyg_console_write_char(' ');
							cyg_console_write_char('\b');
							curidx--;
						}
						strcpy(cmd, cmd_cache[cur_cmd_idx]);
						while (*curidx)
							cyg_console_write_char(*curidx++);
						endidx = curidx;
						if (--cur_cmd_idx < 0)
							cur_cmd_idx = ttl_cmd_idx;
					}
					break;
					
				case 'B':
					if (cur_cmd_idx >= 0)
					{
						if (++cur_cmd_idx > ttl_cmd_idx)
							cur_cmd_idx = 0;
						while(curidx != cmd)
						{
							cyg_console_write_char('\b');
							cyg_console_write_char(' ');
							cyg_console_write_char('\b');
							curidx--;
						}
						strcpy(cmd, cmd_cache[cur_cmd_idx]);
						while (*curidx)
							cyg_console_write_char(*curidx++);
						endidx = curidx;						
					}
					break;

				case 'C':
					if (curidx != endidx)
						cyg_console_write_char(*curidx++);
					break;										
					
				case 'D':
					if (curidx != cmd)
					{
						cyg_console_write_char('\b');
						curidx--;
					}					
					break;					
				}
			}
			break;	
			

		default:

			if (curidx != endidx)
			{
				tmpidx = endidx; 				
				*++endidx = '\0';
				while (tmpidx !=  curidx)
				{
					*tmpidx = *(tmpidx -1);
					--tmpidx;
				}
			}
			
			cyg_console_write_char(c);

			if (curidx != endidx)
			{
				*curidx++ = c;
				tmpidx = curidx;
				while (tmpidx != endidx)
					cyg_console_write_char(*tmpidx++);
				while (tmpidx-- != curidx)
					cyg_console_write_char('\b');					
			} else {
				*curidx = c;
				curidx++ ; endidx++;
			}
			break;
			
		}
		if (curidx == cmd + size -1)
		{
			*curidx = '\0';
			ret == true;
		}
		if (ret == true)
			return ret;

	}
	return ret;
}

static bool cyg_console_read_char(char* c)
{
	bool ret = false;
#ifdef __mips__
	hal_virtual_comm_table_t* chan = CYGACC_CALL_IF_DEBUG_PROCS();

	if (chan)
	{
		
		CYGACC_COMM_IF_CONTROL(*chan, __COMMCTL_SET_TIMEOUT, 0);
		ret = CYGACC_COMM_IF_GETC_TIMEOUT(*chan, c);
		
		/*	*c = CYGACC_COMM_IF_GETC(*chan);*/
	}
	else
	{
		chan = CYGACC_CALL_IF_DEBUG_PROCS();
		*c = CYGACC_COMM_IF_GETC(*chan);
	}
#endif
	return ret;
}

static void cyg_console_write_char(char c)
{
#ifdef __mips__
    hal_virtual_comm_table_t* chan = CYGACC_CALL_IF_CONSOLE_PROCS();

	if (chan)
		CYGACC_COMM_IF_PUTC((void*)chan, c);
	else {
		chan = CYGACC_CALL_IF_DEBUG_PROCS();
		CYGACC_COMM_IF_PUTC((void*)chan, c);
	}
#endif
}

static void cyg_console_write_buf(char* buf, int buflen)
{
	int i;
	for (i = 0; i< buflen; i++)
		cyg_console_write_char(buf[i]);
}


static struct shell_cmd *cmd_parse(char **line, int *argc, char **argv)
{
    char *cp = *line;
    char *pp;
    int indx = 0;

    while (*cp) {
        // Skip leading spaces
        while (*cp && *cp == ' ') cp++;
        if (!*cp) {
            break;  // Line ended with a string of spaces
        }

        if (indx < MAX_ARGV) {
            argv[indx++] = cp;
        } else {
            printf("Too many arguments - stopped at: '%s'\n", cp);
        }
        while (*cp) {
            if (*cp == ' ') {
                *cp++ = '\0';
                break;
            } else if (*cp == ';') {
                break;
	    } else if (*cp == '"') {
                // Swallow quote, scan till following one
                if (argv[indx-1] == cp) {
                    argv[indx-1] = ++cp;
                }
                pp = cp;
                while (*cp && *cp != '"') {
                    if (*cp == '\\') {
                        // Skip over escape - allows for escaped '"'
                        cp++;
                    }
                    // Move string to swallow escapes
                    *pp++ = *cp++;
                }
                if (!*cp) {
                    printf("Unbalanced string!\n");
                } else {
                    if (pp != cp) *pp = '\0';
                    *cp++ = '\0';
                    break;
                }
            } else {
                cp++;
            }
        }
    }
	*line = cp;

    *argc = indx;
    return cmd_search((struct shell_cmd *)__Shell_CMD_TBL__,
    				 (struct shell_cmd *)&__Shell_CMD_TBL_END__, argv[0]);
}

static struct shell_cmd *
cmd_search(struct shell_cmd *tab, struct shell_cmd *tabend, char *arg)
{
    int cmd_len;
    struct shell_cmd *cmd, *cmd2;
    // Search command table
    cmd_len = strlen(arg);
    cmd = tab;
    while (cmd != tabend) {
        if (strncasecmp(arg, cmd->name, cmd_len) == 0) {
            if (strlen(cmd->name) > cmd_len) {
                // Check for ambiguous commands here
                // Note: If there are commands which are not length-unique
                // then this check will be invalid.  E.g. "du" and "dump"
                bool first = true;
                cmd2 = tab;
                while (cmd2 != tabend) {
                    if ((cmd != cmd2) && 
                        (strncasecmp(arg, cmd2->name, cmd_len) == 0)) {
                        if (first) {
                            printf("Ambiguous command '%s', choices are: %s", 
                                        arg, cmd->name);
                            first = false;
                        }
                        printf(" %s", cmd2->name);
                    }
                    cmd2++;
                }
                if (!first) {
                    // At least one ambiguity found - fail the lookup
                    printf("\n");
                    return (struct shell_cmd *)0;
                }
            }
            return cmd;
        }
        cmd++;
    }
    return (struct shell_cmd *)0;
}


void cmd_help(int argc, char *argv[])
{
	struct shell_cmd *cmds, *cmdend;

	cmds = (struct shell_cmd *)__Shell_CMD_TBL__;
	cmdend = (struct shell_cmd *)&__Shell_CMD_TBL_END__;

	while (cmds != cmdend)
	{
		printf ("%s\t\t  %s\n\r", cmds->name, cmds->helpstr);
		cmds++;
	}
}


void cmd_showThreads(int argc, char *argv[])
{
    cyg_handle_t thread = 0;
    cyg_uint16 id = 0;

    while( cyg_thread_get_next( &thread, &id ) )
    {
        cyg_thread_info info;

        if( !cyg_thread_get_info( thread, id, &info ) )
            break;

        printf("ID: %04x name: %10s pri: %d state: %d\n",
                info.id, info.name?info.name:"----", info.set_pri, info.state );
    }
}

extern void _show_all_interfaces(void);

void cmd_showIfs(int argc, char *argv[])
{
	
	//_show_all_interfaces();	
}


void cmd_showMem(int argc, char *argv[])
{
        struct mallinfo meminfo;
        meminfo = mallinfo();

        printf("Memory Information:  Total %d  Free %d  Max %d\n", meminfo.arena
, meminfo.fordblks, meminfo.maxfree);
}
#if 0
void cmd_nvram(int argc, char *argv[])
{
	
        char *name, *value, *buf;
        int size;

        /* Skip program name */
        --argc;
        ++argv;

        if (!*argv)
        {
            printf("usage: nvram [get name] [set name=value] [unset name] [show]\n");
            return;
        }

        buf = (char*)malloc(NVRAM_SPACE);
        memset (buf, '\0', NVRAM_SPACE);
        /* Process the remaining arguments. */
        for (; *argv; argv++) {
                if (!strncmp(*argv, "get", 3)) {
                        if (*++argv) {
                        	#if 0
                                if ((value = nvram_get(*argv)))
                                       printf("%s\n",value);
                            #endif
                        }
                }
                else if (!strncmp(*argv, "set", 3)) {
                        if (*++argv) {
                                strncpy(buf, *argv, strlen(*argv));
                                name = strtok(buf, "=");
                                value = strtok (NULL, "=");
                                if (value == NULL)
                                {
                                printf ("Value to be set can not be null\n");
                                free(buf);
                                return;
                                }
                                //nvram_set(name, value);
                        }
                }
                else if (!strncmp(*argv, "unset", 5)) {
                        if (*++argv) {
                             //   nvram_unset(*argv);
                            }
                }
                else if (!strncmp(*argv, "commit", 5)) {
                        //nvram_commit();
			//wlconf_down("eth1");
			//wlconf("eth1");
                }
                else if (!strncmp(*argv, "show", 4) ||
                           !strncmp(*argv, "getall", 6)) {
                        //nvram_getall(buf,NVRAM_SPACE);
                        for (name = buf; *name; name += strlen(name) + 1)
                                printf("%s\n",name);
                        size = sizeof(struct nvram_header) + (int) name - (int) buf;
                        printf("size: %d bytes (%d left)\n", size, NVRAM_SPACE - size);
                }
                if (!*argv)
                        break;
        }
        free(buf);
        
        return;
}
#endif

void cmd_exit(int argc, char *argv[])
{
	cyg_thread_exit();
}




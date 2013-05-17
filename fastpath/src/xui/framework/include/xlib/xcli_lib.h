
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xcli_lib.c
*
* @purpose
*
* @component CLI Tree Parser
*
* @comments
*
* @create 13-Nov-2007
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef XCLI_LIB_H
#define XCLI_LIB_H

#define XCLI_MAX_ACCESS_LEVEL 255
#define XCLI_MIN_ACCESS_LEVEL 0
#define XCLI_MAX_READ_ACCESS_LEVEL 127

#define XLIB_COND_LESSER 0
#define XLIB_COND_EQUAL 1
#define XLIB_COND_GREATER 2

typedef enum
{
  XCLI_FALSE,
  XCLI_TRUE
} xcli_bool_t;


typedef struct
{
 xcli_bool_t (*cli_node_level_check)(unsigned  user_level,unsigned  level,char condition);
}xcli_node_level_check_t;

typedef  xcli_bool_t (*XCLI_LEVEL_FUNC_t)(unsigned  user_level,unsigned  level,char condition);

xcli_bool_t xcli_node_level_func_register(XCLI_LEVEL_FUNC_t func);
xcli_bool_t xcli_check_node_level(unsigned  user_level,unsigned  level,char condition);

typedef void xcli_printf_t (void *, const char *);
typedef char *xcli_prompt_t (void *, const char *);
typedef const char *xcli_action_t (void *arg, void *cntx, int argc, const char **argv, int indx);
typedef int xcli_decode_t (void *arg, void *cntx, const char *, xcli_bool_t exe);

int xcli_lib_init(void);
int xcli_session_init (void **state, void **menu, char *prompt, xcli_printf_t * pf, void *pfa,unsigned  level);
int xcli_finish (void *state, void *menu);
void xcli_prompt (void *state);
void xcli_set_prompt (void *state, char *prompt);
void *xcli_add_node (void *parent_in, const char *cmd, const char *help, xcli_decode_t * decode,
    xcli_action_t * action, void *arg, unsigned  lvl,char  cond);
void *xcli_add_node_withopt (void *parent_in, const char *cmd, const char *help, xcli_decode_t * decode,
    xcli_action_t * action, void *arg, char lvl, int opt);
void xcli_set_node_flags(void *arg, int flags);
int xcli_get_node_flags(void *arg);

void xcli_set_node_func(void *arg, xcli_action_t * action, void *arg2);
void xcli_set_mode (void *nodeArg, char *prompt, xcli_prompt_t pf, void *pfa);
void xcli_input (void *state, void **menuContx, char *datap, int *bytesp);

const char *xcli_print_action (void *cntx, int argc, const char **argv, int indx);

void xcli_error (void *context, const char *str);
void xcli_putcr (void *context, int count);
void xcli_puts (void *context, const char *str);
void *xcli_filters_get(void *context);
void *xcli_appwa_get(void *context, int size);

void xcli_change_mode (void *cntx, const char *name, int argc, const char **argv, int indx);

void *xcli_mode_get_root (const char *name);
void *xcli_mode_add (const char *name, const char *prompt, int def);
void xcli_mode_set_flags(const char *name, int flags);

void xcli_delete_unused (void *cntx);
void xcli_print_tree (void *cntx);

#define XCLI_MODE_FLAG_NONE     0x00
#define XCLI_MODE_FLAG_RAW      0x01
#define XCLI_MODE_FLAG_SECRET   0x02
#define XCLI_MODE_FLAG_DEFAULT  0x04

#define XCLI_NODE_FLAG_NONE     0x00
#define XCLI_NODE_FLAG_ARG      0x01
#define XCLI_NODE_FLAG_LINE     0x20

typedef enum
{
  XCLI_TOK_CASE_LOWER,
  XCLI_TOK_CASE_UPPER,
  XCLI_TOK_CASE_ORIGINAL
}xcli_token_case_t;

void xcli_token_case_set(xcli_token_case_t newVal);

#endif

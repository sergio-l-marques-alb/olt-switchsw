/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename          xxmon.h
 *
 * @purpose           A simple monitor
 *
 * @component         Routing Utils Component
 *
 * @comments
 *
 * @create            10/01/1998 
 *
 * @author            Igor Ternovsky
 *
 * @end
 *
 * ********************************************************************/
#ifndef xxmon_h

#define xxmon_h 1

#include "xxmon.ext"

/* User interface functions */

#define MAX_NAME_LEN       30
#define MAX_FORMAT_LEN     10
#define MAX_HELP_LEN       40
#define MAX_PARM_LEN       20
#define MAX_NUM_OF_PARMS   15
#define MAX_BUF_LENGTH     256

#define TOKEN_UP           ".."
#define TOKEN_ROOT         "/"
#define TOKEN_BREAK        "#"
#define TOKEN_HELP         "?"
#define TOKEN_DIRHELP      "help"

typedef enum { Dir, Token } t_MonEntrySelector;

/* Parameter name and format */
typedef char (T_ParmName)[MAX_NAME_LEN+1];
typedef char (T_ParmFormat)[MAX_FORMAT_LEN+1];

/* Monitor token structure */
typedef struct t_MonEntry
{
   struct t_MonEntry   *next;
   char                 name[MAX_NAME_LEN+1];   /* Entry name */
   char                 help[MAX_HELP_LEN+1];   /* Help string */
   char                 *alias;                 /* Alias */
   word                 alias_len;              /* Alias length */
   t_MonEntrySelector   sel;                    /* Entry selector */

   union {
      struct 
      {
         struct t_MonEntry   *upper;            /* Upper directory */
         struct t_MonEntry   *first;            /* First entry */
      } dir;
      struct t_Token 
      {
         t_TokenFunc    f_tokenFunc;            /* Token handler */
         word           nParms;                 /* Number of parameters */
         t_MonParm      parm[MAX_NUM_OF_PARMS]; /* Parameters array */
         T_ParmName     name[MAX_NUM_OF_PARMS]; /* Parameter name */
         T_ParmFormat   format[MAX_NUM_OF_PARMS];/* Parameter format */
      } token;
   } u;
} t_MonEntry;

/* Token types */
typedef enum 
{ 
   EmptyToken, 
   UpToken, 
   RootToken, 
   BreakToken,
   HelpToken,
   DirHelpToken,
   NameToken 
} t_TokenType;


#endif /* #ifndef xxmon_h */

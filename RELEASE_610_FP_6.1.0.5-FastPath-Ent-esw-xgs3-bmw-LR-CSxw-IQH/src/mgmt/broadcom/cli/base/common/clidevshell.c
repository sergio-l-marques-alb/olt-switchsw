/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/clidevshell.c
*
* @purpose DevShell code
*
* @component user interface
*
* @comments none
*
* @create   06/?/2000 
*
* @author  Colin Verne 
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "osapi.h"

#ifdef _L7_OS_VXWORKS_
#include "vxWorks.h"
#include "symbol.h"
#include "ioLib.h"
#include "taskLib.h"
#endif /* _L7_OS_VXWORKS_ */

#ifdef _L7_OS_LINUX_
typedef struct
{
  char * name;
   FUNCPTR value;
} SYMBOL;  /* need tools to build symbol table */
typedef unsigned long ULONG;
#endif /* _L7_OS_LINUX_ */

extern L7_int32 osapiDevshell(L7_char8 * name, L7_int32 * rc,
                              L7_int32 arg0,  L7_int32 arg1,  L7_int32 arg2,
                              L7_int32 arg3,  L7_int32 arg4,  L7_int32 arg5,
                              L7_int32 arg6,  L7_int32 arg7,  L7_int32 arg8,
                              L7_int32 arg9,  L7_int32 arg10, L7_int32 arg11,
                              L7_int32 arg12, L7_int32 arg13, L7_int32 arg14,
                              L7_int32 arg15, L7_int32 arg16, L7_int32 arg17,
                              L7_int32 arg18, L7_int32 arg19, L7_int32 arg20,
                              L7_int32 arg21, L7_int32 arg22, L7_int32 arg23,
                              L7_int32 arg24
			      );

/* #include "utils.h" */

#define CMD_MAX_SIZE    256
#define CMD_MAX_ARGS     25 

L7_char8 shell_strings[CMD_MAX_ARGS][CMD_MAX_SIZE];

L7_int32 cliDevShell(L7_char8 * cmd)
{
   L7_char8 func_name[CMD_MAX_SIZE];
   FUNCPTR func_ptr;
   L7_int32 args[CMD_MAX_ARGS];
   L7_int32 rc;

   func_ptr=NULL;

   cliParseCmd(cmd,func_name,args);

   if (func_name[0])
   {
    func_ptr = (FUNCPTR)(osapiAddressLookup(func_name));
      printf("\n");
      if (func_ptr)
      {
         rc=func_ptr(args[0],args[1],args[2],args[3],args[4],
                     args[5],args[6],args[7],args[8],args[9],
                     args[10],args[11],args[12],args[13],args[14],
                     args[15],args[16],args[17],args[18],args[19],
                     args[20],args[21],args[22],args[23],args[24]
		     );

         printf("\nvalue = %d = 0x%x\n",rc,rc);
      }
      else if ((osapiDevshell(func_name, &rc,
                              args[0],args[1],args[2],args[3],args[4],
                              args[5],args[6],args[7],args[8],args[9],
                              args[10],args[11],args[12],args[13],args[14],
                              args[15],args[16],args[17],args[18],args[19],
                              args[20],args[21],args[22],args[23],args[24])) == 0)
      {
         printf("\nvalue = %d = 0x%x\n",rc,rc);
      }
      else
    {
         printf("Undefined symbol: %s\n",func_name);
   }
  }
   else if (cmd[0])
   {
      printf("\nParse error.\n");
   }
   return 0;
}

L7_int32 cliParseCmd(L7_char8 * cmd,L7_char8 * func_name,L7_int32 * args)
{
   L7_int32 i;
   L7_int32 j;
   L7_int32 k;      
   L7_int32 string_start,string_end;
   L7_int32 end_of_cmd=0;

  memset (func_name, 0,CMD_MAX_SIZE);
  memset ((L7_char8 *)args, 0,CMD_MAX_ARGS*sizeof(L7_int32));
  memset ((L7_char8 *)shell_strings, 0,CMD_MAX_ARGS*CMD_MAX_SIZE);
   j=0;
   k=0;
   string_start=0;
   string_end=0;

  for (i=0; i<CMD_MAX_SIZE-1; i++)
   {
      if (cmd[i]==' ' || cmd[i]==0 || cmd[i]==',' || cmd[i]==')')
      {
         if (cmd[i]==0)
      {
            end_of_cmd=1;
      }
         if (!string_start || string_end)
      {
        cmd[i]=0;
      }
         switch (j)
         {
         case 0:
        osapiStrncpySafe(func_name,cmd,CMD_MAX_SIZE);
            j++;
            break;
         default:
            if (string_start && string_end)
            {
               if (j >= 1)
          {
            osapiStrncpy(shell_strings[j-1],&cmd[string_start+1],min((string_end-string_start-1),(sizeof(shell_strings[j-1])-1)));
          }
               string_start=0;
               string_end=0;
               if (j >= 1)
               {
                  args[j-1]=(L7_int32)shell_strings[j-1];
                  j++;
               }
            }
            else if (k!=i && !string_start)
            {
               if (j >= 1)
               {

                  if ((cmd[k] == '0') && ((cmd[k+1] == 'x') || (cmd[k+1] == 'X')))
                  {
              args[j-1]=strtoul(&cmd[k],(L7_char8 * *)NULL,16);
                  }
                  else
                  {
                     args[j-1]=atoi(&cmd[k]);
                  }
                  j++;
               }
            }
         }
         if (end_of_cmd)
      {
        return 0;
      }
         k=i+1;
      }
      else if (cmd[i]==0x27)
      {                                                                     /*double quote*/
         if (string_start)
      {
            string_end=i;
      }
      else
      {
            string_start=i;
      }
    }
      else if (cmd[i]=='(' && !string_start)
      {
         cmd[i]=0;
         if (j==0)
         {
        osapiStrncpySafe(func_name,cmd,CMD_MAX_SIZE);
            j++;
         }
         k=i+1;
      }
   }
   return 0;
}

/*********************************************************************
*
* @purpose The purpose of this function is to add a lookup capability 
* 	   to the Fastpath command prompt to search through all the
* 	   available functions 
*          
* @param char *func_name - the name of the function (case sensitive)
*
* @returns 0 always
*
* @notes there are 4 ways to use this:
* 		devshell "lkup '*function_name'" - ends with function_name
* 		devshell "lkup 'function_name*'" - starts with function_name
* 		devshell "lkup 'function_name'"  - contains function_name
* 		devshell "lkup '*function_name*'"- contains function_name
*
* 		this is case sensitive
* 		do not use spaces
*        
* @end
*********************************************************************/

/* VxWorks already has an implementation of lkup */
#ifdef _L7_OS_LINUX_
extern void osapiLkup(L7_char8 *search_str);
int lkup(char * func_name)
{
  osapiLkup(func_name);
  return 0;
}
#endif

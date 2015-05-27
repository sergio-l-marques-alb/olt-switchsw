/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clidevshell.h
 *
 * @purpose DevShell code header
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   08/09/2000
 *
 * @author  Forrest Samuels
 * @end
 *
 **********************************************************************/

#ifndef CLIDEVSHELL_H
#define CLIDEVSHELL_H

L7_int32 cliParseCmd(L7_char8 * cmd,L7_char8 * func_name,L7_int32 * args,L7_BOOL * arg_strings);
L7_int32 cliLookupCmd(L7_char8 * func_name, L7_FUNCPTR * func_ptr);
L7_int32 cliDevShell(L7_char8 * cmd);

#endif

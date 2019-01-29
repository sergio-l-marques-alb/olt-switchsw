/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename clicommands_llpf.h
*
* @purpose header for llpf commands
*
* @component user interface
*
* @comments none
*
* @create  11/02/2009
*
* @author  Vijayanand K (kvijayan)
* @end
*
***********************************************************************/
#ifndef CLICOMMANDS_LLPF
#define CLICOMMANDS_LLPF

/*****************************************************************************
*
* @purpose build the tree for 'show llpf *'
* @param  depth2 @b{(input)} EwsCliCommandP
*
* @returntype  void
*
* @notes none
*
* @end
******************************************************************************/
void buildTreePrivLlpf(EwsCliCommandP depth2);

/******************************************************************************
*
* @purpose build the tree for configure isdp per port
* @param depth2 @b{(input)} EwsCliCommandP
*
* @returntype  void
* @notes none
*
* @end
******************************************************************************/
void buildTreeInterfaceConfigLlpf(EwsCliCommandP depth1);

/******************************************************************************
*
* @purpose  Enables/Disables llpf on a port.
*
*
* @param  ewsContext    @b{(input)} The current Context
* @param  argc          @b{(input)} argc
* @param  **argv        @b{(input)} argv
* @param index          @b{(input)} index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] llpf enable
*
* @cmdhelp Enables/Disables llpf on a port..
*
* @end
*
******************************************************************************/
const L7_char8 *commandIntfLlpfEnable(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 **argv,
                                 L7_uint32 index);

/******************************************************************************
*
* @purpose  Displays LLPF mode per interface.
* @param  ewsContext    @b{(input)} The current Context
* @param  argc          @b{(input)} argc
* @param  **argv        @b{(input)} argv
* @param index          @b{(input)} index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  show llpf interface all|([unit/]slot/port <cr>)
*
* @cmdhelp Displays LLPF block protocol mode(s) per interface.
*
* @end
*
******************************************************************************/
const L7_char8 * commandShowLlpfInterface(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 * * argv,
                                          L7_uint32 index);
#endif /* CLICOMMANDS_ISDP */

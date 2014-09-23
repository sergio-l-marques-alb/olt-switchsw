/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_tacacs.h
 *
 * @purpose header for TACACS commands in clicommands_tacacs.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/03/2005
 *
 * @authors  Rama Krishna Hazari, Navin Kumar Rungta
 * @end
 *
 ***********************************************************************/

#ifndef CLICOMMANDS_TACACS_H
#define CLICOMMANDS_TACACS_H

/*****************************************************************/

/* TACACS HELP STRINGS TO BE DISPLAYED TO THE USER */

/* Error strings */

#define CLISYNTAX_TACACS_HOST_HELP          "\r\nError! Use \"tacacs-server host <ip-address|hostname>\"\
 to configure the TACACS server."

#define CLISYNTAX_TACACS_PORT_HELP          "\r\nError! Use \"port <port-number>\"\
 to specify a server port number."

#define CLISYNTAX_TACACS_TIMEOUT_HELP       "\r\nError! Use \"timeout <timeout>\"\
to set the connection timeout."

#define CLISYNTAX_TACACS_KEY_HELP           "\r\nError! Use \"key <key-string>\" \
to specify an encryption key."

#define CLISYNTAX_TACACS_GLOBAL_KEY_RANGE_HELP pStringAddCR(pStrErr_base_TacacsKeyRange, 1, 0, 0, 0, pStrErr_common_Error)

#define CLISYNTAX_TACACS_SHOW_HELP          "\r\nError! Use \"show tacacs [ip-address|hostname]\"\
 to display the TACACS+ configuration."

#define CLISYNTAX_TACACS_GLOBAL_KEY_HELP    "\r\nError! Use \"tacacs-server key <key-string>\"\
 to set the global key."

#define CLISYNTAX_TACACS_GLOBAL_TIMEOUT_HELP "\r\nError! Use \"timeout <timeout>\"\
to set the connection timeout."

#define CLIERROR_TACACS_GLOBAL_KEY_HELP     pStringAddCR(pStrErr_base_TacacsKey, 1, 0, 0, 0, pStrErr_common_Error)
#define CLIERROR_TACACS_GLOBAL_TIMEOUT_HELP pStringAddCR(pStrErr_base_TacacsTimeout_1, 1, 0, 0, 0, pStrErr_common_Error)

/* end */

/* definitions of normal display strings in show commands. */
#define CLITACACS_SHOW_HEADING              \
  pStringAddCR(pStrInfo_base_IpAddrPortTimeoutPri, 1, 0, 0, 0, L7_NULLPTR)

#define CLITACACS_SHOW_HEADING_LINE         \
  "\r\n------------------------   -----   -------   --------"

/*****************************************************************/

#define CLITACACS_HOST_IP_LEN         16
#define CLITACACS_DEFAULT_PORT_NUM    FD_TACACS_PLUS_PORT_NUM
#define CLITACACS_PORT_RANGE_MIN      0
#define CLITACACS_PORT_RANGE_MAX      65535
#define CLITACACS_TIMEOUT_RANGE_MIN   1
#define CLITACACS_TIMEOUT_RANGE_MAX   30
#define CLITACACS_KEY_STRING_LEN      128

#define CLITACACS_PRIORITY_RANGE_MIN  0
#define CLITACACS_PRIORITY_RANGE_MAX  65535
#define CLITACACS_DEFAULT_TIMEOUT_VAL FD_TACACS_PLUS_TIMEOUT_VALUE

/* BEGIN TACACS TREE COMMAND STRUCTURE */
extern void buildTreeGlobalConfigTacacs(EwsCliCommandP depth1);
extern void buildTreeTacacsConfig(EwsCliCommandP depth1);
extern void buildTreeTacacsShow(EwsCliCommandP depth1);

/* BEGIN TACACS ACTION COMMANDS DECL */
extern const L7_char8 *cliTacacsMode(EwsContext ewsContext,
                                     L7_uint32 argc,
                                     const L7_char8 * * argv,
                                     L7_uint32 index);
extern const L7_char8 *cliTacacsPrompt(L7_uint32 depth,
                                       L7_char8 * tmpPrompt,
                                       L7_char8 * tmpUpPrompt,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       EwsContext ewsContext,
                                       L7_uint32 index);
extern const L7_char8 *commandTACACSGblKey(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index);
extern const L7_char8 *commandTACACSGblKeyEncrypted(EwsContext ewsContext,
                                                    L7_uint32 argc,
                                                    const L7_char8 * * argv,
                                                    L7_uint32 index);
extern const L7_char8 *commandTACACSGblTimeout(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 * * argv,
                                               L7_uint32 index);
extern const L7_char8 *commandTACACSSingleConnection(EwsContext ewsContext,
                                                     L7_uint32 argc,
                                                     const L7_char8 * * argv,
                                                     L7_uint32 index);
extern const L7_char8 *commandTACACSPort(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 * * argv,
                                         L7_uint32 index);
extern const L7_char8 *commandTACACSTimeout(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 * * argv,
                                            L7_uint32 index);
extern const L7_char8 *commandTACACSKey(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index);
const L7_char8 *commandTACACSKeyEncrypted(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 * * argv,
                                          L7_uint32 index);
extern const L7_char8 *commandTACACSPriority(EwsContext ewsContext,
                                             L7_uint32 argc,
                                             const L7_char8 * * argv,
                                             L7_uint32 index);
extern const L7_char8 *commandTACACSShow(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 * * argv,
                                         L7_uint32 index);
extern L7_char8 * ipAddressGet(EwsContext ewsContext,
                               L7_uint32 unit,
                               L7_char8 * hostIp);

extern L7_char8 * serverInfoGet(EwsContext ewsContext, 
                  L7_IP_ADDRESS_TYPE_t type,
                  L7_uchar8 *serverAddress, L7_uint32 *count);

/* END TACACS ACTION COMMANDS DECL */

/* declarations needs to be changed */

#endif

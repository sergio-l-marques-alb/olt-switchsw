/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_lacp.h
 *
 * @purpose header for LACP commands in clicommands_lacp.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  10/10/2006
 *
 * @author  Sunil Babu
 * @end
 *
 ***********************************************************************/

#ifndef CLICOMMANDS_LACP_H
#define CLICOMMANDS_LACP_H

/*****************************************************************/

#define DOT3AD_STATE_AGGREGATION      0x4
#define DOT3AD_STATE_LACP_TIMEOUT     0x2
#define DOT3AD_STATE_LACP_ACTIVITY    0x1

/*****************************************************************/

/* BEGIN LACP TREE COMMAND STRUCTURE */
extern void buildTreeInterfaceConfigLacp(EwsCliCommandP depth1);
extern void buildTreeShowConfigLacp(EwsCliCommandP depth1);
extern void buildTreeInterfaceLAG(EwsCliCommandP depth1);
extern void buildTreePrivSwDevShowPortChannel(EwsCliCommandP depth2);
extern void buildTreeInterfaceSwDevInterfacePortChannel(EwsCliCommandP depth1);
extern void buildTreeGlobalSwDevPortChannel(EwsCliCommandP depth1);

/* BEGIN LACP ACTION COMMANDS DECL */

extern const L7_char8 *commandLACPCollectorMaxDelay(EwsContext ewsContext,
                                                    L7_uint32 argc,
                                                    const L7_char8 * * argv,
                                                    L7_uint32 index);

extern const L7_char8 *commandLACPActorSysPriority(EwsContext ewsContext,
                                                   L7_uint32 argc,
                                                   const L7_char8 * * argv,
                                                   L7_uint32 index);

extern const L7_char8 *commandLACPActorAdminKey(EwsContext ewsContext,
                                                L7_uint32 argc,
                                                const L7_char8 * * argv,
                                                L7_uint32 index);

extern const L7_char8 *commandLACPActorPortPriority(EwsContext ewsContext,
                                                    L7_uint32 argc,
                                                    const L7_char8 * * argv,
                                                    L7_uint32 index);

extern const L7_char8 *commandLACPPartnerSysPriority(EwsContext ewsContext,
                                                     L7_uint32 argc,
                                                     const L7_char8 * * argv,
                                                     L7_uint32 index);

extern const L7_char8 *commandLACPPartnerAdminKey(EwsContext ewsContext,
                                                  L7_uint32 argc,
                                                  const L7_char8 * * argv,
                                                  L7_uint32 index);
extern const L7_char8 *commandLACPPartnerSysID(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 * * argv,
                                               L7_uint32 index);

extern const L7_char8 *commandLACPPartnerPortID(EwsContext ewsContext,
                                                L7_uint32 argc,
                                                const L7_char8 * * argv,
                                                L7_uint32 index);

extern const L7_char8 *commandLACPPartnerPortPriority(EwsContext ewsContext,
                                                      L7_uint32 argc,
                                                      const L7_char8 * * argv,
                                                      L7_uint32 index);

extern const L7_char8 *commandLACPAdminKey(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index);

extern const L7_char8 *commandLACPActorAdminState(EwsContext ewsContext,
                                                  L7_uint32 argc,
                                                  const L7_char8 * * argv,
                                                  L7_uint32 index);

extern const L7_char8 *commandLACPPartnerAdminState(EwsContext ewsContext,
                                                    L7_uint32 argc,
                                                    const L7_char8 * * argv,
                                                    L7_uint32 index);

extern const L7_char8 *commandLACPActorShow(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 * * argv,
                                            L7_uint32 index);

extern const L7_char8 *commandLACPPartnerShow(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 * * argv,
                                              L7_uint32 index);



/* END LACP ACTION COMMANDS DECL */

#endif

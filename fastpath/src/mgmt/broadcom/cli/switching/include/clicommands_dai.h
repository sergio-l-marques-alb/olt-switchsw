/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/switching/include/clicommands_dai.h
*
* @purpose Dynamic ARP Inspection CLI commands header file
*
* @component Dynamic ARP Inspection
*
* @comments none
*
* @create  09/27/2007
*
* @author  Kiran Kumar Kella
*
* @end
*
*********************************************************************/

#ifndef CLICOMMANDS_DAI_H
#define CLICOMMANDS_DAI_H

#define CLI_MAX_SCROLL_LINES_DAI            (CLI_MAX_SCROLL_LINES-9)

/*********************************************************
**
** BEGIN DYNAMIC ARP INSPECTION FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/
void buildTreeGlobalDai(EwsCliCommandP depth4);
void buildTreeIfDai(EwsCliCommandP depth4);
void buildTreeShowDai(EwsCliCommandP depth5);
void buildTreeClearDai(EwsCliCommandP depth5);
void buildTreeArpAccessListConfig();
void buildTreeGlobalArpAccessList(EwsCliCommandP depth2);

/*********************************************************
**
** END CARD FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/

/********************************************************************
**
** BEGIN DYNAMIC ARP INSPECTION FUNCTIONS COMMANDS STRUCTURE - SHOW & CONFIG
**
*********************************************************************/

const L7_char8 *cliArpAccessListMode(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 **argv, L7_uint32 index);
const L7_char8 *cliArpAccessListPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                       L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                       const L7_char8 **argv, EwsContext ewsContext);

/* commands defined in cli_show_dai.c */
const char *commandShowDai(EwsContext ewsContext, L7_uint32 argc,
                           const L7_char8 **argv, L7_uint32 index);
const char *commandShowDaiVlan(EwsContext ewsContext, L7_uint32 argc,
                               const L7_char8 **argv, L7_uint32 index);
const char *commandShowDaiIf(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index);
const char *commandShowDaiStatsBrief(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 **argv, L7_uint32 index);
const char *commandShowDaiStatsVlan(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 **argv, L7_uint32 index);
const char *commandShowArpAcl(EwsContext ewsContext, L7_uint32 argc,
                              const L7_char8 **argv, L7_uint32 index);

/* cli_clear_dai.c */
const L7_char8 *commandClearDaiStats(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 **argv, L7_uint32 index);

/* commands defined in cli_config_dai.c */
const L7_char8 *commandDaiValidateOptions(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDaiVlan(EwsContext ewsContext, L7_uint32 argc,
                               const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDaiVlanArpAclFilter(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDaiIfTrust(EwsContext ewsContext, L7_uint32 argc,
                                  const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandDaiIfRateLimit(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandArpAclPermitRule(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 **argv, L7_uint32 index);

/********************************************************************
**
** END DYNAMIC ARP INSPECTION FUNCTIONS COMMANDS STRUCTURE - SHOW & CONFIG
**
*********************************************************************/

#endif /* CLICOMMANDS_DAI_H */

/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/voip/clicommands_voip.h
 *
 * @purpose header for clicommands_voip.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/23/2007
 *
 * @author  Amitabha Sen
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDSQOSVOIP_H
#define CLICOMMANDSQOSVOIP_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"

void buildTreeGlobalAutoVoIP(EwsCliCommandP depth1);
void buildTreeInterfaceAutoVoIP(EwsCliCommandP depth1);
void buildTreePrivShowAutoVoIP(EwsCliCommandP depth1);

const L7_char8 *commandGlobalAutoVoIP(EwsContext ewsContext, 
                                      L7_uint32 argc, 
                                      const L7_char8 **argv, 
                                      L7_uint32 index);

const L7_char8 *commandGlobalAutoVoIPMinBandwidth(EwsContext ewsContext, 
                                                  L7_uint32 argc, 
                                                  const L7_char8 **argv, 
                                                  L7_uint32 index);

const L7_char8 *commandInterfaceAutoVoIP( EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 **argv,
                                          L7_uint32 index);

#if 0
const L7_char8  *commandInterfaceAutoVoIPMinBandwidth(EwsContext ewsContext, 
                                                      L7_uint32 argc, 
                                                      const L7_char8 **argv, 
                                                      L7_uint32 index);
#endif

const L7_char8 *commandShowAutoVoIP(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 **argv,
                                    L7_uint32 index);

#endif

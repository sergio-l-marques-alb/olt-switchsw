/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/iscsi/clicommands_iscsi.h
 *
 * @purpose header for clicommands_iscsi.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/24/2008
 *
 * @author  Rajakrishna
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDSQOSISCSI_H
#define CLICOMMANDSQOSISCSI_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"

void buildTreeGlobalIscsi(EwsCliCommandP depth1);
void buildTreePrivShowIscsi(EwsCliCommandP depth2);

const L7_char8  *commandIscsiEnable( EwsContext ewsContext, 
                                        L7_uint32 argc, 
                                        const L7_char8 **argv, 
                                        L7_uint32 index);

const L7_char8  *commandIscsiTargetPort( EwsContext ewsContext, 
                                                    L7_uint32 argc, 
                                                    const L7_char8 **argv, 
                                                    L7_uint32 index);

const L7_char8 *commandIscsiCos( EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 **argv,
                                          L7_uint32 index);

const L7_char8  *commandIscsiAgingTime(EwsContext ewsContext, 
                                                      L7_uint32 argc, 
                                                      const L7_char8 **argv, 
                                                      L7_uint32 index);

const L7_char8 *commandShowIscsi(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 **argv,
                                    L7_uint32 index);

const L7_char8 *commandShowIscsiSessions(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 **argv,
                                    L7_uint32 index);

#endif

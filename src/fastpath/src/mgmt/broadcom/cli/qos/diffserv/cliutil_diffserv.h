/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/diffserv/cliutil_diffserv.h
 *
 * @purpose header for cliutil_diffserv.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   07/08/2002
 *
 * @author  Kathy McDowell
 * @end
 *
 **********************************************************************/

#ifndef CLIUTILDIFFSERV_H
#define CLIUTILDIFFSERV_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include <commdefs.h>
#include <datatypes.h>

void cliDisplayClassRules(EwsContext ewsContext, L7_uint32 unitIndex,
                          L7_uint32 classIndex);
void cliDisplayPolicyClassAttributes(EwsContext ewsContext, L7_uint32 unitIndex,
                                     L7_uint32 policyIndex, L7_uint32 policyInstIndex);
void cliDiffServClassRuleMatchExcludeFlagStringGet(L7_uint32 unitIndex,
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uchar8 * excludeFlagString,
                                                   L7_int32 excludeFlagStringLenMax);
L7_RC_t cliDiffservConvertL4PortValToString(L7_uint32 val, L7_char8 * dscpString, L7_int32 dscpStringLenMax);
void cliDiffservDisplayServiceStats(EwsContext ewsContext, L7_uint32 unitIndex,
                                    L7_uint32 intIfIndex, L7_uint32 ifDirection);
void cliDiffservBuildValidTypes(L7_char8 * validTypes,
                                L7_int32 validTypesLenMax,
                                L7_BOOL * allSupported,
                                L7_BOOL * anySupported,
                                L7_BOOL * aclSupported );
void cliDiffservBuildValidIntfTypes(L7_uint32 direction,
                                    L7_char8 * validTypes,
                                    L7_int32 validTypesLenMax,
                                    L7_BOOL * slotPortSupported,
                                    L7_BOOL * allSupported );
void cliDiffservBuildValidDirs(L7_char8 * dirOption,
                               L7_int32 dirOptionLenMax,
                               L7_char8 * leftEnclosure,
                               L7_char8 * rightEnclosure,
                               L7_BOOL * inSupported,
                               L7_BOOL * outSupported );

void cliConstructAndPrintPoliceSimpleSyntax(EwsContext ewsContext, L7_char8 * buf, L7_int32 bufLenMax);
void cliConstructAndPrintPoliceSingleRateSyntax(EwsContext ewsContext, L7_char8 * buf, L7_int32 bufLenMax);
void cliConstructAndPrintPoliceTwoRateSyntax(EwsContext ewsContext, L7_char8 * buf, L7_int32 bufLenMax);
void cliConstructPoliceParmsSyntax(L7_char8 * buf, L7_int32 bufLenMax);
#endif

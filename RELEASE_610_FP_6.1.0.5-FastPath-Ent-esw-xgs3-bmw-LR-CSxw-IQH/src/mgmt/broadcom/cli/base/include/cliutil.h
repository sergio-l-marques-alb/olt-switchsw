/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cliutil.h
 *
 * @purpose header for cliutil.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   06/08/2000
 *
 * @author  Forrest Samuels and Qun He
 * @end
 *
 **********************************************************************/

#ifndef CLIUTIL_H
#define CLIUTIL_H

#include "commdefs.h"
#include "datatypes.h"
#include "usmdb_rlim_api.h"

#define CLI_MAX_CHARACTERS_PER_ROW   24
#define CLI_MAX_LABLE_LENGTH         50
#define CLI_MAX_LINES_IN_PAGE        10

void cliConvertToLowerCase(L7_char8 * buf);
L7_RC_t cliConvertToUpperCase(L7_char8 * buf);
L7_RC_t cliCheckIfInteger(L7_char8 * buf);
const L7_char8 *cliDepth(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt,
    L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
L7_char8 *cliPrompt(EwsContext ewsContext);

L7_BOOL cliLogoutDeferredGet(EwsContext ewsContext);
L7_RC_t cliLogoutDeferredSet(EwsContext ewsContext, L7_BOOL defer);
void cliContextLogout(EwsContext ewsContext);

L7_BOOL cliLoginCheck(L7_char8 *enteredInfo, EwsContext context);
L7_RC_t cliReadBannerFile(L7_char8 * filename, L7_char8 * buffer);
void cliBannerDisplay(L7_char8 * buffer);
void cliReadDefaultBanner(L7_char8 * buffer, L7_uint32 bufferSize);

L7_uint32 cliNumberOfArguments(L7_uint32 argc, L7_uint32 index);
void cliWriteSerial(L7_char8 * string);
L7_BOOL cliSecurityLevelCheck(lvl7SecurityLevels_t level, L7_char8 * userName);
void cliExamine(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
void cliSyntaxTop(EwsContext ewsContext);
void cliSyntaxBottom(EwsContext ewsContext);
void cliSyntaxNewLine(EwsContext ewsContext);
void cliFormat(EwsContext ewsContext, const L7_char8 * description);
void ipv6CliFormat(EwsContext ewsContext, const L7_char8 * description);
void cliFormatShortText(EwsContext ewsContext, const L7_char8 * description);
void cliFormatShortStringDisplay(EwsContext ewsContext, L7_char8 * buf);
void cliFormatStringDisplay(EwsContext ewsContext, L7_char8 * buf);
void cliFormatStringDisplayGeneral(EwsContext ewsContext, L7_char8 * buf, L7_int32 descrLen);
L7_char8 *cliProcessStringDisplay(L7_char8 * buffer, L7_uint32 len);
void cliFormatExtended(EwsContext ewsContext, const L7_char8 * description, L7_uint32 format_ind, L7_uint32 format_num);
void cliPhaseTwoInitVars();
void cliPhaseThreeInitVars();
void cliClearCharInput();
void cliClearStringInput();
void cliBannerDisplay(L7_char8 * buffer);
void cliProcessCharInput(L7_char8 * datap);
L7_BOOL cliProcessStringInput(L7_char8 * datap);
L7_char8 *cliProcessStringOutput(L7_char8 * outputLine, L7_uint32 outputLength);
L7_char8 *cliCombineStringOutput(L7_char8 * outputLine1, L7_uint32 outputLength1,
    L7_char8 * outputLine2, L7_uint32 outputLength2);
L7_uint32 cliProcessScrollInput(EwsContext context, L7_char8 * datap, L7_uint32 * bytesp);
L7_char8 *cliGetStringInput();
L7_char8 cliGetCharInput();
L7_uint32 cliGetUnitId();
void cliSetCharInputID(L7_uint32 id, EwsContext ewsContext,const L7_char8 * * argv);
void cliSetStringInputID(L7_uint32 id, EwsContext ewsContext,const L7_char8 * * argv);
L7_uint32 cliGetCharInputID();
L7_uint32 cliGetStringInputID();
L7_BOOL cliGetStringPassword();
void cliSetStringPassword();
L7_BOOL cliCheckScrollInput();
void cliScrollBufferFlush();
void cliWrite(L7_char8 * string);
L7_BOOL cliEscapeSequence(L7_char8 * datap, L7_uint32 * bytesp, L7_BOOL logged_in);
L7_BOOL cliCheckAltInput();
void cliInitConnection(L7_uint32 handle);
L7_RC_t cliSlotPortToInterface(const L7_char8 * unit_slot_port, L7_uint32 * unit,
    L7_uint32 * slot, L7_uint32 * port, L7_uint32 * intIfNum);

L7_int32 cliFindSNMPCommunityIndex(L7_char8 * name);
L7_int32 cliFindEmptySNMPCommunityIndex();
L7_int32 cliFindSNMPTrapCommunityIndex(L7_char8 * name, L7_uint32 ipAddr);
L7_int32 cliFindSNMPTrapCommunityV6Index(L7_char8 * name, L7_uchar8 * v6Addr);
L7_int32 cliFindEmptySNMPTrapCommunityIndex();

EwsCliCommandP cliAddBlankNode(EwsCliCommandP nodePointer);

L7_BOOL cliConvertMac(L7_uchar8 * buf, L7_uchar8 * mac);
L7_BOOL cliWebConvertHexMacToString(L7_uchar8 * mac, L7_uchar8 * macBuf);
L7_BOOL cliConvertVlanMac(L7_uchar8 * buf, L7_uchar8 * mac);
L7_BOOL cliConvertEtypeCustomValue(L7_uchar8 * buf, L7_uint32 * val);
L7_RC_t cliIsAlphaNum(L7_char8 * input);
L7_RC_t cliIsLocation(L7_char8 * input);
L7_RC_t cliIsValidSysContact(L7_char8 * input);
L7_RC_t cliPromptIsAlphaNum(L7_char8 * input);
L7_RC_t cliIsBlankLine(L7_char8 * input);
void cliPromptReset();
L7_BOOL cliIsRestrictedFilterMac(L7_uchar8 * macaddr);
L7_RC_t cliSlotPortToIntNum(EwsContext ewsContext, const L7_char8 * slot_port,
    L7_uint32 * slot, L7_uint32 * port, L7_uint32 * intIfNum);

L7_RC_t cliConvertTo32BitUnsignedInteger(const L7_char8 * buf, L7_uint32 * pVal);
L7_RC_t cliConvertHexTo32BitUnsignedInteger(const L7_char8 * buf, L7_uint32 * pVal);
L7_RC_t cliConvertTo32BitSignedInteger(const L7_char8 * buf, L7_int32 * pVal);
L7_RC_t cliConvertVlanRange(const L7_char8 *vlanRange, L7_uint32 *rangeLow,
                            L7_uint32 *rangeHigh);
L7_RC_t cliConvertTo64BitUnsignedInteger(const L7_char8 * buf, L7_uint64 * pVal);
L7_RC_t cliConvertTo64BitSignedInteger(const L7_char8 * buf, L7_int64 * pVal);
L7_RC_t cliSlotPortCpuCheck(L7_uint32 slot,L7_uint32 port);
L7_uint32 cliWebIntfBWGet(L7_uint32 intIfNum, L7_uint32 * bw);
L7_RC_t cliStrCaseCmp(char * str1, char * str2, int n);
L7_RC_t cliRemoveQuotes(L7_char8 * input);
L7_BOOL cliNoCaseCompare(L7_char8 * buf1, L7_char8 * buf2 );
L7_BOOL cliSSHLoginCheck(L7_char8 *enteredInfo, L7_uint32 accessLevel,
                         L7_char8 *challengePhrase, EwsContext context);
void cliCmdScrollSet(L7_BOOL scroll);
L7_BOOL cliCmdScrollGet();
void cliDiffservBuildValDscpKeyWords(L7_char8 * buf, L7_uint32 bufSize);
L7_RC_t cliDiffservConvertDSCPValToString(L7_uint32 dscpVal, L7_char8 * dscpString);
L7_RC_t cliDiffservConvertDSCPStringToValNoVerify(L7_char8 * dscpString, L7_uint32 * dscpVal);
L7_RC_t cliDiffservConvertDSCPStringToVal(L7_char8 * dscpString, L7_uint32 * dscpVal);
L7_RC_t cliDiffservConvertDSCPValToKeyword(L7_uint32 dscpVal, L7_char8 * dscpString, L7_uint32 dscpStringSize);
L7_RC_t cliDiffservConvertEtherTypeStringToKeyId(L7_char8 * strKeyword, L7_uint32 * etypeKeyId);
L7_RC_t cliDiffservConvertEtherTypeKeyIdToString(L7_uint32 etypeKeyId, L7_char8 * strKeyword, L7_uint32 strKeywordSize);

EwsCliCommandP buildTreeInterfaceHelp(EwsCliCommandP depth, L7_uint32 count,
    EwsCliCommandOptions opt, L7_COMPONENT_IDS_t componentId, EwsCliNodeNoFormStatus noFormStatus);
EwsCliCommandP buildTreeLogInterfaceHelp(EwsCliCommandP depth, EwsCliNodeNoFormStatus noFormStatus);
L7_BOOL cliValidIPAddrCheck(const L7_char8 * buf);
L7_RC_t cliValidSpecificUSPCheck(const L7_char8 * buf, L7_uint32 * unit,
    L7_uint32 * slot, L7_uint32 * port);
L7_RC_t cliUSPParse(const L7_char8 *buf, L7_uint32 *intfList, L7_uint32 *intfCnt);
L7_char8 *cliGetIntfName(L7_uint32 intfNum, L7_uint32 unit, L7_uint32 slot, L7_uint32 port);

L7_RC_t cliValidPrefixPrefixLenCheck(const L7_char8 * buf, L7_in6_addr_t * prefix, L7_uint32 * prefixLen);
L7_RC_t cliValidPrefixPrefixLenCheckIPV4(const L7_char8 * buf, L7_uchar8 * prefix, L7_uint32 * mask);

L7_char8 *cliStringSeparateHelp(L7_char8 * * input, const L7_char8 * delim);
L7_char8 *cliSyntaxInterfaceHelp(void);
L7_char8 *cliSyntaxInterfaceTunnelHelp(void);
L7_char8 *cliSyntaxLogInterfaceHelp(void);
L7_char8 *cliDisplayInterfaceHelp(L7_uint32 unit, L7_uint32 slot, L7_uint32 port);
L7_char8 *cliDisplayHashModeHelp(L7_uint32 hashMode);
L7_RC_t  cliValidUnitCheck(const L7_char8 * buf, L7_uint32 * unit, L7_BOOL * allUnits);
L7_uint32 usmCliMaskLength(L7_uint32 mask);
L7_RC_t cliGetMaxMinUnitnum(L7_uint32 * max, L7_uint32 * min);
void cliFormatStringTruncate(EwsContext ewsContext,L7_char8 * buf,L7_uint32 len);
void cliStringTruncate (L7_char8 * buf, L7_uint32 len);
L7_BOOL cliPasswdExpireCheck(L7_char8 * enteredInfo, EwsContext context);
L7_BOOL  configExpiredPasswd(EwsContext ewsContext, L7_char8 * enteredInfo, L7_uint32 * callCount);

char *cliUtilGetRootPrompt (void);
char *cliutilGetTelnetDisabledError(void);
char *cliutilGetMaxConnectionsError(void);
char *cliutilGetConnectionClosedMsg(void);

/* Structure added for the SNMP Show commands */

#define SNMP_MAX_ATTRIBUTES   10
#define OSPF_MAX_ATTRIBUTES   10

typedef struct
{
  L7_BOOL flagRemain;
  L7_uchar8 remainStr[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 width;
  L7_int32 precision;
} snmpAttribute;

typedef snmpAttribute ospfInterfaceAttrib;

L7_BOOL checkPrintRemain(L7_uint32 count, snmpAttribute * attrib);
void printRemain(EwsContext ewsContext, L7_uint32 count, snmpAttribute * attrib);

L7_uint32 cliUrlParser(L7_char8 * in_str, L7_char8 * ipaddr, L7_uint32 ipaddrSize,
    L7_char8 * path, L7_uint32 pathSize, L7_char8 * filename, L7_uint32 filenameSize,
    L7_char8 * xferToken, L7_uint32 xferTokenSize);

L7_char8 *cliTrimLeft (L7_char8 * str);
L7_char8 *cliTrimRight (L7_char8 * str);
L7_char8 *cliTrim (L7_char8 * str);
L7_BOOL cliIsPromptRespQuit (void);
L7_RC_t cliUtilsCodeVersionGet(L7_uint32 unit, L7_char8 *buf, L7_uint32 bufSize);
L7_RC_t cliIPHostAddressValidate(EwsContext context, L7_uchar8 *hostAddr,
                               L7_uint32 *ival, L7_IP_ADDRESS_TYPE_t *addrType);
L7_RC_t cliParseRangeInput(const L7_char8 *buf, L7_uint32 *count, L7_uint32 *list, L7_uint32 listSize);
void displayRangeFormat(EwsContext ewsContext, L7_uint32 *vids, L7_uint32 numVids,  L7_char8 * commandString);
void bubbleSort(int x[], int n);
L7_int32 addIncludeVid(L7_uchar8 *buffer, L7_uint32 firstVid, L7_uint32 lastVid, L7_int32 cmdLen, L7_char8 * commandString);

void  displayUSPRangeFormat(EwsContext ewsContext, L7_uint32 *vids, L7_uint32 numVids,  L7_char8 * commandString);
L7_int32 addIncludeIntf(L7_uchar8 *buffer, L7_uint32 firstVid, L7_uint32 lastVid, L7_int32 cmdLen, L7_char8 * commandString);

L7_RC_t cliSplitLongNameGet(L7_uchar8 **name, L7_uchar8 *splitName);

L7_RC_t cliMonthNumberGet(const char *monthName, L7_uint32 *monthNumber);
L7_RC_t cliDayOfWeekGet(const char *dayName, L7_uint32 *dayNumber);
L7_RC_t cliSummerTimeValidate (L7_uint32 stMonth,
                                L7_uint32 stDay, L7_uint32 stYear, L7_uint32 stWeek,
                                L7_uint32 endMonth, L7_uint32 endDay,
                                L7_uint32 endYear, L7_uint32 endWeek, L7_BOOL recurFlag);

L7_BOOL cliConvertOUI(L7_uchar8 * buf, L7_uchar8 * oui);
void cligettimeinMonthsAndDays(L7_uint32 rawTime, L7_char8 *sbuf);
void cliGetUtcTimeInMonthsAndDays(L7_uint32 rawTime, L7_char8 *sbuf);

#endif

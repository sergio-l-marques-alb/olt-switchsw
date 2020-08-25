/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#ifndef SR_SNMPv3_PACKET_MIB
#undef I_snmpEngineID
#undef I_snmpEngineBoots
#undef I_snmpEngineTime
#undef I_snmpEngineMaxMessageSize
#undef I_snmpUnknownSecurityModels
#undef I_snmpInvalidMsgs
#undef I_snmpUnknownPDUHandlers
#endif /* SR_SNMPv3_PACKET_MIB */

#if (! ( defined(SR_USER_SEC_MODEL) ) || ! ( defined(SR_SNMPv3_PACKET_MIB) ))
#undef I_usmStatsUnsupportedSecLevels
#undef I_usmStatsNotInTimeWindows
#undef I_usmStatsUnknownUserNames
#undef I_usmStatsUnknownEngineIDs
#undef I_usmStatsWrongDigests
#undef I_usmStatsDecryptionErrors
#undef I_usmUserSpinLock
#undef I_usmUserEngineID
#undef I_usmUserName
#undef I_usmUserSecurityName
#undef I_usmUserCloneFrom
#undef I_usmUserAuthProtocol
#undef I_usmUserAuthKeyChange
#undef I_usmUserOwnAuthKeyChange
#undef I_usmUserPrivProtocol
#undef I_usmUserPrivKeyChange
#undef I_usmUserOwnPrivKeyChange
#undef I_usmUserPublic
#undef I_usmUserStorageType
#undef I_usmUserStatus
#undef I_usmTargetTag
#endif	/* (! ( defined(SR_USER_SEC_MODEL) ) || ! ( defined(SR_SNMPv3_PACKET_MIB) )) */


#if (! ( defined(SR_SNMPv3_ADMIN) ) || ! ( defined(SR_SNMP_ADMIN_MIB) ))

#undef I_vacmContextName
#undef I_vacmSecurityModel
#undef I_vacmSecurityName
#undef I_vacmGroupName
#undef I_vacmSecurityToGroupStorageType
#undef I_vacmSecurityToGroupStatus
#undef I_vacmAccessContextPrefix
#undef I_vacmAccessSecurityLevel
#undef I_vacmAccessContextMatch
#undef I_vacmAccessReadViewName
#undef I_vacmAccessWriteViewName
#undef I_vacmAccessNotifyViewName
#undef I_vacmAccessStorageType
#undef I_vacmAccessStatus
#undef I_vacmAccessEntryIndex_vacmGroupName
#undef I_vacmAccessEntryIndex_vacmSecurityModel
#undef I_vacmViewSpinLock
#undef I_vacmViewTreeFamilyViewName
#undef I_vacmViewTreeFamilySubtree
#undef I_vacmViewTreeFamilyMask
#undef I_vacmViewTreeFamilyType
#undef I_vacmViewTreeFamilyStorageType
#undef I_vacmViewTreeFamilyStatus
#undef I_snmpTargetSpinLock
#undef I_snmpTargetAddrName
#undef I_snmpTargetAddrTDomain
#undef I_snmpTargetAddrTAddress
#undef I_snmpTargetAddrTimeout
#undef I_snmpTargetAddrRetryCount
#undef I_snmpTargetAddrTagList
#undef I_snmpTargetAddrParams
#undef I_snmpTargetAddrStorageType
#undef I_snmpTargetAddrRowStatus
#undef I_snmpTargetAddrTMask
#undef I_snmpTargetAddrMMS
#undef I_snmpTargetParamsName
#undef I_snmpTargetParamsMPModel
#undef I_snmpTargetParamsSecurityModel
#undef I_snmpTargetParamsSecurityName
#undef I_snmpTargetParamsSecurityLevel
#undef I_snmpTargetParamsStorageType
#undef I_snmpTargetParamsRowStatus
#undef I_snmpUnavailableContexts
#undef I_snmpUnknownContexts
#undef I_snmpNotifyName
#undef I_snmpNotifyTag
#undef I_snmpNotifyType
#undef I_snmpNotifyStorageType
#undef I_snmpNotifyRowStatus
#undef I_snmpNotifyFilterProfileName
#undef I_snmpNotifyFilterProfileStorType
#undef I_snmpNotifyFilterProfileRowStatus
#undef I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName
#undef I_snmpNotifyFilterSubtree
#undef I_snmpNotifyFilterMask
#undef I_snmpNotifyFilterType
#undef I_snmpNotifyFilterStorageType
#undef I_snmpNotifyFilterRowStatus
#undef I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName
#undef I_snmpProxyName
#undef I_snmpProxyType
#undef I_snmpProxyContextEngineID
#undef I_snmpProxyContextName
#undef I_snmpProxyTargetParamsIn
#undef I_snmpProxySingleTargetOut
#undef I_snmpProxyMultipleTargetOut
#undef I_snmpProxyStorageType
#undef I_snmpProxyRowStatus

#else	/* (! ( defined(SR_SNMPv3_ADMIN) ) || ! ( defined(SR_SNMP_ADMIN_MIB) )) */

#ifdef SR_NOTIFY_BASIC_COMPLIANCE
#ifndef SR_PROXY_COMPLIANCE
#undef I_snmpTargetAddrTimeout
#undef I_snmpTargetAddrRetryCount
#endif /* !SR_PROXY_COMPLIANCE */
#undef I_snmpNotifyFilterProfileName
#undef I_snmpNotifyFilterProfileStorType
#undef I_snmpNotifyFilterProfileRowStatus
#undef I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName 
#undef I_snmpNotifyFilterSubtree
#undef I_snmpNotifyFilterMask
#undef I_snmpNotifyFilterType
#undef I_snmpNotifyFilterStorageType
#undef I_snmpNotifyFilterRowStatus
#undef I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName 
#endif /* SR_NOTIFY_BASIC_COMPLIANCE */

#ifdef SR_NOTIFY_BASIC_WITH_FILTERS_COMPLIANCE
#ifndef SR_PROXY_COMPLIANCE
#undef I_snmpTargetAddrTimeout
#undef I_snmpTargetAddrRetryCount
#endif /* !SR_PROXY_COMPLIANCE */
#endif /* SR_NOTIFY_BASIC_WITH_FILTERS_COMPLIANCE */

#ifndef SR_PROXY_COMPLIANCE
#undef I_snmpProxyTable
#undef I_snmpProxyEntry
#undef I_snmpProxyName
#undef I_snmpProxyType
#undef I_snmpProxyContextEngineID
#undef I_snmpProxyContextName
#undef I_snmpProxyTargetParamsIn
#undef I_snmpProxySingleTargetOut
#undef I_snmpProxyMultipleTargetOut
#undef I_snmpProxyStorageType
#undef I_snmpProxyRowStatus
#endif /* SR_PROXY_COMPLIANCE */

#endif	/* (! ( defined(SR_SNMPv3_ADMIN) ) || ! ( defined(SR_SNMP_ADMIN_MIB) )) */

#undef I_apoTargetVersion


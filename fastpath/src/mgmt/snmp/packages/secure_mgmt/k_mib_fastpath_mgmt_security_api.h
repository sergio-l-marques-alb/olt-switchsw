/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_fastpath_mgmt_security_api.h
*
* Purpose: APIs used for FASTPATH Security MIB
*
* Created by: Colin Verne 09/17/2003
*
* Component: SNMP
*
*********************************************************************/

#ifndef _K_FASTPATH_MGMT_SECURITY_API_H
#define _K_FASTPATH_MGMT_SECURITY_API_H

#include "l7_common.h"

/* Begin Function Declarations: k_mib_fastpath_mgmt_security_api.h */

L7_RC_t
snmpAgentSSLAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

L7_RC_t
snmpAgentSSLAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpAgentSSLProtocolLevelGet(L7_uint32 UnitIndex, L7_uint32 *val);

L7_RC_t
snmpAgentSSLProtocolLevelSet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpAgentSSHAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

L7_RC_t
snmpAgentSSHAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpAgentSSHProtocolLevelGet(L7_uint32 UnitIndex, L7_uint32 *val);

L7_RC_t
snmpAgentSSHProtocolLevelSet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpAgentMaxSSHSessionsSet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpAgentMaxSSHSessionsGet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpAgentSSHTimeoutSet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpAgentSSHTimeoutGet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpAgentSSHKeysPresentGet(L7_uint32 *val);
L7_RC_t
snmpAgentSSHKeyGenerationStatusGet(L7_uint32 *val);

L7_RC_t
snmpAgentSSLCertificateControlSet(L7_uint32 val);

L7_RC_t
snmpAgentSSHDSAKeyControlSet(L7_uint32 val);

L7_RC_t
snmpAgentSSHRSAKeyControlSet(L7_uint32 val);

/* End Function Declarations */

#endif /* _K_FASTPATH_MGMT_SECURITY_API_H */

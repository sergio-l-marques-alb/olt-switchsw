/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_tacacsclient_api.h
*
* Purpose: System-specific code for TACACS Private MIB
*
* Created by: nramesh 08/19/2005
*
* Component: SNMP
* *********************************************************************/
#include <k_private_base.h>
#include <defaultconfig.h>

#define SNMP_TACACS_DEFAULT_PORT_NUM FD_TACACS_PLUS_PORT_NUM
#define SNMP_TACACS_DEFAULT_PRIORITY FD_TACACS_PLUS_PRIORITY

/********************************************************************
  Function: snmpTacacsServerGet
  Purpose:  Verifies whether the given TACACS server is configured
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
  Comment: 
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsServerGet(OctetString *agentTacacsServerIpAddress, L7_uint32 *type, L7_char8 *get_ipAddress);

/********************************************************************
  Function: snmpTacacsServerNextGet
  Purpose:  Retrieves the next TACACS server configured
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsServerIpAddressNext - Next TACACS Server IP Address
  Comment:     
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsServerNextGet(OctetString *agentTacacsServerIpAddress,
                        OctetString **agentTacacsServerIpAddressNext, L7_uint32 *next_type, L7_char8 *next_ipAddress);

/********************************************************************
  Function: snmpTacacsPortNumberGet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsPortNumber - TACACS Server Port Number
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsPortNumberGet(L7_uchar8* agentTacacsServerIpAddress,L7_uint32 type, 
                        L7_uint32 *agentTacacsPortNumber);

/********************************************************************
  Function: snmpTacacsPortNumberSet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsPortNumber - TACACS Server Port Number
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsPortNumberSet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                        L7_uint32 agentTacacsPortNumber);

/********************************************************************
  Function: snmpTacacsTimeOutGet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsTimeOut - Timeout
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsTimeOutGet(L7_uchar8 *agentTacacsServerIpAddress, L7_uint32 type,
                     L7_uchar8 *agentTacacsTimeOut);

/********************************************************************
  Function: snmpTacacsTimeOutSet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsTimeOut - Timeout
  Comment:     Support for the MIB object - Supported.   Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsTimeOutSet(L7_uchar8 *agentTacacsServerIpAddress, L7_uint32 type,
                     L7_uchar8 agentTacacsTimeOut);

/********************************************************************
  Function: snmpTacacsKeyGet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsKey - TACACS server key
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsKeyGet(L7_uchar8* agentTacacsServerIpAddress,L7_uint32 type,
                 L7_char8 *agentTacacsKey);

/********************************************************************
  Function: snmpTacacsKeySet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsKey - TACACS server key
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsKeySet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                 L7_char8 *agentTacacsKey);

/********************************************************************
  Function: snmpTacacsPriorityGet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsPriority - TACACS Server priority
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsPriorityGet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                      L7_uint32 *agentTacacsPriority);

/********************************************************************
  Function: snmpTacacsPrioritySet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsPriority - TACACS Server priority
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsPrioritySet(L7_uchar8* agentTacacsServerIpAddress,L7_uint32 type,
                      L7_uint32 agentTacacsPriority);

/********************************************************************
  Function: snmpTacacsRowStatusSet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsServerStatus - TACACS Server row status
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsRowStatusSet(L7_uchar8* agentTacacsServerIpAddress,L7_uint32 type,
                       L7_uint32 agentTacacsServerStatus);

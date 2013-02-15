/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename   snmp_util_api.h
 *
 * @purpose    Simple Network Management Protocol (SNMP) File
 *
 * @component  snmp component
 *
 * @comments   none
 *
 * @create     03/02/2001
 *
 * @author     asuthan
 *
 * @end
 *             
 **********************************************************************/

#ifndef SNMP_UTIL_API_H
#define SNMP_UTIL_API_H

#include "l7_common.h"
#include "snmp_exports.h"
#include "l3_addrdefs.h"
#include "snmp_exports.h"

typedef struct 
{
  L7_char8    agentCommunityName[L7_SNMP_SIZE];
  L7_char8    agentCommunityOwner[L7_SNMP_SIZE];
  L7_uint32   agentCommunityClientIpAddr;
  L7_uint32   agentCommunityClientIpMask;
  L7_uint32   agentCommunityAccessLevel;
  L7_uint32   agentCommunityStatus;

} snmpComm_t;

typedef enum
{
  L7_SNMP_TRAP_VER_NONE    = 0,
  L7_SNMP_TRAP_VER_SNMPV1  = 1,
  L7_SNMP_TRAP_VER_SNMPV2C = 2,
} snmpTrapVer_t;

typedef struct
{
  L7_char8       agentTrapMgrCommunityName[L7_SNMP_SIZE];
  L7_uint32      agentTrapMgrIpAddr;
  L7_in6_addr_t  agentTrapMgrIPv6Addr;
  L7_uint32      agentTrapMgrStatus;
  snmpTrapVer_t  agentTrapMgrVersion;
  L7_uint32      agentTrapMgrIpAddrType;
} snmpTrapMgr_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  snmpComm_t     snmpComm[L7_MAX_SNMP_COMM];
  snmpTrapMgr_t  snmpTrapMgr[L7_MAX_SNMP_COMM];
  L7_uint32      checkSum;
} snmpCfgData_t;


/*********************************************************************
 * @purpose  SNMP deamon tesk
 *
 * @param    
 *
 * @returns  void
 *
 * @notes    For VxWorks the entry point will be unique so as not to confuse snmpd
 * @notes    with other "main" routines.  The VxWorks shell does no support argc/argv
 * @notes    All functions are called with up to ten arguments.  Unused arguments are
 * @notes    passed in with zero (0) value.
 *
 * @end
 *********************************************************************/
#ifdef _L7_OS_VXWORKS_
L7_int32 snmpd_main(L7_char8 *arg1_in, L7_char8 *arg2_in, L7_char8 *arg3_in, 
    L7_char8 *arg4_in, L7_char8 *arg5_in, L7_char8 *arg6_in, L7_char8 *arg7_in, 
    L7_char8 *arg8_in, L7_char8 *arg9_in, L7_char8 *arg10_in);
#else /* _L7_OS_LINUX_ */
L7_int32 snmpd_main(L7_int32 argc, L7_char8 **argv, L7_char8 **envp);
#endif

/*********************************************************************
 *
 * @purpose  Starts SNMP task function
 *
 * @notes    none 
 *
 * @end
 *********************************************************************/
void L7_snmp_task (void);

/*********************************************************************
 *
 * @purpose Starts monitoring /proc/net/snmp under linux
 *
 * @notes   none
 * 
 * @end
 **********************************************************************/
#ifdef _L7_OS_LINUX_
void L7_snmp_proc_monitor (void);
#endif

/*********************************************************************
 *
 * @purpose  Takes SNMP Semaphore
 *
 * @notes    none 
 *
 * @end
 *********************************************************************/
L7_RC_t snmpSemaTake(void);

/*********************************************************************
 *
 * @purpose  Takes SNMP Semaphore
 *
 * @notes    none 
 *
 * @end
 *********************************************************************/
L7_RC_t snmpSemaGive(void);

L7_RC_t snmpTrapSend(L7_uint32 trap_id, void *pdata, void(*func_ptr)(L7_uint32, void*));

#endif /* SNMP_UTIL_API_H */

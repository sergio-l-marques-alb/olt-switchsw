/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename trap.c
 *
 * @purpose Trap Manager Main File
 *
 * @component trapmgr
 *
 * @comments none
 *
 * @create 08/30/2000
 *
 * @author bmutz
 * @end
 *
 **********************************************************************/

#include <string.h>

#include "l7_common.h"
#include "osapi.h"
#include "trapapi.h"
#include "trap.h"
#include "usmdb_snmp_trap_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_sim_api.h"

trapMgrCfgData_t trapMgrCfgData;

extern trapMgrTrapData_t trapMgrTrapData;
extern void *trapMgrSemId;

/*********************************************************************
 * @purpose  Returns Trap Manager's trap authentication state
 *
 * @param    void
 *
 * @returns  state   trap authentication state (L7_ENABLE or L7_DISABLE)
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapAuth(void)
{
  L7_uint32 temp_val;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (usmDbSnmpEnableAuthenTrapsGet(unit, &temp_val) == L7_SUCCESS)
    return temp_val;
  return L7_DISABLE;
}

/*********************************************************************
 * @purpose  Sets Trap Manager's trap authentication state
 *
 * @param    state   trap authentication state (L7_ENABLE or L7_DISABLE)
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void trapMgrSetTrapAuth(L7_uint32 state)
{
  /* push AuthenTrap configuration to SNMP Agent */
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    usmDbSnmpEnableAuthenTrapsSet(unit, state);

  if (state != trapMgrCfgData.trapAuth)
  {
    trapMgrCfgData.trapAuth = state;
    trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  }
}

/*********************************************************************
 * @purpose  Returns Trap Manager's trap link state
 *
 * @param    void
 *
 * @returns  state   trap link state (L7_ENABLE or L7_DISABLE)
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapLink(void)
{
  return(trapMgrCfgData.trapLink);
}

/*********************************************************************
 * @purpose  Sets Trap Manager's trap link state
 *
 * @param    state   trap link state (L7_ENABLE or L7_DISABLE)
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void trapMgrSetTrapLink(L7_uint32 state)
{
  trapMgrCfgData.trapLink = state;
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
 * @purpose  Returns Trap Manager's trap multiUsers state
 *
 * @param    void
 *
 * @returns  state   trap multiUsers state (L7_ENABLE or L7_DISABLE)
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapMultiUsers(void)
{
  return(trapMgrCfgData.trapMultiUsers);
}

/*********************************************************************
 * @purpose  Sets Trap Manager's trap multiUsers state
 *
 * @param    state   trap multiUsers state (L7_ENABLE or L7_DISABLE)
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void trapMgrSetTrapMultiUsers(L7_uint32 state)
{
  trapMgrCfgData.trapMultiUsers = state;
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
 * @purpose  Returns Trap Manager's trap spanning tree state
 *
 * @param    void
 *
 * @returns  state   trap spanning tree state (L7_ENABLE or L7_DISABLE)
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapSpanningTree(void)
{
  return(trapMgrCfgData.trapSpanningTree);
}

/*********************************************************************
 * @purpose  Sets Trap Manager's trap spanning tree state
 *
 * @param    state   trap spanning tree state (L7_ENABLE or L7_DISABLE)
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void trapMgrSetTrapMultiSpanningTree(L7_uint32 state)
{
  trapMgrCfgData.trapSpanningTree = state;
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
 * @purpose  Returns Trap Manager's trap general system state
 *
 * @param    void
 *
 * @returns  state   trap general system state (L7_ENABLE or L7_DISABLE)
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapGeneralSystem(void)
{
  return(trapMgrCfgData.trapGeneralSystem);
}

/*********************************************************************
 * @purpose  Sets Trap Manager's general system state
 *
 * @param    state   trap general system state (L7_ENABLE or L7_DISABLE)
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void trapMgrSetTrapGeneralSystem(L7_uint32 state)
{
  trapMgrCfgData.trapGeneralSystem = state;
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
 * @purpose  Returns Trap Manager's trap dot1q state
 *
 * @param    void
 *
 * @returns  state   trap dot1q state (L7_ENABLE or L7_DISABLE)
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapDot1q(void)
{
  return(trapMgrCfgData.trapDot1q);
}

/*********************************************************************
 * @purpose  Sets Trap Manager's dot1q  state
 *
 * @param    state   trap dot1q state (L7_ENABLE or L7_DISABLE)
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void trapMgrSetTrapDot1q(L7_uint32 state)
{
  trapMgrCfgData.trapDot1q = state;
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
 * @purpose  Returns Trap Manager's trap POE state
 *
 * @param    void
 *
 * @returns  state   trap POE state (L7_ENABLE or L7_DISABLE)
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapPoe(void)
{
  return(trapMgrCfgData.trapPoe);
}

/*********************************************************************
 * @purpose  Sets Trap Manager's POE state
 *
 * @param    state   trap POE state (L7_ENABLE or L7_DISABLE)
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void trapMgrSetTrapPoe(L7_uint32 state)
{
  trapMgrCfgData.trapPoe = state;
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
 * @purpose  Returns Trap Manager's trap MAC Lock Violation state
 *
 * @param    void
 *
 * @returns  state   trap MAC Lock Violation state (L7_ENABLE or L7_DISABLE)
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapMacLockViolation(void)
{
  return(trapMgrCfgData.trapMacLockViolation);
}

/*********************************************************************
 * @purpose  Sets Trap Manager's MAC Lock Violation state
 *
 * @param    state   trap MAC Lock Violation state (L7_ENABLE or L7_DISABLE)
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void trapMgrSetTrapMacLockViolation(L7_uint32 state)
{
  trapMgrCfgData.trapMacLockViolation = state;
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
 * @purpose  Returns Trap Manager's amount of undisplayed trap messages
 *
 * @param    void
 *
 * @returns  amount  Amount of undisplayed trap messages
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetAmountOfUnDisplayedTraps(void)
{
  return(trapMgrTrapData.amountOfUnDisplayedTraps);
}

/*********************************************************************
 * @purpose  Returns Trap Manager's total amount of traps since last reset
 *
 * @param    void
 *
 * @returns  amount  Total amount of traps since last reset
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTotalAmountOfTraps(void)
{
  return(trapMgrTrapData.totalAmountOfTraps);
}

/*********************************************************************
 * @purpose  Returns Trap Manager's complete trap log
 *
 * @param    user  type of user (TRAPMGR_USER_DISPLAY or TRAPMGR_USER_TFTP)
 * @param    log   pointer to buf with size
 *                (trapMgrTrapLogEntry_t * TRAPMGR_TABLE_SIZE)
 *
 * @returns  amount  Total number of traps returned in log
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapLog(L7_uint32 user, trapMgrTrapLogEntry_t trapLog[])
{
  L7_uint32 count;
  L7_int32 mr;  /* index to last trap received trapLog array */
  L7_int32 i;   /* index to trapLog array */
  L7_int32 j;   /* index to trapMsgs array */

  osapiSemaTake (trapMgrSemId, L7_WAIT_FOREVER);

  count = trapMgrTrapData.currentAmountOfTraps;

  /* if there are more than TRAPMGR_TABLE_SIZE traps, can only display TRAPMGR_TABLE_SIZE */
  if (count > TRAPMGR_TABLE_SIZE)
    count = TRAPMGR_TABLE_SIZE;

  i = mr = trapMgrTrapData.lastReceivedTrap;    /* start with the last trap received */
  for (j=0; ((i >= 0) && (j < count)); i--, j++)  /* go backward in trapLog array */
  {
    trapLog[j].gen = trapMgrTrapData.trapLog[i].gen;
    trapLog[j].spec = trapMgrTrapData.trapLog[i].spec;
    bzero(trapLog[j].timestamp, TRAPMGR_TIME_STR_SIZE);
    strncpy(trapLog[j].timestamp, trapMgrTrapData.trapLog[i].timestamp, TRAPMGR_TIME_STR_SIZE);
    bzero(trapLog[j].message, TRAPMGR_MSG_SIZE);
    strncpy(trapLog[j].message, trapMgrTrapData.trapLog[i].message, TRAPMGR_MSG_SIZE);
  }

  if (j < count) /* if there are still more traps to be copied */
    for (i=TRAPMGR_TABLE_SIZE-1; ((i > mr) && (j < count)); i--, j++) /* wrap and copy more traps */
    {
      trapLog[j].gen = trapMgrTrapData.trapLog[i].gen;
      trapLog[j].spec = trapMgrTrapData.trapLog[i].spec;
      bzero(trapLog[j].timestamp, TRAPMGR_TIME_STR_SIZE);
      strncpy(trapLog[j].timestamp, trapMgrTrapData.trapLog[i].timestamp, TRAPMGR_TIME_STR_SIZE);
      bzero(trapLog[j].message, TRAPMGR_MSG_SIZE);
      strncpy(trapLog[j].message, trapMgrTrapData.trapLog[i].message, TRAPMGR_MSG_SIZE);
    }

  if (user == TRAPMGR_USER_DISPLAY)
  {
    trapMgrTrapData.amountOfUnDisplayedTraps = 0;
  }

  osapiSemaGive (trapMgrSemId);

  return(count);
}

/*********************************************************************
 * @purpose  Returns Trap Manager's last received trap log entry index
 *
 * @returns  index of last received trap log entry

 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 trapMgrGetTrapLogEntryLastReceived(void)
{
  return trapMgrTrapData.lastReceivedTrap;
}

/*********************************************************************
 * @purpose  Returns Trap Manager's trap log entry
 *
 * @param    user  type of user (TRAPMGR_USER_DISPLAY or TRAPMGR_USER_TFTP)
 * @param    log   pointer to trapMgrTrapLogEntry_t structure
 *
 * @returns  L7_SUCCESS if the entry exists
 *           L7_FAILURE if the entry does not exist
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t trapMgrGetTrapLogEntry(L7_uint32 user, L7_uint32 index, trapMgrTrapLogEntry_t *trapLogEntry)
{
  L7_int32 count;
  L7_int32 i;   /* index to trapLog array */

  osapiSemaTake (trapMgrSemId, L7_WAIT_FOREVER);

  count = trapMgrTrapData.currentAmountOfTraps;
  if (count >= TRAPMGR_TABLE_SIZE)
    count = TRAPMGR_TABLE_SIZE;

  if (count == 0)
  {
    osapiSemaGive (trapMgrSemId);
    return L7_FAILURE;
  }

  if ((index > (count-1)) || index < 0)
  {
    osapiSemaGive (trapMgrSemId);
    return L7_FAILURE;
  }

  i = index;

  trapLogEntry->gen = trapMgrTrapData.trapLog[i].gen;
  trapLogEntry->spec = trapMgrTrapData.trapLog[i].spec;
  bzero(trapLogEntry->timestamp, TRAPMGR_TIME_STR_SIZE);
  strncpy(trapLogEntry->timestamp, trapMgrTrapData.trapLog[i].timestamp, TRAPMGR_TIME_STR_SIZE);
  bzero(trapLogEntry->message, TRAPMGR_MSG_SIZE);
  strncpy(trapLogEntry->message, trapMgrTrapData.trapLog[i].message, TRAPMGR_MSG_SIZE);


  if (user == TRAPMGR_USER_DISPLAY && trapMgrTrapData.amountOfUnDisplayedTraps > 0)
  {
    trapMgrTrapData.amountOfUnDisplayedTraps--;
  }

  osapiSemaGive (trapMgrSemId);

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Clear the trap log
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void trapMgrClearTrapLog(void)
{
  osapiSemaTake (trapMgrSemId, L7_WAIT_FOREVER);

  trapMgrTrapData.lastReceivedTrap = 0;
  trapMgrTrapData.totalAmountOfTraps = 0;
  trapMgrTrapData.amountOfUnDisplayedTraps = 0;
  trapMgrTrapData.currentAmountOfTraps = 0;

  osapiSemaGive (trapMgrSemId);
}

/*********************************************************************
 * @purpose  creates trap log file
 *
 * @param    filename
 *
 * @returns  rc
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t trapMgrCreateTrapLogFile (L7_char8 * filename)
{
  static trapMgrTrapLogEntry_t trapLog[TRAPMGR_TABLE_SIZE];
  L7_RC_t rc = L7_SUCCESS;
  L7_int32 count, i, filedesc;

  (void) osapiFsDeleteFile (filename);
  (void) osapiFsCreateFile (filename);

  filedesc = osapiFsOpen (filename);

  count = (L7_int32) trapMgrGetTrapLog (TRAPMGR_USER_TFTP, trapLog);

  if (count == 0)
  {
    osapiSnprintf(trapLog[0].message, sizeof(trapLog[0].message), "Trap Log is empty");
    rc |= osapiFsWriteNoClose(filedesc, trapLog[0].message, strlen(trapLog[0].message));
  }

  for (i = 0; i < count; i++)
  {
    rc |= osapiFsWriteNoClose (filedesc, trapLog[i].timestamp, (L7_int32) strlen (trapLog[i].timestamp));
    rc |= osapiFsWriteNoClose (filedesc, "--", (L7_int32) strlen ("--"));
    rc |= osapiFsWriteNoClose (filedesc, trapLog[i].message, (L7_int32) strlen (trapLog[i].message));
    rc |= osapiFsWriteNoClose (filedesc, "\n", (L7_int32) strlen ("\n"));
  }
  (void) osapiFsClose (filedesc);
  return (0);
}

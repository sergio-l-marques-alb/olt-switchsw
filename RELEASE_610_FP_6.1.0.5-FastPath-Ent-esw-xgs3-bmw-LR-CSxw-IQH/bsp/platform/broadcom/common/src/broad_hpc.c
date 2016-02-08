/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  broad_hpc.c
*
* @purpose   Technology specific functions and data for the hardware
*            platform control component
*
* @component 
*
* @create    04/14/2003
*
* @author    jeffr 
* @end
*
*********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "sysapi_hpc.h"
#include "broad_hpc.h"
#include "osapi.h"
#include "log.h"
#include "unitmgr_api.h"


/*---------------------------------------------------------------------------*/
/*
**  HPC Hardware API Functions and Data  
*/

/**************************************************************************
*
* @purpose  Returns whether software learning is enabled. 
*
* @param    none
*
* @returns  L7_TRUE  - Software learning is enabled.
* @returns  L7_FALSE - Hardware learning is enabled.
*
* @notes 
*
* @end
*
*************************************************************************/
L7_BOOL hpcSoftwareLearningEnabled(void)
{
#ifdef PC_LINUX_HOST
  return L7_TRUE;
#else
  return L7_FALSE;
#endif
}





/**************************************************************************
*
* @purpose  Returns a boolean indicating whether the platform requires
*           HPC to use saved configuration data.
*
* @param    none
*
* @returns  L7_TRUE     saved configuration required
* @returns  L7_FALSE    saved configuration not required
*
* @notes 
*
* @end
*
*************************************************************************/
L7_BOOL hpcHardwareCfgDataRequired(void)
{
#ifdef L7_STACKING_PACKAGE
  return L7_TRUE;
#else
  return L7_FALSE;
#endif
}

/*********************************************************************
* @purpose  Saves HPC's configuration data.
*
* @param    data        pointer to configuration data to be saved
* @param    num_bytes   size of data in bytes
*
* @returns  L7_SUCCESS  no problems in save
* @returns  L7_ERROR    encountered error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareCfgDataSave(void *data, L7_uint32 num_bytes)
{
  L7_RC_t rc;

  rc = L7_SUCCESS;

#ifdef L7_STACKING_PACKAGE
  /* call save NVStore routine */
  if ((rc = osapiFsWrite(HPC_CFG_FILENAME, (L7_char8 *)data, num_bytes)) == L7_ERROR)
  {
    LOG_MSG("Error on call to osapiFsWrite routine on config file %s\n",HPC_CFG_FILENAME);
  }
#endif

  return(rc);
}

/**************************************************************************
*
* @purpose  Function used to retrieve HPC's saved configuration 
*           data.
*
* @param    data        pointer to configuration data to be retrieve
* @param    num_bytes   size of data in bytes
*
* @returns  L7_SUCCESS  no problems configuration retrieval
* @returns  L7_ERROR    encountered error 
*
* @notes 
*
* @end
*
*************************************************************************/
L7_RC_t hpcHardwareCfgDataRetrieve(void *data, L7_uint32 num_bytes)
{
  L7_RC_t rc;

  rc = L7_SUCCESS;

#ifdef L7_STACKING_PACKAGE
  /* call save NVStore routine */
  if ((rc = osapiFsRead(HPC_CFG_FILENAME, (L7_char8 *)data, num_bytes)) == L7_ERROR)
  {
    LOG_MSG("Error on call to osapiFsWrite routine on config file %s\n",HPC_CFG_FILENAME);
  }
#else
  memset (data, 0, num_bytes);
#endif

  return(rc);
}

/*********************************************************************
* @purpose  Given a unit, gets the unit's system identifier key
*
* @param    unit_number unit number
* @param    *key   pointer to unit key storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy L7_HPC_UNIT_ID_KEY_LEN bytes into *key
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareUnitIdentifierKeyGet(L7_uint32 unit, L7_enetMacAddr_t *key)
{
  return(unitMgrUnitIdKeyGet(unit, key));
}

/*********************************************************************
* @purpose  Saves local unit's assigned unit number.
*
* @param    unit        the unit number assigned to the local unit
*
* @returns  L7_SUCCESS  always, no hardware specific steps required
*                       for this technology
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareUnitNumberSet(L7_uint32 unit)
{
  return L7_SUCCESS;
}
#ifndef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Saves management preference
*
* @param    admin_pref  preference
*
* @returns  L7_SUCCESS  always, no hardware specific steps required
*                       for this technology
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareAdminPrefSet(L7_uint32 admin_pref)
{
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Designates a port to be used as a stacking port.
*
* @param    slot             slot number of port to be used for stacking interface
* @param    port             port number of port to be used for stacking interface
* @param    stacking_enabled L7_TRUE indicates the referenced port should be a 
*                            stacking interface, L7_FALSE indicates it should not
*
* @returns  L7_SUCCESS  no problems in save
* @returns  L7_ERROR    encountered error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareStackPortSet(L7_uint32 slot, L7_uint32 port, L7_BOOL stacking_enable)
{
  /* for Phase 1 implementation of FastPath Distributed Architecture, the only supported platform (Broadcom XGS, 
  ** using HiGig stacking ports) uses dedicated stacking ports.  Therefor this function has no purpose. 
  */
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Queries the platform component as to the largest message
*           size in bytes that the transport can handle.
*
* @param    none
*                                       
* @returns  the max size of message payload in bytes
*
* @comments    
*       
* @end
*********************************************************************/
L7_uint32 hpcHardwareTransportMaxMessageLengthGet(void)
{
  return(0);
}

/*********************************************************************
* @purpose  Queries the platform component as to the number of buffers
*           avaiable to the transport.
*
* @param    none
*                                       
* @returns  the max size of message payload in bytes
*
* @comments    
*       
* @end
*********************************************************************/
L7_uint32 hpcHardwareTransportNumberBuffersGet(void)
{
  return(0);
}
#endif

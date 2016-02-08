/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_registry.c
*
* @purpose Provide interface to hardware API's for unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 03-Nov-2000
*
* @author gaunce
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "usmdb_registry_api.h"

#include "registry.h"
#include "log.h"
#include "simapi.h"
#include "default_cnfgr.h"
#include "cardmgr_api.h"
#include "bspcpu_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*********************************************************************
*
* @purpose Get the serial number.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSwDevInfoSerialNumGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  sysapiRegistryGet(SERIAL_NUM, STR_ENTRY, (void*) buf);
  return L7_SUCCESS;    
}

/*********************************************************************
*
* @purpose Get the service Tag.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSwDevInfoServiceTagGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  sysapiRegistryGet(SERVICE_TAG, STR_ENTRY, buf);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Asset Tag.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSwDevInfoAssetTagGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  sysapiRegistryGet(ASSET_TAG, STR_ENTRY, buf);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the Asset Tag.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSwDevInfoAssetTagSet(L7_uint32 UnitIndex, L7_char8 *at)
{
  L7_char8 buf[L7_SYSMGMT_ASSETTAG_MAX + 1];

  osapiStrncpySafe(buf, at, sizeof(buf));
  if (sysapiRegistryPut((L7_uint32)ASSET_TAG, (L7_uint32)STR_ENTRY, buf) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
#if L7_FEAT_ASSET_TAG
  bspCpuWriteAssetTag(buf);
#endif

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the description of the machine.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSystemDescrGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  sysapiRegistryGet(SYSTEM_DESC, STR_ENTRY, buf);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the type of the machine.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMachineTypeGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  sysapiRegistryGet(MACHINE_TYPE, STR_ENTRY, (void*) buf);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the base card id
*          
* @param UnitIndex  the unit for this operation
* @param *cardID    ptr to card id  
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes This api is used primarily by the web code to uniquely 
*        identify all boxes so that the correct base java applet
*        image can be selected.  The card id's must be unique for
*        every box/manufacturer combination.
*        Box ID's are defined in the platform specific hpc_db.h 
*        and reproduced for all platforms in web.h.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbBaseCardIDGet(L7_uint32 UnitIndex, L7_uint32 *cardID)
{
  sysapiRegistryGet(BASE_CARDID, U32_ENTRY, (void*) cardID);
  *cardID = *cardID & 0xFFFF0000;       /* Remove the revision info */
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the machine model
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMachineModelGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  sysapiRegistryGet(MACHINE_MODEL, STR_ENTRY, (void*) buf);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the FRU number.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFRUNumGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  sysapiRegistryGet(FRU, STR_ENTRY, (void*) buf);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the part number.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPartNumGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  sysapiRegistryGet(PART_NUM, STR_ENTRY, (void*) buf);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the maintenance level.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMaintLevelGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  sysapiRegistryGet(MAINT_LEVEL, STR_ENTRY, (void*) buf);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the manufacturer.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents 
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbManufacturerGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  L7_ushort16 value;
  sysapiRegistryGet(MFGR, U16_ENTRY, (void*)&value);
  sprintf(buf,"0x%04x",value);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the software version.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSwVersionGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  return sysapiRegistryGet(SW_VERSION, STR_ENTRY, (void*) buf);
}
/*********************************************************************
*
* @purpose Get the hardware version.
*
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbHwVersionGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  return sysapiRegistryGet(HW_VERSION, STR_ENTRY, (void*) buf);
}
/*********************************************************************
*
* @purpose Get the operating system.
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbOperSysGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  return sysapiRegistryGet(OS, STR_ENTRY, (void*) buf);
}

/*********************************************************************
*
* @purpose Get the NPD - Network Processing Device Version
*          
* @param UnitIndex  the unit for this operation
* @param *buf       ptr to buf contents
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbNPDGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  return sysapiRegistryGet(NPD_TYPE_STRING, STR_ENTRY, (void*) buf);
}
/*********************************************************************
*
* @purpose Get information on which slots are populated.
*          
* @param UnitIndex  the unit for this operation
* @param *slotPop   array of length L7_MAX_SLOTS_PER_UNIT
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes The slotPop[] array will be filled in with L7_TRUE or L7_FALSE
* @notes slotPop[n] = L7_TRUE if slot n is populated with a feature card
* @notes slotPop[n] = L7_FALSE if slot n is not populated 
* @notes The slot numbers are 0-based, ranging from 0 to L7_MAX_SLOTS_PER_UNIT-1
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSlotsPopulatedGet(L7_uint32 UnitIndex, L7_uint32 *slotPop)
{
  L7_uint32 rc, i, status;

  for ( i=0; i < L7_MAX_SLOTS_PER_UNIT; i++ )
  {
    rc = cmgrCardStatusGet(UnitIndex,i,&status);

    slotPop[i] = L7_FALSE;
    if(rc == L7_SUCCESS)
    {
      if((L7_CARD_STATUS_PLUG == status) ||
         (L7_CARD_STATUS_WORKING == status))
         slotPop[i] = L7_TRUE;
	}
  }

  return(L7_SUCCESS);
}


/*********************************************************************
*
* @purpose Get the number of ports in the specified slot
*          
* @param UnitIndex  the unit for this operation
* @param slot       slot number (0 to L7_MAX_SLOTS_PER_UNIT-1)
* @param *val       number of ports in the specified slot
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
* @returns L7_NOT_EXISTS, if the specified slot is not populated
*
* @notes The slotPop[] array will be filled in with L7_TRUE or L7_FALSE
* @notes slotPop[n] = L7_TRUE if slot n is populated with a feature card
* @notes slotPop[n] = L7_FALSE if slot n is not populated 
* @notes The slot numbers are 0-based, ranging from 0 to L7_MAX_SLOTS_PER_UNIT-1
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPortsPerSlotGet(L7_uint32 UnitIndex, L7_uint32 slot, L7_uint32 *val)
{
  L7_uint32 rc=L7_SUCCESS;

  if (cmgrCardNumPortsGet(UnitIndex,slot,val) == L7_SUCCESS)
    return(L7_SUCCESS);
  else if (rc == L7_NOT_EXIST)
    return(L7_NOT_EXISTS);

  return(L7_FAILURE);
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rdisc_config.c
*
* @purpose   To discover router protocols
*
* @component
*
* @comments  none
*
* @create    11/02/2001
*
* @author    ksdesai
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include <string.h>

#include "l7_common.h"
#include "l7_common_l3.h"
#include "log.h"
#include "osapi.h"
#include "nvstoreapi.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "l3_default_cnfgr.h"
#include "rtrdisc.h"
#include "rtrdisc_api.h"
#include "rtrdisc_config.h"
#include "rtrdisc_util.h"
#include "rtrdisc_cnfgr.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "defaultconfig.h"
#include "sysapi.h"

rtrDiscCfgData_t *rtrDiscCfgData  = L7_NULLPTR;
rtrDiscIntf_t    *rtrDiscIntf     = L7_NULLPTR;
L7_uint32        *rtrDiscMapTbl   = L7_NULLPTR;
osapiTimerDescr_t *pRtrDiscTimerHolder   = L7_NULLPTR;

osapiRWLock_t    rtrDiscIntfRWLock;

extern rtrDiscCnfgrState_t rtrDiscCnfgrState;


static void rtrDiscIntfBuildTestConfigData(rtrDiscIntfCfgData_t *pCfg, L7_uint32 seed);
static void rtrDiscCfgShow(void);


/*********************************************************************
* @purpose  Print the current router discovery config values to
*           serial port
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    prints out info for all internal interfaces that are present
*
* @end
*********************************************************************/
L7_RC_t rtrDiscConfigDump()
{
  L7_RC_t rtrDiscRC=L7_SUCCESS;
  L7_char8 buf[32];
  L7_uint32 intIfNum;
  rtrDiscIntfCfgData_t *pCfg;


  for (intIfNum = 1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
  {
    if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
      continue;
    else
    {
      osapiInetNtoa(pCfg->advAddress, buf);
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\nAdv Address (%d) - %s\n",intIfNum,buf);
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Max Adv Interval (%d) - %d\n",intIfNum,pCfg->maxAdvInt);
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Min Adv Interval (%d) - %d\n",intIfNum,pCfg->minAdvInt);
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Adv Lifetime (%d) - %d\n",intIfNum,pCfg->advLifetime);
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Advertise (%d) - %s\n",intIfNum,(pCfg->advertise)?"L7_TRUE":"L7_FALSE");
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Preference Level (%d) - %d\n",intIfNum,pCfg->preferenceLevel);
    }
  }
  return rtrDiscRC;
}


/*********************************************************************
* @purpose  Build default router discovery data
*
* @param    ver   @b{(input)} Software version of Discovery Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 i;

  /* Build Config File Header */
  memset((void*)&rtrDiscCfgData->cfgHdr, 0, sizeof(L7_fileHdr_t));
  strcpy(rtrDiscCfgData->cfgHdr.filename, L7_RTR_DISC_FILENAME);
  rtrDiscCfgData->cfgHdr.version       = ver;
  rtrDiscCfgData->cfgHdr.componentID   = L7_RTR_DISC_COMPONENT_ID;
  rtrDiscCfgData->cfgHdr.type          = L7_CFG_DATA;
  rtrDiscCfgData->cfgHdr.length        = sizeof(rtrDiscCfgData_t);
  rtrDiscCfgData->cfgHdr.dataChanged   = L7_FALSE;
  rtrDiscCfgData->cfgHdr.version       = ver;

  /*-------------------------------------*/
  /* Build Default Router Discovery Data */
  /*-------------------------------------*/


  /* Router Discovery parameters */
  for (i = 1; i < L7_RTR_DISC_INTF_MAX_COUNT; i++)
    rtrDiscIntfBuildDefaultConfigData(&rtrDiscCfgData->rtrDiscIntfCfgData[i]);
}

/*********************************************************************
* @purpose  Build default router discovery data for the interface
*
* @param    pCfg        pointer to intf configuration information
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscIntfBuildDefaultConfigData(rtrDiscIntfCfgData_t *pCfg)
{
  pCfg->advAddress = L7_DEFAULT_ADV_ADDR;
  pCfg->maxAdvInt = MAX_ADV_INTERVAL;
  pCfg->minAdvInt = MIN_ADV_INTERVAL;
  pCfg->advLifetime = ADV_LIFETIME;
  pCfg->advertise = ADVERTISEMENT;
  pCfg->preferenceLevel = PREFERENCE_LEVEL;
}


/*********************************************************************
* @purpose  Save the router discovery data
*
* @param    ver   @b{(input)} Software version of Discovery Data
*
* @returns  void
*
* @notes    Never called since all routing builds use text based
*           persistent configuration.
*
* @end
*********************************************************************/
L7_RC_t rtrDiscSave(void)
{
  if (rtrDiscCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    rtrDiscCfgData->cfgHdr.dataChanged = L7_FALSE;
    rtrDiscCfgData->checkSum = nvStoreCrc32((L7_char8 *)rtrDiscCfgData,
                                            sizeof(rtrDiscCfgData_t) - sizeof(rtrDiscCfgData->checkSum));

    /* call save NVStore routine */
    if (sysapiCfgFileWrite(L7_RTR_DISC_COMPONENT_ID, L7_RTR_DISC_FILENAME,
                           (L7_char8 *)rtrDiscCfgData, sizeof (rtrDiscCfgData_t)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_RTR_DISC_COMPONENT_ID,
             "Failed to save router discovery configuration.");

      return L7_ERROR;
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Restore the router discovery data
*
* @param    ver   @b{(input)} Software version of Discovery Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscRestoreProcess(void)
{

    rtrDiscBuildDefaultConfigData(L7_RTR_DISC_CFG_VER_CURRENT);
    rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;
    memset(rtrDiscMapTbl, 0, sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);

    (void)rtrDiscApplyConfigData();

    return;
}

/*********************************************************************
* @purpose  Checks if router discovery user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL rtrDiscHasDataChanged(void)
{
  return rtrDiscCfgData->cfgHdr.dataChanged;
}
void rtrDiscResetDataChanged(void)
{
  rtrDiscCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}


/*********************************************************************
* @purpose  Applies router discovery config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rtrDiscApplyConfigData (void)
{
  L7_RC_t   rtrDiscRC = L7_SUCCESS;
  L7_uint32 intIfNum;
  L7_uint32 cfgIndex;

  if (RTR_DISC_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_RTR_DISC_INTF_MAX_COUNT; cfgIndex++)
    {

      if (nimIntIfFromConfigIDGet(&(rtrDiscCfgData->rtrDiscIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
      if (rtrDiscApplyIntfConfigData(intIfNum) != L7_SUCCESS)
      {
        rtrDiscRC = L7_FAILURE;
        return rtrDiscRC;
      }
    }
  }
  return rtrDiscRC;
}

/*********************************************************************
* @purpose  Applies interface router discovery config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rtrDiscApplyIntfConfigData(L7_uint32 intIfNum)
{
  rtrDiscIntfCfgData_t *pCfg;

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (rtrDiscAdvertiseSet(intIfNum, pCfg->advertise) != L7_SUCCESS)
      return L7_FAILURE;

    if (rtrDiscAdvAddrSet(intIfNum, pCfg->advAddress) != L7_SUCCESS)
      return L7_FAILURE;

    if (rtrDiscMaxAdvIntervalSet(intIfNum, pCfg->maxAdvInt) != L7_SUCCESS)
      return L7_FAILURE;

    if (rtrDiscMinAdvIntervalSet(intIfNum, pCfg->minAdvInt) != L7_SUCCESS)
      return L7_FAILURE;

    if (rtrDiscAdvLifetimeSet(intIfNum, pCfg->advLifetime) != L7_SUCCESS)
      return L7_FAILURE;

    if (rtrDiscPreferenceLevelSet(intIfNum, pCfg->preferenceLevel) != L7_SUCCESS)
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL rtrDiscMapIntfIsConfigurable(L7_uint32 intIfNum, rtrDiscIntfCfgData_t **pCfg)
{
  L7_uint32 index = 0;

  if (RTR_DISC_IS_READY != L7_TRUE)
  {
      return L7_FALSE;
  }

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  if (rtrDiscIntfIsValid(intIfNum) == L7_FALSE)
    return L7_FALSE;

  index = rtrDiscMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;

  if(pCfg != L7_NULLPTR){
      *pCfg = &rtrDiscCfgData->rtrDiscIntfCfgData[index];
  }

  return L7_TRUE;
}



/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/

/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void rtrDiscBuildTestConfigData(void)
{
  L7_uint32 i;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

  /* NOTE FOR WRITING TEST CONFIG ROUTINES!!!!
     The index of the loop is used as a seed to ensure that each array entry is
     populated with a unique value */

  for (i = 1; i < L7_RTR_DISC_INTF_MAX_COUNT; i++)
    rtrDiscIntfBuildTestConfigData(&rtrDiscCfgData->rtrDiscIntfCfgData[i], i);



 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}



 /*********************************************************************
* @purpose  Build default router discovery data for the interface
*
* @param    pCfg        pointer to intf configuration information
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscIntfBuildTestConfigData(rtrDiscIntfCfgData_t *pCfg, L7_uint32 seed)
{
  /* NOTE FOR WRITING DEBUG TEST FUNCTIONS:

     Ensure at least one entry in the array is affected by the "seed" to ensure
     that we can uniquely determine that each separate entry has migrated
     appropriately. */

  pCfg->advAddress = 0x0A0A0A01 + seed;
  pCfg->maxAdvInt = 750;
  pCfg->minAdvInt = 850;
  pCfg->advLifetime = 950;
  pCfg->advertise = L7_FALSE;
  pCfg->preferenceLevel = 3;
}





/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void rtrDiscCfgShow(void)
{

    nimUSP_t    usp;
    L7_uint32   i;
    L7_uint32   intIfNum;


   /*--------------------------------*/
   /* cfgParms                       */
   /*                                */
   /* DO A RAW DUMP OF THE CONFIG    */
   /* FILE IF A PREVIOUSLY EXISTING  */
   /* DEBUG DUMP ROUTINE DOES NOT    */
   /* EXIST                          */
   /*--------------------------------*/

    /* Print interface information, if any */

    sysapiPrintf( "Printing interface data\n");
    sysapiPrintf( "-----------------------\n");
    for (i = 1; i < L7_RTR_DISC_INTF_MAX_COUNT; i++)
    {
        /* FOR INTERFACE STRUCTURES,  ALWAYS PRINT USP as well as intIfNUm
           TO ENSURE PROPER INSTANCE OF CONFIG MIGRATED */

        intIfNum = 0;
        memset((void *)&usp, 0, sizeof(nimUSP_t));

        if (nimIntIfFromConfigIDGet(&rtrDiscCfgData->rtrDiscIntfCfgData[i].configId,
                                    &intIfNum) == L7_SUCCESS)
        {
            if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
            {
                sysapiPrintf( "USP = %d/%d/%d  (intIfNum  = %d \n",
                               usp.unit, usp.slot, usp.port, intIfNum);
            }
            else
            {

                SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n%50/0/0d");
                sysapiPrintf( "USP = 0/0/0  (intIfNum  = %d \n",  intIfNum);

            }
        }

        /* PRINT REST OF INTERFACE CONFIGURATION */

        sysapiPrintf("rtrDiscCfgData->rtrDiscIntfCfgData[i].advAddress = %d\n",
                     rtrDiscCfgData->rtrDiscIntfCfgData[i].advAddress);

        sysapiPrintf("rtrDiscCfgData->rtrDiscIntfCfgData[i].maxAdvInt = %d\n",
                     rtrDiscCfgData->rtrDiscIntfCfgData[i].maxAdvInt);

        sysapiPrintf("rtrDiscCfgData->rtrDiscIntfCfgData[i].minAdvInt = %d\n",
                     rtrDiscCfgData->rtrDiscIntfCfgData[i].minAdvInt);

        sysapiPrintf("rtrDiscCfgData->rtrDiscIntfCfgData[i].advLifetime = %d\n",
                     rtrDiscCfgData->rtrDiscIntfCfgData[i].advLifetime);

        sysapiPrintf("rtrDiscCfgData->rtrDiscIntfCfgData[i].advertise = %d\n",
                     rtrDiscCfgData->rtrDiscIntfCfgData[i].advertise);

        sysapiPrintf("rtrDiscCfgData->rtrDiscIntfCfgData[i].preferenceLevel = %d\n",
                     rtrDiscCfgData->rtrDiscIntfCfgData[i].preferenceLevel);

        sysapiPrintf("\n");

    }


}



/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void rtrDiscConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(rtrDiscCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*--------------------------------*/
   /* cfgParms                       */
   /*--------------------------------*/

    rtrDiscCfgShow();


    /*-------------------------------*/
    /* Scaling Constants             */
    /*                               */
    /*                               */
    /* NOTE:                         */
    /* Print any limits of arrays    */
    /* or anything else in the       */
    /* confi files which might       */
    /* affect the size of the config */
    /* file.                         */
    /*                               */
    /*-------------------------------*/

    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_RTR_DISC_INTF_MAX_COUNT - %d\n", L7_RTR_DISC_INTF_MAX_COUNT);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("rtrDiscCfgData->checkSum : %u\n", rtrDiscCfgData->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/




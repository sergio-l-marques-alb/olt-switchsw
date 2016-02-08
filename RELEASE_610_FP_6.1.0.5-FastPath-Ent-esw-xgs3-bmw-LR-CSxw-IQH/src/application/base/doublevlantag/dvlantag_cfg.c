/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvlantag_cfg.c
*
* @purpose   Double Vlan Tagging configuration file
*
* @component DvlanTag
*
* @comments 
*
* @create    8/14/2003
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#include "dvlantag_include.h"

DVLANTAG_CFG_t *dvlantagCfg = L7_NULLPTR;
DVLANTAG_PORT_t *dvlantagPort = L7_NULLPTR;
L7_uint32 *dvlantagMapTbl = L7_NULLPTR;

extern dvlantagCnfgrState_t dvlantagCnfgrState;
extern L7_uint32 dvlantagDeviceEthertype; 


static void dvlantagBuildTestIntfConfigData(DVLANTAG_INTF_CFG_t *pCfg, L7_uint32 seed);

/*********************************************************************
* @purpose  Allocate memory for the cfg file and the port structure
*          
* @param    none 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments This also memsets the memory allocated to 0x00
*
* @end
*********************************************************************/
L7_RC_t dtagMemoryMalloc()
{
  dvlantagCfg = (DVLANTAG_CFG_t *)osapiMalloc(L7_DVLANTAG_COMPONENT_ID, (L7_uint32)sizeof(DVLANTAG_CFG_t));
  dvlantagPort = (DVLANTAG_PORT_t *)osapiMalloc(L7_DVLANTAG_COMPONENT_ID, (L7_uint32)sizeof(DVLANTAG_PORT_t) * platIntfMaxCountGet());
  dvlantagMapTbl = osapiMalloc(L7_DVLANTAG_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());

  if (dvlantagCfg == L7_NULLPTR || dvlantagMapTbl == L7_NULLPTR || dvlantagPort == L7_NULLPTR)
  {
    dtagMemoryFree();
    return L7_FAILURE;
  }
  /*memset all memory to zero*/
  memset((void *)dvlantagCfg, 0x00, (L7_uint32)sizeof(DVLANTAG_CFG_t));
  memset((void *)dvlantagPort, 0x00, sizeof(DVLANTAG_PORT_t) * platIntfMaxCountGet());
  memset((void *)dvlantagMapTbl, 0x00, sizeof(L7_uint32) * platIntfMaxCountGet());
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Frees the memory for the cfg file and port structure
*          
* @param    none 
*
* @returns  void  
*
* @comments 
*
* @end
*********************************************************************/
void dtagMemoryFree()
{
  if (dvlantagCfg  != L7_NULLPTR)
  {
    osapiFree(L7_DVLANTAG_COMPONENT_ID, dvlantagCfg);
    dvlantagCfg = L7_NULLPTR;
  }

  if (dvlantagPort  != L7_NULLPTR)
  {
    osapiFree(L7_DVLANTAG_COMPONENT_ID, dvlantagPort);
    dvlantagPort = L7_NULLPTR;
  }

  if (dvlantagMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_DVLANTAG_COMPONENT_ID, dvlantagMapTbl);
    dvlantagMapTbl = L7_NULLPTR;
  }
  return;
}
/*********************************************************************
* @purpose  Builds the cfg file with default parameter values
*          
* @param    ver @((input)) Software version of Config Data
*
* @returns  void  
*
* @comments updates the port structure too.
*
* @end
*********************************************************************/
void dvlantagBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId[L7_DVLANTAG_MAX_INTERFACE_COUNT];

  /* Save the config IDs */
  memset((void *)&configId[0], 0, sizeof(nimConfigID_t) * L7_DVLANTAG_MAX_INTERFACE_COUNT);
  for (cfgIndex = 1; cfgIndex < L7_DVLANTAG_MAX_INTERFACE_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &dvlantagCfg->intfCfg[cfgIndex].configId);

  memset((void *)dvlantagCfg, 0, sizeof(DVLANTAG_CFG_t));

  for (cfgIndex = 1; cfgIndex < L7_DVLANTAG_MAX_INTERFACE_COUNT; cfgIndex++)
  {
    dvlantagBuildDefaultIntfConfigData(&configId[cfgIndex], &dvlantagCfg->intfCfg[cfgIndex]);
  }

  dvlantagDeviceEthertype = L7_DVLANTAG_DEFAULT_ETHERTYPE;
  dvlantagCfg->etherType[0] = L7_DVLANTAG_DEFAULT_ETHERTYPE;

  strcpy((L7_char8 *)dvlantagCfg->cfgHdr.filename, DVLANTAG_CFG_FILENAME);

  dvlantagCfg->cfgHdr.version = ver;

  dvlantagCfg->cfgHdr.componentID = L7_DVLANTAG_COMPONENT_ID;

  dvlantagCfg->cfgHdr.type = L7_CFG_DATA;

  dvlantagCfg->cfgHdr.length = (L7_uint32)sizeof(DVLANTAG_CFG_t);

  dvlantagCfg->cfgHdr.dataChanged = L7_FALSE;

  return;
}

/*********************************************************************
* @purpose  Build default dvlantag interface config data  
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void dvlantagBuildDefaultIntfConfigData(nimConfigID_t *configId, DVLANTAG_INTF_CFG_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  pCfg->custId = L7_DVLANTAG_DEFAULT_CUSTID;
  pCfg->etherType[0] = L7_DVLANTAG_DEFAULT_ETHERTYPE;
  pCfg->mode = L7_DVLANTAG_DEFAULT_PORT_MODE;

  return;
}

/*********************************************************************
* @purpose  Dumps the cfg file
*          
* @param    none 
*
* @returns  void
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dvlantagCfgDump()
{
  L7_uint32 intIfNum, extIfNum, i;
  nimConfigID_t configIdNull;
  DVLANTAG_INTF_CFG_t *pCfg;

                      
  for (i = 1; i < L7_DVLANTAG_MAX_INTERFACE_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&dvlantagCfg->intfCfg[i].configId, &configIdNull))
      continue;
    if (nimIntIfFromConfigIDGet(&(dvlantagCfg->intfCfg[i].configId), &intIfNum) != L7_SUCCESS)
      continue;
    if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
      {
        printf("extIfNum:  %d (0x%x)\n", extIfNum, extIfNum);
        printf("mode:      %d (0x%x)\n", dvlantagCfg->intfCfg[i].mode, dvlantagCfg->intfCfg[i].mode);
        printf("etherType: %d (0x%x)\n", dvlantagCfg->intfCfg[i].etherType[0], dvlantagCfg->intfCfg[i].etherType[0]);
        printf("custId:    %d (0x%x)\n", dvlantagCfg->intfCfg[i].custId, dvlantagCfg->intfCfg[i].custId);
        printf("\n");
      }
    }
  }
  printf("\n");

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Has the data in the cfg file changed
*          
* @param    none 
*
* @returns  L7_TRUE  
* @returns  L7_FALSE  
*
* @comments 
*
* @end
*********************************************************************/
L7_BOOL dvlantagHasDataChanged()
{
  return dvlantagCfg->cfgHdr.dataChanged;
}
void dvlantagResetDataChanged(void)
{
  dvlantagCfg->cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Saves the current configuration
*          
* @param    none 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dvlantagSave()
{
  L7_RC_t rc = L7_SUCCESS;

  if (dvlantagCfg->cfgHdr.dataChanged == L7_TRUE)
  {
	dvlantagCfg->cfgHdr.dataChanged = L7_FALSE;
	dvlantagCfg->checkSum = nvStoreCrc32((L7_char8 *)dvlantagCfg,
										 ((L7_uint32)sizeof(DVLANTAG_CFG_t) - 
										  (L7_uint32)sizeof(dvlantagCfg->checkSum)));

  if ( (rc = sysapiCfgFileWrite(L7_DVLANTAG_COMPONENT_ID, DVLANTAG_CFG_FILENAME,
                                (L7_char8 *)dvlantagCfg,
                                (L7_int32)sizeof(DVLANTAG_CFG_t))) != L7_SUCCESS )
  {
    rc = L7_ERROR;
	  LOG_MSG("Dvlantag failed to save configuration");
  }

  }
  return rc;
}
/*********************************************************************
* @purpose  Applies the configured saved configuration
*          
* @param    none 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dvlantagApplyConfigData()
{
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  /* Now check for ports that are valid for dvlan tagging.  All non-zero values in the
   * config structure contain a port config ID that is valid for dvlan tagging.
   * Convert this to internal interface number and apply the port config.
   * The internal interface number is used as an index into the operational
   * interface data.
   */
  if (DVLANTAG_IS_READY)
  {
    memset(&configIdNull, 0, sizeof(nimConfigID_t));

    for (cfgIndex = 1; cfgIndex < L7_DVLANTAG_MAX_INTERFACE_COUNT; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&dvlantagCfg->intfCfg[cfgIndex].configId, &configIdNull))
        continue;
      if (nimIntIfFromConfigIDGet(&(dvlantagCfg->intfCfg[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
      if (dvlantagApplyIntfConfigData(intIfNum) != L7_SUCCESS)
      {
        LOG_MSG("dvlantagApplyConfigData: Failure in applying config data for interface %u\n", intIfNum);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies the configured saved configuration for the specified interface
*          
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dvlantagApplyIntfConfigData(L7_uint32 intIfNum)
{
  DVLANTAG_INTF_CFG_t *pCfg;
  L7_uint32 cfgIndex = dvlantagMapTbl[intIfNum];
  L7_uint32 tmp;

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_SUCCESS;

  if ( (dvlantagCfg->intfCfg[cfgIndex].mode == L7_ENABLE) &&
       (dvlantagIntfIsAcquired(intIfNum) == L7_FALSE) )
  {
    (void)dvlantagIntfModeApply(intIfNum, L7_ENABLE);

    for (tmp = 1; tmp < L7_DVLANTAG_MAX_TPIDS; tmp++)
    {
      if (pCfg->etherType[tmp])
      {
        (void) dvlantagIntfEthertypeApply (intIfNum,
                                           pCfg->etherType[tmp],
                                           L7_ENABLE);
      }
    }
  }
  else
  {
    (void)dvlantagIntfModeApply(intIfNum, L7_DISABLE);
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Remove any configured Double vlan tagging interface
*          
* @param    none 
*
* @returns  void
*
* @comments 
*
* @end
*********************************************************************/
void dvlantagCfgRemove()
{
  L7_uint32 cfgIndex, intIfNum;

  for (cfgIndex = 1; cfgIndex < L7_DVLANTAG_MAX_INTERFACE_COUNT; cfgIndex++)
  {
    if (dvlantagCfg->intfCfg[cfgIndex].mode == L7_ENABLE)
    {
      if (nimIntIfFromConfigIDGet(&(dvlantagCfg->intfCfg[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
      dvlantagCfg->etherType[0] = 0x8100;
      (void)dvlantagIntfModeSet(intIfNum, L7_DISABLE);
    }
  }
  return;
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
void dvlantagBuildTestConfigData(void)
{
    L7_uint32 cfgIndex;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

  /* NOTE FOR WRITING TEST CONFIG ROUTINES!!!!  
     The index of the loop is used as a seed to ensure that each array entry is
     populated with a unique value */
  

  for (cfgIndex = 1; cfgIndex < L7_DVLANTAG_MAX_INTERFACE_COUNT; cfgIndex++)
  {
    dvlantagBuildTestIntfConfigData(&dvlantagCfg->intfCfg[cfgIndex], cfgIndex);
  }
        
 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   dvlantagCfg->cfgHdr.dataChanged = L7_TRUE;  
   sysapiPrintf("Built test config data\n");


}



/*********************************************************************
* @purpose  Build default dvlantag interface config data  
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void dvlantagBuildTestIntfConfigData(DVLANTAG_INTF_CFG_t *pCfg, L7_uint32 seed)
{

  pCfg->custId = L7_DVLANTAG_DEFAULT_CUSTID + seed ;
  pCfg->etherType[0] = L7_DVLANTAG_DEFAULT_ETHERTYPE + seed;
  pCfg->mode = L7_DVLANTAG_DEFAULT_PORT_MODE + seed ;

  return;
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
void dvlantagConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(dvlantagCfg->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*--------------------------------*/
   /* cfgParms                       */
   /*--------------------------------*/

    dvlantagCfgDump();


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


    sysapiPrintf( "L7_DVLANTAG_MAX_INTERFACE_COUNT - %d\n", L7_DVLANTAG_MAX_INTERFACE_COUNT);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("dvlantagCfg->checkSum : %u\n", dvlantagCfg->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/




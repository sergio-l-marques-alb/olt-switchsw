/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    mirror_debug.c
* @purpose     Port mirroring debug routines
* @component   mirroring
* @comments    none
* @create      02/18/2004
* @author      harvinder 
* @end
*
**********************************************************************/

#include <string.h>

#include "l7_common.h"
#include "nimapi.h"
#include "mirror_cfg.h"
#include "mirror_api.h"
#include "cnfgr.h"

extern mirrorCfgData_t   *mirrorCfgData;
extern mirrorSessionInfo_t mirrorInfo[];
extern L7_uint32 intfList[L7_MAX_INTERFACE_COUNT];
extern void *intfListSema;

static void mirrorBuildRandomIntfMask (L7_INTF_MASK_t *pMask, L7_uint32 seed);

L7_uint32 mirrorConfigTraceModeGet(void);


/*********************************************************************
* @purpose  Print the current mirror config values to
*           serial port
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mirrorConfigDump(void)
{
  L7_RC_t rc=L7_SUCCESS;
  nimUSP_t srcUSP,dstUSP;
  L7_uint32 numPorts, portIndex;
  L7_uint32 sessionCount, sessionIndex;    
  L7_MIRROR_DIRECTION_t tempMirrorType; 


  /* take semaphore */
  osapiSemaTake(intfListSema,L7_NO_WAIT);

  sessionCount = mirrorSessionCountGet();
  for(sessionIndex = 0; sessionIndex < sessionCount; sessionIndex++ )
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n\nConfiguration of session %u\n", sessionIndex + 1);

    
    nimMaskToList((NIM_INTF_MASK_t *)&(mirrorCfgData->sessionData[sessionIndex].srcPorts),intfList,&numPorts);

    if (numPorts > L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION)
    {
     /* should never get here */
     SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n\n mirrorConfigDump:: Error!!\
                     Number of source interface : %d is greater than \
                     Maximum number of source interfaces allowed : %d.\n",
                     numPorts,L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION) ;
    }

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\nSource Ports %s",numPorts ? "": "is not set");
 
    for(portIndex = 0; portIndex < numPorts; portIndex++)
    {
      if (nimGetUnitSlotPort(intfList[portIndex],&srcUSP) == L7_SUCCESS)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n%u.%u.%u", srcUSP.unit, srcUSP.slot, srcUSP.port);

        /* add details of Probe type for each source port*/
        if((cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
                                  L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID)) == L7_TRUE)
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " \nProbe Type is ");
          if(mirrorSourcePortDirectionGet(sessionIndex+1,intfList[portIndex],&tempMirrorType) != L7_SUCCESS)
          {
            /* should never get here */
            continue; 
          }

          switch(tempMirrorType)
          {
          case L7_MIRROR_UNCONFIGURED: 
              /* error -- should never get here . If source port is configured then, 
              one of the other directions must be configured */
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Error!!! Direction unconfigured");
              break;
            case L7_MIRROR_BIDIRECTIONAL:
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "BIDIRECTIONAL");
              break;
            case L7_MIRROR_INGRESS:
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "INGRESS");
              break;
            case L7_MIRROR_EGRESS:
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "EGRESS");
              break;
          } /* switch*/
          tempMirrorType = L7_MIRROR_BIDIRECTIONAL;

        }/*if feature,L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID  present */

      }
    }

    
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " \nDestination Port is ");  

    if (nimUspFromConfigIDGet(&mirrorCfgData->sessionData[sessionIndex].destID,
                              &dstUSP) == L7_SUCCESS)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," %u.%u.%u", dstUSP.unit, dstUSP.slot, dstUSP.port);
    }
    else
    { 
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," not set");
    }

       
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " \nStatus : %s",
             mirrorCfgData->sessionData[sessionIndex].mode? "ENABLE" : "DISABLE");

   /*SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " \n\nOperation data ");
   SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " \n\nStatus : ");

   if (mirrorInfo[sessionIndex].mode == L7_ENABLE)
   {
     SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s","enable");
   }
   else 
   {
     SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s","disable");
   } */
  
  } /* End of for loop */

  /* release semaphore */
    osapiSemaGive(intfListSema);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n\nFile Header Data\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\nFile Name: %s",mirrorCfgData->cfgHdr.filename);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\nVersion: %d",mirrorCfgData->cfgHdr.version);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\nComponent ID: %d",mirrorCfgData->cfgHdr.componentID);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\nFile Size: %d",mirrorCfgData->cfgHdr.length);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\nData Changed: %s", mirrorCfgData->cfgHdr.dataChanged ? "Yes" : "No");
  
  return(rc);
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
void mirrorBuildTestConfigData(void)
{

  L7_uint32 firstIntIfNum;
  L7_uint32 sessionIndex; 
  L7_uint32 numSrcIntf=0, i=0;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
  /* take semaphore */
  osapiSemaTake(intfListSema,L7_NO_WAIT);


  for(sessionIndex = 1; sessionIndex <= L7_MIRRORING_MAX_SESSIONS; sessionIndex++)
  {
    mirrorCfgData->sessionData[sessionIndex - 1].mode = L7_DISABLE; 
    if (sessionIndex%2 == 0) 
    {
        mirrorCfgData->sessionData[sessionIndex - 1].mode = L7_ENABLE; 
    }
    
    mirrorBuildRandomIntfMask (&mirrorCfgData->sessionData[sessionIndex-1].srcPorts,sessionIndex);

    /* For the destination port, ensure that it is not also a source port */

    if (nimFirstValidIntfNumberByType(L7_PHYSICAL_INTF, &firstIntIfNum) == L7_SUCCESS)
    {
        L7_INTF_CLRMASKBIT( mirrorCfgData->sessionData[sessionIndex-1].srcPorts, firstIntIfNum);    
        (void)nimConfigIdGet(firstIntIfNum, &mirrorCfgData->sessionData[sessionIndex-1].destID);
    }

    /* add direction per source interface */ 
    nimMaskToList((NIM_INTF_MASK_t *)&mirrorCfgData->sessionData[sessionIndex-1].srcPorts, intfList, 
                &(numSrcIntf));
    for (i = 0; i < numSrcIntf; i++ )

    {
      L7_INTF_SETMASKBIT(mirrorCfgData->sessionData[sessionIndex-1].srcIngressMirrorType,intfList[i]);
      L7_INTF_SETMASKBIT(mirrorCfgData->sessionData[sessionIndex-1].srcEgressMirrorType,intfList[i]);
    } 

    
  }

 /* End of Component's Test Non-default configuration Data */

  /*release semaphore */
  osapiSemaGive(intfListSema);

  /* enable debug trace*/
  mirrorCfgData->tracemode = L7_ENABLE;


   /* Force write of config file */
   mirrorCfgData->cfgHdr.dataChanged = L7_TRUE;  
     sysapiPrintf("Built test config data\n");


}



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
void mirrorBuildRandomIntfMask (L7_INTF_MASK_t *pMask, L7_uint32 seed)
{

    L7_uint32  j, k;
    L7_uint32 min, max; 
    L7_INTF_MASK_t temp;


    memcpy(&temp, pMask, L7_INTF_INDICES);

    /* For test purposes, only turn on a subset of eligible ports */

    k =  seed;  /*  seed to randomize mask */

    
    (void) nimIntIfNumRangeGet(L7_PHYSICAL_INTF, &min, &max);

    /* Reset the maximum source ports set to maximum source ports per session */
    max = L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION;

    for (j = 1; j <= max; j++)
    {
 
      /* Physical ports are the highest intIfNums eligible to participate in mirroring */

      /*  Force each group to have a different port list range */
      if (j < (2*k) ) 
      {
          continue;
      }


      /*  Force each group to have a different port list range */
      if (j > (4*k) ) 
      {
          continue;
      }

      L7_INTF_SETMASKBIT( (L7_INTF_MASK_t) temp, j);    
      
      k++;
    }


    memcpy(pMask, &temp, L7_INTF_INDICES);
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
void mirrorConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;
    
    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(mirrorCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    mirrorConfigDump();

    sysapiPrintf( "\nScaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_MIRRORING_MAX_SESSIONS - %d\n", L7_MIRRORING_MAX_SESSIONS);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("mirrorCfgData->checkSum : %u\n", mirrorCfgData->checkSum);

    /*------------------------------------*/
    /* Port Mirroring TraceMode           */
    /*------------------------------------*/

    if (mirrorConfigTraceModeGet() == L7_ENABLE)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\nMirror Trace Mode:Enable\n");
    }
    else 
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\nMirror Trace Mode:Disable\n");
    }



}

 
/*********************************************************************
* @purpose  Set Port Mirroring tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void mirrorConfigTraceModeSet(L7_uint32 mode)
{
  mirrorCfgData->tracemode = mode; 
  mirrorCfgData->cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Get Port Mirroring tracing mode
*
* @param    none
*
* @returns  L7_DISABLE - trace mode is disabled 
*           L7_ENABLE - trace mode is enabled 
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 mirrorConfigTraceModeGet(void)
{
  return (mirrorCfgData->tracemode); 
  
}

/*********************************************************************
* @purpose  Print valid source ports
*
* @param    none
*
* @returns none 
*
* @notes
*
* @end
*********************************************************************/
void mirrorDebugValidSrcIntfList(void)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 intCurrIntf=0,intPrevIntf=0;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n Valid Source Interfaces.\n");

  rc=mirrorValidSrcIntfFirstGet(&intCurrIntf);
  while(rc==L7_SUCCESS)
  {
    if(intCurrIntf !=0)
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n %d",intCurrIntf); 
    intPrevIntf=intCurrIntf;
    rc=mirrorValidSrcIntfNextGet(intPrevIntf,&intCurrIntf);

  }
}

/*********************************************************************
* @purpose  Print valid Destination ports
*
* @param    none
*
* @returns none 
*
* @notes
*
* @end
*********************************************************************/
void mirrorDebugValidDestIntfList(void)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 intCurrIntf=0,intPrevIntf=0;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n Valid Destination Interfaces.\n");

  rc=mirrorValidDestIntfFirstGet(&intCurrIntf);
  while(rc==L7_SUCCESS)
  {
    if(intCurrIntf !=0)
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n %d",intCurrIntf); 
    intPrevIntf=intCurrIntf;
    rc=mirrorValidDestIntfNextGet(intPrevIntf,&intCurrIntf);

  }
}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/



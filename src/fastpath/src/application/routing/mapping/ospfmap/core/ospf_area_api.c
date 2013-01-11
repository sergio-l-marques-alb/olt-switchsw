/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ospf_area_api.c
*
* @purpose   Ospf Area Api functions
*
* @component Ospf Mapping Layer
*
* @comments  This file includes getters & setters for the ospf area
*            & stub area tables. (rfc 1850)
*
* @create    07/10/2001
*
* @author    anayar
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#include "l7_ospfinclude.h"
#include "l7_ospf_api.h"

extern L7_ospfMapCfg_t    *pOspfMapCfgData;
extern ospfInfo_t         *pOspfInfo;
extern ospfAreaInfo_t     *pOspfAreaInfo;
extern ospfIntfInfo_t     *pOspfIntfInfo;
extern L7_uint32 tosToTosIndex[L7_RTR_MAX_TOS];
extern L7_uint32 tosIndexToTos[L7_RTR_MAX_TOS_INDICES];

/*
**********************************************************************
*                    API FUNCTIONS  -  AREA CONFIG
**********************************************************************
*/

/*--------------------------------------------------------------------------*/
/* The OSPF Area Data Structure contains information regarding the various  */
/* areas. The interfaces and virtual links are configured as part of these  */
/* areas. Area 0.0.0.0, by definition, is the backbone Area.                */
/*--------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Creates an ospf area
*
* @param    areaID  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @notes    This function is called when an area is automatically
*           created via an 'Apply Function'. This generally occurs
*           when a user tries to configure a non-existant area.
*
*           The area is created with default values.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaCreate(L7_uint32 areaId, L7_uint32 *index)
{
  L7_uint32 i;
  L7_uint32 areaIndex;
  L7_uint32 tosIndex;

  for(i = 0; i < L7_OSPF_MAX_AREAS; i++)
  {
    /* Area already exists -  exit */
    if(pOspfMapCfgData->area[i].inUse == L7_TRUE)
      if(pOspfMapCfgData->area[i].area == areaId)
        return L7_SUCCESS;
  }

  /* Find the next available area index */
  for (areaIndex = 1;  areaIndex < L7_OSPF_MAX_AREAS; areaIndex++)
    if (pOspfMapCfgData->area[areaIndex].inUse == L7_FALSE)
      break;

  if((areaIndex >= L7_OSPF_MAX_AREAS) ||
     (ospfMapAreaIdMibTblInsert((L7_IP_ADDR_t)areaId) != L7_SUCCESS))
    return L7_FAILURE;

  bzero((L7_char8 *)&pOspfMapCfgData->area[areaIndex], sizeof(ospfAreaCfg_t));

  pOspfMapCfgData->area[areaIndex].inUse = L7_TRUE;
  pOspfMapCfgData->area[areaIndex].area = areaId;

  /* Initialize the area configuration */
  pOspfMapCfgData->area[areaIndex].summary       = FD_OSPF_AREA_DEFAULT_SUMMARY;
  pOspfMapCfgData->area[areaIndex].extRoutingCapability  =  FD_OSPF_AREA_DEFAULT_EXT_ROUTING_CAPABILITY;

  /* Configure stub metrics */
  for (tosIndex = 0; tosIndex < L7_RTR_MAX_TOS_INDICES; tosIndex++)
  {
   pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].stubMetricType
         = FD_OSPF_AREA_DEFAULT_STUB_METRIC_TYPE;
   pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].stubMetric = FD_OSPF_AREA_DEFAULT_STUB_METRIC;
   pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].inUse   = L7_TRUE;
  }  /*   tos < L7_MAX_TOS_INDICES */

  /* NSSA Configuration */
  pOspfMapCfgData->area[areaIndex].nssaCfg.translatorRole = FD_OSPF_NSSA_DEFAULT_TRANSLATOR_ROLE;
  pOspfMapCfgData->area[areaIndex].nssaCfg.translatorStabilityInterval = FD_OSPF_NSSA_DEFAULT_TRANSLATOR_STABILITY_INTERVAL;
  pOspfMapCfgData->area[areaIndex].nssaCfg.importSummaries = FD_OSPF_NSSA_DEFAULT_IMPORT_SUMMARIES;
  pOspfMapCfgData->area[areaIndex].nssaCfg.redistribute = FD_OSPF_NSSA_DEFAULT_REDISTRIBUTE;
  pOspfMapCfgData->area[areaIndex].nssaCfg.defInfoOrig = FD_OSPF_NSSA_DEFAULT_DEFAULT_INFO_ORIGINATE;
  pOspfMapCfgData->area[areaIndex].nssaCfg.defMetric = FD_OSPF_NSSA_DEFAULT_METRIC;
  pOspfMapCfgData->area[areaIndex].nssaCfg.defMetricType = FD_OSPF_NSSA_DEFAULT_METRIC_TYPE;

 /* Return the area index */
  *index = areaIndex;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deletes an ospf area
*
* @param    areaID  @b{(input)} AreaId of the area to be deleted
*
* @returns  L7_SUCCESS  Area was successfully deleted
* @returns  L7_ERROR    Specified area has not been configured
* @returns  L7_FAILURE  Area contains active interface(s) (these
*                       need to be deleted before removing the area)
*
* @notes    This function is called when a 'no area' command is
*           issued via the user interface.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaDelete(L7_uint32 areaId)
{
  L7_uint32 areaIndex, i;

  /* If the area has not been previously configured, it cannot be
  ** deleted - return with an error
  */
  if(ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS)
    return L7_ERROR;

  /* If there is at least one interface configured on this area
  ** return with a failure
  */
  for (i = 0; i < L7_OSPF_INTF_MAX_COUNT; i++)
      if(pOspfMapCfgData->ckt[i].area == areaId)
        return L7_FAILURE;

  /* If this is a transit area for a virtual link return with a
  ** failure
  */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
    if(pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == areaId)
      return L7_FAILURE;

  ospfMapAreaIdMibTblDelete(areaId);

  bzero((L7_char8 *)&pOspfMapCfgData->area[areaIndex], sizeof(ospfAreaCfg_t));
  pOspfMapCfgData->area[areaIndex].inUse = L7_FALSE;
  ospfDataChangedSet(__FUNCTION__);

  /* Only if the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() != L7_TRUE) ||
     (ospfMapExtenAreaExists(areaId) != L7_SUCCESS))
    return L7_SUCCESS;

  return ospfMapOspfAreaDelete(areaId);
}

/*********************************************************************
* @purpose  Frees an ospf area
*
* @param    areaID  @b{(input)}
* @param    *index   @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @notes    This function is called as a result of an ospfRestore
*           operation.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaFree(L7_uint32 areaId)
{
  L7_uint32 areaIndex;

  for(areaIndex = 0; areaIndex < L7_OSPF_MAX_AREAS; areaIndex++)
  {
    if(areaId == pOspfMapCfgData->area[areaIndex].area)
    {
      break;
    }
  }
  if ( areaIndex >= L7_OSPF_MAX_AREAS )
      return L7_FAILURE;

  ospfMapAreaFreeApply(areaId);

  ospfMapAreaIdMibTblDelete(areaId);

  bzero((L7_char8 *)&pOspfMapCfgData->area[areaIndex], sizeof(ospfAreaCfg_t));
  ospfDataChangedSet(__FUNCTION__);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Determine if OSPF area exists
*
* @param    areaId      area ID
*
* @returns  L7_SUCCESS, if area exists
* @returns  L7_FAILURE  otherwise
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaEntryExists(L7_uint32 areaId )
{
   L7_uint32 i;

   for (i = 0;  i < L7_OSPF_MAX_AREAS; i++)
    {
      if(pOspfMapCfgData->area[i].inUse == L7_TRUE)
      {

          if (pOspfMapCfgData->area[i].area == areaId)
            break;
      }
    } /*  area < L7_MAX_OSPF_AREAS */


    if(i < L7_OSPF_MAX_AREAS)
      return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the area id of the specified area.
*
* @param    areaID  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaGet(L7_uint32 areaId)
{
  L7_uint32 index;

  if (ospfMapOspfInitialized() == L7_TRUE)
    if (ospfMapExtenAreaEntryGet(areaId) == L7_SUCCESS)
      return ospfMapAreaIndexGet(areaId, &index);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the area id of the subsequent area given a
*           starting point area id.
*
* @param    areaID  @b{(input)}
* @param    *index   @b{(output)}
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaGetNext(L7_uint32 areaId, L7_uint32 *nextAreaId)
{
  L7_uint32 index;

  if (ospfMapOspfInitialized() == L7_TRUE)
    if(ospfMapExtenAreaEntryGetNext(areaId, nextAreaId) == L7_SUCCESS)
      return ospfMapAreaIndexGet(*nextAreaId, &index);

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the Ospf Area Id for the first area of the Ospf Router
*
* @param    *p_areaId      L7_uint32 area Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This routine returns the first entry in an ordered list.
*
* @end
*********************************************************************/
L7_uint32 ospfMapAreaIdFirst(L7_uint32 *p_areaId)
{
    ospfmap_areaIdSearch_t data;
    L7_IP_ADDR_t searchArea;

    searchArea = 0;

    /* Search for backbone area first */
    if (ospfMapAreaIdMibTblFind(&searchArea, L7_MATCH_EXACT,&data) == L7_SUCCESS)
    {
        *p_areaId = data.areaId;
        return L7_SUCCESS;
    }

    return(ospfMapAreaGetNext(searchArea, p_areaId));
}

/*********************************************************************
* @purpose  Gets the first configured OSPF area ID.
*
* @param    areaID  @b{(output)}
*
* @returns  L7_SUCCESS if an area is configured
* @returns  L7_FAILURE, otherwise
*
* @comments This function doesn't depend on operational state.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaCfgGetFirst(L7_uint32 *areaId)
{
    L7_uint32 i;

    for (i = 0;  i < L7_OSPF_MAX_AREAS; i++)
    {
        if (pOspfMapCfgData->area[i].inUse == L7_TRUE)
        {
            *areaId = pOspfMapCfgData->area[i].area;
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Given an OSPF area ID, gets the next configured OSPF area ID.
*
* @param    areaId  @b{(input)}
* @param    nextAreaId  @b{(output)}
*
* @returns  L7_SUCCESS if a next area ID is returned
* @returns  L7_FAILURE if there is not another area or if the input
*                      area is not found.
*
* @comments This function doesn't depend on operational state.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaCfgGetNext(L7_uint32 areaId, L7_uint32 *nextAreaId)
{
    L7_uint32 i;
    L7_uint32 index;

    /* Find the input area */
    if (ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
        return L7_FAILURE;

    for (i = index + 1;  i < L7_OSPF_MAX_AREAS; i++)
    {
        if (pOspfMapCfgData->area[i].inUse == L7_TRUE)
        {
            *nextAreaId = pOspfMapCfgData->area[i].area;
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the array index associated with the area
*
* @param     areaId @b{(input)}
* @param    *index   @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaIndexGet(L7_uint32 areaId, L7_uint32 *index)
{
  L7_uint32 i;

  for (i=0;  i < L7_OSPF_MAX_AREAS; i++)
  {
    if ( pOspfMapCfgData->area[i].inUse == L7_TRUE)
    {
      if ( pOspfMapCfgData->area[i].area == areaId)
      {
        *index = i;
        return L7_SUCCESS;
      }
    }
  } /*  area < L7_MAX_OSPF_AREAS */

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the area's support for importing AS external
*           link-state advertisements.
*
* @param    areaId      type
* @param    *val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Area's support for importing AS external link-state
*             advertisements."
*
* @end
*********************************************************************/
L7_RC_t ospfMapImportAsExternGet ( L7_uint32 areaId, L7_int32 *val )
{
  L7_uint32 index;

  if(ospfMapAreaIndexGet(areaId, &index) == L7_SUCCESS)
  {
    *val = pOspfMapCfgData->area[index].extRoutingCapability;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test if the area's support for importing AS external link-state
*           advertisements is settable
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapImportAsExternSetTest ( L7_uint32 areaId, L7_int32 val )
{
  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenImportAsExternSetTest(areaId, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the area's support for importing AS external
*           link-state advertisements.
*
* @param    areaId      type
* @param    val         value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_REQUEST_DENIED   if attempt to make backbone a stub or NSSA
*
* @notes    " The Area's support for importing AS external link-state
*             advertisements."
*
* @end
*********************************************************************/
L7_RC_t ospfMapImportAsExternSet ( L7_uint32 areaId, L7_int32 val )
{
  L7_uint32 index;
  L7_int32  prevConf;

  /* Backbone cannot be a stub or NSSA */
  if ((areaId == OSPF_BACKBONE_AREA_ID) &&
      ((val == L7_OSPF_AREA_IMPORT_NO_EXT) || (val == L7_OSPF_AREA_IMPORT_NSSA)))
  {
    return L7_REQUEST_DENIED;
  }

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &index) != L7_SUCCESS))
    return L7_FAILURE;

  /* If the current external routing capability is the same as the
  ** value specified by the user, exit successfully
  */
  if(pOspfMapCfgData->area[index].extRoutingCapability == val)
    return L7_SUCCESS;

  prevConf = pOspfMapCfgData->area[index].extRoutingCapability;

  /* We can only transition from a regular area to a stub or NSSA.
  ** i.e. if the area is currently configured as a stub we cannot
  ** re-configure it as a NSSA before removing the stub configuration
  */
  if(((prevConf == L7_OSPF_AREA_IMPORT_NO_EXT) ||
      (prevConf == L7_OSPF_AREA_IMPORT_NSSA)) &&
     (val != L7_OSPF_AREA_IMPORT_EXT))
    return L7_FAILURE;

  pOspfMapCfgData->area[index].extRoutingCapability = val;
  ospfDataChangedSet(__FUNCTION__);

  ospfMapAreaImportAsExternApply(areaId, prevConf, val);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieve the number of SPF Runs for the specified area
*
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 number of SPF runs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of times that the intra-area route table
*             has been calculated using this area's link-state
*             database. This is typically done using Djikstra's
*             algorithm."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNumSPFRunsGet(L7_uint32 areaId, L7_uint32 *val)
{
  L7_uint32 index;

  if(ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
    return L7_FAILURE;

  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    /* If the area has not been created in the underlying vendor
    ** code because it does not have any interfaces/ virtual
    ** links or area ranges configured, return the number of
    ** spf runs as 0
    */
    if (ospfMapExtenAreaExists(areaId) != L7_SUCCESS)
    {
      *val = 0;
      return L7_SUCCESS;
    }

    /* If the area has been initialized in the vendor code return
    ** the value stored in the vendor area configuration
    */
    return ospfMapExtenNumSPFRunsGet(areaId, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the total number of Autonomous System border routers
*           reachable  within  this area.
*
* @param    areaId      type
* @param    *val        number of routers
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The total number of Autonomous System border routers
*             reachable within this area.  This is initially zero
*             and is calculated in each SPF Pass."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAsBdrRtrCountGet ( L7_uint32 areaId, L7_uint32 *val )
{
  L7_uint32 index;

  if(ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
    return L7_FAILURE;

  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    /* If the area has not been created in the underlying vendor
    ** code because it does not have any interfaces/ virtual
    ** links or area ranges configured, return the number of
    ** asbdr routers as 0
    */
    if (ospfMapExtenAreaExists(areaId) != L7_SUCCESS)
    {
      *val = 0;
      return L7_SUCCESS;
    }

    /* If the area has been initialized in the vendor code return
    ** the value stored in the vendor area configuration
    */
    return ospfMapExtenAsBdrRtrCountGet(areaId, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieve the Area Border Count for the specified area
*
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 area border count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The total number of area border routers reachable within
*             this area. This is initially zero, and is calculated
*             in each SPF Pass."
*
* @end
*********************************************************************/
L7_uint32 ospfMapAreaBorderCountGet(L7_uint32 areaId, L7_uint32 *val)
{
  L7_uint32 index;

  if(ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
    return L7_FAILURE;

  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    /* If the area has not been created in the underlying vendor
    ** code because it does not have any interfaces/ virtual
    ** links or area ranges configured, return the number of
    ** area border routers as 0
    */
    if (ospfMapExtenAreaExists(areaId) != L7_SUCCESS)
    {
      *val = 0;
      return L7_SUCCESS;
    }

    /* If the area has been initialized in the vendor code return
    ** the value stored in the vendor area configuration
    */
    return ospfMapExtenAreaBorderCountGet(areaId, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Retrieve the Area LSA Count for the specified area
*
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 area LSA count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The total number of link-state advertisements in this
*             area's link-state database, excluding AS External LSAs"
*
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSACountGet(L7_uint32 areaId, L7_uint32 *val)
{
  L7_uint32 index;

  if(ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
    return L7_FAILURE;

  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    /* If the area has not been created in the underlying vendor
    ** code because it does not have any interfaces/ virtual
    ** links or area ranges configured, return the number of
    ** LSAs in the area as 0
    */
    if (ospfMapExtenAreaExists(areaId) != L7_SUCCESS)
    {
      *val = 0;
      return L7_SUCCESS;
    }

    /* If the area has been initialized in the vendor code return
    ** the value stored in the vendor area configuration
    */
    return ospfMapExtenAreaLSACountGet(areaId, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Retrieve the Area LSA Statistics
*
* @param    areaId    L7_uint32 area ID
* @param    *stats    area LSA stats
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSAStatGet(L7_uint32 areaId, L7_OspfAreaDbStats_t *stats)
{
  L7_uint32 index;

  if(ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
    return L7_FAILURE;

  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    /* If the area has not been created in the underlying vendor
    ** code because it does not have any interfaces/ virtual
    ** links or area ranges configured, return the number of
    ** LSAs in the area as 0
    */
    if (ospfMapExtenAreaExists(areaId) != L7_SUCCESS)
    {
      bzero((void *)stats, sizeof(L7_OspfAreaDbStats_t));
      return L7_SUCCESS;
    }

    /* If the area has been initialized in the vendor code return
    ** the value stored in the vendor area configuration
    */
    return ospfMapExtenAreaLSAStatGet(areaId, stats);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Retrieve the Area LSA Checksum for the specified area
*
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 area LSA checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The 32-bit unsigned sum of the link-state advertisements'
*             LS checksums contained in this area's link-state database.
*             This sum excludes external (LS type 5) link-state
*             advertisements. The sum can be used to determine if there
*             has been a change in a router's link state database, and
*             to compare the link-state database of two routers."
*
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSAChecksumGet(L7_uint32 areaId, L7_uint32 *val)
{
  L7_uint32 index;

  if(ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
    return L7_FAILURE;

  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    /* If the area has not been created in the underlying vendor
    ** code because it does not have any interfaces/ virtual
    ** links or area ranges configured, return the lsa checksum
    ** as 0
    */
    if (ospfMapExtenAreaExists(areaId) != L7_SUCCESS)
    {
      *val = 0;
      return L7_SUCCESS;
    }

    /* If the area has been initialized in the vendor code return
    ** the value stored in the vendor area configuration
    */
    return ospfMapExtenAreaLSAChecksumGet(areaId, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the area summary.
*
* @param    areaId      type
* @param    *val        variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The variable ospfAreaSummary controls the import of
*             summary LSAs into stub areas. It has no effect on other areas.
*
*             Valid outputs:
*               noAreaSummary (1)
*               sendAreaSummary (2)
*
*             If it is noAreaSummary, the router will neither originate
*             nor propagate summary LSAs into the stub area.  It will
*             rely entirely on its default route.
*
*             If it is sendAreaSummary, the router will both summarize
*             and propagate summary LSAs."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaSummaryGet ( L7_uint32 areaId, L7_int32 *val )
{
  L7_uint32 index;

  if (ospfMapAreaIndexGet(areaId, &index) == L7_SUCCESS)
  {
    *val = pOspfMapCfgData->area[index].summary;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test if the area summary is settable
*
* @param    areaId      type
* @param    *val        variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The variable ospfAreaSummary controls the import of
*             summary LSAs into stub areas. It has no effect on other areas.
*
*             Valid outputs:
*               noAreaSummary (1)
*               sendAreaSummary (2)
*
*             If it is noAreaSummary, the router will neither originate
*             nor propagate summary LSAs into the stub area.  It will
*             rely entirely on its default route.
*
*             If it is sendAreaSummary, the router will both summarize
*             and propagate summary LSAs."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaSummarySetTest ( L7_uint32 areaId, L7_int32 val )
{
  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenAreaSummarySetTest(areaId, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the area summary.
*
* @param    areaId      type
* @param    val         variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_REQUEST_DENIED if area is backbone area
*
* @notes    " The variable ospfAreaSummary controls the import of
*             summary LSAs into stub areas. It has no effect on other areas.
*
*             Valid outputs:
*               noAreaSummary (1)
*               sendAreaSummary (2)
*
*             If it is noAreaSummary, the router will neither originate
*             nor propagate summary LSAs into the stub area.  It will
*             rely entirely on its default route.
*
*             If it is sendAreaSummary, the router will both summarize
*             and propagate summary LSAs."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaSummarySet ( L7_uint32 areaId, L7_int32 val )
{
  L7_uint32 index;
  L7_RC_t rc = L7_FAILURE;

  if (areaId == OSPF_BACKBONE_AREA_ID)
  {
    return L7_REQUEST_DENIED;
  }

  if (ospfMapAreaIndexGet(areaId,&index) == L7_SUCCESS)
  {
    pOspfMapCfgData->area[index].summary = val;
    ospfDataChangedSet(__FUNCTION__);

    if ((rc = ospfMapAreaSummaryApply( areaId, val)) == L7_SUCCESS)
      return L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the OSPF area entry status.
*
* @param    areaId      type
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if the area is currently active
* @returns  L7_FAILURE  otherwise
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaStatusGet ( L7_uint32 areaId, L7_uint32 *val )
{
  L7_uint32 index;

  if(ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
    return L7_FAILURE;

  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    /* If the area has not been created in the underlying vendor
    ** code because it does not have any interfaces/ virtual
    ** links or area ranges configured, return the number of
    ** spf runs as 0
    */
    if (ospfMapExtenAreaExists(areaId) != L7_SUCCESS)
    {
      *val = L7_OSPF_ROW_ACTIVE;
      return L7_SUCCESS;
    }

    /* If the area has been initialized in the vendor code return
    ** the value stored in the vendor area configuration
    */
    return ospfMapExtenAreaStatusGet(areaId, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test if the OSPF area entry status is settable.
*
* @param    areaId      type
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaStatusSetTest ( L7_uint32 areaId, L7_int32 val )
{
  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenAreaStatusSetTest(areaId, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the OSPF area entry status.
*
* @param    areaId      type
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperative.
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaStatusSet ( L7_uint32 areaId, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenAreaStatusSet(areaId, val);

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Check the External Routing Capability for the specified Area
*
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 external routing capability
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ospfMapAreaExternalRoutingCapabilityGet(L7_uint32 areaId, L7_uint32 *val)
{
  L7_uint32 index;

  if(ospfMapAreaIndexGet(areaId, &index) == L7_SUCCESS)
  {
    *val = pOspfMapCfgData->area[index].extRoutingCapability;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}
/*
**********************************************************************
*                    API FUNCTIONS  -  STUB AREA CONFIG
**********************************************************************
*/

/*--------------------------------------------------------------------------*/
/* OSPF Area Default Metric Table:                                          */
/* The OSPF Area Default Metric Table describes the metrics that a default  */
/* Area Border Router will advertise into a Stub Area.                      */
/*--------------------------------------------------------------------------*/



/*********************************************************************
* @purpose  Check if TOS Type is supported
*
* @param    TOS         TOS value
*
*
* @returns  L7_TRUE    if TOS value is supported
* @returns  L7_FALSE     otherwise
*
*
* @end
*********************************************************************/
L7_BOOL ospfMapTosIsValid (  L7_uint32 TOS)
{

    /* Return true if TOS value is 0 (i.e. normal service) or if
       tos index value is non-zero in the tos table */

    if (TOS >=  L7_RTR_MAX_TOS )
        return L7_FALSE;

    if (TOS == L7_TOS_NORMAL_SERVICE)
        return L7_TRUE;

    if (tosToTosIndex[TOS] != 0)
        return L7_TRUE;

    return L7_FALSE;

}

/*********************************************************************
* @purpose  Get the information associated with stub area configuration
*
* @param    areaId      area id
* @param    TOS         TOS  (L7_TOSTYPES)
* @param    *metric     pointer to stub area metric
* @param    *metric     pointer to stub area metric type
*
* @returns  L7_SUCCESS  if stub area configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    " The metric for a given Type of Service that will be
*             advertised by a default Area Border Router into a stub
*             area."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubAreaEntryGet ( L7_uint32 areaId, L7_uint32 TOS,
                                  L7_uint32 *metric, L7_uint32 *metricType)
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized())
  {
    rc = ospfMapExtenStubAreaEntryGet(areaId, TOS);
    return rc;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Given a stub area specification, return the area ID and TOS
*           of the next stub area
*
* @param    areaId      area id
* @param    TOS         TOS  (L7_TOSTYPES)
*
* @returns  L7_SUCCESS  if stub area configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    " The metric for a given Type of Service that will be
*             advertised by a default Area Border Router into a stub
*             area."
*
*           This routine returns the configured values for the stub area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubAreaEntryNext ( L7_uint32 *areaId, L7_uint32 *TOS)
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized())
  {
    rc = ospfMapExtenStubAreaEntryNext(areaId, TOS);
    return rc;
  }
  return L7_FAILURE;

}



/*********************************************************************
* @purpose  Check if the stub entry is active.
*
* @param    *AreaId     type
* @param    TOS         entry
* @param    *val        L7_TRUE, if entry is active, L7_FALSE, otherwise
*
*
* @returns  L7_SUCCESS    if entry could be found
* @returns  L7_FAILUE     otherwise
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperable
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubStatusGet ( L7_uint32 areaId, L7_int32 TOS, L7_int32 *val )
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized())
  {
    rc = ospfMapExtenStubAreaStatusGet(areaId, TOS, val);
    return rc;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the type of metric set as a default route.
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    *metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricTypeGet ( L7_uint32 areaId, L7_int32 TOS, L7_int32 *metricType )
{
  L7_uint32 areaIndex;
  L7_uint32 tosIndex;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if (ospfMapTosIsValid(TOS) == L7_TRUE)
    {
        tosIndex = tosToTosIndex[TOS];

        if (pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].inUse == L7_TRUE)
        {
            *metricType = pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].stubMetricType;
            return L7_SUCCESS;
        }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to Set the stub metric type.
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricTypeSetTest ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metricType )
{
  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenStubAreaMetricTypeSetTest(areaId, TOS, metricType);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the stub metric type.
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    *metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricTypeSet ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metricType )
{
    L7_uint32 areaIndex;
    L7_uint32 tosIndex;

    if ( (metricType !=  L7_OSPF_AREA_STUB_METRIC) &&
         (metricType !=  L7_OSPF_AREA_STUB_COMPARABLE_COST) &&
         (metricType !=  L7_OSPF_AREA_STUB_NON_COMPARABLE_COST) )
    {
        return L7_FAILURE;
    }

    if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
    {
      if (ospfMapTosIsValid(TOS) == L7_TRUE)
      {
          tosIndex = tosToTosIndex[TOS];

          pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].stubMetricType = metricType;
          ospfDataChangedSet(__FUNCTION__);


          if (ospfMapOspfInitialized() == L7_TRUE)
            return ospfMapExtenStubAreaMetricCfgSet(areaId, TOS,
                                                 pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].stubMetric,
                                                 metricType);


          return L7_SUCCESS;
      }
    }

  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Get the stub metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area ID
* @param    TOS         TOS type (L7_TOSTYPES)
* @param    *metric     stubMetric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    By default, this equals the least metric at the type of
*           service among the interfaces to other areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricGet ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 *metric )
{
  L7_uint32 areaIndex;
  L7_uint32 tosIndex;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if (ospfMapTosIsValid(TOS) == L7_TRUE)
    {
        tosIndex = tosToTosIndex[TOS];

        if (pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].inUse == L7_TRUE)
        {

            *metric     = pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].stubMetric;
            return L7_SUCCESS;
        }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to Set the stub metric for the given area and TOS
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @notes     If metric is set to 0, metric is calculated as a factor
*            of ifSpeed.
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricSetTest ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metric )
{
  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenStubAreaMetricSetTest(areaId, TOS, metric);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the stub metric for the given area and TOS
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @notes     If metric is set to 0, metric is calculated as a factor
*            of ifSpeed.
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricSet ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metric )
{
  L7_uint32 areaIndex;
  L7_uint32 tosIndex;
  L7_uint32 currMode;
  L7_uint32 stubMetricType;   /* This is meaningless */

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* If this is not a valid TOS or one that is not currently supported
  ** return with a failure
  */
  if (ospfMapTosIsValid(TOS) != L7_TRUE)
    return L7_FAILURE;

  tosIndex = tosToTosIndex[TOS];

  pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].stubMetric = metric;
  ospfDataChangedSet(__FUNCTION__);

  /* If the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() == L7_TRUE) &&
     (ospfMapExtenAreaExists(areaId) == L7_SUCCESS))
  {
    /* Both NSSAs and stubs use default-cost. How we apply the default
     * cost is different for each area type. */
    if (ospfMapImportAsExternGet(areaId, &currMode) == L7_SUCCESS)
    {
      if (currMode == L7_OSPF_AREA_IMPORT_NO_EXT)
      {
        /* If we reached here this is a stub area and we need to update
         ** the configured default metric value
         */
        /* stub metric type is meaningless. A T3 LSA has no metric type. */
        stubMetricType =
          pOspfMapCfgData->area[areaIndex].stubCfg[tosIndex].stubMetricType;
        return ospfMapExtenStubAreaMetricCfgSet(areaId, TOS, metric,
                                                stubMetricType);
      }
      else if (currMode == L7_OSPF_AREA_IMPORT_NSSA)
      {
        return ospfMapExtenNSSADefaultCostSet(areaId, metric);
      }
    }
  }

  /* If OSPF has not yet been initialized or the area has not been
  ** created in the vendor ospf code, return successfully, these
  ** configuration changes will be applied when the area is
  ** created
  */
  return L7_SUCCESS;
}

/*
**********************************************************************
*                    API FUNCTIONS  -  AREA RANGE CONFIG
**********************************************************************
*/

/*--------------------------------------------------------------------*/
/* The Address Range Table acts as an adjunct to the Area Table; It   */
/* describes those Address Range Summaries that are configured to be  */
/* propagated from an Area to reduce the amount of information about  */
/* it which is known beyond its borders.                              */
/* Note: This table was obsoleted by rfc 1850                         */
/*--------------------------------------------------------------------*/


/*
**********************************************************************
*                    API FUNCTIONS  -  AREA AGGREGATE CONFIG
**********************************************************************
*/

/*-------------------------------------------------------------------------*/
/* The OSPF Area Aggregate Table:                                          */
/* This table replaces the OSPF Area Summary Table, being and extension    */
/* of that for CIDR routers. An Area Aggregate entry is a range of IP      */
/* addresses specified by an IP address/IP network mask pair. For example  */
/* class B address range of X.X.X.X with a network mask of 255.255.0.0     */
/* includes all IP addresses from X.X.0.0 to X.X.255.255. Note that        */
/* if ranges are configured such that one range subsumes another range     */
/* (e.g., 10.0.0.0 mask 255.0.0.0 and 10.1.0.0 mask 255.255.0.0 the        */
/* most specific match is the preferred one.                               */
/*-------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Create an area range entry.
*
* @param    areaId      type
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
* @param    advertise   advertise aggregate area (L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_ERROR    if conflicting area range exists.
*                       We can't configure the same address range as
*                       summary and nssa.
*
* @notes    " A range of IP Addresses specified by an IP Address/IP
*             network mask pair.  For example, calss B address range
*             of X.X.X.X with a network mask of 255.255.0.0 includes
*             all IP addresses from X.X.0.0 to X.X.255.255."
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggregateRangeCreate ( L7_uint32 areaId,
                                          L7_uint32 lsdbType,
                                          L7_uint32 ipAddr,
                                          L7_uint32 netMask,
                                          L7_uint32 advertise )
{
  L7_uint32 index, areaIndex;
  L7_RC_t rc = L7_SUCCESS;

  /* check if conflicting area range exists for the given network
   * address and netmask in the given area */
  if (ospfMapAreaAggregateRangeConflictExists(areaId, lsdbType, ipAddr, netMask))
  {
    return L7_ERROR;
  }

  /* Find the first empty area range configuration */
  rc = ospfMapAreaAggregateRangeCfgIndexFind (areaId, lsdbType, ipAddr,
                                              netMask, L7_MATCH_FREE, &index);

  if(rc == L7_ALREADY_CONFIGURED)
    return L7_SUCCESS;
  else if(rc == L7_FAILURE)
    return L7_FAILURE;

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* Save the configuration in the config file */
  pOspfMapCfgData->areaRange[index].inUse       = L7_TRUE;
  pOspfMapCfgData->areaRange[index].area        = areaId;
  pOspfMapCfgData->areaRange[index].lsdbType    = lsdbType;
  pOspfMapCfgData->areaRange[index].ipAddr      = ipAddr;
  pOspfMapCfgData->areaRange[index].subnetMask  = netMask;
  pOspfMapCfgData->areaRange[index].advertise   = advertise;

  ospfDataChangedSet(__FUNCTION__);

  /* Apply the configuration */
  if (ospfMapAreaAggregateRangeApply(areaId, lsdbType, ipAddr, netMask, advertise)
      != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Delete an area range entry.
*
* @param    areaId      type
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure, if aggregate range is not configured
*                       or if deletion fails
*
* @notes    " A range of IP Addresses specified by an IP Address/IP
*             network mask pair.  For example, calss B address range
*             of X.X.X.X with a network mask of 255.255.0.0 includes
*             all IP addresses from X.X.0.0 to X.X.255.255."
*
*             obsolete
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggregateRangeDelete ( L7_uint32 areaId,
                                 L7_uint32 lsdbType,
                                 L7_uint32 ipAddr,
                                 L7_uint32 netMask )
{
    L7_uint32 index;
    L7_RC_t   rc;

    rc = L7_SUCCESS;

    if (ospfMapAreaAggregateRangeCfgIndexFind (areaId, lsdbType, ipAddr,
                                               netMask,L7_MATCH_EXACT, &index )
        != L7_ALREADY_CONFIGURED)
    {
     return L7_FAILURE;
    }

    /* Apply the configuration */
    if (ospfMapAreaAggregateRangeDeleteApply(areaId, lsdbType, ipAddr, netMask) != L7_SUCCESS)
        rc = L7_FAILURE;


    /* Save the configuration */
    memset(&(pOspfMapCfgData->areaRange[index]), 0, sizeof(ospfAreaRangeCfg_t));

    ospfDataChangedSet(__FUNCTION__);

  return rc;
}

/*********************************************************************
* @purpose  Get the first area range for a given OSPF area.
*
* @param  areaId @b{(input)} Area ID of the area the range applies to
* @param  rangeType  @b{(input/output)} Whether the range is used when
*                      generating type 3 summary LSAs or used when
*                      translating T7 LSAs to T5 LSAs at the border of
*                      an NSSA.
* @param  prefix  @b{(input/output)}  Network prefix
* @param  mask    @b{(input/output)}  Network mask
*
* @returns  L7_SUCCESS  if a next range is returned
* @returns  L7_FAILURE  if there is not another range
*
* @notes    Function returns either type of area range (summary or
*           NSSA), regardless of the input rangeType.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaRangeFirst(L7_uint32 *areaId,
                              L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t *rangeType,
                              L7_int32 *prefix, L7_uint32 *mask)
{
    L7_uint32 i;
    for (i = 0; i < L7_OSPF_MAX_AREA_RANGES; i++)
    {
        if ((pOspfMapCfgData->areaRange[i].inUse == L7_TRUE) &&
            (pOspfMapCfgData->areaRange[i].area == *areaId))
        {
            *rangeType = (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t) pOspfMapCfgData->areaRange[i].lsdbType;
            *prefix = pOspfMapCfgData->areaRange[i].ipAddr;
            *mask = pOspfMapCfgData->areaRange[i].subnetMask;
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get next area range for a given OSPF area.
*
* @param  areaId @b{(input)} Area ID of the area the range applies to
* @param  rangeType  @b{(input/output)} Whether the range is used when
*                      generating type 3 summary LSAs or used when
*                      translating T7 LSAs to T5 LSAs at the border of
*                      an NSSA. One of L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t.
*                      0 to get the first range of either type.
* @param  prefix  @b{(input/output)}  Network prefix
* @param  mask    @b{(input/output)}  Network mask
*
* @returns  L7_SUCCESS  if a next range is returned
* @returns  L7_FAILURE  if no next range
* @returns  L7_ERROR if input parameters are invalid or previous range not found
*
* @notes    Function returns either type of area range (summary or
*           NSSA), regardless of the input rangeType.
*
*           If rangeType is 0 on input, function assumes caller is looking
*           for the first range entry. If rangeType is non-zero, function
*           assumes input parameters identify a range entry and gets the
*           next entry. Note that a range can have a prefix and mask of 0.
*
*           CLI and Web use this API because their displays are intended to
*           show area ranges for a specific area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrEntryNext(L7_uint32 *areaId,
                                 L7_uint32 *rangeType,
                                 L7_int32 *prefix, L7_uint32 *mask)
{
  L7_uint32 index;
  L7_uint32 i;
  L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t rType;

  if (*rangeType == 0)
  {
      if ((*prefix != 0) || (*mask != 0))
      {
          return L7_ERROR;
      }
      if (ospfMapAreaRangeFirst(areaId, &rType, prefix, mask) == L7_SUCCESS)
      {
          *rangeType = rType;
          return L7_SUCCESS;  /* Return first entry */
      }
      else
      {
          return L7_FAILURE;   /* No ranges configured */
      }
  }

  /* Find previous range */
  if (ospfMapAreaAggregateRangeCfgIndexFind(*areaId, *rangeType, *prefix,
                                            *mask, L7_MATCH_EXACT,
                                            &index) != L7_ALREADY_CONFIGURED)
  {
      /* Previous range not found. */
      return L7_ERROR;
  }

  /* Found previous entry. Find next. */
  for (i = index + 1; i < L7_OSPF_MAX_AREA_RANGES; i++)
  {
      if ((pOspfMapCfgData->areaRange[i].inUse == L7_TRUE) &&
          (pOspfMapCfgData->areaRange[i].area == *areaId))
      {
          *rangeType = pOspfMapCfgData->areaRange[i].lsdbType;
          *prefix = pOspfMapCfgData->areaRange[i].ipAddr;
          *mask = pOspfMapCfgData->areaRange[i].subnetMask;
          return L7_SUCCESS;
      }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get next area range for any OSPF area.
*
* @param  areaId @b{(input/output)} Area ID of the area the range applies to
* @param  rangeType  @b{(input/output)} Whether the range is used when
*                      generating type 3 summary LSAs or used when
*                      translating T7 LSAs to T5 LSAs at the border of
*                      an NSSA. One of L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t.
*                      0 to get the first range of either type.
* @param  prefix  @b{(input/output)}  Network prefix
* @param  mask    @b{(input/output)}  Network mask
*
* @returns  L7_SUCCESS  if a next range is returned
* @returns  L7_FAILURE  if no next range
* @returns  L7_ERROR if input parameters are invalid or previous range not found
*
* @notes    Function returns either type of area range (summary or
*           NSSA), regardless of the input rangeType.
*
*           This function is provided to enable SNMP to walk all area range
*           entries for all areas for the ospfAreaAggregateTable in RFC 1850.
*           SNMP treats the combination of the 4 arguments as the key. After
*           retrieving the first area range, SNMP will call this function
*           again with the "key" incremented, thinking that might be the
*           next possible entry. Thus, the input parameters are not exactly
*           those of an existing area range. We have to find the area range
*           with the next larger "key." We get this behavior from the OSPF
*           MIB code, but note that OSPF MIB data is only available is OSPF
*           has already been initialized. Since SNMP does not work off the
*           same data as CLI and Web, the results may differ, esp when OSPF
*           has not been initialized yet.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAnyAreaAggrEntryNext(L7_uint32 *areaId,
                                    L7_uint32 *rangeType,
                                    L7_int32 *prefix, L7_uint32 *mask)
{
    L7_RC_t rc;

    if (ospfMapOspfInitialized() == L7_TRUE)
    {
        rc = ospfMapExtenAreaAggrEntryNext(areaId, rangeType, prefix, mask);
        return rc;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Find a specific area range.
*
* @param  areaId @b{(input)} Area ID of the area the range applies to
* @param  rangeType  @b{(input)} Whether the range is used when
*                      generating type 3 summary LSAs or used when
*                      translating T7 LSAs to T5 LSAs at the border of
*                      an NSSA. One of L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t.
* @param  prefix  @b{(input)}  Network prefix
* @param  mask    @b{(input)}  Network mask
*
* @returns  L7_SUCCESS  if an area range with these parameters is configured
* @returns  L7_FAILURE  otherwise
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrEntryGet (L7_uint32 areaId, L7_uint32 rangeType,
                                 L7_int32 prefix, L7_uint32 mask)
{
    L7_uint32 index = 0;
  if (ospfMapAreaAggregateRangeCfgIndexFind(areaId, rangeType, (L7_uint32) prefix, mask,
                                            L7_MATCH_EXACT, &index) == L7_ALREADY_CONFIGURED)
      return L7_SUCCESS;
  else
      return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the SNMP row status of an area range.
*
* @param    areaId    @b{(input)}  area id
* @param    lsdbType  @b{(input)}  link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr    @b{(input)}  network
* @param    netMask   @b{(input)}  mask
* @param    *val      @b{(output)} entry status  (e_RowStatus)
*
* @returns  L7_SUCCESS, if entry found
* @returns  L7_FAILURE  otherwise
*
* @notes    This function is only used by SNMP. Because we are interested
*           in the row status, goes to the protocol SNMP tables, rather
*           than using the configuration data. This function only succeeds
*           if OSPF is initialized.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrStatusGet ( L7_uint32 areaId,
                                   L7_uint32 lsdbType,
                                   L7_uint32 ipAddr,
                                   L7_uint32 netMask,
                                   L7_uint32 *val )
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    rc = ospfMapExtenAreaAggrStatusGet(areaId,lsdbType,ipAddr,netMask,val);
    return rc;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to set the status of area aggregate entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    *val        entry status  (L7_TRUE, if active,
*                                      L7_FALSE, if inactive or nonexistent)
*
* @returns  L7_SUCCESS, if entry could be found
* @returns  L7_FAILURE  otherwise
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrStatusSetTest ( L7_uint32 areaId,
                                        L7_uint32 lsdbType,
                                        L7_uint32 ipAddr,
                                        L7_uint32 netMask,
                                        L7_uint32 val )
{
  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenAreaAggrStatusSetTest(areaId, lsdbType, ipAddr, netMask, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the status of area aggregate entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    *val        entry status  (L7_TRUE, if active,
*                                      L7_FALSE, if inactive or nonexistent)
*
* @returns  L7_SUCCESS, if entry could be found
* @returns  L7_FAILURE  otherwise
*
* @notes    This API is unused, but would apparently be used by SNMP to
*           set the row status of an area range.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrStatusSet ( L7_uint32 areaId,
                                        L7_uint32 lsdbType,
                                        L7_uint32 ipAddr,
                                        L7_uint32 netMask,
                                        L7_uint32 val )
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    rc = ospfMapExtenAreaAggrStatusSet(areaId,lsdbType,ipAddr,netMask,val);
    return rc;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine whether the action for a given range is advertise
*           or no advertise.
*
* @param    areaId      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    *val        subnet effect  (L7_TRUE, if advertise, L7_FALSE, no advertise)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure, if aggregate range is not configured
*
* @notes    Subnets subsumed by ranges either trigger the advertisement
*           of indicated aggregate, or result in subnet's not being
*           advertised at all outside the area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrEffectGet ( L7_uint32 areaId, L7_uint32 Type,
                                   L7_int32 Net, L7_uint32 Mask, L7_int32 *val )
{
    L7_uint32 index;

    if (ospfMapAreaAggregateRangeCfgIndexFind (areaId, Type, Net, Mask,
                                               L7_MATCH_EXACT, &index )
        != L7_ALREADY_CONFIGURED)
    {
      return L7_FAILURE;
    }

    *val = pOspfMapCfgData->areaRange[index].advertise;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test to set the subnet effect.
*
* @param    area      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    advertise   advertise range (L7_BOOL)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure, if aggregate range is not configured
*
* @notes    " Subnets subsumed by ranges either trigger the advertisement
*             of indicated aggregate, or result in subnet's not being
*             advertised at all outside the area."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrEffectSetTest ( L7_uint32 areaId, L7_uint32 lsdbType,
                                        L7_int32 ipAddr, L7_uint32 netMask, L7_BOOL advertise )
{
  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenAreaAggrEffectSetTest(areaId, lsdbType, ipAddr, netMask, advertise);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set whether the effect of an area range is advertise or no advertise.
*
* @param    area      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    advertise   L7_TRUE if range should be advertised.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure, if aggregate range is not configured
*
* @notes    " Subnets subsumed by ranges either trigger the advertisement
*             of indicated aggregate, or result in subnet's not being
*             advertised at all outside the area."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrEffectSet ( L7_uint32 areaId, L7_uint32 lsdbType,
                                        L7_int32 ipAddr, L7_uint32 netMask, L7_BOOL advertise )
{

    L7_uint32 index;

    if (ospfMapAreaAggregateRangeCfgIndexFind (areaId, lsdbType, ipAddr,
                                               netMask,L7_MATCH_EXACT, &index )
        != L7_ALREADY_CONFIGURED)
    {
      return L7_FAILURE;
    }

    /* Save the configuration in the config file */
    pOspfMapCfgData->areaRange[index].advertise   = advertise;
    ospfDataChangedSet(__FUNCTION__);

    /* Apply the configuration */
    if (ospfMapAreaAggregateRangeApply(areaId, lsdbType, ipAddr, netMask, advertise) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*
**********************************************************************
*                    API FUNCTIONS  -  Network Area command
**********************************************************************
*/

/*********************************************************************
* @purpose  Find a specific network area object.
*
* @param  ipAddr        @b{(input)} network configured for ospf
* @param  wildcardMask  @b{(input)} wildcard mask
* @param  areaId        @b{(input)} areaid for this network
*
* @returns  L7_SUCCESS  if the specified network area object is found
* @returns  L7_FAILURE  otherwise
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaEntryGet (L7_uint32 ipAddr, L7_uint32 wildcardMask,
                                    L7_int32 areaId)
{
  if(ospfMapNetworkAreaGet(ipAddr, wildcardMask, areaId) == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/***************************************************************************
* @purpose  Get next network area configured for OSPF.
*
* @param  ipAddr     @b{(input/output)} network of the current object
* @param  wildcardMask @b{(input/output)} wildcardMask of the current object
* @param  areaId     @b{(input/output)}  areaId of the current object
*
* @returns  L7_SUCCESS  if a next network area object exists
* @returns  L7_FAILURE  if no next object exists
* @returns  L7_ERROR if input parameters are invalid or previous network
*                    area object is not found
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaEntryNext(L7_uint32 *ipAddr, L7_uint32 *wildcardMask,
                                    L7_int32 *areaId)
{
  L7_RC_t rc;

  rc = ospfMapNetworkAreaGetNext(ipAddr, wildcardMask, areaId);

  return rc;
}

/*********************************************************************
* @purpose  Create/Update a network area entry.
*
* @param    ipAddr        @b{(input)} ip address
* @param    wildcardMask  @b{(input)} wild card mask
* @param    areaId        @b{(input)} area associated with the address
*
* @returns  L7_SUCCESS  if entry is successfully added to the list
* @returns  L7_FAILURE  if entry can't be added to the list
*
* @notes    This command defines the interfaces covered by the network
*           ipAddr on which OSPF runs and also defines the area ID
*           for those interfaces.
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaEntryCreate (L7_uint32 ipAddr,
                                       L7_uint32 wildcardMask,
                                       L7_uint32 areaId)
{
  L7_RC_t rc;
  L7_uint32 areaIndex;

  /* add the command to the operational list of "network area" commands */
  rc = ospfMapNetworkAreaUpdate(ipAddr, wildcardMask, areaId);

  if(rc == L7_ALREADY_CONFIGURED)
    return L7_SUCCESS; /* command already exists */
  else if(rc == L7_FAILURE)
    return L7_FAILURE; /* commands might have reached max capacity */

  /* Try to find the index for the specified area, if it is not found
  ** create a new area
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER);

  /* Set the config changed flag now,
   * Shall copy to pOspfMapCfgData structure when user
   * saves the config to NVRAM */
  ospfDataChangedSet(__FUNCTION__);

  /* Apply the existing network commands
   * if they apply to the routing interfaces */
  if (ospfMapIntfModeAndAreaCompute(0) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to create \"Network %x %x area %x\"\n",
            ipAddr, wildcardMask, areaId);
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_FAILURE;
  }

  osapiSemaGive(ospfMapCtrl_g.cfgSema);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a network area entry.
*
* @param    ipAddr        @b{(input)} ip address
* @param    wildcardMask  @b{(input)} wild card mask
* @param    areaId        @b{(input)} area associated with the address
*
* @returns  L7_SUCCESS  if the network command is successfully deleted
* @returns  L7_FAILURE  if the network command has not existed
*
* @notes    This command defines the interfaces covered by the network
*           ipAddr on which OSPF runs and also defines the area ID
*           for those interfaces.
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaEntryDelete (L7_uint32 ipAddr,
                                       L7_uint32 wildcardMask,
                                       L7_uint32 areaId)
{
  L7_RC_t   rc;

  rc = L7_SUCCESS;

  /* delete the command from the operational list of "network area" commands */
  if(ospfMapNetworkAreaDelete(ipAddr, wildcardMask, areaId)
                           != L7_SUCCESS)
    return L7_FAILURE; /* the command doesn't exist */

  osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER);

  /* Set the config changed flag now,
   * Shall copy to pOspfMapCfgData structure when user
   * saves the config to NVRAM */
  ospfDataChangedSet(__FUNCTION__);

  /* Apply the existing network commands
   * if they apply to the routing interfaces */
  if (ospfMapIntfModeAndAreaCompute(0) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to delete \"Network %x %x area %x\"\n",
            ipAddr, wildcardMask, areaId);
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_FAILURE;
  }

  osapiSemaGive(ospfMapCtrl_g.cfgSema);

  return L7_SUCCESS;
}

/*
**********************************************************************
*                    API FUNCTIONS  -  NSSA CONFIG  (RFC 3101)
**********************************************************************
*/


/*********************************************************************
* @purpose  Get the information associated with NSSA configuration
*
* @param    areaId       area id
* @param    *metric      pointer to stub area metric
* @param    *metricType  pointer to stub area metric type
*
* @returns  L7_SUCCESS  if NSSA configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    Placeholder for private MIB support for OSPF NSSAs
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSAEntryGet ( L7_uint32 areaId, L7_uint32 *metric,
                              L7_uint32 *metricType)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Given a NSSA area specification, return the area ID
*           of the next NSSA area
*
* @param    areaId      area id
*
* @returns  L7_SUCCESS  if NSSA configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    Placeholder for private MIB support for OSPF NSSAs
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSAEntryNext ( L7_uint32 *areaId, L7_uint32 *TOS)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    areaId  area identifier
* @param    status  used to create and delete nssa configurations
*                   (L7_OSPF_ROW_STATUS_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperable
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSAStatusSet (L7_uint32 areaId, L7_int32 status)
{
  if(ospfMapAreaEntryExists(areaId)  == L7_SUCCESS)
    return ospfMapExtenNSSAStatusSet(areaId, status);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the NSSA Translator Role of the specified NSSA
*
* @param    areaId    areaID
* @param    *nssaTR   NSSA translator role (L7_OSPF_NSSA_TRANSLATOR_ROLE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " This variable Specifies whether or not an NSSA border router will
*        unconditionally translate Type-7 LSAs into Type-5 LSAs.  When
*        it is set to Always, an NSSA border router always translates
*        Type-7 LSAs into Type-5 LSAs regardless of the translator state
*        of other NSSA border routers.  When it is set to Candidate, an
*        NSSA border router participates in the translator election
*        process described in Section 3.1.  The default setting is
*        Candidate"
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSATranslatorRoleGet (L7_uint32 areaId, L7_uint32 *nssaTR)
{
  L7_uint32 areaIndex;

  if(nssaTR == L7_NULLPTR)
    return L7_FAILURE;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if(pOspfMapCfgData->area[areaIndex].extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA)
    {
      *nssaTR = pOspfMapCfgData->area[areaIndex].nssaCfg.translatorRole;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the NSSA Translator Role of the specified NSSA
*
* @param    areaId    areaID
* @param    nssaTR   NSSA translator role (L7_OSPF_NSSA_TRANSLATOR_ROLE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_REQUEST_DENIED    if area is backbone
*
* @notes " This variable Specifies whether or not an NSSA border router will
*        unconditionally translate Type-7 LSAs into Type-5 LSAs.  When
*        it is set to Always, an NSSA border router always translates
*        Type-7 LSAs into Type-5 LSAs regardless of the translator state
*        of other NSSA border routers.  When it is set to Candidate, an
*        NSSA border router participates in the translator election
*        process described in Section 3.1.  The default setting is
*        Candidate"
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSATranslatorRoleSet (L7_uint32 areaId, L7_uint32 nssaTR)
{
  L7_uint32 areaIndex;

  if (areaId == OSPF_BACKBONE_AREA_ID)
  {
    return L7_REQUEST_DENIED;
  }

  if ((nssaTR !=  L7_OSPF_NSSA_TRANSLATOR_ALWAYS) &&
      (nssaTR !=  L7_OSPF_NSSA_TRANSLATOR_CANDIDATE))
    return L7_FAILURE;

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* Save the configuration information */
  pOspfMapCfgData->area[areaIndex].extRoutingCapability = L7_OSPF_AREA_IMPORT_NSSA;
  pOspfMapCfgData->area[areaIndex].nssaCfg.translatorRole = nssaTR;
  ospfDataChangedSet(__FUNCTION__);

  /* If the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() == L7_TRUE) &&
     (ospfMapExtenAreaExists(areaId) == L7_SUCCESS))
    return ospfMapExtenNSSATranslatorRoleSet(areaId,nssaTR);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the NSSA Translator State of the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId    areaID
* @param    *nssaTR   NSSA translator state (L7_OSPF_NSSA_TRANSLATOR_STATE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " This variable specifies the translator state of a NSSA border
*        router.  The translator state is determined by translator
*        election taking into account the user configured NSSA Translator
*        Role and is a read-only parm."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSATranslatorStateGet (L7_uint32 areaId, L7_uint32 *nssaTRState)
{
  if(nssaTRState == L7_NULLPTR)
    return L7_FAILURE;

  if(ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenNSSATranslatorStateGet(areaId, nssaTRState);

  *nssaTRState = L7_OSPF_NSSA_TRANS_STATE_DISABLED;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Translator Stability Interval of the specified NSSA
*
* @param    areaId     areaID
* @param    *stabInt   TranslatorStabilityInterval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " This variable defines the length of time an elected Type-7
*        translator will continue to perform its translator duties once
*        it has determined that its translator status has been deposed by
*        another NSSA border router translator as described in Section
*        3.1 and 3.3.  The default setting is 40 seconds."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSATranslatorStabilityIntervalGet (L7_uint32 areaId,
                                                   L7_uint32 *stabInt)
{
  L7_uint32 areaIndex;

  if(stabInt == L7_NULLPTR)
    return L7_FAILURE;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if(pOspfMapCfgData->area[areaIndex].extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA)
    {
      *stabInt = pOspfMapCfgData->area[areaIndex].nssaCfg.translatorStabilityInterval;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the Translator Stability Interval of the specified NSSA
*
* @param    areaId     areaID
* @param    *stabInt   TranslatorStabilityInterval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_REQUEST_DENIED    if area is backbone
*
* @notes " This variable defines the length of time an elected Type-7
*        translator will continue to perform its translator duties once
*        it has determined that its translator status has been deposed by
*        another NSSA border router translator as described in Section
*        3.1 and 3.3.  The default setting is 40 seconds."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSATranslatorStabilityIntervalSet (L7_uint32 areaId,
                                                   L7_uint32 stabInt)
{
  L7_uint32 areaIndex;

  if (areaId == OSPF_BACKBONE_AREA_ID)
  {
    return L7_REQUEST_DENIED;
  }

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* Save the configuration information */
  pOspfMapCfgData->area[areaIndex].extRoutingCapability = L7_OSPF_AREA_IMPORT_NSSA;
  pOspfMapCfgData->area[areaIndex].nssaCfg.translatorStabilityInterval = stabInt;
  ospfDataChangedSet(__FUNCTION__);

  /* If the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() == L7_TRUE) &&
     (ospfMapExtenAreaExists(areaId) == L7_SUCCESS))
    return ospfMapExtenNSSATranslatorStabilityIntervalSet(areaId,stabInt);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the import summary configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    *impSum    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " When set to enabled, OSPF's summary routes are imported into
*        the NSSA as Type-3 summary-LSAs.  When set to disabled, summary
*        routes are not imported into the NSSA.  The default setting is
*        enabled."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSAImportSummariesGet (L7_uint32 areaId, L7_uint32 *impSum)
{
  L7_uint32 areaIndex;

  if(impSum == L7_NULLPTR)
    return L7_FAILURE;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if(pOspfMapCfgData->area[areaIndex].extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA)
    {
      *impSum = pOspfMapCfgData->area[areaIndex].nssaCfg.importSummaries;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the import summary configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    impSum    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_REQUEST_DENIED if area is the backbone
*
* @notes " When set to enabled, OSPF's summary routes are imported into
*        the NSSA as Type-3 summary-LSAs.  When set to disabled, summary
*        routes are not imported into the NSSA.  The default setting is
*        enabled."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSAImportSummariesSet (L7_uint32 areaId, L7_uint32 impSum)
{
  L7_uint32 areaIndex;

  if (areaId == OSPF_BACKBONE_AREA_ID)
  {
    return L7_REQUEST_DENIED;
  }

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* Save the configuration information */
  pOspfMapCfgData->area[areaIndex].extRoutingCapability = L7_OSPF_AREA_IMPORT_NSSA;
  pOspfMapCfgData->area[areaIndex].nssaCfg.importSummaries = impSum;
  ospfDataChangedSet(__FUNCTION__);

  /* If the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() == L7_TRUE) &&
     (ospfMapExtenAreaExists(areaId) == L7_SUCCESS))
    return ospfMapExtenNSSAImportSummariesSet(areaId,impSum);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the route redistribution configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    *redist    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " L7_ENABLE/ L7_DISABLE (Used when the router is a NSSA ABR
*          and you want the redistribute command to import routes only
*          into the normal areas, but not into the NSSA area)."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSARedistributeGet (L7_uint32 areaId, L7_uint32 *redist)
{
  L7_uint32 areaIndex;

  if(redist == L7_NULLPTR)
    return L7_FAILURE;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if(pOspfMapCfgData->area[areaIndex].extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA)
    {
      *redist = pOspfMapCfgData->area[areaIndex].nssaCfg.redistribute;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the route redistribution configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    redist    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_REQUEST_DENIED   if area is BB
*
* @notes " L7_ENABLE/ L7_DISABLE (Used when the router is a NSSA ABR
*          and you want the redistribute command to import routes only
*          into the normal areas, but not into the NSSA area)."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSARedistributeSet (L7_uint32 areaId, L7_uint32 redist)
{
  L7_uint32 areaIndex;

  if (areaId == OSPF_BACKBONE_AREA_ID)
  {
    return L7_REQUEST_DENIED;
  }

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* Save the configuration information */
  pOspfMapCfgData->area[areaIndex].extRoutingCapability = L7_OSPF_AREA_IMPORT_NSSA;
  pOspfMapCfgData->area[areaIndex].nssaCfg.redistribute = redist;
  ospfDataChangedSet(__FUNCTION__);

  /* If the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() == L7_TRUE) &&
     (ospfMapExtenAreaExists(areaId) == L7_SUCCESS))
    return ospfMapExtenNSSARedistributeSet(areaId,redist);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the default information origination configuration for
*           the specified NSSA, includes metric & metric type
*
* @param    areaId      areaID
* @param    *defInfoOrg L7_TRUE/ L7_FALSE
* @param    *metricType metric type
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultInfoGet (L7_uint32 areaId, L7_uint32 *defInfoOrg,
                                   L7_uint32 *metric,
                                   L7_OSPF_STUB_METRIC_TYPE_t *metricType)
{
  L7_uint32 areaIndex;

  if((defInfoOrg == L7_NULLPTR) || (metricType == L7_NULLPTR) || (metric == L7_NULLPTR))
    return L7_FAILURE;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if(pOspfMapCfgData->area[areaIndex].extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA)
    {
      *defInfoOrg = pOspfMapCfgData->area[areaIndex].nssaCfg.defInfoOrig;
      *metricType = pOspfMapCfgData->area[areaIndex].nssaCfg.defMetricType;
      *metric     = pOspfMapCfgData->area[areaIndex].nssaCfg.defMetric;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the default information origination configuration for
*           the specified NSSA
*
* @param    areaId      areaID
* @param    defInfoOrg L7_TRUE/ L7_FALSE
* @param    metricType metric type
* @param    metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_REQUEST_DENIED   if area is backbone
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultInfoSet(L7_uint32 areaId, L7_uint32 defInfoOrg,
                                  L7_OSPF_STUB_METRIC_TYPE_t metricType,
                                  L7_uint32 metric)
{
  L7_uint32 areaIndex;

  if (areaId == OSPF_BACKBONE_AREA_ID)
  {
    return L7_REQUEST_DENIED;
  }

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* Save the configuration information */
  pOspfMapCfgData->area[areaIndex].extRoutingCapability   = L7_OSPF_AREA_IMPORT_NSSA;
  pOspfMapCfgData->area[areaIndex].nssaCfg.defInfoOrig    = defInfoOrg;
  pOspfMapCfgData->area[areaIndex].nssaCfg.defMetric      = metric;
  pOspfMapCfgData->area[areaIndex].nssaCfg.defMetricType  = metricType;
  ospfDataChangedSet(__FUNCTION__);

  /* If the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() == L7_TRUE) &&
     (ospfMapExtenAreaExists(areaId) == L7_SUCCESS))
    return ospfMapExtenNSSADefaultInfoSet(areaId, defInfoOrg, metricType, metric);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the default information origination configuration for
*           the specified NSSA
*
* @param    areaId        areaID
* @param    *defInfoOrg  L7_TRUE/ L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " L7_TRUE/ L7_FALSE (Used to generate a Type 7 default into
*          the NSSA area. This argument takes effect only on a NSSA ABR)."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultInfoOriginateGet (L7_uint32 areaId, L7_uint32 *defInfoOrg)
{
  L7_uint32 areaIndex;

  if(defInfoOrg == L7_NULLPTR)
    return L7_FAILURE;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if(pOspfMapCfgData->area[areaIndex].extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA)
    {
      *defInfoOrg = pOspfMapCfgData->area[areaIndex].nssaCfg.defInfoOrig;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the default information origination configuration for
*           the specified NSSA
*
* @param    areaId        areaID
* @param    defInfoOrg  L7_TRUE/ L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " L7_TRUE/ L7_FALSE (Used to generate a Type 7 default into
*          the NSSA area. This argument takes effect only on a NSSA ABR)."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultInfoOriginateSet (L7_uint32 areaId, L7_uint32 defInfoOrg)
{
  L7_uint32 areaIndex;

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* Save the configuration information */
  pOspfMapCfgData->area[areaIndex].extRoutingCapability = L7_OSPF_AREA_IMPORT_NSSA;
  pOspfMapCfgData->area[areaIndex].nssaCfg.defInfoOrig = defInfoOrg;
  ospfDataChangedSet(__FUNCTION__);

  /* If the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() == L7_TRUE) &&
     (ospfMapExtenAreaExists(areaId) == L7_SUCCESS))
    return ospfMapExtenNSSADefaultInfoOriginateSet(areaId, defInfoOrg);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the type of metric set as a default route.
*
* @param    areaId      areaID
* @param    *metricType metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultMetricTypeGet ( L7_uint32 areaId,
                                          L7_OSPF_STUB_METRIC_TYPE_t *metricType )
{
  L7_uint32 areaIndex;

  if(metricType == L7_NULLPTR)
    return L7_FAILURE;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if(pOspfMapCfgData->area[areaIndex].extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA)
    {
      *metricType = pOspfMapCfgData->area[areaIndex].nssaCfg.defMetricType;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the metric type of the default route for the NSSA.
*
* @param    areaId      areaID
* @param    metricType metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultMetricTypeSet ( L7_uint32 areaId,
                                          L7_OSPF_STUB_METRIC_TYPE_t metricType )
{
  L7_uint32 areaIndex;

  if ((metricType !=  L7_OSPF_AREA_STUB_METRIC) &&
      (metricType !=  L7_OSPF_AREA_STUB_COMPARABLE_COST) &&
      (metricType !=  L7_OSPF_AREA_STUB_NON_COMPARABLE_COST))
    return L7_FAILURE;

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* Save the configuration information */
  pOspfMapCfgData->area[areaIndex].extRoutingCapability = L7_OSPF_AREA_IMPORT_NSSA;
  pOspfMapCfgData->area[areaIndex].nssaCfg.defMetricType = metricType;
  ospfDataChangedSet(__FUNCTION__);

  /* If the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() == L7_TRUE) &&
     (ospfMapExtenAreaExists(areaId) == L7_SUCCESS))
    return ospfMapExtenNSSADefaultMetricTypeSet(areaId, metricType);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the NSSA metric value.
*
* @param    areaId      area ID
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultMetricGet ( L7_uint32 areaId, L7_uint32 *metric )
{
  L7_uint32 areaIndex;

  if(metric == L7_NULLPTR)
    return L7_FAILURE;

  /* Check to see if this entry exists */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
  {
    if(pOspfMapCfgData->area[areaIndex].extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA)
    {
      *metric = pOspfMapCfgData->area[areaIndex].nssaCfg.defMetric;
      return L7_SUCCESS;
    }
  }

  /* Not a configured NSSA */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the value of the default metric for the given NSSA
*
* @param    areaId      areaID
* @param    metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultMetricSet ( L7_uint32 areaId, L7_uint32 metric )
{
    L7_uint32 areaIndex;

  /* Try to find the index for the specified area, if it is not found
  ** create a new area since this is a read-create parm
  */
  if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) &&
     (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
    return L7_FAILURE;

  /* Save the configuration information */
  pOspfMapCfgData->area[areaIndex].extRoutingCapability = L7_OSPF_AREA_IMPORT_NSSA;
  pOspfMapCfgData->area[areaIndex].nssaCfg.defMetric = metric;
  ospfDataChangedSet(__FUNCTION__);

  /* If the OSPF vendor code has been initialized and the area exists proceed
  ** with configuring the vendor area
  */
  if((ospfMapOspfInitialized() == L7_TRUE) &&
     (ospfMapExtenAreaExists(areaId) == L7_SUCCESS))
    return ospfMapExtenNSSADefaultMetricSet(areaId, metric);

  return L7_SUCCESS;
}

/* End Function Declarations */

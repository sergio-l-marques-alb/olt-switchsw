/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_pim.c
 *
 * @purpose  Provide interface to hardware API's PIM rfc2934 MIB components
 *
 * @component unitmgr
 *
 * @comments Also includes IPMROUTE MIB since PIM rfc2934 imports it.
 *
 * @create 02/04/2002
 *
 * @author Abdul Shareef(BRI)
 * @end
 *
 **********************************************************************/
/********************************************************************
 *
 *******************************************************************/
#include "k_private_base.h"
#include "k_mib_pim_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pim_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"
#include "usmdb_mib_pimsm_api.h"
#endif


L7_uint32 snmp_pim_mode = L7_SNMP_PIM_MODE_DENSE;

pim_t *
k_pim_get(int serialNum, ContextInfo *contextInfo,
    int nominator)
{
  static pim_t pimData;

  ZERO_VALID(pimData.valid);

  if (snmp_pim_mode == L7_SNMP_PIM_MODE_SPARSE)
  {
    switch(nominator)
    {
      case -1:
	break;

      case I_pimJoinPruneInterval:
#ifdef SNMP_FP_MCAST_MIB_TODO
	if(usmDbPimsmJoinPruneIntervalGet(USMDB_UNIT_CURRENT, &pimData.pimJoinPruneInterval) == L7_SUCCESS)
	  SET_VALID(I_pimJoinPruneInterval, pimData.valid);
#endif
	break;

      default:
	/* unknown nominator */
	return(NULL);
	break;
    }
  }

  if (nominator >= 0 && !VALID(nominator, pimData.valid))
    return(NULL);

  return(&pimData);
}

#ifdef SETS
int
k_pim_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_pim_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_pim_set(pim_t *data,
    ContextInfo *contextInfo, int function)
{
#ifdef SNMP_FP_MCAST_MIB_TODO
  if (VALID(I_pimJoinPruneInterval, data->valid) &&
      usmDbPimsmJoinPruneIntervalSet(USMDB_UNIT_CURRENT,data->pimJoinPruneInterval, L7_AF_INET) != L7_SUCCESS)
#endif
  {
    CLR_VALID(I_pimJoinPruneInterval, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  return NO_ERROR;
}

#ifdef SR_pim_UNDO
/* add #define SR_pim_UNDO in sitedefs.h to
 * include the undo routine for the pim family.
 */
int
pim_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_pim_UNDO */

#endif /* SETS */

pimInterfaceEntry_t *
k_pimInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_INT32 pimInterfaceIfIndex)
{
 
  

  static pimInterfaceEntry_t pimInterfaceEntryData;
  L7_uint32 intIfNum;
  L7_inet_addr_t pimIpSrc, pimIpMask, pimIntfDR;

  /* Clear all the bits*/
  
  ZERO_VALID(pimInterfaceEntryData.valid);
  pimInterfaceEntryData.pimInterfaceIfIndex = pimInterfaceIfIndex;
  SET_VALID(I_pimInterfaceIfIndex, pimInterfaceEntryData.valid);


  inetAddressReset(&pimIpSrc);
  inetAddressReset(&pimIpMask);
  inetAddressReset(&pimIntfDR);
if (snmp_pim_mode == L7_SNMP_PIM_MODE_DENSE)
{ 
 if ( (searchType == EXACT) ?
	( (usmDbIntIfNumFromExtIfNum(pimInterfaceEntryData.pimInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
	  (usmDbPimInterfaceEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum) != L7_SUCCESS) ) :

	( ( (usmDbExtIfNumTypeCheckValid(USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, pimInterfaceEntryData.pimInterfaceIfIndex) != L7_SUCCESS) &&
	    (usmDbExtIfNumTypeNextGet(USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, pimInterfaceEntryData.pimInterfaceIfIndex, &pimInterfaceEntryData.pimInterfaceIfIndex) != L7_SUCCESS) ) ||
	  (usmDbIntIfNumFromExtIfNum(pimInterfaceEntryData.pimInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
	  ( (usmDbPimInterfaceEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum) != L7_SUCCESS) &&
	    (usmDbPimInterfaceEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET, &intIfNum) != L7_SUCCESS)) ||
	  (usmDbExtIfNumFromIntIfNum(intIfNum, &pimInterfaceEntryData.pimInterfaceIfIndex) != L7_SUCCESS) )
       )
    { 
      return(NULL);
    }


   switch (nominator)
    {
      case -1:
      case I_pimInterfaceIfIndex: /* already got it above*/
	break;

      case I_pimInterfaceAddress:
	if (usmDbPimInterfaceIPAddressGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,
	      &pimIpSrc) == L7_SUCCESS )
   {
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimIpSrc, &pimInterfaceEntryData.pimInterfaceAddress))
      return(NULL);
	  SET_VALID(I_pimInterfaceAddress, pimInterfaceEntryData.valid);
   }
	break;

      case I_pimInterfaceNetMask:
	if (usmDbPimInterfaceNetMaskGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,
	      &pimIpMask) == L7_SUCCESS )
  {
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimIpMask, &pimInterfaceEntryData.pimInterfaceNetMask))
      return(NULL);
	  SET_VALID(I_pimInterfaceNetMask, pimInterfaceEntryData.valid);
  }
	break;

      case I_pimInterfaceMode:
	pimInterfaceEntryData.pimInterfaceMode = D_pimInterfaceMode_dense;
	SET_VALID(I_pimInterfaceMode, pimInterfaceEntryData.valid);
	break;
      case I_pimInterfaceDR:
	if (usmDbPimInterfaceDRGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,
	      &pimIntfDR) == L7_SUCCESS )
  {
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimIntfDR, &pimInterfaceEntryData.pimInterfaceDR))
      return(NULL);
	  SET_VALID(I_pimInterfaceDR, pimInterfaceEntryData.valid);
  }
	break;
      case I_pimInterfaceHelloInterval:
	if (usmDbPimInterfaceHelloIntervalGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,
	      &pimInterfaceEntryData.pimInterfaceHelloInterval) == L7_SUCCESS )
	  SET_VALID(I_pimInterfaceHelloInterval, pimInterfaceEntryData.valid);
	break;

      
      case I_pimInterfaceStatus:
	if (snmpPimInterfaceStatusGet(USMDB_UNIT_CURRENT, intIfNum,
	      &pimInterfaceEntryData.pimInterfaceStatus) == L7_SUCCESS )
        SET_VALID(I_pimInterfaceStatus, pimInterfaceEntryData.valid);
       
	break;

      
      case I_pimInterfaceJoinPruneInterval:
	if (usmDbPimInterfaceJoinPruneIntervalGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,
	      &pimInterfaceEntryData.pimInterfaceJoinPruneInterval) == L7_SUCCESS )
	  SET_VALID(I_pimInterfaceJoinPruneInterval, pimInterfaceEntryData.valid);
	break;
      case I_pimInterfaceCBSRPreference:
	if (usmDbPimInterfaceCBSRPreferenceGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,
	      &pimInterfaceEntryData.pimInterfaceCBSRPreference) == L7_SUCCESS )
	  SET_VALID(I_pimInterfaceCBSRPreference, pimInterfaceEntryData.valid);
	break;

      default:
	/* unknown nominator */
	return(NULL);
	break;
    }
  }
  else if (snmp_pim_mode == L7_SNMP_PIM_MODE_SPARSE)
  {
    if ( (searchType == EXACT) ?
	( (usmDbIntIfNumFromExtIfNum(pimInterfaceEntryData.pimInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
	  (usmDbPimsmSNMPInterfaceEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum) != L7_SUCCESS) ) :

	( ( (usmDbExtIfNumTypeCheckValid(USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, pimInterfaceEntryData.pimInterfaceIfIndex) != L7_SUCCESS) &&
	    (usmDbExtIfNumTypeNextGet(USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, pimInterfaceEntryData.pimInterfaceIfIndex, &pimInterfaceEntryData.pimInterfaceIfIndex) != L7_SUCCESS) ) ||
	  (usmDbIntIfNumFromExtIfNum(pimInterfaceEntryData.pimInterfaceIfIndex, &intIfNum) != L7_SUCCESS) ||
	  ( (usmDbPimsmSNMPInterfaceEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum) != L7_SUCCESS) &&
	    (usmDbPimsmSNMPInterfaceEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET, &intIfNum) != L7_SUCCESS)) ||
	  (usmDbExtIfNumFromIntIfNum(intIfNum, &pimInterfaceEntryData.pimInterfaceIfIndex) != L7_SUCCESS) )
       )
    {
      return(NULL);
    }

    switch (nominator)
    {
      case -1:
      case I_pimInterfaceIfIndex: /* already got it above*/
	break;

      case I_pimInterfaceAddress:
	if (usmDbPimsmInterfaceIPAddressGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,
	      &pimIpSrc) == L7_SUCCESS )
  {
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimIpSrc, &pimInterfaceEntryData.pimInterfaceAddress))
      return(NULL);
	  SET_VALID(I_pimInterfaceAddress, pimInterfaceEntryData.valid);
  }
	break;

      case I_pimInterfaceNetMask:
	if (usmDbPimsmInterfaceNetMaskGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,
	      &pimIpMask) == L7_SUCCESS )
  {
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimIpMask, &pimInterfaceEntryData.pimInterfaceNetMask))
      return(NULL);
	  SET_VALID(I_pimInterfaceNetMask, pimInterfaceEntryData.valid);
  }
	break;

      case I_pimInterfaceMode:
	pimInterfaceEntryData.pimInterfaceMode = D_pimInterfaceMode_sparse;
	SET_VALID(I_pimInterfaceMode, pimInterfaceEntryData.valid);
	break;
      case I_pimInterfaceDR:
	if (usmDbPimsmInterfaceDRGet(USMDB_UNIT_CURRENT, L7_AF_INET, intIfNum,
	      &pimIntfDR) == L7_SUCCESS )
  {
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimIntfDR, &pimInterfaceEntryData.pimInterfaceDR))
      return(NULL);
	  SET_VALID(I_pimInterfaceDR, pimInterfaceEntryData.valid);
  }
	break;
      case I_pimInterfaceHelloInterval:
	if (usmDbPimsmInterfaceHelloIntervalGet(USMDB_UNIT_CURRENT, L7_AF_INET, 
        intIfNum,
	      &pimInterfaceEntryData.pimInterfaceHelloInterval) == L7_SUCCESS )
	  SET_VALID(I_pimInterfaceHelloInterval, pimInterfaceEntryData.valid);
	break;

       
      case I_pimInterfaceStatus:
	if (snmpPimInterfaceStatusGet(USMDB_UNIT_CURRENT, intIfNum,
	      &pimInterfaceEntryData.pimInterfaceStatus) == L7_SUCCESS )
	  SET_VALID(I_pimInterfaceStatus, pimInterfaceEntryData.valid);
	break;
       

      case I_pimInterfaceJoinPruneInterval:
#ifdef PIMSM_FOR_FUTURE_ENHANCEMENT
	if (usmDbPimsmInterfaceJoinPruneIntervalGet(USMDB_UNIT_CURRENT, intIfNum,
	      &pimInterfaceEntryData.pimInterfaceJoinPruneInterval) == L7_SUCCESS )
	  SET_VALID(I_pimInterfaceJoinPruneInterval, pimInterfaceEntryData.valid);
#endif /*PIMSM_FOR_FUTURE_ENHANCEMENT*/
	break;
      case I_pimInterfaceCBSRPreference:
	if (usmDbPimsmInterfaceCBSRPreferenceGet(USMDB_UNIT_CURRENT, L7_AF_INET, 
        intIfNum,
	      &pimInterfaceEntryData.pimInterfaceCBSRPreference) == L7_SUCCESS )
	  SET_VALID(I_pimInterfaceCBSRPreference, pimInterfaceEntryData.valid);
	break;

      default:
	/* unknown nominator */
	return(NULL);
	break;
    }
  }


  if (nominator >= 0 && !VALID(nominator, pimInterfaceEntryData.valid))
    return(NULL);
  return(&pimInterfaceEntryData);
 }

#ifdef SETS
int
k_pimInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{
  
  /* Following line implemented per VLAN mib implementaion...*/
  ZERO_VALID(((pimInterfaceEntry_t *) (dp->data))->valid);
  return NO_ERROR;
}

int
k_pimInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}  
  int
k_pimInterfaceEntry_set_defaults(doList_t *dp)
{
   
  pimInterfaceEntry_t *data = (pimInterfaceEntry_t *) (dp->data);

  data->pimInterfaceMode = D_pimInterfaceMode_dense;
  data->pimInterfaceHelloInterval = 30;
  data->pimInterfaceCBSRPreference = 0;
  data->pimInterfaceStatus = D_pimInterfaceStatus_active;
  SET_ALL_VALID(data->valid);
  return NO_ERROR;
  
}

int
k_pimInterfaceEntry_set(pimInterfaceEntry_t *data,
    ContextInfo *contextInfo, int function)
{

  L7_RC_t rc;
  SR_INT32 pimInterfaceIfIndex;

   /* Getting internal interface number */
  rc = usmDbIntIfNumFromExtIfNum(data->pimInterfaceIfIndex,&pimInterfaceIfIndex);
  if (rc != L7_SUCCESS)
  {
    return (COMMIT_FAILED_ERROR);
  }
  data->pimInterfaceIfIndex = pimInterfaceIfIndex;
  
  if (VALID(I_pimInterfaceMode, data->valid))
  {
    if(data->pimInterfaceMode!=D_pimInterfaceMode_dense)  
    return(NO_SUCH_NAME_ERROR);
  }

  if (snmp_pim_mode == L7_SNMP_PIM_MODE_DENSE)
  {
    if (VALID(I_pimInterfaceStatus, data->valid))
    {
      
      if ( snmpPimInterfaceStatusSet(USMDB_UNIT_CURRENT, data->pimInterfaceIfIndex,
	    data->pimInterfaceStatus) != L7_SUCCESS)
      {
	CLR_VALID(I_pimInterfaceStatus, data->valid);
	return (COMMIT_FAILED_ERROR);
      }
     
    }

    if (VALID(I_pimInterfaceHelloInterval, data->valid))
    {
      if ( !((data->pimInterfaceHelloInterval >= L7_PIMDM_HELLOINTERVAL_MIN) &&
	    (data->pimInterfaceHelloInterval <= L7_PIMDM_HELLOINTERVAL_MAX)) ||
	   usmDbPimInterfaceHelloIntervalSet(USMDB_UNIT_CURRENT,L7_AF_INET, data->pimInterfaceIfIndex,
	    data->pimInterfaceHelloInterval) != L7_SUCCESS)
	return(COMMIT_FAILED_ERROR);
    }

    if (VALID(I_pimInterfaceJoinPruneInterval, data->valid))
    {
      /* Only in SM*/
      if (usmDbPimInterfaceJoinPruneIntervalSet(USMDB_UNIT_CURRENT, data->pimInterfaceIfIndex,
	    data->pimInterfaceJoinPruneInterval, L7_AF_INET) != L7_SUCCESS)
	return(COMMIT_FAILED_ERROR);
    }
    if (VALID(I_pimInterfaceCBSRPreference, data->valid))
    {
      /* Only in SM */
      if (usmDbPimInterfaceCBSRPreferenceSet(USMDB_UNIT_CURRENT, data->pimInterfaceIfIndex,
	    data->pimInterfaceCBSRPreference, L7_AF_INET) != L7_SUCCESS)
	return(COMMIT_FAILED_ERROR);
    }
  }

  else if (snmp_pim_mode == L7_SNMP_PIM_MODE_SPARSE)
  {
    if (VALID(I_pimInterfaceStatus, data->valid))
    {
      
      if ( snmpPimInterfaceStatusSet(USMDB_UNIT_CURRENT, data->pimInterfaceIfIndex,
	    data->pimInterfaceStatus) != L7_SUCCESS)
      {
	CLR_VALID(I_pimInterfaceStatus, data->valid);
	return (COMMIT_FAILED_ERROR);
      }
      
    }

    if (VALID(I_pimInterfaceHelloInterval, data->valid))
    {
      if ( !((data->pimInterfaceHelloInterval >= L7_PIMSM_INTERFACE_HELLO_INTERVAL_MIN) &&
	    (data->pimInterfaceHelloInterval <= L7_PIMSM_INTERFACE_HELLO_INTERVAL_MAX)) ||
	  usmDbPimsmInterfaceHelloIntervalSet(USMDB_UNIT_CURRENT, L7_AF_INET, data->pimInterfaceIfIndex,
	    data->pimInterfaceHelloInterval) != L7_SUCCESS)
	return(COMMIT_FAILED_ERROR);
    }
    if (VALID(I_pimInterfaceJoinPruneInterval, data->valid))
    {
#ifdef PIMSM_FOR_FUTURE_ENHANCEMENT

      if (usmDbPimsmInterfaceJoinPruneIntervalSet(USMDB_UNIT_CURRENT, data->pimInterfaceIfIndex,
	    data->pimInterfaceJoinPruneInterval) != L7_SUCCESS)
	return(COMMIT_FAILED_ERROR);
#else
      return NO_SUCH_NAME_ERROR;
#endif
    
    }
    if (VALID(I_pimInterfaceCBSRPreference, data->valid))
    {
#ifdef PIMSM_FOR_FUTURE_ENHANCEMENT
      if (usmDbPimsmInterfaceCBSRPreferenceSet(USMDB_UNIT_CURRENT, L7_AF_INET, data->pimInterfaceIfIndex,
	    data->pimInterfaceCBSRPreference) != L7_SUCCESS)
	return(COMMIT_FAILED_ERROR);
      
#else
      return NO_SUCH_NAME_ERROR;
#endif
      
    }
  
}
 
 return NO_ERROR;
 
}

#ifdef SR_pimInterfaceEntry_UNDO
/* add #define SR_pimInterfaceEntry_UNDO in sitedefs.h to
 * include the undo routine for the pimInterfaceEntry family.
 */
int
pimInterfaceEntry_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_pimInterfaceEntry_UNDO */

#endif /* SETS */

pimNeighborEntry_t *
k_pimNeighborEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_UINT32 pimNeighborAddress)
{

  
  static pimNeighborEntry_t pimNeighborEntryData;
  L7_uint32 tmpIntIfNum = 0;
  L7_inet_addr_t pimIpSrc, pimIpMask, pimIntfDR;

  /* Clear all the bits*/
  ZERO_VALID(pimNeighborEntryData.valid);
  pimNeighborEntryData.pimNeighborAddress = pimNeighborAddress;
  SET_VALID(I_pimNeighborAddress, pimNeighborEntryData.valid);

  inetAddressZeroSet(L7_AF_INET, &pimIpSrc);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &pimNeighborAddress, &pimIpSrc))
    return (NULL);
  inetAddressReset(&pimIpMask);
  inetAddressReset(&pimIntfDR);

  /*if(L7_SUCCESS != usmDbPimNeighborIfIndexGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpSrc, &tmpIntIfNum))
    return (NULL);*/

    if ((searchType == EXACT) ?
	(usmDbPimNeighborEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, tmpIntIfNum, &pimIpSrc) != L7_SUCCESS) :
	((usmDbPimNeighborEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, tmpIntIfNum, &pimIpSrc) != L7_SUCCESS) &&
	 (usmDbPimNeighborEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET, &tmpIntIfNum, &pimIpSrc) != L7_SUCCESS)))
    {
      return(NULL);
    }


  if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimIpSrc, &pimNeighborEntryData.pimNeighborAddress))
    return (NULL);

    switch (nominator)
    {
      case -1:
      case I_pimNeighborAddress: /* already got it above*/
	break;

      case I_pimNeighborIfIndex:

	if ((usmDbPimNeighborIfIndexGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpSrc,
		&tmpIntIfNum) == L7_SUCCESS) &&
	    usmDbExtIfNumFromIntIfNum(tmpIntIfNum,&pimNeighborEntryData.pimNeighborIfIndex) == L7_SUCCESS)
	  SET_VALID(I_pimNeighborIfIndex, pimNeighborEntryData.valid);
	break;

      case I_pimNeighborUpTime:
	if (snmpPimNeighborUpTimeGet(USMDB_UNIT_CURRENT, pimNeighborEntryData.pimNeighborAddress,
	      &pimNeighborEntryData.pimNeighborUpTime) == L7_SUCCESS )
	  SET_VALID(I_pimNeighborUpTime, pimNeighborEntryData.valid);
	break;

      case I_pimNeighborExpiryTime:
	if (snmpPimNeighborExpiryTimeGet(USMDB_UNIT_CURRENT, pimNeighborEntryData.pimNeighborAddress,
	      &pimNeighborEntryData.pimNeighborExpiryTime) == L7_SUCCESS )
	  SET_VALID(I_pimNeighborExpiryTime, pimNeighborEntryData.valid);
	break;

      case I_pimNeighborMode:
	if (usmDbPimNeighborModeGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpSrc,
	      &pimNeighborEntryData.pimNeighborMode) == L7_SUCCESS )
	  SET_VALID(I_pimNeighborMode, pimNeighborEntryData.valid);
	break;

      default:
	/* unknown nominator */
	return(NULL);
	break;
    }

  if (nominator >= 0 && !VALID(nominator, pimNeighborEntryData.valid))
    return(NULL);

  return(&pimNeighborEntryData);
}

pimIpMRouteEntry_t *
k_pimIpMRouteEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_UINT32 ipMRouteGroup,
    SR_UINT32 ipMRouteSource,
    SR_UINT32 ipMRouteSourceMask)
{
  
  static pimIpMRouteEntry_t pimIpMRouteEntryData;
  L7_uint32 seconds;
  L7_uint32 flags;
  L7_uchar8 val8;
  static L7_BOOL firstTime = L7_TRUE;
  L7_inet_addr_t pimIpSrc, pimIpMask, pimIpGrp;

  /* Clear all the bits*/
  ZERO_VALID(pimIpMRouteEntryData.valid);
  pimIpMRouteEntryData.ipMRouteGroup = ipMRouteGroup;
  pimIpMRouteEntryData.ipMRouteSource = ipMRouteSource;
  pimIpMRouteEntryData.ipMRouteSourceMask = ipMRouteSourceMask;
  SET_VALID(I_pimIpMRouteEntryIndex_ipMRouteGroup, pimIpMRouteEntryData.valid);
  SET_VALID(I_pimIpMRouteEntryIndex_ipMRouteSource, pimIpMRouteEntryData.valid);
  SET_VALID(I_pimIpMRouteEntryIndex_ipMRouteSourceMask, pimIpMRouteEntryData.valid);

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    pimIpMRouteEntryData.pimIpMRouteFlags = MakeOctetString(NULL, 0);
  }

  inetAddressReset(&pimIpSrc);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteSource, &pimIpSrc))
    return (NULL);
  inetAddressReset(&pimIpMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteSourceMask, &pimIpMask))
    return (NULL);
  inetAddressReset(&pimIpGrp);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteGroup, &pimIpGrp))
    return (NULL);

  if (snmp_pim_mode == L7_SNMP_PIM_MODE_DENSE)
  {
#ifdef SUPPORTED
    if ((searchType == EXACT) ?
	(usmDbPimIpMRouteEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask) != L7_SUCCESS) :
	((usmDbPimIpMRouteEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp,&pimIpSrc,&pimIpMask) != L7_SUCCESS) &&
	 (usmDbPimIpMRouteEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp,&pimIpSrc,&pimIpMask) != L7_SUCCESS)))
#endif
    {
      return(NULL);
    }


    switch (nominator)
    {
      case -1:
      case I_pimIpMRouteEntryIndex_ipMRouteGroup: /* already got it above*/
      case I_pimIpMRouteEntryIndex_ipMRouteSource: /* already got it above*/
      case I_pimIpMRouteEntryIndex_ipMRouteSourceMask: /* already got it above*/
	break;

      case I_pimIpMRouteUpstreamAssertTimer:
	if (usmDbPimIpMRouteUpstreamAssertTimerGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask,
	      &seconds) == L7_SUCCESS )
	{
	  /* converting seconds into ticks 1 second = 100 ticks*/
	  pimIpMRouteEntryData.pimIpMRouteUpstreamAssertTimer = seconds * 100;
	  SET_VALID(I_pimIpMRouteUpstreamAssertTimer, pimIpMRouteEntryData.valid);
	}
	break;

      case I_pimIpMRouteAssertMetric:
	if (usmDbPimIpMRouteAssertMetricGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask,
	      &pimIpMRouteEntryData.pimIpMRouteAssertMetric) == L7_SUCCESS )
	  SET_VALID(I_pimIpMRouteAssertMetric, pimIpMRouteEntryData.valid);
	break;

      case I_pimIpMRouteAssertMetricPref:
	if (usmDbPimIpMRouteAssertMetricPrefGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask,
	      &pimIpMRouteEntryData.pimIpMRouteAssertMetricPref) == L7_SUCCESS )
	  SET_VALID(I_pimIpMRouteAssertMetricPref, pimIpMRouteEntryData.valid);
	break;
      case I_pimIpMRouteAssertRPTBit:
	if (usmDbPimIpMRouteAssertRPTBitGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask,
	      &pimIpMRouteEntryData.pimIpMRouteAssertRPTBit) == L7_SUCCESS )
	  SET_VALID(I_pimIpMRouteAssertRPTBit, pimIpMRouteEntryData.valid);
	break;
	/*case I_pimIpMRouteFlags:
	  break;
	 * This is undefined in L7_undefs.h, please define it for PIM-SM
	 */

      default:
	/* unknown nominator */
	return(NULL);
	break;
    }
  }
  else if (snmp_pim_mode == L7_SNMP_PIM_MODE_SPARSE)
  {

    if ((searchType == EXACT) ?
	(usmDbPimsmIpMRouteEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask) != L7_SUCCESS) :
	((usmDbPimsmIpMRouteEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask) != L7_SUCCESS) &&
	 (usmDbPimsmIpMRouteEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask) != L7_SUCCESS))){
      return(NULL);
    }

    switch (nominator)
    {
      case -1:
      case I_pimIpMRouteEntryIndex_ipMRouteGroup: /* already got it above*/
      case I_pimIpMRouteEntryIndex_ipMRouteSource: /* already got it above*/
      case I_pimIpMRouteEntryIndex_ipMRouteSourceMask: /* already got it above*/
	break;

      case I_pimIpMRouteUpstreamAssertTimer:
	if (usmDbPimsmIpMRouteUpstreamAssertTimerGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask,
	      &seconds) == L7_SUCCESS )
	{   /* converting seconds into ticks 1 second = 100 ticks*/
	  pimIpMRouteEntryData.pimIpMRouteUpstreamAssertTimer = seconds * 100;
	  SET_VALID(I_pimIpMRouteUpstreamAssertTimer, pimIpMRouteEntryData.valid);
	}
	break;

      case I_pimIpMRouteAssertMetric:
	if (usmDbPimsmIpMRouteAssertMetricGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask,
	      &pimIpMRouteEntryData.pimIpMRouteAssertMetric) == L7_SUCCESS )
	  SET_VALID(I_pimIpMRouteAssertMetric, pimIpMRouteEntryData.valid);
	break;

      case I_pimIpMRouteAssertMetricPref:
	if (usmDbPimsmIpMRouteAssertMetricPrefGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask,
	      &pimIpMRouteEntryData.pimIpMRouteAssertMetricPref) == L7_SUCCESS )
	  SET_VALID(I_pimIpMRouteAssertMetricPref, pimIpMRouteEntryData.valid);
	break;
      case I_pimIpMRouteAssertRPTBit:
	if (usmDbPimsmIpMRouteAssertRPTBitGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask,
	      &pimIpMRouteEntryData.pimIpMRouteAssertRPTBit) == L7_SUCCESS )
	  SET_VALID(I_pimIpMRouteAssertRPTBit, pimIpMRouteEntryData.valid);
	break;
      case I_pimIpMRouteFlags:
	if(usmDbPimsmIpMRouteFlagsGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc,&pimIpMask,
	      &flags) == L7_SUCCESS)
	  if(flags == 0)
	    val8 = D_pimIpMRouteFlags_rpt;
	if(flags == 1)
	  val8 = D_pimIpMRouteFlags_spt;
	if (SafeMakeOctetString(&pimIpMRouteEntryData.pimIpMRouteFlags, &val8, sizeof(L7_uchar8)) == L7_TRUE)
	  SET_VALID(I_pimIpMRouteFlags, pimIpMRouteEntryData.valid);
	break;

      default:
	/* unknown nominator */
	return(NULL);
	break;
    }
  }

  if (nominator >= 0 && !VALID(nominator, pimIpMRouteEntryData.valid))
    return(NULL);

  return(&pimIpMRouteEntryData);

}

pimRPEntry_t *
k_pimRPEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_UINT32 pimRPGroupAddress,
    SR_UINT32 pimRPAddress)
{
#ifdef NOT_YET
  static pimRPEntry_t pimRPEntryData;

  /*
   * put your code to retrieve the information here
   */

  pimRPEntryData.pimRPGroupAddress = ;
  pimRPEntryData.pimRPAddress = ;
  pimRPEntryData.pimRPState = ;
  pimRPEntryData.pimRPStateTimer = ;
  pimRPEntryData.pimRPLastChange = ;
  pimRPEntryData.pimRPRowStatus = ;
  SET_ALL_VALID(pimRPEntryData.valid);
  return(&pimRPEntryData);
#else /* NOT_YET */
  return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_pimRPEntry_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_pimRPEntry_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_pimRPEntry_set_defaults(doList_t *dp)
{
  pimRPEntry_t *data = (pimRPEntry_t *) (dp->data);


  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_pimRPEntry_set(pimRPEntry_t *data,
    ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_pimRPEntry_UNDO
/* add #define SR_pimRPEntry_UNDO in sitedefs.h to
 * include the undo routine for the pimRPEntry family.
 */
int
pimRPEntry_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_pimRPEntry_UNDO */

#endif /* SETS */

pimRPSetEntry_t *
k_pimRPSetEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_INT32 pimRPSetComponent,
    SR_UINT32 pimRPSetGroupAddress,
    SR_UINT32 pimRPSetGroupMask,
    SR_UINT32 pimRPSetAddress)
{
  static pimRPSetEntry_t pimRPSetEntryData;
  L7_inet_addr_t pimIpSrc, pimIpMask, pimIpGrp;


  /* Clear all the bits*/
  ZERO_VALID(pimRPSetEntryData.valid);

  if (snmp_pim_mode == L7_SNMP_PIM_MODE_SPARSE)
  {
    pimRPSetEntryData.pimRPSetComponent = pimRPSetComponent;
    pimRPSetEntryData.pimRPSetGroupAddress = pimRPSetGroupAddress;
    pimRPSetEntryData.pimRPSetGroupMask = pimRPSetGroupMask;
    pimRPSetEntryData.pimRPSetAddress = pimRPSetAddress;

    SET_VALID(I_pimRPSetComponent, pimRPSetEntryData.valid);
    SET_VALID(I_pimRPSetGroupAddress, pimRPSetEntryData.valid);
    SET_VALID(I_pimRPSetGroupMask, pimRPSetEntryData.valid);
    SET_VALID(I_pimRPSetAddress, pimRPSetEntryData.valid);

    inetAddressReset(&pimIpSrc);
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &pimRPSetAddress, &pimIpSrc))
      return (NULL);
    inetAddressReset(&pimIpMask);
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &pimRPSetGroupMask, &pimIpMask))
      return (NULL);
    inetAddressReset(&pimIpGrp);
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &pimRPSetGroupAddress, &pimIpGrp))
      return (NULL);

#ifdef SNMP_FP_MCAST_MIB_TODO
    if (( searchType == EXACT) ?
	(usmDbPimsmRPSetEntryGet(USMDB_UNIT_CURRENT, pimRPSetComponent, &pimIpGrp,
				 &pimIpMask,&pimIpSrc) != L7_SUCCESS) :
	((usmDbPimsmRPSetEntryGet(USMDB_UNIT_CURRENT, pimRPSetComponent, &pimIpGrp,
				  &pimIpMask,&pimIpSrc) != L7_SUCCESS) &&
	 (usmDbPimsmRPSetEntryNextGet(USMDB_UNIT_CURRENT, &pimRPSetEntryData.pimRPSetComponent,
				      &pimIpGrp,&pimIpMask,&pimIpSrc) != L7_SUCCESS)))
#endif
    {
      return(NULL);
    }

    switch (nominator)
    {
      case -1:
      case I_pimRPSetComponent: /* already got it above*/
      case I_pimRPSetGroupAddress: /* already got it above*/
      case I_pimRPSetGroupMask: /* already got it above*/
      case I_pimRPSetAddress: /* already got it above*/
	break;

      case I_pimRPSetHoldTime:
	if (usmDbPimsmRPSetHoldTimeGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp,
	     &pimIpMask,&pimIpSrc, pimRPSetEntryData.pimRPSetComponent,
	      &pimRPSetEntryData.pimRPSetHoldTime) == L7_SUCCESS)
	  SET_VALID(I_pimRPSetHoldTime, pimRPSetEntryData.valid);
	break;

      case I_pimRPSetExpiryTime:
	if (snmpPimsmRPSetExpiryTimeGet(USMDB_UNIT_CURRENT, pimRPSetEntryData.pimRPSetGroupAddress,
	      pimRPSetEntryData.pimRPSetGroupMask, pimRPSetEntryData.pimRPSetAddress, pimRPSetEntryData.pimRPSetComponent,
	      &pimRPSetEntryData.pimRPSetExpiryTime) == L7_SUCCESS)
	  SET_VALID(I_pimRPSetExpiryTime, pimRPSetEntryData.valid);
	break;

      default:
	/* unknown nominator */
	return(NULL);
	break;
    }
  }

  if (nominator >= 0 && !VALID(nominator, pimRPSetEntryData.valid))
    return(NULL);

  return(&pimRPSetEntryData);

}

pimIpMRouteNextHopEntry_t *
k_pimIpMRouteNextHopEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_UINT32 ipMRouteNextHopGroup,
    SR_UINT32 ipMRouteNextHopSource,
    SR_UINT32 ipMRouteNextHopSourceMask,
    SR_INT32 ipMRouteNextHopIfIndex,
    SR_UINT32 ipMRouteNextHopAddress)
{

  static pimIpMRouteNextHopEntry_t pimIpMRouteNextHopEntryData;
  L7_uint32 intIfNum;
  L7_inet_addr_t pimIpGrp, pimIpSrc, pimIpMask, pimNextHopAddr;

  /* Clear all the bits*/
  ZERO_VALID(pimIpMRouteNextHopEntryData.valid);
  pimIpMRouteNextHopEntryData.ipMRouteNextHopGroup = ipMRouteNextHopGroup;
  pimIpMRouteNextHopEntryData.ipMRouteNextHopSource = ipMRouteNextHopSource;
  pimIpMRouteNextHopEntryData.ipMRouteNextHopSourceMask = ipMRouteNextHopSourceMask;
  pimIpMRouteNextHopEntryData.ipMRouteNextHopAddress = ipMRouteNextHopAddress;
  pimIpMRouteNextHopEntryData.ipMRouteNextHopIfIndex = ipMRouteNextHopIfIndex;

  SET_VALID(I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopGroup, pimIpMRouteNextHopEntryData.valid);
  SET_VALID(I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopSource, pimIpMRouteNextHopEntryData.valid);
  SET_VALID(I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopSourceMask, pimIpMRouteNextHopEntryData.valid);
  SET_VALID(I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopIfIndex, pimIpMRouteNextHopEntryData.valid);
  SET_VALID(I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopAddress, pimIpMRouteNextHopEntryData.valid);

  inetAddressReset(&pimIpSrc);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteNextHopSource, &pimIpSrc))
    return (NULL);
  inetAddressReset(&pimIpMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteNextHopSourceMask, &pimIpMask))
    return (NULL);
  inetAddressReset(&pimIpGrp);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteNextHopGroup, &pimIpGrp))
    return (NULL);
  inetAddressReset(&pimNextHopAddr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRouteNextHopAddress, &pimNextHopAddr))
    return (NULL);

  if (snmp_pim_mode == L7_SNMP_PIM_MODE_DENSE)
  {
#ifdef SUPPORTED  
  if ( (searchType == EXACT) ?
	( (usmDbIntIfNumFromExtIfNum(pimIpMRouteNextHopEntryData.ipMRouteNextHopIfIndex, &intIfNum) != L7_SUCCESS) ||
	  (usmDbPimIpMRouteNextHopEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc, &pimIpMask,
					   intIfNum,&pimNextHopAddr) != L7_SUCCESS) ) :

	( ( (usmDbVisibleExtIfNumberCheck( USMDB_UNIT_CURRENT, pimIpMRouteNextHopEntryData.ipMRouteNextHopIfIndex) != L7_SUCCESS) &&
	    (usmDbGetNextVisibleExtIfNumber(pimIpMRouteNextHopEntryData.ipMRouteNextHopIfIndex, &pimIpMRouteNextHopEntryData.ipMRouteNextHopIfIndex) != L7_SUCCESS) ) ||
	  (usmDbIntIfNumFromExtIfNum(pimIpMRouteNextHopEntryData.ipMRouteNextHopIfIndex, &intIfNum) != L7_SUCCESS) ||
	  ( (usmDbPimIpMRouteNextHopEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc, &pimIpMask,intIfNum, &pimNextHopAddr) != L7_SUCCESS) &&
	    (usmDbPimIpMRouteNextHopEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc, &pimIpMask,
						 &intIfNum, &pimNextHopAddr) != L7_SUCCESS)) ||
	  (usmDbExtIfNumFromIntIfNum(intIfNum, &pimIpMRouteNextHopEntryData.ipMRouteNextHopIfIndex) != L7_SUCCESS) )
       )
#endif 
    {
      return(NULL);
    }


    switch (nominator)
    {
      case -1:
      case I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopGroup: /* already got it above*/
      case I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopSource: /* already got it above*/
      case I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopSourceMask: /* already got it above*/
      case I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopIfIndex: /* already got it above*/
      case I_pimIpMRouteNextHopEntryIndex_ipMRouteNextHopAddress: /* already got it above*/

	break;

      case I_pimIpMRouteNextHopPruneReason:
	if (usmDbPimIpMRouteNextHopPruneReasonGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpGrp, &pimIpSrc, &pimIpMask,
	      intIfNum,&pimNextHopAddr,
	      &pimIpMRouteNextHopEntryData.pimIpMRouteNextHopPruneReason) == L7_SUCCESS )
	  SET_VALID(I_pimIpMRouteNextHopPruneReason, pimIpMRouteNextHopEntryData.valid);
	break;


      default:
	/* unknown nominator */
	return(NULL);
	break;
    }
  }
  else if (snmp_pim_mode == L7_SNMP_PIM_MODE_SPARSE)

  {
    /* not supported as of now */
    return(NULL);
  }
  if (nominator >= 0 && !VALID(nominator, pimIpMRouteNextHopEntryData.valid))
    return(NULL);

  return(&pimIpMRouteNextHopEntryData);
}


pimCandidateRPEntry_t *
k_pimCandidateRPEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_UINT32 pimCandidateRPGroupAddress,
    SR_UINT32 pimCandidateRPGroupMask)
{
  static pimCandidateRPEntry_t pimCandidateRPEntryData;
  L7_inet_addr_t pimIpSrc, pimIpMask, pimRpAddr;

  /* Clear all the bits*/
  ZERO_VALID(pimCandidateRPEntryData.valid);

  if (snmp_pim_mode == L7_SNMP_PIM_MODE_SPARSE)
  {
    pimCandidateRPEntryData.pimCandidateRPGroupAddress = pimCandidateRPGroupAddress;
    pimCandidateRPEntryData.pimCandidateRPGroupMask = pimCandidateRPGroupMask;
    SET_VALID(I_pimCandidateRPGroupAddress, pimCandidateRPEntryData.valid);
    SET_VALID(I_pimCandidateRPGroupMask, pimCandidateRPEntryData.valid);

    inetAddressReset(&pimIpSrc);
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &pimCandidateRPGroupAddress, &pimIpSrc))
      return (NULL);
    inetAddressReset(&pimIpMask);
    if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &pimCandidateRPGroupMask, &pimIpMask))
      return (NULL);

#ifdef SNMP_FP_MCAST_MIB_TODO
    if (( searchType == EXACT) ?
	(usmDbPimsmCandRPEntryGet(USMDB_UNIT_CURRENT, &pimIpSrc, &pimIpMask) != L7_SUCCESS) :
	((usmDbPimsmCandRPEntryGet(USMDB_UNIT_CURRENT, &pimIpSrc, &pimIpMask) != L7_SUCCESS) &&
	 (usmDbPimsmCandRPEntryNextGet(USMDB_UNIT_CURRENT, &pimIpSrc, &pimIpMask) != L7_SUCCESS)))
#endif
    {
      return(NULL);
    }


    switch (nominator)
    {
      case -1:
      case I_pimCandidateRPGroupAddress: /* already got it above*/
      case I_pimCandidateRPGroupMask: /* already got it above*/
	break;

      case I_pimCandidateRPAddress:
	if (usmDbPimsmCandRPAddressGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpSrc, &pimIpMask, &pimRpAddr) == L7_SUCCESS)
  {
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimRpAddr, &pimCandidateRPEntryData.pimCandidateRPAddress))
      return(NULL);
	  SET_VALID(I_pimCandidateRPAddress, pimCandidateRPEntryData.valid);
  }
	break;

      case I_pimCandidateRPRowStatus:
	pimCandidateRPEntryData.pimCandidateRPRowStatus = D_pimCandidateRPRowStatus_active;
	SET_VALID(I_pimCandidateRPRowStatus, pimCandidateRPEntryData.valid);
	break;

      default:
	/* unknown nominator */
	return(NULL);
	break;
    }
  }
  if (nominator >= 0 && !VALID(nominator, pimCandidateRPEntryData.valid))
    return(NULL);

  return(&pimCandidateRPEntryData);
}

#ifdef SETS
int
k_pimCandidateRPEntry_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_pimCandidateRPEntry_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_pimCandidateRPEntry_set_defaults(doList_t *dp)
{
  pimCandidateRPEntry_t *data = (pimCandidateRPEntry_t *) (dp->data);


  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_pimCandidateRPEntry_set(pimCandidateRPEntry_t *data,
    ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_pimCandidateRPEntry_UNDO
/* add #define SR_pimCandidateRPEntry_UNDO in sitedefs.h to
 * include the undo routine for the pimCandidateRPEntry family.
 */
int
pimCandidateRPEntry_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_pimCandidateRPEntry_UNDO */

#endif /* SETS */

pimComponentEntry_t *
k_pimComponentEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_INT32 pimComponentIndex)
{
  static pimComponentEntry_t pimComponentEntryData;
#ifdef SNMP_FP_MCAST_MIB_TODO
  L7_inet_addr_t pimRpAddr;
#endif

  /* Clear all the bits*/
  ZERO_VALID(pimComponentEntryData.valid);

  if (snmp_pim_mode == L7_SNMP_PIM_MODE_SPARSE)
  {
    pimComponentEntryData.pimComponentIndex = pimComponentIndex;
    SET_VALID(I_pimComponentIndex, pimComponentEntryData.valid);
#ifdef SNMP_FP_MCAST_MIB_TODO
    if (( searchType == EXACT) ?
	(usmDbPimsmComponentEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, pimComponentIndex) != L7_SUCCESS) :
	((usmDbPimsmComponentEntryGet(USMDB_UNIT_CURRENT, L7_AF_INET, pimComponentIndex) != L7_SUCCESS) &&
	 (usmDbPimsmComponentEntryNextGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimComponentEntryData.pimComponentIndex) != L7_SUCCESS)))
#endif
    {
      return(NULL);
    }

    switch (nominator)
    {
      case -1:
      case I_pimComponentIndex: /* already got it above*/
	break;

      case I_pimComponentBSRAddress:
#ifdef SNMP_FP_MCAST_MIB_TODO
	if (usmDbPimsmComponentBSRAddressGet(USMDB_UNIT_CURRENT, L7_AF_INET, pimComponentEntryData.pimComponentIndex,
	      &pimRpAddr) == L7_SUCCESS)
  {
    if(L7_SUCCESS != inetAddressGet(L7_AF_INET, &pimRpAddr, &pimComponentEntryData.pimComponentBSRAddress))
      return(NULL);
	  SET_VALID(I_pimComponentBSRAddress, pimComponentEntryData.valid);
  }
#endif
    break;

      case I_pimComponentBSRExpiryTime:
	if (snmpPimsmComponentBSRExpiryTimeGet(USMDB_UNIT_CURRENT, pimComponentEntryData.pimComponentIndex,
	      &pimComponentEntryData.pimComponentBSRExpiryTime) == L7_SUCCESS)
	  SET_VALID(I_pimComponentBSRExpiryTime, pimComponentEntryData.valid);
	break;

      case I_pimComponentCRPHoldTime:
#ifdef SNMP_FP_MCAST_MIB_TODO
	if (usmDbPimsmComponentCRPHoldTimeGet(USMDB_UNIT_CURRENT, pimComponentEntryData.pimComponentIndex,
	      &pimComponentEntryData.pimComponentCRPHoldTime) == L7_SUCCESS)
	  SET_VALID(I_pimComponentCRPHoldTime, pimComponentEntryData.valid);
#endif
    break;

      case I_pimComponentStatus:
	pimComponentEntryData.pimComponentStatus = D_pimComponentStatus_active;
	SET_VALID(I_pimComponentStatus, pimComponentEntryData.valid);
	break;

      default:
	/* unknown nominator */
	return(NULL);
	break;
    }
  }
  if (nominator >= 0 && !VALID(nominator, pimComponentEntryData.valid))
    return(NULL);

  return(&pimComponentEntryData);
}

#ifdef SETS
int
k_pimComponentEntry_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_pimComponentEntry_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_pimComponentEntry_set_defaults(doList_t *dp)
{
  pimComponentEntry_t *data = (pimComponentEntry_t *) (dp->data);

  data->pimComponentCRPHoldTime = 0;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_pimComponentEntry_set(pimComponentEntry_t *data,
    ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_pimComponentEntry_UNDO
/* add #define SR_pimComponentEntry_UNDO in sitedefs.h to
 * include the undo routine for the pimComponentEntry family.
 */
int
pimComponentEntry_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_pimComponentEntry_UNDO */

#endif /* SETS */

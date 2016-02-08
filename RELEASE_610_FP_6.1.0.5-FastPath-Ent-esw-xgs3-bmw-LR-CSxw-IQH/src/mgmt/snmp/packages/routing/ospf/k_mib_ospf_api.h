/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_ospf_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create 05/15/2001
*
* @author soma
*
* @end
*             
**********************************************************************/

#include "usmdb_common.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_ospf_api.h"
#include "usmdb_util_api.h"


/**************************@null{*************************************
                     
 *******************************}*************************************}
 **********************************************************************/

L7_RC_t          
snmpOspfAdminStatGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfAdminStatGet (UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_ENABLE:
        *val = D_ospfAdminStat_enabled;
        break;

      case L7_DISABLE:
        *val = D_ospfAdminStat_disabled;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfAdminStatSet ( L7_uint32 UnitIndex, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_ospfAdminStat_enabled:
      temp_val = L7_ENABLE;
      break;

    case D_ospfAdminStat_disabled:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfAdminStatSet (UnitIndex, temp_val);
    }

  return rc;
}


L7_RC_t          
snmpOspfVersionNumberGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfVersionNumberGet (UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      if(temp_val == 2)
      {
        *val = D_ospfVersionNumber_version2;
      }
      else
      {
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfAreaBdrRtrStatusGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfAreaBdrRtrStatusGet (UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_TRUE:
        *val = D_ospfAreaBdrRtrStatus_true;
        break;

      case L7_FALSE:
        *val = D_ospfAreaBdrRtrStatus_false;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfASBdrRtrStatusGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfASBdrRtrStatusGet (UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_TRUE:
        *val = D_ospfAreaBdrRtrStatus_true;
        break;

      case L7_FALSE:
        *val = D_ospfAreaBdrRtrStatus_false;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfASBdrRtrStatusSet ( L7_uint32 UnitIndex, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_ospfAreaBdrRtrStatus_true:
      temp_val = L7_TRUE;
      break;

    case D_ospfAreaBdrRtrStatus_false:
      temp_val = L7_FALSE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfASBdrRtrStatusSet (UnitIndex, temp_val);
    }

  return rc;
}


L7_RC_t          
snmpOspfTOSSupportGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfTOSSupportGet (UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_TRUE:
        *val = D_ospfTOSSupport_true;
        break;

      case L7_FALSE:
        *val = D_ospfTOSSupport_false;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfTOSSupportSet ( L7_uint32 UnitIndex, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_ospfTOSSupport_true:
      temp_val = L7_TRUE;
      break;

    case D_ospfTOSSupport_false:
      temp_val = L7_FALSE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfTOSSupportSet (UnitIndex, temp_val);

      if (rc == L7_SUCCESS)
      {
        rc = usmDbOspfTOSSupportSet (UnitIndex, temp_val);
      }
    }

  return rc;
}

L7_RC_t          
snmpOspfDemandExtensionsGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfDemandExtensionsGet (UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_TRUE:
        *val = D_ospfDemandExtensions_true;
        break;

      case L7_FALSE:
        *val = D_ospfDemandExtensions_false;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfDemandExtensionsSet ( L7_uint32 UnitIndex, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_ospfDemandExtensions_true:
      temp_val = L7_TRUE;
      break;

    case D_ospfDemandExtensions_false:
      temp_val = L7_FALSE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfDemandExtensionsSet (UnitIndex, temp_val);
    }

  return rc;
}

/**********************************************************************/

L7_RC_t
snmpOspfImportAsExternGet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 *val )
{
 
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfImportAsExternGet(UnitIndex, AreaId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_AREA_IMPORT_EXT:
      *val = D_ospfImportAsExtern_importExternal;
      break;
    
    case L7_OSPF_AREA_IMPORT_NO_EXT:
      *val = D_ospfImportAsExtern_importNoExternal;
      break;
    
    case L7_OSPF_AREA_IMPORT_NSSA:
      *val = D_ospfImportAsExtern_importNssa;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpOspfImportAsExternSet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;


  switch (val)
  {
  case D_ospfImportAsExtern_importExternal:
    temp_val = L7_OSPF_AREA_IMPORT_EXT;
    break;
  
  case D_ospfImportAsExtern_importNoExternal:
    temp_val = L7_OSPF_AREA_IMPORT_NO_EXT;
    break;

  case D_ospfImportAsExtern_importNssa:
    temp_val = L7_OSPF_AREA_IMPORT_NSSA;
    break;
  
  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfImportAsExternSet ( UnitIndex, AreaId, temp_val );
  }

  return rc;
}


L7_RC_t
snmpOspfAreaSummaryGet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfAreaSummaryGet(UnitIndex, AreaId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_AREA_NO_SUMMARY:
      *val = D_ospfAreaSummary_noAreaSummary;
      break;
    
    case L7_OSPF_AREA_SEND_SUMMARY:
      *val = D_ospfAreaSummary_sendAreaSummary;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpOspfAreaSummarySet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_ospfAreaSummary_noAreaSummary:
    temp_val = L7_OSPF_AREA_NO_SUMMARY;
    break;
  
  case D_ospfAreaSummary_sendAreaSummary:
    temp_val = L7_OSPF_AREA_SEND_SUMMARY;
    break;
  
  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfAreaSummarySet ( UnitIndex, AreaId, temp_val );
  }

  return rc;
}


L7_RC_t
snmpOspfAreaStatusGet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfAreaStatusGet(UnitIndex, AreaId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_ROW_ACTIVE:
      *val = D_ospfAreaStatus_active;
      break;

    case L7_OSPF_ROW_NOT_IN_SERVICE:
      *val = D_ospfAreaStatus_notInService;
      break;

    case L7_OSPF_ROW_NOT_READY:
      *val = D_ospfAreaStatus_notReady;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t          
snmpOspfStubMetricTypeGet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfStubMetricTypeGet(UnitIndex, AreaId, TOS, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_AREA_STUB_METRIC:
      *val = D_ospfStubMetricType_ospfMetric;
      break;
    
    case L7_OSPF_AREA_STUB_COMPARABLE_COST:
      *val = D_ospfStubMetricType_comparableCost;
      break;
    
    case L7_OSPF_AREA_STUB_NON_COMPARABLE_COST:
      *val = D_ospfStubMetricType_nonComparable;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}


L7_RC_t
snmpOspfStubMetricTypeSet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 TOS, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_ospfStubMetricType_ospfMetric:
    temp_val = L7_OSPF_AREA_STUB_METRIC;
    break;
  
  case D_ospfStubMetricType_comparableCost:
    temp_val = L7_OSPF_AREA_STUB_COMPARABLE_COST;
    break;
  
  case D_ospfStubMetricType_nonComparable:
    temp_val = L7_OSPF_AREA_STUB_NON_COMPARABLE_COST;
    break;
  
  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfStubMetricTypeSet ( UnitIndex, AreaId, TOS, temp_val );
  }

  return rc;
}

L7_RC_t
snmpOspfStubStatusGet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfStubStatusGet(UnitIndex, AreaId, TOS, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_ROW_ACTIVE:
      *val = D_ospfStubStatus_active;
      break;

    case L7_OSPF_ROW_NOT_IN_SERVICE:
      *val = D_ospfStubStatus_notInService;
      break;

    case L7_OSPF_ROW_NOT_READY:
      *val = D_ospfStubStatus_notReady;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpOspfStubStatusSet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 TOS, L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_ospfStubStatus_active:
    break;

  case D_ospfStubStatus_notInService:
  case D_ospfStubStatus_createAndWait:
    rc = L7_FAILURE;
    break;

  case D_ospfStubStatus_createAndGo:
    rc = usmDbOspfImportAsExternGet(UnitIndex, AreaId, &temp_val);
    if (rc == L7_SUCCESS && temp_val != L7_OSPF_AREA_IMPORT_NO_EXT)
    {
       rc = usmDbOspfImportAsExternSet(UnitIndex, AreaId, L7_OSPF_AREA_IMPORT_NO_EXT);
    }
    else
    {  
       rc = L7_FAILURE;
    }
    break;

  case D_ospfStubStatus_destroy:
    rc = usmDbOspfImportAsExternGet(UnitIndex, AreaId, &temp_val);
    if (rc == L7_SUCCESS && temp_val != L7_OSPF_AREA_IMPORT_EXT)
    {
       rc = usmDbOspfImportAsExternSet(UnitIndex, AreaId, L7_OSPF_AREA_IMPORT_EXT);
    }
    else
    {  
       rc = L7_FAILURE;
    }
    break;

  default:
    rc = L7_FAILURE;
  }

#ifdef OLD_CODE
  switch (val)
  {
  case D_ospfStubStatus_active:
    temp_val = L7_OSPF_ROW_ACTIVE;
    break;

  case D_ospfStubStatus_notInService:
    temp_val = L7_OSPF_ROW_NOT_IN_SERVICE;
    break;

  case D_ospfStubStatus_createAndGo:
    temp_val = L7_OSPF_ROW_CREATE_AND_GO;
    break;

  case D_ospfStubStatus_createAndWait:
    temp_val = L7_OSPF_ROW_CREATE_AND_WAIT;
    break;

  case D_ospfStubStatus_destroy:
    temp_val = L7_OSPF_ROW_DESTROY;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfStubStatusSet ( UnitIndex, AreaId, TOS, temp_val );
  }
#endif

  return rc;
}


/**********************************************************************/

L7_RC_t
snmpOspfLsdbEntryGet ( L7_uint32 UnitIndex, L7_uint32 AreaID, L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId )
{
  L7_uint32 temp_Type;
  L7_ospfLsdbEntry_t temp_Lsa;

  switch (Type)
  {
  case D_ospfLsdbType_routerLink:
    temp_Type = L7_S_ROUTER_LSA;
    break;

  case D_ospfLsdbType_networkLink:
    temp_Type = L7_S_NETWORK_LSA;
    break;

  case D_ospfLsdbType_summaryLink:
    temp_Type = L7_S_IPNET_SUMMARY_LSA;
    break;

  case D_ospfLsdbType_asSummaryLink:
    temp_Type = L7_S_ASBR_SUMMARY_LSA;
    break;

  case D_ospfLsdbType_asExternalLink:
    temp_Type = L7_S_AS_EXTERNAL_LSA;
    break;

  case D_ospfLsdbType_multicastLink:
    temp_Type = L7_S_GROUP_MEMBER_LSA;
    break;

  case D_ospfLsdbType_nssaExternalLink:
  default:
    return L7_FAILURE;
  }

  return usmDbOspfLsdbEntryGet(UnitIndex, AreaID, temp_Type,
                               Lsid, RouterId, &temp_Lsa);
}


L7_RC_t
snmpOspfLsdbEntryNext ( L7_uint32 UnitIndex, L7_uint32 *AreaID, L7_int32 *Type, L7_uint32 *Lsid, L7_uint32 *RouterId )
{
  L7_uint32 temp_Type;
  L7_ospfLsdbEntry_t temp_Lsa;

  switch (*Type)
  {
  case 0:
    temp_Type = 0;
    break;

  case D_ospfLsdbType_routerLink:
    temp_Type = L7_S_ROUTER_LSA;
    break;

  case D_ospfLsdbType_networkLink:
    temp_Type = L7_S_NETWORK_LSA;
    break;

  case D_ospfLsdbType_summaryLink:
    temp_Type = L7_S_IPNET_SUMMARY_LSA;
    break;

  case D_ospfLsdbType_asSummaryLink:
    temp_Type = L7_S_ASBR_SUMMARY_LSA;
    break;

  case D_ospfLsdbType_asExternalLink:
    temp_Type = L7_S_AS_EXTERNAL_LSA;
    break;

  case D_ospfLsdbType_multicastLink:
    temp_Type = L7_S_GROUP_MEMBER_LSA;
    break;

  case D_ospfLsdbType_nssaExternalLink:
  default:
    return L7_FAILURE;
  }


  if (usmDbOspfLsdbEntryNext(UnitIndex, AreaID, &temp_Type, Lsid, RouterId, &temp_Lsa) == L7_SUCCESS)
  {
    switch (temp_Type)
    {
    case L7_S_ROUTER_LSA:
      *Type = D_ospfLsdbType_routerLink;
      break;

    case L7_S_NETWORK_LSA:
      *Type = D_ospfLsdbType_networkLink;
      break;

    case L7_S_IPNET_SUMMARY_LSA:
      *Type = D_ospfLsdbType_summaryLink;
      break;

    case L7_S_ASBR_SUMMARY_LSA:
      *Type = D_ospfLsdbType_asSummaryLink;
      break;

    case L7_S_AS_EXTERNAL_LSA:
      *Type = D_ospfLsdbType_asExternalLink;
      break;

    case L7_S_GROUP_MEMBER_LSA:
      *Type = D_ospfLsdbType_multicastLink;
      break;

    default:
      return L7_FAILURE;
    }

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/**********************************************************************/

L7_RC_t
snmpOspfHostStatusGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfHostStatusGet(UnitIndex, IpAddress, TOS, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_ROW_ACTIVE:
      *val = D_ospfHostStatus_active;
      break;

    case L7_OSPF_ROW_NOT_IN_SERVICE:
      *val = D_ospfHostStatus_notInService;
      break;

    case L7_OSPF_ROW_NOT_READY:
      *val = D_ospfHostStatus_notReady;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpOspfHostStatusSet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_ospfHostStatus_active:
    temp_val = L7_OSPF_ROW_ACTIVE;
    break;

  case D_ospfHostStatus_notInService:
    temp_val = L7_OSPF_ROW_NOT_IN_SERVICE;
    break;

  case D_ospfHostStatus_createAndGo:
    temp_val = L7_OSPF_ROW_CREATE_AND_GO;
    break;

  case D_ospfHostStatus_createAndWait:
    temp_val = L7_OSPF_ROW_CREATE_AND_WAIT;
    break;

  case D_ospfHostStatus_destroy:
    temp_val = L7_OSPF_ROW_DESTROY;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfHostStatusSet ( UnitIndex, IpAddress, TOS, temp_val );
  }

  return rc;
}

/**********************************************************************/

L7_RC_t
snmpOspfIfEntryGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_uint32 LessIf )
{
  L7_uint32 intIfNum;

  if ((usmDbIntIfNumFromExtIfNum(LessIf, &intIfNum) != L7_SUCCESS) ||
      (usmDbOspfIfEntryGet(UnitIndex, IpAddress, intIfNum) != L7_SUCCESS))
    return L7_FAILURE;

  return L7_SUCCESS;
}

L7_RC_t
snmpOspfIfEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *IpAddress, L7_uint32 *LessIf )
{
  L7_uint32 intIfNum;

  if (usmDbExtIfNumTypeCheckValid(UnitIndex, USM_ROUTER_INTF, 0, *LessIf) != L7_SUCCESS &&
      usmDbExtIfNumTypeNextGet(UnitIndex, USM_ROUTER_INTF, 0, *LessIf, LessIf) != L7_SUCCESS)
  {
    *IpAddress += 1; /* increment ip address */
    *LessIf = 0; /* start from beginning of ports */
  }
  else
  {
    /* convert from external to internal */
    if (usmDbIntIfNumFromExtIfNum(*LessIf, &intIfNum) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* search for the next entry */
  if ((usmDbOspfIfEntryGet(UnitIndex, *IpAddress, intIfNum) != L7_SUCCESS &&
       usmDbOspfIfEntryNext(UnitIndex, IpAddress, &intIfNum) != L7_SUCCESS) ||
      usmDbExtIfNumFromIntIfNum(intIfNum, LessIf) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

L7_RC_t          
snmpOspfIfTypeGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_FAILURE;

  rc = usmDbOspfIfTypeGet (LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_INTF_BROADCAST:
      *val = D_ospfIfType_broadcast;
      break;
    
    case L7_OSPF_INTF_NBMA:
      *val = D_ospfIfType_nbma;
      break;
    
    case L7_OSPF_INTF_PTP:
      *val = D_ospfIfType_pointToPoint;
      break;
    
    case L7_OSPF_INTF_POINTTOMULTIPOINT:
      *val = D_ospfIfType_pointToMultipoint;
      break;

    /* RFC 1850 doesn't have a value for loopback. So show p2p. */
    case L7_OSPF_INTF_TYPE_LOOPBACK:
      *val = D_ospfIfType_pointToPoint;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t          
snmpOspfIfTypeSet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_ospfIfType_broadcast:
    temp_val = L7_OSPF_INTF_BROADCAST;
    break;

  case D_ospfIfType_nbma:
    temp_val = L7_OSPF_INTF_NBMA;
    break;

  case D_ospfIfType_pointToPoint:
    temp_val = L7_OSPF_INTF_PTP;
    break;

  case D_ospfIfType_pointToMultipoint:
    temp_val = L7_OSPF_INTF_POINTTOMULTIPOINT;
    break;

  default:
    rc = L7_FAILURE;
  }
  
  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfIfTypeSet (LessIf, temp_val);
  }

  return rc;
}

L7_RC_t 
snmpOspfIfAdminStatGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbOspfIfAdminStatGet (UnitIndex, IpAddress, LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_ENABLE:
        *val = D_ospfIfAdminStat_enabled;
        break;

      case L7_DISABLE:
        *val = D_ospfIfAdminStat_disabled;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfIfAdminStatSet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 val )
{
    L7_uint32 temp_val = L7_DISABLE;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    #ifdef NOT_SUPPORTED
    case D_ospfIfAdminStat_enabled:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;

    case D_ospfIfAdminStat_disabled:
      temp_val = L7_DISABLE;
      break;
    #endif /* NOT_SUPPORTED */

    default:
      rc = L7_FAILURE;
    }
    
    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfIfAdminStatSet (UnitIndex, IpAddress, LessIf,temp_val);
    }

  return rc;
}

L7_RC_t          
snmpOspfIfStateGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfIfStateGet (UnitIndex, IpAddress, LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_INTF_DOWN:
      *val = D_ospfIfState_down;
      break;
    
    case L7_OSPF_INTF_LOOPBACK:
      *val = D_ospfIfState_loopback;
      break;
    
    case L7_OSPF_INTF_WAITING:
      *val = D_ospfIfState_waiting;
      break;
    
    case L7_OSPF_INTF_POINTTOPOINT:
      *val = D_ospfIfState_pointToPoint;
      break;
    
    case L7_OSPF_INTF_DESIGNATEDROUTER:
      *val = D_ospfIfState_designatedRouter;
      break;
    
    case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
      *val = D_ospfIfState_backupDesignatedRouter;
      break;
    
    case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
      *val = D_ospfIfState_otherDesignatedRouter;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpOspfIfStatusGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfIfStatusGet(UnitIndex, IpAddress, LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_ROW_ACTIVE:
      *val = D_ospfIfStatus_active;
      break;

    case L7_OSPF_ROW_NOT_IN_SERVICE:
      *val = D_ospfIfStatus_notInService;
      break;

    case L7_OSPF_ROW_NOT_READY:
      *val = D_ospfIfStatus_notReady;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpOspfIfStatusSet (L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 val )
{
#ifdef OLD_CODE
  L7_uint32 temp_val;
#endif
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_ospfIfStatus_active:
    rc = usmDbOspfIfAdminStatSet(UnitIndex, IpAddress, LessIf, L7_ENABLE);
    break;

  case D_ospfIfStatus_notInService:
    rc = L7_FAILURE;
    break;

  case D_ospfIfStatus_createAndWait:
    rc = L7_FAILURE;
    break;

  case D_ospfIfStatus_createAndGo:
    rc = usmDbOspfIfAdminStatSet(UnitIndex, IpAddress, LessIf, L7_ENABLE);
    break;

  case D_ospfIfStatus_destroy:
    rc = usmDbOspfIfAdminStatSet(UnitIndex, IpAddress, LessIf, L7_DISABLE);
    break;

  default:
    rc = L7_FAILURE;
  }

#ifdef OLD_CODE
  switch (val)
  {
  case D_ospfIfStatus_active:
    temp_val = L7_OSPF_ROW_ACTIVE;
    break;

  case D_ospfIfStatus_notInService:
    temp_val = L7_OSPF_ROW_NOT_IN_SERVICE;
    break;

  case D_ospfIfStatus_createAndGo:
    temp_val = L7_OSPF_ROW_CREATE_AND_GO;
    break;

  case D_ospfIfStatus_createAndWait:
    temp_val = L7_OSPF_ROW_CREATE_AND_WAIT;
    break;

  case D_ospfIfStatus_destroy:
    temp_val = L7_OSPF_ROW_DESTROY;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfIfStatusSet ( UnitIndex, IpAddress, LessIf, temp_val );
  }
#endif

  return rc;
}

L7_RC_t          
snmpOspfIfMulticastForwardingGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfIfMulticastForwardingGet (UnitIndex, IpAddress, LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_MULTICAST_FORWARDING_BLOCKED:
      *val = D_ospfIfMulticastForwarding_blocked;
      break;
    
    case L7_OSPF_MULTICAST_FORWARDING_MULTICAST:
      *val = D_ospfIfMulticastForwarding_multicast;
      break;
    
    case L7_OSPF_MULTICAST_FORWARDING_UNICAST:
      *val = D_ospfIfMulticastForwarding_unicast;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  
  return rc;
}

L7_RC_t 
snmpOspfIfDemandGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfIfDemandGet (UnitIndex, IpAddress, LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_TRUE:
        *val = D_ospfIfDemand_true;
        break;

      case L7_FALSE:
        *val = D_ospfIfDemand_false;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfIfDemandSet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_ospfIfDemand_true:
      temp_val = L7_TRUE;
      break;

    case D_ospfIfDemand_false:
      temp_val = L7_FALSE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfIfDemandSet (UnitIndex, IpAddress, LessIf, temp_val);
    }

  return rc;
}
/* lvl7_@p1081 end */

/* lvl7_@p1079 start */
L7_RC_t 
snmpOspfIfAuthTypeGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfIfAuthTypeGet (UnitIndex, IpAddress, LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_AUTH_TYPE_NONE:
        *val = 0; /* RFC 1850 */
        break;

      case L7_AUTH_TYPE_SIMPLE_PASSWORD:
        *val = 1; /* RFC 1850 */
        break;

      case L7_AUTH_TYPE_MD5:
        *val = 2; /* RFC 1850 */
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}


L7_RC_t          
snmpOspfIfAuthTypeSet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case 0: /* RFC 1850 */
      temp_val = L7_AUTH_TYPE_NONE;
      break;

    case 1: /* RFC 1850 */
      temp_val = L7_AUTH_TYPE_SIMPLE_PASSWORD;
      break;

    case 2: /* RFC 1850 */
      temp_val = L7_AUTH_TYPE_MD5;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfIfAuthTypeSet (UnitIndex, IpAddress, LessIf, temp_val);
    }

  return rc;
}

L7_RC_t 
snmpAgentOspfIfIpMtuIgnoreFlagGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbOspfIntfMtuIgnoreGet(UnitIndex, LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_ENABLE:
        *val = D_agentOspfIfIpMtuIgnoreFlag_enable;
        break;

      case L7_DISABLE:
        *val = D_agentOspfIfIpMtuIgnoreFlag_disable;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}


L7_RC_t          
snmpAgentOspfIfIpMtuIgnoreFlagSet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_agentOspfIfIpMtuIgnoreFlag_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentOspfIfIpMtuIgnoreFlag_disable:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfIntfMtuIgnoreSet (UnitIndex, LessIf, temp_val);
    }

  return rc;
}

L7_RC_t 
snmpAgentOspfIfPassiveModeGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_BOOL temp_val;
  L7_RC_t rc;

  rc = usmDbOspfIntfPassiveModeGet(UnitIndex, LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_TRUE:
        *val = D_agentOspfIfPassiveMode_true;
        break;

      case L7_FALSE:
        *val = D_agentOspfIfPassiveMode_false;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpAgentOspfIfPassiveModeSet( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 val )
{
    L7_BOOL temp_val = L7_FALSE;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_agentOspfIfPassiveMode_true:
      temp_val = L7_TRUE;
      break;

    case D_agentOspfIfPassiveMode_false:
      temp_val = L7_FALSE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfIntfPassiveModeSet (UnitIndex, LessIf, temp_val);
    }

  return rc;
}

L7_RC_t
snmpAgentOspfIfAdvertiseSecondariesGet( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbOspfIntfConfigSecondariesFlagGet(UnitIndex, LessIf, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
        case L7_ENABLE:
          *val = D_agentOspfIfAdvertiseSecondaries_enable;
          break;

        case L7_DISABLE:
          *val = D_agentOspfIfAdvertiseSecondaries_disable;
          break;

        default:
          *val = 0;
          rc = L7_FAILURE;
      }
  }

  return rc;
}
L7_RC_t
snmpAgentOspfIffAdvertiseSecondariesSet( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 val )
{
    L7_uint32 temp_val = L7_FALSE;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
       case D_agentOspfIfAdvertiseSecondaries_enable:
         temp_val = L7_ENABLE;
         break;

       case D_agentOspfIfAdvertiseSecondaries_disable:
         temp_val = L7_DISABLE;
         break;

       default:
         rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbOspfIntfSecondariesFlagSet(UnitIndex, LessIf, temp_val);
    }

  return rc;
}


/**********************************************************************/

L7_RC_t
snmpOspfIfMetricEntryGet( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_uint32 LessIf, L7_uint32 TOS )
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(LessIf, &intIfNum) != L7_SUCCESS ||
      (usmDbOspfIfMetricEntryGet(UnitIndex, IpAddress, intIfNum, TOS) != L7_SUCCESS))
    return L7_FAILURE;

  return L7_SUCCESS;
}

L7_RC_t
snmpOspfIfMetricEntryNextGet( L7_uint32 UnitIndex, L7_uint32 *IpAddress, L7_uint32 *LessIf, L7_uint32 *TOS )
{
  L7_uint32 intIfNum;

  if ((usmDbExtIfNumTypeCheckValid(UnitIndex, USM_ROUTER_INTF, 0, *LessIf) != L7_SUCCESS) &&
      (usmDbExtIfNumTypeNextGet(UnitIndex, USM_ROUTER_INTF, 0, *LessIf, LessIf) != L7_SUCCESS) )
  {
    *IpAddress += 1;
    *LessIf = 0;
    *TOS = 0;
  }
  else
  {
    /* convert to internal */
    if (usmDbIntIfNumFromExtIfNum(*LessIf, &intIfNum) != L7_SUCCESS)
      return L7_FAILURE;
  }

  if ((usmDbOspfIfMetricEntryGet(UnitIndex, *IpAddress, intIfNum, *TOS) != L7_SUCCESS &&
       usmDbOspfIfMetricEntryNext(UnitIndex, IpAddress, &intIfNum, TOS) != L7_SUCCESS) ||
      usmDbExtIfNumFromIntIfNum(intIfNum, LessIf) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

L7_RC_t
snmpOspfIfMetricStatusGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 LessIf, L7_int32 TOS, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfIfMetricStatusGet(UnitIndex, IpAddress, LessIf, TOS, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_ROW_ACTIVE:
      *val = D_ospfIfMetricStatus_active;
      break;

    case L7_OSPF_ROW_NOT_IN_SERVICE:
      *val = D_ospfIfMetricStatus_notInService;
      break;

    case L7_OSPF_ROW_NOT_READY:
      *val = D_ospfIfMetricStatus_notReady;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**********************************************************************/

L7_RC_t          
snmpOspfVirtIfStateGet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 Neighbor, L7_int32 *val )
{
    L7_uint32 temp_val;
    L7_RC_t rc;

    rc = L7_FAILURE;

    rc = usmDbOspfVirtIfStateGet(UnitIndex, AreaId, Neighbor, &temp_val);

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_OSPF_INTF_DOWN:
        *val = D_ospfVirtIfState_down;
        break;

      case L7_OSPF_INTF_POINTTOPOINT:
        *val = D_ospfVirtIfState_pointToPoint;
        break;
/*
      case L7_OSPF_IFSTATE_DOWN:
        *val = D_ospfIfState_down;
        break;

      case L7_OSPF_IFSTATE_LOOPBACK:
        *val = D_ospfIfState_loopback;
        break;

      case L7_OSPF_IFSTATE_WAITING:
        *val = D_ospfIfState_waiting;
        break;

      case L7_OSPF_IFSTATE_POINTTOPOINT:
        *val = D_ospfIfState_pointToPoint;
        break;

      case L7_OSPF_IFSTATE_DESIGNATED_ROUTER:
        *val = D_ospfIfState_designatedRouter;
        break;

      case L7_OSPF_IFSTATE_BACKUP_DESIGNATED_ROUTER:
        *val = D_ospfIfState_backupDesignatedRouter;
        break;

      case L7_OSPF_IFSTATE_OTHER_DESIGNATED_ROUTER:
        *val = D_ospfIfState_otherDesignatedRouter;
        break;
*/
      default:
        *val = 0;
        rc = L7_FAILURE;
      }
    }

    return rc;
}

L7_RC_t
snmpOspfVirtIfStatusGet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 Neighbor, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfVirtIfStatusGet(UnitIndex, AreaId, Neighbor, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_ROW_ACTIVE:
      *val = D_ospfVirtIfStatus_active;
      break;

    case L7_OSPF_ROW_NOT_IN_SERVICE:
      *val = D_ospfVirtIfStatus_notInService;
      break;

    case L7_OSPF_ROW_NOT_READY:
      *val = D_ospfVirtIfStatus_notReady;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpOspfVirtIfStatusSet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 Neighbor, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_ospfVirtIfStatus_active:
    break;

  case D_ospfVirtIfStatus_notInService:
  case D_ospfVirtIfStatus_createAndWait:
    rc = L7_FAILURE;
    break;

  case D_ospfVirtIfStatus_createAndGo:
    rc = usmDbOspfVirtIfCreate(UnitIndex, AreaId, Neighbor);
    break;

  case D_ospfVirtIfStatus_destroy:
    rc = usmDbOspfVirtIfDelete(UnitIndex, AreaId, Neighbor);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

/**********************************************************************/

L7_RC_t
snmpOspfNbrEntryGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_int32 AddressLessIndex )
{
  L7_uint32 intIfNum;

  if ( (usmDbIntIfNumFromExtIfNum(AddressLessIndex, &intIfNum) != L7_SUCCESS) ||
       (usmDbOspfNbrEntryGet(UnitIndex, IpAddr, intIfNum) != L7_SUCCESS) )
    return L7_FAILURE;

  return L7_SUCCESS;
}

L7_RC_t
snmpOspfNbrEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *IpAddr, L7_int32 *AddressLessIndex )
{
  L7_uint32 intIfNum;

  if (usmDbExtIfNumTypeCheckValid(UnitIndex, USM_ROUTER_INTF, 0, *AddressLessIndex) != L7_SUCCESS &&
      usmDbExtIfNumTypeNextGet(UnitIndex, USM_ROUTER_INTF, 0, *AddressLessIndex, AddressLessIndex) != L7_SUCCESS)
  {
    *IpAddr += 1; /* increment ip address */
    *AddressLessIndex = 0; /* start from beginning of ports */
  }
  else
  {
    /* convert from external to internal */
    if (usmDbIntIfNumFromExtIfNum(*AddressLessIndex, &intIfNum) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* search for the next entry */
  if ((usmDbOspfNbrEntryGet(UnitIndex, *IpAddr, intIfNum) != L7_SUCCESS &&
       usmDbOspfNbrEntryNext(UnitIndex, IpAddr, &intIfNum) != L7_SUCCESS) ||
      usmDbExtIfNumFromIntIfNum(intIfNum, AddressLessIndex) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

L7_RC_t          
snmpOspfNbrStateGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_int32 AddressLessIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfNbrStateGet (UnitIndex, IpAddr, AddressLessIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_DOWN:
      *val = D_ospfNbrState_down;
      break;
    
    case L7_OSPF_ATTEMPT:
      *val = D_ospfNbrState_attempt;
      break;
    
    case L7_OSPF_INIT:
      *val = D_ospfNbrState_init;
      break;
    
    case L7_OSPF_TWOWAY:
      *val = D_ospfNbrState_twoWay;
      break;
    
    case L7_OSPF_EXCHANGESTART:
      *val = D_ospfNbrState_exchangeStart;
      break;
    
    case L7_OSPF_EXCHANGE:
      *val = D_ospfNbrState_exchange;
      break;
    
    case L7_OSPF_LOADING:
      *val = D_ospfNbrState_loading;
      break;
    
    case L7_OSPF_FULL:
      *val = D_ospfNbrState_full;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpOspfNbmaNbrStatusGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_int32 AddressLessIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfNbmaNbrStatusGet(UnitIndex, IpAddr, AddressLessIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_ROW_ACTIVE:
      *val = D_ospfNbmaNbrStatus_active;
      break;

    case L7_OSPF_ROW_NOT_IN_SERVICE:
      *val = D_ospfNbmaNbrStatus_notInService;
      break;

    case L7_OSPF_ROW_NOT_READY:
      *val = D_ospfNbmaNbrStatus_notReady;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t          
snmpOspfNbmaNbrPermanenceGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_int32 AddressLessIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  rc = usmDbOspfNbmaNbrPermanenceGet (UnitIndex, IpAddr, AddressLessIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_NBMA_NBR_PERMANENCE_DOWN:
      *val = D_ospfNbmaNbrPermanence_dynamic;
      break;
    
    case L7_OSPF_NBMA_NBR_PERMANENCE_PERMANENT:
      *val = D_ospfNbmaNbrPermanence_permanent;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t 
snmpOspfNbrHelloSuppressedGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 AddressLessIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfNbrHelloSuppressedGet (UnitIndex, IpAddr, AddressLessIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_TRUE:
        *val = D_ospfNbrHelloSuppressed_true;
        break;

      case L7_FALSE:
        *val = D_ospfNbrHelloSuppressed_false;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}


/**********************************************************************/

L7_RC_t
snmpOspfAreaAggregateEntryGet ( L7_uint32 UnitIndex, L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask )
{
  L7_uint32 temp_Type;

  switch (Type)
  {
  case D_ospfAreaAggregateLsdbType_summaryLink:
    temp_Type = L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK;
    break;

  case D_ospfAreaAggregateLsdbType_nssaExternalLink:
    temp_Type = L7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK;
    break;

  default:
    return L7_FAILURE;
  }

  return usmDbOspfAreaAggregateEntryGet(UnitIndex, AreaID, temp_Type, Net, Mask);
}


L7_RC_t
snmpOspfAreaAggregateEntryNext ( L7_uint32 UnitIndex, L7_uint32 *AreaID, L7_uint32 *Type, L7_int32 *Net, L7_uint32 *Mask )
{
  L7_uint32 temp_Type;

  switch (*Type)
  {
  case 0:
    temp_Type = 0;
    break;

  case D_ospfAreaAggregateLsdbType_summaryLink:
    temp_Type = L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK;
    break;

  case D_ospfAreaAggregateLsdbType_nssaExternalLink:
    temp_Type = L7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK;
    break;

  default:
    return L7_FAILURE;
  }

  if (usmDbOspfAnyAreaAggregateEntryNext(UnitIndex, AreaID, &temp_Type, Net, Mask) == L7_SUCCESS)
  {
    switch (temp_Type)
    {
    case L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK:
      *Type = D_ospfAreaAggregateLsdbType_summaryLink;
      break;

    case L7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK:
      *Type = D_ospfAreaAggregateLsdbType_nssaExternalLink;
      break;

    default:
      return L7_FAILURE;
    }

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t          
snmpOspfAreaAggregateStatusGet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 *val )
{
  L7_uint32 temp_Type = 0;
  L7_uint32 temp_val;
  L7_RC_t rc = L7_SUCCESS;

  switch (Type)
  {
  case D_ospfAreaAggregateLsdbType_summaryLink:
    temp_Type = L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK;
    break;

  case D_ospfAreaAggregateLsdbType_nssaExternalLink:
    temp_Type = L7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfAreaAggregateStatusGet(UnitIndex, AreaId, temp_Type, Net, Mask, &temp_val);

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_OSPF_ROW_ACTIVE:
        *val = D_ospfAreaAggregateStatus_active;
        break;

      case L7_OSPF_ROW_NOT_IN_SERVICE:
        *val = D_ospfAreaAggregateStatus_notInService;
        break;

      case L7_OSPF_ROW_NOT_READY:
        *val = D_ospfAreaAggregateStatus_notReady;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}

L7_RC_t          
snmpOspfAreaAggregateEffectGet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfAreaAggregateEffectGet (UnitIndex, AreaId, Type, Net, Mask, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_OSPF_AREA_AGGREGATE_EFFECT_ADVERTISEMATCHING:
        *val = D_ospfAreaAggregateEffect_advertiseMatching;
        break;

      case L7_OSPF_AREA_AGGREGATE_EFFECT_DONOT_ADVERTISEMATCHING:
        *val = D_ospfAreaAggregateEffect_doNotAdvertiseMatching;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfAreaAggregateEffectSet ( L7_uint32 UnitIndex, L7_uint32 AreaId, L7_int32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_ospfAreaAggregateEffect_advertiseMatching:
    temp_val = L7_OSPF_AREA_AGGREGATE_EFFECT_ADVERTISEMATCHING;
    break;

  case D_ospfAreaAggregateEffect_doNotAdvertiseMatching:
    temp_val = L7_OSPF_AREA_AGGREGATE_EFFECT_DONOT_ADVERTISEMATCHING;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfAreaAggregateEffectSet (UnitIndex, AreaId, Type, Net, Mask, temp_val);
  }

  return rc;
}

/**********************************************************************/

L7_RC_t 
snmpOspfVirtNbrHelloSuppressedGet ( L7_uint32 UnitIndex, L7_uint32 Area, L7_uint32 RtrId, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbOspfVirtNbrHelloSuppressedGet (UnitIndex, Area, RtrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_TRUE:
        *val = D_ospfVirtNbrHelloSuppressed_true;
        break;

      case L7_FALSE:
        *val = D_ospfVirtNbrHelloSuppressed_false;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

L7_RC_t          
snmpOspfVirtNbrStateGet ( L7_uint32 UnitIndex, L7_uint32 Area, L7_int32 RtrId, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbOspfVirtNbrStateGet (UnitIndex, Area, RtrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_DOWN:
      *val = D_ospfVirtNbrState_down;
      break;
    
    case L7_OSPF_ATTEMPT:
      *val = D_ospfVirtNbrState_attempt;
      break;
    
    case L7_OSPF_INIT:
      *val = D_ospfVirtNbrState_init;
      break;
    
    case L7_OSPF_TWOWAY:
      *val = D_ospfVirtNbrState_twoWay;
      break;
    
    case L7_OSPF_EXCHANGESTART:
      *val = D_ospfVirtNbrState_exchangeStart;
      break;
    
    case L7_OSPF_EXCHANGE:
      *val = D_ospfVirtNbrState_exchange;
      break;
    
    case L7_OSPF_LOADING:
      *val = D_ospfVirtNbrState_loading;
      break;
    
    case L7_OSPF_FULL:
      *val = D_ospfVirtNbrState_full;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**********************************************************************/

L7_RC_t
snmpOspfExtLsdbEntryGet ( L7_uint32 UnitIndex, L7_uint32 Type, L7_uint32 Lsid, L7_int32 RouterId)
{
  L7_uint32 temp_Type;
  L7_ospfLsdbEntry_t Lsa;

  if (Type  == D_ospfExtLsdbType_asExternalLink) 
  {
    temp_Type = L7_S_AS_EXTERNAL_LSA;
  }
  else
    return L7_FAILURE;
  

  return usmDbOspfExtLsdbEntryGet(UnitIndex, temp_Type, Lsid, RouterId, &Lsa);
}

L7_RC_t
snmpOspfExtLsdbEntryNext ( L7_uint32 UnitIndex, L7_uint32 *Type, L7_uint32 *Lsid, L7_int32 *RouterId)
{
  L7_uint32 temp_Type;
  L7_ospfLsdbEntry_t temp_Lsa;

  switch (*Type)
  {
  case 0:
    temp_Type = 0;
    break;

  case D_ospfExtLsdbType_asExternalLink:
    temp_Type = L7_S_AS_EXTERNAL_LSA;
    break;

  default:
    return L7_FAILURE;
    break;
  }

  if (usmDbOspfExtLsdbEntryNext(UnitIndex, &temp_Type, Lsid, RouterId, &temp_Lsa) == L7_SUCCESS)
  {
    if (temp_Type == L7_S_AS_EXTERNAL_LSA)
    {
      *Type = D_ospfExtLsdbType_asExternalLink;
    }
    else
      return L7_FAILURE;

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/**********************************************************************/

L7_RC_t
snmpOspfSetTrapGet( L7_uint32 UnitIndex, L7_char8 *buf )
{
  L7_uint32 j; /* iterator */
  L7_uint32 snmp_trap_index = 0, ospf_trap_index = 0;
  L7_uint32 temp_val, temp_out_val;
  L7_RC_t rc = L7_SUCCESS;

  temp_out_val = 0; /* initialize output value to 0 */
  j = 1; /* start with bit one in position 0 on */

  for (snmp_trap_index = 0; snmp_trap_index < 16; snmp_trap_index++)
  {
    switch (snmp_trap_index)
    {
    case 0: /* ospfVirtIfStateChange */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE;
      break;
    case 1: /* ospfNbrStateChange */
      ospf_trap_index = L7_OSPF_TRAP_NBR_STATE_CHANGE;
      break;
    case 2: /* ospfVirtNbrStateChange */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE;
      break;
    case 3: /* ospfIfConfigError */
      ospf_trap_index = L7_OSPF_TRAP_IF_CONFIG_ERROR;
      break;
    case 4: /* ospfVirtIfConfigError */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR;
      break;
    case 5: /* ospfIfAuthFailure */
      ospf_trap_index = L7_OSPF_TRAP_IF_AUTH_FAILURE;
      break;
    case 6: /* ospfVirtIfAuthFailure */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE;
      break;
    case 7: /* ospfRxBadPacket */
      ospf_trap_index = L7_OSPF_TRAP_RX_BAD_PACKET;
      break;
    case 8: /* ospfVirtIfRxBadPacket */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET;
      break;
    case 9: /* ospfTxRetransmit */
      ospf_trap_index = L7_OSPF_TRAP_TX_RETRANSMIT;
      break;
    case 10: /* ospfVirtIfTxRetransmit */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT;
      break;
    case 11: /* ospfOriginateLSA */
      ospf_trap_index = L7_OSPF_TRAP_ORIGINATE_LSA;
      break;
    case 12: /* ospfMaxAgeLSA */
      ospf_trap_index = L7_OSPF_TRAP_MAX_AGE_LSA;
      break;
    case 13: /* ospfLsDbOverflow */
      ospf_trap_index = L7_OSPF_TRAP_LS_DB_OVERFLOW;
      break;
    case 14: /* ospfLsDbApproachingOverflow */
      ospf_trap_index = L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW;
      break;
    case 15: /* ospfIfStateChange */
      ospf_trap_index = L7_OSPF_TRAP_IF_STATE_CHANGE;
      break;
    }

    /* get the trap status */
    /* if (usmDbOspfTrapModeGet(ospf_trap_index, &temp_val) != L7_SUCCESS)  */
    if (usmDbOspfTrapModeGet(UnitIndex, &temp_val, ospf_trap_index) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    
    if (temp_val == L7_ENABLE)
    {
      temp_out_val = temp_out_val | j;
    }

    /* shift the trap index to the next trap*/
    j = j << 1;
  }

  /* set the first four bytes in buf array to the output value */
  *((L7_uint32*)buf) = temp_out_val;
  /*(L7_uint32)*buf = temp_val;*/
  return rc;
}


L7_RC_t
snmpOspfSetTrapSet( L7_uint32 UnitIndex, L7_char8 *buf )
{
  L7_uint32 j; /* iterator */
  L7_uint32 snmp_trap_index, ospf_trap_index = 0;
  L7_uint32 temp_val;
  L7_RC_t rc = L7_SUCCESS;

  /* cast first four bytes in buf array to a single uint32 */
  temp_val = *((L7_uint32*)buf);
  /*temp_val = (L7_uint32)val;*/

  j = 1; /* start with bit one in position 0 on */

  for (snmp_trap_index = 0; snmp_trap_index < 16; snmp_trap_index++)
  {
    switch (snmp_trap_index)
    {
    case 0: /* ospfVirtIfStateChange */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE;
      break;
    case 1: /* ospfNbrStateChange */
      ospf_trap_index = L7_OSPF_TRAP_NBR_STATE_CHANGE;
      break;
    case 2: /* ospfVirtNbrStateChange */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE;
      break;
    case 3: /* ospfIfConfigError */
      ospf_trap_index = L7_OSPF_TRAP_IF_CONFIG_ERROR;
      break;
    case 4: /* ospfVirtIfConfigError */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR;
      break;
    case 5: /* ospfIfAuthFailure */
      ospf_trap_index = L7_OSPF_TRAP_IF_AUTH_FAILURE;
      break;
    case 6: /* ospfVirtIfAuthFailure */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE;
      break;
    case 7: /* ospfRxBadPacket */
      ospf_trap_index = L7_OSPF_TRAP_RX_BAD_PACKET;
      break;
    case 8: /* ospfVirtIfRxBadPacket */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET;
      break;
    case 9: /* ospfTxRetransmit */
      ospf_trap_index = L7_OSPF_TRAP_TX_RETRANSMIT;
      break;
    case 10: /* ospfVirtIfTxRetransmit */
      ospf_trap_index = L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT;
      break;
    case 11: /* ospfOriginateLSA */
      ospf_trap_index = L7_OSPF_TRAP_ORIGINATE_LSA;
      break;
    case 12: /* ospfMaxAgeLSA */
      ospf_trap_index = L7_OSPF_TRAP_MAX_AGE_LSA;
      break;
    case 13: /* ospfLsDbOverflow */
      ospf_trap_index = L7_OSPF_TRAP_LS_DB_OVERFLOW;
      break;
    case 14: /* ospfLsDbApproachingOverflow */
      ospf_trap_index = L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW;
      break;
    case 15: /* ospfIfStateChange */
      ospf_trap_index = L7_OSPF_TRAP_IF_STATE_CHANGE;
      break;
    }

    if (temp_val & j )
    /*if (temp_val | j != 0)*/
    {
      /* set this trap to enable */
      rc = usmDbOspfTrapModeSet(UnitIndex, L7_ENABLE, ospf_trap_index);
    }
    else
    {
      /* set this trap to disable */
      rc = usmDbOspfTrapModeSet(UnitIndex, L7_DISABLE, ospf_trap_index);
    }

    if (rc != L7_SUCCESS)
    {
      return rc;
    }
    /* shift the trap index to the next trap*/
    j = j << 1;
  }

  return rc;
}

/**********************************************************************/

/**************************************************************************************************************/

L7_RC_t
snmpAgentOspfDefaultMetricGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 temp_val;

  rc = usmDbOspfDefaultMetricGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
     *val = temp_val;

  else if (rc == L7_NOT_EXIST)
  {
    *val = 0;
    rc = L7_SUCCESS;
  }
   
  return rc;
}

L7_RC_t
snmpAgentOspfDefaultMetricSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
 
  rc = usmDbOspfDefaultMetricSet(USMDB_UNIT_CURRENT, val);
  return rc; 
}


L7_RC_t
snmpAgentOspfDefaultMetricConfiguredGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc, rc1;
  L7_uint32 temp_val;

  rc1 = usmDbOspfDefaultMetricGet(USMDB_UNIT_CURRENT, &temp_val);

 
  switch (rc1)
  {
    case L7_SUCCESS:
      *val = D_agentOspfDefaultMetricConfigured_true;
      rc = L7_SUCCESS;
      break;

    case L7_NOT_EXIST:
      *val = D_agentOspfDefaultMetricConfigured_false;
      rc = L7_SUCCESS;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    } 

    return rc;
}

/* Setting of 'DefaultMetricConfigured' will only support setting the MetricConfigured 
   parameter to 'false', so it 'll be used for unconfiguring the metric */
L7_RC_t
snmpAgentOspfDefaultMetricConfiguredSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  
  switch (val)
  {
    case D_agentOspfDefaultMetricConfigured_true:
      rc = L7_FAILURE; /* setting to 'true' is not supported */
      break;

    case D_agentOspfDefaultMetricConfigured_false:
      rc = usmDbOspfDefaultMetricClear(USMDB_UNIT_CURRENT);
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    } 

    return rc; 
}


L7_RC_t
snmpAgentOspfDefaultInfoOriginateGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbOspfDefaultRouteOrigGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentOspfDefaultInfoOriginate_true;
      break;

    case L7_FALSE:
      *val = D_agentOspfDefaultInfoOriginate_false;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentOspfDefaultInfoOriginateSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentOspfDefaultInfoOriginate_true:
    temp_val = L7_TRUE;
    break;

  case D_agentOspfDefaultInfoOriginate_false:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfDefaultRouteOrigSet(USMDB_UNIT_CURRENT, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentOspfDefaultInfoOriginateAlwaysGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbOspfDefaultRouteAlwaysGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentOspfDefaultInfoOriginateAlways_true;
      break;

    case L7_FALSE:
      *val = D_agentOspfDefaultInfoOriginateAlways_false;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentOspfDefaultInfoOriginateAlwaysSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentOspfDefaultInfoOriginateAlways_true:
    temp_val = L7_TRUE;
    break;

  case D_agentOspfDefaultInfoOriginateAlways_false:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfDefaultRouteAlwaysSet(USMDB_UNIT_CURRENT, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentOspfDefaultRouteMetricGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 temp_val;

  rc = usmDbOspfDefaultRouteMetricGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
     *val = temp_val;

  else if (rc == L7_NOT_EXIST)
  {
    *val = 0;
    rc = L7_SUCCESS;
  }
   
  return rc;
}

L7_RC_t
snmpAgentOspfDefaultRouteMetricSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
 
  rc = usmDbOspfDefaultRouteMetricSet(USMDB_UNIT_CURRENT, val);
  return rc; 
}


L7_RC_t
snmpAgentOspfDefaultInfoOrigMetricConfiguredGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbOspfDefaultRouteMetricGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
     *val = D_agentOspfDefaultInfoOriginateMetricConfigured_true;

  else if (rc == L7_NOT_EXIST)
  {
    *val = D_agentOspfDefaultInfoOriginateMetricConfigured_false;
    rc = L7_SUCCESS;
  }
   
  return rc;
}

/* Setting of 'DefaultRouteMetricConfigured' will only support setting this 
   parameter to 'false', so it 'll be used for unconfiguring the metric */
L7_RC_t
snmpAgentOspfDefaultInfoOrigMetricConfiguredSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
    case D_agentOspfDefaultInfoOriginateMetricConfigured_true:
      rc = L7_FAILURE; /* setting to 'true' is not supported */
      break;

    case D_agentOspfDefaultInfoOriginateMetricConfigured_false:
      rc = usmDbOspfDefaultRouteMetricClear(USMDB_UNIT_CURRENT);
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    } 

    return rc; 
}
 

L7_RC_t
snmpAgentOspfDefaultInfoOrigMetricTypeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbOspfDefaultRouteMetricTypeGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_METRIC_EXT_TYPE1:
      *val = D_agentOspfDefaultInfoOriginateMetricType_externalType1;
      break;

    case L7_OSPF_METRIC_EXT_TYPE2:
      *val = D_agentOspfDefaultInfoOriginateMetricType_externalType2;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentOspfDefaultInfoOrigMetricTypeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentOspfDefaultInfoOriginateMetricType_externalType1:
    temp_val = L7_OSPF_METRIC_EXT_TYPE1;
    break;

  case D_agentOspfDefaultInfoOriginateMetricType_externalType2:
    temp_val = L7_OSPF_METRIC_EXT_TYPE2;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfDefaultRouteMetricTypeSet(USMDB_UNIT_CURRENT, temp_val);
  }

  return rc;
}



L7_RC_t
snmpAgentRouterOspfRFC1583CompatibilityModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbOspfRfc1583CompatibilityGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_ENABLE:
      *val = D_agentRouterOspfRFC1583CompatibilityMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentRouterOspfRFC1583CompatibilityMode_disable;
      break;

    default:
      /* unknown value */
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}



L7_RC_t
snmpAgentRouterOspfRFC1583CompatibilityModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch(val)
  {
  case D_agentRouterOspfRFC1583CompatibilityMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentRouterOspfRFC1583CompatibilityMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfRfc1583CompatibilitySet(UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentOspfOpaqueLsaSupportGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val = L7_NULL;

  rc = usmDbOspfOpaqueLsaSupportGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
      case L7_ENABLE:
        *val = D_agentOspfOpaqueLsaSupport_true;
        break;

      case L7_DISABLE:
        *val = D_agentOspfOpaqueLsaSupport_false;
        break;

      default:
        /* unknown value */
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentOspfOpaqueLsaSupportSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch(val)
  {
  case D_agentOspfOpaqueLsaSupport_true:
    temp_val = L7_ENABLE;
    break;

  case D_agentOspfOpaqueLsaSupport_false:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfOpaqueLsaSupportSet(temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentOspfDefaultPassiveModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbOspfPassiveModeGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentOspfDefaultPassiveMode_true;
      break;

    case L7_FALSE:
      *val = D_agentOspfDefaultPassiveMode_false;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentOspfDefaultPassiveModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL temp_val = 0;

  switch (val)
  {
  case D_agentOspfDefaultPassiveMode_true:
    temp_val = L7_TRUE;
    break;

  case D_agentOspfDefaultPassiveMode_false:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfPassiveModeSet(USMDB_UNIT_CURRENT, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentOspfRoutePrefIntraAreaGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return usmDbIpRouterPreferenceGet(USMDB_UNIT_CURRENT, ROUTE_PREF_OSPF_INTRA_AREA, val);
}

L7_RC_t
snmpAgentOspfRoutePrefIntraAreaSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return usmDbIpRouterPreferenceSet(USMDB_UNIT_CURRENT, ROUTE_PREF_OSPF_INTRA_AREA, val);
}

L7_RC_t
snmpAgentOspfRoutePrefInterAreaGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return usmDbIpRouterPreferenceGet(USMDB_UNIT_CURRENT, ROUTE_PREF_OSPF_INTER_AREA, val);
}

L7_RC_t
snmpAgentOspfRoutePrefInterAreaSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return usmDbIpRouterPreferenceSet(USMDB_UNIT_CURRENT, ROUTE_PREF_OSPF_INTER_AREA, val);
}

L7_RC_t
snmpAgentOspfRoutePrefExternalGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return usmDbIpRouterPreferenceGet(USMDB_UNIT_CURRENT, ROUTE_PREF_OSPF_EXTERNAL, val);
}

L7_RC_t
snmpAgentOspfRoutePrefExternalSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return usmDbIpRouterPreferenceSet(USMDB_UNIT_CURRENT, ROUTE_PREF_OSPF_EXTERNAL, val);
}

L7_RC_t
snmpAgentOspfRouteRedistributeSourceGet(L7_uint32 UnitIndex, L7_uint32 redistSource)
{
  L7_RC_t rc;
  L7_uint32 redistSrcProto = L7_NULL;

  if( redistSource <   D_agentOspfRouteRedistSource_connected)
        redistSrcProto = REDIST_RT_FIRST;

  if( redistSource >   D_agentOspfRouteRedistSource_bgp)
        redistSrcProto = REDIST_RT_LAST;

  switch (redistSource)
  {
    case D_agentOspfRouteRedistSource_connected :
      redistSrcProto = REDIST_RT_LOCAL;
      break;

    case D_agentOspfRouteRedistSource_static :
      redistSrcProto = REDIST_RT_STATIC;
      break;

    case D_agentOspfRouteRedistSource_rip :
      redistSrcProto = REDIST_RT_RIP;
      break;

    case D_agentOspfRouteRedistSource_bgp :
      redistSrcProto = REDIST_RT_BGP;
      break;              

    default:
      break;
  }                     

  rc = usmDbOspfRouteRedistributeGet(USMDB_UNIT_CURRENT,redistSrcProto);
  
  return rc;
}

L7_RC_t
snmpAgentOspfRouteRedistributeSourceGetNext(L7_uint32 UnitIndex, L7_uint32 *redistSource)
{
    L7_RC_t rc;
    L7_uint32 redistSrcProto = 0, nextRedistSrcProto;

    if( *redistSource <   D_agentOspfRouteRedistSource_connected)
        redistSrcProto = REDIST_RT_FIRST;

    if( *redistSource >   D_agentOspfRouteRedistSource_bgp)
      return L7_FAILURE;

    switch (*redistSource)
    {
      case D_agentOspfRouteRedistSource_connected :
        redistSrcProto = REDIST_RT_LOCAL;
        break;

      case D_agentOspfRouteRedistSource_static :
        redistSrcProto = REDIST_RT_STATIC;
        break;

      case D_agentOspfRouteRedistSource_rip :
        redistSrcProto = REDIST_RT_RIP;
        break;

      case D_agentOspfRouteRedistSource_bgp :
        redistSrcProto = REDIST_RT_BGP;
        break;              

      default:
        break;
    }                
    
  rc = usmDbOspfRouteRedistributeGetNext(USMDB_UNIT_CURRENT,
                                         redistSrcProto,
                                         &nextRedistSrcProto);
  *redistSource = nextRedistSrcProto;
  return rc;
}

L7_RC_t
snmpAgentOspfRedistMetricGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 temp_val;
  
  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;
  

  rc = usmDbOspfRedistMetricGet(USMDB_UNIT_CURRENT, redistSource, &temp_val);

  if (rc == L7_SUCCESS)
     *val = temp_val;

  else if (rc == L7_NOT_EXIST)
  {
    *val = 0;
    rc = L7_SUCCESS;
  }
   
  return rc;
}

L7_RC_t
snmpAgentOspfRedistMetricSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
 
  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;
  rc = usmDbOspfRedistMetricSet(USMDB_UNIT_CURRENT, redistSource, val);
  return rc; 
}


L7_RC_t
snmpAgentOspfRedistributeModeGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;
  
  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  rc = usmDbOspfRedistributeGet(USMDB_UNIT_CURRENT,redistSource,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE :
      *val = D_agentOspfRouteRedistMode_enable;
      break;

    case L7_FALSE :
      *val = D_agentOspfRouteRedistMode_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentOspfRedistributeModeSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL temp_val = L7_FALSE;

  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  switch (val)
  {
  case D_agentOspfRouteRedistMode_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentOspfRouteRedistMode_disable:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfRedistributeSet(USMDB_UNIT_CURRENT, redistSource, temp_val);
    if(rc == L7_SUCCESS && val == D_agentOspfRouteRedistMode_disable)
      rc = usmDbOspfRedistributeRevert(USMDB_UNIT_CURRENT,redistSource);
  }

  return rc;
}

L7_RC_t
snmpAgentOspfRedistMetricConfiguredGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  rc = usmDbOspfRedistMetricGet(USMDB_UNIT_CURRENT, redistSource, &temp_val);

  if (rc == L7_SUCCESS)
     *val = D_agentOspfRouteRedistMetricConfigured_true;

  else if (rc == L7_NOT_EXIST)
  {
    *val = D_agentOspfRouteRedistMetricConfigured_false;
    rc = L7_SUCCESS;
  }
   
  return rc;
}


/* Setting of 'RedistMetricConfigured' will only support setting this 
   parameter to 'false', so it 'll be used for unconfiguring the metric */
L7_RC_t
snmpAgentOspfRedistMetricConfiguredSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  
  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  switch (val)
  {
    case D_agentOspfRouteRedistMetricConfigured_true:
      rc = L7_FAILURE; /* setting to 'true' is not supported */
      break;

    case D_agentOspfRouteRedistMetricConfigured_false:
      rc = usmDbOspfRedistMetricClear(USMDB_UNIT_CURRENT, redistSource);
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    } 

    return rc; 
}


L7_RC_t
snmpAgentOspfRedistMetricTypeGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  rc = usmDbOspfRedistMetricTypeGet(USMDB_UNIT_CURRENT,redistSource,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_OSPF_METRIC_EXT_TYPE1 :
      *val = D_agentOspfRouteRedistMetricType_externalType1;
      break;

    case L7_OSPF_METRIC_EXT_TYPE2 :
      *val = D_agentOspfRouteRedistMetricType_externalType2;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentOspfRedistMetricTypeSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  switch (val)
  {
  case D_agentOspfRouteRedistMetricType_externalType1:
    temp_val = L7_OSPF_METRIC_EXT_TYPE1;
    break;

  case D_agentOspfRouteRedistMetricType_externalType2:
    temp_val = L7_OSPF_METRIC_EXT_TYPE2;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfRedistMetricTypeSet(USMDB_UNIT_CURRENT, redistSource, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentOspfTagGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 temp_val;

  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  rc = usmDbOspfTagGet(USMDB_UNIT_CURRENT, redistSource, &temp_val);

  if (rc == L7_SUCCESS)
     *val = temp_val;
  
  return rc;
}

L7_RC_t
snmpAgentOspfTagSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
 
  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;
  rc = usmDbOspfTagSet(USMDB_UNIT_CURRENT, redistSource, val);
  return rc; 
}

L7_RC_t
snmpAgentOspfRedistSubnetsGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  rc = usmDbOspfRedistSubnetsGet(USMDB_UNIT_CURRENT,redistSource,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE :
      *val = D_agentOspfRouteRedistSubnets_true;
      break;

    case L7_FALSE :
      *val = D_agentOspfRouteRedistSubnets_false;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentOspfRedistSubnetsSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  switch (val)
  {
  case D_agentOspfRouteRedistSubnets_true:
    temp_val = L7_TRUE;
    break;

  case D_agentOspfRouteRedistSubnets_false:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbOspfRedistSubnetsSet(USMDB_UNIT_CURRENT, redistSource, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentOspfDistListGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 temp_val;

  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  rc = usmDbOspfDistListGet(USMDB_UNIT_CURRENT, redistSource, &temp_val);

  if (rc == L7_SUCCESS)
     *val = temp_val;

  else if (rc == L7_NOT_EXIST)
  {
    *val = 0;
    rc = L7_SUCCESS;
  }
   
  return rc;
}

L7_RC_t
snmpAgentOspfDistListSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
 
  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;
  rc = usmDbOspfDistListSet(USMDB_UNIT_CURRENT, redistSource, val);
  return rc; 
}



L7_RC_t
snmpAgentOspfDistListConfiguredGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  rc = usmDbOspfDistListGet(USMDB_UNIT_CURRENT, redistSource, &temp_val);

  if (rc == L7_SUCCESS)
     *val = D_agentOspfRouteRedistDistListConfigured_true;

  else if (rc == L7_NOT_EXIST)
  {
    *val = D_agentOspfRouteRedistDistListConfigured_false;
    rc = L7_SUCCESS;
  }
   
  return rc;
}


/* Setting of 'DistListConfigured' will only support setting this 
   parameter to 'false', so it 'll be used for unconfiguring the access-list */
L7_RC_t
snmpAgentOspfRedistDistListConfiguredSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  
  if( redistSource ==   D_agentOspfRouteRedistSource_bgp)
      redistSource++;

  switch (val)
  {
    case D_agentOspfRouteRedistDistListConfigured_true:
      rc = L7_FAILURE; /* setting to 'true' is not supported */
      break;

    case D_agentOspfRouteRedistDistListConfigured_false:
      rc = usmDbOspfDistListClear(USMDB_UNIT_CURRENT, redistSource);
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    } 

    return rc; 
}



/**************************************************************************************************************/


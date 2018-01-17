/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfGeneralConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfGeneral-object.xml
*
* @create  28 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfGeneralConfig_obj.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_VersionNumber
*
* @purpose Get 'VersionNumber'
*
* @description [VersionNumber]: The current version number of the OSPF proto-
*              col is 2. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_VersionNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVersionNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfVersionNumberGet (L7_UNIT_CURRENT, &objVersionNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVersionNumberValue,
                     sizeof (objVersionNumberValue));

  /* return the object value: VersionNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVersionNumberValue,
                           sizeof (objVersionNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_AdminStat
*
* @purpose Get 'AdminStat'
*
* @description [AdminStat]: The administrative status of OSPF in the router.
*              The value 'enabled' denotes that the OSPF Process is active
*              on at least one inter- face; 'disabled' disables it on
*              all inter- faces. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_AdminStat (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStatValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfAdminStatGet (L7_UNIT_CURRENT, &objAdminStatValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminStatValue, sizeof (objAdminStatValue));

  /* return the object value: AdminStat */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminStatValue,
                           sizeof (objAdminStatValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_AdminStat
*
* @purpose Set 'AdminStat'
*
* @description [AdminStat]: The administrative status of OSPF in the router.
*              The value 'enabled' denotes that the OSPF Process is active
*              on at least one inter- face; 'disabled' disables it on
*              all inter- faces. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_AdminStat (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStatValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminStat */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminStatValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminStatValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfAdminStatSet (L7_UNIT_CURRENT, objAdminStatValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_ExternLsaCount
*
* @purpose Get 'ExternLsaCount'
*
* @description [ExternLsaCount]: The number of external (LS type 5) link-state
*              advertisements in the link-state database. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_ExternLsaCount (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objExternLsaCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfExternalLSACountGet (L7_UNIT_CURRENT, &objExternLsaCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objExternLsaCountValue,
                     sizeof (objExternLsaCountValue));

  /* return the object value: ExternLsaCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objExternLsaCountValue,
                           sizeof (objExternLsaCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_ExternLsaCksumSum
*
* @purpose Get 'ExternLsaCksumSum'
*
* @description [ExternLsaCksumSum]: The 32-bit unsigned sum of the LS checksums
*              of the external link-state advertisements con- tained
*              in the link-state database. This sum can be used to determine
*              if there has been a change in a router's link state database,
*              and to compare the link-state database of two routers.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_ExternLsaCksumSum (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objExternLsaCksumSumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfExternalLSAChecksumGet (L7_UNIT_CURRENT,
                                     &objExternLsaCksumSumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objExternLsaCksumSumValue,
                     sizeof (objExternLsaCksumSumValue));

  /* return the object value: ExternLsaCksumSum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objExternLsaCksumSumValue,
                           sizeof (objExternLsaCksumSumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_RouterId
*
* @purpose Get 'RouterId'
*
* @description [RouterId]: A 32-bit integer uniquely identifying the router
*              in the Autonomous System. By convention, to ensure uniqueness,
*              this should default to the value of one of the router's
*              IP interface addresses. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_RouterId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRouterIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfRouterIdGet (L7_UNIT_CURRENT, &objRouterIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRouterIdValue, sizeof (objRouterIdValue));

  /* return the object value: RouterId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRouterIdValue,
                           sizeof (objRouterIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_RouterId
*
* @purpose Set 'RouterId'
*
* @description [RouterId]: A 32-bit integer uniquely identifying the router
*              in the Autonomous System. By convention, to ensure uniqueness,
*              this should default to the value of one of the router's
*              IP interface addresses. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_RouterId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRouterIdValue, currentRouterID, ospfMode, rc;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RouterId */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRouterIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRouterIdValue, owa.len);
  
  if (objRouterIdValue == 0)
  {
    owa.rc = XLIBRC_OSPF_INVALID_ROUTERID;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbOspfRouterIdGet(L7_UNIT_CURRENT, &currentRouterID) == L7_SUCCESS)
  {
     /* If non-zero, check admin state of OSPF. */
     if (currentRouterID && (currentRouterID != objRouterIdValue))
     {
        rc = usmDbOspfAdminModeGet(L7_UNIT_CURRENT, &ospfMode);
        if ((rc == L7_SUCCESS) && (ospfMode == L7_ENABLE))
        {
          /* Require OSPF admin state to be disabled.  */
          owa.rc = XLIBRC_OSPF_MUST_BE_DISABLED;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
     }
  }
 
  /* set the value in application */
  owa.l7rc = usmDbOspfRouterIdSet (L7_UNIT_CURRENT, objRouterIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_RxNewLsas
*
* @purpose Get 'RxNewLsas'
*
* @description [RxNewLsas]: The number of link-state advertisements re- ceived
*              determined to be new instantiations. This number does
*              not include newer instantia- tions of self-originated link-state
*              advertise- ments. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_RxNewLsas (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxNewLsasValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfNumLSAReceivedGet (L7_UNIT_CURRENT, &objRxNewLsasValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRxNewLsasValue, sizeof (objRxNewLsasValue));

  /* return the object value: RxNewLsas */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxNewLsasValue,
                           sizeof (objRxNewLsasValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_ASBdrRtrStatus
*
* @purpose Get 'ASBdrRtrStatus'
*
* @description [ASBdrRtrStatus]: A flag to note whether this router is config-
*              ured as an Autonomous System border router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_ASBdrRtrStatus (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objASBdrRtrStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfASBdrRtrStatusGet (L7_UNIT_CURRENT, &objASBdrRtrStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objASBdrRtrStatusValue,
                     sizeof (objASBdrRtrStatusValue));

  /* return the object value: ASBdrRtrStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objASBdrRtrStatusValue,
                           sizeof (objASBdrRtrStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_ASBdrRtrStatus
*
* @purpose Set 'ASBdrRtrStatus'
*
* @description [ASBdrRtrStatus]: A flag to note whether this router is config-
*              ured as an Autonomous System border router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_ASBdrRtrStatus (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objASBdrRtrStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ASBdrRtrStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objASBdrRtrStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objASBdrRtrStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbOspfASBdrRtrStatusSet (L7_UNIT_CURRENT, objASBdrRtrStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_TOSSupport
*
* @purpose Get 'TOSSupport'
*
* @description [TOSSupport]: The router's support for type-of-service rout-
*              ing. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_TOSSupport (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTOSSupportValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfTOSSupportGet (L7_UNIT_CURRENT, &objTOSSupportValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTOSSupportValue, sizeof (objTOSSupportValue));

  /* return the object value: TOSSupport */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTOSSupportValue,
                           sizeof (objTOSSupportValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_TOSSupport
*
* @purpose Set 'TOSSupport'
*
* @description [TOSSupport]: The router's support for type-of-service rout-
*              ing. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_TOSSupport (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTOSSupportValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TOSSupport */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTOSSupportValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTOSSupportValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTOSSupportSet (L7_UNIT_CURRENT, objTOSSupportValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_AreaBdrRtrStatus
*
* @purpose Get 'AreaBdrRtrStatus'
*
* @description [AreaBdrRtrStatus]: A flag to note whether this router is an
*              area border router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_AreaBdrRtrStatus (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaBdrRtrStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfAreaBdrRtrStatusGet (L7_UNIT_CURRENT, &objAreaBdrRtrStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAreaBdrRtrStatusValue,
                     sizeof (objAreaBdrRtrStatusValue));

  /* return the object value: AreaBdrRtrStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAreaBdrRtrStatusValue,
                           sizeof (objAreaBdrRtrStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_MulticastExtensions
*
* @purpose Get 'MulticastExtensions'
*
* @description [MulticastExtensions]: A Bit Mask indicating whether the router
*              is forwarding IP multicast (Class D) datagrams based on
*              the algorithms defined in the Multi- cast Extensions to OSPF.
*              Bit 0, if set, indicates that the router can forward IP
*              multicast datagrams in the router's directly attached areas
*              (called intra-area mul- ticast routing). Bit 1, if set,
*              indicates that the router can forward IP multicast datagrams
*              between OSPF areas (called inter-area multicast routing).
*              Bit 2, if set, indicates that the router can forward IP multicast
*              datagrams between Auto- nomous Systems (called inter-AS
*              multicast rout- ing). Only certain combinations of bit
*              settings are allowed, namely: 0 (no multicast forwarding is
*              enabled), 1 (intra-area multicasting only), 3 (intra-area
*              and inter-area multicasting), 5 (intra-area and inter-AS multicasting)
*              and 7 (multicasting everywhere). By default, no
*              mul- ticast forwarding is enabled. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_MulticastExtensions (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastExtensionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfMulticastExtensionsGet (L7_UNIT_CURRENT,
                                     &objMulticastExtensionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastExtensionsValue,
                     sizeof (objMulticastExtensionsValue));

  /* return the object value: MulticastExtensions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastExtensionsValue,
                           sizeof (objMulticastExtensionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_MulticastExtensions
*
* @purpose Set 'MulticastExtensions'
*
* @description [MulticastExtensions]: A Bit Mask indicating whether the router
*              is forwarding IP multicast (Class D) datagrams based on
*              the algorithms defined in the Multi- cast Extensions to OSPF.
*              Bit 0, if set, indicates that the router can forward IP
*              multicast datagrams in the router's directly attached areas
*              (called intra-area mul- ticast routing). Bit 1, if set,
*              indicates that the router can forward IP multicast datagrams
*              between OSPF areas (called inter-area multicast routing).
*              Bit 2, if set, indicates that the router can forward IP multicast
*              datagrams between Auto- nomous Systems (called inter-AS
*              multicast rout- ing). Only certain combinations of bit
*              settings are allowed, namely: 0 (no multicast forwarding is
*              enabled), 1 (intra-area multicasting only), 3 (intra-area
*              and inter-area multicasting), 5 (intra-area and inter-AS multicasting)
*              and 7 (multicasting everywhere). By default, no
*              mul- ticast forwarding is enabled. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_MulticastExtensions (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastExtensionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastExtensions */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMulticastExtensionsValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastExtensionsValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbOspfMulticastExtensionsSet (L7_UNIT_CURRENT,
                                     objMulticastExtensionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_ExtLsdbLimit
*
* @purpose Get 'ExtLsdbLimit'
*
* @description [ExtLsdbLimit]: The maximum number of non-default AS- external-LSAs
*              entries that can be stored in the link-state database.
*              If the value is -1, then there is no limit. When the number
*              of non-default AS-external-LSAs in a router's link-state
*              database reaches ospfExtLsdbLimit, the router enters Overflow-
*              State. The router never holds more than ospfExtLsdbLimit
*              non-default AS-external-LSAs in its database. OspfExtLsdbLimit
*              MUST be set identically in all routers attached to
*              the OSPF backbone and/or any regular OSPF area. (i.e., OSPF
*              stub areas and NSSAs are excluded). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_ExtLsdbLimit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibS32_t));
  xLibS32_t objExtLsdbLimitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  memset(&objExtLsdbLimitValue, 0, sizeof(objExtLsdbLimitValue));

  owa.l7rc = usmDbOspfExtLsdbLimitGet (L7_UNIT_CURRENT, &objExtLsdbLimitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objExtLsdbLimitValue,
                     sizeof (objExtLsdbLimitValue));

  /* return the object value: ExtLsdbLimit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objExtLsdbLimitValue,
                           sizeof (objExtLsdbLimitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_ExtLsdbLimit
*
* @purpose Set 'ExtLsdbLimit'
*
* @description [ExtLsdbLimit]: The maximum number of non-default AS- external-LSAs
*              entries that can be stored in the link-state database.
*              If the value is -1, then there is no limit. When the number
*              of non-default AS-external-LSAs in a router's link-state
*              database reaches ospfExtLsdbLimit, the router enters Overflow-
*              State. The router never holds more than ospfExtLsdbLimit
*              non-default AS-external-LSAs in its database. OspfExtLsdbLimit
*              MUST be set identically in all routers attached to
*              the OSPF backbone and/or any regular OSPF area. (i.e., OSPF
*              stub areas and NSSAs are excluded). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_ExtLsdbLimit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibS32_t objExtLsdbLimitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ExtLsdbLimit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objExtLsdbLimitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objExtLsdbLimitValue, owa.len);
  
  if((objExtLsdbLimitValue < L7_OSPF_EXT_LSDB_LIMIT_MIN) ||
     (objExtLsdbLimitValue > L7_OSPF_EXT_LSDB_LIMIT_MAX))
  {
    owa.rc = XLIBRC_OSPF_EXTLSDB_LIMIT_INVALID_RANGE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbOspfExtLsdbLimitSet (L7_UNIT_CURRENT, objExtLsdbLimitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_OriginateNewLsas
*
* @purpose Get 'OriginateNewLsas'
*
* @description [OriginateNewLsas]: The number of new link-state advertisements
*              that have been originated. This number is in- cremented
*              each time the router originates a new LSA. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_OriginateNewLsas (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOriginateNewLsasValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfNewLSAOrigGet (L7_UNIT_CURRENT, &objOriginateNewLsasValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objOriginateNewLsasValue,
                     sizeof (objOriginateNewLsasValue));

  /* return the object value: OriginateNewLsas */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOriginateNewLsasValue,
                           sizeof (objOriginateNewLsasValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_ExitOverflowInterval
*
* @purpose Get 'ExitOverflowInterval'
*
* @description [ExitOverflowInterval]: The number of seconds that, after entering
*              OverflowState, a router will attempt to leave OverflowState.
*              This allows the router to again originate non-default
*              AS-external-LSAs. When set to 0, the router will not leave
*              Overflow- State until restarted. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_ExitOverflowInterval (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objExitOverflowIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfExitOverflowIntervalGet (L7_UNIT_CURRENT,
                                      &objExitOverflowIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objExitOverflowIntervalValue,
                     sizeof (objExitOverflowIntervalValue));

  /* return the object value: ExitOverflowInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objExitOverflowIntervalValue,
                           sizeof (objExitOverflowIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_ExitOverflowInterval
*
* @purpose Set 'ExitOverflowInterval'
*
* @description [ExitOverflowInterval]: The number of seconds that, after entering
*              OverflowState, a router will attempt to leave OverflowState.
*              This allows the router to again originate non-default
*              AS-external-LSAs. When set to 0, the router will not leave
*              Overflow- State until restarted. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_ExitOverflowInterval (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objExitOverflowIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ExitOverflowInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objExitOverflowIntervalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objExitOverflowIntervalValue, owa.len);
  /* set the value in application */
  owa.l7rc =
    usmDbOspfExitOverflowIntervalSet (L7_UNIT_CURRENT,
                                      objExitOverflowIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_DemandExtensions
*
* @purpose Get 'DemandExtensions'
*
* @description [DemandExtensions]: The router's support for demand routing.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_DemandExtensions (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDemandExtensionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfDemandExtensionsGet (L7_UNIT_CURRENT, &objDemandExtensionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDemandExtensionsValue,
                     sizeof (objDemandExtensionsValue));

  /* return the object value: DemandExtensions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDemandExtensionsValue,
                           sizeof (objDemandExtensionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_opaqueASLSACount
*
* @purpose Get 'opaqueASLSACount'
 *@description  [opaqueASLSACount] Get the number of opaque AS (LS type 11)
* link-state advertisements in the link-state database.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_opaqueASLSACount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objopaqueASLSACountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfOpaqueASLSACountGet(L7_UNIT_CURRENT, &objopaqueASLSACountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objopaqueASLSACountValue, sizeof (objopaqueASLSACountValue));

  /* return the object value: opaqueASLSACount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objopaqueASLSACountValue,
                           sizeof (objopaqueASLSACountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_ASOpaqueLSAChecksumSum
*
* @purpose Get 'ASOpaqueLSAChecksumSum'
 *@description  [ASOpaqueLSAChecksumSum] Get checksumSum of the AS OPAQUE LSA.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_ASOpaqueLSAChecksumSum (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objASOpaqueLSAChecksumSumValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfASOpaqueLSAChecksumSumGet(&objASOpaqueLSAChecksumSumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objASOpaqueLSAChecksumSumValue,
                     sizeof (objASOpaqueLSAChecksumSumValue));

  /* return the object value: ASOpaqueLSAChecksumSum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objASOpaqueLSAChecksumSumValue,
                           sizeof (objASOpaqueLSAChecksumSumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_SelfOrigExtLsaFlush
*
* @purpose Set 'SelfOrigExtLsaFlush'
 *@description  [SelfOrigExtLsaFlush] Flush and reoriginate all self-originated
* external LSAs.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_SelfOrigExtLsaFlush (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSelfOrigExtLsaFlushValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SelfOrigExtLsaFlush */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSelfOrigExtLsaFlushValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSelfOrigExtLsaFlushValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbOspfMapSelfOrigExtLsaFlush();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_CountersClear
*
* @purpose Set 'CountersClear'
 *@description  [CountersClear] Clear OSPFv2 statistics.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_CountersClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCountersClearValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CountersClear */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCountersClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCountersClearValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbOspfCountersClear();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_OspfHelperSupportMode
*
* @purpose Get 'OspfHelperSupportMode'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_OspfHelperSupportMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOspfHelperSupportModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfHelperSupportGet (&objOspfHelperSupportModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objOspfHelperSupportModeValue, sizeof (objOspfHelperSupportModeValue));

  /* return the object value: OspfHelperSupportMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOspfHelperSupportModeValue,
                           sizeof (objOspfHelperSupportModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_OspfHelperSupportMode
*
* @purpose Set 'OspfHelperSupportMode'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_OspfHelperSupportMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOspfHelperSupportModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: OspfHelperSupportMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objOspfHelperSupportModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objOspfHelperSupportModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfHelperSupportSet (objOspfHelperSupportModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfGeneralConfig_OspfHelperStrictLSAChecking
*
* @purpose Get 'OspfHelperStrictLSAChecking'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfGeneralConfig_OspfHelperStrictLSAChecking (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOspfHelperStrictLSACheckingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfStrictLsaCheckingGet (&objOspfHelperStrictLSACheckingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objOspfHelperStrictLSACheckingValue, sizeof (objOspfHelperStrictLSACheckingValue));

  /* return the object value: OspfHelperStrictLSAChecking */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOspfHelperStrictLSACheckingValue,
                           sizeof (objOspfHelperStrictLSACheckingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingospfGeneralConfig_OspfHelperStrictLSAChecking
*
* @purpose Set 'OspfHelperStrictLSAChecking'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfGeneralConfig_OspfHelperStrictLSAChecking (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOspfHelperStrictLSACheckingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: OspfHelperStrictLSAChecking */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objOspfHelperStrictLSACheckingValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objOspfHelperStrictLSACheckingValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfStrictLsaCheckingSet (objOspfHelperStrictLSACheckingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

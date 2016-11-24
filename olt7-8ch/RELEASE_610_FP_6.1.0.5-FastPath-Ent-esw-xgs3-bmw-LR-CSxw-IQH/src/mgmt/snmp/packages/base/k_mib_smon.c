/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_smon.c
*
* @purpose  Provide interface to SMON MIB
*
* @component SNMP
*
* @comments
*
* @create 03/23/2005
*
* @author Milind Mahadik
* @end*
**********************************************************************/
#include "snmpapi.h"
#include "k_private_base.h"
#include "k_mib_smon_api.h"
#include "k_mib_if_api.h"
#include "snmp_util_api.h"
#include "comm_mask.h"
#include "usmdb_common.h"

/* Comment out the methods generated for RMON2 and Token Ring MIBS. */

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
smonVlanStatsControlEntry_t *
k_smonVlanStatsControlEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_INT32 smonVlanStatsControlIndex)
{
#ifdef NOT_YET
   static smonVlanStatsControlEntry_t smonVlanStatsControlEntryData;

   /*
    * put your code to retrieve the information here
    */

   smonVlanStatsControlEntryData.smonVlanStatsControlIndex = ;
   smonVlanStatsControlEntryData.smonVlanStatsControlDataSource = ;
   smonVlanStatsControlEntryData.smonVlanStatsControlCreateTime = ;
   smonVlanStatsControlEntryData.smonVlanStatsControlOwner = ;
   smonVlanStatsControlEntryData.smonVlanStatsControlStatus = ;
   SET_ALL_VALID(smonVlanStatsControlEntryData.valid);
   return(&smonVlanStatsControlEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
int
k_smonVlanStatsControlEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
int
k_smonVlanStatsControlEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
int
k_smonVlanStatsControlEntry_set_defaults(doList_t *dp)
{
    smonVlanStatsControlEntry_t *data = 
                                   (smonVlanStatsControlEntry_t *) (dp->data);

    if ((data->smonVlanStatsControlDataSource = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->smonVlanStatsControlOwner = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
int
k_smonVlanStatsControlEntry_set(smonVlanStatsControlEntry_t *data,
                                ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
#ifdef SR_smonVlanStatsControlEntry_UNDO
/* add #define SR_smonVlanStatsControlEntry_UNDO in sitedefs.h to
 * include the undo routine for the smonVlanStatsControlEntry family.
 */
int
smonVlanStatsControlEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_smonVlanStatsControlEntry_UNDO */

#endif /* SETS */

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
smonVlanIdStatsEntry_t *
k_smonVlanIdStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 smonVlanStatsControlIndex,
                           SR_INT32 smonVlanIdStatsId)
{
#ifdef NOT_YET
   static smonVlanIdStatsEntry_t smonVlanIdStatsEntryData;

   /*
    * put your code to retrieve the information here
    */

   smonVlanIdStatsEntryData.smonVlanIdStatsId = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsTotalPkts = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsTotalOverflowPkts = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsTotalHCPkts = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsTotalOctets = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsTotalOverflowOctets = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsTotalHCOctets = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsNUcastPkts = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsNUcastOverflowPkts = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsNUcastHCPkts = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsNUcastOctets = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsNUcastOverflowOctets = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsNUcastHCOctets = ;
   smonVlanIdStatsEntryData.smonVlanIdStatsCreateTime = ;
   smonVlanIdStatsEntryData.smonVlanStatsControlIndex = ;
   SET_ALL_VALID(smonVlanIdStatsEntryData.valid);
   return(&smonVlanIdStatsEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
smonPrioStatsControlEntry_t *
k_smonPrioStatsControlEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_INT32 smonPrioStatsControlIndex)
{
#ifdef NOT_YET
   static smonPrioStatsControlEntry_t smonPrioStatsControlEntryData;

   /*
    * put your code to retrieve the information here
    */

   smonPrioStatsControlEntryData.smonPrioStatsControlIndex = ;
   smonPrioStatsControlEntryData.smonPrioStatsControlDataSource = ;
   smonPrioStatsControlEntryData.smonPrioStatsControlCreateTime = ;
   smonPrioStatsControlEntryData.smonPrioStatsControlOwner = ;
   smonPrioStatsControlEntryData.smonPrioStatsControlStatus = ;
   SET_ALL_VALID(smonPrioStatsControlEntryData.valid);
   return(&smonPrioStatsControlEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
#ifdef SETS
int
k_smonPrioStatsControlEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
int
k_smonPrioStatsControlEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
int
k_smonPrioStatsControlEntry_set_defaults(doList_t *dp)
{
    smonPrioStatsControlEntry_t *data = 
                                   (smonPrioStatsControlEntry_t *) (dp->data);

    if ((data->smonPrioStatsControlDataSource = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->smonPrioStatsControlOwner = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
int
k_smonPrioStatsControlEntry_set(smonPrioStatsControlEntry_t *data,
                                ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
#ifdef SR_smonPrioStatsControlEntry_UNDO
/* add #define SR_smonPrioStatsControlEntry_UNDO in sitedefs.h to
 * include the undo routine for the smonPrioStatsControlEntry family.
 */
int
smonPrioStatsControlEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_smonPrioStatsControlEntry_UNDO */

#endif /* SETS */
/*********************************************************************
  Function:
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Not Supported.
  Change Report:
*********************************************************************/
smonPrioStatsEntry_t *
k_smonPrioStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 smonPrioStatsControlIndex,
                         SR_INT32 smonPrioStatsId)
{
#ifdef NOT_YET
   static smonPrioStatsEntry_t smonPrioStatsEntryData;

   /*
    * put your code to retrieve the information here
    */

   smonPrioStatsEntryData.smonPrioStatsId = ;
   smonPrioStatsEntryData.smonPrioStatsPkts = ;
   smonPrioStatsEntryData.smonPrioStatsOverflowPkts = ;
   smonPrioStatsEntryData.smonPrioStatsHCPkts = ;
   smonPrioStatsEntryData.smonPrioStatsOctets = ;
   smonPrioStatsEntryData.smonPrioStatsOverflowOctets = ;
   smonPrioStatsEntryData.smonPrioStatsHCOctets = ;
   smonPrioStatsEntryData.smonPrioStatsControlIndex = ;
   SET_ALL_VALID(smonPrioStatsEntryData.valid);
   return(&smonPrioStatsEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}


/*********************************************************************
 * SMON MIB : Methods to support the port copy functionality         *
 *            of the SMON MIB.                                       *
 *********************************************************************/
/*********************************************************************
  Function:    k_dataSourceCapsEntry_get
  Purpose:
  Arguments:
  Comment:     Support for the MIB object - Required for port copy.
  Change Report:
*********************************************************************/
dataSourceCapsEntry_t *
k_dataSourceCapsEntry_get(int serialNum,
                          ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          OID * dataSourceCapsObject)
{
#ifdef NOT_YET
   static dataSourceCapsEntry_t dataSourceCapsEntryData;

   /*
    * put your code to retrieve the information here
    */

   dataSourceCapsEntryData.dataSourceCapsObject = ;
   dataSourceCapsEntryData.dataSourceRmonCaps = ;
   dataSourceCapsEntryData.dataSourceCopyCaps = ;
   dataSourceCapsEntryData.dataSourceCapsIfIndex = ;
   SET_ALL_VALID(dataSourceCapsEntryData.valid);
   return(&dataSourceCapsEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

/*********************************************************************
  Function: k_portCopyEntry_get
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
portCopyEntry_t *
k_portCopyEntry_get(int serialNum,
                    ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 portCopySource,
                    SR_INT32 portCopyDest)
{
  /* Buffer to store ThePortCopyEntry object. */
  static portCopyEntry_t portCopyEntryData;

  L7_uint32 intIfNum;
  L7_uint32 value;

  /* Invalidate ThePortCopyEntry object's contents. */
  ZERO_VALID(portCopyEntryData.valid);

  /* Set the ThePortCopyEntry object's indices */
  portCopyEntryData.portCopySource = portCopySource;
  SET_VALID(I_portCopySource, portCopyEntryData.valid);
  portCopyEntryData.portCopyDest = portCopyDest;
  SET_VALID(I_portCopyDest, portCopyEntryData.valid);

  /* Check for the feasiblity of get or get-next. */
  if ((searchType == EXACT) ?
      (snmpSmonPortCopyEntryGet(USMDB_UNIT_CURRENT, 
                            portCopyEntryData.portCopySource, 
                            portCopyEntryData.portCopyDest) != L7_SUCCESS) :
      ((snmpSmonPortCopyEntryGet(USMDB_UNIT_CURRENT, 
                            portCopyEntryData.portCopySource,
                            portCopyEntryData.portCopyDest) != L7_SUCCESS) &&
       (snmpSmonPortCopyEntryNextGet(USMDB_UNIT_CURRENT, 
		&portCopyEntryData.portCopySource, 
                            &portCopyEntryData.portCopyDest) != L7_SUCCESS)))
  {
    /* Failed to get the next object. Invalidate the object's contents.*/
    ZERO_VALID(portCopyEntryData.valid);

    /* Return NULL indicating requested object is not supported. */
    return(NULL);
  }

  /* Step 4.0: Get the columnar objects. */
  switch (nominator)
  {
  case -1:
  case I_portCopySource:
  case I_portCopyDest:
    break;

  case I_portCopyDestDropEvents:
    if ( snmpSmonPortCopyDestDropEventsGet(USMDB_UNIT_CURRENT,
                                   portCopyEntryData.portCopySource,
                                   portCopyEntryData.portCopyDest,
                                   &portCopyEntryData.portCopyDestDropEvents)
                                   == L7_SUCCESS )
      SET_VALID(I_portCopyDestDropEvents, portCopyEntryData.valid);
    if(nominator != -1)
      break;
    /* else pass thru */

  case I_portCopyDirection:
    if ( snmpSmonPortCopyDirectionGet(USMDB_UNIT_CURRENT,
                                   portCopyEntryData.portCopySource,
                                   portCopyEntryData.portCopyDest,
                                   &portCopyEntryData.portCopyDirection) 
                                   == L7_SUCCESS )
      SET_VALID(I_portCopyDirection, portCopyEntryData.valid);
    if(nominator != -1)
      break;
    /* else pass thru */

  case I_portCopyStatus:

    if((nimGetIntfNumber(portCopyEntryData.portCopySource, 
                           &intIfNum) != L7_SUCCESS) ||
    (snmpIfOperStatusGet(USMDB_UNIT_CURRENT, intIfNum, &value) != L7_SUCCESS) ||
    (value != D_ifOperStatus_up) ||
    (snmpIfAdminStatusGet(USMDB_UNIT_CURRENT, intIfNum, 
                          &value) != L7_SUCCESS) ||
    (value != D_ifAdminStatus_up) ||
    (nimGetIntfNumber(portCopyEntryData.portCopyDest, 
                      &intIfNum) != L7_SUCCESS) ||
    (snmpIfOperStatusGet(USMDB_UNIT_CURRENT, intIfNum, &value) != L7_SUCCESS) ||
    (value != D_ifOperStatus_up) ||
    (snmpIfAdminStatusGet(USMDB_UNIT_CURRENT, intIfNum, 
                          &value) != L7_SUCCESS) ||
    (value != D_ifAdminStatus_up))
    {
      portCopyEntryData.portCopyStatus = D_portCopyStatus_notReady;
      SET_VALID(I_portCopyStatus, portCopyEntryData.valid);
    }
    else if ( snmpSmonPortCopyStatusGet(USMDB_UNIT_CURRENT,
                                        portCopyEntryData.portCopySource,
                                        portCopyEntryData.portCopyDest,
                                        &portCopyEntryData.portCopyStatus)
                                     == L7_SUCCESS )
      SET_VALID(I_portCopyStatus, portCopyEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  /* Step 5.0: Return NULL if object's contents are invalid. */
  if ( nominator >= 0 && !VALID(nominator, portCopyEntryData.valid) )
    return(NULL);

  /* Step 6.0: Return the reference to the object. */
  return(&portCopyEntryData);
}

#ifdef SETS
/*********************************************************************
  Function: k_portCopyEntry_test
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     <TBD>
  Change Report:
*********************************************************************/
int
k_portCopyEntry_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{
  portCopyEntry_t *data = (portCopyEntry_t*) (dp->data);
  L7_uint32 intIfNumSrc;
  L7_uint32 intIfNumDest;
  L7_uint32 val;

  if(nimGetIntfNumber(data->portCopySource, &intIfNumSrc) != L7_SUCCESS)
  {
    return(COMMIT_FAILED_ERROR);
  } 
  if(nimGetIntfNumber(data->portCopyDest, &intIfNumDest) != L7_SUCCESS)
  {
    return(COMMIT_FAILED_ERROR);
  } 

  if((strcmp(object->oidname, "portCopyStatus") != 0) ||
     (value->u.slval != D_portCopyStatus_createAndGo))
  {
    return NO_ERROR;
  }

  if((snmpIfOperStatusGet(USMDB_UNIT_CURRENT, intIfNumSrc, 
                          &val) == L7_SUCCESS) &&
     (val == D_ifOperStatus_up) &&
     (snmpIfAdminStatusGet(USMDB_UNIT_CURRENT, intIfNumSrc, 
                           &val) == L7_SUCCESS) &&
     (val == D_ifAdminStatus_up) &&
     (snmpIfOperStatusGet(USMDB_UNIT_CURRENT, intIfNumDest, 
                          &val) == L7_SUCCESS) &&
     (val == D_ifOperStatus_up) &&
     (snmpIfAdminStatusGet(USMDB_UNIT_CURRENT, intIfNumDest, 
                           &val) == L7_SUCCESS) &&
     (val == D_ifAdminStatus_up))
  {
    return NO_ERROR;
  }
  return(COMMIT_FAILED_ERROR);
}

/*********************************************************************
  Function: k_portCopyEntry_ready
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/

int
k_portCopyEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *doHead, doList_t *dp)
{

   /* Step 1.0: Get pointer to the entry object. */
   /*portCopyEntry_t *pCopyEntryData = (portCopyEntry_t*) (dp->data);*/

   /* <TBD> List out and implement the cross-check the points.*/

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

/*********************************************************************
  Function: k_portCopyEntry_set_defaults
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
int
k_portCopyEntry_set_defaults(doList_t *dp)
{
    portCopyEntry_t *data = (portCopyEntry_t *) (dp->data);

    /* <TBD> The default values to be set for the interfaces.*/
    data->portCopyDestDropEvents = (SR_UINT32) 0;
    data->portCopyDirection = D_portCopyDirection_copyBoth;
    /* <TBD> The default values to be set for row status.*/

    ZERO_VALID(data->valid);

    return NO_ERROR;
}

/*********************************************************************
  Function: k_portCopyEntry_set
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
int
k_portCopyEntry_set(portCopyEntry_t *data,
                    ContextInfo *contextInfo,
                    int function)
{
  /* Step 1.0: Allocate the buffers for data.*/
  char buffer[SNMP_BUFFER_LEN];
  char subBuffer[SNMP_BUFFER_LEN];
  char ownerBuffer[SNMP_BUFFER_LEN];
  L7_BOOL isSetSuccess = L7_FALSE;

  /*
   * Defining temporary variable for storing the valid bits for the case 
   * when the set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));
  bzero(buffer,SNMP_BUFFER_LEN);
  bzero(subBuffer,SNMP_BUFFER_LEN);
  bzero(ownerBuffer,SNMP_BUFFER_LEN);

  /* Step 2.0: */
  /* Evalauate the row status column value. */
  if((data->portCopyStatus == D_portCopyStatus_createAndGo) &&
     (snmpSmonPortCopyEntryGet(USMDB_UNIT_CURRENT, data->portCopySource, 
                                     data->portCopyDest) == L7_SUCCESS))
  {
     return(COMMIT_FAILED_ERROR);
  }
  else if((data->portCopyStatus == D_portCopyStatus_destroy) &&
          (snmpSmonPortCopyEntryGet(USMDB_UNIT_CURRENT, 
                                    data->portCopySource, 
                                    data->portCopyDest) != L7_SUCCESS))
  {
     return(COMMIT_FAILED_ERROR);
  }
  else
  {
    /* check for row creation */
    if (VALID(I_portCopyStatus, data->valid) &&
       ((data->portCopyStatus == D_portCopyStatus_createAndGo) || 
        (data->portCopyStatus == D_portCopyStatus_active) ||
        (data->portCopyStatus == D_portCopyStatus_destroy)) &&
       snmpSmonPortCopyStatusSet(USMDB_UNIT_CURRENT, 
                                 data->portCopySource, 
                                 data->portCopyDest, 
                                 data->portCopyStatus) == L7_SUCCESS)
    {
      isSetSuccess = L7_TRUE;
      SET_VALID(I_portCopyStatus, tempValid);
    }
    if(VALID(I_portCopyDirection, data->valid) &&
           (snmpSmonPortCopyDirectionSet(USMDB_UNIT_CURRENT, 
                                         data->portCopySource, 
                                         data->portCopyDest, 
                                         data->portCopyDirection) 
                                         == L7_SUCCESS))
    {
      isSetSuccess = L7_TRUE;
      SET_VALID(I_portCopyDirection, tempValid);
    }
    if(isSetSuccess != L7_TRUE)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }

  }

  return(NO_ERROR);

}

/*********************************************************************
  Function: portCopyEntry_undo
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
#ifdef SR_portCopyEntry_UNDO
/* add #define SR_portCopyEntry_UNDO in sitedefs.h to
 * include the undo routine for the portCopyEntry family.
 */
int
portCopyEntry_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{

   portCopyEntry_t *data = (portCopyEntry_t *) doCur->data;
   portCopyEntry_t *undodata = (portCopyEntry_t *) doCur->undodata;
   portCopyEntry_t *setdata = NULL;
   int function = SR_UNKNOWN;

   /*
    * Modifications for UNDO Feature
    * Setting valid bits of undodata same as that for data 
    */
   if ( data->valid == NULL || undodata == NULL )
     return UNDO_FAILED_ERROR;
   memcpy(undodata->valid,data->valid,sizeof(data->valid));

   /* undoing a modify, replace the original data */
   setdata = undodata;
   function = SR_ADD_MODIFY;

   /* use the set method for the undo */
   if (k_portCopyEntry_set(setdata, contextInfo, function) == NO_ERROR)
     return NO_ERROR;

   return UNDO_FAILED_ERROR;

}
#endif /* SR_portCopyEntry_UNDO */
#endif /* SETS */


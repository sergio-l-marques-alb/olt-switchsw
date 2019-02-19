/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_qos_diffserv_private.c
*
* @purpose  Provide interface to DiffServ Private MIB
*                  
* @component SNMP
*
* @comments 
*
* @create 07/12/2002
*
* @author cpverne
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "k_mib_qos_diffserv_private_api.h"
#include "cnfgr.h"
#include "../../../../../src/l7public/api/l7_diffserv_api.h"
#include "usmdb_common.h"
#include "diffserv_exports.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_util_api.h"

extern L7_RC_t diffServPolicyInstGetNext(L7_uint32 prevPolicyIndex,
                                  L7_uint32 prevPolicyInstIndex,
                                  L7_uint32 *pPolicyIndex,
                                  L7_uint32 *pPolicyInstIndex);

extern L7_RC_t diffServPolicyAttrGetNext(L7_uint32 prevPolicyIndex,
                                  L7_uint32 prevPolicyInstIndex,
                                  L7_uint32 prevPolicyAttrIndex,
                                  L7_uint32 *pPolicyIndex,
                                  L7_uint32 *pPolicyInstIndex,
                                  L7_uint32 *pPolicyAttrIndex);

extern L7_RC_t diffServClassRuleGetNext(L7_uint32 prevClassIndex,
                                 L7_uint32 prevClassRuleIndex,
                                 L7_uint32 *pClassIndex,
                                 L7_uint32 *pClassRuleIndex);

extern L7_RC_t diffServPolicyAttrSetTest(L7_uint32 policyIndex,
                                  L7_uint32 policyInstIndex,
                                  L7_uint32 policyAttrIndex,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid,
                                  void *pValue);


L7_uint32 agentDiffServPolicyInstEntry_count =0 ;
L7_uint32 agentDiffServPolicyAttrEntry_count = 0;
L7_uint32 agentDiffServClassRuleEntry_count=0;


/* These variables are declared to avoid deletion error in undo method. */
L7_BOOL creationErrorClassEntry = L7_FALSE;
L7_BOOL creationErrorClassRuleEntry = L7_FALSE;
L7_BOOL creationErrorPolicyEntry = L7_FALSE;
L7_BOOL creationErrorPolicyInstEntry = L7_FALSE;
L7_BOOL creationErrorPolicyAttrEntry = L7_FALSE;
L7_BOOL creationErrorServiceEntry = L7_FALSE;



agentDiffServGenStatusGroup_t *
k_agentDiffServGenStatusGroup_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator)
{
static agentDiffServGenStatusGroup_t agentDiffServGenStatusGroupData;
   
  ZERO_VALID(agentDiffServGenStatusGroupData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }



 /*
  * if ( nominator != -1 ) condition is added to all the case statements 
  * for storing all the values to support the undo functionality.
  */ 
  switch(nominator)
  {
  
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */
  
  case I_agentDiffServGenStatusAdminMode:
    CLR_VALID(I_agentDiffServGenStatusAdminMode, 
			  agentDiffServGenStatusGroupData.valid);

    if (snmpDiffServGenAdminModeGet(USMDB_UNIT_CURRENT, 
		                             &agentDiffServGenStatusGroupData.
							agentDiffServGenStatusAdminMode)== L7_SUCCESS)
      SET_VALID(I_agentDiffServGenStatusAdminMode,
				agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
  
  case I_agentDiffServGenStatusClassTableSize:
    CLR_VALID(I_agentDiffServGenStatusClassTableSize, 
			  agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenClassTableSizeGet(USMDB_UNIT_CURRENT, 
		                                  &agentDiffServGenStatusGroupData.
							agentDiffServGenStatusClassTableSize)== L7_SUCCESS)
      SET_VALID(I_agentDiffServGenStatusClassTableSize, 
				agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusClassTableMax:
    CLR_VALID(I_agentDiffServGenStatusClassTableMax, 
	    	  agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenClassTableMaxGet(USMDB_UNIT_CURRENT, 
									     &agentDiffServGenStatusGroupData.
						agentDiffServGenStatusClassTableMax)== L7_SUCCESS)
    SET_VALID(I_agentDiffServGenStatusClassTableMax, 
	          agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusClassRuleTableSize:
    CLR_VALID(I_agentDiffServGenStatusClassRuleTableSize, 
	    	  agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenClassRuleTableSizeGet(USMDB_UNIT_CURRENT, 
									          &agentDiffServGenStatusGroupData.
						agentDiffServGenStatusClassRuleTableSize)== L7_SUCCESS)
    SET_VALID(I_agentDiffServGenStatusClassRuleTableSize, 
	          agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusClassRuleTableMax:
    CLR_VALID(I_agentDiffServGenStatusClassRuleTableMax, 
	    	  agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenClassRuleTableMaxGet(USMDB_UNIT_CURRENT, 
			    				          &agentDiffServGenStatusGroupData.
						agentDiffServGenStatusClassRuleTableMax)== L7_SUCCESS)

    SET_VALID(I_agentDiffServGenStatusClassRuleTableMax, 
	          agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusPolicyTableSize:
    CLR_VALID(I_agentDiffServGenStatusPolicyTableSize, 
	    	  agentDiffServGenStatusGroupData.valid);
    if (usmDbDiffServGenPolicyTableSizeGet(USMDB_UNIT_CURRENT, 
	        							  &agentDiffServGenStatusGroupData.
					       agentDiffServGenStatusPolicyTableSize)== L7_SUCCESS)

    SET_VALID(I_agentDiffServGenStatusPolicyTableSize, 
		      agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusPolicyTableMax:
    CLR_VALID(I_agentDiffServGenStatusPolicyTableMax, 
              agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenPolicyTableMaxGet(USMDB_UNIT_CURRENT, 
           								 &agentDiffServGenStatusGroupData. 
	           		       agentDiffServGenStatusPolicyTableMax)== L7_SUCCESS)
    SET_VALID(I_agentDiffServGenStatusPolicyTableMax, 
		      agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusPolicyInstTableSize:
    CLR_VALID(I_agentDiffServGenStatusPolicyInstTableSize, 
		      agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenPolicyInstTableSizeGet(USMDB_UNIT_CURRENT, 
										  &agentDiffServGenStatusGroupData. 
					agentDiffServGenStatusPolicyInstTableSize)== L7_SUCCESS)
    SET_VALID(I_agentDiffServGenStatusPolicyInstTableSize, 
		      agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusPolicyInstTableMax:
    CLR_VALID(I_agentDiffServGenStatusPolicyInstTableMax, 
		      agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenPolicyInstTableMaxGet(USMDB_UNIT_CURRENT, 
										  &agentDiffServGenStatusGroupData. 
					agentDiffServGenStatusPolicyInstTableMax)== L7_SUCCESS)
    SET_VALID(I_agentDiffServGenStatusPolicyInstTableMax, 
		      agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusPolicyAttrTableSize:
    CLR_VALID(I_agentDiffServGenStatusPolicyAttrTableSize, 
		      agentDiffServGenStatusGroupData.valid);
    if (usmDbDiffServGenPolicyAttrTableSizeGet(USMDB_UNIT_CURRENT, 
										  &agentDiffServGenStatusGroupData. 
					agentDiffServGenStatusPolicyAttrTableSize)== L7_SUCCESS)
    SET_VALID(I_agentDiffServGenStatusPolicyAttrTableSize, 
		      agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusPolicyAttrTableMax:
    CLR_VALID(I_agentDiffServGenStatusPolicyAttrTableMax, 
		      agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenPolicyAttrTableMaxGet(USMDB_UNIT_CURRENT, 
										  &agentDiffServGenStatusGroupData. 
					agentDiffServGenStatusPolicyAttrTableMax)== L7_SUCCESS)
    SET_VALID(I_agentDiffServGenStatusPolicyAttrTableMax, 
		      agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusServiceTableSize:
    CLR_VALID(I_agentDiffServGenStatusServiceTableSize, 
		      agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenServiceTableSizeGet(USMDB_UNIT_CURRENT, 
										  &agentDiffServGenStatusGroupData. 
					agentDiffServGenStatusServiceTableSize)== L7_SUCCESS)
    SET_VALID(I_agentDiffServGenStatusServiceTableSize, 
		      agentDiffServGenStatusGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDiffServGenStatusServiceTableMax:
    CLR_VALID(I_agentDiffServGenStatusServiceTableMax, 
	    	  agentDiffServGenStatusGroupData.valid);

    if (usmDbDiffServGenServiceTableMaxGet(USMDB_UNIT_CURRENT, 
									  &agentDiffServGenStatusGroupData. 
	 			agentDiffServGenStatusServiceTableMax)== L7_SUCCESS)
    SET_VALID(I_agentDiffServGenStatusServiceTableMax, 
	     	  agentDiffServGenStatusGroupData.valid);
    break;
  
  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentDiffServGenStatusGroupData.valid))
    return(NULL);

  return(&agentDiffServGenStatusGroupData);
}

#ifdef SETS
int
k_agentDiffServGenStatusGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServGenStatusGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                    doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServGenStatusGroup_set(agentDiffServGenStatusGroup_t *data,
                                  ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));


  if (VALID(I_agentDiffServGenStatusAdminMode, data->valid)) 
  {
    if (snmpDiffServGenAdminModeSet(USMDB_UNIT_CURRENT, data->agentDiffServGenStatusAdminMode)
                                   != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServGenStatusAdminMode, tempValid);
    }
  }

  return(NO_ERROR);
}

#ifdef SR_agentDiffServGenStatusGroup_UNDO
/* add #define SR_agentDiffServGenStatusGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServGenStatusGroup family.
 */
int
agentDiffServGenStatusGroup_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
  agentDiffServGenStatusGroup_t *data = (agentDiffServGenStatusGroup_t *) doCur->data;
  agentDiffServGenStatusGroup_t *undodata = (agentDiffServGenStatusGroup_t *) doCur->undodata;
  agentDiffServGenStatusGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
   */
  if ( data->valid == NULL  || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_agentDiffServGenStatusGroup_set(setdata, contextInfo, function) == NO_ERROR) 
      return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServGenStatusGroup_UNDO */

#endif /* SETS */

agentDiffServClassGroup_t *
k_agentDiffServClassGroup_get(int serialNum, ContextInfo *contextInfo,
                              int nominator)
{
  static agentDiffServClassGroup_t agentDiffServClassGroupData;

  ZERO_VALID(agentDiffServClassGroupData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  if (usmDbDiffServClassIndexNext(USMDB_UNIT_CURRENT, 
		          &agentDiffServClassGroupData.agentDiffServClassIndexNextFree)== L7_SUCCESS)
    SET_VALID(I_agentDiffServClassIndexNextFree, agentDiffServClassGroupData.valid);
  
  if (nominator >= 0 && !VALID(nominator, agentDiffServClassGroupData.valid))
    return(NULL);

  return(&agentDiffServClassGroupData);
}

agentDiffServClassEntry_t *
k_agentDiffServClassEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 agentDiffServClassIndex)
{
  static agentDiffServClassEntry_t agentDiffServClassEntryData;

  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentDiffServClassEntryData.agentDiffServClassName = MakeOctetString(NULL, 0);
  }
  ZERO_VALID(agentDiffServClassEntryData.valid);
  agentDiffServClassEntryData.agentDiffServClassIndex = agentDiffServClassIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_agentDiffServClassIndex, agentDiffServClassEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpDiffServClassGet(&agentDiffServClassEntryData,
                             nominator) != L7_SUCCESS) :
       ( (snmpDiffServClassGet(&agentDiffServClassEntryData,
                               nominator) != L7_SUCCESS) &&
         (snmpDiffServClassGetNext(&agentDiffServClassEntryData,
                                    nominator) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentDiffServClassEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentDiffServClassEntryData.valid))
    return(NULL);

  return(&agentDiffServClassEntryData); 
}

#ifdef SETS
int
k_agentDiffServClassEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServClassEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServClassEntry_set_defaults(doList_t *dp)
{
    agentDiffServClassEntry_t *data = (agentDiffServClassEntry_t *) (dp->data);

    if ((data->agentDiffServClassName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->agentDiffServClassStorageType = D_agentDiffServClassStorageType_nonVolatile;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDiffServClassEntry_set(agentDiffServClassEntry_t *data,
                              ContextInfo *contextInfo, int function)
{
    L7_BOOL isCreatedNew = L7_FALSE;
    char snmp_buffer[SNMP_BUFFER_LEN];
   
   /* This variable is added to pass to snmpDiffServClassGet() function so that 
    * our set values in 'data' are not lost.
    */
    agentDiffServClassEntry_t *tempData;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

   tempData = Clone_agentDiffServClassEntry(data);
   tempData->agentDiffServClassIndex = data->agentDiffServClassIndex;  /* QSCAN: DELETE -- Temp fixup */

   /* If class-entry doesn't exist, then try to create it */
  if (snmpDiffServClassGet(tempData, -1) != L7_SUCCESS)
  {
    /* creation is only allowed when setting row status to createAndGo or createAndWait */
    if (VALID(I_agentDiffServClassRowStatus, data->valid) && 
        (data->agentDiffServClassRowStatus == D_agentDiffServClassRowStatus_createAndGo ||
         data->agentDiffServClassRowStatus == D_agentDiffServClassRowStatus_createAndWait)  &&
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE))
    {
      if (usmDbDiffServClassCreate(USMDB_UNIT_CURRENT, 
                                   data->agentDiffServClassIndex, 
                                   ((data->agentDiffServClassRowStatus == D_agentDiffServClassRowStatus_createAndGo) ? L7_TRUE : L7_FALSE)) != L7_SUCCESS)
      {
        creationErrorClassEntry = L7_TRUE;
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
      }
      else
      {
        isCreatedNew = L7_TRUE;
        SET_VALID(I_agentDiffServClassRowStatus, tempValid);
      }

    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(NO_CREATION_ERROR);
    }

  }

  if (VALID(I_agentDiffServClassName, data->valid) && data->agentDiffServClassName != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDiffServClassName->octet_ptr, data->agentDiffServClassName->length);
    if ((usmDbStringAlphaNumericCheck(snmp_buffer) != L7_SUCCESS) ||
        (usmDbDiffServClassNameSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                   snmp_buffer) != L7_SUCCESS))
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
        SET_VALID(I_agentDiffServClassName, tempValid);
    }
  }

  if (VALID(I_agentDiffServClassType, data->valid) )
  {
    if  (snmpDiffServClassTypeSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                data->agentDiffServClassType) != L7_SUCCESS)
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassType, tempValid);
    }
  }

  if (VALID(I_agentDiffServClassAclNum, data->valid))
  { 
    if  (usmDbDiffServClassAclNumSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                data->agentDiffServClassAclNum) != L7_SUCCESS)
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassAclNum, tempValid);
    }
  }

  if (VALID(I_agentDiffServClassStorageType, data->valid))
  {
    if  (snmpDiffServClassStorageTypeSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                data->agentDiffServClassStorageType) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassStorageType, tempValid);
    }
  }
 
  if (VALID(I_agentDiffServClassAclType, data->valid) )
  {
    if  (snmpDiffServClassAclTypeSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                     data->agentDiffServClassAclType) != L7_SUCCESS)
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassAclType, tempValid);
    }
  }

  if (VALID(I_agentDiffServClassProtoType, data->valid) )
  {
    if(snmpDiffServClassProtoTypeSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                     data->agentDiffServClassProtoType) != L7_SUCCESS)
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassProtoType, tempValid);
    }
  }

  if( data->agentDiffServClassRowStatus == D_agentDiffServClassRowStatus_destroy)
  {
    L7_uint32 classIndex, classRuleIndex,nextClassIndex;

    agentDiffServClassRuleEntry_count=0;
    classIndex = data->agentDiffServClassIndex;
    classRuleIndex = 0;
    while ((diffServClassRuleGetNext(classIndex, classRuleIndex, &nextClassIndex,
                                     &classRuleIndex) == L7_SUCCESS) &&
           (nextClassIndex == classIndex))
    {
      if (classRuleIndex == 0)
        break;
      else
      {
        agentDiffServClassRuleEntry_count++;
      }/* else end */
    } /* endwhile */

    if( agentDiffServClassRuleEntry_count > 0 )
    {
      return COMMIT_FAILED_ERROR;
    }
  }/* end of block for destroy */

  if (VALID(I_agentDiffServClassRowStatus, data->valid) &&
        (isCreatedNew != L7_TRUE)  &&
      (snmpDiffServClassRowStatusSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                data->agentDiffServClassRowStatus) != L7_SUCCESS))
  {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}

#ifdef SR_agentDiffServClassEntry_UNDO
/* add #define SR_agentDiffServClassEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServClassEntry family.
 */
int
agentDiffServClassEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
  agentDiffServClassEntry_t *data = (agentDiffServClassEntry_t *) doCur->data;
  agentDiffServClassEntry_t *undodata = (agentDiffServClassEntry_t *) doCur->undodata;
  agentDiffServClassEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;
  
  if( creationErrorClassEntry == L7_TRUE )
  {
     creationErrorClassEntry = L7_FALSE;
     return NO_ERROR;
  }


  if( agentDiffServClassRuleEntry_count > 0 )
  {
    agentDiffServClassRuleEntry_count = 0;
    return NO_ERROR;
  }

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL  && (data->agentDiffServClassRowStatus != D_agentDiffServClassRowStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
      /* undoing an add, so delete */
      data->agentDiffServClassRowStatus = D_agentDiffServClassRowStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  } 
  else 
  {
    /* undoing a delete or modify, replace the original data */
    if((undodata->agentDiffServClassRowStatus == D_agentDiffServClassRowStatus_notReady)
       || (undodata->agentDiffServClassRowStatus == D_agentDiffServClassRowStatus_active)) 
    {

       undodata->agentDiffServClassRowStatus = D_agentDiffServClassRowStatus_createAndGo;
    } 
    else 
    {
       if(undodata->agentDiffServClassRowStatus == D_agentDiffServClassRowStatus_notInService) 
         undodata->agentDiffServClassRowStatus = D_agentDiffServClassRowStatus_createAndWait;
    }

    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentDiffServClassEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServClassEntry_UNDO */

#endif /* SETS */

agentDiffServClassRuleEntry_t *
k_agentDiffServClassRuleEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_UINT32 agentDiffServClassIndex,
                                  SR_UINT32 agentDiffServClassRuleIndex)
{
    static agentDiffServClassRuleEntry_t agentDiffServClassRuleEntryData;

    static L7_BOOL firstTime = L7_TRUE;

    if (firstTime == L7_TRUE)
    {
      firstTime = L7_FALSE;
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchDstMacAddr = MakeOctetString(NULL, 0);
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchDstMacMask = MakeOctetString(NULL, 0);
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchIpTosBits = MakeOctetString(NULL, 0);
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchIpTosMask = MakeOctetString(NULL, 0);
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchSrcMacAddr = MakeOctetString(NULL, 0);
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchSrcMacMask = MakeOctetString(NULL, 0);
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchSrcIpv6Prefix = MakeOctetString(NULL, 0);
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchDstIpv6Prefix = MakeOctetString(NULL, 0);
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchSrcIpv6PrefixLength = 0;
      agentDiffServClassRuleEntryData.agentDiffServClassRuleMatchDstIpv6PrefixLength = 0;
    }
    
    ZERO_VALID(agentDiffServClassRuleEntryData.valid);
    agentDiffServClassRuleEntryData.agentDiffServClassIndex = agentDiffServClassIndex;
    agentDiffServClassRuleEntryData.agentDiffServClassRuleIndex = agentDiffServClassRuleIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }


	SET_VALID(I_agentDiffServClassIndex, agentDiffServClassRuleEntryData.valid);
	SET_VALID(I_agentDiffServClassRuleIndex, agentDiffServClassRuleEntryData.valid);

	if ( (searchType == EXACT) ?
	   (snmpDiffServClassRuleGet(&agentDiffServClassRuleEntryData,
                                 nominator) != L7_SUCCESS) :
	   ( (snmpDiffServClassRuleGet(&agentDiffServClassRuleEntryData,
                                   nominator) != L7_SUCCESS) &&
		 (snmpDiffServClassRuleGetNext(&agentDiffServClassRuleEntryData,
                                       nominator) != L7_SUCCESS) ) )
	{
	  ZERO_VALID(agentDiffServClassRuleEntryData.valid);
	  return(NULL);
	}

	if (nominator >= 0 && !VALID(nominator, agentDiffServClassRuleEntryData.valid))
	  return(NULL);

	return(&agentDiffServClassRuleEntryData);

}

#ifdef SETS
int
k_agentDiffServClassRuleEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServClassRuleEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                    doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServClassRuleEntry_set_defaults(doList_t *dp)
{
    agentDiffServClassRuleEntry_t *data = (agentDiffServClassRuleEntry_t *) (dp->data);

    if ((data->agentDiffServClassRuleMatchDstMacAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentDiffServClassRuleMatchDstMacMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentDiffServClassRuleMatchIpTosBits = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentDiffServClassRuleMatchIpTosMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentDiffServClassRuleMatchSrcMacAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentDiffServClassRuleMatchSrcMacMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentDiffServClassRuleMatchSrcIpv6Prefix = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentDiffServClassRuleMatchDstIpv6Prefix = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->agentDiffServClassRuleMatchSrcIpv6PrefixLength = 0;
    data->agentDiffServClassRuleMatchDstIpv6PrefixLength = 0;
    data->agentDiffServClassRuleStorageType = D_agentDiffServClassRuleStorageType_nonVolatile;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDiffServClassRuleEntry_set(agentDiffServClassRuleEntry_t *data,
                                  ContextInfo *contextInfo, int function)
{
  L7_BOOL            isCreatedNew = L7_FALSE;
  char               snmp_buffer[SNMP_BUFFER_LEN];
  L7_in6_addr_t      prefix;  

  /* This variable is added to pass to snmpDiffServClassGet() function so that 
   * our set values in 'data' are not lost.
   */
    agentDiffServClassRuleEntry_t *tempData;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_agentDiffServClassRuleEntry(data);
  tempData->agentDiffServClassRuleIndex = data->agentDiffServClassRuleIndex;  /* QSCAN: DELETE -- Temp fixup */
  tempData->agentDiffServClassIndex = data->agentDiffServClassIndex;          /* QSCAN: DELETE -- Temp fixup */

  /* If classRule-entry doesn't exist, then try to create it */
  if (snmpDiffServClassRuleGet(tempData, -1) != L7_SUCCESS)
  {
    /* creation is only allowed when setting row status to createAndGo or createAndWait */
    if (VALID(I_agentDiffServClassRuleRowStatus, data->valid) && 
        (data->agentDiffServClassRuleRowStatus == D_agentDiffServClassRuleRowStatus_createAndGo ||
         data->agentDiffServClassRuleRowStatus == D_agentDiffServClassRuleRowStatus_createAndWait) &&
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE))
    {
      if (usmDbDiffServClassRuleCreate(USMDB_UNIT_CURRENT, 
                                       data->agentDiffServClassIndex,
									   data->agentDiffServClassRuleIndex, 
                                       ((data->agentDiffServClassRuleRowStatus == D_agentDiffServClassRuleRowStatus_createAndGo) ? L7_TRUE : L7_FALSE)) != L7_SUCCESS)
      {
        creationErrorClassRuleEntry = L7_TRUE;
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
      }
      else
      {
        isCreatedNew = L7_TRUE;
        SET_VALID(I_agentDiffServClassRuleRowStatus, tempValid);
      }

    }
    else
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
      return(NO_CREATION_ERROR);
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchEntryType, data->valid))
  {
    if (snmpDiffServClassRuleMatchEntryTypeSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                data->agentDiffServClassRuleIndex,
                                data->agentDiffServClassRuleMatchEntryType) != L7_SUCCESS)
    {
         memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
        SET_VALID(I_agentDiffServClassRuleMatchEntryType, tempValid);
    }
  }
 
  /* ExcludeFlag must be set after MatchEntryType, and before any other match object */
  if (VALID(I_agentDiffServClassRuleMatchExcludeFlag, data->valid))
  {
    if  (snmpDiffServClassRuleMatchExcludeFlagSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                 data->agentDiffServClassRuleIndex,
                                                 data->agentDiffServClassRuleMatchExcludeFlag) != L7_SUCCESS)
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassRuleMatchExcludeFlag, tempValid);
    }
  }	

  if (VALID(I_agentDiffServClassRuleMatchCos, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    {
       if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                    L7_DIFFSERV_CLASS_MATCH_COS_FEATURE_ID) != L7_TRUE) ||
          (usmDbDiffServClassRuleMatchCosSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                             data->agentDiffServClassRuleIndex,
                                             data->agentDiffServClassRuleMatchCos) != L7_SUCCESS))
       {
          memcpy(data->valid, tempValid, sizeof(data->valid));
          return(COMMIT_FAILED_ERROR);
       }
       else
       {
          SET_VALID(I_agentDiffServClassRuleMatchCos, tempValid);
       }
    }
    else
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
  }     
  
  if (VALID(I_agentDiffServClassRuleMatchDstIpAddr, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) != L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_MATCH_DSTIP_FEATURE_ID) != L7_TRUE) ||
       (usmDbDiffServClassRuleMatchDstIpAddrSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                data->agentDiffServClassRuleIndex,
                                                data->agentDiffServClassRuleMatchDstIpAddr) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassRuleMatchDstIpAddr, tempValid);
    }
  }     

  if (VALID(I_agentDiffServClassRuleMatchDstIpMask, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) != L7_TRUE) ||
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_MATCH_DSTIP_FEATURE_ID) != L7_TRUE) ||
         (usmDbDiffServClassRuleMatchDstIpMaskSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                data->agentDiffServClassRuleIndex,
                                                data->agentDiffServClassRuleMatchDstIpMask) != L7_SUCCESS))
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassRuleMatchDstIpMask, tempValid);
    }
  }

  /* if range is not supported, set the end equal to the start */
   if(VALID(I_agentDiffServClassRuleMatchDstL4PortStart, data->valid))
   {
      if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
      {
         if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                      L7_DIFFSERV_CLASS_MATCH_DSTL4PORT_FEATURE_ID) != L7_TRUE) ||
            (usmDbDiffServClassRuleMatchDstL4PortStartSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                          data->agentDiffServClassRuleIndex,
                                                          data->agentDiffServClassRuleMatchDstL4PortStart) != L7_SUCCESS) ||
            ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE) && 
             (usmDbDiffServClassRuleMatchDstL4PortEndSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                         data->agentDiffServClassRuleIndex,
                                                         data->agentDiffServClassRuleMatchDstL4PortStart) != L7_SUCCESS)))
         {
            memcpy(data->valid, tempValid, sizeof(data->valid));
            return(COMMIT_FAILED_ERROR);
         }
         else
         {
            SET_VALID(I_agentDiffServClassRuleMatchDstL4PortStart, tempValid);
         }
      }
      else
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
      }
   }
 
  /* if range is not supported, set the start equal to the end */
  if (VALID(I_agentDiffServClassRuleMatchDstL4PortEnd, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    { 
       if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                    L7_DIFFSERV_CLASS_MATCH_DSTL4PORT_FEATURE_ID) != L7_TRUE) ||
           (usmDbDiffServClassRuleMatchDstL4PortEndSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                        data->agentDiffServClassRuleIndex,
                                                        data->agentDiffServClassRuleMatchDstL4PortEnd) != L7_SUCCESS) ||
           ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE) && 
           (usmDbDiffServClassRuleMatchDstL4PortStartSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                         data->agentDiffServClassRuleIndex,
                                                         data->agentDiffServClassRuleMatchDstL4PortEnd) != L7_SUCCESS)))
       {
           memcpy(data->valid, tempValid, sizeof(data->valid));
           return(COMMIT_FAILED_ERROR);
       }
       else
       {
          SET_VALID(I_agentDiffServClassRuleMatchDstL4PortEnd, tempValid);
       }
    }
    else
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
  }

  if (data->agentDiffServClassRuleMatchDstMacAddr != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDiffServClassRuleMatchDstMacAddr->octet_ptr, 
		   data->agentDiffServClassRuleMatchDstMacAddr->length);
    if (VALID(I_agentDiffServClassRuleMatchDstMacAddr, data->valid))
    {
      if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
      {
         if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                       L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID) != L7_TRUE) ||
             (usmDbDiffServClassRuleMatchDstMacAddrSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                       data->agentDiffServClassRuleIndex,
                                                       snmp_buffer) != L7_SUCCESS))
         {
            memcpy(data->valid, tempValid, sizeof(data->valid));
            return(COMMIT_FAILED_ERROR);
         }
         else
         {
            SET_VALID(I_agentDiffServClassRuleMatchDstMacAddr, tempValid);
         }
      }
      else
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
      }
    }  
  }

  if (data->agentDiffServClassRuleMatchDstMacMask != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDiffServClassRuleMatchDstMacMask->octet_ptr, 
		   data->agentDiffServClassRuleMatchDstMacMask->length);
    if (VALID(I_agentDiffServClassRuleMatchDstMacMask, data->valid))
    {
      if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
      { 
         if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                      L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID) != L7_TRUE) ||
             (usmDbDiffServClassRuleMatchDstMacMaskSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                      data->agentDiffServClassRuleIndex,
                                                      snmp_buffer) != L7_SUCCESS))
         {
            memcpy(data->valid, tempValid, sizeof(data->valid));
            return(COMMIT_FAILED_ERROR);
         }
         else
         {
            SET_VALID(I_agentDiffServClassRuleMatchDstMacMask, tempValid);
         }
      }
      else
      {
          memcpy(data->valid, tempValid, sizeof(data->valid));
          return(COMMIT_FAILED_ERROR);
      }
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchIpDscp, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE) ) 
    {
       if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                      L7_DIFFSERV_CLASS_MATCH_IPDSCP_FEATURE_ID) != L7_TRUE) ||
            (usmDbDiffServClassRuleMatchIpDscpSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                  data->agentDiffServClassRuleIndex,
                                                  data->agentDiffServClassRuleMatchIpDscp) != L7_SUCCESS))
       {
           memcpy(data->valid, tempValid, sizeof(data->valid));
           return(COMMIT_FAILED_ERROR);
       }
       else
       {
          SET_VALID(I_agentDiffServClassRuleMatchIpDscp, tempValid);
       }
    }
    else
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
  }	

  if (VALID(I_agentDiffServClassRuleMatchIpPrecedence, data->valid))
  {
    /* Check whether the class type is Ipv4 or not */ 
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) != L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_MATCH_IPPRECEDENCE_FEATURE_ID) != L7_TRUE) ||
       (usmDbDiffServClassRuleMatchIpPrecedenceSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                   data->agentDiffServClassRuleIndex,
                                                   data->agentDiffServClassRuleMatchIpPrecedence) != L7_SUCCESS))
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassRuleMatchIpPrecedence, tempValid);
    }
  }

  if (data->agentDiffServClassRuleMatchIpTosBits != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDiffServClassRuleMatchIpTosBits->octet_ptr, 
		   data->agentDiffServClassRuleMatchIpTosBits->length);
    if (VALID(I_agentDiffServClassRuleMatchIpTosBits, data->valid))
    {

      if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) != L7_TRUE) ||
           (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_MATCH_IPTOS_FEATURE_ID) != L7_TRUE) ||
          (snmpDiffServClassRuleMatchIpTosBitsSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                 data->agentDiffServClassRuleIndex,
                                                 snmp_buffer, 
                                                 data->agentDiffServClassRuleMatchIpTosBits->length) != L7_SUCCESS))
      
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
      }
      else
      {
         SET_VALID(I_agentDiffServClassRuleMatchIpTosBits, tempValid);
      }
    }
  } 

  if (data->agentDiffServClassRuleMatchIpTosMask != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDiffServClassRuleMatchIpTosMask->octet_ptr, 
		   data->agentDiffServClassRuleMatchIpTosMask->length);
    if (VALID(I_agentDiffServClassRuleMatchIpTosMask, data->valid))
    {
      if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) != L7_TRUE) || 
          (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_MATCH_IPTOS_FEATURE_ID) != L7_TRUE) ||
         (snmpDiffServClassRuleMatchIpTosMaskSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                data->agentDiffServClassRuleIndex,
                                                snmp_buffer,
                                                data->agentDiffServClassRuleMatchIpTosMask->length) != L7_SUCCESS))
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentDiffServClassRuleMatchIpTosMask, tempValid);
      }
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchProtocolNum, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
          (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    {
       if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                    L7_DIFFSERV_CLASS_MATCH_PROTOCOL_FEATURE_ID) != L7_TRUE) ||
           (usmDbDiffServClassRuleMatchProtocolNumSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                     data->agentDiffServClassRuleIndex,
                                                     data->agentDiffServClassRuleMatchProtocolNum) != L7_SUCCESS))
       {
           memcpy(data->valid, tempValid, sizeof(data->valid));
           return(COMMIT_FAILED_ERROR);
       }
       else
       {
          SET_VALID(I_agentDiffServClassRuleMatchProtocolNum, tempValid);
       }
    }
    else
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchRefClassIndex, data->valid))
  {
     if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    {
       if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                    L7_DIFFSERV_CLASS_MATCH_REFCLASS_FEATURE_ID) != L7_TRUE) ||
           (usmDbDiffServClassRuleMatchRefClassIndexSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                       data->agentDiffServClassRuleIndex,
                                                       data->agentDiffServClassRuleMatchRefClassIndex) != L7_SUCCESS))
       {
           memcpy(data->valid, tempValid, sizeof(data->valid));
           return(COMMIT_FAILED_ERROR);
       }
       else
       {
          SET_VALID(I_agentDiffServClassRuleMatchRefClassIndex, tempValid);
       }
     }
     else
     {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
     }
  }

  if (VALID(I_agentDiffServClassRuleMatchSrcIpAddr, data->valid))
  {
    /* Check for class type */ 
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) != L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_MATCH_SRCIP_FEATURE_ID) != L7_TRUE) ||
       (usmDbDiffServClassRuleMatchSrcIpAddrSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                data->agentDiffServClassRuleIndex,
                                                data->agentDiffServClassRuleMatchSrcIpAddr) != L7_SUCCESS))
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassRuleMatchSrcIpAddr, tempValid);
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchSrcIpMask, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) != L7_TRUE)  ||
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,  
                                 L7_DIFFSERV_CLASS_MATCH_SRCIP_FEATURE_ID) != L7_TRUE) ||
        (usmDbDiffServClassRuleMatchSrcIpMaskSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                data->agentDiffServClassRuleIndex,
                                                data->agentDiffServClassRuleMatchSrcIpMask) != L7_SUCCESS))
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassRuleMatchSrcIpMask, tempValid);
    }
  }

  /* if range is not supported, set the end equal to the start */
  if (VALID(I_agentDiffServClassRuleMatchSrcL4PortStart, data->valid))
  {
     if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
     {
        if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_MATCH_SRCL4PORT_FEATURE_ID) != L7_TRUE) ||
        (usmDbDiffServClassRuleMatchSrcL4PortStartSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                     data->agentDiffServClassRuleIndex,
                                                     data->agentDiffServClassRuleMatchSrcL4PortStart) != L7_SUCCESS) ||
        ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE) && 
        (usmDbDiffServClassRuleMatchSrcL4PortEndSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                    data->agentDiffServClassRuleIndex,
                                                    data->agentDiffServClassRuleMatchSrcL4PortStart) != L7_SUCCESS)))
       {
           memcpy(data->valid, tempValid, sizeof(data->valid));
           return(COMMIT_FAILED_ERROR);
       }
       else
       {
          SET_VALID(I_agentDiffServClassRuleMatchSrcL4PortStart, tempValid);
       }
     }
     else
     {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
     }
  }
  /* if range is not supported, set the start equal to the start */
  if (VALID(I_agentDiffServClassRuleMatchSrcL4PortEnd, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    {

       if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                      L7_DIFFSERV_CLASS_MATCH_SRCL4PORT_FEATURE_ID) != L7_TRUE) ||
           (usmDbDiffServClassRuleMatchSrcL4PortEndSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                      data->agentDiffServClassRuleIndex,
                                                      data->agentDiffServClassRuleMatchSrcL4PortEnd) != L7_SUCCESS) ||
           ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE) &&
           (usmDbDiffServClassRuleMatchSrcL4PortStartSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                         data->agentDiffServClassRuleIndex,
                                                         data->agentDiffServClassRuleMatchSrcL4PortStart) != L7_SUCCESS)))
       {
           memcpy(data->valid, tempValid, sizeof(data->valid));
           return(COMMIT_FAILED_ERROR);
       }
       else
       {
          SET_VALID(I_agentDiffServClassRuleMatchSrcL4PortEnd, tempValid);
       }
    }
    else
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
  }

  if (data->agentDiffServClassRuleMatchSrcMacAddr != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDiffServClassRuleMatchSrcMacAddr->octet_ptr, 
           data->agentDiffServClassRuleMatchSrcMacAddr->length);
    if (VALID(I_agentDiffServClassRuleMatchSrcMacAddr, data->valid))
    {
      if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
      {
         if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                      L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID) != L7_TRUE) ||
            (usmDbDiffServClassRuleMatchSrcMacAddrSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                      data->agentDiffServClassRuleIndex,
                                                      snmp_buffer) != L7_SUCCESS))
         {
            memcpy(data->valid, tempValid, sizeof(data->valid));
            return(COMMIT_FAILED_ERROR);
         }
         else
         {
           SET_VALID(I_agentDiffServClassRuleMatchSrcMacAddr, tempValid);
         }
      }
      else
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
      }
    }
  }

  if (data->agentDiffServClassRuleMatchSrcMacMask != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDiffServClassRuleMatchSrcMacMask->octet_ptr, 
		   data->agentDiffServClassRuleMatchSrcMacMask->length);
    if (VALID(I_agentDiffServClassRuleMatchSrcMacMask, data->valid))
    {
      if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
      {
         if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                      L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID) != L7_TRUE) ||
            (usmDbDiffServClassRuleMatchSrcMacMaskSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                      data->agentDiffServClassRuleIndex,
                                                      snmp_buffer) != L7_SUCCESS))
         {
            memcpy(data->valid, tempValid, sizeof(data->valid));
            return(COMMIT_FAILED_ERROR);
         }
         else
         {
            SET_VALID(I_agentDiffServClassRuleMatchSrcMacMask, tempValid);
         }
      }
      else
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
      }
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchVlanId, data->valid))
  {
#ifdef OBSOLETE
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    {
       if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                    L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID) != L7_TRUE) ||
          (usmDbDiffServClassRuleMatchVlanIdSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                data->agentDiffServClassRuleIndex,
                                                data->agentDiffServClassRuleMatchVlanId) != L7_SUCCESS))
       {
          memcpy(data->valid, tempValid, sizeof(data->valid)); 
          return(COMMIT_FAILED_ERROR);
       }
       else
       {
          SET_VALID(I_agentDiffServClassRuleMatchVlanId, tempValid);
       }
    }
    else
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
#else
     memcpy(data->valid, tempValid, sizeof(data->valid));
     return(COMMIT_FAILED_ERROR);
#endif
  }

  if (VALID(I_agentDiffServClassRuleStorageType, data->valid))
  {
    if (snmpDiffServClassRuleStorageTypeSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
							  data->agentDiffServClassRuleIndex,
							  data->agentDiffServClassRuleStorageType) != L7_SUCCESS)
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServClassRuleStorageType, tempValid);
    }
  }
	

  if (VALID(I_agentDiffServClassRuleRowStatus, data->valid) &&
        (isCreatedNew != L7_TRUE) &&
     (snmpDiffServClassRuleRowStatusSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
							  data->agentDiffServClassRuleIndex,
							  data->agentDiffServClassRuleRowStatus) != L7_SUCCESS))
  {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
  }

  if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                               L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
     (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                               L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
  {
     if (VALID(I_agentDiffServClassRuleMatchCos2, data->valid) &&
         usmDbDiffServClassRuleMatchCos2Set(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                            data->agentDiffServClassRuleIndex,
                                            data->agentDiffServClassRuleMatchCos2) != L7_SUCCESS)
     {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
     }
  }
  else
  {
     memcpy(data->valid, tempValid, sizeof(data->valid));
     return(COMMIT_FAILED_ERROR);
  }
  

  if (VALID(I_agentDiffServClassRuleMatchEtypeKey, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) != L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID) != L7_TRUE) ||
        snmpAgentDiffServClassRuleMatchEtypeKeySet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                   data->agentDiffServClassRuleIndex,
                                                   data->agentDiffServClassRuleMatchEtypeKey) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchEtypeValue, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) != L7_TRUE) ||
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID) != L7_TRUE) ||
        usmDbDiffServClassRuleMatchEtypeValueSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                 data->agentDiffServClassRuleIndex,
                                                 data->agentDiffServClassRuleMatchEtypeValue) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchVlanIdStart, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    {
       if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                     L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID) != L7_TRUE) ||
           (usmDbDiffServClassRuleMatchVlanIdStartSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                      data->agentDiffServClassRuleIndex,
                                                      data->agentDiffServClassRuleMatchVlanIdStart) != L7_SUCCESS) ||
           ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE) && 
            (usmDbDiffServClassRuleMatchVlanIdEndSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                     data->agentDiffServClassRuleIndex,
                                                     data->agentDiffServClassRuleMatchVlanIdStart) != L7_SUCCESS)))
       {
          memcpy(data->valid, tempValid, sizeof(data->valid));
          return(COMMIT_FAILED_ERROR);
       }
    }
    else
    {   
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchVlanIdEnd, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    {
       if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                     L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID) != L7_TRUE) ||
          (usmDbDiffServClassRuleMatchVlanIdEndSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                   data->agentDiffServClassRuleIndex,
                                                   data->agentDiffServClassRuleMatchVlanIdEnd) != L7_SUCCESS) ||
          ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE) && 
          (usmDbDiffServClassRuleMatchVlanIdStartSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                     data->agentDiffServClassRuleIndex,
                                                     data->agentDiffServClassRuleMatchVlanIdEnd) != L7_SUCCESS)))
       {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
       }
    }
    else
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchVlanId2Start, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    { 
       if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                     L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID) != L7_TRUE) ||
          (usmDbDiffServClassRuleMatchVlanId2StartSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                      data->agentDiffServClassRuleIndex,
                                                      data->agentDiffServClassRuleMatchVlanId2Start) != L7_SUCCESS) ||
          ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE) && 
          (usmDbDiffServClassRuleMatchVlanId2EndSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                    data->agentDiffServClassRuleIndex,
                                                    data->agentDiffServClassRuleMatchVlanId2Start) != L7_SUCCESS)))
       {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
       }
    }
    else
    { 
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_agentDiffServClassRuleMatchVlanId2End, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE))
    { 
       if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                     L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID) != L7_TRUE) ||
          (usmDbDiffServClassRuleMatchVlanId2EndSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                    data->agentDiffServClassRuleIndex,
                                                    data->agentDiffServClassRuleMatchVlanId2End) != L7_SUCCESS) ||
          ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE) && 
          (usmDbDiffServClassRuleMatchVlanId2StartSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                      data->agentDiffServClassRuleIndex,
                                                      data->agentDiffServClassRuleMatchVlanId2End) != L7_SUCCESS)))
       {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
       }
    }
    else
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
  }

  if(VALID(I_agentDiffServClassRuleMatchFlowLabel, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) != L7_TRUE) ||
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_FLOWLBL_FEATURE_ID) != L7_TRUE) ||
       (usmDbDiffServClassRuleMatchIp6FlowLabelSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                   data->agentDiffServClassRuleIndex,
                                                   data->agentDiffServClassRuleMatchFlowLabel) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  /* IPv6 source and Destination Prefix */
  if((VALID(I_agentDiffServClassRuleMatchSrcIpv6Prefix, data->valid)) && 
     (VALID(I_agentDiffServClassRuleMatchSrcIpv6PrefixLength, data->valid)))
  {
     memcpy(snmp_buffer, data->agentDiffServClassRuleMatchSrcIpv6Prefix->octet_ptr, data->agentDiffServClassRuleMatchSrcIpv6Prefix->length);
     if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) != L7_TRUE) ||
        (osapiInetPton(L7_AF_INET6, snmp_buffer, (L7_uchar8 *) prefix.in6.addr8) != L7_SUCCESS) ||
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_MATCH_IPV6_SRCIP_FEATURE_ID) != L7_TRUE) ||
        (usmDbDiffServClassRuleMatchSrcIpv6AddrSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                   data->agentDiffServClassRuleIndex,
                                                   &prefix) != L7_SUCCESS) || 
        (usmDbDiffServClassRuleMatchSrcIpv6PrefLenSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                   data->agentDiffServClassRuleIndex,
                                                   data->agentDiffServClassRuleMatchSrcIpv6PrefixLength) != L7_SUCCESS))
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
     }
     else
     {
       SET_VALID(I_agentDiffServClassRuleMatchSrcIpv6Prefix, tempValid);
       SET_VALID(I_agentDiffServClassRuleMatchSrcIpv6PrefixLength, tempValid);
     }
  }

  /* IPv6 source and Destination Prefix */
  if((VALID(I_agentDiffServClassRuleMatchDstIpv6Prefix, data->valid)) &&
     (VALID(I_agentDiffServClassRuleMatchDstIpv6PrefixLength, data->valid)))
  {
     memcpy(snmp_buffer, data->agentDiffServClassRuleMatchDstIpv6Prefix->octet_ptr, data->agentDiffServClassRuleMatchDstIpv6Prefix->length);
     if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) != L7_TRUE) ||
        (osapiInetPton(L7_AF_INET6, snmp_buffer, (L7_uchar8 *) prefix.in6.addr8) != L7_SUCCESS) ||
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_MATCH_IPV6_DSTIP_FEATURE_ID) != L7_TRUE) ||
        (usmDbDiffServClassRuleMatchDstIpv6AddrSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                   data->agentDiffServClassRuleIndex,
                                                   &prefix) != L7_SUCCESS) ||
        (usmDbDiffServClassRuleMatchDstIpv6PrefLenSet(USMDB_UNIT_CURRENT, data->agentDiffServClassIndex,
                                                   data->agentDiffServClassRuleIndex,
                                                   data->agentDiffServClassRuleMatchDstIpv6PrefixLength) != L7_SUCCESS))
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
     }
     else
     {
       SET_VALID(I_agentDiffServClassRuleMatchDstIpv6Prefix, tempValid);
       SET_VALID(I_agentDiffServClassRuleMatchDstIpv6PrefixLength, tempValid);
     }
  }

  
  return(NO_ERROR);

}

#ifdef SR_agentDiffServClassRuleEntry_UNDO
/* add #define SR_agentDiffServClassRuleEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServClassRuleEntry family.
 */
int
agentDiffServClassRuleEntry_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
  agentDiffServClassRuleEntry_t *data = (agentDiffServClassRuleEntry_t *) doCur->data;
  agentDiffServClassRuleEntry_t *undodata = (agentDiffServClassRuleEntry_t *) doCur->undodata;
  agentDiffServClassRuleEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  if( creationErrorClassRuleEntry == L7_TRUE )
  {
    creationErrorClassRuleEntry = L7_FALSE;
    return NO_ERROR;
  }

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && (data->agentDiffServClassRuleRowStatus != D_agentDiffServClassRuleRowStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL ) 
  {
      /* undoing an add, so delete */
      data->agentDiffServClassRuleRowStatus = D_agentDiffServClassRuleRowStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  } 
  else 
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->agentDiffServClassRuleRowStatus == D_agentDiffServClassRuleRowStatus_notReady)
         || (undodata->agentDiffServClassRuleRowStatus == D_agentDiffServClassRuleRowStatus_active)) 
      {
          undodata->agentDiffServClassRuleRowStatus = D_agentDiffServClassRuleRowStatus_createAndGo;
      } 
      else 
      {
         if(undodata->agentDiffServClassRuleRowStatus == D_agentDiffServClassRuleRowStatus_notInService) 
         {
           undodata->agentDiffServClassRuleRowStatus = D_agentDiffServClassRuleRowStatus_createAndWait;
         }
      }

      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentDiffServClassRuleEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServClassRuleEntry_UNDO */

#endif /* SETS */

agentDiffServPolicyGroup_t *
k_agentDiffServPolicyGroup_get(int serialNum, ContextInfo *contextInfo,
                               int nominator)
{
  static agentDiffServPolicyGroup_t agentDiffServPolicyGroupData;

  ZERO_VALID(agentDiffServPolicyGroupData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  if (usmDbDiffServPolicyIndexNext(USMDB_UNIT_CURRENT, 
		          &agentDiffServPolicyGroupData.agentDiffServPolicyIndexNextFree) == L7_SUCCESS)
    SET_VALID(I_agentDiffServPolicyIndexNextFree, agentDiffServPolicyGroupData.valid);
  
  if (nominator >= 0 && !VALID(nominator, agentDiffServPolicyGroupData.valid))
    return(NULL);

  return(&agentDiffServPolicyGroupData);
}

agentDiffServPolicyEntry_t *
k_agentDiffServPolicyEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_UINT32 agentDiffServPolicyIndex)
{

  static agentDiffServPolicyEntry_t agentDiffServPolicyEntryData;

  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentDiffServPolicyEntryData.agentDiffServPolicyName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentDiffServPolicyEntryData.valid);
  agentDiffServPolicyEntryData.agentDiffServPolicyIndex = agentDiffServPolicyIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_agentDiffServPolicyIndex, agentDiffServPolicyEntryData.valid);

	if ( (searchType == EXACT) ?
	   (snmpDiffServPolicyGet(&agentDiffServPolicyEntryData,
                                 nominator) != L7_SUCCESS) :
	   ( (snmpDiffServPolicyGet(&agentDiffServPolicyEntryData,
                                   nominator) != L7_SUCCESS) &&
		   (snmpDiffServPolicyGetNext(&agentDiffServPolicyEntryData,
                                     nominator) != L7_SUCCESS) ) )
	{
	  ZERO_VALID(agentDiffServPolicyEntryData.valid);
	  return(NULL);
	}

	if (nominator >= 0 && !VALID(nominator, agentDiffServPolicyEntryData.valid))
	  return(NULL);

  return(&agentDiffServPolicyEntryData); 
}

#ifdef SETS
int
k_agentDiffServPolicyEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServPolicyEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServPolicyEntry_set_defaults(doList_t *dp)
{
    agentDiffServPolicyEntry_t *data = (agentDiffServPolicyEntry_t *) (dp->data);

    if ((data->agentDiffServPolicyName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->agentDiffServPolicyStorageType = D_agentDiffServPolicyStorageType_nonVolatile;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
	k_agentDiffServPolicyEntry_set(agentDiffServPolicyEntry_t *data,
                               ContextInfo *contextInfo, int function)
{
  L7_BOOL isCreatedNew = L7_FALSE;
  char snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

   /* If policy-entry doesn't exist, then try to create it */
  if (usmDbDiffServPolicyGet(USMDB_UNIT_CURRENT, data->agentDiffServPolicyIndex) != L7_SUCCESS)
  {
    /* creation is only allowed when setting row status to createAndGo or createAndWait */
    if (VALID(I_agentDiffServPolicyRowStatus, data->valid) && 
        (data->agentDiffServPolicyRowStatus == D_agentDiffServPolicyRowStatus_createAndGo ||
         data->agentDiffServPolicyRowStatus == D_agentDiffServPolicyRowStatus_createAndWait) &&
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE))
    {
      if (usmDbDiffServPolicyCreate(USMDB_UNIT_CURRENT, 
                                    data->agentDiffServPolicyIndex, 
                                    ((data->agentDiffServPolicyRowStatus == D_agentDiffServPolicyRowStatus_createAndGo) ? L7_TRUE : L7_FALSE))!= L7_SUCCESS)
      {
        creationErrorPolicyEntry = L7_TRUE;
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
      }
      else
      {
       isCreatedNew = L7_TRUE;
       SET_VALID(I_agentDiffServPolicyRowStatus, tempValid);
      }

    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(NO_CREATION_ERROR);
    }
  }

  if (data->agentDiffServPolicyName != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDiffServPolicyName->octet_ptr, data->agentDiffServPolicyName->length);
    if (VALID(I_agentDiffServPolicyName, data->valid))
    {
      if ((usmDbStringAlphaNumericCheck(snmp_buffer) != L7_SUCCESS) ||
          (usmDbDiffServPolicyNameSet(USMDB_UNIT_CURRENT, data->agentDiffServPolicyIndex,
                                      snmp_buffer) != L7_SUCCESS))
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
      }
      else
      {
         SET_VALID(I_agentDiffServPolicyName, tempValid);
      }
    }
  }

  if (VALID(I_agentDiffServPolicyType, data->valid))
  {
    if (snmpDiffServPolicyTypeSet(USMDB_UNIT_CURRENT, data->agentDiffServPolicyIndex,
                                data->agentDiffServPolicyType) != L7_SUCCESS)
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
        SET_VALID(I_agentDiffServPolicyType, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyStorageType, data->valid))
  {
    if (snmpDiffServPolicyStorageTypeSet(USMDB_UNIT_CURRENT, data->agentDiffServPolicyIndex,
                                data->agentDiffServPolicyStorageType) != L7_SUCCESS)
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
        SET_VALID(I_agentDiffServPolicyStorageType, tempValid);
    }
  }
   
  if( data->agentDiffServPolicyRowStatus == D_agentDiffServPolicyRowStatus_destroy) 
  {
    L7_uint32 	policyIndex, nextPolicyIndex, policyInstIndex;

    agentDiffServPolicyInstEntry_count = 0;
    policyIndex = data->agentDiffServPolicyIndex;
    policyInstIndex = 0;

    while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex, &nextPolicyIndex,
                                    &policyInstIndex) == L7_SUCCESS) &&
                                    (nextPolicyIndex == policyIndex))
    {
      if( policyInstIndex == 0 )
        break;
      else
        agentDiffServPolicyInstEntry_count++; 
    }

  
    if ( agentDiffServPolicyInstEntry_count > 0)
    {
      return COMMIT_FAILED_ERROR;
    }

  }/* end of block for destroy condition */ 

  if (VALID(I_agentDiffServPolicyRowStatus, data->valid) &&
        (isCreatedNew != L7_TRUE) &&
      (snmpDiffServPolicyRowStatusSet(USMDB_UNIT_CURRENT, data->agentDiffServPolicyIndex,
                                data->agentDiffServPolicyRowStatus) != L7_SUCCESS))
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }

  return(NO_ERROR);
}

#ifdef SR_agentDiffServPolicyEntry_UNDO
/* add #define SR_agentDiffServPolicyEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServPolicyEntry family.
 */
int
agentDiffServPolicyEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
  agentDiffServPolicyEntry_t *data = (agentDiffServPolicyEntry_t *) doCur->data;
  agentDiffServPolicyEntry_t *undodata = (agentDiffServPolicyEntry_t *) doCur->undodata;
  agentDiffServPolicyEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  if( creationErrorPolicyEntry == L7_TRUE )
  {
    creationErrorPolicyEntry = L7_FALSE;
    return NO_ERROR;
  }

  if ( agentDiffServPolicyInstEntry_count > 0 )
  {
    agentDiffServPolicyInstEntry_count = 0;
    return NO_ERROR;
  }

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && data->agentDiffServPolicyRowStatus != D_agentDiffServPolicyRowStatus_destroy)
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL ) 
  {
      /* undoing an add, so delete */
      data->agentDiffServPolicyRowStatus = D_agentDiffServPolicyRowStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  } 
  else 
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->agentDiffServPolicyRowStatus == D_agentDiffServPolicyRowStatus_notReady)
         || (undodata->agentDiffServPolicyRowStatus == D_agentDiffServPolicyRowStatus_active)) 
      {
          undodata->agentDiffServPolicyRowStatus = D_agentDiffServPolicyRowStatus_createAndGo;
      } 
      else  
      {
         if(undodata->agentDiffServPolicyRowStatus == D_agentDiffServPolicyRowStatus_notInService) 
           undodata->agentDiffServPolicyRowStatus = D_agentDiffServPolicyRowStatus_createAndWait;
      }
     
     setdata = undodata;
     function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentDiffServPolicyEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServPolicyEntry_UNDO */

#endif /* SETS */

agentDiffServPolicyInstEntry_t *
k_agentDiffServPolicyInstEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_UINT32 agentDiffServPolicyIndex,
                                   SR_UINT32 agentDiffServPolicyInstIndex)
{

  static agentDiffServPolicyInstEntry_t agentDiffServPolicyInstEntryData;

  ZERO_VALID(agentDiffServPolicyInstEntryData.valid);
  agentDiffServPolicyInstEntryData.agentDiffServPolicyIndex = agentDiffServPolicyIndex;
  agentDiffServPolicyInstEntryData.agentDiffServPolicyInstIndex = agentDiffServPolicyInstIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }
  SET_VALID(I_agentDiffServPolicyIndex, agentDiffServPolicyInstEntryData.valid);
  SET_VALID(I_agentDiffServPolicyInstIndex, agentDiffServPolicyInstEntryData.valid);

	if ( (searchType == EXACT) ?
	   (snmpDiffServPolicyInstGet(&agentDiffServPolicyInstEntryData,
                                 nominator) != L7_SUCCESS) :
	   ( (snmpDiffServPolicyInstGet(&agentDiffServPolicyInstEntryData,
                                   nominator) != L7_SUCCESS) &&
		   (snmpDiffServPolicyInstGetNext(&agentDiffServPolicyInstEntryData,
                                     nominator) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentDiffServPolicyInstEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentDiffServPolicyInstEntryData.valid))
    return(NULL);

  return(&agentDiffServPolicyInstEntryData); 
   
}

#ifdef SETS
int
k_agentDiffServPolicyInstEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServPolicyInstEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServPolicyInstEntry_set_defaults(doList_t *dp)
{
    agentDiffServPolicyInstEntry_t *data = (agentDiffServPolicyInstEntry_t *) (dp->data);

    data->agentDiffServPolicyInstStorageType = D_agentDiffServPolicyInstStorageType_nonVolatile;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDiffServPolicyInstEntry_set(agentDiffServPolicyInstEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{
  L7_BOOL isCreatedNew = L7_FALSE;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

   /* If policy-instance entry doesn't exist, then try to create it */
  if (usmDbDiffServPolicyInstGet(USMDB_UNIT_CURRENT,
								 data->agentDiffServPolicyIndex,
								 data->agentDiffServPolicyInstIndex) != L7_SUCCESS)
  {
    /* creation is only allowed when setting row status to createAndGo or createAndWait */
    if (VALID(I_agentDiffServPolicyInstRowStatus, data->valid) && 
        (data->agentDiffServPolicyInstRowStatus == D_agentDiffServPolicyInstRowStatus_createAndGo ||
         data->agentDiffServPolicyInstRowStatus == D_agentDiffServPolicyInstRowStatus_createAndWait) &&
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE))
    {
      if (usmDbDiffServPolicyInstCreate(USMDB_UNIT_CURRENT,
					data->agentDiffServPolicyIndex,
					data->agentDiffServPolicyInstIndex, 
                                        ((data->agentDiffServPolicyInstRowStatus == D_agentDiffServPolicyInstRowStatus_createAndGo) ? L7_TRUE : L7_FALSE)) != L7_SUCCESS)
      {
        creationErrorPolicyInstEntry = L7_TRUE;
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
      }
    else
    {
        isCreatedNew = L7_TRUE;
        SET_VALID(I_agentDiffServPolicyInstRowStatus, tempValid);
    }

    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(NO_CREATION_ERROR);
    }
  }

  if ( VALID(I_agentDiffServPolicyInstClassIndex, data->valid))
  {
    if (usmDbDiffServPolicyInstClassIndexSet(USMDB_UNIT_CURRENT, data->agentDiffServPolicyIndex,
					 data->agentDiffServPolicyInstIndex,
                                         data->agentDiffServPolicyInstClassIndex) != L7_SUCCESS) 
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyInstClassIndex, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyInstStorageType, data->valid))
  {
    if (snmpDiffServPolicyInstStorageTypeSet(USMDB_UNIT_CURRENT,
					data->agentDiffServPolicyIndex,
					data->agentDiffServPolicyInstIndex,
                                        data->agentDiffServPolicyInstStorageType) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyInstStorageType, tempValid);
    }
  }
  
  if( data->agentDiffServPolicyInstRowStatus == D_agentDiffServPolicyInstRowStatus_destroy )
  {
     L7_uint32   policyIndex, nextPolicyIndex, policyInstIndex,nextPolicyInstIndex, policyAttrIndex;
 
     agentDiffServPolicyAttrEntry_count = 0;
     policyIndex = data->agentDiffServPolicyIndex;
     policyInstIndex = data->agentDiffServPolicyInstIndex;
     policyAttrIndex = 0;
     while ((diffServPolicyAttrGetNext(policyIndex, policyInstIndex, policyAttrIndex,
                &nextPolicyIndex, &nextPolicyInstIndex, &policyAttrIndex) == L7_SUCCESS) &&
                (nextPolicyIndex == policyIndex) &&
                (nextPolicyInstIndex == policyInstIndex))
    {
      if( policyAttrIndex == 0 )
         break;
      else
        agentDiffServPolicyAttrEntry_count++;
    }

    if ( agentDiffServPolicyAttrEntry_count > 0 )
      return COMMIT_FAILED_ERROR;
  }    

  if (VALID(I_agentDiffServPolicyInstRowStatus, data->valid) &&
        (isCreatedNew != L7_TRUE) &&
      (snmpDiffServPolicyInstRowStatusSet(USMDB_UNIT_CURRENT, 
										   data->agentDiffServPolicyIndex,
										   data->agentDiffServPolicyInstIndex,
                                data->agentDiffServPolicyInstRowStatus) != L7_SUCCESS))
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}

#ifdef SR_agentDiffServPolicyInstEntry_UNDO
/* add #define SR_agentDiffServPolicyInstEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServPolicyInstEntry family.
 */
int
agentDiffServPolicyInstEntry_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
  agentDiffServPolicyInstEntry_t *data = (agentDiffServPolicyInstEntry_t *) doCur->data;
  agentDiffServPolicyInstEntry_t *undodata = (agentDiffServPolicyInstEntry_t *) doCur->undodata;
  agentDiffServPolicyInstEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  if( creationErrorPolicyInstEntry == L7_TRUE )
  {
     creationErrorPolicyInstEntry = L7_FALSE;
     return NO_ERROR;
  }

  if( agentDiffServPolicyAttrEntry_count > 0 )
  {
    agentDiffServPolicyAttrEntry_count = 0;
    return NO_ERROR;
  }

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && (data->agentDiffServPolicyInstRowStatus != D_agentDiffServPolicyInstRowStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));
  
  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL ) 
  {
      /* undoing an add, so delete */
      data->agentDiffServPolicyInstRowStatus = D_agentDiffServPolicyInstRowStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  } 
  else 
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->agentDiffServPolicyInstRowStatus == D_agentDiffServPolicyInstRowStatus_notReady)
         || (undodata->agentDiffServPolicyInstRowStatus == D_agentDiffServPolicyInstRowStatus_active)) 
      {
          undodata->agentDiffServPolicyInstRowStatus = D_agentDiffServPolicyInstRowStatus_createAndGo;
      } 
      else  
      {
         if(undodata->agentDiffServPolicyInstRowStatus == D_agentDiffServPolicyInstRowStatus_notInService) 
           undodata->agentDiffServPolicyInstRowStatus = D_agentDiffServPolicyInstRowStatus_createAndWait;
      }
      
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentDiffServPolicyInstEntry_set(setdata, contextInfo, function) == NO_ERROR)) 
      return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServPolicyInstEntry_UNDO */

#endif /* SETS */

agentDiffServPolicyAttrEntry_t *
k_agentDiffServPolicyAttrEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_UINT32 agentDiffServPolicyIndex,
                                   SR_UINT32 agentDiffServPolicyInstIndex,
                                   SR_UINT32 agentDiffServPolicyAttrIndex)
{

  static agentDiffServPolicyAttrEntry_t agentDiffServPolicyAttrEntryData;

  ZERO_VALID(agentDiffServPolicyAttrEntryData.valid);
  agentDiffServPolicyAttrEntryData.agentDiffServPolicyIndex = agentDiffServPolicyIndex;
  agentDiffServPolicyAttrEntryData.agentDiffServPolicyInstIndex = agentDiffServPolicyInstIndex;
  agentDiffServPolicyAttrEntryData.agentDiffServPolicyAttrIndex = agentDiffServPolicyAttrIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }


 
  SET_VALID(I_agentDiffServPolicyAttrEntryIndex_agentDiffServPolicyIndex, 
			agentDiffServPolicyAttrEntryData.valid);
  SET_VALID(I_agentDiffServPolicyAttrEntryIndex_agentDiffServPolicyInstIndex, 
			agentDiffServPolicyAttrEntryData.valid);
  SET_VALID(I_agentDiffServPolicyAttrIndex,agentDiffServPolicyAttrEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpDiffServPolicyAttrGet(&agentDiffServPolicyAttrEntryData, 
                        nominator) != L7_SUCCESS) :
       ( (snmpDiffServPolicyAttrGet(&agentDiffServPolicyAttrEntryData, 
                                    nominator) != L7_SUCCESS) &&
         (snmpDiffServPolicyAttrGetNext(&agentDiffServPolicyAttrEntryData, 
						nominator) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentDiffServPolicyAttrEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentDiffServPolicyAttrEntryData.valid))
    return(NULL);

  return(&agentDiffServPolicyAttrEntryData); 
}

#ifdef SETS
int
k_agentDiffServPolicyAttrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServPolicyAttrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServPolicyAttrEntry_set_defaults(doList_t *dp)
{
    agentDiffServPolicyAttrEntry_t *data = (agentDiffServPolicyAttrEntry_t *) (dp->data);

    data->agentDiffServPolicyAttrStmtPoliceConformAct = D_agentDiffServPolicyAttrStmtPoliceConformAct_send;
    data->agentDiffServPolicyAttrStmtPoliceExceedAct = D_agentDiffServPolicyAttrStmtPoliceExceedAct_drop;
    data->agentDiffServPolicyAttrStmtPoliceNonconformAct = D_agentDiffServPolicyAttrStmtPoliceNonconformAct_drop;
    data->agentDiffServPolicyAttrStorageType = D_agentDiffServPolicyAttrStorageType_nonVolatile;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDiffServPolicyAttrEntry_set(agentDiffServPolicyAttrEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{
  /* This variable is added to pass to snmpDiffServClassGet() function so that 
   * our set values in 'data' are not lost.
   */
  agentDiffServPolicyAttrEntry_t *tempData;

  L7_BOOL isCreatedNew = L7_FALSE;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_agentDiffServPolicyAttrEntry(data);
  tempData->agentDiffServPolicyAttrIndex = data->agentDiffServPolicyAttrIndex;  /* QSCAN: DELETE -- Temp fixup */
  tempData->agentDiffServPolicyIndex = data->agentDiffServPolicyIndex;          /* QSCAN: DELETE -- Temp fixup */
  tempData->agentDiffServPolicyInstIndex = data->agentDiffServPolicyInstIndex;  /* QSCAN: DELETE -- Temp fixup */

  /* If policy-instance attribute entry doesn't exist, then try to create it */
  if (snmpDiffServPolicyAttrGet(tempData, -1) != L7_SUCCESS)
  {
    /* creation is only allowed when setting row status to createAndGo or createAndWait */
    if (VALID(I_agentDiffServPolicyAttrRowStatus, data->valid) && 
        (data->agentDiffServPolicyAttrRowStatus == D_agentDiffServPolicyAttrRowStatus_createAndGo ||
         data->agentDiffServPolicyAttrRowStatus == D_agentDiffServPolicyAttrRowStatus_createAndWait) &&
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE))
    {
      if (usmDbDiffServPolicyAttrCreate(USMDB_UNIT_CURRENT, 
                                        data->agentDiffServPolicyIndex,
                                        data->agentDiffServPolicyInstIndex,
                                        data->agentDiffServPolicyAttrIndex, 
                                        ((data->agentDiffServPolicyAttrRowStatus == D_agentDiffServPolicyAttrRowStatus_createAndGo) ? L7_TRUE : L7_FALSE)) != L7_SUCCESS)
      {
        creationErrorPolicyAttrEntry = L7_TRUE;
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
      }
      else
      {
        isCreatedNew = L7_TRUE;
        SET_VALID(I_agentDiffServPolicyAttrRowStatus, tempValid);
      }
    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(NO_CREATION_ERROR);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtEntryType, data->valid))
  {
    if (snmpDiffServPolicyAttrStmtEntryTypeSet(USMDB_UNIT_CURRENT, 
                                               data->agentDiffServPolicyIndex,
                                               data->agentDiffServPolicyInstIndex,
                                               data->agentDiffServPolicyAttrIndex,
                                               data->agentDiffServPolicyAttrStmtEntryType) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtEntryType, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtMarkCosVal, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtMarkCosValSet(USMDB_UNIT_CURRENT, 
                                                 data->agentDiffServPolicyIndex,
                                                 data->agentDiffServPolicyInstIndex,
                                                 data->agentDiffServPolicyAttrIndex,
                                                 data->agentDiffServPolicyAttrStmtMarkCosVal) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtMarkCosVal, tempValid);
    }
  }

#ifdef I_agentDiffServPolicyAttrStmtMarkCosAsSecCos
  if (VALID(I_agentDiffServPolicyAttrStmtMarkCosAsSecCos, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtMarkCosAsCos2Set(USMDB_UNIT_CURRENT, 
                                                 data->agentDiffServPolicyIndex,
                                                 data->agentDiffServPolicyInstIndex,
                                                 data->agentDiffServPolicyAttrIndex,
                                                 data->agentDiffServPolicyAttrStmtMarkCosAsSecCos) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtMarkCosAsSecCos, tempValid);
    }
  }
#endif

  if (VALID(I_agentDiffServPolicyAttrStmtMarkIpDscpVal, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtMarkIpDscpValSet(USMDB_UNIT_CURRENT, 
                                                    data->agentDiffServPolicyIndex,
                                                    data->agentDiffServPolicyInstIndex,
                                                    data->agentDiffServPolicyAttrIndex,
                                                    data->agentDiffServPolicyAttrStmtMarkIpDscpVal) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtMarkIpDscpVal, tempValid);
    }
  }



  if (VALID(I_agentDiffServPolicyAttrStmtMarkIpPrecedenceVal, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValSet(USMDB_UNIT_CURRENT, 
                                                          data->agentDiffServPolicyIndex,
                                                          data->agentDiffServPolicyInstIndex,
                                                          data->agentDiffServPolicyAttrIndex,
                                                          data->agentDiffServPolicyAttrStmtMarkIpPrecedenceVal) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtMarkIpPrecedenceVal, tempValid);
    }
  }


  if (VALID(I_agentDiffServPolicyAttrStmtMirrorIntf, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtMirrorIntfSet(USMDB_UNIT_CURRENT, 
                                                 data->agentDiffServPolicyIndex,
                                                 data->agentDiffServPolicyInstIndex,
                                                 data->agentDiffServPolicyAttrIndex,
                                                 data->agentDiffServPolicyAttrStmtMirrorIntf) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtMirrorIntf, tempValid);
    }
  }


  if (VALID(I_agentDiffServPolicyAttrStmtPoliceConformAct, data->valid))
  {
    if (snmpDiffServPolicyAttrStmtPoliceConformActSet(USMDB_UNIT_CURRENT, 
                                                      data->agentDiffServPolicyIndex,
                                                      data->agentDiffServPolicyInstIndex,
                                                      data->agentDiffServPolicyAttrIndex,
                                                      data->agentDiffServPolicyAttrStmtPoliceConformAct) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceConformAct, tempValid);
    }
  }



  if (VALID(I_agentDiffServPolicyAttrStmtPoliceConformVal, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceConformValSet(USMDB_UNIT_CURRENT, 
                                                       data->agentDiffServPolicyIndex,
                                                       data->agentDiffServPolicyInstIndex,
                                                       data->agentDiffServPolicyAttrIndex,
                                                       data->agentDiffServPolicyAttrStmtPoliceConformVal) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceConformVal, tempValid);
    }
  }


  if (VALID(I_agentDiffServPolicyAttrStmtPoliceExceedAct, data->valid))
  {
    if (snmpDiffServPolicyAttrStmtPoliceExceedActSet(USMDB_UNIT_CURRENT, 
                                                     data->agentDiffServPolicyIndex,
                                                     data->agentDiffServPolicyInstIndex,
                                                     data->agentDiffServPolicyAttrIndex,
                                                     data->agentDiffServPolicyAttrStmtPoliceExceedAct) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceExceedAct, tempValid);
    }
  }



  if (VALID(I_agentDiffServPolicyAttrStmtPoliceExceedVal, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceExceedValSet(USMDB_UNIT_CURRENT, 
                                                      data->agentDiffServPolicyIndex,
                                                      data->agentDiffServPolicyInstIndex,
                                                      data->agentDiffServPolicyAttrIndex,
                                                      data->agentDiffServPolicyAttrStmtPoliceExceedVal) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceExceedVal, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtPoliceNonconformAct, data->valid))
  {
    if (snmpDiffServPolicyAttrStmtPoliceNononformActSet(USMDB_UNIT_CURRENT, 
                                                        data->agentDiffServPolicyIndex,
                                                        data->agentDiffServPolicyInstIndex,
                                                        data->agentDiffServPolicyAttrIndex,
                                                        data->agentDiffServPolicyAttrStmtPoliceNonconformAct) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceNonconformAct, tempValid);
    }
  }



  if (VALID(I_agentDiffServPolicyAttrStmtPoliceNonconformVal, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceNonconformValSet(USMDB_UNIT_CURRENT, 
                                                          data->agentDiffServPolicyIndex,
                                                          data->agentDiffServPolicyInstIndex,
                                                          data->agentDiffServPolicyAttrIndex,
                                                          data->agentDiffServPolicyAttrStmtPoliceNonconformVal) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceNonconformVal, tempValid);
    }
  }


  if (VALID(I_agentDiffServPolicyAttrStmtPoliceSimpleCrate, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceSimpleCrateSet(USMDB_UNIT_CURRENT, 
                                                        data->agentDiffServPolicyIndex,
                                                        data->agentDiffServPolicyInstIndex,
                                                        data->agentDiffServPolicyAttrIndex,
                                                        data->agentDiffServPolicyAttrStmtPoliceSimpleCrate) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSimpleCrate, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtPoliceSimpleCburst, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceSimpleCburstSet(USMDB_UNIT_CURRENT, 
                                                         data->agentDiffServPolicyIndex,
                                                         data->agentDiffServPolicyInstIndex,
                                                         data->agentDiffServPolicyAttrIndex,
                                                         data->agentDiffServPolicyAttrStmtPoliceSimpleCburst) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSimpleCburst, tempValid);
    }
  }


  if (VALID(I_agentDiffServPolicyAttrStmtPoliceSinglerateCrate, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateSet(USMDB_UNIT_CURRENT, 
                                                            data->agentDiffServPolicyIndex,
                                                            data->agentDiffServPolicyInstIndex,
                                                            data->agentDiffServPolicyAttrIndex,
                                                            data->agentDiffServPolicyAttrStmtPoliceSinglerateCrate) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSinglerateCrate, tempValid);
    }
  }



  if (VALID(I_agentDiffServPolicyAttrStmtPoliceSinglerateCburst, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstSet(USMDB_UNIT_CURRENT, 
                                                             data->agentDiffServPolicyIndex,
                                                             data->agentDiffServPolicyInstIndex,
                                                             data->agentDiffServPolicyAttrIndex,
                                                             data->agentDiffServPolicyAttrStmtPoliceSinglerateCburst) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSinglerateCburst, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtPoliceSinglerateEburst, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstSet(USMDB_UNIT_CURRENT, 
                                                             data->agentDiffServPolicyIndex,
                                                             data->agentDiffServPolicyInstIndex,
                                                             data->agentDiffServPolicyAttrIndex,
                                                             data->agentDiffServPolicyAttrStmtPoliceSinglerateEburst) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSinglerateEburst, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtPoliceTworateCrate, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceTworateCrateSet(USMDB_UNIT_CURRENT, 
                                                         data->agentDiffServPolicyIndex,
                                                         data->agentDiffServPolicyInstIndex,
                                                         data->agentDiffServPolicyAttrIndex,
                                                         data->agentDiffServPolicyAttrStmtPoliceTworateCrate) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceTworateCrate, tempValid);
    }
  }


  if (VALID(I_agentDiffServPolicyAttrStmtPoliceTworateCburst, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceTworateCburstSet(USMDB_UNIT_CURRENT, 
                                                          data->agentDiffServPolicyIndex,
                                                          data->agentDiffServPolicyInstIndex,
                                                          data->agentDiffServPolicyAttrIndex,
                                                          data->agentDiffServPolicyAttrStmtPoliceTworateCburst) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceTworateCburst, tempValid);
    }
  }


  if (VALID(I_agentDiffServPolicyAttrStmtPoliceTworatePrate, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceTworatePrateSet(USMDB_UNIT_CURRENT, 
                                                         data->agentDiffServPolicyIndex,
                                                         data->agentDiffServPolicyInstIndex,
                                                         data->agentDiffServPolicyAttrIndex,
                                                         data->agentDiffServPolicyAttrStmtPoliceTworatePrate) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceTworatePrate, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtPoliceTworatePburst, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtPoliceTworatePburstSet(USMDB_UNIT_CURRENT, 
                                                          data->agentDiffServPolicyIndex,
                                                          data->agentDiffServPolicyInstIndex,
                                                          data->agentDiffServPolicyAttrIndex,
                                                          data->agentDiffServPolicyAttrStmtPoliceTworatePburst) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceTworatePburst, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStorageType, data->valid))
  {
    if (snmpDiffServPolicyAttrStorageTypeSet(USMDB_UNIT_CURRENT, 
                                             data->agentDiffServPolicyIndex,
                                             data->agentDiffServPolicyInstIndex,
                                             data->agentDiffServPolicyAttrIndex,
                                             data->agentDiffServPolicyAttrStorageType) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStorageType, tempValid);
    }
  }


  if (VALID(I_agentDiffServPolicyAttrRowStatus, data->valid) &&
      (isCreatedNew != L7_TRUE) &&
      (snmpDiffServPolicyAttrRowStatusSet(USMDB_UNIT_CURRENT, 
                                          data->agentDiffServPolicyIndex,
                                          data->agentDiffServPolicyInstIndex,
                                          data->agentDiffServPolicyAttrIndex,
                                          data->agentDiffServPolicyAttrRowStatus) != L7_SUCCESS))
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDiffServPolicyAttrStmtAssignQueueId, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtAssignQueueIdSet(USMDB_UNIT_CURRENT, 
                                                    data->agentDiffServPolicyIndex,
                                                    data->agentDiffServPolicyInstIndex,
                                                    data->agentDiffServPolicyAttrIndex,
                                                    data->agentDiffServPolicyAttrStmtAssignQueueId) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtAssignQueueId, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtMarkCos2Val, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtMarkCos2ValSet(USMDB_UNIT_CURRENT, 
                                                  data->agentDiffServPolicyIndex,
                                                  data->agentDiffServPolicyInstIndex,
                                                  data->agentDiffServPolicyAttrIndex,
                                                  data->agentDiffServPolicyAttrStmtMarkCos2Val) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtMarkCos2Val, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtPoliceColorConformIndex, data->valid))
  {
    if (snmpAgentDiffServPolicyAttrStmtPoliceColorConformIndexSet(USMDB_UNIT_CURRENT, 
                                                                  data->agentDiffServPolicyIndex,
                                                                  data->agentDiffServPolicyInstIndex,
                                                                  data->agentDiffServPolicyAttrIndex,
                                                                  data->agentDiffServPolicyAttrStmtPoliceColorConformIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceColorConformIndex, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtPoliceColorExceedIndex, data->valid))
  {
    if (snmpAgentDiffServPolicyAttrStmtPoliceColorExceedIndexSet(USMDB_UNIT_CURRENT, 
                                                                 data->agentDiffServPolicyIndex,
                                                                 data->agentDiffServPolicyInstIndex,
                                                                 data->agentDiffServPolicyAttrIndex,
                                                                 data->agentDiffServPolicyAttrStmtPoliceColorExceedIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtPoliceColorExceedIndex, tempValid);
    }
  }

  if (VALID(I_agentDiffServPolicyAttrStmtRedirectIntf, data->valid))
  {
    if (usmDbDiffServPolicyAttrStmtRedirectIntfSet(USMDB_UNIT_CURRENT, 
                                                   data->agentDiffServPolicyIndex,
                                                   data->agentDiffServPolicyInstIndex,
                                                   data->agentDiffServPolicyAttrIndex,
                                                   data->agentDiffServPolicyAttrStmtRedirectIntf) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentDiffServPolicyAttrStmtRedirectIntf, tempValid);
    }
  }

  return(NO_ERROR);
}

#ifdef SR_agentDiffServPolicyAttrEntry_UNDO
/* add #define SR_agentDiffServPolicyAttrEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServPolicyAttrEntry family.
 */
int
agentDiffServPolicyAttrEntry_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
  agentDiffServPolicyAttrEntry_t *data = (agentDiffServPolicyAttrEntry_t *) doCur->data;
  agentDiffServPolicyAttrEntry_t *undodata = (agentDiffServPolicyAttrEntry_t *) doCur->undodata;
  agentDiffServPolicyAttrEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;


  if ( creationErrorPolicyAttrEntry == L7_TRUE )
  {
      creationErrorPolicyAttrEntry = L7_FALSE;
      return NO_ERROR;
  }


  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && (data->agentDiffServPolicyAttrRowStatus != D_agentDiffServPolicyAttrRowStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));
  
  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL ) 
  {
      /* undoing an add, so delete */
      data->agentDiffServPolicyAttrRowStatus = D_agentDiffServPolicyAttrRowStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  } 
  else 
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->agentDiffServPolicyAttrRowStatus == D_agentDiffServPolicyAttrRowStatus_notReady)
         || (undodata->agentDiffServPolicyAttrRowStatus == D_agentDiffServPolicyAttrRowStatus_active)) 
      {
          undodata->agentDiffServPolicyAttrRowStatus = D_agentDiffServPolicyAttrRowStatus_createAndGo;
      } 
      else 
      {
         if(undodata->agentDiffServPolicyAttrRowStatus == D_agentDiffServPolicyAttrRowStatus_notInService) 
           undodata->agentDiffServPolicyAttrRowStatus = D_agentDiffServPolicyAttrRowStatus_createAndWait;
      }

      setdata = undodata;
      function = SR_ADD_MODIFY;

  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentDiffServPolicyAttrEntry_set(setdata, contextInfo, function) == NO_ERROR)) 
      return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServPolicyAttrEntry_UNDO */

#endif /* SETS */

agentDiffServPolicyPerfInEntry_t *
k_agentDiffServPolicyPerfInEntry_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator,
                                     int searchType,
                                     SR_UINT32 agentDiffServPolicyIndex,
                                     SR_UINT32 agentDiffServPolicyInstIndex,
                                     SR_INT32 ifIndex)
{
  static agentDiffServPolicyPerfInEntry_t agentDiffServPolicyPerfInEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentDiffServPolicyPerfInEntryData.agentDiffServPolicyPerfInHCOfferedOctets = MakeCounter64(0);
    agentDiffServPolicyPerfInEntryData.agentDiffServPolicyPerfInHCOfferedPackets = MakeCounter64(0);
    agentDiffServPolicyPerfInEntryData.agentDiffServPolicyPerfInHCDiscardedOctets = MakeCounter64(0);
    agentDiffServPolicyPerfInEntryData.agentDiffServPolicyPerfInHCDiscardedPackets = MakeCounter64(0);
  }


  ZERO_VALID(agentDiffServPolicyPerfInEntryData.valid);
  agentDiffServPolicyPerfInEntryData.agentDiffServPolicyIndex = agentDiffServPolicyIndex;
  agentDiffServPolicyPerfInEntryData.agentDiffServPolicyInstIndex = agentDiffServPolicyInstIndex;
  agentDiffServPolicyPerfInEntryData.ifIndex = ifIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_agentDiffServPolicyPerfInEntryIndex_agentDiffServPolicyIndex, 
			agentDiffServPolicyPerfInEntryData.valid);
  SET_VALID(I_agentDiffServPolicyPerfInEntryIndex_agentDiffServPolicyInstIndex, 
			agentDiffServPolicyPerfInEntryData.valid);
  SET_VALID(I_agentDiffServPolicyPerfInEntryIndex_ifIndex,agentDiffServPolicyPerfInEntryData.valid);

  if ( (searchType == EXACT) ?
       ( (usmDbIntIfNumFromExtIfNum(agentDiffServPolicyPerfInEntryData.ifIndex, &intIfNum) != L7_SUCCESS) ||
         (snmpDiffServPolicyPerfInGet(&agentDiffServPolicyPerfInEntryData, intIfNum, 
                                      nominator) != L7_SUCCESS) ) :

       ( ( (usmDbVisibleExtIfNumberCheck( USMDB_UNIT_CURRENT, agentDiffServPolicyPerfInEntryData.ifIndex) != L7_SUCCESS) &&
           (usmDbGetNextVisibleExtIfNumber(agentDiffServPolicyPerfInEntryData.ifIndex, &agentDiffServPolicyPerfInEntryData.ifIndex) != L7_SUCCESS) ) ||
         (usmDbIntIfNumFromExtIfNum(agentDiffServPolicyPerfInEntryData.ifIndex, &intIfNum) != L7_SUCCESS) ||
         ( (snmpDiffServPolicyPerfInGet(&agentDiffServPolicyPerfInEntryData, intIfNum,
                                        nominator) != L7_SUCCESS) &&
           (snmpDiffServPolicyPerfInGetNext(&agentDiffServPolicyPerfInEntryData, &intIfNum,
						                                nominator) != L7_SUCCESS) )  || 
         (usmDbExtIfNumFromIntIfNum(intIfNum, &agentDiffServPolicyPerfInEntryData.ifIndex) != L7_SUCCESS) )
     )
  {
    ZERO_VALID(agentDiffServPolicyPerfInEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentDiffServPolicyPerfInEntryData.valid))
    return(NULL);

  return(&agentDiffServPolicyPerfInEntryData);
}

agentDiffServPolicyPerfOutEntry_t *
k_agentDiffServPolicyPerfOutEntry_get(int serialNum, ContextInfo *contextInfo,
                                      int nominator,
                                      int searchType,
                                      SR_UINT32 agentDiffServPolicyIndex,
                                      SR_UINT32 agentDiffServPolicyInstIndex,
                                      SR_INT32 ifIndex)
{
  static agentDiffServPolicyPerfOutEntry_t agentDiffServPolicyPerfOutEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCTailDroppedOctets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCTailDroppedPackets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCRandomDroppedOctets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCRandomDroppedPackets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCShapeDelayedOctets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCShapeDelayedPackets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCSentOctets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCSentPackets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCOfferedOctets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCOfferedPackets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCDiscardedOctets = MakeCounter64(0);
    agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyPerfOutHCDiscardedPackets = MakeCounter64(0);
  }


  ZERO_VALID(agentDiffServPolicyPerfOutEntryData.valid);
  agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyIndex = agentDiffServPolicyIndex;
  agentDiffServPolicyPerfOutEntryData.agentDiffServPolicyInstIndex = agentDiffServPolicyInstIndex;
  agentDiffServPolicyPerfOutEntryData.ifIndex = ifIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_agentDiffServPolicyPerfOutEntryIndex_agentDiffServPolicyIndex, 
			agentDiffServPolicyPerfOutEntryData.valid);
  SET_VALID(I_agentDiffServPolicyPerfOutEntryIndex_agentDiffServPolicyInstIndex, 
			agentDiffServPolicyPerfOutEntryData.valid);
  SET_VALID(I_agentDiffServPolicyPerfOutEntryIndex_ifIndex,agentDiffServPolicyPerfOutEntryData.valid);

  if ( (searchType == EXACT) ?
       ( (usmDbIntIfNumFromExtIfNum(agentDiffServPolicyPerfOutEntryData.ifIndex, &intIfNum) != L7_SUCCESS) ||
         (snmpDiffServPolicyPerfOutGet(&agentDiffServPolicyPerfOutEntryData, intIfNum, 
                                       nominator) != L7_SUCCESS) ) :

       ( ( (usmDbVisibleExtIfNumberCheck( USMDB_UNIT_CURRENT, agentDiffServPolicyPerfOutEntryData.ifIndex) != L7_SUCCESS) &&
           (usmDbGetNextVisibleExtIfNumber(agentDiffServPolicyPerfOutEntryData.ifIndex, &agentDiffServPolicyPerfOutEntryData.ifIndex) != L7_SUCCESS) ) ||
         (usmDbIntIfNumFromExtIfNum(agentDiffServPolicyPerfOutEntryData.ifIndex, &intIfNum) != L7_SUCCESS) ||
         ( (snmpDiffServPolicyPerfOutGet(&agentDiffServPolicyPerfOutEntryData, intIfNum,
                                         nominator) != L7_SUCCESS) &&
           (snmpDiffServPolicyPerfOutGetNext(&agentDiffServPolicyPerfOutEntryData, &intIfNum,
						                                 nominator) != L7_SUCCESS) )  || 
         (usmDbExtIfNumFromIntIfNum(intIfNum, &agentDiffServPolicyPerfOutEntryData.ifIndex) != L7_SUCCESS) )
     )
  {
    ZERO_VALID(agentDiffServPolicyPerfOutEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentDiffServPolicyPerfOutEntryData.valid))
    return(NULL);

  return(&agentDiffServPolicyPerfOutEntryData);
}

agentDiffServServiceEntry_t *
k_agentDiffServServiceEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_INT32 agentDiffServServiceIfIndex,
                                SR_INT32 agentDiffServServiceIfDirection)
{
  static agentDiffServServiceEntry_t agentDiffServServiceEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  L7_uint32 intIfNum;
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t intDirection;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentDiffServServiceEntryData.agentDiffServServicePerfHCOfferedOctets = MakeCounter64(0);
    agentDiffServServiceEntryData.agentDiffServServicePerfHCOfferedPackets = MakeCounter64(0);
    agentDiffServServiceEntryData.agentDiffServServicePerfHCDiscardedOctets = MakeCounter64(0);
    agentDiffServServiceEntryData.agentDiffServServicePerfHCDiscardedPackets = MakeCounter64(0);
    agentDiffServServiceEntryData.agentDiffServServicePerfHCSentOctets = MakeCounter64(0);
    agentDiffServServiceEntryData.agentDiffServServicePerfHCSentPackets = MakeCounter64(0);
  }

  ZERO_VALID(agentDiffServServiceEntryData.valid);
  agentDiffServServiceEntryData.agentDiffServServiceIfIndex = agentDiffServServiceIfIndex;
  agentDiffServServiceEntryData.agentDiffServServiceIfDirection = agentDiffServServiceIfDirection;
   
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_agentDiffServServiceIfIndex, agentDiffServServiceEntryData.valid);
  SET_VALID(I_agentDiffServServiceIfDirection, agentDiffServServiceEntryData.valid);
   

  if ( (searchType == EXACT) ?
       ( (usmDbIntIfNumFromExtIfNum(agentDiffServServiceEntryData.agentDiffServServiceIfIndex, &intIfNum) != L7_SUCCESS) ||
         (snmpDiffServServiceGet(&agentDiffServServiceEntryData, intIfNum,
                                 nominator, &intDirection) != L7_SUCCESS) ) :

       ( ( (usmDbVisibleExtIfNumberCheck( USMDB_UNIT_CURRENT, agentDiffServServiceEntryData.agentDiffServServiceIfIndex) != L7_SUCCESS) &&
           (usmDbGetNextVisibleExtIfNumber(agentDiffServServiceEntryData.agentDiffServServiceIfIndex, &agentDiffServServiceEntryData.agentDiffServServiceIfIndex) != L7_SUCCESS) ) ||
         (usmDbIntIfNumFromExtIfNum(agentDiffServServiceEntryData.agentDiffServServiceIfIndex, &intIfNum) != L7_SUCCESS) ||
         ( (snmpDiffServServiceGet(&agentDiffServServiceEntryData, intIfNum,
                                   nominator, &intDirection) != L7_SUCCESS) &&
           (snmpDiffServServiceGetNext(&agentDiffServServiceEntryData, &intIfNum,
                                       nominator, &intDirection) != L7_SUCCESS) ) ||
         (usmDbExtIfNumFromIntIfNum(intIfNum, &agentDiffServServiceEntryData.agentDiffServServiceIfIndex) != L7_SUCCESS) )
     )
  {
    ZERO_VALID(agentDiffServServiceEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentDiffServServiceEntryData.valid))
    return(NULL);

  return(&agentDiffServServiceEntryData);
}

#ifdef SETS
int
k_agentDiffServServiceEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServServiceEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                  doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServServiceEntry_set_defaults(doList_t *dp)
{
    agentDiffServServiceEntry_t *data = (agentDiffServServiceEntry_t *) (dp->data);

    data->agentDiffServServiceStorageType = D_agentDiffServServiceStorageType_nonVolatile;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDiffServServiceEntry_set(agentDiffServServiceEntry_t *data,
                                ContextInfo *contextInfo, int function)
{
  static L7_BOOL firstTime = L7_TRUE;
  static L7_BOOL allPortsOnlyIn = L7_FALSE;
  static L7_BOOL allPortsOnlyOut = L7_FALSE;
  static L7_uint32 compId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;

  L7_RC_t rc;
  L7_BOOL allPortsOnly;
  L7_uint32  intIfNum;
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t intDirection;

  agentDiffServServiceEntry_t *tempData = NULL;

  L7_BOOL isCreatedNew = L7_FALSE;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    /* check for inbound special case of "all ports only" based on feature support */
    if ( (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, compId, L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID) == L7_FALSE) &&
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, compId, L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID) == L7_TRUE) )
      allPortsOnlyIn = L7_TRUE;

    /* check for outbound special case of "all ports only" based on feature support */
    if ( (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, compId, L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_FALSE) &&
         (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, compId, L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE) )
      allPortsOnlyOut = L7_TRUE;
  }

  if (usmDbIntIfNumFromExtIfNum(data->agentDiffServServiceIfIndex, &intIfNum) != L7_SUCCESS)
  {
    return(NO_CREATION_ERROR);
  }

  if((tempData = Clone_agentDiffServServiceEntry(data))== NULL)
     return COMMIT_FAILED_ERROR;
  tempData->agentDiffServServiceIfIndex = data->agentDiffServServiceIfIndex;          /* QSCAN: DELETE -- Temp fixup */
  tempData->agentDiffServServiceIfDirection = data->agentDiffServServiceIfDirection;  /* QSCAN: DELETE -- Temp fixup */

  rc = snmpDiffServServiceGet(tempData, intIfNum, I_agentDiffServServiceIfIndex, &intDirection);

  if (intDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
  {
    allPortsOnly = allPortsOnlyIn;
  }
  else if (intDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT)
  {
    allPortsOnly = allPortsOnlyOut;
  }
  else
  {
    return(NO_CREATION_ERROR);
  }

  /* change the individual intIfNum to the special "all interfaces" value for
   * the special case where the object create/set must always be applied to 
   * all ports
   *
   * NOTE: must do this AFTER the ServiceGet call, since that can only handle
   *       an individual internal interface number (for the special all ports
   *       case, the existence of a single interface is representative of 
   *       all interfaces)
   */
  if (allPortsOnly == L7_TRUE)
    intIfNum = L7_ALL_INTERFACES;

  /* If service entry doesn't exist, then try to create it */
  if (rc != L7_SUCCESS)
  {
    /* creation is only allowed when setting row status to createAndGo or createAndWait */
    if (VALID(I_agentDiffServServiceRowStatus, data->valid) && 
        (data->agentDiffServServiceRowStatus == D_agentDiffServServiceRowStatus_createAndGo ||
         data->agentDiffServServiceRowStatus == D_agentDiffServServiceRowStatus_createAndWait) &&
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, compId, L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE))
    {
      if (usmDbDiffServServiceCreate(USMDB_UNIT_CURRENT, 
                                     intIfNum,
									                   intDirection,
                                     ((data->agentDiffServServiceRowStatus == D_agentDiffServServiceRowStatus_createAndGo) ? L7_TRUE : L7_FALSE)) != L7_SUCCESS)
      {
        creationErrorServiceEntry = L7_TRUE;
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
      }
      else
      {
        isCreatedNew = L7_TRUE;
        SET_VALID(I_agentDiffServServiceRowStatus, tempValid);
      }

    }
    else
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
    }
  }

  if ( VALID(I_agentDiffServServicePolicyIndex, data->valid))
  {
    if (usmDbDiffServServicePolicyIndexSet(USMDB_UNIT_CURRENT,
										                      intIfNum,
   				                                intDirection,
                                          data->agentDiffServServicePolicyIndex) != L7_SUCCESS )
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServServicePolicyIndex, tempValid);
    }
  }
    

  if ( VALID(I_agentDiffServServiceStorageType, data->valid))
  {
    if  (snmpDiffServServiceStorageTypeSet(USMDB_UNIT_CURRENT, 
                                          intIfNum,
   				                                intDirection,
                                          data->agentDiffServServiceStorageType) != L7_SUCCESS) 
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
    }
    else
    {
       SET_VALID(I_agentDiffServServiceStorageType, tempValid);
    }
  }
  
  if ( VALID(I_agentDiffServServiceRowStatus, data->valid) &&
        (isCreatedNew != L7_TRUE) &&
       (snmpDiffServServiceRowStatusSet(USMDB_UNIT_CURRENT, 
                                        intIfNum,
                                        intDirection,
                                        data->agentDiffServServiceRowStatus) != L7_SUCCESS)) 
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(NO_CREATION_ERROR);
    }

  return(NO_ERROR);
}

#ifdef SR_agentDiffServServiceEntry_UNDO
/* add #define SR_agentDiffServServiceEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServServiceEntry family.
 */
int
agentDiffServServiceEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
  agentDiffServServiceEntry_t *data = (agentDiffServServiceEntry_t *) doCur->data;
  agentDiffServServiceEntry_t *undodata = (agentDiffServServiceEntry_t *) doCur->undodata;
  agentDiffServServiceEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;
  
  if( creationErrorServiceEntry == L7_TRUE )
  {
    creationErrorServiceEntry = L7_FALSE;
    return NO_ERROR;
  }

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && (data->agentDiffServServiceRowStatus != D_agentDiffServServiceRowStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL ) 
  {
      /* undoing an add, so delete */
      data->agentDiffServServiceRowStatus = D_agentDiffServServiceRowStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  } 
  else 
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->agentDiffServServiceRowStatus == D_agentDiffServServiceRowStatus_notReady)
         || (undodata->agentDiffServServiceRowStatus == D_agentDiffServServiceRowStatus_active)) 
      {
          undodata->agentDiffServServiceRowStatus = D_agentDiffServServiceRowStatus_createAndGo;
      } 
      else  
      {
         if(undodata->agentDiffServServiceRowStatus == D_agentDiffServServiceRowStatus_notInService) 
           undodata->agentDiffServServiceRowStatus = D_agentDiffServServiceRowStatus_createAndWait;
      }
      

      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentDiffServServiceEntry_set(setdata, contextInfo, function) == NO_ERROR)) 
      return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServServiceEntry_UNDO */

#endif /* SETS */


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   usmdb_mib_diffserv_api.h
*
* @purpose    Provide interface to DiffServ draft MIB APIs for unitmgr 
*             components
*
* @component  unitmgr
*
* @comments   none
*
* @create     03/18/2002
*
* @author     gpaussa
* @end
*             
**********************************************************************/
#ifndef USMDB_MIB_DIFFSERV_API_H
#define USMDB_MIB_DIFFSERV_API_H


#include "usmdb_mib_diffserv_common.h"


/*
***********************************************************************
*   MIB DEFINED ENUMERATIONS
***********************************************************************
*/

/* Values for Token Bucket Meter Parameter type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_NONE = 0,
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_SIMPLE_TOKEN_BUCKET,
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_AVG_RATE,
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_SRTCM_BLIND,
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_SRTCM_AWARE,
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_TRTCM_BLIND,
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_TRTCM_AWARE,
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_TSWTCM,
  /* extensions MIB */
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_SIMPLE_TOKEN_BUCKET_AWARE,

  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_TOTAL     /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_t;

/* Values for Algorithmic Dropper type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_NONE = 0,
  L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_OTHER,
  L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_TAIL_DROP,
  L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_HEAD_DROP,
  L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_RANDOM_DROP,
  L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_ALWAYS_DROP,

  L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_TOTAL     /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_t;

/* Values for Color Level */
typedef enum
{
 L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_NONE = 0,
 L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_CONFORM,
 L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_EXCEED,

 L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_TOTAL        /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_t;

/* Values for Color Mode identifier */
typedef enum
{
 L7_USMDB_MIB_DIFFSERV_COLOR_MODE_NONE = 0,
 L7_USMDB_MIB_DIFFSERV_COLOR_MODE_BLIND,
 L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_COS,
 L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_COS2,
 L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_IPDSCP,
 L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_IPPREC,
 L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_UNUSED,

 L7_USMDB_MIB_DIFFSERV_COLOR_MODE_TOTAL  /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_COLOR_MODE_t;


/*
=========================
=========================
=========================

   DATA PATH TABLE API

=========================
=========================
=========================
*/
 
/*********************************************************************
* @purpose  Verify that a Data Path Table row exists for the specified 
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServDataPathGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);

/*********************************************************************
* @purpose  Determine next sequential row in the Data Path Table
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    prevIntIfNum    @b{(input)}  Internal interface number to 
                                         begin search
* @param    prevIfDirection @b{(input)}  Interface direction to begin search
* @param    pIntIfNum       @b{(output)} Pointer to next sequential internal 
*                                        interface number value
* @param    pIfDirection    @b{(output)} Pointer to next sequential
*                                        interface direction value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevIntIfNum value of 0 is used to find the first interface
*           entry in the table.
*
* @notes    A prevIfDirection value of 0 is used to find the first
*           interface direction entry relative to the specified prevIntIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServDataPathGetNext(L7_uint32 UnitIndex, 
                                     L7_uint32 prevIntIfNum, 
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t prevIfDirection,
                                     L7_uint32 *pIntIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t *pIfDirection);

/*********************************************************************
* @purpose  Get the row pointer of the first DiffServ functional data 
*           path element to handle traffic for this data path
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pRowPtr     @b{(output)} Pointer to a row pointer value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServDataPathStartGet(L7_uint32 UnitIndex, 
                                     L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                              L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);

/*********************************************************************
* @purpose  Get the storage type for the specified Data Path Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServDataPathStorageGet(L7_uint32 UnitIndex,
                                        L7_uint32 intIfNum,
                                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                        L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Get the row status for the specified Data Path Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServDataPathStatusGet(L7_uint32 UnitIndex,
                                       L7_uint32 intIfNum,
                                       L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                       L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);


/*
==========================
==========================
==========================

   CLASSIFIER TABLE API

==========================
==========================
==========================
*/

/*************************************************************************
* @purpose  Obtain the current value of the classifier index next variable
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    pClfrIndex  @b{(output)} Pointer to classifier index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pClfrIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrIndexNext(L7_uint32 UnitIndex, L7_uint32 *pClfrIndex);

/*********************************************************************
* @purpose  Verify that a Classifeir Table row exists for the specified 
*           classifier Id.
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    clfrId      @b{(input)} Classifier Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrGet(L7_uint32 UnitIndex, L7_uint32 clfrId);

/*********************************************************************
* @purpose  Determine next sequential row in the Classifier Table
*
* @param    UnitIndex   @b{(input)}  System unit number                         
* @param    prevClfrId  @b{(input)}  Classifier Id to begin search
* @param    pClfrId     @b{(output)} Pointer to next sequential classifier Id.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevClfrId value of 0 is used to find the first classifier
*           entry in the table.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrGetNext(L7_uint32 UnitIndex, L7_uint32 prevClfrId,
                                 L7_uint32 *pClfrId);

/*********************************************************************
* @purpose  Get the storage type for the specified Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    clfrId      @b{(input)}  Classifier Id
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrStorageGet(L7_uint32 UnitIndex, L7_uint32 clfrId,
                                 L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Get the row status for the specified Classifier Table row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    clfrId      @b{(input)}  Classifier Id
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrStatusGet(L7_uint32 UnitIndex, L7_uint32 clfrId,
                                   L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);


/*
==================================
==================================
==================================

   CLASSIFIER ELEMENT TABLE API

==================================
==================================
==================================
*/

/*************************************************************************
* @purpose  Obtain the current value of the classifier element index
*           next variable
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    pClfrElemIndex  @b{(output)} Pointer to classifier element
*                                        index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pClfrElemIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrElemIndexNext(L7_uint32 UnitIndex, 
                                       L7_uint32 *pClfrElemIndex);

/*********************************************************************
* @purpose  Verify that a Classifier Element Table row exists for the 
*           specified Classifier Element Id.
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    clfrId      @b{(input)} Classifier Id
* @param    clfrElemId  @b{(input)} Classifier Element Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrElemGet(L7_uint32 UnitIndex, L7_uint32 clfrId, 
                                 L7_uint32 clfrElemId);

/*********************************************************************
* @purpose  Determine next sequential row in the Classifier Element Table
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    prevClfrId      @b{(input)}  Classifier Id to begin search
* @param    prevClfrElemId  @b{(input)}  Classifier Element Id to begin search
* @param    pClfrId         @b{(output)} Pointer to next sequential classifier 
*                                        Id.
* @param    pClfrElemId     @b{(output)} Pointer to next sequential classifier 
*                                        Element Id.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevClfrElemId value of 0 is used to find the first classifier
*           element entry in the table.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrElemGetNext(L7_uint32 UnitIndex,
                                     L7_uint32 prevClfrId,
                                     L7_uint32 prevClfrElemId,
                                     L7_uint32 *pClfrId,
                                     L7_uint32 *pClfrElemId);

/*********************************************************************
* @purpose  Get the precedence for the specified Classifier Element Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    clfrId      @b{(input)} Classifier Id
* @param    clfrElemId  @b{(input)}  Classifier Element Id
* @param    pValue      @b{(output)} Pointer to precendence value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrElemPrecedenceGet(L7_uint32 UnitIndex, 
                                           L7_uint32 clfrId,
                                           L7_uint32 clfrElemId,
                                           L7_uint32 *pValue);

/*********************************************************************
* @purpose  Get the next functional data path element  for the specified 
*           Classifier Element Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    clfrId      @b{(input)} Classifier Id
* @param    clfrElemId  @b{(input)}  Classifier Element Id
* @param    pRowPtr     @b{(output)} Pointer to the next data path element 
*                                    entry
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrElemNextGet(L7_uint32 UnitIndex,
                                     L7_uint32 clfrId,
                                     L7_uint32 clfrElemId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);

/*********************************************************************
* @purpose  Get the specific(valid) entry in another table for the specified 
*           Classifier Element Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    clfrId      @b{(input)}  Classifier Id
* @param    clfrElemId  @b{(input)}  Classifier Element Id
* @param    pRowPtr     @b{(output)} Pointer to specific entry
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrElemSpecificGet(L7_uint32 UnitIndex,
                                         L7_uint32 clfrId,
                                         L7_uint32 clfrElemId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);

/*********************************************************************
* @purpose  Get the storage type for the specified Classifier Element Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    clfrId      @b{(input)}  Classifier Id
* @param    clfrElemId  @b{(input)}  Classifier Element Id
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrElemStorageGet(L7_uint32 UnitIndex, 
                                        L7_uint32 clfrId,
                                        L7_uint32 clfrElemId,
                                  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Get the row status for the specified Classifier Element Table row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    clfrId      @b{(input)}  Classifier Id
* @param    clfrElemId  @b{(input)}  Classifier Element Id
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClfrElemStatusGet(L7_uint32 UnitIndex,
                                       L7_uint32 clfrId,
                                       L7_uint32 clfrElemId,
                                  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*
=========================================
=========================================
=========================================

   IP MULTI-FIELD CLASSIFIER TABLE API

=========================================
=========================================
=========================================
*/

/*************************************************************************
* @purpose  Obtain the current value of the multifield classifier index
*           next variable
*
* @param    UnitIndex     @b{(input)}  System unit number
* @param    pMFClfrIndex  @b{(output)} Pointer to multifield classifier
*                                      index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pMFClfrIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrIndexNext(L7_uint32 UnitIndex, 
                                     L7_uint32 *pMFClfrIndex);

/*********************************************************************
* @purpose  Verify that a IP Multi Field Classifier Table row exists for the 
*           specified Multi Field Clfr Id.
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    mfClfrId    @b{(input)} Multi field Classifier Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrGet(L7_uint32 UnitIndex, L7_uint32 mfClfrId);

/*********************************************************************
* @purpose  Determine next sequential row in the IP Multi Field Classifier 
*           Table
*
* @param    UnitIndex     @b{(input)}  System unit number
* @param    prevMfClfrId  @b{(input)}  Multi field clfr id to begin search
* @param    pMfClfrId     @b{(output)} Pointer to next sequential multi field
*                                      clfr id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A pMfClfrId value of 0 is used to find the first multi field 
*           classifier entry in the table.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrGetNext(L7_uint32 UnitIndex, 
                                   L7_uint32 prevMfClfrId,
                                   L7_uint32 *pMfClfrId);

/*********************************************************************
* @purpose  Get the address type for the specified  Multi field classifier
*           Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pAddrType   @b{(output)} Pointer to the address type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrAddrTypeGet(L7_uint32 UnitIndex, 
                                       L7_uint32 mfClfrId,
          L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_t *pAddrType);

/*********************************************************************
* @purpose  Get the IP address to match against the packet's dest IP address
*           for the specified  Multi field classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pIpAddr     @b{(output)} Pointer to the destination address value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrDstAddrGet(L7_uint32 UnitIndex, 
                                      L7_uint32 mfClfrId,
                                      L7_uint32 *pIpAddr);

/*********************************************************************
* @purpose  Get the length of the CIDR prefix carried in Dst Addr field for 
*           the specified  Multi Field Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pPrefixLen  @b{(output)} Pointer to the prefix length value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrDstPrefixLenGet(L7_uint32 UnitIndex, 
                                           L7_uint32 mfClfrId,
                                           L7_uint32 *pPrefixLen);

/*********************************************************************
* @purpose  Get the IP address to match against the packet's source IP
*           addr for the specified  Multi Field Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pIpAddr     @b{(output)} Pointer to the src addr value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrSrcAddrGet(L7_uint32 UnitIndex, 
                                      L7_uint32 mfClfrId,
                                      L7_uint32 *pIpAddr);

/*********************************************************************
* @purpose  Get the length of the CIDR prefix carried in Src Addr field for 
*           the specified Multi Field Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pPrefixLen  @b{(output)} Pointer to the prefix length value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrSrcPrefixLenGet(L7_uint32 UnitIndex, 
                                           L7_uint32 mfClfrId,
                                           L7_uint32 *pPrefixLen);

/*********************************************************************
* @purpose  Get the matched DSCP value for the specified  Multi Field 
*           Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pDscpVal    @b{(output)} Pointer to the DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    MIB uses -1 to indicate 'match any' DSCP value
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrDscpGet(L7_uint32 UnitIndex, 
                                   L7_uint32 mfClfrId,
                                   L7_int32 *pDscpVal);

/*********************************************************************
* @purpose  Get flow identifier in an IPv6 header for the specified  Multi 
*           Field Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi Field Classifier Id
* @param    pFlowId     @b{(output)} Pointer to flow id value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED   
*
* @notes    always returns L7_NOT_SUPPORTED 
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrFlowIdGet(L7_uint32 UnitIndex, 
                                     L7_uint32 mfClfrId,
                                     L7_uint32 *pFlowId);

/*********************************************************************
* @purpose  Get the matched IP protocol number for the specified  Multi Field 
*           Classifier Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    mfClfrId     @b{(input)}  Multi Field Classifier Id
* @param    pProtocolNum @b{(output)} Pointer to the IP protocol value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    MIB uses 255 to indicate 'match any' protocol number
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrProtocolGet(L7_uint32 UnitIndex, 
                                       L7_uint32 mfClfrId,
                                       L7_uint32 *pProtocolNum);

/*********************************************************************
* @purpose  Get the minimum layer-4 destination port value for the specified  
*           Multi Field Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pPortNum    @b{(output)} Pointer to the port value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrDstL4PortMinGet(L7_uint32 UnitIndex, 
                                           L7_uint32 mfClfrId,
                                           L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Get the maximum layer-4 destination port value for the specified  
*           Multi Field Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi Field Classifier Id
* @param    pPortNum    @b{(output)} Pointer to the port value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrDstL4PortMaxGet(L7_uint32 UnitIndex, 
                                           L7_uint32 mfClfrId,
                                           L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Get the minimum layer-4 source port value for the specified  
*           Multi Field Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pPortNum    @b{(output)} Pointer to the port value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrSrcL4PortMinGet(L7_uint32 UnitIndex, 
                                           L7_uint32 mfClfrId,
                                           L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Get the maximum layer-4 source port value for the specified  
*           Multi Field Classifier Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pPortNum    @b{(output)} Pointer to the port value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrSrcL4PortMaxGet(L7_uint32 UnitIndex, 
                                           L7_uint32 mfClfrId,
                                           L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Get the storage type for the specified Multi Field Classifier 
*           Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrStorageGet(L7_uint32 UnitIndex, 
                                      L7_uint32 mfClfrId,
                                      L7_uint32 *pValue);

/*********************************************************************
* @purpose  Get the row status for the specified Multi Field Classifier 
*           Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMFClfrStatusGet(L7_uint32 UnitIndex, 
                                     L7_uint32 mfClfrId,
                                  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*
==============================================
==============================================
==============================================

   AUXILIARY MULTI-FIELD CLASSIFIER TABLE API

==============================================
==============================================
==============================================
*/

/*************************************************************************
* @purpose  Obtain the current value of the Auxiliary Multifield 
*           Classifier index next variable
*
* @param    UnitIndex        @b{(input)}  System unit number
* @param    pAuxMFClfrIndex  @b{(output)} Pointer to Auxiliary Multifield
*                                         Classifier index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pAuxMFClfrIndex value
*           of 0 means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrIndexNext(L7_uint32 UnitIndex, 
                                        L7_uint32 *pAuxMFClfrIndex);

/*********************************************************************
* @purpose  Verify that an Auxiliary Multi Field Classifier Table row exists 
*           for the specified Id.
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    auxMFClfrId @b{(input)} Auxiliary Multi field Classifier Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrGet(L7_uint32 UnitIndex, L7_uint32 auxMFClfrId);

/*********************************************************************
* @purpose  Determine next sequential row in the IP Multi Field Classifier 
*           Table
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    prevAuxMfClfrId @b{(input)}  Auxiliary Multi field clfr id to 
*                                        begin search
* @param    pAuxMfClfrId    @b{(output)} Pointer to next sequential aux multi 
*                                        field clfr id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A pAuxMfClfrId value of 0 is used to find the first auxiliary multi 
*           field classifier entry in the table.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrGetNext(L7_uint32 UnitIndex,  
                                      L7_uint32 prevAuxMfClfrId,
                                      L7_uint32 *pAuxMfClfrId);

/*********************************************************************
* @purpose  Get the destination IP address match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pIpAddr     @b{(output)} Pointer to the destination address value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrDstAddrGet(L7_uint32 UnitIndex,       
                                         L7_uint32 auxMFClfrId,
                                         L7_uint32 *pIpAddr);

/*********************************************************************
* @purpose  Get the destination IP address mask match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pIPMask     @b{(output)} Pointer to the mask value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrDstMaskGet(L7_uint32 UnitIndex, 
                                         L7_uint32 auxMFClfrId,
                                         L7_uint32 *pIpMask);

/*********************************************************************
* @purpose  Get the source IP address match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pIpAddr     @b{(output)} Pointer to the src addr value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrSrcAddrGet(L7_uint32 UnitIndex, 
                                         L7_uint32 auxMFClfrId,
                                         L7_uint32 *pIpAddr);

/*********************************************************************
* @purpose  Get the source IP address mask match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pIpMask     @b{(output)} Pointer to the mask value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrSrcMaskGet(L7_uint32 UnitIndex, 
                                         L7_uint32 auxMFClfrId,
                                         L7_uint32 *pIpMask);

/*********************************************************************
* @purpose  Get the IP protocol match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Multi Field Classifier Id
* @param    pProtocol   @b{(output)} Pointer to the IP protocol value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    MIB uses 255 to indicate 'match any' protocol number
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrProtocolGet(L7_uint32 UnitIndex, 
                                          L7_uint32 auxMFClfrId,
                                          L7_uint32 *pProtocolNum);

/*********************************************************************
* @purpose  Get the minimum layer-4 destination port match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pPortNum    @b{(output)} Pointer to the port value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrDstL4PortMinGet(L7_uint32 UnitIndex, 
                                              L7_uint32 auxMFClfrId,
                                              L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Get the maximum layer-4 destination port match value 
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi Field Classifier Id
* @param    pPortNum    @b{(output)} Pointer to the port value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrDstL4PortMaxGet(L7_uint32 UnitIndex, 
                                              L7_uint32 auxMFClfrId,
                                              L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Get the minimum layer-4 source port match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pPortNum    @b{(output)} Pointer to the port value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrSrcL4PortMinGet(L7_uint32 UnitIndex, 
                                              L7_uint32 auxMFClfrId,
                                              L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Get the maximum layer-4 source port match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pPortNum    @b{(output)} Pointer to the port value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrSrcL4PortMaxGet(L7_uint32 UnitIndex, 
                                              L7_uint32 auxMFClfrId,
                                              L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Get the Class of Service (COS) match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pCos        @b{(output)} Pointer to COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
*
* @notes    The Class of Service field is defined as the three-bit 
*           priority field in the first/outer 802.1Q header of a tagged
*           Ethernet frame. A value of -1 indicates that a specific COS value 
*           has not been defined and thus all such values are considered
*           a match.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrCosGet(L7_uint32 UnitIndex, 
                                     L7_uint32 auxMFClfrId,
                                     L7_int32 *pCos);

/*********************************************************************
* @purpose  Get the Secondary Class of Service (COS2) match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pCos        @b{(output)} Pointer to COS2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
*
* @notes    The Secondary Class of Service field is defined as the three-bit 
*           priority field in the second/inner 802.1Q header of a double VLAN 
*           tagged Ethernet frame. A value of -1 indicates that a specific 
*           Secondary COS value has not been defined and thus all such values
*           are considered a match.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrCos2Get(L7_uint32 UnitIndex, 
                                      L7_uint32 auxMFClfrId,
                                      L7_int32 *pCos);

/*********************************************************************
* @purpose  Get the Ethertype match value #1
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pEtypeVal   @b{(output)} Pointer to the Ethertype value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    Some Ethernet protocols are defined by two Ethertype values.
*           This object represents the first (or only) value.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrEtypeVal1Get(L7_uint32 UnitIndex, 
                                           L7_uint32 auxMFClfrId,
                                           L7_uint32 *pEtypeVal);

/*********************************************************************
* @purpose  Get the Ethertype match value #2
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pEtypeVal   @b{(output)} Pointer to the Ethertype value #2 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    Some Ethernet protocols are defined by two Ethertype values.
*           This object represents the second value, with a value of zero
*           indicating this object is not being used by the classifier.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrEtypeVal2Get(L7_uint32 UnitIndex, 
                                           L7_uint32 auxMFClfrId,
                                           L7_uint32  *pEtypeVal);

/*********************************************************************
* @purpose  Get the IP TOS bits match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pTos        @b{(output)} Pointer to TOS bits value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrTosGet(L7_uint32 UnitIndex, 
                                     L7_uint32 auxMFClfrId,
                                     L7_uchar8 *pTos);

/*********************************************************************
* @purpose  Get the IP TOS bits mask match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pTosMask    @b{(output)} Pointer to IP TOS bits mask value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
* 
* @notes    A valid IP TOS match condition requires both the TOS bits 
*           and mask values.  This can be used as a "free form" TOS
*           match specification.
*
* @notes    A mask value of '0' indicatesd a match of any TOS value.  
*         
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrTosMaskGet(L7_uint32 UnitIndex, 
                                         L7_uint32 auxMFClfrId,
                                         L7_uchar8 *pTosMask);

/*********************************************************************
* @purpose  Get the destination MAC address match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pMacAddrPtr @b{(output)} Pointer to MAC address
* @param    pLen        @b{(input/output)} Pointer to length of the MAC address
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type or buffer 
*                       length is too short
* @returns  L7_FAILURE  
*
* @notes    `pMacAddrPtr` points to a char buffer of minimum length equals to
*            L7_MAC_ADDR_LEN
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrDstMacGet(L7_uint32 UnitIndex, 
                                        L7_uint32 auxMFClfrId,
                                        L7_uchar8 *pMacAddrPtr,
                                        L7_uint32 *pLen);

/*********************************************************************
* @purpose  Get the destination MAC address mask match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pMacMaskPtr @b{(output)} Pointer to MAC mask string
* @param    pLen        @b{(input/output)} Pointer to length of the string
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type or buffer 
*                       length is too short
* @returns  L7_FAILURE  
*
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
*
* @notes    `pMacMaskPtr` points to a char buffer of minimum length equals to
*            L7_MAC_ADDR_LEN
*
* @end

* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrDstMacMaskGet(L7_uint32 UnitIndex,
                                            L7_uint32 auxMFClfrId,
                                            L7_uchar8 *pMacMaskPtr,
                                            L7_uint32 *pLen);

/*********************************************************************
* @purpose  Get the source MAC address match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pMacAddrPtr @b{(output)} Pointer to MAC address string
* @param    pLen        @b{(input/output)} Pointer to length of the MAC address
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type or buffer 
*                       length is too short
* @returns  L7_FAILURE  
*
* @notes    `pMacAddrPtr` points to a char buffer of minimum length equals to
*            L7_MAC_ADDR_LEN
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrSrcMacGet(L7_uint32 UnitIndex, 
                                        L7_uint32 auxMFClfrId,
                                        L7_uchar8 *pMacAddrPtr,
                                        L7_uint32 *pLen);

/*********************************************************************
* @purpose  Get the source MAC address mask match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pMacMaskPtr @b{(output)} Pointer to MAC mask string
* @param    pLen        @b{(input/output)} Pointer to length of the string
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type or buffer 
*                       length is too short
* @returns  L7_FAILURE  
*
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
*
* @notes    `pMacMaskPtr` points to a char buffer of minimum length equals to
*            L7_MAC_ADDR_LEN
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrSrcMacMaskGet(L7_uint32 UnitIndex, 
                                            L7_uint32 auxMFClfrId,
                                            L7_uchar8 *pMacMaskPtr,
                                            L7_uint32 *pLen);

/*********************************************************************
* @purpose  Get the minimum Vlan Id match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pVlanId     @b{(output)} Pointer to the value of Vlan Id
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    The VLAN ID is defined as the 12-bit VLAN identifier in 
*           the first/outer 802.1Q header of a tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrVlanIdMinGet(L7_uint32 UnitIndex, 
                                           L7_uint32 auxMFClfrId,
                                           L7_uint32  *pVlanId);

/*********************************************************************
* @purpose  Get the maximum Vlan Id match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pVlanId     @b{(output)} Pointer to the value of Vlan Id
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    The VLAN ID is defined as the 12-bit VLAN identifier in 
*           the first/outer 802.1Q header of a tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrVlanIdMaxGet(L7_uint32 UnitIndex, 
                                           L7_uint32 auxMFClfrId,
                                           L7_uint32  *pVlanId);

/*********************************************************************
* @purpose  Get the minimum Secondary VLAN Id (VLAN Id2) match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pVlanId     @b{(output)} Pointer to the value of VLAN Id2
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    The Secondary ID is defined as the 12-bit VLAN identifier in 
*           the second/inner 802.1Q header of a double VLAN tagged Ethernet
*           frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrVlanId2MinGet(L7_uint32 UnitIndex, 
                                            L7_uint32 auxMFClfrId,
                                            L7_uint32  *pVlanId);

/*********************************************************************
* @purpose  Get the maximum Secondary VLAN Id (VLAN Id2) match value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pVlanId     @b{(output)} Pointer to the value of VLAN Id2
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    The Secondary VLAN ID is defined as the 12-bit VLAN identifier in 
*           the second/inner 802.1Q header of a double VLAN tagged Ethernet
*           frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrVlanId2MaxGet(L7_uint32 UnitIndex, 
                                            L7_uint32 auxMFClfrId,
                                            L7_uint32  *pVlanId);

/*********************************************************************
* @purpose  Get the storage type value of the specified Auxiliary Multi Filed
*           Classifier row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrStorageGet(L7_uint32 UnitIndex, 
                                         L7_uint32 auxMFClfrId,
                                         L7_uint32 *pValue);

/*********************************************************************
* @purpose  Get the row status value of the specified Auxiliary Multi Filed
*           Classifier row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAuxMFClfrStatusGet(L7_uint32 UnitIndex, 
                                        L7_uint32 auxMFClfrId,
                                  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);
/*
=====================
=====================
=====================

   METER TABLE API

=====================
=====================
=====================
*/

/*************************************************************************
* @purpose  Obtain the current value of the meter index next variable
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    pMeterIndex  @b{(output)} Pointer to meter index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pMeterIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMeterIndexNext(L7_uint32 UnitIndex, 
                                    L7_uint32 *pMeterIndex);

/*********************************************************************
* @purpose  Verify that a Meter Table row exists for the specified 
*           Meter Id
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    meterId     @b{(input)} Meter Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMeterGet(L7_uint32 UnitIndex, L7_uint32 meterId);

/*********************************************************************
* @purpose  Determine next sequential row in the Meter Table
*
* @param    UnitIndex   @b{(input)} System unit number                       
* @param    prevMeterId @b{(input)}  Meter Id to begin search
* @param    pMeterId    @b{(output)} Pointer to next sequential
*                                    Meter Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevMeterId value of 0 is used to find the first Meter 
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMeterGetNext(L7_uint32 UnitIndex, 
                                  L7_uint32 prevMeterId,
                                  L7_uint32 *pMeterId);

/*********************************************************************
* @purpose  Get the pointer to the next data path element(if traffic does 
*           conform) for the specified Meter Table Row
*
* @param    UnitIndex  @b{(input)}  System unit number
* @param    meterId    @b{(input)}  Meter Id
* @param    pRowPtr    @b{(output)} Pointer to the next data path element
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMeterSucceedNextGet(L7_uint32 UnitIndex, 
                                         L7_uint32 meterId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);

/*********************************************************************
* @purpose  Get the pointer to the next data path element(if traffic does 
*           not conform) for the specified Meter Table Row
*
* @param    UnitIndex  @b{(input)}  System unit number
* @param    meterId    @b{(input)}  Meter Id
* @param    pRowPtr    @b{(output)} Pointer to the next data path element
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMeterFailNextGet(L7_uint32 UnitIndex, 
                                      L7_uint32 meterId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);
                            
/*********************************************************************
* @purpose  Get the specific data path element for the specified Meter 
*           Table Row
*
* @param    UnitIndex  @b{(input)}  System unit number
* @param    meterId    @b{(input)}  Meter Id
* @param    pRowPtr    @b{(output)} Pointer to the specific data path element
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMeterSpecificGet(L7_uint32 UnitIndex, 
                                      L7_uint32 meterId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);
                            
/*********************************************************************
* @purpose  Get the storage type for the specified Meter Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    meterId     @b{(input)}  Meter Id
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMeterStorageGet(L7_uint32 UnitIndex, L7_uint32 meterId,
                                     L7_uint32 *pValue);
                            
/*********************************************************************
* @purpose  Get the row status for the specified Meter Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    meterId     @b{(input)}  Meter Id
* @param    pValue      @b{(output)} Pointer to the row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMeterStatusGet(L7_uint32 UnitIndex, L7_uint32 meterId,
                                 L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);


/*
======================================
======================================
======================================

   TOKEN BUCKET PARAMETER TABLE API

======================================
======================================
======================================
*/
/*************************************************************************
* @purpose  Obtain the current value of the TB param index next variable
*
* @param    UnitIndex      @b{(input)}  System unit number
* @param    pTBParamIndex  @b{(output)} Pointer to TB param index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pTBParamIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServTBParamIndexNext(L7_uint32 UnitIndex, 
                                      L7_uint32 *pTBParamIndex);

/*********************************************************************
* @purpose  Verify that a Token Bucket Parameter Table row exists for the 
*           specified TB Parameter Id
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    tbParamId   @b{(input)} TB Parameter Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServTBParamGet(L7_uint32 UnitIndex, L7_uint32 tbParamId);

/*********************************************************************
* @purpose  Determine next sequential row in the Token Bucket Parameter Table 
*
* @param    UnitIndex     @b{(input)}  System unit number                     
* @param    prevTbParamId @b{(input)}  TB Parameter Id to begin search
* @param    pTbParamId    @b{(output)} Pointer to next sequential
*                                      TB Parameter Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevTbParamId value of 0 is used to find the first TB parameter 
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServTBParamGetNext(L7_uint32 UnitIndex, 
                                    L7_uint32 prevTbParamId,
                                    L7_uint32 *pTbParamId);

/*********************************************************************
* @purpose  Get the metering algorithm type for the specified Token Bucket 
*           Parameter Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    tbParamId    @b{(input)}  TB Parameter Id
* @param    pTbMeterType @b{(output)} Pointer to the meter type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServTBParamTypeGet(L7_uint32 UnitIndex, 
                                    L7_uint32 tbParamId,
                    L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_t *pTbMeterType);
                            
/*********************************************************************
* @purpose  Get the token-bukcet rate(in kbps) for the specified Token Bucket 
*           Parameter Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    tbParamId    @b{(input)}  TB Parameter Id
* @param    pRate        @b{(output)} Pointer to the rate value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServTBParamRateGet(L7_uint32 UnitIndex, 
                                    L7_uint32 tbParamId,
                                    L7_uint32 *pRate);

/*********************************************************************
* @purpose  Get the burst size for the specified Token Bucket 
*           Parameter Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    tbParamId    @b{(input)}  TB Parameter Id
* @param    pBurst       @b{(output)} Pointer to the burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServTBParamBurstSizeGet(L7_uint32 UnitIndex, 
                                         L7_uint32 tbParamId,
                                         L7_uint32 *pBurst);

/*********************************************************************
* @purpose  Get the time interval used for the specified Token Bucket 
*           Parameter Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    tbParamId    @b{(input)}  TB Parameter Id
* @param    pInterval    @b{(output)} Pointer to the time interval value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR     Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServTBParamIntervalGet(L7_uint32 UnitIndex, 
                                        L7_uint32 tbParamId,
                                        L7_uint32 *pInterval);

/*********************************************************************
* @purpose  Get the storage type for the specified Token Bucket Parameter 
*           Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    tbParamId   @b{(input)}  TB Parameter Id
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServTBParamStorageGet(L7_uint32 UnitIndex, 
                                       L7_uint32 tbParamId,
                                       L7_uint32 *pValue);

/*********************************************************************
* @purpose  Get the row status for the specified Token Bucket Parameter 
*           Table Row  
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    tbParamId   @b{(input)}  TB Parameter Id
* @param    pValue      @b{(output)} Pointer to the row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServTBParamStatusGet(L7_uint32 UnitIndex, 
                                      L7_uint32 tbParamId,
                                  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*
============================
============================
============================

   COLOR AWARE TABLE API

============================
============================
============================
*/

/*********************************************************************
* @purpose  Get the conformance level for the specified Color Aware Table Row
*
* @param    UnitIndex  @b{(input)}  System unit number
* @param    meterId    @b{(input)}  Meter Id
* @param    pLevel     @b{(output)} Pointer to the conformance level
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    Augments the Meter Table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServColorAwareLevelGet(L7_uint32 UnitIndex, 
                                        L7_uint32 meterId,
                    L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_t *pLevel);

/*********************************************************************
* @purpose  Get the color mode for the specified Color Aware Table Row
*
* @param    UnitIndex  @b{(input)}  System unit number
* @param    meterId    @b{(input)}  Meter Id
* @param    pMode      @b{(output)} Pointer to the color mode value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    Augments the Meter Table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServColorAwareModeGet(L7_uint32 UnitIndex, 
                                       L7_uint32 meterId,
                    L7_USMDB_MIB_DIFFSERV_COLOR_MODE_t *pMode);

/*********************************************************************
* @purpose  Get the color match value for the specified Color Aware Table Row
*
* @param    UnitIndex  @b{(input)}  System unit number
* @param    meterId    @b{(input)}  Meter Id
* @param    pValue     @b{(output)} Pointer to the color match value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    Augments the Meter Table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServColorAwareValueGet(L7_uint32 UnitIndex, 
                                        L7_uint32 meterId,
                                        L7_uint32 *pValue);

/*
======================
======================
======================

   ACTION TABLE API

======================
======================
======================
*/
/*************************************************************************
* @purpose  Obtain the current value of the action index next variable
*
* @param    UnitIndex     @b{(input)}  System unit number
* @param    pActionIndex  @b{(output)} Pointer to action index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pActionIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServActionIndexNext(L7_uint32 UnitIndex, 
                                     L7_uint32 *pActionIndex);

/*********************************************************************
* @purpose  Verify that an Action Table row exists for the specified Action id
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    actionId    @b{(input)} Action Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServActionGet(L7_uint32 UnitIndex, L7_uint32 actionId);

/*********************************************************************
* @purpose  Determine next sequential row in the Action Table
*
* @param    UnitIndex     @b{(input)}  System unit number
* @param    prevActionId  @b{(input)}  Action id to begin search
* @param    pActionId     @b{(output)} Pointer to next sequential
*                                      Action id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevActionId value of 0 is used to find the first Action 
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServActionGetNext(L7_uint32 UnitIndex, 
                                   L7_uint32 prevActionId,
                                   L7_uint32 *pActionId);

/*********************************************************************
* @purpose  Get the internal interface index for the specified Action 
*           table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    actionId    @b{(input)}  Action Id
* @param    pIntIfNum   @b{(output)} Pointer to the internal interface 
*                                    index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServActionInterfaceGet(L7_uint32 UnitIndex, 
                                        L7_uint32 actionId,
                                        L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the next data path element for the specified Action 
*           table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    actionId    @b{(input)}  Action Id
* @param    pRowPtr     @b{(output)} Pointer to the specific data path element
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServActionNextGet(L7_uint32 UnitIndex, 
                                   L7_uint32 actionId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);

/*********************************************************************
* @purpose  Get the specific data path entry for the specified Action 
*           table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    actionId    @b{(input)}  Action Id
* @param    pRowPtr     @b{(output)} Pointer to the specific data path element
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServActionSpecificGet(L7_uint32 UnitIndex, 
                                       L7_uint32 actionId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);
                            
/*********************************************************************
* @purpose  Get the storage type for the specified Action table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    actionId    @b{(input)}  Action Id
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServActionStorageGet(L7_uint32 UnitIndex, 
                                      L7_uint32 actionId,
                                      L7_uint32 *pValue);

/*********************************************************************
* @purpose  Get the row status for the specified Action Table Row  
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    actionId    @b{(input)}  Action Id
* @param    pValue      @b{(output)} Pointer to the row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServActionStatusGet(L7_uint32 UnitIndex, 
                                     L7_uint32 actionId,
                                  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*
================================
================================
================================

   COS MARK ACTION TABLE API

================================
================================
================================
*/

/*********************************************************************
* @purpose  Verify that a COS Mark Action Table row exists for the 
*           specified COS value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    cos         @b{(input)} COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if cos 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCosMarkActGet(L7_uint32 UnitIndex, L7_uint32 cos);

/*********************************************************************
* @purpose  Determine next sequential row in the COS Mark Action Table
*
* @param    UnitIndex        @b{(input)}  System unit number                                               
* @param    prevCos          @b{(input)}  COS value to begin search
* @param    cos              @b{(output)} Pointer to next sequential
*                                         COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevCosMarkAct of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCosMarkActGetNext(L7_uint32 UnitIndex, 
                                       L7_uint32 prevCos,
                                       L7_uint32 *pCos);

/*********************************************************************
* @purpose  Get COS value for the specified COS mark action table row index
*
* @param    UnitIndex @b{(input)}  System unit number                                               
* @param    cos       @b{(input)}  Index COS value
* @param    pCos      @b{(output)} Pointer to the output COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCosMarkActCosGet(L7_uint32 UnitIndex, L7_uint32 cos,
                                      L7_uint32 *pCos);

/*
================================================
================================================
================================================

   SECONDARY COS (COS2) MARK ACTION TABLE API

================================================
================================================
================================================
*/
 
/*********************************************************************
* @purpose  Verify that a Secondary COS Mark Action Table row
*           exists for the specified COS value
*
* @param    UnitIndex @b{(input)} System unit number
* @param    cos       @b{(input)} COS2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    Must use this function to determine if secondary cos 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCos2MarkActGet(L7_uint32 UnitIndex, L7_uint32 cos);

/*********************************************************************
* @purpose  Determine next sequential row in the Secondary COS Mark Action Table
*
* @param    UnitIndex   @b{(input)}  System unit number                                               
* @param    prevCos     @b{(input)}  COS2 value to begin search
* @param    pCos        @b{(output)} Pointer to next sequential COS2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevCos of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCos2MarkActGetNext(L7_uint32 UnitIndex, 
                                        L7_uint32 prevCos,
                                        L7_uint32 *pCos);

/*********************************************************************
* @purpose  Get COS value for the specified Secondary COS mark action table
*           row index
*
* @param    UnitIndex @b{(input)}  System unit number                                               
* @param    cos       @b{(input)}  Index COS2 value
* @param    pCos      @b{(output)} Pointer to the output COS2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCos2MarkActCosGet(L7_uint32 UnitIndex, 
                                       L7_uint32 cos,
                                       L7_uint32 *pCos);

/*
================================
================================
================================

   DSCP MARK ACTION TABLE API

================================
================================
================================
*/
 
/*********************************************************************
* @purpose  Verify that a DSCP Mark Action Table row exists for the 
*           specified DSCP value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    dscp        @b{(input)} DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if dscp 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServDscpMarkActGet(L7_uint32 UnitIndex, L7_uint32 dscp);

/*********************************************************************
* @purpose  Determine next sequential row in the DSCP Mark Action Table
*
* @param    UnitIndex @b{(input)}  System unit number                                               
* @param    prevDscp  @b{(input)}  DSCP value to begin search
* @param    pDscp     @b{(output)} Pointer to next sequential DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A pDscp of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServDscpMarkActGetNext(L7_uint32 UnitIndex, 
                                        L7_uint32 prevDscp,
                                        L7_uint32 *pDscp);

/*********************************************************************
* @purpose  Get DSCP value for the specified DSCP mark action table row index
*
* @param    UnitIndex    @b{(input)}  System unit number                                               
* @param    dscp         @b{(input)}  Index DSCP value
* @param    pDscp        @b{(output)} Pointer to the ouput DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServDscpMarkActDscpGet(L7_uint32 UnitIndex, L7_uint32 dscp,
                                        L7_uint32 *pDscp);

/*
=======================================
=======================================
=======================-===============

   IP PRECEDENCE  MARK ACTION TABLE API

=======================================
=======================================
=======================================
*/

/*********************************************************************
* @purpose  Verify that a IP Precedence Mark Action Table row exists for the 
*           specified DSCP value
*
* @param    UnitIndex    @b{(input)} System unit number
* @param    ipPrecedence @b{(input)} IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if ipPrecedence 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServIpPrecMarkActGet(L7_uint32 UnitIndex, 
                                      L7_uint32 ipPrecedence);

/*********************************************************************
* @purpose  Determine next sequential row in the IP Precedence Mark Action Table
*
* @param    UnitIndex        @b{(input)}  System unit number                                               
* @param    prevIpPrecedence @b{(input)}  Ip Precedence value to begin search
* @param    pIpPrecedence    @b{(output)} Pointer to next sequential
*                                         IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A pDscp of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServIpPrecMarkActGetNext(L7_uint32 UnitIndex, 
                                          L7_uint32 prevIpPrecedence,
                                          L7_uint32 *pIpPrecedence);

/*********************************************************************
* @purpose  Get IP Precedence value for the specified IP Precedence mark 
*           action table row index
*
* @param    UnitIndex     @b{(input)}  System unit number                                               
* @param    ipPrecedence  @b{(input)}  Index Ip Precedence value 
* @param    pIpPrecedence @b{(output)} Pointer to the output IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServIpPrecMarkActIpPrecedenceGet(L7_uint32 UnitIndex, 
                                                  L7_uint32 ipPrecedence,
                                                  L7_uint32 *pIpPrecedence);
/*
============================
============================
============================

   COUNT ACTION TABLE API

============================
============================
============================
*/
/*************************************************************************
* @purpose  Obtain the current value of count action index next variable
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    pCountActIndex  @b{(output)} Pointer to count action index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pCountActIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCountActIndexNext(L7_uint32 UnitIndex, 
                                       L7_uint32 *pCountActIndex);

/*********************************************************************
* @purpose  Verify that a Count Action Table row exists for the 
*           specified Count Action Id
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    countActId  @b{(input)} Count Action Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCountActGet(L7_uint32 UnitIndex, L7_uint32 countActId);

/*********************************************************************
* @purpose  Determine next sequential row in the Count Action Table
*
* @param    UnitIndex      @b{(input)}  System unit number                                               
* @param    prevCountActId @b{(input)}  Count Action Id to begin search
* @param    pCountActId    @b{(output)} Pointer to next sequential
*                                       Count Action Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevCountActId value of 0 is used to find the first Count 
*           Action Entry in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCountActGetNext(L7_uint32 UnitIndex, 
                                     L7_uint32 prevCountActId,
                                     L7_uint32 *pCountActId);

/*********************************************************************
* @purpose  Get the number of octets at the Action data path element
*           for the specified Count Action table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    countActId  @b{(input)}  Count Action Id
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCountActOctetsGet(L7_uint32 UnitIndex, 
                                       L7_uint32 countActId,
                                       L7_uint32 *pValueHi,
                                       L7_uint32 *pValueLo);

/*********************************************************************
* @purpose  Get the number of packets at the Action data path element
*           for the specified Count Action table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    countActId  @b{(input)}  Count Action Id
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCountActPacketsGet(L7_uint32 UnitIndex, 
                                        L7_uint32 countActId,
                                        L7_uint32 *pValueHi,
                                        L7_uint32 *pValueLo);
                            
/*********************************************************************
* @purpose  Get the storage type for the specified Count Action table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    CountActId   @b{(input)}  Count Action Id
* @param    pValue       @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCountActStorageGet(L7_uint32 UnitIndex, 
                                        L7_uint32 countActId,
                                        L7_uint32 *pValue);
                            
/*********************************************************************
* @purpose  Get the row status for the specified Count Action Table Row  
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    countActId  @b{(input)}  Count Action Id
* @param    pValue      @b{(output)} Pointer to the row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServCountActStatusGet(L7_uint32 UnitIndex, 
                                       L7_uint32 actionId,
                                 L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);


/*
============================
============================
============================

   ASSIGN QUEUE TABLE API

============================
============================
============================
*/

/*************************************************************************
* @purpose  Obtain the current value of the assign queue index next variable
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    pAssignIndex @b{(output)} Pointer to assign queue index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pAssignIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAssignQueueIndexNext(L7_uint32 UnitIndex, 
                                          L7_uint32 *pAssignIndex);

/*********************************************************************
* @purpose  Verify that an Assign Queue Table row exists for the specified 
*           Assign Index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    assignIndex @b{(input)} Assign Index
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAssignQueueGet(L7_uint32 UnitIndex, L7_uint32 assignIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Assign Queue Table
*
* @param    UnitIndex       @b{(input)}  System unit number                       
* @param    prevAssignIndex @b{(input)}  Assign Index to begin search
* @param    pAssignIndex    @b{(output)} Pointer to next sequential
*                                          Assign Index value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevAssignIndex value of 0 is used to find the first Assign 
*           Queue Entry in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAssignQueueGetNext(L7_uint32 UnitIndex, 
                                        L7_uint32 prevAssignIndex,
                                        L7_uint32 *pAssignIndex);

/*********************************************************************
* @purpose  Get the queue number for specified Assign Queue Table Row 
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    assignIndex @b{(input)}  Assign Index
* @param    pQnum       @b{(output)} Pointer to the queue number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAssignQueueQnumGet(L7_uint32 UnitIndex, 
                                        L7_uint32 assignIndex,
                                        L7_uint32 *pQnum);

/*********************************************************************
* @purpose  Get the storage type for the specified Assign Queue Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    assignIndex @b{(input)}  Assign Index
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAssignQueueStorageGet(L7_uint32 UnitIndex, 
                                           L7_uint32 assignIndex,
                                           L7_uint32 *pValue);

/*********************************************************************
* @purpose  Get the row status for the specified Assign Queue Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    assignIndex @b{(input)}  Assign Index
* @param    pValue      @b{(output)} Pointer to the row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAssignQueueStatusGet(L7_uint32 UnitIndex, 
                                          L7_uint32 assignIndex,
                                          L7_uint32 *pValue);

/*
========================
========================
========================

   REDIRECT TABLE API

========================
========================
========================
*/

/*************************************************************************
* @purpose  Obtain the current value of the redirect Id next variable
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    pRedirectIndex  @b{(output)} Pointer to redirect index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pRedirectIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServRedirectIndexNext(L7_uint32 UnitIndex, 
                                    L7_uint32 *pRedirectIndex);

/*********************************************************************
* @purpose  Verify that an Redirect Table row exists for the specified 
*           Redirect Id 
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    redirectId  @b{(input)} Redirect Id 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServRedirectGet(L7_uint32 UnitIndex, L7_uint32 redirectId);

/*********************************************************************
* @purpose  Determine next sequential row in the Redirect Table
*
* @param    UnitIndex       @b{(input)}  System unit number                       
* @param    prevRedirectId  @b{(input)}  Redirect Id to begin search
* @param    pRedirectId     @b{(output)} Pointer to next sequential
*                                          Redirect Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevRedirectId value of 0 is used to find the first Redirect
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServRedirectGetNext(L7_uint32 UnitIndex, 
                                     L7_uint32 prevRedirectId,
                                     L7_uint32 *pRedirectId);

/*********************************************************************
* @purpose  Get the interface number for specified Redirect Table Row 
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    redirectId  @b{(input)}  Redirect Id 
* @param    pIntIfNum   @b{(output)} Pointer to the interface number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServRedirectIntfGet(L7_uint32 UnitIndex, 
                                     L7_uint32 redirectId,
                                     L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the storage type for the specified Redirect Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    redirectId  @b{(input)}  Redirect Id 
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServRedirectStorageGet(L7_uint32 UnitIndex, 
                                        L7_uint32 redirectId,
                                        L7_uint32 *pValue);

/*********************************************************************
* @purpose  Get the row status for the specified Redirect Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    redirectId  @b{(input)}  Redirect Id 
* @param    pValue      @b{(output)} Pointer to the row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServRedirectStatusGet(L7_uint32 UnitIndex, 
                                       L7_uint32 redirectId,
                                       L7_uint32 *pValue);

/*
======================
======================
======================

   MIRROR TABLE API

======================
======================
======================
*/

/*************************************************************************
* @purpose  Obtain the current value of the mirror Id next variable
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    pMirrorIndex    @b{(output)} Pointer to mirror index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pMirrorIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMirrorIndexNext(L7_uint32 UnitIndex, 
                                     L7_uint32 *pMirrorIndex);

/*********************************************************************
* @purpose  Verify that an Mirror Table row exists for the specified 
*           Mirror Id 
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    mirrorId    @b{(input)} Mirror Id 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMirrorGet(L7_uint32 UnitIndex, L7_uint32 mirrorId);

/*********************************************************************
* @purpose  Determine next sequential row in the Mirror Table
*
* @param    UnitIndex       @b{(input)}  System unit number                       
* @param    prevMirrorId    @b{(input)}  Mirror Id to begin search
* @param    pMirrorId       @b{(output)} Pointer to next sequential
*                                          Mirror Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevMirrorId value of 0 is used to find the first Mirror
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMirrorGetNext(L7_uint32 UnitIndex, 
                                   L7_uint32 prevMirrorId,
                                   L7_uint32 *pMirrorId);

/*********************************************************************
* @purpose  Get the interface number for specified Mirror Table Row 
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mirrorId    @b{(input)}  Mirror Id 
* @param    pIntIfNum   @b{(output)} Pointer to the interface number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMirrorIntfGet(L7_uint32 UnitIndex, 
                                   L7_uint32 mirrorId,
                                   L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the storage type for the specified Mirror Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mirrorId    @b{(input)}  Mirror Id 
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMirrorStorageGet(L7_uint32 UnitIndex, 
                                      L7_uint32 mirrorId,
                                      L7_uint32 *pValue);

/*********************************************************************
* @purpose  Get the row status for the specified Mirror Table Row
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    mirrorId    @b{(input)}  Mirror Id 
* @param    pValue      @b{(output)} Pointer to the row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServMirrorStatusGet(L7_uint32 UnitIndex, 
                                     L7_uint32 mirrorId,
                                     L7_uint32 *pValue);

/*
================================
================================
================================

   ALGORITHMIC DROP TABLE API

================================
================================
================================
*/
/*************************************************************************
* @purpose  Obtain the current value of the algorithmic drop index next
*           variable
*
* @param    UnitIndex      @b{(input)}  System unit number
* @param    pAlgDropIndex  @b{(output)} Pointer to algorithmic drop
*                                       index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pAlgDropIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropIndexNext(L7_uint32 UnitIndex, 
                                      L7_uint32 *pAlgDropIndex);

/*********************************************************************
* @purpose  Verify that an Algorithmic Drop Table row exists for the 
*           specified Algorithmic Drop Id
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    algDropId   @b{(input)} Algorithmic Drop Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropGet(L7_uint32 UnitIndex, L7_uint32 algDropId);

/*********************************************************************
* @purpose  Determine next sequential row in the Algorithmic Drop Table
*
* @param    UnitIndex      @b{(input)}  System unit number                                               
* @param    prevAlgDropId  @b{(input)}  Algorithmic Drop Id to begin search
* @param    pAlgDropId     @b{(output)} Pointer to next sequential
*                                       Algorithmic Drop Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevAlgDropId value of 0 is used to find the first Algorithmic
*           Drop Entry in the table
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropGetNext(L7_uint32 UnitIndex, 
                                    L7_uint32 prevAlgDropId,
                                    L7_uint32 *pAlgDropId);

/*********************************************************************
* @purpose  Get the type of the algorithm used at this dropper for the 
*           specified Algorithmic Drop Table Row
*
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    algDropId    @b{(input)}  Algorithmic Drop Id
* @param    pAlgDropType @b{(output)} Pointer to algorithm type value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropTypeGet(L7_uint32 UnitIndex, 
                                    L7_uint32 algDropId,
          L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_t *pAlgDropType);
                            
/*********************************************************************
* @purpose  Get the next data path element for the specified Algorithmic 
*           Drop Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    algDropId    @b{(input)}  Algorithmic Drop Id
* @param    pRowPtr      @b{(output)} Pointer to the next data path element 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropNextGet(L7_uint32 UnitIndex, 
                                    L7_uint32 algDropId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);

/*********************************************************************
* @purpose  Get an entry in the Q table for the specified Algorithmic 
*           Drop Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    algDropId    @b{(input)}  Algorithmic Drop Id
* @param    pRowPtr      @b{(output)} Pointer to an entry in the Q table 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropQMeasureGet(L7_uint32 UnitIndex, 
                                        L7_uint32 algDropId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);
                            
/*********************************************************************
* @purpose  Get the threshold of the queue for the specified Algorithmic 
*           Drop Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    algDropId    @b{(input)}  Algorithmic Drop Id
* @param    pValue       @b{(output)} Pointer to the threshold value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropQThresholdGet(L7_uint32 UnitIndex, 
                                          L7_uint32 algDropId,
                                          L7_uint32 *pValue);
                            
/*********************************************************************
* @purpose  Get the specific data path element for the specified Algorithmic 
*           Drop Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    algDropId    @b{(input)}  Algorithmic Drop Id
* @param    pRowPtr      @b{(output)} Pointer to the specific data path element
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropSpecificGet(L7_uint32 UnitIndex, 
                                        L7_uint32 algDropId,
                    L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *pRowPtr);
                            
/*********************************************************************
* @purpose  Get the number of octets that have been dropped for the specified 
*           Algorithmic Drop Table Row
*
* @param    UnitIndex    @b{(input)}   System unit number
* @param    algDropId    @b{(input)}   Algorithmic Drop Id
* @param    pValueHi     @b{(output)}  Pointer to counter value (upper 32-bits)
* @param    pValueLo     @b{(output)}  Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropOctetsGet(L7_uint32 UnitIndex, 
                                      L7_uint32 algDropId,
                                      L7_uint32 *pValueHi,
                                      L7_uint32 *pValueLo);
                            
/*********************************************************************
* @purpose  Get the number of packets that have been dropped for the specified 
*           Algorithmic Drop Table Row
*
* @param    UnitIndex    @b{(input)}   System unit number
* @param    algDropId    @b{(input)}   Algorithmic Drop Id
* @param    pValueHi     @b{(output)}  Pointer to counter value (upper 32-bits)
* @param    pValueLo     @b{(output)}  Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropPacketsGet(L7_uint32 UnitIndex, 
                                       L7_uint32 algDropId,
                                       L7_uint32 *pValueHi,
                                       L7_uint32 *pValueLo);
                            
/*********************************************************************
* @purpose  Get the number of octets that have been randomly dropped for 
*           the specified Algorithmic Drop Table Row
*
* @param    UnitIndex    @b{(input)}   System unit number
* @param    algDropId    @b{(input)}   Algorithmic Drop Id
* @param    pValueHi     @b{(output)}  Pointer to counter value (upper 32-bits)
* @param    pValueLo     @b{(output)}  Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgRandomDropOctetsGet(L7_uint32 UnitIndex, 
                                            L7_uint32 algDropId,
                                            L7_uint32 *pValueHi,
                                            L7_uint32 *pValueLo);
                            
/*********************************************************************
* @purpose  Get the number of packets that have been randomly dropped for 
*           the specified Algorithmic Drop Table Row
*
* @param    UnitIndex    @b{(input)}   System unit number
* @param    algDropId    @b{(input)}   Algorithmic Drop Id
* @param    pValueHi     @b{(output)}  Pointer to counter value (upper 32-bits)
* @param    pValueLo     @b{(output)}  Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgRandomDropPacketsGet(L7_uint32 UnitIndex, 
                                             L7_uint32 algDropId,
                                             L7_uint32 *pValueHi,
                                             L7_uint32 *pValueLo);

/*********************************************************************
* @purpose  Get the storage type for the specified Algorithmic Drop 
*           Table Row
*
* @param    UnitIndex    @b{(input)}  System unit number
* @param    algDropId    @b{(input)}  Algorithmic Drop Id
* @param    pValue       @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropStorageGet(L7_uint32 UnitIndex, 
                                       L7_uint32 algDropId,
                                       L7_uint32 *pValue);
                            
/*********************************************************************
* @purpose  Get the row status for the specified Algorithmic Drop Table Row  
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    algDropId   @b{(input)}  Algorithmic Drop Id
* @param    pValue      @b{(output)} Pointer to the row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServAlgDropStatusGet(L7_uint32 UnitIndex, 
                                      L7_uint32 algDropId,
                                  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);


/*********************************************************************
*
* @purpose check to see if intIfNum is a valid diffserv Interface
*
* @param    UnitIndex    @b{(input)} System unit number
* @param    intIfNum     @b((input)) Interface Number
*
* @returns L7_TRUE     If valid interface
* @returns L7_FALSE    If not valid interface 
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_BOOL usmDbQosDiffServIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);


#endif /* USMDB_MIB_DIFFSERV_API_H */

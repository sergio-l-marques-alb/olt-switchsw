/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_trace.h
*
* @purpose    DiffServ component trace support header
*
* @component  diffserv
*
* @comments   none
*
* @create     10/25/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/

#ifndef INCLUDE_DIFFSERV_TRACE_H
#define INCLUDE_DIFFSERV_TRACE_H

#include "l7_common.h"
#include "trace_api.h"
    

/* trace definitions */
#define DS_TRACE_ENTRY_MAX                  512                                  
#define DS_TRACE_ENTRY_SIZE_MAX             36
#define DS_TRACE_REGISTRATION_NAME          "DiffServ"                           
#define DS_TRACE_OBJ_VAL_LEN_MAX            8                                    
#define DS_TRACE_DISPLAY_CLUSTER            8                                    
#define DS_TRACE_ENHANCED                   (L7_ENABLE + 1)
                                                                                 
#define DS_TRACE_HANDLE                     (pDiffServInfo_g->diffServTraceHandle) 
#define DS_TRACE_FIELD_UNUSED               0                                    
                                                                                 
/* subcomponent identifiers */                                                   
#define DS_TRACE_SUBID_NONE                 0                                    
#define DS_TRACE_SUBID_PRVTMIB              1                                    
#define DS_TRACE_SUBID_STDMIB               2                                    
#define DS_TRACE_SUBID_DSTL                 3                                    

/* trace point identifiers */
/* - general - */
#define DS_TRACE_ID_ROW_CREATE_ENTER        0x0101
#define DS_TRACE_ID_ROW_CREATE_EXIT         0x0102
#define DS_TRACE_ID_ROW_DELETE_ENTER        0x0103
#define DS_TRACE_ID_ROW_DELETE_EXIT         0x0104
#define DS_TRACE_ID_OBJECT_SETTEST_FAIL     0x0105
#define DS_TRACE_ID_OBJECT_SET              0x0106
#define DS_TRACE_ID_ADMIN_MODE_CHNG         0x0110  /* distiller              */
#define DS_TRACE_ID_INTF_EVENT              0x0111  /* distiller              */
#define DS_TRACE_ID_EVAL_ALL                0x0112  /* distiller              */
#define DS_TRACE_ID_ROW_EVENT               0x0113  /* distiller              */
/* - private MIB specific - */
#define DS_TRACE_ID_POL_EVAL_INST           0x0401
#define DS_TRACE_ID_POL_EVAL_EXIT           0x0402
#define DS_TRACE_ID_POL_BUILD_UP            0x0403
#define DS_TRACE_ID_POL_TEAR_DOWN           0x0404
#define DS_TRACE_ID_POL_TLV_ADD_ENTER       0x0405
#define DS_TRACE_ID_POL_TLV_ADD_SEP_EXIT    0x0406
#define DS_TRACE_ID_POL_TLV_ADD_COMBO_EXIT  0x0407
#define DS_TRACE_ID_POL_TLV_DEL_ENTER       0x0408
#define DS_TRACE_ID_POL_TLV_DEL_EXIT        0x0409
#define DS_TRACE_ID_POL_TLV_ADD_DTL         0x040A
#define DS_TRACE_ID_POL_TLV_DEL_DTL         0x040B
/* - standard MIB specific - */
/* - other - */
#define DS_TRACE_ID_RSVD_START              0xBBBB  /* rsvd for trace utility */
#define DS_TRACE_ID_RSVD_STOP               0xEEEE  /* rsvd for trace utility */


/* trace point content structures (fields in host order) */

/* header used by all trace points
 * NOTE: The handle and component ID fields are part of trace API function call
 */
typedef struct
{
  L7_uchar8         handle;                     /* 00   : trace buffer handle */
  L7_uchar8         compId;                     /* 01   : component id        */
  L7_ushort16       traceId;                    /* 02-03: trace id code       */
  L7_ulong32        timeStamp;                  /* 04-07: entry timestamp     */
} dsTracePtHdr_t;

/* common trace fields for MIB table information */
typedef struct
{
  L7_uchar8         subId;                      /* 08   : subcomponent id     */
  L7_uchar8         tableId;                    /* 09   : MIB table id        */
  L7_ushort16       index1;                     /* 10-11: table index 1       */
  L7_ushort16       index2;                     /* 12-13: table index 2       */
  L7_ushort16       index3;                     /* 14-15: table index 3       */
  L7_ulong32        rowPtr;                     /* 16-19: row pointer         */
} dsTracePtMibTable_t;

/* row create/delete */
typedef struct
{
  dsTracePtMibTable_t  mibTable;                /* 08-19: (MIB table info)    */
  L7_uchar8         rc;                         /* 20   : return code         */
  L7_uchar8         act;                        /* 21   : row activation flag */
} dsTracePtRowMgmt_t;

/* object set/setTest */
typedef struct
{
  dsTracePtMibTable_t  mibTable;                /* 08-19: (MIB table info)    */
  L7_uchar8         rc;                         /* 20   : return code         */
  L7_uchar8         rsvd_21;                    /* 21   : (currently unused)  */
  L7_uchar8         oid;                        /* 22   : object id (oid)     */
  L7_uchar8         valLen;                     /* 23   : object id (oid)     */
                                                /* 24-31: value(network order)*/
  L7_uchar8         value[DS_TRACE_OBJ_VAL_LEN_MAX];
} dsTracePtObjectSet_t;

/* common indexing for policy evaluation */
typedef struct
{
  L7_ushort16       policyIndex;                /* 08-09: policyIndex         */
  L7_ushort16       policyInstIndex;            /* 10-11: policyInstIndex     */
  L7_ushort16       intIfNum;                   /* 12-13: internal intf number*/
  L7_uchar8         ifDirection;                /* 14   : intf direction      */
  L7_uchar8         rsvd_15;                    /* 15   : (currently unused)  */
} dsTracePtPolIndex_t;

/* policy evaluate */
typedef struct
{
  dsTracePtPolIndex_t  index;                   /* 08-15: (policy eval index) */
  L7_uchar8         linkState;                  /* 16   : link state          */
  L7_uchar8         policyIsReady;              /* 17   : pol is ready flag   */
  L7_uchar8         servRowStatus;              /* 18   : service row status  */
  L7_uchar8         adminMode;                  /* 19   : DiffServ admin mode */
  L7_uchar8         reissueTlv;                 /* 20   : reissue TLV flag    */
  L7_uchar8         rc;                         /* 21   : return code         */
  L7_uchar8         needBuildUp;                /* 22   : need build-up flag  */
  L7_uchar8         needTearDown;               /* 23   : need tear-down flag */
} dsTracePtPolEval_t;

/* policy build-up/tear-down */
typedef struct
{
  dsTracePtPolIndex_t  index;                   /* 08-15: (policy eval index) */
} dsTracePtPolUpDn_t;

/* policy TLV add/delete */
typedef struct
{
  dsTracePtPolIndex_t  index;                   /* 08-15: (policy eval index) */
  L7_ulong32        instKey;                    /* 16-19: instance key        */
  L7_uchar8         rc;                         /* 20   : return code         */
  L7_uchar8         combined;                   /* 21   : "combined" flag     */
} dsTracePtPolTlvAddDel_t;

/* policy TLV DTL add/delete */
typedef struct
{
  dsTracePtPolIndex_t  index;                   /* 08-15: (policy eval index) */
  L7_ulong32        tlvHandle;                  /* 16-19: TLV handle          */
  L7_uchar8         rc;                         /* 20   : return code         */
  L7_uchar8         rsvd_21_22_23;              /* 21-23: (currently unused)  */
  L7_ulong32        tlvType;                    /* 24-27: TLV type            */
  L7_ulong32        tlvLen;                     /* 28-31: TLV length          */
  L7_ulong32        instKey;                    /* 32-35: instance key        */
} dsTracePtPolTlvDtl_t;

/* generic event occurrence */
typedef struct
{
  L7_uchar8         tableId;                    /* 08   : MIB table identifier*/
  L7_uchar8         event;                      /* 09   : event code          */
  L7_uchar8         adminMode;                  /* 10   : DiffServ admin mode */
  L7_uchar8         rsvd_11;                    /* 11   : (currently unused)  */
  L7_ulong32        rowPtr;                     /* 12-15: row pointer         */
  L7_ulong32        intIfNum;                   /* 16-19: internal intf num   */
} dsTracePtGenEvent_t;


/* trace-related macros */ 
#define DS_TRACE_IS_ENABLED() \
  ((L7_BOOL)(pDiffServInfo_g->diffServTraceMode >= L7_ENABLE))

#define DS_TRACE_MODE_ENHANCED() \
  ((L7_BOOL)(pDiffServInfo_g->diffServTraceMode >= DS_TRACE_ENHANCED))

#define DS_TRACE_PT_CREATE_ENTER(_subid, _tid, _idx1, _idx2, _idx3, _aflg) \
  dsTracePtRowMgmt(DS_TRACE_ID_ROW_CREATE_ENTER, (_subid), \
                   (_tid), (L7_ushort16)(_idx1), \
                   (L7_ushort16)(_idx2), (L7_ushort16)(_idx3), \
                   0, 0, (L7_uchar8)(_aflg))
  
#define DS_TRACE_PT_CREATE_EXIT(_subid, _tid, _idx1, _idx2, _idx3, _rowp, \
                                _rc, _aflg) \
  dsTracePtRowMgmt(DS_TRACE_ID_ROW_CREATE_EXIT, (_subid), \
                   (_tid), (L7_ushort16)(_idx1), \
                   (L7_ushort16)(_idx2), (L7_ushort16)(_idx3), \
                   (L7_ulong32)(_rowp), (_rc), (_aflg))
  
#define DS_TRACE_PT_DELETE_ENTER(_subid, _tid, _idx1, _idx2, _idx3) \
  dsTracePtRowMgmt(DS_TRACE_ID_ROW_DELETE_ENTER, (_subid), \
                   (_tid), (L7_ushort16)(_idx1), \
                   (L7_ushort16)(_idx2), (L7_ushort16)(_idx3), \
                   0, 0, 0)
  
#define DS_TRACE_PT_DELETE_EXIT(_subid, _tid, _idx1, _idx2, _idx3, _rowp, _rc) \
  dsTracePtRowMgmt(DS_TRACE_ID_ROW_DELETE_EXIT, (_subid), \
                   (_tid), (L7_ushort16)(_idx1), \
                   (L7_ushort16)(_idx2), (L7_ushort16)(_idx3), \
                   (L7_ulong32)(_rowp), (_rc), 0)
  
#define DS_TRACE_PT_OBJECT_SETTEST_FAIL(_subid, _tid, _idx1, _idx2, _idx3, \
                                        _rowp, _rc, _oid, _vlen, _pval) \
  dsTracePtObjectSet(DS_TRACE_ID_OBJECT_SETTEST_FAIL, (_subid), \
                     (_tid), (L7_ushort16)(_idx1), \
                     (L7_ushort16)(_idx2), (L7_ushort16)(_idx3), \
                     (L7_ulong32)(_rowp), (_rc), (_oid), (_vlen), (_pval))
  
#define DS_TRACE_PT_OBJECT_SET(_subid, _tid, _idx1, _idx2, _idx3, \
                               _rowp, _rc, _oid, _vlen, _pval) \
  dsTracePtObjectSet(DS_TRACE_ID_OBJECT_SET, (_subid), \
                     (_tid), (L7_ushort16)(_idx1), \
                     (L7_ushort16)(_idx2), (L7_ushort16)(_idx3), \
                     (L7_ulong32)(_rowp), (_rc), (_oid), (_vlen), (_pval))
  
#define DS_TRACE_PT_POL_EVAL_INST(_pidx, _iin, _dir, \
                                  _lnkst, _prdy, _srstat, _mode, _reiss) \
  dsTracePtPolEval(DS_TRACE_ID_POL_EVAL_INST, \
                   (L7_ushort16)(_pidx), \
                   (L7_ushort16)(_iin), (L7_uchar8)(_dir), \
                   (L7_uchar8)(_lnkst), (L7_uchar8)(_prdy), \
                   (L7_uchar8)(_srstat), (L7_uchar8)(_mode), \
                   (L7_uchar8)(_reiss), \
                   0, 0, 0)
  
#define DS_TRACE_PT_POL_EVAL_EXIT(_pidx, _iin, _dir, \
                                  _lnkst, _prdy, _srstat, _mode, _reiss, \
                                  _rc, _nbup, _ntdn) \
  dsTracePtPolEval(DS_TRACE_ID_POL_EVAL_EXIT, \
                   (L7_ushort16)(_pidx), \
                   (L7_ushort16)(_iin), (L7_uchar8)(_dir), \
                   (L7_uchar8)(_lnkst), (L7_uchar8)(_prdy), \
                   (L7_uchar8)(_srstat), (L7_uchar8)(_mode), \
                   (L7_uchar8)(_reiss), \
                   (L7_uchar8)(_rc), (L7_uchar8)(_nbup), (L7_uchar8)(_ntdn))
  
#define DS_TRACE_PT_POL_BUILD_UP(_pidx, _iin, _dir) \
  dsTracePtPolUpDn(DS_TRACE_ID_POL_BUILD_UP, (L7_ushort16)(_pidx), \
                   (L7_ushort16)(_iin), (L7_uchar8)(_dir))
  
#define DS_TRACE_PT_POL_TEAR_DOWN(_pidx, _iin, _dir) \
  dsTracePtPolUpDn(DS_TRACE_ID_POL_TEAR_DOWN, (L7_ushort16)(_pidx), \
                   (L7_ushort16)(_iin), (L7_uchar8)(_dir))
  
#define DS_TRACE_PT_POL_TLV_ADD_ENTER(_pidx, _iin, _dir, _combo) \
  dsTracePtPolTlvAddDel(DS_TRACE_ID_POL_TLV_ADD_ENTER, \
                        (L7_ushort16)(_pidx), 0, \
                        (L7_ushort16)(_iin), (L7_uchar8)(_dir), \
                        0, 0, (L7_uchar8)(_combo))
  
#define DS_TRACE_PT_POL_TLV_ADD_SEP_EXIT(_pidx, _pidx2, _iin, _dir, _key, _rc) \
  dsTracePtPolTlvAddDel(DS_TRACE_ID_POL_TLV_ADD_SEP_EXIT, \
                        (L7_ushort16)(_pidx), (L7_ushort16)(_pidx2), \
                        (L7_ushort16)(_iin), (L7_uchar8)(_dir), \
                        (L7_ulong32)(_key), (L7_uchar8)(_rc), 0)
  
#define DS_TRACE_PT_POL_TLV_ADD_COMBO_EXIT(_pidx, _iin, _dir, _key, _rc) \
  dsTracePtPolTlvAddDel(DS_TRACE_ID_POL_TLV_ADD_COMBO_EXIT, \
                        (L7_ushort16)(_pidx), 0, \
                        (L7_ushort16)(_iin), (L7_uchar8)(_dir), \
                        (L7_ulong32)(_key), (L7_uchar8)(_rc), 0)
  
#define DS_TRACE_PT_POL_TLV_DEL_ENTER(_pidx, _iin, _dir) \
  dsTracePtPolTlvAddDel(DS_TRACE_ID_POL_TLV_DEL_ENTER, \
                        (L7_ushort16)(_pidx), 0, \
                        (L7_ushort16)(_iin), (L7_uchar8)(_dir), \
                        0, 0, 0)
  
#define DS_TRACE_PT_POL_TLV_DEL_EXIT(_pidx, _iin, _dir, _rc) \
  dsTracePtPolTlvAddDel(DS_TRACE_ID_POL_TLV_DEL_EXIT, \
                        (L7_ushort16)(_pidx), 0, \
                        (L7_ushort16)(_iin), (L7_uchar8)(_dir), \
                        0, (L7_uchar8)(_rc), 0)
  
#define DS_TRACE_PT_POL_TLV_ADD_DTL(_pidx, _pidx2, _iin, _dir, _hndl, _rc, \
                                    _tlvtype, _tlvlen, _key) \
  dsTracePtPolTlvDtl(DS_TRACE_ID_POL_TLV_ADD_DTL, \
                     (L7_ushort16)(_pidx), (L7_ushort16)(_pidx2), \
                     (L7_ushort16)(_iin), (L7_uchar8)(_dir), \
                     (L7_ulong32)(_hndl), (L7_uchar8)(_rc), \
                     (L7_ulong32)(_tlvtype), (L7_ulong32)(_tlvlen), \
                     (L7_ulong32)(_key))
  
#define DS_TRACE_PT_POL_TLV_DEL_DTL(_pidx, _iin, _dir, _hndl, _rc, \
                                    _tlvtype, _tlvlen) \
  dsTracePtPolTlvDtl(DS_TRACE_ID_POL_TLV_DEL_DTL, \
                     (L7_ushort16)(_pidx), 0, \
                     (L7_ushort16)(_iin), (L7_uchar8)(_dir), \
                     (L7_ulong32)(_hndl), (L7_uchar8)(_rc), \
                     (_tlvtype), (_tlvlen), \
                     0)
  
#define DS_TRACE_PT_ADMIN_MODE_CHNG(_admin) \
  dsTracePtGenEvent(DS_TRACE_ID_ADMIN_MODE_CHNG, \
                    0, 0, (L7_uchar8)(_admin), 0, 0)
  
#define DS_TRACE_PT_INTF_EVENT(_event, _admin, _iin) \
  dsTracePtGenEvent(DS_TRACE_ID_INTF_EVENT, \
                    0, (L7_uchar8)(_event), (L7_uchar8)(_admin), \
                    0, (L7_ulong32)(_iin))
  
#define DS_TRACE_PT_EVAL_ALL(_admin) \
  dsTracePtGenEvent(DS_TRACE_ID_EVAL_ALL, \
                    0, 0, (L7_uchar8)(_admin), 0, 0)

#define DS_TRACE_PT_ROW_EVENT(_tid, _event, _admin, _rowp) \
  dsTracePtGenEvent(DS_TRACE_ID_ROW_EVENT, \
                    (L7_uchar8)(_tid), (L7_uchar8)(_event), \
                    (L7_uchar8)(_admin), (L7_ulong32)(_rowp), 0)
  

/* prototype of trace id formatting functions */
typedef L7_RC_t (*dsTraceFmtFunc_t)(L7_ushort16 traceId, void *pDataStart); 


/* trace function prototypes */
void dsTraceModeApply(L7_uint32 mode);
void dsTracePtRowMgmt(L7_ushort16 traceId,
                      L7_uchar8   subId,
                      L7_uchar8   tableId,
                      L7_ushort16 index1,
                      L7_ushort16 index2,
                      L7_ushort16 index3,
                      L7_ulong32  rowPtr,
                      L7_uchar8   rc,
                      L7_uchar8   activateFlag);
void dsTracePtObjectSet(L7_ushort16 traceId,
                        L7_uchar8   subId,
                        L7_uchar8   tableId,
                        L7_ushort16 index1,
                        L7_ushort16 index2,
                        L7_ushort16 index3,
                        L7_ulong32  rowPtr,
                        L7_uchar8   rc,
                        L7_uchar8   oid,
                        L7_uchar8   valLen,
                        L7_uchar8   *pVal);
void dsTracePtPolEval(L7_ushort16 traceId,
                      L7_ushort16 policyIndex,
                      L7_ushort16 intIfNum,
                      L7_uchar8   ifDirection,
                      L7_uchar8   linkState,
                      L7_uchar8   policyIsReady,
                      L7_uchar8   servRowStatus,
                      L7_uchar8   adminMode,
                      L7_uchar8   reissueTlv,
                      L7_uchar8   rc,
                      L7_uchar8   needBuildUp,
                      L7_uchar8   needTearDown);
void dsTracePtPolUpDn(L7_ushort16 traceId,
                      L7_ushort16 policyIndex,
                      L7_ushort16 intIfNum,
                      L7_uchar8   ifDirection);
void dsTracePtPolTlvAddDel(L7_ushort16 traceId,
                           L7_ushort16 policyIndex,
                           L7_ushort16 policyInstIndex,
                           L7_ushort16 intIfNum,
                           L7_uchar8   ifDirection,
                           L7_ulong32  instKey,
                           L7_uchar8   rc,
                           L7_uchar8   combined);
void dsTracePtPolTlvDtl(L7_ushort16 traceId,
                        L7_ushort16 policyIndex,
                        L7_ushort16 policyInstIndex,
                        L7_ushort16 intIfNum,
                        L7_uchar8   ifDirection,
                        L7_ulong32  tlvHandle,
                        L7_uchar8   rc,
                        L7_ulong32  tlvType,  
                        L7_ulong32  tlvLen,  
                        L7_ulong32  instKey);
void dsTracePtGenEvent(L7_ushort16 traceId,
                       L7_uchar8   tableId,
                       L7_uchar8   event, 
                       L7_uchar8   adminMode,
                       L7_ulong32  rowPtr,
                       L7_ulong32  intIfNum);
void dsTraceObjectValFormat(void * pValue, size_t valLen, L7_uchar8 *pOutputBuf);
L7_RC_t dsTraceShow(L7_BOOL format, L7_uint32 count);
void dsTraceFmtMibTable(void *pDataStart);
L7_RC_t dsTraceFmtRowMgmt(L7_ushort16 traceId, void *pDataStart);
L7_RC_t dsTraceFmtObjectSet(L7_ushort16 traceId, void *pDataStart);
L7_RC_t dsTraceFmtPolIndex(void *pDataStart);
L7_RC_t dsTraceFmtPolEval(L7_ushort16 traceId, void *pDataStart);
L7_RC_t dsTraceFmtPolUpDn(L7_ushort16 traceId, void *pDataStart);
L7_RC_t dsTraceFmtPolTlvAddDel(L7_ushort16 traceId, void *pDataStart);
L7_RC_t dsTraceFmtPolTlvDtl(L7_ushort16 traceId, void *pDataStart);
L7_RC_t dsTraceFmtGenEvent(L7_ushort16 traceId, void *pDataStart);


#endif /* INCLUDE_DIFFSERV_TRACE_H */

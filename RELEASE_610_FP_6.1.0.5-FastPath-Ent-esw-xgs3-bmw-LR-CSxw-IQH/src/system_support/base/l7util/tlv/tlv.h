/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   tlv.h
*
* @purpose    TLV utility private data structures, constants, etc.
*
* @component  tlv
*
* @comments   none
*
* @create     07/05/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#ifndef INCLUDE_TLV_H
#define INCLUDE_TLV_H

#include "l7_common.h"
#include "tlv_api.h"

/* internal function prototypes */
L7_RC_t tlvCtrlShow(L7_tlvHandle_t tlvHandle);
void tlvBlockSummaryShow(void);

/* general definitions */
#define L7_TLV_HANDLE_MAX         128           /* max number block handles   */
#define L7_TLV_BLK_SIZE_MIN       4             /* min TLV block size allowed */
#define L7_TLV_BLK_SIZE_MAX       65536         /* max TLV block size allowed */
#define L7_TLV_AREA_ROUNDUP_BNDY  4             /* area alloc round up        */
#define L7_TLV_AREA_ROUNDUP_MASK  (L7_TLV_AREA_ROUNDUP_BNDY-1)
#define L7_TLV_HANDLE_INVALID     0             /* invalid TLV block handle   */
#define L7_TLV_OPEN_STK_MAX       12            /* max nested open TLVs       */
#define L7_TLV_OPEN_STK_UNUSED    0             /* open TLV stack entry unused*/
#define L7_TLV_SHOW_DATA_MAX      64            /*TLV bytes to display in show*/
#define L7_TLV_SHOW_DATA_ROW      16            /*num bytes to display per row*/

/* checking macros */
#define L7_TLV_BLK_SIZE_IN_RANGE(_s) \
  ((L7_BOOL)(((_s) >= L7_TLV_BLK_SIZE_MIN) && ((_s) <= L7_TLV_BLK_SIZE_MAX)))

/* fixed header portion of each TLV */
typedef struct
{
  L7_ushort16   type;                           /* TLV type identifier        */
  L7_ushort16   length;                         /* TLV length of value field  */
} L7_tlvHdr_t;

/* TLV entry data start location */
typedef struct
{
  L7_tlvHdr_t   hdr;                            /* TLV header (type, length)  */
  L7_uchar8     dataStart[1];                   /* start of TLV data (value)  */
} L7_tlvData_t;

#define L7_TLV_TYPE_VAL_MAX     ((1<<16) -1)    /* max TLV type value allowed */
#define L7_TLV_LENGTH_VAL_MAX   ((1<<16) -1)    /* max TLV len value allowed  */

#define L7_TLV_ENTRY_SIZE()           (sizeof(L7_tlvHdr_t))
#define L7_TLV_ENTRY_DATA_START(_phdr)  (((L7_tlvData_t *)(_phdr))->dataStart)

/* TLV block control structure -- SIZE MUST BE MULTIPLE OF 4 BYTES */
typedef struct L7_tlvCtrl_s
{
  struct L7_tlvCtrl_s *pChain;                  /* linked-list chain pointer  */
  L7_tlvHandle_t  tlvHandle;                    /* handle used for this block */
  L7_uint32       allocSize;                    /* total allocation size      */
  L7_uint32       appId;                        /* owning application ident   */
  L7_uchar8       appName[L7_TLV_APP_NAME_MAX+1]; /* owning application name  */
  L7_uint32       active;                       /* blk active (L7_NO, L7_YES) */
  L7_uint32       complete;                     /* blk cmplt (L7_NO, L7_YES)  */
  L7_uint32       nextOffset;                   /* next TLV data write loc'n  */
  L7_uint32       maxOffset;                    /* max offset of TLV data area*/
  L7_uint32       numOpen;                      /* outstanding open TLVs      */
  L7_uint32       openOffset[L7_TLV_OPEN_STK_MAX]; /* open TLV LIFO stack ofst*/
  L7_uint32       sizeLo;                       /* TLV size low-water mark    */
  L7_uint32       sizeHi;                       /* TLV size high-water mark   */
  L7_uint32       traceId;                      /* trace ID                   */
} L7_tlvCtrl_t;

/* TLV block data start location */
typedef struct
{
  L7_tlvCtrl_t  ctrl;                           /* TLV block controls         */
  L7_uchar8     dataStart[1];                   /* start of TLV block data    */
} L7_tlvBlkData_t;

#define L7_TLV_CTRL_SIZE()                (sizeof(L7_tlvCtrl_t))
#define L7_TLV_BLK_ALLOC_SIZE(_dsize)     ((sizeof(L7_tlvCtrl_t)) + (_dsize))
#define L7_TLV_BLK_DATA_START(_pc)        (((L7_tlvBlkData_t *)(_pc))->dataStart)
#define L7_TLV_BLK_DATA_LENGTH(_pc)       ((_pc)->nextOffset - sizeof(L7_tlvCtrl_t))
#define L7_TLV_BLK_DATA_MAX(_pc)          ((_pc)->maxOffset - sizeof(L7_tlvCtrl_t))
#define L7_TLV_BLK_HAS_ROOM(_pc, _len)    ((L7_BOOL)(((_pc)->nextOffset + (_len)) <= (_pc)->maxOffset))
#define L7_TLV_OPEN_HDR_START(_pc, _lev)  ((L7_uchar8 *)(_pc) + (_pc)->openOffset[(_lev)])
#define L7_TLV_NEXT_WRITE_PTR(_pc)        ((L7_uchar8 *)(_pc) + (_pc)->nextOffset)
#define L7_TLV_IS_ACTIVE(_pc)             ((L7_BOOL)((_pc)->active == L7_YES))
#define L7_TLV_IS_COMPLETE(_pc)           ((L7_BOOL)((_pc)->complete == L7_YES))

/* Trace definitions */
#define L7_TLV_TRACE_ID_INVALID           (L7_uint32)0xFFFFFFFFUL
#define L7_TLV_TRACE_NAME_LEN             12

/* L7_TLV_TRACE_ENTRY_MAX and L7_TLV_TRACE_ENTRY_SIZE_MAX moved to l7_resources.h */

#define L7_TLV_TRACE_DISPLAY_SECTIONS     2
typedef enum
{
  L7_TLV_TRACE_CODE_CREATE        = 0xCC,
  L7_TLV_TRACE_CODE_COMPLETE      = 0x80,
  L7_TLV_TRACE_CODE_DELETE        = 0xDD,
  L7_TLV_TRACE_CODE_QUERY         = 0x88,
  L7_TLV_TRACE_CODE_WRITE         = 0x30,
  L7_TLV_TRACE_CODE_OPEN          = 0x20,
  L7_TLV_TRACE_CODE_CLOSE         = 0x2F
} L7_tlvTraceCode_t;

#endif /* INCLUDE_TLV_H */

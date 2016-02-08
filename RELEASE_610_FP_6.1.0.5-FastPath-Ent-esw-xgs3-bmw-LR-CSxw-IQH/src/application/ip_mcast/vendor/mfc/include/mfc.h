/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  mfc.h
*
* @purpose   The purpose of this file is to export the functionalities
*            implemented by the corresponding C file (mfc.c).
*
* @component Multicast Forwarding Cache (MFC)
*
* @comments  This file should not be included by any header or C files
*            other than those that belong to the MFC component itself.
*            All other components MUST use the functionalities exported
*            by the mfc_api.h file only.
*
* @create    January 18, 2006
*
* @author    ddevi
* @end
*
**********************************************************************/
#ifndef _MFC_H
#define _MFC_H

/**********************************************************************
       Include Files (only those required to compile this file)
**********************************************************************/
#include "l7_common.h"
#include "mfc_api.h"
#include "avl_api.h"

/**********************************************************************
                  Function Prototypes
**********************************************************************/
typedef struct mfc_pkt_s
{
  L7_uint32     iif;    /* Router Interface pkt came in on */
} mfc_pkt_t;

typedef struct mfcCacheEntry_s
{
  L7_inet_addr_t     source;
  L7_inet_addr_t     group;
  L7_uint32          rxPort;            /* Physical port where packet was received */
  L7_uint32          iif;               /* incoming router interface number */
  interface_bitset_t oif;               /* Outgoing router interface bitmask */
  L7_BOOL            newRoute;          /* New Route entry? ..Reset when upcall 
                                           MRP has updated the mroute entry */
  L7_BOOL            inUseBySw;         /* Entry is in use ? */
  /*L7_BOOL            inUseByHw;          Entry in use by HW? */
  L7_BOOL            addedToHwTable;    /* Set when an entry is added to the
                                           Hardware table */
  L7_uint32          mcastProtocol;     /* The multicast protocol */
  L7_uint32          dataTTL;           /* TTL seen in the last received datapkt */
  L7_int32           holdtime;          /* Hold Time is time the mroute entry 
                                           stays in MFC after the entry is not
                                           being used for  forwarding*/
  L7_int32           ctime;             /* Create Time in secs */
  L7_int32           expire;            /* Entry Expire time in secs*/
  L7_uint32          upcallExpire;      /* Upcall expire time in secs*/
  L7_uint32          numBytes;          /* The num of bytes */
  L7_uint32          numForwardedPkts;  /* The num of packets forwarded based on
                                           this mroute entry */
  L7_uint32          numWrongIfPkts;    /* The number of packets arrived
                                           on wrong if */
  L7_uint32          numNoCacheEvents;  /* Number NO-CACHE events notified for this (S,G) */
  L7_uint32          numWrongIfEvents;  /* Number WRONG-IFF events notified for this (S,G) */
  L7_uint32          numWholePktEvents; /* Number WHOLE-PKT events notified for this (S,G) */
  L7_BOOL            rpfAction;
  void               *dummy;            /* Void pointer for use by the AVL Tree Lib.
                                         * Last field of this data structure should
                                         * be a void pointer type.
                                         */
} mfcCacheEntry_t;

typedef struct mfcTimers_s
{
  osapiTimerDescr_t     *pUpcallsTimer; /* to process Upcalls expire  */
  osapiTimerDescr_t     *pExpireTimer;  /* to process Entry expire */
}mfcTimers_t;

typedef enum
{
  MFC_NUM_HW_CACHE_ENTRIES = 0,
  MFC_NUM_NEW_CACHE_ENTIRES,
  MFC_NUM_FORWARDED_PKTS,
  MFC_COUNTERS_TYPE_MAX
} MFC_COUNTERS_TYPE_t;

typedef enum
{
  MFC_COUNTERS_INCREMENT,
  MFC_COUNTERS_DECREMENT,
  MFC_COUNTERS_RESET,
  MFC_COUNTERS_ACTION_MAX
} MFC_COUNTERS_ACTION_t;

typedef struct mfcInfo_s
{
  avlTreeTables_t       *pTreeHeap;
  void                  *pDataHeap;
  avlTree_t              mfcAvlTree;
  L7_BOOL                mfcV4Enabled;   /* This enable-disable flags specific for family get dispersed  */
  L7_BOOL                mfcV6Enabled;   /* once the control blocks for v4 and v6 is done */
  L7_uint32              mfcCounters[MFC_COUNTERS_TYPE_MAX];
  mfcTimers_t            mfcTimers;
}mfcInfo_t;

/* MFC Registration List Definition */
typedef struct mfcRegisterList_s
{
  mfcNotifyFn_t mfcNotifyFunction;
  L7_uint32     wrongIfRateLimitCount;
  L7_BOOL       wrongIfRateLimitEnable;
  L7_BOOL       wrongIfRateLimitStarted;
} mfcRegisterList_t;

/**********************************************************************
                  Typedefs
***********************************************************************/


#define MFC_AVL_TREE_TYPE                  0x10

/* in milli-seconds - Multicast Cache Timer Interval is the rate at which the 
   MFC Table is parsed to process expired entries */
#define MFC_ENTRY_EXPIRE_TIMER_INTERVAL_MSECS   10000 

/* in milli-seconds - upcall Timer Interval is the rate at which the MFC Table 
   is parsed to act upon the new routes not resolved by upcall MRPs */
#define MFC_UPCALL_EXPIRE_TIMER_INTERVAL_MSECS  (L7_MCAST_FORWARDING_UPCALL_LIFETIME * 1000)

/*extern L7_BOOL               mfcEnabled;*/
extern mfcInfo_t             mfcInfo ;
/* MFC Registration List */
extern mfcRegisterList_t mfcRegisterList[L7_MRP_MAXIMUM];



extern L7_RC_t mfcEntryUpdate(mfcEntry_t *pData);

extern L7_RC_t mfcEntryRemove(mfcEntry_t *pData);
 
extern L7_RC_t mfcRegisteredUsersNotify(mcastEventTypes_t operType, L7_uint32 msgLen,
                            mfcEntry_t *pData);   

extern L7_RC_t mfcMroutePktForward(mfcEntry_t *pData);
extern L7_BOOL mfcIsEnabled(L7_uchar8 family);

extern L7_uint32 mfcCountersValueGet (MFC_COUNTERS_TYPE_t counterType);

extern mfcCacheEntry_t *mfcEntryGet(L7_inet_addr_t *pSource, 
                                    L7_inet_addr_t *pGroup,
                                    L7_uint32 searchMode);
extern L7_RC_t
mfcCountersUpdate (MFC_COUNTERS_TYPE_t counterType,
                   MFC_COUNTERS_ACTION_t counterAction);

#endif /* _MFC_H */

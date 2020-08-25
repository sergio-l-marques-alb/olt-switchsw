/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  mfc_rxtx.h
*
* @purpose   The purpose of this file is to export the functionalities
*            implemented by the corresponding C file (mfc_rxtx.c).
*
* @component Multicast Forwarding Cache (MFC)
*
* @comments  This file should not be included by any header or C files
*            other than those that belong to the MFC component itself.
*            All other components MUST use the functionalities exported
*            by the mfc_api.h file only.
*
* @create    January 24, 2006
*
* @author    ddevi.
* @end
*
**********************************************************************/
#ifndef _MFC_RXTX_H
#define _MFC_RXTX_H

/**********************************************************************
       Include Files (only those required to compile this file)
**********************************************************************/
#include "l3_addrdefs.h"
#include "intf_bitset.h" 
#include "mfc_api.h"
#include "mfc.h"

/**********************************************************************
                  Function Prototypes
**********************************************************************/
extern L7_RC_t mfcHwEntryUpdate(mfcCacheEntry_t *pData,
                         L7_BOOL forceDrop);
extern L7_RC_t mfcHwEntryDelete(L7_inet_addr_t *pSource,
                                L7_inet_addr_t *pGroup,
                                L7_uint32 iif,
                                L7_uint32 mcastProtocol);
extern L7_RC_t mfcHwInterfaceStatusChange(mfcInterfaceChng_t *pInterfaceChange);
extern L7_BOOL mfcHwUseCheck(L7_inet_addr_t *pSource,
                             L7_inet_addr_t *pGroup,
                             L7_uint32 iif,
                             L7_uint32 mcastProtocol);
extern L7_RC_t mfcPktTransmit(struct rtmbuf *pBuf,
                              mfcEntry_t    *pEntry);
extern L7_RC_t mfcV6SockInit();
extern L7_RC_t mfcV6SockDeInit();

#endif /* _MFC_RXTX_H */

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_timer.h
*
* @purpose Contains structures,constants,macros, for dvmrp protocol
*
* @component
*
* @comments
*
* @create 07/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#ifndef _DVMRP_TIMER_H_
#define _DVMRP_TIMER_H_
#include <dvmrp_common.h>

#define DVMRP_ROUTE_HOLD_TIME                   120
#define DVMRP_FLASH_INTERVAL         5

/*********************************************************************
* @purpose  This function is the expiry handler for graft timer. 
*
* @param    pParam    - -  @b{(input)} Void pointer to the timer info.

* @returns  None
*
* @notes    Graft expiry handler.
*       
* @end
*********************************************************************/
void dvmrp_graft_timeout (void *pDvmrp);

/*********************************************************************
* @purpose  This function is the expiry handler for nbr node. 
*
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    nbr node is removed.
*       
* @end
*********************************************************************/
void dvmrp_neighbor_timeout (void *pParam);

/*********************************************************************
* @purpose  This function is the expiry handler for prune timer. 
* 
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    Prune timer expiry handler
*       
* @end
*********************************************************************/
void dvmrp_prune_expire (void *pParam);

/*********************************************************************
* @purpose  This function is the expiry handler for route entry. 
* 
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    route entry expiry handler
*       
* @end
*********************************************************************/
void dvmrp_routes_timeout (void *gDvmrp);

/*********************************************************************
* @purpose  This function is called when the Cache Remove timer
*           is expired.
*
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void dvmrp_Cache_Remove (void *pParam);

/*********************************************************************
* @purpose  This function is called when the periodic route advertiser 
*           is expired.
*            
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    Route reports are sent on all the interfaces
*       
* @end
*********************************************************************/
void dvmrp_timer_update (void *pParam );

/*********************************************************************
* @purpose  This function is called when the flash timer  is expired. 
*
* @param    pParam    -   @b{(input)} Void pointer to the timer info.

* @returns  None
*
* @notes    only the updated routes are sent
*       
* @end
*********************************************************************/
void dvmrp_flash_update (void *pParam);

/*********************************************************************
* @purpose  This function is used to set the flash timer. 
*
* @param     dvmrpcb  -  @b{(input)}Pointer to the DVMRP control block .
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_flash_timer_set (dvmrp_t *dvmrpcb);

/*********************************************************************
* @purpose  This function sends the prune packet once again
* 
* @param    pParam    -   @b{(input)} Void pointer to the timer info.
* 
* @returns  None
*
* @notes    This function is invoked when a prune retry timer expires.  
*           It will see if the  entry for the prune is still in the cache and
*           is still negative, if so it marks the entry for deletion by 
*           cache_timer_expire
*       
* @end
*********************************************************************/
void dvmrp_prune_retransmit_timer_expire(void * pParam);

/*********************************************************************
* @purpose  This function is used as a call back function in the Timer Init function 
* @param    timerCtrlBlk -@b{(input)} Timer control block
* @param    pParam       -@b{(input)} Timer info
* @returns    None
*
* @notes   None
* @end
*********************************************************************/
void dvmrpTimerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void *pParam);

#endif

/**********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename     cpdm_connstatus_util.c
 *
 * @purpose      Session monitoring utility funcs
 *
 * @component    CPDM
 *
 * @comments     none
 *
 * @create       8/21/2007
 *
 * @authors      dcaugherty
 *
 * @end
 *
 **********************************************************************/

#ifndef CPDM_CONNSTATUS_UTIL_H
#define CPDM_CONNSTATUS_UTIL_H

#include "captive_portal_commdefs.h"

void cpdmMonitorSessions_private(void);


/*********************************************************************
*
* @purpose  Recalculate session timeout for client connections
*           of a particular user
*
* @param    L7_uchar8 * @b{(input)} userId - ID of user
* @param    L7_uint32   @b{(input)} newVal - new timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments DO NOT USE on connections using RADIUS authentication!
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusSessionTimeoutByUIdUpdate(uId_t uId,
                                              L7_uint32 newVal);

/*********************************************************************
*
* @purpose  Recalculate idle timeout for client connections of a
*           particular user
*
* @param    L7_uchar8 * @b{(input)} userId - ID of user
* @param    L7_uint32   @b{(input)} newVal - new timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments DO NOT USE on connections using RADIUS authentication!
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusIdleTimeoutByUIdUpdate(uId_t uId,
                                           L7_uint32 newVal);



/*********************************************************************
*
* @purpose  Recalculate idle timeout for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Connections created via RADIUS authentication will
*           not be affected.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusIdleTimeoutByCpIdUpdate(cpId_t cpId,
                                            L7_uint32 newVal);

/*********************************************************************
*
* @purpose  Recalculate session timeout for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Connections created via RADIUS authentication will
*           not be affected.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusSessionTimeoutByCpIdUpdate(cpId_t cpId,
                                               L7_uint32 newVal);


/*********************************************************************
*
* @purpose  Recalculate max octets for specific local user 
*
* @param    uId_t      @b{(input)} uId - User ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxInputOctetsByUIdUpdate(uId_t uId,
                                             L7_uint32 newVal);

/*********************************************************************
*
* @purpose  Recalculate max octets for specific local user 
*
* @param    uId_t      @b{(input)} uId - User ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxOutputOctetsByUIdUpdate(uId_t uId,
                                              L7_uint32 newVal);

/*********************************************************************
*
* @purpose  Recalculate max octets for specific local user 
*
* @param    uId_t      @b{(input)} cpId - User ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxTotalOctetsByUIdUpdate(uId_t uId,
                                             L7_uint32 newVal);

/*********************************************************************
*
* @purpose  Recalculate max octets for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxInputOctetsByCpIdUpdate(cpId_t cpId,
                                          L7_uint32 newVal);

/*********************************************************************
*
* @purpose  Recalculate max octets for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxOutputOctetsByCpIdUpdate(cpId_t cpId,
                                          L7_uint32 newVal);

/*********************************************************************
*
* @purpose  Recalculate max octets for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxTotalOctetsByCpIdUpdate(cpId_t cpId,
                                          L7_uint32 newVal);



/* FASTPATH frowns on the use of unions, but there's no other way in C to
 * allow structure definitions to be overlaid on the same buffer. (We could
 * calculate aggregate structure size explicitly, but this is prone to error.
 * We could just include every structure type in our queue element definition,
 * but this is wasteful of memory (and would make us look incredibly stupid).
 *
 * The key reason for this disdain of unions is that, if the discriminator in
 * our structure definition (the field that tells us which union type to use)
 * occupies less than a machine word, we risk reading a bogus value on 
 * architectures with different endian-ness.  So let's remove this possibility
 * by ensuring that the discriminator occupies an entire machine word.
 */


typedef struct newConnQEntry_s /* just in case we need to add a field */
{
  L7_enetMacAddr_t mac;
} newConnQEntry_t;

typedef struct deleteConnQEntry_s /* just in case we need to add a field */
{
  L7_enetMacAddr_t mac;
} deleteConnQEntry_t;

typedef struct deauthConnQEntry_s /* just in case we need to add a field */
{
  L7_enetMacAddr_t peerMac, clientMac;
} deauthConnQEntry_t;


typedef union cpConnTransactionData_u
{
  newConnQEntry_t newConn;
  deleteConnQEntry_t deleteConn;  
  deauthConnQEntry_t deauthConn;  
} cpConnTransactionData_t;

typedef struct cpConnTransaction_s
{
  L7_uint32 trType; /* our discriminator -- MUST remain a L7_uint32 !! */
  cpConnTransactionData_t tr;
} cpConnTransaction_t;



/*********************************************************************
*
* @purpose  Initialize our connections transaction queue
*
* @param    L7_uint32 @b{(input)} max new transactions to enqueue
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!  
*
*            Returns L7_FAILURE if buffer space cannot be allocated.           
*
* @end
*
*********************************************************************/
L7_RC_t    connTransQInit(L7_uint32 maxNewTrans);


/*********************************************************************
*
* @purpose  Finalizes queue - wipes out entries, prepares for reuse.
*
* @param    none
*
* @returns  void
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!  Allocated memory is retained for later
*            use, it is NOT freed.
*
* @end
*
*********************************************************************/
void       connTransQFinalize(void);


/*********************************************************************
*
* @purpose  Get top transaction in queue
*
* @param    cpConnTransaction_t * @b{(output)} copy of next transaction
*
* @returns  L7_SUCCESS if transaction exists
*           L7_FAILURE if no transaction available
*           L7_ERROR if input parameter is faulty
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function! 
*
* @end
*
*********************************************************************/
L7_RC_t    connTransQTop(cpConnTransaction_t * pCopy);


/*********************************************************************
*
* @purpose  Pop next transaction off queue
*
* @param    none
*
* @returns  void
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!  
*
* @end
*
*********************************************************************/
void    connTransQPop(void);


/*********************************************************************
*
* @purpose  add new connection information to queue
*
* @param    L7_enetMacAddr_t * @b{(input)} pMac - new connection
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!
*            
*            Returns L7_SUCCESS if buffer allocated, and space 
*            exists on queue to add new connection information.
* @end
*
*********************************************************************/
L7_RC_t    newConnAdd(L7_enetMacAddr_t * pMac);


/*********************************************************************
*
* @purpose  add delete connection information to queue
*
* @param    L7_enetMacAddr_t * @b{(input)} pMac - connection to delete
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!
*            
*            Returns L7_SUCCESS if buffer allocated, and space 
*            exists on queue to add delete connection information.
* @end
*
*********************************************************************/
L7_RC_t    deleteConnAdd(L7_enetMacAddr_t * pMac);


/*********************************************************************
*
* @purpose  add deauth connection information to queue
*
* @param    L7_enetMacAddr_t * @b{(input)} pPeerMac 
* @param    L7_enetMacAddr_t * @b{(input)} pClientMac 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!
*            
*            Returns L7_SUCCESS if buffer allocated, and space 
*            exists on queue to add deauth connection information.
* @end
*
*********************************************************************/
L7_RC_t
deauthConnAdd(L7_enetMacAddr_t * pPeerMac, L7_enetMacAddr_t * pClientMac);

/*********************************************************************
* @purpose  Check if client is authenticated on a physical interface
*
* @param    L7_uint32 intf @b{(input)} client connection intf
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments  
*
* @end
*********************************************************************/
L7_BOOL cpdmIsClientWired(L7_uint32 intf);

#endif

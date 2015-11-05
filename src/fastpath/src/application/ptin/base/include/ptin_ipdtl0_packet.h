/**
 * ptin_ipdtl0_packet.h
 *  
 * Implements routines to trap and to process packets on dtl0 IP 
 * Virtual Inteface 
 *
 * @author joaom (9/26/2013)
 * (joao-v-mateiro@ptinovacao.pt) 
 *  
 */

#include "ptin_globaldefs.h"

#ifndef _PTIN_IPDTL0_PACKET_H
#define _PTIN_IPDTL0_PACKET_H

#ifdef PTIN_ENABLE_DTL0TRAP

#include "ptin_include.h"
#include "sysnet_api.h"

#include "datatypes.h"


/***************************************
 * DATATYPES
 ***************************************/

/* IP/ARP dtl0 PDU Message format */
typedef struct ptin_IPDTL0_PDU_Msg_s {
  L7_uint32        msgId;         /* Of type snoopMgmtMessages_t          */
  L7_uint32        intIfNum;      /* Interface on which PDU was received  */
  L7_uint32        vlanId;        /* VLAN on which PDU was received       */
  L7_uint32        innerVlanId;   /* Inner VLAN if present                */
  L7_ulong32       timestamp;     /* Rx timestamp                         */
  L7_netBufHandle  bufHandle;     /* Buffer handle                        */
  L7_uint32        payloadLen;    /* Length of received PDU               */
  L7_uchar8        *payload;      /* Pointer to the received PDU          */
} ptin_IPDTL0_PDU_Msg_t;


/***************************************
 * DEFINES
 ***************************************/

#define PTIN_IPDTL0_PDU_MSG_SIZE            sizeof(ptin_IPDTL0_PDU_Msg_t)

/* Maximum number of Packets in queue */
#define PTIN_IPDTL0_MAX_MESSAGES            256

/* Message ID used in queue */
#define PTIN_IPDTL0_PACKET_MESSAGE_ID       1
#define PTIN_IPDTL0_MIRRORPKT_MESSAGE_ID    2

#define PTIN_IPDTL0_UNUSED_VLAN_ENTRY       0


/**********************
 * EXTERNAL ROUTINES
 **********************/

/**
 * IP dtl0 module initialization. 
 * Creates IP/ARP Packets queue and task to dispatch.
 * 
 * @author joaom (9/27/2013)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_init(void);

/**
 * IP dtl0 module deinitialization. 
 * Destroys IP/ARP Packets queue and task.
 * 
 * @author joaom (9/27/2013)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_deinit(void);

/*********************************************************************
* @purpose  Receives sampled packet
*
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @end
*********************************************************************/
L7_RC_t ptin_ipdtl0_mirrorPacketCapture(L7_netBufHandle netBufHandle,
                                        sysnet_pdu_info_t *pduInfo);

/**
 * Enables/Disables IP/ARP packets through dtl0
 * 
 * @author joaom (10/01/2013)
 * 
 * @param dtl0Vid 
 * @param outerVid 
 * @param internalVid 
 * @param intfNum 
 * @param type 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_control(L7_uint16 dtl0Vid, L7_uint16 outerVid, L7_uint16 internalVid, L7_uint32 intfNum, ptin_ipdtl0_type_t type, L7_BOOL enable);

/**
 * Enables/Disables IP/ARP packets through dtl0
 * 
 * @author joaom (01/31/2014)
 * 
 * @param dtl0Vid 
 * @param outerVid 
 * @param lag_idx 
 * @param type 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_control_b(L7_uint16 dtl0Vid, L7_uint16 outerVid, L7_uint32 lag_idx, ptin_ipdtl0_type_t type, L7_BOOL enable);

/**
 * Get dtl0 VLAN ID from internal VLAN ID
 * 
 * @author daniel (15/4/2013)
 * 
 * @param intVid 
 * 
 * @return L7_uint16 dtl0Vid
 */
L7_uint16 ptin_ipdtl0_dtl0Vid_get(L7_uint16 intVid);

/**
 * Get Internal VLAN ID from dtl0 VLAN ID
 * 
 * @author joaom (10/3/2013)
 * 
 * @param dtl0Vid 
 * 
 * @return L7_RC_t internalVid
 */
L7_uint16 ptin_ipdtl0_internalVid_get(L7_uint16 dtl0Vid);

/**
 * Get outer VLAN ID from dtl0 VLAN ID
 * 
 * @author joaom (10/3/2013)
 * 
 * @param dtl0Vid 
 * 
 * @return L7_uint16 internalVid
 */
L7_uint16 ptin_ipdtl0_outerVid_get(L7_uint16 dtl0Vid);

/**
 * Get dtl0 type from dtl0 VLAN ID
 * 
 * @author joaom (1/10/2015)
 * 
 * @param vlanId 
 * 
 * @return L7_RC_t 
 */
L7_uint16 ptin_ipdtl0_dtl0Type_get(L7_uint16 dtl0Vid);


#else

extern L7_uint16 ptin_ipdtl0_dtl0Vid_get(L7_uint16 intVid);
extern L7_uint16 ptin_ipdtl0_internalVid_get(L7_uint16 dtl0Vid);
extern L7_uint16 ptin_ipdtl0_outerVid_get(L7_uint16 dtl0Vid);
extern L7_uint16 ptin_ipdtl0_dtl0Type_get(L7_uint16 dtl0Vid);

#endif /* PTIN_ENABLE_DTL0TRAP */

#endif  /* _PTIN_IP_DTL0_PACKET_H */


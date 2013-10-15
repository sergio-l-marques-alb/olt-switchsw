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
#ifdef PTIN_ENABLE_DTL0TRAP

#ifndef _PTIN_IPDTL0_PACKET_H
#define _PTIN_IPDTL0_PACKET_H

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

/**
 * Enables/Disables IP/ARP packets through dtl0
 * 
 * @author joaom (10/01/2013)
 * 
 * @param dtl0Vid 
 * @param outerVid 
 * @param intfNum 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_control(L7_uint16 dtl0Vid, L7_uint16 outerVid, L7_uint32 intfNum, L7_BOOL enable);

/**
 * Get Internal VLAN ID from dtl0 VLAN ID
 * 
 * @author joaom (10/3/2013)
 * 
 * @param dtl0Vid 
 * 
 * @return L7_RC_t internalVid
 */
L7_uint16 ptin_ipdtl0_getInternalVid(L7_uint16 dtl0Vid);

/**
 * Get outer VLAN ID from dtl0 VLAN ID
 * 
 * @author joaom (10/3/2013)
 * 
 * @param dtl0Vid 
 * 
 * @return L7_uint16 internalVid
 */
L7_uint16 ptin_ipdtl0_getOuterVid(L7_uint16 dtl0Vid);

#endif  /* _PTIN_IP_DTL0_PACKET_H */

#else
L7_uint16 ptin_ipdtl0_getInternalVid(L7_uint16 dtl0Vid);
L7_uint16 ptin_ipdtl0_getOuterVid(L7_uint16 dtl0Vid);
#endif /* PTIN_ENABLE_DTL0TRAP */


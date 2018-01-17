/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename bootp.h
*
* @purpose bootp header file
*
* @component bootp
*
* @comments none
*
* @create 
*
* @author   Deepesh Aggarwal
*
* @end
*             
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#ifndef INCLUDE_BOOTP
#define INCLUDE_BOOTP

#include <l7_common.h>
#include <datatypes.h>
#include <osapi.h>
#include <simapi.h>
#include <log.h>
#include <osapi_support.h>
#include "dhcp_bootp_api.h"

#include <string.h>

#define L7_BP_CHADDR_LEN	16
#define L7_BP_SNAME_LEN	    64
#define L7_BP_FILE_LEN	    128

#define L7_BP_VEND_LEN	    64
#define L7_BP_MINPKTSZ	    300		                                      

#define L7_BP_SERVICE_PORT 0
#define L7_BP_NETWORK_PORT 1

/* Structure involves pointer to notify callback function and componentId assigned to it */
typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID;
  L7_RC_t (*notifyFunction)(L7_bootp_dhcp_t *networkParamsPtr);
} bootpNotifyList_t;

typedef struct bootp {
  L7_uchar8        bp_op;			    /* packet opcode type */
  L7_uchar8        bp_htype;			/* hardware addr type */
  L7_uchar8        bp_hlen;			/* hardware addr length */
  L7_uchar8        bp_hops;			/* gateway hops */
  L7_uint32        bp_xid;			/* transaction ID */
  L7_ushort16      bp_secs;			/* seconds since boot began */
  L7_ushort16      bp_flags;			/* RFC1532 broadcast, etc. */
  L7_ulong32       bp_ciaddr;			/* client IP address */
  L7_ulong32       bp_yiaddr;			/* 'your' IP address */
  L7_ulong32       bp_siaddr;			/* server IP address */
  L7_ulong32       bp_giaddr;			/* gateway IP address */
  L7_uchar8        bp_chaddr[L7_BP_CHADDR_LEN];	/* client hardware address */
  L7_char8	       bp_sname[L7_BP_SNAME_LEN];	/* server host name */
  L7_char8	       bp_file[L7_BP_FILE_LEN];	/* boot file name */
  L7_uchar8        bp_vend[L7_BP_VEND_LEN];	/* vendor-specific area */
  /* note that bp_vend can be longer, extending to end of packet. */
}L7_bootp_t;

/* structure covers one BOOTP Vendor specific value */
typedef struct bp_value
{
  L7_uchar8 tag;
  L7_uchar8 dataLen;
  L7_uchar8 data;
} L7_bp_value_t;

/* IP packet TTL number used in requests*/
#define L7_BP_IP_TTL_NUMBER 255

/* UDP port numbers, server and client */
 
#define	L7_IPPORT_BOOTPS		67
#define	L7_IPPORT_BOOTPC		68

#define L7_BOOT_REPLY		2
#define L7_BOOT_REQUEST		1

/* Hardware types from Assigned Numbers RFC */
#define L7_HTYPE_ETHERNET		  1
#define L7_HTYPE_EXP_ETHERNET	  2
#define L7_HTYPE_AX25		      3
#define L7_HTYPE_PRONET		      4
#define L7_HTYPE_CHAOS		      5   
#define L7_HTYPE_IEEE802		  6
#define L7_HTYPE_ARCNET		      7

/* Vendor magic cookie (v_magic) for CMU */
#define VENDOR_MAGIC_CMU_COOKIE		"CMU"

/* Vendor magic cookie (v_magic) for RFC1048 */
#define VENDOR_MAGIC_RFC1048_COOKIE	{ 99, 130, 83, 99 }

#define L7_VENDOR_MAGIC_COOKIE      { 99, 130, 83, 99, 255 } 
/*
 * Tag values used to specify what information is being supplied in
 * the vendor (options) data area of the packet.
 */
/* RFC 1048 */

/* padding for alignment */
#define L7_TAG_PAD			    ((L7_uchar8 )   0)

/* Subnet mask */
#define L7_TAG_SUBNET_MASK		((L7_uchar8 )   1)

/* Time offset from UTC for this system */
#define L7_TAG_TIME_OFFSET		((L7_uchar8 )   2)

/* List of routers on this subnet */
#define L7_TAG_GATEWAY		    ((L7_uchar8 )   3)

/* List of rfc868 time servers available to client */
#define L7_TAG_TIME_SERVER		((L7_uchar8 )   4)

/* List of IEN 116 name servers */
#define L7_TAG_NAME_SERVER		((L7_uchar8 )   5)

/* List of DNS name servers */
#define L7_TAG_DOMAIN_SERVER	((L7_uchar8 )   6)

/* List of MIT-LCS UDL log servers */
#define L7_TAG_LOG_SERVER		((L7_uchar8 )   7)

/* List of rfc865 cookie servers */
#define L7_TAG_COOKIE_SERVER	((L7_uchar8 )   8)

/* List of rfc1179 printer servers (in order to try) */
#define L7_TAG_LPR_SERVER		((L7_uchar8 )   9)

/* List of Imagen Impress servers (in prefered order) */
#define L7_TAG_IMPRESS_SERVER	((L7_uchar8 )  10)

/* List of rfc887 Resourse Location servers */
#define L7_TAG_RLP_SERVER		((L7_uchar8 )  11)

/* Hostname of client */
#define L7_TAG_HOST_NAME		((L7_uchar8 )  12)

/* boot file size */
#define L7_TAG_BOOT_SIZE		((L7_uchar8 )  13)

/* domain name for use with the DNS */
#define L7_TAG_DOMAIN_NAME		((L7_uchar8 )  15)

/* TFTP server address */
#define L7_TAG_TFTP_SERVER_NAME ((L7_uchar8 )  66)

/* configuration file name */
#define L7_TAG_BOOT_FILENAME    ((L7_uchar8 )  67)

/* TFTP server address */
#define L7_TAG_TFTP_SERVER      ((L7_uchar8 )  150)

/* End of cookie */
#define L7_TAG_END			    ((L7_uchar8 ) 255)


/*functions defined in bootp_prot.c*/

/**************************************************************************
* @purpose   In case factory default static configuration exists resolve
             the rest of bootp parameters
*
* @param     L7_uchar8	 @b{(input)}  Ptr to Interface name
* @param     L7_bootp_t  @b{(output)} Ptr to L7_bootp_t structure Received
* @param     L7_int32    @b{(input)}  Timeout in Sec
*
* @returns   L7_TRUE  if receives the reply packet successfully 
* @returns   L7_FALSE if any error occurs
*
* @comments None.
*
* @end
*************************************************************************/
L7_BOOL bootpVendorSpecificValuesRequest(const L7_uchar8 *intf, L7_bootp_t *bootpReplyPacket, L7_int32 timeout);

/**************************************************************************
* @purpose   This Function implements the basic bootp functionality means
*            sends a BOOTP Request and then Wait for the BOOTP Reply 
*            packet till the time specified by timeout parameter
*
* @param     intf	  @b{(input)} Ptr to Interface name
* @param     res     @b{(output)} Ptr to L7_bootp_t structure Received
* @param     timeout @b{(input)} Timeout in Sec
*
* @returns  L7_TRUE  if receives the reply packet successfully 
* @returns  L7_FALSE if any error occurs
*
* @comments None.
*
* @end
*************************************************************************/

L7_BOOL L7_bootp_do(const L7_uchar8 *intf, L7_bootp_t *res,L7_int32 timeout);

/**************************************************************************
* @purpose  Interrogate a bootp record for a particular option 
*
* @param    bp      @b{(input)} Ptr to L7_bootp_t structure
* @param    tag		@b{(input)} Tag Identity
* @param    opt     @b{(output)} Ptr to option Value
* @param    optlen     @b{(input)} option value max length
*
* @returns  L7_TRUE  if the specified option found in the packet
* @returns  L7_FALSE if couldn't find the specified option 
*
* @comments None.
*
* @end
*************************************************************************/

L7_BOOL L7_bootp_option_get(L7_bootp_t *bp, L7_uchar8 tag, void *opt, L7_uint32 optlen);

/*************************************************************************
* @purpose  To show bootp parameters
*
* @param    intf     @b{(input)} Ptr to Interface Name
* @param    bp       @b{(input)} Ptr to L7_bootp_t structure
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/
void L7_bootp_show(const L7_char8 *intf, L7_bootp_t *bp);

/*functions defined in bootp_support.c*/

/**************************************************************************
* @purpose  Extract parameters from bootp parameters structue.
*
* @param    bootpParams    @b{(input)} Ptr to L7_bootp_t structure
* @param    networkParams  @b{(output)} Ptr to L7_bootp_dhcp_t structure 
* 
*
* @returns  L7_SUCCESS   If parameter extracted successfully
*           
* @returns  L7_FAILURE   If an error occurs while looking for the parameter 
*
* @comments None.
*
* @end
*************************************************************************/

L7_RC_t getParamsFromBootpStruct(L7_bootp_t *bootpParams, L7_bootp_dhcp_t* networkParams);

/**************************************************************************
* @purpose  BOOTP task entry point for configuration of Sercvice port and 
*            network port parameters 
*			
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/

L7_RC_t L7_bootp();
/**************************************************************************
* @purpose  BOOTP task startup function.
*            
* @comments None.
*
* @end
*************************************************************************/
void bootpTaskStart();

/*********************************************************************
* @purpose  Notification Registration 
*
* @param    L7_uint32  component_ID   @b((input)) componant id
* @param    L7_uint32  (*notify)(L7_bootp_dhcp_t)   @b((input))  function to callback
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t bootpNotificationRegister( L7_COMPONENT_IDS_t component_ID, L7_RC_t (*notifyFuncPtr)(L7_bootp_dhcp_t *networkParamsPtr));

/*********************************************************************
* @purpose Deregister the routine to be called when a DHCP request completes.
*
* @param   L7_COMPONENT_IDS_t componentId  @b((input)) component ID      
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, invalid argument
*
* @comments
*                                 
* @end
*
*********************************************************************/
L7_RC_t bootpNotificationDeregister(L7_COMPONENT_IDS_t componentId);

/*********************************************************************
* @purpose  Call registered users with information 
*
* @param L7_bootp_dhcp_t networkParamsPtr @b((input)) pointer to network parameters structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void bootpNotifyRegisteredUsers(L7_bootp_dhcp_t *networkParamsPtr);

/**************************************************************************
* @purpose  Create notification protection semaphore.
*
* @comments The DHCP task is started by SIM in phase 2.
*			The configuration protection semaphore allows the task to 
*			read configuration only when the system is in phase 3.
*
* @end
*************************************************************************/
void bootpNotificationSemCreate (void);

/**************************************************************************
* @purpose  Get the notification semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpNotificationSemGet (void);

/**************************************************************************
* @purpose  Give the notification semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpNotificationSemFree (void);

/**************************************************************************
* @purpose  Create configuration protection semaphore.
*
* @comments The BOOTP task is started by SIM in phase 2.
*			The configuration protection semaphore allows the task to 
*			read configuration only when the system is in phase 3.
*
* @end
*************************************************************************/
void bootpConfigSemCreate (void);

/**************************************************************************
* @purpose  Get the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpConfigSemGet (void);

/**************************************************************************
* @purpose  Get the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpConfigSemFree (void);

/* Local define for network port interface */
#define L7_LOCAL_NETWORK_PORT_IF "dtl"

#endif 
 
/* EOF bootp.h*/

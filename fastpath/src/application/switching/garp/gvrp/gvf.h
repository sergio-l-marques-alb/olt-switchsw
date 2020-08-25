/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gvf.h
* @purpose     GVRP message formating
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef INCLUDE_GVF_H_
#define INCLUDE_GVF_H_

#include "l7_common.h"
#include "defaultconfig.h"
#include "gvrapi.h"
#include "garpcfg.h"
#include "gidapi.h"

/******************************************************************************
 * GVF : GARP VLAN REGISTRATION APPLICATION PDU FORMATTING
 ******************************************************************************
 */



typedef struct
{  /*
	* This data structure saves the temporary state required to parse GVR
	* PDUs in particular. Gpdu provides a common basis for GARP application
	* formatters, additional state can be added here as required by GVF.
	*/

	/* Gpdu gpdu;  */
	
    L7_uchar8        *start;
	L7_uchar8        *current;
	L7_uint32         vlan_id;
	L7_uint32         current_attr_type;
	
} Gvf;

typedef struct /* Gvf_msg_data */
{
	Attribute_type attribute;

	Gid_event	   event;

	Vlan_id        key1;

} Gvf_msg;


/*********************************************************************
* @purpose  init read message 
*
* @param    Pdu         pointer to Pdu
* @param    Gvf*        pointer to gvf
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gvf_rdmsg_init(GarpPdu *pdu, Gvf *gvf, L7_uint32 portNum);

/*********************************************************************
* @purpose  init write message 
*
* @param    Gvf*        pointer to gvf
* @param    Pdu         pointer to Pdu
* @param    L7_uint32   valan id
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gvf_wrmsg_init(Gvf *gvf, GarpPdu  *pdu, L7_uint32 vlan_id);

/*********************************************************************
* @purpose  read message 
*
* @param    Gvf*        pointer to gvf
* @param    Gvf_msg     pointer to message
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gvf_rdmsg(     Gvf *gvf, Gvf_msg *msg, L7_uint32 port_no);

/*********************************************************************
* @purpose  write message 
*
* @param    Gvf*        pointer to gvf
* @param    Gvf_msg     pointer to message
*
* @returns  L7_uchar8   number of bytes written
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_uchar8 gvf_wrmsg(     Gvf *gvf, Gvf_msg *msg);

/*********************************************************************
* @purpose  add end mark to message
*
* @param    Gvf*        pointer to gvf
*
* @returns  None
*
* @notes  
*       
* @end
*********************************************************************/
extern void gvf_close_wrmsg(Gvf *gvf);

#endif 




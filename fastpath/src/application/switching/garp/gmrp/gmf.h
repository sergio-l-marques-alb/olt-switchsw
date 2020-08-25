/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gmf.h
* @purpose     GMRP message formating
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/
/*************************************************************

*************************************************************/


#ifndef INCLUDE_GMRP_H_
#define INCLUDE_GMRP_H_

#include "l7_common.h"
#include "defaultconfig.h"
#include "garpcfg.h"
#include "gidapi.h"
#include "gmrapi.h"


/******************************************************************************
 * GMF : GARP MULTICAST REGISTRATION APPLICATION PDU FORMATTING
 ******************************************************************************
 */

typedef struct
{  /*
  * This data structure saves the temporary state required to parse GMR
  * PDUs in particular. Gpdu provides a common basis for GARP application
  * formatters, additional state can be added here as required by GMF.
  */

  /* Gpdu gpdu; */

  L7_uchar8        *start;
  L7_uchar8        *current;
  L7_uint32        vlan_id;
  L7_uint32        current_attr_type;

} Gmf;

typedef struct /* Gmf_msg_data */
{
  Gmr_Attribute_type attribute;

  Gid_event    event;

  L7_uchar8      key1[6];

  L7_uchar8      key2[6];

  Gmr_Legacy_control legacy_control;
  
} Gmf_msg;

/*********************************************************************
* @purpose  init read message 
*
* @param    Pdu         pointer to Pdu
* @param    gmf*        pointer to gvf
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL    gmf_rdmsg_init(GarpPdu *pdu, Gmf *gmf);

/*********************************************************************
* @purpose  init write message 
*
* @param    Gvf*        pointer to gvf
* @param    Pdu         pointer to Pdu
* @param    vlan_id     valan id
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL    gmf_wrmsg_init(Gmf *gmf, GarpPdu  *pdu, L7_ushort16 vlan_id);

/*********************************************************************
* @purpose  read message
*
* @param    Gvf*                              pointer to gvf
* @param    Gvf_msg                           pointer to message
* @param    valid_event_attribute_rcvd        pointer to valid_event_attribute_rcvd
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
extern L7_BOOL gmf_rdmsg (Gmf * gmf, Gmf_msg * msg, L7_BOOL *valid_event_attribute_rcvd);

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
extern L7_BOOL gmf_wrmsg(Gmf *gmf, Gmf_msg *msg);

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
void gmf_close_wrmsg(Gmf *gmf);

#endif /* gmf_h__ */




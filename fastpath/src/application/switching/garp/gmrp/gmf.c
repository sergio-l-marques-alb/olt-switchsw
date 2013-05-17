
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gmf.c
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

#include "string.h"
#include "gmf.h"
#include "gmrp_api.h"

#define GARP_PROTO_ID_HI        0x00
#define GARP_PROTO_ID_LO        0x01

/*********************************************************************
* @purpose   convert GARP attribute events to internal gid event format
*
* @param    L7_uchar8        attribute value
*
* @returns  corresponding gid event
*
* @notes  
*       
* @end
*********************************************************************/
static Gid_event attr_ev_2_gid_ev (L7_uchar8 val)
{
  Gid_event ReturnVal;

  switch (val)
  {
  case 0:
    ReturnVal = Gid_rcv_leaveall;
    break;
  case 1:
    ReturnVal = Gid_rcv_joinempty;
    break;
  case 2:
    ReturnVal = Gid_rcv_joinin;
    break;
  case 3:
    ReturnVal = Gid_rcv_leaveempty;
    break;
  case 4:
    ReturnVal = Gid_rcv_leavein;
    break;
  case 5:
    ReturnVal = Gid_rcv_empty;
    break;
  default:
    ReturnVal = Gid_null;       /* Default to null value */
    break;
  }
  return ReturnVal;
}

/*********************************************************************
* @purpose   convert internal gid events to GARP attribute event format
*
* @param    Gid_event        event
*
* @returns  GARP attribute event value
*
* @notes  
*       
* @end
*********************************************************************/
static L7_uchar8 gid_ev_2_attr_ev (Gid_event val)
{
  Gid_event ReturnVal;

  switch (val)
  {
  case Gid_tx_leaveall:
    ReturnVal = 0;
    break;
  case Gid_tx_joinempty:
    ReturnVal = 1;
    break;
  case Gid_tx_joinin:
    ReturnVal = 2;
    break;
  case Gid_tx_leaveempty:
    ReturnVal = 3;
    break;
  case Gid_tx_leavein:
    ReturnVal = 4;
    break;
  case Gid_tx_empty:
    ReturnVal = 5;
    break;
  default:
    ReturnVal = 0xFF;           /* Default to illegal value */
    break;
  }
  return ReturnVal;
}

/*********************************************************************
* @purpose  init read message 
*
* @param    Pdu         pointer to Pdu
* @param    gmf*        pointer to gmf
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gmf_rdmsg_init (GarpPdu * pdu, Gmf * gmf)
{
  L7_BOOL returnVal = L7_FALSE;
  L7_uchar8 protocol_id[2];

  /*
   * Initialize the start and current pointers to the beginning of the PDU.
   */
  gmf->current = (L7_uchar8 *) pdu;
  gmf->start = (L7_uchar8 *) pdu;

  protocol_id[0] = *gmf->current++;
  protocol_id[1] = *gmf->current++;

  if ((protocol_id[0] == GARP_PROTO_ID_HI) && (protocol_id[1] == GARP_PROTO_ID_LO))
  {
    returnVal = L7_TRUE;
  }

  /* Initialize the current attribute type to prime the readmsg() function, */
  /* and advance the read pointer to the first attribute in the list.       */
  gmf->current_attr_type = (L7_uint32) (*gmf->current++);

  return returnVal;
}

/*********************************************************************
* @purpose  init write message 
*
* @param    Gmf*        pointer to gmf
* @param    Pdu         pointer to Pdu
* @param    macAddress  mac Address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gmf_wrmsg_init (Gmf * gmf, GarpPdu * pdu, L7_ushort16 vlan_id)
{
  /*
   * Initialize the start and current pointers to the beginning of the PDU.
   */
  gmf->current = (L7_uchar8 *) pdu;
  gmf->start = (L7_uchar8 *) pdu;

  /*
   * Write the 2-byte protocol ID field for the GARP protocol ID (0x0001).
   */
  *gmf->current++ = GARP_PROTO_ID_HI;
  *gmf->current++ = GARP_PROTO_ID_LO;

  /*
   * Save the vlan id in the PDU state.
   */
  gmf->vlan_id = vlan_id;
  /*
   * Initialize the current attribute type to an illegal value (this will 
   * force a new attribute type on the first write).
   */
  gmf->current_attr_type = 0xFF;

  return L7_TRUE;

}

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
extern L7_BOOL gmf_rdmsg (Gmf * gmf, Gmf_msg * msg, L7_BOOL *valid_event_attribute_rcvd)
{
  L7_BOOL returnVal = L7_TRUE;
  L7_uchar8 attr_length;
  Gid_event gid_event;

  *valid_event_attribute_rcvd = L7_TRUE;

  /*
   * Check if this is an end mark
   */

  if (*gmf->current == 0)
  {
    /*
     * This is an end mark.  See if any attribute lists remain in the PDU.
     * The attribute is 16 bits.
     */
    gmf->current_attr_type = (L7_uint32) * (++gmf->current);

    /*
     * Advance to next attribute.
     */
    gmf->current++;

    /*
     * check if there is another end mark 
     */
    if (gmf->current_attr_type == 0)
    {
      /*
       * This is an end mark.  No more attribute lists remain in the PDU.
       */
      returnVal = L7_FALSE;
    }
  }

  if (returnVal == L7_TRUE)
  {
    attr_length = *(gmf->current++);
    if (attr_length >= 2)
    {
      /*  Store the attribute type */
      msg->attribute = gmf->current_attr_type;

      /* Store the attribute event */
      gid_event = attr_ev_2_gid_ev (*(gmf->current++));

      if (gid_event == Gid_null)
      {
        /* an invalid attribute event was received - try next attribute */
        *valid_event_attribute_rcvd = L7_FALSE;
        gmf->current += (attr_length - 2);
      }
      else if (gid_event == Gid_rcv_leaveall)   /* Leaveall has no value */
      {
        /*
         * Leaveall -- Check for valid attribute length. It should be 2. If    
         * OK, then zero out the data area.  No need to advance the current 
         * pointer, since leavealls have no data field. Otherwise, discard PDU.
         */
        if (attr_length == 2)
        {
          msg->event = gid_event;
          /* no key is send with Gid_rcv_leaveall */
          memset (msg->key1, 0x00, GmrpMacAddressSize);
        }
        else
        {
          returnVal = L7_FALSE;
        }
      }
      else
      {
        /*
         * Now check for a valid attribute length.  For events other than
         * leaveall, the the attribute length is 2 (itself and the event)  
         * plus number of bytes in the attribute value. If error, then discard
         * the PDU, since there is no way to know were to start next attribute.
         */
        if ((attr_length == 3) || (attr_length == 8))
        {
          /* copy the specified number of bytes to the */
          /* specified destination address.            */
          msg->event = gid_event;

          /* check if this is a multicast address attribute */
          if (msg->attribute == Gmr_Multicast_attribute)
          {
            if (attr_length == 8)
            {
              if (gmrpGroupAddrCheck (gmf->current) == L7_FAILURE)
              {
                returnVal = L7_FALSE;
              }
              else
              {
                /* multicast address */
                memcpy (&msg->key1, gmf->current, GmrpMacAddressSize);
                gmf->current += 6;
              }
            }
            else
            {
              returnVal = L7_FALSE;
            }
          }
          else if (msg->attribute == Gmr_Legacy_attribute)      /* this is Gmr_Legacy_attribute */
          {
            if (attr_length == 3)
            {
              msg->legacy_control = *(gmf->current++);
            }
            else
            {
              returnVal = L7_FALSE;
            }
          }
          else
          {
            returnVal = L7_FALSE;
          }
        }
        else
        {
          returnVal = L7_FALSE;
        }
      }
    }
    else
    {
      returnVal = L7_FALSE;
    }
  }

  return returnVal;
}

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
extern L7_BOOL gmf_wrmsg (Gmf * gmf, Gmf_msg * msg)
{
  Gid_event gid_event;
  Gmr_Attribute_type attr_type;
  L7_uchar8 req_len = 2;        /* 1 byte length, 1 byte event */
  L7_uchar8 returnVal = 0;
  L7_BOOL wr_attr_type = L7_FALSE;
  L7_BOOL wr_end_mark = L7_FALSE;

  gid_event = msg->event;
  attr_type = msg->attribute;

  /* Compute the required length.  Leaveall events have no attribute value. */
  if (gid_event != Gid_tx_leaveall)
  {
    /* need to figure out the logic here ??????? */
    if (attr_type == Gmr_Multicast_attribute)
    {
      /* add the length multicast address */
      req_len += L7_MAC_ADDR_LEN;
    }
    else if (attr_type == Gmr_Legacy_attribute)
    {
      /* add length for legacy control */
      req_len += 1;
    }
  }

  if (attr_type != gmf->current_attr_type)
  {

    /*
     * Either the attribute type has changed, or this is the first attribute
     * type in this PDU.  If it has changed, we need to write an end mark and
     * new type; if this is the first attribute, we just need to write the new
     * type.  Make sure to save the current attribute type in the pdu state.
     */
    if (gmf->current - gmf->start > 2)  /* not first type */
    {
      wr_end_mark = L7_TRUE;
      req_len++;
    }

    wr_attr_type = L7_TRUE;
    req_len++;
    gmf->current_attr_type = attr_type;

  }


  /*
   * We now know the required length for this attribute.  Ensure that enough
   * space remains in the PDU before writing; if this PDU is full, set the
   * return code to indicate that the attribute can't be written.
   */
  if (((gmf->current - gmf->start) + req_len) < (GARP_MAX_PDU_SIZE - 2 - 17))
  {
    /* set the returnVal to the req_len */
    returnVal = req_len;

    /* leave room for end mark */
    /*
     * First, write the end mark and new attribute type if required.
     */
    if (wr_end_mark == L7_TRUE)
    {
      *(gmf->current++) = 0x00;
      req_len--;

    }
    if (wr_attr_type == L7_TRUE)
    {
      *(gmf->current++) = (L7_uchar8) attr_type;
      req_len--;
    }

    /* Next, write the attribute length and event. */
    *(gmf->current++) = (L7_uchar8) req_len;

    *(gmf->current++) = gid_ev_2_attr_ev (gid_event);

    /* Finally, write the attribute value. */
    if (gid_event != Gid_tx_leaveall)
    {
      if (attr_type == Gmr_Multicast_attribute)
      {
        /* copy the multicast mac address to the message */
        memcpy (gmf->current, msg->key1, GmrpMacAddressSize);
        gmf->current += GmrpMacAddressSize;

      }
      else if (attr_type == Gmr_Legacy_attribute)
      {
        /* add the legacy control to the message */
        *(gmf->current++) = msg->legacy_control;
      }
    }
  }
  else
  {
    returnVal = 0;
  }

  return returnVal;
}

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
void gmf_close_wrmsg (Gmf * gmf)
{
  *(gmf->current++) = 0x00;
  *(gmf->current++) = 0x00;
}

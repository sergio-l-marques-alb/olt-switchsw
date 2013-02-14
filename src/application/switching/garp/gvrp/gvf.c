/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gvf.c
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
#include "string.h"
#include "gvf.h"
#include "garpapi.h"
#include "osapi_support.h"

extern gvrpInfo_t *gvrpInfo;

#define GARP_PROTO_ID_HI	0x00
#define GARP_PROTO_ID_LO	0x01
#define END_MARK            0x00
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
static Gid_event attr_ev_2_gid_ev(L7_uchar8 val )
{
	Gid_event ReturnVal;

	switch( val )
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
			ReturnVal = Gid_null;	/* Default to null value */
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
static L7_uchar8 gid_ev_2_attr_ev( Gid_event val )
{
	Gid_event ReturnVal;

	switch( val )
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
			ReturnVal = 0xFF;				/* Default to illegal value */
			break;
	}
	return ReturnVal;
}


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
L7_BOOL gvf_rdmsg_init(GarpPdu *pdu, Gvf *gvf, L7_uint32 portNum)
{
    L7_BOOL returnVal = L7_FALSE;
    L7_uchar8    protocol_id[2];


    /*
     * Initialize the start and current pointers to the beginning of the PDU.
     */
    gvf->current = (L7_uchar8*)pdu;
    gvf->start   = (L7_uchar8*)pdu;

    protocol_id[0] = *gvf->current++;
    protocol_id[1] = *gvf->current++;


    if ((protocol_id[0] == GARP_PROTO_ID_HI) &&
        (protocol_id[1] == GARP_PROTO_ID_LO))
    {
        returnVal = L7_TRUE;
    }
    else
    {
      /* increment the error statistic */
      gvrpInfo->garpErrorStats[portNum].invalidProtocolId++;
    }

   /* Initialize the current attribute type to prime the readmsg() function, */
   /* and advance the read pointer to the first attribute in the list.       */
    gvf->current_attr_type = (L7_uint32)(*gvf->current++);

   return returnVal;
}


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
L7_BOOL gvf_wrmsg_init(Gvf *gvf, GarpPdu  *pdu, L7_uint32 vlan_id)
{
    /*
     * Initialize the start and current pointers to the beginning of the PDU.
     */
	gvf->current = (L7_uchar8 *)pdu;
    gvf->start =   (L7_uchar8 *)pdu;

    /*
     * Write the 2-byte protocol ID field for the GARP protocol ID (0x0001).
     */
	*gvf->current++ = GARP_PROTO_ID_HI;
	*gvf->current++ = GARP_PROTO_ID_LO;


    /*
     * Save the vlan ID in the PDU state.
     */
	gvf->vlan_id = vlan_id;

    /*
     * Initialize the current attribute type to an illegal value (this will
     * force a new attribute type on the first write).
     */
	gvf->current_attr_type = 0xFF;

    return L7_TRUE;
}

/*********************************************************************
* @purpose  is the attribute type GARP_GVRP_VLAN_ATTRIBUTE
*
* @param    Gvf*        pointer to gvf
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
static L7_BOOL gvf_IsGvrpVlanAttribute(Gvf *gvf, L7_uint32 port_no)
{
    L7_uchar8     attr_length;
    L7_BOOL       returnVal = L7_TRUE;
    L7_uint32     totalLength = 0;

    if (gvf->current_attr_type != GARP_GVRP_VLAN_ATTRIBUTE)
    {
        if (gvf->current_attr_type != END_MARK)
        {
          /* increment the error statistic */
          gvrpInfo->garpErrorStats[port_no].invalidAttrType++;
        }
        returnVal = L7_FALSE;
        attr_length = *(gvf->current);
        if (attr_length > 0)
        {
            for (;(totalLength <= GARP_MAX_PDU_SIZE) && (returnVal == L7_FALSE);)
            {
                /* advance to the next pdu */
                gvf->current += attr_length;

                attr_length = *(gvf->current);

                totalLength += attr_length;

                if (attr_length == END_MARK)
                {
                    gvf->current_attr_type = (L7_uint32)*(++gvf->current);
                    /* advance to the next pdu */
                    gvf->current++;

                    if ((gvf->current_attr_type == GARP_GVRP_VLAN_ATTRIBUTE) ||
                        (gvf->current_attr_type == END_MARK))
                    {
                        returnVal = L7_TRUE;
                    }
                    if (gvf->current_attr_type != END_MARK &&
                        gvf->current_attr_type != GARP_GVRP_VLAN_ATTRIBUTE)
                    {
                      /* increment the statistic */
                      gvrpInfo->garpErrorStats[port_no].invalidAttrType++;

                    }
                }
            } /* end for */
        } /* end if */
    }

    return returnVal;

}


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
L7_BOOL gvf_rdmsg(Gvf *gvf, Gvf_msg *msg, L7_uint32 port_no)
{
    L7_BOOL       returnVal = L7_TRUE;
    L7_uchar8     attr_length;
    Gid_event     gid_event;

	/*
     * Check if this is an end mark
     */

    if (*gvf->current == 0)
	{
       /*
        * This is an end mark.  See if any attribute lists remain in the PDU.
        * The attribute is 16 bits.
        */
		gvf->current_attr_type = (L7_uint32)*(++gvf->current);

        /*
         * Advance to next attribute.
         */
		gvf->current++;

        /*
        * check if there is another end mark
        */
		if (gvf->current_attr_type == 0)
		{
           /*
            * This is an end mark.  No more attribute lists remain in the PDU.
            */
			returnVal = L7_FALSE;
		}
	}

    returnVal = gvf_IsGvrpVlanAttribute(gvf, port_no);

    if (returnVal == L7_TRUE)
    {
        attr_length = *(gvf->current++);
        if (attr_length > 1)
        {
            /*
             * Store the attribute type
             */
            msg->attribute = gvf->current_attr_type;


            /*
             * Store the attribute event
             */
            gid_event = attr_ev_2_gid_ev( *(gvf->current++) );


            if (gid_event == Gid_rcv_leaveall)    /* Leaveall has no value */
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
                     msg->key1  = 0;
                 }
                 else
                 {
                   /* increment the error statistic */
                   gvrpInfo->garpErrorStats[port_no].invalidAttrLen++;
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
                if (attr_length == 4)
                {
                   /*
                    * Just copy the specified number of bytes to the
                    * specified destination address.
                    */
                    msg->event = gid_event;
                    memcpy(&msg->key1,gvf->current,sizeof(L7_ushort16));
					msg->key1 = osapiNtohs(msg->key1);

                    /*inclrement the pointer */
                    gvf->current += 2;
                }
                else
                {
                  /* increment the error statistic */
                  gvrpInfo->garpErrorStats[port_no].invalidAttrLen++;
                  returnVal = L7_FALSE;
                }
            }
        }
        else
        {
          if (gvf->current_attr_type != END_MARK)
          {
            /* increment the error statistic */
            gvrpInfo->garpErrorStats[port_no].invalidAttrLen++;
          }
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
L7_uchar8 gvf_wrmsg(Gvf *gvf, Gvf_msg *msg)
{
    Gid_event      gid_event;
    L7_uchar8      req_len = 2;     /* 1 byte length, 1 byte event */
    L7_uchar8      returnVal = 0;
    L7_BOOL        wr_attr_type = L7_FALSE;
    L7_BOOL        wr_end_mark  = L7_FALSE;
	Vlan_id			attr_key;
    Attribute_type attr_type;

    gid_event = msg->event;
    attr_type = msg->attribute;

    /*
     * Compute the required length.  Leaveall events have no attribute value.
     */
		if (gid_event != Gid_tx_leaveall)
		{
            /* the Vlan Id is 2 octets */
			req_len += 2;
		}

		if (attr_type != gvf->current_attr_type)
		{

           /*
            * Either the attribute type has changed, or this is the first attribute
            * type in this PDU.  If it has changed, we need to write an end mark and
            * new type; if this is the first attribute, we just need to write the new
            * type.  Make sure to save the current attribute type in the pdu state.
            */
	        if (gvf->current - gvf->start > 2)  /* not first type */
		    {
		       wr_end_mark = L7_TRUE;
               req_len++;
		    }

            wr_attr_type = L7_TRUE;
            req_len++;
            gvf->current_attr_type = attr_type;

        }


       /*
        * We now know the required length for this attribute.  Ensure that enough
        * space remains in the PDU before writing; if this PDU is full, set the
        * return code to indicate that the attribute can't be written.
        */
		if (((gvf->current - gvf->start) + req_len) < (GARP_MAX_PDU_SIZE-2))
        {
            /* set the returnVal to the req_len */
            returnVal = req_len;

            /* leave room for end mark */
            /*
             * First, write the end mark and new attribute type if required.
             */
			if (wr_end_mark == L7_TRUE)
			{
				*(gvf->current++) = 0x00;
				req_len--;

			}
			if (wr_attr_type == L7_TRUE)
			{
				*(gvf->current++) = (L7_uchar8)attr_type;
				req_len--;
			}

            /*
             * Next, write the attribute length and event.
             */
            *(gvf->current++) = (L7_uchar8)req_len;

            *(gvf->current++) = gid_ev_2_attr_ev( gid_event );

            /*
             * Finally, write the attribute value.
             */
            if (gid_event != Gid_tx_leaveall)
            {

				attr_key = msg->key1;
                *(gvf->current++) = (L7_uchar8)(attr_key >> 8);
                *(gvf->current++) = (L7_uchar8)attr_key;
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
void gvf_close_wrmsg(Gvf *gvf)
{
    *(gvf->current++) = END_MARK;
    *(gvf->current++) = END_MARK;

}


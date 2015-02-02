/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name:       sysnet_util.c
*
* Purpose:    Sysnet Utility function
*
* Component:  sysnet
*
* Comments:   none
*
* Created by: anayar
*
*********************************************************************/
/*************************************************************

*************************************************************/



#include "sysnetinclude.h"
#include "default_cnfgr.h"
#include "osapi_support.h"

#include "wireless_api.h"
#include "dvlantag_api.h"
#include "usmdb_common.h"
#include "usmdb_mib_bridge_api.h"

#include "logger.h"

extern sysnetPduHooks_t      sysnetPduHooks[SYSNET_AF_MAX][L7_SYSNET_PDU_MAX_HOOKS];
extern sysnetPduRcCounters_t sysnetPduRcCounters[SYSNET_AF_MAX][L7_SYSNET_PDU_MAX_HOOKS];
extern L7_BOOL               sysnetPduHooksInitialized;
sysnetNotifyList_t           sysnetNotifyList;
extern L7_uint32             sysnetPduAFMap[];

/* PTin added: packet trap */
#if 1
L7_BOOL pdu_process_debug = 0;

void pdu_process_debug_enable(L7_BOOL enable)
{
  pdu_process_debug = enable;
}
#endif

/*********************************************************************
* @purpose  Determines if the specified vlan id is valid
*
* @param    vlanId    @b{(input)} The vlan id to be checked for validity
*
* @returns  L7_TRUE   If the specified vlan id is valid
* @returns  L7_FALSE  If the specified vlan id is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL sysNetIsValid8021qVlanId( L7_ushort16 vlanId )
{
  if (vlanId >= L7_DOT1Q_MIN_VLAN_ID &&
      vlanId <= L7_DOT1Q_MAX_VLAN_ID)
  {
    return L7_TRUE;
  }
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Register a routine to be called when a pdu is received
*
* @param    *snEntry  @b{(input)} Registering routines need to provide
* @param              a) funcName: Name of the registered pdu receive
* @param              b) entry type: if SYSNET_MAC_ENTRY
* @param                                SYSNET_ETHERTYPE_ENTRY
* @param                                SYSNET_DSAP_ENTRY
* @param              c) macAddr, protocol_type or dsap value depending
* @param                 on the type of entry (b)
* @param              d) Pointer to the associated pduReceive function
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetRegisterPduReceive(sysnetNotifyEntry_t *snEntry)
{
  L7_uint32  i;

  /* WPJ_TBD:  Add locks around registrations */

  if (snEntry->notify_pdu_receive == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "SYSNET: Invalid Registration Function\n");
    return(L7_FAILURE);
  }

  /* Is there an entry that is already registered? */
  for (i = 0; i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS; i++)
  {
    if (snEntry->type ==  SYSNET_PKT_RX_REASON)
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.rxReason
           == snEntry->u.rxReason)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "SYSNET Registration already exists\n");
        return(L7_ERROR);
      }

    if (snEntry->type == SYSNET_SUBTYPE_ENTRY)
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.subType == snEntry->u.subType)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "SYSNET Registration already exists\n");
        return(L7_ERROR);
      }

    if (snEntry->type == SYSNET_MAC_ENTRY)
      if (memcmp(sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr,
                 snEntry->u.macAddr, L7_MAC_ADDR_LEN) == 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "SYSNET Registration already exists\n");
        return(L7_ERROR);
      }

    if (snEntry->type == SYSNET_ETHERTYPE_ENTRY)
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.protocol_type
          == snEntry->u.protocol_type)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "SYSNET Registration already exists\n");
        return(L7_ERROR);
      }

    if (snEntry->type == SYSNET_DSAP_ENTRY)
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.dsap
          == snEntry->u.dsap)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "SYSNET Registration already exists\n");
        return(L7_ERROR);
      }
  }

  /* Find an empty entry */
  for (i = 0; i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS; i++)
  {
    if (sysnetNotifyList.sysnetNotifyEntries[i].inUse == L7_FALSE)
    {
        break;
    }
  }

  if (i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS)
  {
    //PTIn added: force common_aps_ccm_packetRx_callback() into 1st position to reduce its latency
    //if (common_aps_ccm_packetRx_callback==snEntry->notify_pdu_receive)
    if (L7_ETYPE_CFM==snEntry->u.protocol_type && SYSNET_ETHERTYPE_ENTRY==snEntry->type) {
        memcpy((L7_char8 *)&sysnetNotifyList.sysnetNotifyEntries[i], (L7_char8 *)&sysnetNotifyList.sysnetNotifyEntries[0],
               sizeof(sysnetNotifyEntry_t));
        i=0;
    }

    bzero((L7_char8 *)&sysnetNotifyList.sysnetNotifyEntries[i],
          sizeof(sysnetNotifyEntry_t));

    sysnetNotifyList.sysnetNotifyEntries[i].inUse = L7_TRUE;
    strcpy(sysnetNotifyList.sysnetNotifyEntries[i].funcName,
           snEntry->funcName);
    switch (snEntry->type)
    {
    case SYSNET_SUBTYPE_ENTRY:
      sysnetNotifyList.sysnetNotifyEntries[i].u.subType = snEntry->u.subType;
      break;

    case SYSNET_MAC_ENTRY:
      memcpy(sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr,
             snEntry->u.macAddr, L7_MAC_ADDR_LEN);
      break;
    case SYSNET_ETHERTYPE_ENTRY:
      sysnetNotifyList.sysnetNotifyEntries[i].u.protocol_type
      = snEntry->u.protocol_type;
      break;
    case SYSNET_DSAP_ENTRY:
      sysnetNotifyList.sysnetNotifyEntries[i].u.dsap
      = snEntry->u.dsap;
      break;
    case SYSNET_PKT_RX_REASON:
      sysnetNotifyList.sysnetNotifyEntries[i].u.rxReason
      = snEntry->u.rxReason;
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "SYSNET Unknown Entry Type\n");
      return(L7_FAILURE);
    }
    sysnetNotifyList.sysnetNotifyEntries[i].type = snEntry->type;
    sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive
    = snEntry->notify_pdu_receive;
    sysnetNotifyList.curr++;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "SYSNET Registration Failed\n");
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}



/*********************************************************************
* @purpose  Deregister a routine from being called when a pdu is received
*
* @param    *snEntry  @b{(input)} Deregistering routines need to provide
* @param              a) funcName: Name of the registered pdu receive
* @param              b) entry type: if SYSNET_MAC_ENTRY
* @param                                SYSNET_ETHERTYPE_ENTRY
* @param                                SYSNET_DSAP_ENTRY
* @param              c) macAddr, protocol_type or dsap value depending
* @param                 on the type of entry (b)
* @param              d) Pointer to the associated pduReceive function
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetDeregisterPduReceive(sysnetNotifyEntry_t *snEntry)
{
  L7_uint32  i;
  L7_BOOL    entry_found;

  entry_found = L7_FALSE;

  if (snEntry->notify_pdu_receive == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "SYSNET: Invalid Registration Function\n");
    return(L7_FAILURE);
  }

  for (i = 0; i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS; i++)
  {
    if (snEntry->type == SYSNET_PKT_RX_REASON)
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.rxReason
          == snEntry->u.rxReason)
      {
          if ( (snEntry->notify_pdu_receive ==
                sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive)  &&
               (snEntry->type == sysnetNotifyList.sysnetNotifyEntries[i].type) )
          {
              entry_found = L7_TRUE;
              break;
          }
      }

    if (snEntry->type == SYSNET_SUBTYPE_ENTRY)
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.subType  == snEntry->u.subType)
      {
         if ( (snEntry->notify_pdu_receive ==
               sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive)  &&
              (snEntry->type == sysnetNotifyList.sysnetNotifyEntries[i].type) )
         {
             entry_found = L7_TRUE;
             break;
         }
      }


    if (snEntry->type == SYSNET_MAC_ENTRY)
      if (memcmp(sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr,
                 snEntry->u.macAddr, L7_MAC_ADDR_LEN) == 0)
      {
         if ( (snEntry->notify_pdu_receive ==
               sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive)  &&
              (snEntry->type == sysnetNotifyList.sysnetNotifyEntries[i].type) )
         {
             entry_found = L7_TRUE;
             break;
         }
      }

    if (snEntry->type == SYSNET_ETHERTYPE_ENTRY)
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.protocol_type
          == snEntry->u.protocol_type)
      {
          if ( (snEntry->notify_pdu_receive ==
                sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive)  &&
               (snEntry->type == sysnetNotifyList.sysnetNotifyEntries[i].type) )
          {
              entry_found = L7_TRUE;
              break;
          }
      }

    if (snEntry->type == SYSNET_DSAP_ENTRY)
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.dsap
          == snEntry->u.dsap)
      {
          if ( (snEntry->notify_pdu_receive ==
                sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive)  &&
               (snEntry->type == sysnetNotifyList.sysnetNotifyEntries[i].type) )
          {
              entry_found = L7_TRUE;
              break;
          }
      }
  }

  if (entry_found == L7_TRUE)
  {
    bzero((L7_char8 *)&sysnetNotifyList.sysnetNotifyEntries[i],
          sizeof(sysnetNotifyEntry_t));
    sysnetNotifyList.curr--;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "SYSNET Deregistration Failed\n");
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Notifies registered routines on receipt of a pdu on the
*           of reason code with which it is received by the CPU
*
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
* @param    bufHandle   @b{(input)}pdu handle which stores reason code
* @param    *data       @b{(input)}pointer to the start of the pdu
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetFindReasonCodeMatch(sysnet_pdu_info_t *pduInfo, L7_netBufHandle bufHandle,
                                  L7_char8 *data)
{
  L7_uint32   i;
  L7_RC_t     rc = L7_FAILURE;

  if (pdu_process_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"RxReason reference=0x%x",((SYSAPI_NET_MBUF_HEADER_t *)bufHandle)->rxCode);

  for (i = 0; i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS; i++)
  {
    if (sysnetNotifyList.sysnetNotifyEntries[i].type == SYSNET_PKT_RX_REASON)
    {
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.rxReason & ((SYSAPI_NET_MBUF_HEADER_t *)bufHandle)->rxCode)
      {
        if (pdu_process_debug)
          LOG_TRACE(LOG_CTX_PTIN_DTL,"Calling callback for rxReason=%u",sysnetNotifyList.sysnetNotifyEntries[i].u.rxReason);

        rc = (*sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive)(bufHandle, pduInfo);
        if (rc == L7_SUCCESS)
        {
          return rc;
        }
      }
    }
  }
  return rc;
}


/*********************************************************************
* @purpose  Notifies registered routines on receipt of a pdu on the
*           of a sub-type entry match
*
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
* @param    bufHandle   @b{(input)}pdu handle
* @param    *data       @b{(input)}pointer to the start of the pdu
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetFindSubTypeMatch(sysnet_pdu_info_t *pduInfo, L7_netBufHandle bufHandle,
                               L7_uchar8 *data)
{
  L7_uchar8 sub_type;
  L7_ushort16 protocol_type;
  L7_RC_t     rc = L7_FAILURE;
  L7_char8    *pEtype;
  L7_uint32   i;
  L7_uchar8   tci;/*for the first byte of the Tag Control Information of vlan tagged frame*/
  L7_uint32   offset=0;
  L7_ushort16 temp16;

  /* Determine the protocol type */
  pEtype = data + L7_ENET_HDR_SIZE;
  bcopy (pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

  temp16 = osapiNtohs(protocol_type);
  protocol_type = temp16;

  offset =  L7_ENET_HDR_SIZE;
  if (protocol_type == L7_ETYPE_8021Q)
  {
    /*This is a Vlan tagged frame */
    /*drop this frame if the CFI bit in the TCI is set to 1*/
    /*CFI bit set means VLAN tag is extended to include */
    /*embedded source routing information-- embedded RIF*/
    pEtype = data + L7_ENET_HDR_SIZE + SYSNET_PROTOCOL_ID_LEN;
    bcopy(pEtype, &tci, 1);
    if (tci & SYSNET_DOT1Q_CFI_BIT)
    {
      rc = L7_FAILURE;
      return rc;
    }

    /* This is an Vlan tagged Frame */
    pEtype =  data + L7_ENET_HDR_SIZE + L7_8021Q_ENCAPS_HDR_SIZE;
    bcopy(pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

    temp16 = osapiNtohs(protocol_type);
    protocol_type = temp16;

    offset  =  L7_ENET_HDR_SIZE + L7_8021Q_ENCAPS_HDR_SIZE;
  }
  /* Check-if EFM-OAM protocol Type is matched */
  if (protocol_type == L7_ETYPE_EFMOAM)
  {
    bcopy(data+offset+L7_ENET_HDR_TYPE_LEN_SIZE, (L7_uchar8 *)&sub_type, sizeof(L7_uchar8));
  }
  else
  {
    return rc;
  }

  if (pdu_process_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"Searching for subType=%u",sub_type);

  for (i = 0; i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS; i++)
  {
    if (sysnetNotifyList.sysnetNotifyEntries[i].type == SYSNET_SUBTYPE_ENTRY)
    {
      if (sysnetNotifyList.sysnetNotifyEntries[i].u.subType == sub_type)
      {
        if (pdu_process_debug)
          LOG_TRACE(LOG_CTX_PTIN_DTL,"Calling callback for subType=%u",sub_type);

        rc = (*sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive)(bufHandle, pduInfo);
        if (rc == L7_SUCCESS)
        {
          return rc;
        }
      }
    }
  }
  return rc;
}
/*********************************************************************
* @purpose  Notifies registered routines on receipt of a pdu on the
*           of a mac address entry match
*
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
* @param    bufHandle   @b{(input)}pdu handle
* @param    *data       @b{(input)}pointer to the start of the pdu
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetFindMacAddrMatch(sysnet_pdu_info_t *pduInfo, L7_netBufHandle bufHandle,
                               L7_char8 *data)
{
  L7_uchar8   macAddr[L7_MAC_ADDR_LEN];
  L7_uint32   i;
  L7_RC_t     rc = L7_FAILURE;

  /* Determine Destination Mac Address */
  bcopy(data, (L7_uchar8 *)macAddr, L7_MAC_ADDR_LEN);

  if (pdu_process_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"Searching for macAddr=%02x:%02x:%02x:%02x:%02x:%02x",macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);

  for (i = 0; i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS; i++)
  {
    if (sysnetNotifyList.sysnetNotifyEntries[i].type == SYSNET_MAC_ENTRY)
    {
      if (memcmp(sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr,
                 macAddr, L7_MAC_ADDR_LEN) == 0)
      {
        if (pdu_process_debug)
          LOG_TRACE(LOG_CTX_PTIN_DTL,"Calling callback for macAddr=%02x:%02x:%02x:%02x:%02x:%02x",macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);

        rc = (*sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive)(bufHandle, pduInfo);
        if (rc == L7_SUCCESS)
        {
          return rc;
        }
      }
    }
  }
  return rc;
}


/*********************************************************************
* @purpose  Notifies registered routines on receipt of a pdu on the
*           of a protocol type entry match
*
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
* @param    bufHandle   @b{(input)}pdu handle
* @param    *data       @b{(input)}pointer to the start of the pdu
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetFindProtocolTypeMatch(sysnet_pdu_info_t *pduInfo, L7_netBufHandle bufHandle,
                                    L7_char8 *data)
{
  L7_ushort16 protocol_type;
  L7_RC_t     rc = L7_FAILURE;
  L7_char8    *pEtype;
  L7_uint32   i;
  L7_uchar8   tci;/*for the first byte of the Tag Control Information of vlan tagged frame*/
  L7_uint32   offset=0;
  L7_ushort16 temp16;

  /* Determine the protocol type */
  pEtype = data + L7_ENET_HDR_SIZE;
  bcopy (pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

  temp16 = osapiNtohs(protocol_type);
  protocol_type = temp16;

  offset =  L7_ENET_HDR_SIZE;
  if (protocol_type == L7_ETYPE_8021Q)
  {
    /*This is a Vlan tagged frame */
    /*drop this frame if the CFI bit in the TCI is set to 1*/
    /*CFI bit set means VLAN tag is extended to include */
    /*embedded source routing information-- embedded RIF*/
    pEtype = data + L7_ENET_HDR_SIZE + SYSNET_PROTOCOL_ID_LEN;
    bcopy(pEtype, &tci, 1);
    if (tci & SYSNET_DOT1Q_CFI_BIT)
    {
      rc = L7_FAILURE;
      return rc;
    }

    /* This is an Vlan tagged Frame */
    pEtype =  data + L7_ENET_HDR_SIZE + L7_8021Q_ENCAPS_HDR_SIZE;
    bcopy(pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

    temp16 = osapiNtohs(protocol_type);
    protocol_type = temp16;

    offset  =  L7_ENET_HDR_SIZE + L7_8021Q_ENCAPS_HDR_SIZE;
  }
  if (protocol_type < 1500)
  {
    /* This is a LLC & SNAP Encapsulated Ethernet Frame */
    /* sizeof LLC header = 5 bytes, SNAP OUI field = 3 bytes */
    /* TBD: Need to optimize sizof LLC & SNAP headers */
    pEtype = data + offset + 8;
    bcopy(pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

    temp16 = osapiNtohs(protocol_type);
    protocol_type = temp16;

  }

  if (pdu_process_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"Searching for protocol_type=0x%04X",protocol_type);

  for (i = 0; i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS; i++)
  {
    if (sysnetNotifyList.sysnetNotifyEntries[i].type == SYSNET_ETHERTYPE_ENTRY)
    {
      if (protocol_type ==  sysnetNotifyList.sysnetNotifyEntries[i].u.protocol_type)
      {
        if (protocol_type == L7_ETYPE_IP)
        {
          /* align IP header within mbuf to start on 4 byte boundary. This ASSUMES
           * the ethernet header (w/ or w/out VLAN tag) is 14 or 18 bytes. So move
           * ahead by 2 bytes. Since source and dest overlap, use memmove().
           * Not necessary for PPC, but ignoring that for now. */
          L7_uchar8 *newDataStart = sysapiNetMbufGetDataStart(bufHandle) + L7_MBUF_IP_CORRECTION;
          memmove((void *)newDataStart,
                  (void *)sysapiNetMbufGetDataStart(bufHandle),
                  sysapiNetMbufGetDataLength(bufHandle));
          sysapiNetMbufSetDataStart(bufHandle, newDataStart);
        }

        if (pdu_process_debug)
          LOG_TRACE(LOG_CTX_PTIN_DTL,"Calling callback for protocol_type=0x%04X",protocol_type);

        rc = (*(sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive))(bufHandle, pduInfo);
            break;
      }
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Notifies registered routines on receipt of a pdu on the
*           of a dsap entry match
*
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
* @param    bufHandle   @b{(input)}pdu handle
* @param    *data       @b{(input)}pointer to the start of the pdu
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetFindDsapEntryMatch(sysnet_pdu_info_t *pduInfo, L7_netBufHandle bufHandle,
                                 L7_char8 *data)
{
  L7_RC_t rc = L7_NOT_IMPLEMENTED_YET;

  /* TBD: To be implemented when pkts snap hdr & vlan tags are to be processes
  for(i = 0; i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS; i++)
  {
    if(sysnetNotifyList[i].type == SYSNET_DSAP_ENTRY)
    {
      rc = (*sysnetNotifyList[i].notify_pdu_receive)(bufHandle, intIfNum);
      return rc;
    }
  } */

  return rc;
}

/*********************************************************************
* @purpose  Notifies registered routines on receipt of the associated
*           pdu.
*
* @param    bufHandle   @b{(input)}pdu handle
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetNotifyPduReceive (L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8    *data;
  L7_RC_t     rc = L7_FAILURE;
  L7_uint32   intIfNum;

  if (bufHandle == L7_NULL)
  {
    return rc;
  }
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  if (pdu_process_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"I have received a packet here");

  rc = sysNetFindReasonCodeMatch(pduInfo, bufHandle, data);

  if(rc  != L7_SUCCESS)
  {
    rc = sysNetFindSubTypeMatch(pduInfo, bufHandle, data);
  }

  if (rc != L7_SUCCESS)
  {
    rc = sysNetFindMacAddrMatch(pduInfo, bufHandle, data);
  }


  if (rc != L7_SUCCESS)
  {
    rc = sysNetFindProtocolTypeMatch(pduInfo, bufHandle, data);
  }

  if (rc != L7_SUCCESS)
  {
    rc = sysNetFindDsapEntryMatch(pduInfo, bufHandle, data);
  }

  if (rc != L7_SUCCESS)
  {
    /* Check if the vlanId is one of the tunneled wireless vlan */
    if ((wtnnlFuncTable.wtnnlVlanIsTnnl != L7_NULLPTR) &&
        (wtnnlFuncTable.wtnnlVlanIsTnnl(pduInfo->vlanId, &intIfNum) == L7_SUCCESS))
    {
        /* passed to wtnnl for more processing */
        if (wtnnlFuncTable.wtnnlL2McastWiredPktProcess != L7_NULLPTR)
        {
          rc = wtnnlFuncTable.wtnnlL2McastWiredPktProcess(bufHandle, intIfNum);
        }
    }
  }

  if (rc != L7_SUCCESS)
  {
    rc = L7_FAILURE;
    SYSAPI_NET_MBUF_FREE (bufHandle);
  }
  return rc;
}

/*********************************************************************
* @purpose  Determines the exit port & the destination vlan id
*
* @param    vlanId        @b{(input)}   Destination vlan id
* @param    intIfNum      @b{(input)}   Destination intIfNum
* @param    destMac       @b{(input)}   Pointer to the Destination mac addr
* @param    pExitPort     @b{(output)}  Pointer to the Exit Port
* @param    pDestVlanId   @b{(output)}  Pointer to the Destination Vlan
* @param    pflag         @b{(output)}  Pointer to the transmit Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Destination vlan id is set only if the entry has a
* @notes    valid vlan id else it is set to L7_DOT1Q_NULL_VLAN_ID
*
* @end
*********************************************************************/
L7_RC_t sysNetExitPortGet(L7_ushort16 vlanId, L7_uint32 intIfNum,
                          L7_uchar8* destMac, L7_uint32 *pExitPort,
                          L7_ushort16 *pDestVlanId, L7_ushort16 *pflag)
{
  usmdbFdbEntry_t   fdbEntry;
  L7_RC_t           rc = L7_SUCCESS;

  /* Initialize destination vlan id to NULL */
  *pDestVlanId = L7_DOT1Q_NULL_VLAN_ID;

  if (intIfNum != 0)
  {
    *pExitPort = intIfNum;
  }
  else if (sysNetIsValid8021qVlanId(vlanId) == L7_TRUE)
  {
    *pDestVlanId = vlanId;
    memcpy(fdbEntry.usmdbFdbMac, destMac, L7_MAC_ADDR_LEN);
    rc = usmDbFDBEntryGet(simGetThisUnit(), &fdbEntry);
    if (rc == L7_SUCCESS)
    {
      *pExitPort = fdbEntry.usmdbFdbIntIfNum;
    }
    else
      rc = L7_FAILURE;
  }

  /* Determine the encapsulation type */
  *pflag = sysNetEncapsulationTypeGet(vlanId, intIfNum, *pExitPort);
  if (*pflag == 0)
  {
    rc = L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
* @purpose  Determines the encapsulation type
*
* @param    vlanId    @b{(input)}  Destination vlan Id
* @param    intIfNum  @b{(input)}  Destination Internal Interface Num
* @param    exitPort  @b{(input)}  Exit Port
*
* @returns  L7_TRUE   If the specified vlan id is valid
* @returns  L7_FALSE  If the specified vlan id is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_ushort16 sysNetEncapsulationTypeGet(L7_ushort16 vlanId, L7_uint32 intIfNum,
                                       L7_uint32 exitPort)
{
  L7_ushort16 flag = 0;
  L7_uint32   encapType;
  L7_RC_t     rc = L7_FAILURE;

  if (intIfNum == 0)
  {
    if (vlanId == 0)
    {
      /* Both the circuit vlanid & intIfNum are 0 - error */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Circuit Error - vlan id & intIfNum = 0\n");
      return 0;
    }
    else
    {
      /* Destination is a VLAN. Assume Ethernet encapsulation */
      /* TBD: post-INTEROP:  If make VLAN an internal interface,
          we will consider configuring an encapsulation type */
      flag     |= L7_LL_ENCAP_ENET;
    }
  }
  else
  {
    /* Destined to a particular interface.  Get encaps type */
    rc = nimEncapsulationTypeGet(intIfNum, &encapType);
    if (rc != L7_SUCCESS)
    {
      return 0;
    }
    if (encapType == L7_ENCAP_802)
    {
      flag     |= L7_LL_ENCAP_802;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Error - Unrecognized Encapsulation type\n");
      return 0;
    }
  }
  if (sysNetIsValid8021qVlanId(vlanId) == L7_TRUE)
  {
    flag      |= L7_LL_ENCAP_8021Q_VLAN_TAGGED;
  }

  /* If the exit port = 0, broadcast pdu over the vlan */
  if (exitPort == 0)
  {
    flag        |= L7_LL_BROADCAST_VLAN;
  }
  return flag;
}

/*********************************************************************
* @purpose  Encapsulates the pdu with an ethernet or
* @purpose  SNAP header depending on the specified flag
*
* @param    pflag     @b{(input)}   Pointer to the transmit flag
* @param    pdataStart @b{(output)} Pointer to location containing the
*                                   starting address of the pdu
* @param    plen      @b{(output)}  Current length of the pdu
* @param    datalen   @b{(input)}   Size of the ip packet
* @param    etype     @b{(input)}   Ethertype
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetPduEnetOrSnapHdrEncapsulate(L7_ushort16 *pflag, L7_char8 **pdataStart,
                                          L7_uint32 *plen, L7_uint32 datalen,
                                          L7_ushort16 etype)
{
  L7_enet_encaps_t  encapEnet;
  L7_802_encaps_t   encap802;
  L7_RC_t           rc = L7_SUCCESS;
  L7_ushort16       temp16;

  temp16 = osapiHtons(etype);
  etype = temp16;

  if (*pflag & L7_LL_ENCAP_ENET)
  {
    encapEnet.type = etype;
    memcpy(*pdataStart, (L7_char8 *)&encapEnet, L7_ENET_ENCAPS_HDR_SIZE); /*Endian*/
    *pdataStart   += L7_ENET_ENCAPS_HDR_SIZE;
    *plen         += L7_ENET_ENCAPS_HDR_SIZE;
  }
  else if (*pflag & L7_LL_ENCAP_802)
  {
    /* TBD:  Double check format of 802 encapsulation with VLAN tag */
    bzero((L7_char8 *)&encap802, sizeof(L7_802_encaps_t));
    encap802.dsap     = 0xAA;
    encap802.ssap     = 0xAA;
    encap802.cntl     = 0x03;
    encap802.type     = etype;    /*Endian*/
    encap802.length   = osapiHtons(datalen + L7_802_ENCAPS_HDR_SIZE);  /*Endian*/
    memcpy(*pdataStart, (L7_char8 *)&encap802, L7_802_ENCAPS_HDR_SIZE); /*Endian*/
    *pdataStart     += L7_802_ENCAPS_HDR_SIZE;
    *plen           += L7_802_ENCAPS_HDR_SIZE;
  }
  else
    rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
* @purpose  Allocates buffer space for a pdu and creates its
* @purpose  header
*
* @param    destMac     @b{(input)}   Destination mac Address
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    pflag       @b{(output)}  Pointer to the transmit flag
* @param    datalen     @b{(input)}   Length of the ip packet
* @param    destVlanId  @b{(input)}   Destination vlan id
* @param    etype       @b{(input)}   Ethernet type
* @param    data        @b{(input)}   pointer to the start of the pdu
*                                     to be transmitted
* @param    pBufHandle  @b{(output)}  Stores the buffer handle returned
*                                     by sysapiNetMbufGet()
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetPduHdrEncapsulate(L7_uchar8* destMac, L7_uint32 intIfNum,
                                L7_ushort16 *pflag, L7_uint32 datalen,
                                L7_uint32 destVlanId, L7_ushort16 etype,
                                L7_netBufHandle *pBufHandle, L7_char8 *data)
{
  L7_enetHeader_t    macHdr;
  L7_RC_t            rc = L7_SUCCESS;
  L7_char8           *dataStart;
  L7_uint32          len = 0;
  L7_uchar8          mac[L7_MAC_ADDR_LEN];
  L7_uint32          macType = 0;

  /* Allocate a buffer */
  SYSAPI_NET_MBUF_GET(*pBufHandle);
  if (*pBufHandle == L7_NULL)
  {
    return L7_FAILURE;
  }

  /* Get the starting address of the buffer */
  SYSAPI_NET_MBUF_GET_DATASTART(*pBufHandle, dataStart);

  memcpy((L7_char8 *)macHdr.dest.addr, destMac, L7_ENET_MAC_ADDR_LEN);

  if (intIfNum == 0)
  {
    /* Special case where we want to insert the system mgmt interface MAC */

    macType = simGetSystemIPMacType();

    simMacAddrGet(mac);

    memcpy((L7_char8 *)macHdr.src.addr, mac, L7_MAC_ADDR_LEN);
  }
  else
  {
    if (*pflag & L7_LL_USE_L3_SRC_MAC)
    {
      rc = nimGetIntfL3MacAddress(intIfNum, L7_NULL, (L7_char8 *)macHdr.src.addr);
    }
    else
    {
      rc = nimGetIntfAddress(intIfNum, L7_NULL, (L7_char8 *)macHdr.src.addr);
    }

    if (rc != L7_SUCCESS)
  {
    SYSAPI_NET_MBUF_FREE(*pBufHandle);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "No source MAC address found\n");
    rc = L7_FAILURE;
    return rc;
    }
  }

  /* Copy the MAC Header to the MBuf */
  memcpy(dataStart, &macHdr, L7_ENET_HDR_SIZE);
  dataStart   +=  L7_ENET_HDR_SIZE;
  len         +=  L7_ENET_HDR_SIZE;

  if (*pflag & L7_LL_ENCAP_8021Q_VLAN_TAGGED)
  {
    sysNetPdu8021qHdrEncapsulate(&dataStart, &len, destVlanId);
  }

  rc = sysNetPduEnetOrSnapHdrEncapsulate(pflag, &dataStart, &len, datalen, etype);

  /* Determine the length of the data frame */
  memcpy(dataStart, data, datalen);
  len += datalen;

  /*  Set the buffer size */
  SYSAPI_NET_MBUF_SET_DATALENGTH(*pBufHandle, len);

  return rc;
}

/*********************************************************************
* @purpose  Creates a vlan tagged pdu header
*
* @param    pdataStart  @b{(output)}  Pointer to location containing the
*                                       starting address of the pdu
* @param    plen        @b{(output)}  Current length of the pdu
* @param    destVlanId  @b{(input)}   Destination vlan id
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void sysNetPdu8021qHdrEncapsulate(L7_char8 **pdataStart, L7_uint32 *plen,
                                  L7_ushort16 destVlanId)
{
  L7_8021QTag_t     encap8021q;
  L7_ushort16       temp16;

  bzero((L7_uchar8 *)&encap8021q, L7_8021Q_ENCAPS_HDR_SIZE);
  encap8021q.tpid  = L7_ETYPE_8021Q;
  encap8021q.tci  |= L7_VLAN_TAG_DEFAULT_PRIORITY_MASK;
  encap8021q.tci  |= L7_VLAN_TAG_DEFAULT_CFI_MASK;
  encap8021q.tci  |= destVlanId;

  temp16 = osapiHtons(encap8021q.tpid);
  encap8021q.tpid = temp16;

  temp16 = osapiHtons(encap8021q.tci);
  encap8021q.tci = temp16;

  memcpy(*pdataStart, (L7_char8 *)&encap8021q, L7_8021Q_ENCAPS_HDR_SIZE);  /*Endian*/
  *pdataStart     += L7_8021Q_ENCAPS_HDR_SIZE;
  *plen           += L7_8021Q_ENCAPS_HDR_SIZE;
}

/*********************************************************************
* @purpose  Forward frame to be transmitted to dtlPduTransmit
*
* @param    data        @b{(input)} Pointer to pdu to be xmitted
* @param    destMac     @b{(input)} Destination Mac Address
* @param    vlanId      @b{(input)} Destination vlan id
* @param    intIfNum    @b{(input)} Destination internal interface num
* @param    etype       @b{(input)} Ether type
* @param    datalen     @b{(input)} Total length of the pdu to be
*                                   transmitted
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void sysNetForward( L7_char8 *data, L7_uchar8* destMac,
                    L7_ushort16 vlanId, L7_uint32 intIfNum,
                    L7_ushort16 etype, L7_uint32 datalen)
{
  L7_ushort16        flag = 0;
  DTL_CMD_TX_INFO_t  dtlCmd;
  L7_uint32          exitPort = 0;
  L7_ushort16        destVlanId;
  L7_netBufHandle    bufHandle;

  /* Determine the port of exit & vlan */
  if (sysNetExitPortGet(vlanId, intIfNum, destMac, &exitPort,
                        &destVlanId, &flag) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Invalid exit port - could not resolve exit intfNum\n");
    return;
  }

  /* Allocate a buffer and set the source and destination mac
     addresses */
  if (sysNetPduHdrEncapsulate(destMac, intIfNum, &flag, datalen, destVlanId,
                              etype, &bufHandle, data) != L7_SUCCESS)
  {
    return;
  }

  bzero((L7_uchar8 *)&dtlCmd, sizeof(DTL_CMD_TX_INFO_t));
  dtlCmd.intfNum    = intIfNum;
  dtlCmd.priority   = L7_8021Q_DEFAULT_PRIORITY;
  dtlCmd.typeToSend = (intIfNum != 0)?
                      DTL_NORMAL_UNICAST: DTL_VLAN_MULTICAST;

  dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd);                     /* send the buffer out */
}

/*********************************************************************
* @purpose  calculate where the actual data pcakets header starts devoid of L2 headers
*
* @param    *data       @b{(input)} Pointer to pdu that is received
*
* @returns  offset      @b{(output)} Number of bytes of header
*
* @notes    check if packet is lan tagged or LLC&SNAP or ethernet enacpsulations
* @notes                offset is dst(6)+src(6)+encaphrd(x)+protocoltype(2)
* @notes    vlan tagged  offset is 6+6+4+2 = 18
* @notes    LLC&SNAP    offset is 6+6+8+2 = 22
* @notes    Ether encap offset is 6+6+0+2 = 14
*
* @end
*********************************************************************/
L7_uint32 sysNetDataOffsetGet(L7_uchar8 *data)
{
  L7_ushort16 protocol_type;
  L7_char8    *pEtype;
  L7_uint32   offset= 0;
  L7_ushort16 temp16;

  /* Determine the protocol type */
  data += L7_ENET_HDR_SIZE;
  pEtype = data;
  bcopy (pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

  temp16 = osapiNtohs(protocol_type);
  protocol_type = temp16;
  while (protocol_type == L7_ETYPE_8021Q)
  {
    /* This is an Vlan tagged Frame */
    pEtype = data + L7_8021Q_ENCAPS_HDR_SIZE;
    data += L7_8021Q_ENCAPS_HDR_SIZE;
    bcopy (pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

    temp16 = osapiNtohs(protocol_type);
    protocol_type = temp16;

    offset = offset + 4;

  }
  if (protocol_type < 1500)
  {
    /* This is a LLC & SNAP Encapsulated Ethernet Frame */
    /* sizeof LLC header = 5 bytes, SNAP OUI field = 3 bytes */
    /* TBD: Need to optimize sizof LLC & SNAP headers */
    offset = offset + L7_ENET_HDR_SIZE + 8 + 2;

  }
  else
  {
    /*This is ehternet enacapsulation RFC894*/
    offset = offset + L7_ENET_HDR_SIZE + 2;
  }
  return offset;
}


/*********************************************************************
* @purpose  Removes the offset after the 802.1q VLAN tag if present
*           from the pdu
*
* @param    *data       @b{(input)}pointer to the start of the pdu
*
*
* @returns  *offset     @b{(output)}pointer to the point in the pdu
*                           following the VLAN tag
*
* @notes    none
*
* @end
*********************************************************************/
L7_uchar8* sysNetPost8021qVlanTagOffsetGet(L7_char8 *data)
{
  L7_ushort16 protocol_type;
  L7_char8    *offset;
  L7_ushort16 temp16;

  offset = data;
  /* Determine the protocol type */
  memcpy ((L7_uchar8 *) &protocol_type, data+L7_ENET_HDR_SIZE, sizeof(L7_ushort16)); /*Endian*/

  temp16 = osapiNtohs(protocol_type);
  protocol_type = temp16;

  if (protocol_type == L7_ETYPE_8021Q)
  {
    /*This is a Vlan tagged frame */
    offset = offset + L7_ENET_HDR_SIZE + L7_8021Q_ENCAPS_HDR_SIZE;
  }
  return offset;
}

/*********************************************************************
* @purpose  Return start of 802.1q VLAN tag if present
*
* @param    *data       @b{(input)}pointer to the start of the pdu
*
*
* @returns  *tag        @b{(output)}pointer to the vlan tag
*
* @notes    none
*
* @end
*********************************************************************/
L7_8021QTag_t* sysNet8021qVlanTagGet(L7_char8 *data)
{
  L7_ushort16 protocol_type;
  L7_char8    *offset;
  L7_ushort16 temp16;

  offset = data;
  /* Determine the protocol type */
  memcpy ((L7_uchar8 *) &protocol_type, data+L7_ENET_HDR_SIZE, sizeof(L7_ushort16)); /*Endian*/

  temp16 = osapiNtohs(protocol_type);
  protocol_type = temp16;
  if (protocol_type == L7_ETYPE_8021Q)
  {
    return (L7_8021QTag_t *)(data+L7_ENET_HDR_SIZE);
  }
  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Checks for the presence of 802.1Q E-RIF field in a packet
*
* @param    *data       @b{(input)}pointer to the start of the pdu
*
*
* @returns  flag       @b{(output)}flag indicating L7_TRUE if E-RIF
*                           present else L7_FALSE if absent
*
* @notes    none
*
* @end
*********************************************************************/

L7_BOOL sysNetHas8021qVlanERIF(L7_char8 *data)
{
    L7_ushort16 protocol_type;
    L7_BOOL     flag = L7_FALSE;
    L7_char8    *pEtype;
    L7_uchar8   tci;/*for the first byte of the Tag Control Information of vlan tagged frame*/
    L7_ushort16 temp16;

    /* Determine the protocol type */
    pEtype = data + L7_ENET_HDR_SIZE;
    bcopy (pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

    temp16 = osapiNtohs(protocol_type);
    protocol_type = temp16;

    if (protocol_type == L7_ETYPE_8021Q)
    {
      /*This is a Vlan tagged frame */
      /*drop this frame if the CFI bit in the TCI is set to 1*/
      /*CFI bit set means VLAN tag is extended to include */
      /*embedded source routing information-- embedded RIF*/
        pEtype = data + L7_ENET_HDR_SIZE + SYSNET_PROTOCOL_ID_LEN;
        bcopy(pEtype, &tci, 1);

        if (tci & SYSNET_DOT1Q_CFI_BIT)
        {
            flag = L7_TRUE;
        }
        else
        {
            flag = L7_FALSE;
        }
    }
    return flag;
}


/*********************************************************************
* @purpose  Call the registered intercept functions for the specified address family and hook ID
*
* @param    addressFamily    @b{(input)} Address Family of hook (i.e. AF_INET)
* @param    hookId           @b{(input)} Hook ID being called (i.e. SYSNET_INET_IN)
* @param    bufHandle        @b{(input)} Buffer handle to the frame
* @param    *pduInfo         @b{(input)} Pointer to the PDU info (intIfNum, VLAN ID, etc.)
* @param    continueFunc     @b{(input)} Function to be called to continue processing the frame
* @param    *hookAction   _evc_dump   @b{(output)} Action taken by the hook intercept call (optional parm)
*
* @returns  L7_TRUE   Frame has either been discarded or consumed
* @returns  L7_FALSE  Caller should continue processing the frame
*
* @notes    If the frame is discarded or consumed, no further processing will be done
* @notes    on the frame.  In the case of discard, the network buffer is freed here.
* @notes    In the case of consumed, the interceptor should free the frame.  If the frame
* @notes    is neither discarded nor consumed, the continueFunc is called, which will continue
* @notes    processing the frame.  If the continueFunc is NULL, control will return to the
* @notes    caller (hook point) and frame processing will continue there.
*
* @end
*********************************************************************/
L7_BOOL sysNetPduIntercept(L7_uint32 addressFamily,
                           L7_uint32 hookId,
                           L7_netBufHandle bufHandle,
                           sysnet_pdu_info_t *pduInfo,
                           L7_FUNCPTR_t continueFunc,
                           SYSNET_PDU_RC_t *hookAction)
{
  SYSNET_PDU_RC_t hookVerdict = SYSNET_PDU_RC_IGNORED;
  L7_BOOL intercepted = L7_FALSE;
  L7_uint32 sysnetAFIndex;

  if (sysnetPduHooksInitialized == L7_FALSE)
    return L7_FALSE;

  /* Convert AF to index into sysnet hook table */
  if (sysNetPduAFMapGet(addressFamily, &sysnetAFIndex) != L7_SUCCESS)
    return L7_FALSE;

  if (sysnetPduHooks[sysnetAFIndex][hookId].numHooks != 0)
  {
    hookVerdict = sysNetPduInterceptIterate(sysnetPduHooks[sysnetAFIndex][hookId].interceptFuncList,
                                            sysnetAFIndex, hookId, bufHandle, pduInfo, continueFunc);
    switch (hookVerdict)
    {
    case SYSNET_PDU_RC_DISCARD:
      SYSAPI_NET_MBUF_FREE(bufHandle);
      intercepted = L7_TRUE;
      break;

    case SYSNET_PDU_RC_CONSUMED:
      intercepted = L7_TRUE;
      break;

    case SYSNET_PDU_RC_COPIED:
    case SYSNET_PDU_RC_IGNORED:
    case SYSNET_PDU_RC_PROCESSED:
    case SYSNET_PDU_RC_MODIFIED:
      if (continueFunc != L7_NULLPTR)
      {
        /* Assume that a successful return code means the frame was consumed or discarded */
        if ( (*continueFunc)(bufHandle, pduInfo) == L7_SUCCESS )
          intercepted = L7_TRUE;
      }
      break;

    default:
      break;
    }
  }
  else if (continueFunc != L7_NULLPTR)
  {
    /* Assume that a successful return code means the frame was consumed or discarded */
    if ( (*continueFunc)(bufHandle, pduInfo) == L7_SUCCESS )
      intercepted = L7_TRUE;
  }

  /* Give the caller the return code if asked for it */
  if(hookAction != L7_NULLPTR)
    *hookAction = hookVerdict;

  return intercepted;
}

/*********************************************************************
* @purpose  Iterate through the list of registered interceptors for the specified hook ID
*
* @param    interceptList[]  @b{(input)} List of interceptors for this hook ID
* @param    sysnetAFIndex    @b{(input)} Index into hook ID table
* @param    hookId           @b{(input)} Hook ID being called (i.e. SYSNET_INET_IN)
* @param    bufHandle        @b{(input)} Buffer handle to the frame
* @param    *pduInfo         @b{(input)} Pointer to the PDU info (intIfNum, VLAN ID, etc.)
* @param    continueFunc     @b{(input)} Function to be called to continue processing the frame
*
* @returns  SYSNET_PDU_DISCARD    Discard this frame
* @returns  SYSNET_PDU_CONSUMED   Frame has been consumed by interceptor
* @returns  SYSNET_PDU_COPIED     Frame has been copied by interceptor
* @returns  SYSNET_PDU_IGNORED    Frame ignored by interceptor
* @returns  SYSNET_PDU_PROCESSED  Frame has been processed by interceptor
* @returns  SYSNET_PDU_MODIFIED   Frame has been modified by interceptor
*
* @notes    The hook table is semaphore protected in the calling function, sysNetPduIntercept
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t sysNetPduInterceptIterate(SYSNET_INTERCEPT_FUNC_t interceptList[],
                                          L7_uint32 sysnetAFIndex,
                                          L7_uint32 hookId,
                                          L7_netBufHandle bufHandle,
                                          sysnet_pdu_info_t *pduInfo,
                                          L7_FUNCPTR_t continueFunc)
{
  L7_uint32 i;
  SYSNET_PDU_RC_t hookRc = SYSNET_PDU_RC_IGNORED;
  SYSNET_INTERCEPT_FUNC_t interceptFunc;

  for (i = 0; i < L7_SYSNET_HOOK_PRECEDENCE_LAST; i++)
  {
    if ( (interceptFunc = interceptList[i]) != L7_NULLPTR )
    {
      hookRc = (*interceptFunc)(hookId, bufHandle, pduInfo, continueFunc);
      switch (hookRc)
      {
      case SYSNET_PDU_RC_DISCARD:
        sysnetPduRcCounters[sysnetAFIndex][hookId].sysnetPduRcDiscard++;
        return SYSNET_PDU_RC_DISCARD;
        break;

      case SYSNET_PDU_RC_CONSUMED:
        sysnetPduRcCounters[sysnetAFIndex][hookId].sysnetPduRcConsumed++;
        return SYSNET_PDU_RC_CONSUMED;
        break;

      case SYSNET_PDU_RC_COPIED:
        /* continue to next hook */
        sysnetPduRcCounters[sysnetAFIndex][hookId].sysnetPduRcCopied++;
        break;

      case SYSNET_PDU_RC_IGNORED:
        /* continue to next hook */
        sysnetPduRcCounters[sysnetAFIndex][hookId].sysnetPduRcIgnored++;
        break;

      case SYSNET_PDU_RC_PROCESSED:
        /* continue to next hook */
        sysnetPduRcCounters[sysnetAFIndex][hookId].sysnetPduRcProcessed++;
        break;

      case SYSNET_PDU_RC_MODIFIED:
        /* continue to next hook */
        sysnetPduRcCounters[sysnetAFIndex][hookId].sysnetPduRcModified++;
        break;

      default:
        /* Unknown Sysnet hook rc; continue to next hook */
        break;
      }
    }
  }

  return hookRc;
}

/*********************************************************************
* @purpose  Convert an address family to an index into the SYSNET PDU hook table
*
* @param    af                @b{(input)} Address Family
* @param    *sysnetAFIndex    @b{(output)} Sysnet AF index into hook table
*
* @returns  L7_SUCCESS    If address family was found in the table
* @returns  L7_SUCCESS    If address family is not in the table
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetPduAFMapGet(L7_uint32 af, L7_uint32 *sysnetAFIndex)
{
  L7_uint32 i;

  for (i = 0; i < SYSNET_AF_MAX; i++)
  {
    if (sysnetPduAFMap[i] == af)
    {
      *sysnetAFIndex = i;
      return L7_SUCCESS;
    }
  }

  /* AF not found */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next available slot in the sysnet AF map table
*
* @param    *sysnetAFIndex    @b{(output)} Sysnet AF index into hook table
*
* @returns  L7_SUCCESS    If address family was found in the table
* @returns  L7_SUCCESS    If address family is not in the table
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetPduAFMapNextAvailGet(L7_uint32 *sysnetAFIndex)
{
  L7_uint32 i;

  for (i = 0; i < SYSNET_AF_MAX; i++)
  {
    if (sysnetPduAFMap[i] == 0xFFFFFFFF)
    {
      *sysnetAFIndex = i;
      return L7_SUCCESS;
    }
  }

  /* Table is full */
  return L7_FAILURE;
}

/* PTin added: callbacks */
#if 1
/**
 * Show Notify callbacks List
 * 
 * @return L7_RC_t 
 */
L7_RC_t sysNetNotifyListDebugShow()
{
  L7_uint32 i;

  for (i = 0; i < FD_CNFGR_SYSNET_MAX_REGISTRATIONS; i++)
  {
    /* Skip empty entries */
    if (!sysnetNotifyList.sysnetNotifyEntries[i].inUse || 
        sysnetNotifyList.sysnetNotifyEntries[i].type == SYSNET_INVALID_ENTRY)
      continue;

    printf("Entry %u:\r\n", i);
    printf("  Type=%u   funcName=%s\r\n", sysnetNotifyList.sysnetNotifyEntries[i].type, sysnetNotifyList.sysnetNotifyEntries[i].funcName);
    printf("  macAddr=%02x:%02x:%02x:%02x:%02x:%02x protocol_type=0x%04x subType=0x%02x dsap=0x%02x rxReason=0x%08x\r\n",
           sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr[0],
           sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr[1],
           sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr[2],
           sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr[3],
           sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr[4],
           sysnetNotifyList.sysnetNotifyEntries[i].u.macAddr[5],
           sysnetNotifyList.sysnetNotifyEntries[i].u.protocol_type,
           sysnetNotifyList.sysnetNotifyEntries[i].u.subType,
           sysnetNotifyList.sysnetNotifyEntries[i].u.dsap,
           sysnetNotifyList.sysnetNotifyEntries[i].u.rxReason);
    printf("notify_pdu_receive callback = 0x%08x", (L7_uint32) sysnetNotifyList.sysnetNotifyEntries[i].notify_pdu_receive);
  }

  printf("ipMapArpRecvIP = 0x%08x\r\n", (L7_uint32) ipMapArpRecvIP);
  printf("ipMapRecvIP    = 0x%08x\r\n", (L7_uint32) ipMapRecvIP);

  return L7_SUCCESS;
}
#endif

/**********
** Debug **
**********/

L7_RC_t sysNetInterceptDebugShow()
{
  L7_uint32 i, j, k;

  if (sysnetPduHooksInitialized == L7_FALSE)
    return L7_FAILURE;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "Registered Interceptors:\n");
  for (i = 0; i < SYSNET_AF_MAX; i++)
  {
    for (j = 0; j < L7_SYSNET_PDU_MAX_HOOKS; j++)
    {
      if (sysnetPduHooks[i][j].numHooks != 0)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "   SysnetAF=%d(AF=%d), HookId=%d:\n",
                      i, sysnetPduAFMap[i], j);

        for (k = 0; k < L7_SYSNET_HOOK_PRECEDENCE_LAST; k++)
        {
          if (sysnetPduHooks[i][j].interceptFuncList[k] != L7_NULLPTR)
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                          "      name=%s precedence=%d func=%p\n",
                          sysnetPduHooks[i][j].interceptFuncNameList[k],
                          k,
                          sysnetPduHooks[i][j].interceptFuncList[k]);
        }
      }
    }
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
  return L7_SUCCESS;
}

L7_RC_t sysNetInterceptDebugCountersShow()
{
  L7_uint32 i, j;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\nSysnet Hook Return Code Counters:\n");
  for (i = 0; i < SYSNET_AF_MAX; i++)
  {
    if (sysnetPduAFMap[i] == 0xFFFFFFFF)
      continue;

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "  SysnetAF=%d (AF=%d)\n",
                  i, sysnetPduAFMap[i]);
    for (j = 0; j < L7_SYSNET_PDU_MAX_HOOKS; j++)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "    HookId=%d:\n", j);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "      Discard=%d Consumed=%d Copied=%d ",
                    sysnetPduRcCounters[i][j].sysnetPduRcDiscard,
                    sysnetPduRcCounters[i][j].sysnetPduRcConsumed,
                    sysnetPduRcCounters[i][j].sysnetPduRcCopied);

      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Ignored=%d Processed=%d Modified=%d\n",
                    sysnetPduRcCounters[i][j].sysnetPduRcIgnored,
                    sysnetPduRcCounters[i][j].sysnetPduRcProcessed,
                    sysnetPduRcCounters[i][j].sysnetPduRcModified);
    }
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Strip off the VLan Tagging to the Layer2 header.
*
*
* @param    *data       @b{(input)} Pointer to pdu that is received
*
* @returns  L7_SUCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_uint32 sysNetDataStripVlanTag(L7_netBufHandle bufHandle)
{
    L7_ushort16 protocol_type;
    L7_char8    *pEtype;
    L7_ushort16 temp16;
    L7_int32 index;
    L7_uint32 messageLen;
    L7_uchar8 *data;


    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data); /* get the data start */

    /* Determine the protocol type */
    pEtype = data + L7_ENET_HDR_SIZE;
    bcopy (pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

    temp16 = osapiNtohs(protocol_type);
    protocol_type = temp16;

    if (protocol_type == L7_ETYPE_8021Q)
    {
        /* Strip the VLAN header */
        for (index = L7_ENET_HDR_SIZE-1; index >= 0; index--)
        {
            /* Shift the Destination MAC Address and the Source MAC Address by 4 bytes.
            ** These 4 bytes are the VLAN header.
            */
            *(data + index + L7_8021Q_ENCAPS_HDR_SIZE) = *(data + index);
        }

        SYSAPI_NET_MBUF_SET_DATASTART(bufHandle, data + L7_8021Q_ENCAPS_HDR_SIZE);
        SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, messageLen);
        messageLen -= L7_8021Q_ENCAPS_HDR_SIZE;
        SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, messageLen);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}


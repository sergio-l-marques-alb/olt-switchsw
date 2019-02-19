/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename lldp_tlv.c
*
* @purpose 802.1AB functions for building/parsing TLVs
*
* @component 802.1AB
*
* @comments These functions hide all TLV formatting details.
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#define L7_MAC_LLDP_PDU

#include <string.h>
#include "l7_common.h"
#include "osapi_support.h"
#include "lldp_tlv.h"
#include "lldp_api.h"
#include "poe_api.h"
extern lldpCfgData_t  *lldpCfgData; 
const L7_uchar8 LLDP_MED_TIA_OUI_STRING[LLDP_TIA_OUI_STRING_LEN] = {0x00, 0x12, 0xBB};
const L7_uchar8 LLDP_8023_TIA_OUI_STRING[LLDP_TIA_OUI_STRING_LEN] = {0x00, 0x12, 0x0F};
const L7_uchar8 LLDP_8021_TIA_OUI_STRING[LLDP_TIA_OUI_STRING_LEN] = {0x00, 0x80, 0xC2};

extern L7_BOOL lldpMedDebugTxFlag;

/*********************************************************************
*
* @purpose  Write an LLDP TLV header to a LLDPDU
*
* @param    lldpTLVType_t   type    @b((input)) LLDP TLV type
* @param    L7_ushort16     length  @b((input)) length of information string
* @param    lldpPDUHandle_t pduHdl  @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if there is not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpTLVHeaderWrite(lldpTLVType_t    type,
                                  L7_ushort16      length,
                                  lldpPDUHandle_t *pduHdl)
{

  /* make sure there will be room for the entire TLV */
  if ((pduHdl->offset + LLDP_TLV_HEADER_SIZE + length) > (LLDP_PDU_SIZE_MAX - 1))
  {
    return L7_FAILURE;
  }

  /*
   * The TLV type field occupies the seven most significant bits of the
   * first octet of the TLV format. The least significant bit in the
   * first octet of the TLV format is the most significant bit of the
   * TLV information string length field.
   */

  pduHdl->buffer[pduHdl->offset++] = ((L7_uchar8)type << 1) | (L7_uchar8)(length >> 8);
  pduHdl->buffer[pduHdl->offset++] = (L7_uchar8)length;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Start an output LLDPDU buffer
*
* @param    lldpPDUHandle_t pduHdl    @b((input)) pointer to LLDP PDU type.
* @param    L7_uint32       intIfNum  @b((input)) NIM internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpPDUStart(lldpPDUHandle_t *pduHdl, L7_uint32 intIfNum)
{
  L7_uchar8         source[L7_MAC_ADDR_LEN];
  L7_uint32         addrType;
  L7_enet_encaps_t *encap;

  memset((void *)(pduHdl->buffer), 0, sizeof(pduHdl->buffer));
  pduHdl->offset = 0;


  /* get source MAC for interface */
  if (nimGetIntfAddrType(intIfNum, &addrType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (nimGetIntfAddress(intIfNum, addrType, source) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* set destination MAC address */
  memcpy(pduHdl->buffer, L7_ENET_LLDP_MULTICAST_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  /* set source MAC address */
  memcpy(pduHdl->buffer + L7_MAC_ADDR_LEN, source, L7_MAC_ADDR_LEN);
  pduHdl->offset += L7_ENET_HDR_SIZE;
  /* set ethertype */
  encap = (L7_enet_encaps_t *)(pduHdl->buffer + pduHdl->offset);
  encap->type = osapiHtons(L7_ETYPE_LLDP);
  pduHdl->offset += L7_ENET_ENCAPS_HDR_SIZE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  End an output LLDPDU buffer
*
* @param    lldpPDUHandle_t pduHdl  @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpPDUEnd(lldpPDUHandle_t *pduHdl)
{
  /* write the end of LLDPPDU */
  return lldpTLVHeaderWrite(LLDP_TLV_TYPE_END_OF_LLDPDU, 0, pduHdl);
}


/*********************************************************************
*
* @purpose  Write a Time to Live TLV to a LLDPPDU
*
* @param    L7_ushort16            ttl         @b((input)) time to live seconds
* @param    lldpPDUHandle_t        *pduHdl     @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVTTLWrite(L7_ushort16      ttl,
                        lldpPDUHandle_t *pduHdl)
{

  if (lldpTLVHeaderWrite(LLDP_TLV_TYPE_TIME_TO_LIVE,
                         LLDP_TLV_TTL_SIZE,
                         pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  *((L7_ushort16 *)(pduHdl->buffer + pduHdl->offset)) = osapiHtons(ttl);
  pduHdl->offset += LLDP_TLV_TTL_SIZE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Write an ID TLV to a LLDPPDU (id + string)
*
* @param    lldpTLVType_t     type      @b((input)) TLV type
* @param    L7_uchar8         subtype   @b((input)) id subtype
* @param    L7_char8          *id       @b((input)) id string
* @param    lldpPDUHandle_t   *pduHdl   @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpTLVIdWrite(lldpTLVType_t      type,
                              L7_uchar8          subtype,
                              L7_uchar8         *id,
                              L7_ushort16        length,
                              lldpPDUHandle_t   *pduHdl)
{
  /* ID field must be non-null */
  if (length < 1 ||
      length > LLDP_TLV_MGMT_STRING_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  if (lldpTLVHeaderWrite(type, LLDP_TLV_SUBTYPE_SIZE + length, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  pduHdl->buffer[pduHdl->offset] = subtype;
  pduHdl->offset += LLDP_TLV_SUBTYPE_SIZE;
  memcpy(pduHdl->buffer + pduHdl->offset, id, length);
  pduHdl->offset += length;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Write a Chassis ID TLV to a LLDPPDU
*
* @param    L7_uchar8         *subtype @b((input)) Chassis ID subtype
* @param    L7_uchar8         *id      @b((input)) The Chassis ID
* @param    L7_ushort16       length   @b((input)) The Chassis ID length
* @param    lldpPDUHandle_t   *pduHdl  @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVChassisIdWrite(L7_uchar8          subtype,
                              L7_uchar8         *id,
                              L7_ushort16        length,
                              lldpPDUHandle_t   *pduHdl)
{
  return lldpTLVIdWrite(LLDP_TLV_TYPE_CHASSIS_ID, subtype, id, length, pduHdl);
}

/*********************************************************************
*
* @purpose  Write a Port ID TLV to a LLDPPDU
*
* @param    L7_uchar8         subtype  @b((input)) Port ID subtype
* @param    L7_uchar8         *id      @b((input)) The Port ID
* @param    L7_ushort16       length   @b((input)) The Port ID length
* @param    lldpPDUHandle_t   *pduHdl  @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVPortIdWrite(L7_uchar8              subtype,
                           L7_uchar8             *id,
                           L7_ushort16            length,
                           lldpPDUHandle_t       *pduHdl)
{
  return lldpTLVIdWrite(LLDP_TLV_TYPE_PORT_ID, subtype, id, length, pduHdl);
}

/*********************************************************************
*
* @purpose  Write a Description TLV to a LLDPDU (string name or desc)
*
* @param    lldpTLVType_t     type    @b((input)) TLV type
* @param    L7_char8          *desc   @b((input)) string description
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpTLVDescWrite(lldpTLVType_t     type,
                                L7_char8          *desc,
                                lldpPDUHandle_t   *pduHdl)
{
  L7_ushort16 length = strlen(desc);

  if (length > LLDP_TLV_MGMT_STRING_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  if (lldpTLVHeaderWrite(type, length, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  memcpy(pduHdl->buffer + pduHdl->offset, desc, length);
  pduHdl->offset += length;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Write a Port Description TLV to a LLDPPDU
*
* @param    L7_char8          *desc   @b((input)) The Port Description
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVPortDescWrite(L7_char8          *desc,
                             lldpPDUHandle_t   *pduHdl)
{
  return lldpTLVDescWrite(LLDP_TLV_TYPE_PORT_DESC, desc, pduHdl);
}

/*********************************************************************
*
* @purpose  Write a System Name TLV to a LLDPPDU
*
* @param    L7_char8           *name   @b((input)) The system name
* @param    lldpPDUHandle_t    *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysNameWrite(L7_char8        *name,
                            lldpPDUHandle_t *pduHdl)
{
  return lldpTLVDescWrite(LLDP_TLV_TYPE_SYS_NAME, name, pduHdl);
}

/*********************************************************************
*
* @purpose  Write a System Description TLV to a LLDPDU
*
* @param    L7_char8          *desc   @b((input)) The system description
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysDescWrite(L7_char8        *desc,
                            lldpPDUHandle_t *pduHdl)
{
  return lldpTLVDescWrite(LLDP_TLV_TYPE_SYS_DESC, desc, pduHdl);
}


/*********************************************************************
*
* @purpose  Write a System Capabilities TLV to a LLDPPDU
*
* @param    L7_ushort16      supported  @b((input)) system capabilities supported mask
* @param    L7_ushort16      enabled    @b((input)) system capabilities enabled mask
* @param    lldpPDUHandle_t  *pduHdl    @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysCapWrite(L7_ushort16      supported,
                           L7_ushort16      enabled,
                           lldpPDUHandle_t *pduHdl)
{

  if (lldpTLVHeaderWrite(LLDP_TLV_TYPE_SYS_CAP,
                         LLDP_TLV_SYS_CAP_SIZE * 2,
                         pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  *((L7_ushort16 *)(pduHdl->buffer + pduHdl->offset)) = osapiHtons(supported);
  pduHdl->offset += LLDP_TLV_SYS_CAP_SIZE;
  *((L7_ushort16 *)(pduHdl->buffer + pduHdl->offset)) = osapiHtons(enabled);
  pduHdl->offset += LLDP_TLV_SYS_CAP_SIZE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Write a Management Address TLV to a LLDPPDU
*
* @param    lldpIANAAddrFamilyNumber_t  addrFamily  @b((input)) IANA address family (IPv4, etc.)
* @param    L7_uchar8                   *addr       @b((input)) management address
* @param    L7_ushort16                 *addrLen    @b((input)) management address length
* @param    lldpManAddrIfSubtype_t      intfSubtype @b((input)) management intf subtype
* @param    L7_uint32                   intfNum     @b((input)) management interface number
* @param    L7_char8                    *oid        @b((input)) management OID
* @param    lldpPDUHandle_t             *pduHdl     @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVMgmtAddrWrite(lldpIANAAddrFamilyNumber_t  addrFamily,
                             L7_uchar8                  *addr,
                             L7_ushort16                 addrLen,
                             lldpManAddrIfSubtype_t      intfSubtype,
                             L7_uint32                   intfNum,
                             L7_char8                   *oid,
                             lldpPDUHandle_t            *pduHdl)
{
  L7_ushort16 oidLen = strlen(oid);
  L7_ushort16 tlvLen = 0;

  if (addrLen < 1 ||
      addrLen > LLDP_TLV_MGMT_ADDR_SIZE_MAX ||
      oidLen  > LLDP_TLV_MGMT_ADDR_OID_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  tlvLen = LLDP_TLV_MGMT_ADDR_LEN_SIZE + \
           LLDP_TLV_SUBTYPE_SIZE + \
           addrLen + \
           LLDP_TLV_SUBTYPE_SIZE + \
           LLDP_TLV_INTF_NUM_SIZE + \
           LLDP_TLV_OID_LEN_SIZE + \
           oidLen;

  if (lldpTLVHeaderWrite(LLDP_TLV_TYPE_MGMT_ADDR,
                         tlvLen,
                         pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* management address string length = length of subtype + address */
  pduHdl->buffer[pduHdl->offset++] = (L7_uchar8)(LLDP_TLV_SUBTYPE_SIZE + addrLen);
  pduHdl->buffer[pduHdl->offset++] = (L7_uchar8)addrFamily;
  memcpy(pduHdl->buffer + pduHdl->offset, addr, addrLen);
  pduHdl->offset += addrLen;
  pduHdl->buffer[pduHdl->offset++] = (L7_uchar8)intfSubtype;
  intfNum = osapiHtonl(intfNum);
  memcpy((pduHdl->buffer + pduHdl->offset),&intfNum, sizeof(intfNum));
  pduHdl->offset += LLDP_TLV_INTF_NUM_SIZE;
  pduHdl->buffer[pduHdl->offset++] = (L7_uchar8)oidLen;
  memcpy(pduHdl->buffer + pduHdl->offset, oid, oidLen);
  pduHdl->offset += oidLen;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Read TLV Header from buffer
*
* @param    lldpTLVType_t   *type       @b((output)) LLDP TLV type
* @param    L7_ushort16     *length     @b((output)) length of information string
* @param    lldpPDUHandle_t *pduHdl     @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32        pduLength  @b((input))  total length of PDU buffer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV, or end of LLDPDU
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVHeaderRead(lldpTLVType_t   *type,
                          L7_ushort16     *length,
                          lldpPDUHandle_t *pduHdl,
                          L7_uint32        pduLength)
{
  if ((pduLength - pduHdl->offset) < LLDP_TLV_HEADER_SIZE)
  {
    return L7_FAILURE;
  }

  *type   = (lldpTLVType_t)(pduHdl->buffer[pduHdl->offset] >> 1);
  *length = (((L7_ushort16)(pduHdl->buffer[pduHdl->offset] & 0x01)) << 8) | \
            (L7_ushort16)(pduHdl->buffer[pduHdl->offset+1]);

  pduHdl->offset += LLDP_TLV_HEADER_SIZE;

  /* make sure we have that much left in PDU */
  if ((pduLength - pduHdl->offset) < (*length))
  {
    *type = 0;
    *length = 0;
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Read an ID TLV (subtype + id string)
*
* @param    L7_uchar8              *subtype  @b((output)) ID subtype
* @param    L7_uchar8              *id       @b((output)) remote Chassis ID
* @param    L7_ushort16            *idLength @b((output)) remote Chassis ID length
* @param    lldpPDUHandle_t        *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32              tlvLength @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments id buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*
*********************************************************************/
static L7_RC_t lldpTLVIdRead(L7_uchar8        *subtype,
                             L7_uchar8        *id,
                             L7_ushort16      *idLength,
                             lldpPDUHandle_t  *pduHdl,
                             L7_uint32         tlvLength)
{
  L7_uint32 length = tlvLength - LLDP_TLV_SUBTYPE_SIZE;
  if (length < 1 ||
      length > LLDP_TLV_MGMT_STRING_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  *subtype = pduHdl->buffer[pduHdl->offset];
  pduHdl->offset += LLDP_TLV_SUBTYPE_SIZE;
  memcpy(id, pduHdl->buffer + pduHdl->offset, length);
  pduHdl->offset += length;
  *idLength = length;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Read Chassis ID TLV
*
* @param    L7_uchar8             *subtype  @b((output)) remote Chassis ID subtype
* @param    L7_uchar8             *id       @b((output)) remote Chassis ID
* @param    L7_ushort16           *idLength @b((output)) remote Chassis ID length
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             tlvLength @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments id buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVChassisIdRead(L7_uchar8         *subtype,
                             L7_uchar8         *id,
                             L7_ushort16       *idLength,
                             lldpPDUHandle_t   *pduHdl,
                             L7_uint32          tlvLength)
{
  return lldpTLVIdRead(subtype, id, idLength, pduHdl, tlvLength);
}

/*********************************************************************
*
* @purpose  Read Port ID TLV
*
* @param    L7_uchar8             *subtype  @b((output)) remote Port ID subtype
* @param    L7_uchar8             *id       @b((output)) remote Chassis ID
* @param    L7_ushort16           *idLength @b((output)) remote Chassis ID length
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             tlvLength @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments id buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVPortIdRead(L7_uchar8            *subtype,
                          L7_char8             *id,
                          L7_ushort16          *idLength,
                          lldpPDUHandle_t      *pduHdl,
                          L7_uint32             tlvLength)
{
  return lldpTLVIdRead(subtype, id, idLength, pduHdl, tlvLength);
}

/*********************************************************************
*
* @purpose  Read Time-to-live TLV
*
* @param    L7_uint32             *ttl      @b((output)) remote time to live
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVTTLRead(L7_uint32        *ttl,
                       lldpPDUHandle_t  *pduHdl,
                       L7_uint32         length)
{
  L7_ushort16 tmp;
  if (length != LLDP_TLV_TTL_SIZE)
  {
    return L7_FAILURE;
  }

  memcpy(&tmp, (L7_ushort16 *)(pduHdl->buffer + pduHdl->offset), sizeof(L7_ushort16));
  *ttl = osapiNtohs(tmp);
 pduHdl->offset += LLDP_TLV_TTL_SIZE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Read a description TLV (no subtype field)
*
* @param    L7_char8          *desc     @b((output)) string
* @param    lldpPDUHandle_t   *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32         length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments id buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*
*********************************************************************/
static L7_RC_t lldpTLVDescRead(L7_char8         *desc,
                               lldpPDUHandle_t  *pduHdl,
                               L7_uint32         length)
{
  if (length > LLDP_TLV_MGMT_STRING_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  memcpy(desc, pduHdl->buffer + pduHdl->offset, length);
  desc[length] = '\0';
  pduHdl->offset += length;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Read port description TLV
*
* @param    L7_char8              *portDesc @b((output)) remote port description
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVPortDescRead(L7_char8         *portDesc,
                            lldpPDUHandle_t  *pduHdl,
                            L7_uint32         length)
{
  return lldpTLVDescRead(portDesc, pduHdl, length);
}

/*********************************************************************
*
* @purpose  Read system name TLV
*
* @param    L7_char8              *sysName  @b((output)) remote system name
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysNameRead(L7_char8         *sysName,
                           lldpPDUHandle_t  *pduHdl,
                           L7_uint32         length)
{
  return lldpTLVDescRead(sysName, pduHdl, length);
}

/*********************************************************************
*
* @purpose  Read system description TLV
*
* @param    L7_char8              *sysDesc  @b((output)) remote system description
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysDescRead(L7_char8         *sysDesc,
                           lldpPDUHandle_t  *pduHdl,
                           L7_uint32         length)
{
  return lldpTLVDescRead(sysDesc, pduHdl, length);
}

/*********************************************************************
*
* @purpose  Read system capabilities TLV
*
* @param    L7_ushort16        *supported @b((output)) system capabilities supported
* @param    L7_ushort16        *enabled   @b((output)) system capabilities enabled
* @param    lldpPDUHandle_t    *pduHdl    @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32           length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysCapRead(L7_ushort16      *supported,
                          L7_ushort16      *enabled,
                          lldpPDUHandle_t  *pduHdl,
                          L7_uint32         length)
{
  L7_ushort16 tmp;
  if (length != (2 * LLDP_TLV_SYS_CAP_SIZE))
  {
    return L7_FAILURE;
  }
  
  memcpy(&tmp,(L7_ushort16 *)(pduHdl->buffer + pduHdl->offset),sizeof( L7_ushort16 ));
  *supported = osapiNtohs(tmp);
  pduHdl->offset += LLDP_TLV_SYS_CAP_SIZE;
  memcpy(&tmp,(L7_ushort16 *)(pduHdl->buffer + pduHdl->offset),sizeof( L7_ushort16 ));
  *enabled =  osapiNtohs(tmp);
  pduHdl->offset += LLDP_TLV_SYS_CAP_SIZE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Read management address TLV
*
* @param    lldpMgmtAddrEntry_t *entry     @b((output))  remote management addr entry
* @param    lldpPDUHandle_t     *pduHdl    @b((input))   pointer to LLDP PDU type.
* @param    L7_uint32            length    @b((input))   length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVMgmtAddrRead(lldpMgmtAddrEntry_t *entry,
                            lldpPDUHandle_t     *pduHdl,
                            L7_uint32            length)
{
  L7_uchar8 oidLen = 0;
  L7_uint32 IntfNum;

  if (length > (LLDP_TLV_MGMT_ADDR_TLV_SIZE - LLDP_TLV_HEADER_SIZE))
  {
    return L7_FAILURE;
  }

  /* management address string length = length of subtype + address */
  entry->length =  pduHdl->buffer[pduHdl->offset] - LLDP_TLV_SUBTYPE_SIZE;
  pduHdl->offset += LLDP_TLV_MGMT_ADDR_LEN_SIZE;
  if (entry->length < 1 ||
      entry->length > LLDP_TLV_MGMT_ADDR_SIZE_MAX)
  {
    memset(entry, 0, sizeof(lldpMgmtAddrEntry_t));
    return L7_FAILURE;
  }

  entry->family = pduHdl->buffer[pduHdl->offset];
  pduHdl->offset += LLDP_TLV_SUBTYPE_SIZE;
  memcpy(entry->address, pduHdl->buffer + pduHdl->offset, entry->length);
  pduHdl->offset += entry->length;

  entry->ifSubtype = pduHdl->buffer[pduHdl->offset];
  pduHdl->offset += LLDP_TLV_SUBTYPE_SIZE;

  memcpy(&IntfNum,(pduHdl->buffer + pduHdl->offset), sizeof(IntfNum));
  entry->ifId = osapiNtohl(IntfNum);
  pduHdl->offset += LLDP_TLV_INTF_NUM_SIZE;

  oidLen = pduHdl->buffer[pduHdl->offset];
  pduHdl->offset += LLDP_TLV_OID_LEN_SIZE;

  if (oidLen > LLDP_TLV_MGMT_ADDR_OID_SIZE_MAX)
  {
    memset(entry, 0, sizeof(lldpMgmtAddrEntry_t));
    return L7_FAILURE;
  }
  memcpy(entry->oid, pduHdl->buffer + pduHdl->offset, oidLen);
  entry->oid[oidLen] = '\0';
  pduHdl->offset += oidLen;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Read organizationally defined TLV
*
* @param    lldpOrgDefInfoEntry_t *entry     @b((output)) remote organizationally defined TLV
* @param    lldpPDUHandle_t       *pduHdl    @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length     @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVOrgDefInfoRead(lldpOrgDefInfoEntry_t *entry,
                              lldpPDUHandle_t       *pduHdl,
                              L7_uint32              length)
{
  L7_uchar8 infoLen = 0;

  if (length > (LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX + LLDP_TLV_SUBTYPE_SIZE + LLDP_TLV_ORG_DEF_INFO_SIZE_MAX) ||
      length < (LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX + LLDP_TLV_SUBTYPE_SIZE))
  {
    return L7_FAILURE;
  }

  memcpy(entry->oui, pduHdl->buffer + pduHdl->offset, LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX);
  entry->oui[LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX] = '\0';
  pduHdl->offset += LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX;

  entry->subtype = pduHdl->buffer[pduHdl->offset++];

  infoLen = length - (LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX + LLDP_TLV_SUBTYPE_SIZE);
  entry->length = infoLen;
  memcpy(entry->info, pduHdl->buffer + pduHdl->offset, infoLen);
  pduHdl->offset += infoLen;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Read unknown TLV
*
* @param    lldpUnknownTLVEntry_t *entry     @b((output)) remote unknown TLV
* @param    lldpPDUHandle_t       *pduHdl    @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length     @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVUnknownRead(lldpUnknownTLVEntry_t *entry,
                           lldpPDUHandle_t       *pduHdl,
                           L7_uint32              length)
{
  if (length > LLDP_TLV_INFO_STRING_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  entry->length = length;
  memcpy(entry->info, pduHdl->buffer + pduHdl->offset, length);
  pduHdl->offset += length;

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Write a MED TLVs
*
* @param    L7_uint32 			index @((input)) index to the port 
* @param    L7_uint32 			intIfNum @((input)) Internal Interface Number 
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  
* @returns  
*
* @comments none
*
* @end
*
*********************************************************************/
void lldpMedTLVsWrite(L7_uint32 index,
					  L7_uint32 intIfNum,
					  lldpPDUHandle_t *pduHdl)
{
  L7_RC_t rc = L7_FAILURE;

  /* Ensure that there is space in the pdu */
  if (pduHdl->offset < (LLDP_PDU_SIZE_MAX - LLDP_TLV_END_OF_LLDPDU_TLV_SIZE))
	rc = L7_SUCCESS;
  else
	return;

  if (rc == L7_SUCCESS)
  {
	/* Mandatory for MED protocol */
	rc = lldp8023TLVsWrite(index, intIfNum, pduHdl);
  }
  /* Which TLVs are to be transmitted, check the medTLVsEnabled & medTLVsSupported for this interface */
  if (rc == L7_SUCCESS &&
	  lldpCfgData->intfCfgData[index].medTLVsSupported.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK &&
	  lldpCfgData->intfCfgData[index].medTLVsEnabled.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
  {
	/* Transmit the MED Capabilities TLV */
	rc = lldpMedCapTLVWrite(index, intIfNum, pduHdl);
  }
  /* Check for RC as we have to send a cap sup TLV. The other TLVs are dependant
   * on whether the information actually exists. So we can potentially have failure 
   * return codes
   */
  if (rc == L7_SUCCESS &&
	  lldpCfgData->intfCfgData[index].medTLVsSupported.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK &&
	  lldpCfgData->intfCfgData[index].medTLVsEnabled.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
  {
	/* Transmit the Network Policy TLV(s) */
	/* Ensure all the supported network policies are transmitted */
	/* Currently we support only the voice network policy */
	rc = lldpMedNetPolicyTLVWrite(index, intIfNum, pduHdl);
  }
  if (((lldpCfgData->intfCfgData[index].medTLVsSupported.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK) &&
      (lldpCfgData->intfCfgData[index].medTLVsEnabled.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)) ||
	   lldpMedDebugTxFlag == L7_TRUE)
  {
	/* Transmit the Location TLV */
	rc = lldpMedLocTLVWrite(index, intIfNum, pduHdl);
  }
  if (((lldpCfgData->intfCfgData[index].medTLVsSupported.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK) &&
      (lldpCfgData->intfCfgData[index].medTLVsEnabled.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)) ||
	   lldpMedDebugTxFlag == L7_TRUE)
  {
	/* Transmit the PSE TLV */
	rc = lldpMedPSETLVWrite(index, intIfNum, pduHdl);
  }
  if (((lldpCfgData->intfCfgData[index].medTLVsSupported.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK) &&
    (lldpCfgData->intfCfgData[index].medTLVsEnabled.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)) ||
     lldpMedDebugTxFlag == L7_TRUE)
  {
	/* Transmit the PD TLV */
	rc = lldpMedPDTLVWrite(index, intIfNum, pduHdl);
  }
  if (((lldpCfgData->intfCfgData[index].medTLVsSupported.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK) &&
    (lldpCfgData->intfCfgData[index].medTLVsEnabled.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)) ||
	   lldpMedDebugTxFlag == L7_TRUE)
  {
	/* Transmit the Inventory TLV */
	rc = lldpMedInventoryTLVsWrite(index, intIfNum, pduHdl);
  }
  return;
}
/*********************************************************************
*
* @purpose  Write a MED TLV to a LLDPPDU (id + string)
*
* @param    lldpTLVType_t     type      @b((input)) TLV type
* @param    L7_uchar8         subtype   @b((input)) id subtype
* @param    L7_char8          *id       @b((input)) id string
* @param    lldpPDUHandle_t   *pduHdl   @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpMedTLVWrite(lldpTLVType_t      type,
						  L7_uchar8          subtype,
						  L7_uchar8         *id,
						  L7_ushort16        length,
						  lldpPDUHandle_t   *pduHdl)
{
  /* ID field must be non-null */
  if (length < 1 ||
      length > LLDP_TLV_INFO_STRING_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  if (lldpMedTLVHeaderWrite(type, LLDP_TLV_SUBTYPE_SIZE + length, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  pduHdl->buffer[pduHdl->offset] = subtype;
  pduHdl->offset += LLDP_TLV_SUBTYPE_SIZE;
  memcpy(pduHdl->buffer + pduHdl->offset, id, length);
  pduHdl->offset += length;

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Write an LLDP TLV header to a LLDPDU
*
* @param    lldpTLVType_t   type    @b((input)) LLDP TLV type
* @param    L7_ushort16     length  @b((input)) length of information string
* @param    lldpPDUHandle_t pduHdl  @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if there is not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpMedTLVHeaderWrite(lldpTLVType_t    type,
                              L7_ushort16      length,
                              lldpPDUHandle_t *pduHdl)
{

  /* make sure there will be room for the entire TLV */
  if ((pduHdl->offset + LLDP_TLV_HEADER_SIZE + LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX + length) > (LLDP_PDU_SIZE_MAX - 1))
  {
    return L7_FAILURE;
  }

  /*
   * The TLV type field occupies the seven most significant bits of the
   * first octet of the TLV format. The least significant bit in the
   * first octet of the TLV format is the most significant bit of the
   * TLV information string length field.
   */

  pduHdl->buffer[pduHdl->offset++] = ((L7_uchar8)type << 1) | (L7_uchar8)(length >> 8);
  pduHdl->buffer[pduHdl->offset++] = (L7_uchar8)length + LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX;
  memcpy(&pduHdl->buffer[pduHdl->offset], LLDP_MED_TIA_OUI_STRING, LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX);
  pduHdl->offset += LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX;

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Write a 8023 TLV to a LLDPPDU (id + string)
*
* @param    lldpTLVType_t     type      @b((input)) TLV type
* @param    L7_uchar8         subtype   @b((input)) id subtype
* @param    L7_char8          *id       @b((input)) id string
* @param    lldpPDUHandle_t   *pduHdl   @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldp8023TLVWrite(lldpTLVType_t      type,
						 L7_uchar8          subtype,
						 L7_uchar8         *id,
						 L7_ushort16        length,
						 lldpPDUHandle_t   *pduHdl)
{
  /* ID field must be non-null */
  if (length < 1 ||
      length > LLDP_TLV_INFO_STRING_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  if (lldp8023TLVHeaderWrite(type, LLDP_TLV_SUBTYPE_SIZE + length, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  pduHdl->buffer[pduHdl->offset] = subtype;
  pduHdl->offset += LLDP_TLV_SUBTYPE_SIZE;
  memcpy(pduHdl->buffer + pduHdl->offset, id, length);
  pduHdl->offset += length;

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Write an LLDP 8023 TLV header to a LLDPDU
*
* @param    lldpTLVType_t   type    @b((input)) LLDP TLV type
* @param    L7_ushort16     length  @b((input)) length of information string
* @param    lldpPDUHandle_t pduHdl  @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if there is not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldp8023TLVHeaderWrite(lldpTLVType_t    type,
                               L7_ushort16      length,
                               lldpPDUHandle_t *pduHdl)
{

  /* make sure there will be room for the entire TLV */
  if ((pduHdl->offset + LLDP_TLV_HEADER_SIZE + LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX + length) > (LLDP_PDU_SIZE_MAX - 1))
  {
    return L7_FAILURE;
  }

  /*
   * The TLV type field occupies the seven most significant bits of the
   * first octet of the TLV format. The least significant bit in the
   * first octet of the TLV format is the most significant bit of the
   * TLV information string length field.
   */

  pduHdl->buffer[pduHdl->offset++] = ((L7_uchar8)type << 1) | (L7_uchar8)(length >> 8);
  pduHdl->buffer[pduHdl->offset++] = (L7_uchar8)length + LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX;
  memcpy(&pduHdl->buffer[pduHdl->offset], LLDP_8023_TIA_OUI_STRING, LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX);
  pduHdl->offset += LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX;

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Write 8023 TLVs
*
* @param    L7_uint32 			index @((input)) index to the port 
* @param    L7_uint32 			intIfNum @((input)) Internal Interface Number 
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldp8023TLVsWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl)
{
  /* Currently we only have to send the mac phy TLV if MEd is enabled.
   * If at a later stage more TLVs are to be transmitted a switch case can be added
   */
  lldp8023MacPhyTLVWrite(index, intIfNum, pduHdl);
  lldp8023StatefulCntlTLVWrite(index, intIfNum, pduHdl);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Write 8023 MAC Phy TLV
*
* @param    L7_uint32 			index @((input)) index to the port 
* @param    L7_uint32 			intIfNum @((input)) Internal Interface Number 
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldp8023MacPhyTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl)
{
  lldp8023MacPhyTLV_t data;
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL sup, enabled;
  L7_uint32 type;

  memset(&data,  0x00,  sizeof(lldp8023MacPhyTLV_t)); 

  do
  {
	rc = lldpXdot3LocPortAutoNegSupportedGet(intIfNum, &sup);
	if (rc != L7_SUCCESS)
	{
	  break;
	}
	rc = lldpXdot3LocPortAutoNegEnabledGet(intIfNum, &enabled);
	if (rc != L7_SUCCESS)
	{
	  break;
	}
	rc = lldpXdot3LocPortAutoNegAdvertizedCapGet(intIfNum, data.autoNegAdvt);
	if (rc != L7_SUCCESS)
	{
	  break;
	}
	rc = lldpXdot3LocPortAutoNegOperMauTypeGet(intIfNum, &type);
	if (rc != L7_SUCCESS)
	{
	  break;
	}

	if (sup == L7_TRUE)
	{
	  data.autoNegSupStatus[0] |= LLDP_8023_MAC_PHY_AUTO_NEG_SUP;
    }
	if (enabled == L7_TRUE)
	{
	  data.autoNegSupStatus[0] |= LLDP_8023_MAC_PHY_AUTO_NEG_STATUS;
	}
	/* auto nego advertized is already filled out */
	data.operMAUType[0] = 0x00;
	data.operMAUType[1] = (L7_uchar8)type;
    
	rc = lldp8023TLVWrite(LLDP_TLV_TYPE_ORG_DEF,
					      LLDP_8023_SUBTYPE_MAC_PHY,
					      (char*)&data,
					      sizeof(lldp8023MacPhyTLV_t),
					      pduHdl);


  } while (0);

  return rc;
}
/*********************************************************************
*
* @purpose  Write 8023 Stateful Control TLV
*
* @param    L7_uint32       index @((input)) index to the port
* @param    L7_uint32       intIfNum @((input)) Internal Interface Number
* @param    lldpPDUHandle_t *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldp8023StatefulCntlTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl)
{
  lldp8023StatefulCntlTLV_t data;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 deviceType, priority, powerAv, powerSrc, ackVal;
  L7_char8 deviceTypeTx=0, priorityTx=0, powerSrcTx=0, ackTx=0;
  L7_ushort16 pwValue;
  L7_ushort16 pwValNet;
  L7_uchar8 workByte;
  memset(&data,  0x00,  sizeof(lldp8023StatefulCntlTLV_t));

  do
  {
    rc = lldpXdot3PoePortReqPowerDeviceTypeGet(intIfNum, &deviceType);
   if (rc != L7_SUCCESS)
   {
      return rc;
    }
    rc = lldpXdot3PoePortReqPowerSourceGet(intIfNum, &powerSrc);
   if (rc != L7_SUCCESS)
   {
      return rc;
    }
    rc = lldpXdot3PoePortReqPowerPriorityGet(intIfNum, &priority);
    if (rc != L7_SUCCESS)
    {
      return rc;
    }
    rc = lldpXdot3PoePortReqPowerAvGet(intIfNum, &powerAv);
    if (rc != L7_SUCCESS)
    {
      return rc;
    }
    rc = lldpXdot3PoePortReqPowerAckGet(intIfNum, &ackVal);
    if (rc != L7_SUCCESS)
    {
      return rc;
    }

    /* The values returned for device type, priority and source are MIB tailored
     * The actual values to be transmitted are different so map it here to the
     * correct values
     */
    rc = L7_SUCCESS;
    switch (deviceType)
    {
      case pseDevice:
        deviceTypeTx = POWER_DEVICE_TYPE_PSE;
        break;
      case pdDevice:
        deviceTypeTx = POWER_DEVICE_TYPE_PD;
	   break;
	 default:
	   rc = L7_FAILURE;
	   break;
   }
   if (rc != L7_SUCCESS)
   {
	 /* We should not be transmitting a Ext Pwr TLV if we are not a PSE or PD device */
       return rc;
    }

    if(deviceType == pseDevice)
    {
      switch (powerSrc)
      {
        case unknown_psesrc:
          powerSrcTx = POWER_SOURCE_UNKNOWN;
          break;
        case primary_psesrc:
          powerSrcTx = POWER_SOURCE_PRIMARY;
          break;
        case backup_psesrc:
          powerSrcTx = POWER_SOURCE_BACKUP;
          break;
        default:
          rc = L7_FAILURE;
          break;
      }
    }
    else
    {
      switch (powerSrc)
      {
        case unknown_pdsrc:
          powerSrcTx = POWER_SOURCE_UNKNOWN;
          break;
        case fromPSE_pdsrc:
          powerSrcTx = POWER_SOURCE_PSE;
          break;
        case local_pdsrc:
          powerSrcTx = POWER_SOURCE_LOCAL;
          break;
        case localAndPSE_pdsrc:
          powerSrcTx = POWER_SOURCE_PSE_AND_LOCAL;
          break;
        default:
          rc = L7_FAILURE;
          break;
      }
   }
   if (rc != L7_SUCCESS)
   {
	 /* We should not be transmitting a Ext Pwr TLV if we cannot map the source */
      return rc;
   }

    switch (priority)
    {
      case unknown_pwpri:
        priorityTx = POWER_PRIORITY_UNKNOWN;
        break;
      case critical_pwpri:
        priorityTx = POWER_PRIORITY_CRITICAL;
        break;
      case high_pwpri:
        priorityTx = POWER_PRIORITY_HIGH;
        break;
      case low_pwpri:
        priorityTx = POWER_PRIORITY_LOW;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
    if (rc != L7_SUCCESS)
    {
      /* We should not be transmitting a Status Control TLV if we cannot map the priority */
      return rc;
    }

    switch(ackVal)
    {
      case ignore_pwack:
        ackTx = POWER_TLVACK_IGNORE;
        break;
      case ack_pwack:
        ackTx = POWER_TLVACK_ACK;
        break;
      case nack_pwack:
        ackTx = POWER_TLVACK_NACK;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
    if (rc != L7_SUCCESS)
    {
      /* We should not be transmitting a Status Control TLV if we cannot map the Ack */
      return rc;
    }

    /* Power Value */
    pwValue = (L7_ushort16)powerAv;

    pwValNet = osapiHtons(pwValue);
    memcpy(data.reqPowerValue, &pwValNet, sizeof(L7_ushort16));

    data.reqPowerBits[0] = (priorityTx|powerSrcTx|deviceTypeTx);

    /* Power Acknowledge */
    workByte = (L7_uchar8)ackTx;
    data.powerAck[0] = workByte;


    rc = lldp8023TLVWrite(LLDP_TLV_TYPE_ORG_DEF,
        LLDP_8023_SUBTYPE_POE_STATEFUL_CONTROL,
        (char*)&data,
        sizeof(lldp8023StatefulCntlTLV_t),
        pduHdl);

  } while (0);

  return rc;
}


/*********************************************************************
*
* @purpose  Write MED Capability TLV
*
* @param    L7_uint32       index @((input)) index to the port 
* @param    L7_uint32       intIfNum @((input)) Internal Interface Number 
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpMedCapTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl)
{
  lldpMedCapTLV_t data;
  L7_uint32 deviceClass = 0;
  L7_RC_t rc = L7_SUCCESS;
  lldpXMedCapabilities_t cap;

  memset(&data, 0x00, sizeof(data));
  memset(&cap,  0x00,  sizeof(cap));
  do
  {
    rc = lldpXMedPortCapSupportedGet(intIfNum, &cap);
    if (rc != L7_SUCCESS)
  {
    break;
  }
  memcpy(data.capabilities, cap.bitmap,  sizeof(lldpXMedCapabilities_t));
  rc = lldpXMedLocDeviceClassGet(&deviceClass);
  if (rc != L7_SUCCESS)
  {
    break;
  }
  data.deviceType[0] = (L7_uchar8)deviceClass;
  
  rc = lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
             LLDP_MED_SUBTYPE_CAP, 
             (L7_uchar8 *)&data, 
             sizeof(lldpMedCapTLV_t), 
             pduHdl); 

  }while(0);
  return rc;
}
/*********************************************************************
*
* @purpose  Write MED Network Policy TLV
*
* @param    L7_uint32       index @((input)) index to the port 
* @param    L7_uint32       intIfNum @((input)) Internal Interface Number 
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpMedNetPolicyTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl)
{
  lldpMedNetPolicyTLV_t data;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 dscp, pri, vid;
  L7_ushort16 vidShort, vidNetwork;
  L7_BOOL tagged, unknown;
  L7_uchar8 workByte, workByte1;
  L7_uint32 deviceType;

  memset(&data, 0x00, sizeof(data));
  /* Currently we only have voice as the network policy */
  /* Additonal policies can be coded here */
  rc = lldpXMedLocDeviceClassGet(&deviceType);
  if (rc != L7_SUCCESS)
  {
  return L7_FAILURE;
  }
  if (lldpXMedLocMediaPolicyAppSupportedGet(intIfNum, voice_policyapptype) == L7_TRUE)
  {
  do
  {
    rc = lldpXMedLocMediaPolicyDscpGet(intIfNum, voice_policyapptype, &dscp);
    if (rc != L7_SUCCESS)
    {
    break;
    }
    rc = lldpXMedLocMediaPolicyPriorityGet(intIfNum, voice_policyapptype, &pri);
    if (rc != L7_SUCCESS)
    {
    break;
    }
    rc = lldpXMedLocMediaPolicyTaggedGet(intIfNum, voice_policyapptype, &tagged);
    if (rc != L7_SUCCESS)
    {
    break;
    }
    rc = lldpXMedLocMediaPolicyUnknownGet(intIfNum, voice_policyapptype, &unknown);
    if (rc != L7_SUCCESS || (unknown == L7_TRUE && deviceType == networkConnectivity))
    {
    rc = L7_FAILURE;
    break;
    }
    rc = lldpXMedLocMediaPolicyVlanIDGet(intIfNum, voice_policyapptype, &vid);
    if (rc != L7_SUCCESS)
    {
    break;
    }

    data.appType[0] = (L7_uchar8)voice_policyapptype;
    if (unknown == L7_TRUE)
    {
    data.policyBits[0] |= LLDP_MED_NETWORK_POLICY_U;
    }
    if (tagged == L7_TRUE)
    {
    data.policyBits[0] |= LLDP_MED_NETWORK_POLICY_T;
    }
    /* Bit for X is already set to zero by the memset 0x00 */

    /* Vlan ID is a 12 bit value */

    vidShort = (L7_ushort16)vid;
    vidNetwork = osapiHtons(vidShort);
    /* Get the top 5 bits of the vid, remember to ignore the top 4 bits of the short */
    workByte = vidNetwork >> 7;

    data.policyBits[0] |= workByte;

    workByte = (vidNetwork << 9) >> 8;

    data.policyBits[1] |= workByte;

    /* Priority is a 3 bit value */

    workByte = (L7_uchar8) pri;
    workByte1 = (L7_uchar8) pri;
    workByte1 = workByte1 << 5;
    workByte1 = workByte1 >> 7;
    data.policyBits[1] |= workByte1;

    data.policyBits[2] |= (workByte << 6);

    /* DSCP is a 6 bit value */

    workByte = (L7_uchar8) dscp;
    workByte = workByte << 2;
    workByte = workByte >> 2;

    data.policyBits[2] |= workByte;

    rc = lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
               LLDP_MED_SUBTYPE_NET_POLICY, 
               (L7_uchar8 *)&data, 
               sizeof(lldpMedNetPolicyTLV_t), 
               pduHdl);  

  } while (0);
  }

  return rc;
}
/*********************************************************************
*
* @purpose  Write MED Location TLV
*
* @param    L7_uint32       index @((input)) index to the port 
* @param    L7_uint32       intIfNum @((input)) Internal Interface Number 
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpMedLocTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl)
{
  /* Not Supported currently */

  if (lldpMedDebugTxFlag == L7_TRUE)
  {
  lldpMedLocELINTLV_t data;
  memset(&data,  0x00,  sizeof(lldpMedLocELINTLV_t));

  lldpXMedLocLocationInfoGet(intIfNum, elin_locsubtype, data.locID);
  data.dataFormat[0] = (L7_uchar8)elin_locsubtype;

  lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF,
          LLDP_MED_SUBTYPE_LOCATION,
          (L7_uchar8 *)&data,
          sizeof(lldpMedLocELINTLV_t),
          pduHdl);
  }


  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Write MED Extended Power PSE TLV
*
* @param    L7_uint32       index @((input)) index to the port 
* @param    L7_uint32       intIfNum @((input)) Internal Interface Number 
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpMedPSETLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl)
{
  lldpMedExtPwrTLV_t data;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 deviceType, priority, powerAv, powerSrc;
  L7_uint32 deviceTypeTx=0, priorityTx=0, powerSrcTx=0;
  L7_ushort16 pwValue;
  L7_ushort16 pwValNet;

  if((lldpPoeIsValidIntf(intIfNum) == L7_FAILURE))
     return L7_FAILURE;

  if(lldpPoeIsValidPDIntf(intIfNum) == L7_SUCCESS)
     return L7_SUCCESS;
     
  memset(&data, 0x00, sizeof(data));

  do
  {
   rc = lldpXMedLocXPoeDeviceTypeGet(intIfNum,&deviceType);
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   rc = lldpXMedLocXPoePSEPortPriorityGet(intIfNum, &priority);
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   rc = lldpXMedLocXPoePSEPortPowerAvGet(intIfNum, &powerAv);
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   rc = lldpXMedLocXPoePSEPowerSourceGet(&powerSrc);
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   /* The values returned for device type, priority and source are MIB tailored
  * The actual values to be transmitted are different so map it here to the
  * correct values
  */
   rc = L7_SUCCESS;
   switch (deviceType)
   {
   case pseDevice:
     deviceTypeTx = POWER_DEVICE_TYPE_PSE;
     break;
   case pdDevice:
     deviceTypeTx = POWER_DEVICE_TYPE_PD;
     break;
   default:
     rc = L7_FAILURE;
     break;
   }
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   switch (priority)
   {
   case unknown_pwpri:
     priorityTx = POWER_PRIORITY_UNKNOWN;
     break;
   case critical_pwpri:
     priorityTx = POWER_PRIORITY_CRITICAL;
     break;
   case high_pwpri:
     priorityTx = POWER_PRIORITY_HIGH;
     break;
   case low_pwpri:
     priorityTx = POWER_PRIORITY_LOW;
     break;
   default:
     rc = L7_FAILURE;
     break;
   }
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   switch (powerSrc)
   {
   case unknown_psesrc:
     powerSrcTx = POWER_SOURCE_UNKNOWN;
     break;
   case primary_psesrc:
     powerSrcTx = POWER_SOURCE_PSE;
     break;
   case backup_psesrc:
     powerSrcTx = POWER_SOURCE_LOCAL;
     break;
   default:
     rc = L7_FAILURE;
     break;
   }
   if (rc != L7_SUCCESS)
   {
     return rc;
   }

   /* Power Value */
   pwValue = (L7_ushort16)powerAv;

   pwValNet = osapiHtons(pwValue);
   memcpy(data.powerValue, &pwValNet, sizeof(L7_ushort16));
   data.powerBits[0] = (deviceTypeTx|powerSrcTx|priorityTx);

   rc = lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
            LLDP_MED_SUBTYPE_EXT_POWER, 
            (L7_uchar8 *)&data, 
            sizeof(lldpMedExtPwrTLV_t), 
            pduHdl);  

  } while (0);

  return rc;
}
/*********************************************************************
*
* @purpose  Write MED Extended Power PD TLV
*
* @param    L7_uint32 			index @((input)) index to the port 
* @param    L7_uint32 			intIfNum @((input)) Internal Interface Number 
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpMedPDTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl)
{
  /* Device is a PSE device hence PD TLV should not be tramsitted */
  lldpMedExtPwrTLV_t data;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 deviceType, priority, powerAv, powerSrc;
  L7_uint32 deviceTypeTx=0, priorityTx=0, powerSrcTx=0;
  L7_ushort16 pwValue;
  L7_ushort16 pwValNet;

  if((lldpPoeIsValidIntf(intIfNum) == L7_FAILURE))
     return L7_FAILURE;

  if((lldpPoeIsValidPSEIntf(intIfNum) == L7_SUCCESS))
     return L7_SUCCESS;

  memset(&data, 0x00, sizeof(data));

  do
  {
   rc = lldpXMedLocXPoeDeviceTypeGet(intIfNum,&deviceType);
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   rc = lldpXMedLocXPoePDPowerPriorityGet(intIfNum,&priority);
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   rc = lldpXMedLocXPoePDPowerReqGet(intIfNum, &powerAv);
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   rc = lldpXMedLocXPoePDPowerSouceGet(intIfNum,&powerSrc);
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   /* The values returned for device type, priority and source are MIB tailored
  * The actual values to be transmitted are different so map it here to the
  * correct values
  */
   rc = L7_SUCCESS;
   switch (deviceType)
   {
   case pseDevice:
     deviceTypeTx = POWER_DEVICE_TYPE_PSE;
     break;
   case pdDevice:
     deviceTypeTx = POWER_DEVICE_TYPE_PD;
     break;
   default:
     rc = L7_FAILURE;
     break;
   }
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   switch (priority)
   {
   case unknown_pwpri:
     priorityTx = POWER_PRIORITY_UNKNOWN;
     break;
   case critical_pwpri:
     priorityTx = POWER_PRIORITY_CRITICAL;
     break;
   case high_pwpri:
     priorityTx = POWER_PRIORITY_HIGH;
     break;
   case low_pwpri:
     priorityTx = POWER_PRIORITY_LOW;
     break;
   default:
     rc = L7_FAILURE;
     break;
   }
   if (rc != L7_SUCCESS)
   {
     return rc;
   }
   switch (powerSrc)
   {
   case unknown_pdsrc:
     powerSrcTx = POWER_SOURCE_UNKNOWN;
     break;
   case fromPSE_pdsrc:
     powerSrcTx = POWER_SOURCE_PSE;
     break;
   case local_pdsrc:
     powerSrcTx = POWER_SOURCE_LOCAL;
     break;
   case localAndPSE_pdsrc:
     powerSrcTx = POWER_SOURCE_PSE_AND_LOCAL;
     break;
   default:
     rc = L7_FAILURE;
     break;
   }
   if (rc != L7_SUCCESS)
   {
     return rc;
   }

   /* Power Value */
   pwValue = (L7_ushort16)powerAv;

   pwValNet = osapiHtons(pwValue);
   memcpy(data.powerValue, &pwValNet, sizeof(L7_ushort16));

   data.powerBits[0] = (deviceTypeTx|powerSrcTx|priorityTx);

   rc = lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
            LLDP_MED_SUBTYPE_EXT_POWER, 
            (L7_uchar8 *)&data, 
            sizeof(lldpMedExtPwrTLV_t), 
            pduHdl);  
  } while (0);

  return rc;
}
/*********************************************************************
*
* @purpose  Write MED Inventory TLVs
*
* @param    L7_uint32 			index @((input)) index to the port 
* @param    L7_uint32 			intIfNum @((input)) Internal Interface Number 
* @param    lldpPDUHandle_t   *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpMedInventoryTLVsWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl)
{
  /* Since we have a partial entity MIB and SNMP, spec recommeds we not transmit the inventory TLV */

  if (lldpMedDebugTxFlag == L7_TRUE)
  {
	lldpMedInventoryTLV_t data;
	memset(&data,  0x00,  sizeof(lldpMedInventoryTLV_t));
    
	lldpXMedLocHardwareRevGet(data.data);
	lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
					LLDP_MED_SUBTYPE_INVENTORY_HW, 
					(L7_uchar8 *)&data, 
					sizeof(lldpMedInventoryTLV_t), 
					pduHdl);
    lldpXMedLocFirmwareRevGet(data.data);
	lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
					LLDP_MED_SUBTYPE_INVENTORY_FW, 
					(L7_uchar8 *)&data, 
					sizeof(lldpMedInventoryTLV_t), 
					pduHdl);
	lldpXMedLocSoftwareRevGet(data.data);
	lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
					LLDP_MED_SUBTYPE_INVENTORY_SW, 
					(L7_uchar8 *)&data, 
					sizeof(lldpMedInventoryTLV_t), 
					pduHdl);
	lldpXMedLocSerialNumGet(data.data);
	lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
					LLDP_MED_SUBTYPE_INVENTORY_SNO, 
					(L7_uchar8 *)&data, 
					sizeof(lldpMedInventoryTLV_t), 
					pduHdl);
	lldpXMedLocMfgNameGet(data.data);
	lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
					LLDP_MED_SUBTYPE_INVENTORY_MFG_NAME, 
					(L7_uchar8 *)&data, 
					sizeof(lldpMedInventoryTLV_t), 
					pduHdl);
	lldpXMedLocModelNameGet(data.data);
	lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
					LLDP_MED_SUBTYPE_INVENTORY_MODEL_NAME, 
					(L7_uchar8 *)&data, 
					sizeof(lldpMedInventoryTLV_t), 
					pduHdl);
	lldpXMedLocAssetIDGet(data.data);
	lldpMedTLVWrite(LLDP_TLV_TYPE_ORG_DEF, 
					LLDP_MED_SUBTYPE_INVENTORY_ASSET_ID, 
					(L7_uchar8 *)&data, 
					sizeof(lldpMedInventoryTLV_t), 
					pduHdl);
  }

  return L7_SUCCESS;
}

/*
 * $Id: pkt.c,v 1.15 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */

#include <shared/alloc.h>
#include <sal/core/libc.h>

#include <soc/cm.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <bcm/pkt.h>
#include <bcm_int/common/pkt.h>
#include <bcm_int/control.h>

#define UNIT_VALID(unit) \
{ \
  if (!BCM_UNIT_VALID(unit)) { return BCM_E_UNIT; } \
}

/*
 * Function:
 *      bcm_pkt_byte_index
 * Purpose:
 *      Return a pointer to a location in a packet
 * Parameters:
 *      pkt - The packet of interest
 *      n   - Byte offset in the packet
 *      len - (OUT) number of bytes left in this allocation block
 *      blk - (OUT) if not NULL, gets the pointer to the current block
 *      location - (OUT) pointer to the requested byte
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_pkt_byte_index(bcm_pkt_t *pkt, int n, int *len, int *blk_idx,
                   uint8 **location)
{
    int cnt = 0;
    int blk = 0;
    uint8 *ptr = NULL;
    int offset;

    for (blk = 0; blk < pkt->blk_count; blk++) {
        if (cnt + pkt->pkt_data[blk].len > n) {
            offset = n - cnt;
            ptr = &pkt->pkt_data[blk].data[offset];
            if (len) {
                *len = pkt->pkt_data[blk].len - offset;
            }
            if (blk_idx) {
                *blk_idx = blk;
            }
            break;
        } else {
            cnt += pkt->pkt_data[blk].len;
        }
    }

    *location = ptr;
    return (NULL !=ptr) ? BCM_E_NONE : BCM_E_NOT_FOUND;
}


/*
 * Function:
 *      bcm_pkt_memcpy
 * Purpose:
 *
 * Parameters:
 *      pkt - Packet to copy into
 *      dest_byte - Integer offset into packet
 *      src - Source buffer to copy from
 *      len - Number of bytes to copy
 * Returns:
 *      Number of bytes actually copied
 */

int
bcm_pkt_memcpy(bcm_pkt_t *pkt, int dest_byte, uint8 *src, int len)
{
    int blk;
    uint8 *ptr = NULL;
    int blk_bytes, rv;
    int copied = 0;
    
    rv = bcm_pkt_byte_index(pkt, dest_byte, &blk_bytes, &blk, &ptr);

    if ((BCM_E_NONE == rv) && (ptr)) {
        while (1) {
            if (blk_bytes > 0) {
                if (len - copied <= blk_bytes) { /* Last block */
                    sal_memcpy(ptr, &src[copied], len - copied);
                    copied = len;
                    break;
                }
                sal_memcpy(ptr, &src[copied], blk_bytes);
                copied += blk_bytes;
            }
            if (++blk >= pkt->blk_count) {
                break;
            }
            ptr = pkt->pkt_data[blk].data;
            blk_bytes = pkt->pkt_data[blk].len;
        }
    }

    return copied;
}

/*
 * Function:
 *      bcm_pkt_t_init
 * Purpose:
 *      Initialize packet structure.
 * Parameters:
 *      pkt - Pointer to packet structure.
 * Returns:
 *      NONE
 */
void
bcm_pkt_t_init(bcm_pkt_t *pkt)
{
    if (pkt != NULL) {
        sal_memset(pkt, 0, sizeof (*pkt));
    }
    return;
}

/*
 * Function:
 *      bcm_pkt_blk_t_init
 * Purpose:
 *      Initialize packet block structure.
 * Parameters:
 *      pkt_blk - Pointer to packet block structure.
 * Returns:
 *      NONE
 */
void
bcm_pkt_blk_t_init(bcm_pkt_blk_t *pkt_blk)
{
    if (pkt_blk != NULL) {
        sal_memset(pkt_blk, 0, sizeof (*pkt_blk));
    }
    return;
}

/*
 * Function:
 *      bcm_pkt_clear
 * Purpose:
 *      Clear a packet structure and install its data buffer
 * Parameters:
 *      unit - for what unit is this being set up
 *      pkt - pointer to pointer packet to setup.  (*pkt) may be NULL
 *      blks - Pointer to array of gather blocks for the packet
 *      blk_count - Number of elts in the array
 *      flags - flags for CRC and VLAN tag.  See notes.
 *      pkt_buf (OUT) - pkt, or allocated packet if pkt is NULL
 * Returns:
 *      Pointer to packet or NULL if cannot allocate new packet.
 * Notes:
 *      If pkt is null, allocate a new one with sal_alloc.
 *
 *      Flags can be the bitwise or of:
 *        BCM_TX_CRC_NONE        No action on CRC
 *        BCM_TX_CRC_ALLOC       CRC is not in packet; allocate
 *        BCM_TX_CRC_REGEN       Regenerate the CRC on egress
 *        BCM_TX_CRC_APPEND      Allocate and regenerate (same as alloc)
 *        BCM_PKT_F_NO_VTAG      Packet does not contain vlan tag
 *
 *      In addition, the following may be used to override normal
 *      behavior, but are not generally advertised or documented.
 *        BCM_TX_CRC_FORCE_ERROR Force an error in the CRC (unsupported)
 *        BCM_PKT_F_HGHDR        HiGig header setup for packet DMA
 *        BCM_PKT_F_SLTAG        SL tag setup for packet DMA
 *
 *      The payload length of the packet is set as large as possible
 *      assuming the allocation blocks contain the MAC addresses and
 *      (unless NO_VTAG is specified) the VLAN tag; if CRC append (or alloc)
 *      is indicated, the CRC is assumed NOT to be part of the gather
 *      blocks.
 */

int
bcm_pkt_clear(int unit, bcm_pkt_t *pkt, bcm_pkt_blk_t *blks, int blk_count,
              uint32 flags, bcm_pkt_t **pkt_buf)
{
    int rv, i;
    int bytes = 0;
    int local_alloc = FALSE;

    UNIT_VALID(unit);

    if (pkt == NULL) {  /* Allocate new packet */
        local_alloc = TRUE;
        if ((pkt = sal_alloc(sizeof(bcm_pkt_t), "pkt_setup")) == NULL) {
            *pkt_buf = NULL;
            return BCM_E_MEMORY;
        }
    }
    sal_memset(pkt, 0, sizeof(bcm_pkt_t));
    pkt->unit = unit;

    if (blk_count == 0) {
        /*
         * Permit allocation of an empty packet structure.
         * A single buffer can be added alter using the
         * BCM_PKT_ONE_BUF_SETUP macro.
         */
        bcm_pkt_flags_init(unit, pkt, flags);
    } else {
        for (i = 0; i < blk_count; i++) {
            bytes += blks[i].len;
        }

        if (bytes < BCM_PKT_ALLOC_MIN) {
            *pkt_buf = NULL;
            if (local_alloc) {
                sal_free(pkt);
            }
            return BCM_E_MEMORY;
        }

        pkt->pkt_data = blks;
        pkt->blk_count = blk_count;

        rv = bcm_pkt_flags_len_setup(unit, pkt, bytes, -1, flags);
        if (BCM_FAILURE(rv)) {
            *pkt_buf = NULL;
            if (local_alloc) {
                sal_free(pkt);
            }
            return rv;
        }
    }

    *pkt_buf = pkt;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_pkt_flags_len_setup
 * Purpose:
 *      Install data in a packet without clearing fields
 * Parameters:
 *      unit - for what unit is this being set up
 *      pkt - the packet to setup
 *      buf - buffer to use; must be non-null
 *      alloc_bytes - buffer length (allocated)
 *      payload_len - Length of payload.  See notes
 *      flags - See above.
 * Returns:
 *      >= 0:  Payload length
 *      < 0:   BCM_E_XXX
 * Notes:
 *      If payload_len < 0, this means make payload as large as possible.
 *
 *      The payload_len is checked because space for the VLAN tag,
 *      HiGig header and SL tag is always allocated.
 *
 *      The payload length includes everything in the packet EXCEPT:
 *          dest and src MAC;
 *          VLAN tag (4 bytes)
 *          CRC
 *          SL Tag
 *          HiGig header
 *      The txrx length includes all of the above.
 *      The packet length value depends on the format indications.
 *      It will exclude the VLAN tag if indicated in flags; it will
 *      exclude the CRC if CRC_ALLOC is indicated.
 *
 *      Valid values for CRC are:
 *         BCM_TX_CRC_NONE, BCM_TX_CRC_ALLOC, BCM_TX_CRC_REGEN,
 *         BCM_TX_CRC_APPEND.  These maybe combined with
 *         BCM_TX_CRC_FORCE_ERROR,
 */

int
bcm_pkt_flags_len_setup(int unit, bcm_pkt_t *pkt, int alloc_bytes,
                        int payload_len, uint32 flags)
{
    UNIT_VALID(unit);

    if (payload_len < 0) {
        payload_len = alloc_bytes - BCM_PKT_ALLOC_MIN;
    } else if (payload_len > alloc_bytes - BCM_PKT_ALLOC_MIN) {
        return BCM_E_PARAM;
    }

    bcm_pkt_flags_init(unit, pkt, flags);

    return payload_len;
}


int
bcm_pkt_flags_init(int unit, bcm_pkt_t *pkt, uint32 flags)
{
    int rv;

    BCM_UNIT_BUSY(unit);
    if (BCM_UNIT_VALID(unit)) {
        pkt->flags = flags;
        pkt->unit = unit;   /* Set unit as well */

        if (BCM_IS_LOCAL(unit)) {
            if (SOC_UNIT_VALID(unit) && SOC_IS_XGS12_FABRIC(unit)) {
                pkt->flags |= BCM_PKT_F_HGHDR;
            }
            /* Was SL flags handling, but SL is no longer supported */
        }
        rv = BCM_E_NONE;
        
    } else {
        rv = BCM_E_UNIT;
    }
    BCM_UNIT_IDLE(unit);
    
    return rv;
}





/****************************************************************
 *
 * Default allocate and free routines.  Uses soc_cm_ shared
 * memory routines.  These use single data buffers and do
 * runtime allocation and free.
 *
 ****************************************************************/

#define DEFAULT_ALLOCATOR_UNIT 0
#define NO_ALLOCATOR_UNIT -1

/*
  Return a local unit number (or NO_ALLOCATOR_UNIT) that can allocate
  a buffer for the given unit.

  For local units, the unit returned is the unit passed. For remote
  units, return DEFAULT_ALLOCATOR_UNIT if DEFAULT_ALLOCATOR_UNIT unit
  itself is valid and local, otherwise return NO_ALLOCATOR_UNIT (which
  usually means tunneling to a remote unit when there are no local
  units, which is a valid configuration).

*/

static int
_bcm_pkt_allocator_unit(int unit)
{
    /* allocation unit */
    if (BCM_IS_REMOTE(unit)) {
        unit = (BCM_UNIT_VALID(DEFAULT_ALLOCATOR_UNIT) &&
                BCM_IS_LOCAL(DEFAULT_ALLOCATOR_UNIT)) ?
            DEFAULT_ALLOCATOR_UNIT : NO_ALLOCATOR_UNIT;
    }

    return unit;
}
 
/* could be bcm_pkt_blk_t_alloc */

static int
_bcm_pkt_data_alloc(int unit, int size, bcm_pkt_blk_t *pkt_data)
{
    int	aunit;
    int rv = BCM_E_MEMORY;

    /* allocation unit, which may not be the same as unit */
    aunit = _bcm_pkt_allocator_unit(unit);

    BCM_UNIT_BUSY(aunit);
    /* If aunit == NO_ALLOCATOR_UNIT, use a heap allocator, because
       there are no DMAable devices available. */
    pkt_data->data = (aunit == NO_ALLOCATOR_UNIT) ?
        sal_alloc(size, "pkt alloc data") :
        soc_cm_salloc(aunit, size, "pkt alloc data");

    if (pkt_data->data) {
        pkt_data->len = size;
        rv = BCM_E_NONE;
    }
    BCM_UNIT_IDLE(aunit);

    return rv; 
}

static int
_bcm_pkt_data_free(int unit, bcm_pkt_blk_t *pkt_data)
{
    int	aunit;

    /* allocation unit, which may not be the same as unit */
    aunit = _bcm_pkt_allocator_unit(unit);
    BCM_UNIT_BUSY(aunit);
    
    if (aunit == NO_ALLOCATOR_UNIT) {
        sal_free(pkt_data->data);
    } else {
        soc_cm_sfree(aunit, pkt_data->data);
    }
    pkt_data->data = NULL;
    pkt_data->len = 0;
    BCM_UNIT_IDLE(aunit);

    return BCM_E_NONE; 
}



/*
 * Function:
 *      bcm_pkt_alloc
 * Purpose:
 *      Basic, single block packet allocation using sal and soc functions
 * Parameters:
 *      unit - Strata unit
 *      size - Bytes to allocate
 *      flags - See pkt.h for more about these flags
 * Returns:
 *      Pointer to packet if successful or NULL if not
 * Notes:
 *	Flags are copied into the packet, and so may indicate if CRC/VLAN
 *	should be stripped.
 *
 *      If the unit allocator returns NO_ALLOCATOR_UNIT, then assume
 *      that a DMA allocator is not required, and use a regular memory
 *      allocator.
 */

int
bcm_pkt_alloc(int unit, int size, uint32 flags, bcm_pkt_t **pkt_buf)
{
    bcm_pkt_t *pkt;
    int rv = BCM_E_INTERNAL;

    UNIT_VALID(unit);

    pkt = sal_alloc(sizeof(bcm_pkt_t), "bcm_pkt_alloc");
    if (!pkt) {
        *pkt_buf = NULL;
        return BCM_E_MEMORY;
    }
    sal_memset(pkt, 0, sizeof(bcm_pkt_t));
    pkt->pkt_data = &pkt->_pkt_data;
    pkt->blk_count = 1;

    rv = _bcm_pkt_data_alloc(unit, size, pkt->pkt_data);

    if (BCM_FAILURE(rv)) {
        sal_free(pkt);
        *pkt_buf = NULL;
        return rv;
    }
    bcm_pkt_flags_init(unit, pkt, flags);

    *pkt_buf = pkt;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_pkt_free
 * Purpose:
 *      Basic, free routine using sal and soc functions
 * Parameters:
 *      unit - Strata unit
 *      size - Bytes to allocate
 *      flags - See pkt.h for more about these flags
 * Returns:
 * Notes:
 *      Works with bcm_pkt_alloc.  Checks for multiple blocks,
 *      so could be used with other routines that use sal_alloc
 *      and soc_cm_salloc.
 */

int
bcm_pkt_free(int unit, bcm_pkt_t *pkt)
{
    int i;

    UNIT_VALID(unit);

    if (pkt) {
        for (i = 0; i < pkt->blk_count; i++) {
            if (pkt->pkt_data[i].data) {
                _bcm_pkt_data_free(unit, &pkt->pkt_data[i]);
            }
        }
        sal_free(pkt);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_pkt_blk_alloc
 * Purpose:
 *      Allocate an array of packets
 * Parameters:
 *      unit - Strata device number
 *      count - How many packets to alloc
 *      size - Bytes in each packet
 *      flags - flags to use for packet alloc
 * Returns:
 *      Pointer to allocated structure or NULL if fails
 */

int
bcm_pkt_blk_alloc(int unit, int count, int size, uint32 flags, 
                  bcm_pkt_t ***packet_array)
{
    bcm_pkt_t          **p_array;
    int i, j;

    UNIT_VALID(unit);

    if (!(p_array = sal_alloc(count * sizeof(bcm_pkt_t *), "pkt_blk"))) {
        *packet_array = NULL;
        return (BCM_E_MEMORY);
    }

    for (i = 0; i < count; i++) {
        if (BCM_FAILURE(bcm_pkt_alloc(unit, size, flags, p_array + i))) {

            for (j = 0; j < i; j++) {
                bcm_pkt_free(unit, p_array[j]);
            }
            sal_free(p_array);
            *packet_array = NULL;
            return (BCM_E_MEMORY);
        }
    }

    *packet_array = p_array;
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_pkt_blk_free
 * Purpose:
 *      Free an array of packets allocated with bcm_pkt_blk_alloc
 * Parameters:
 *      unit - Strata device number
 *      pkts - pointer to array of packets
 *      count - How many packets to alloc
 * Returns:
 */

int 
bcm_pkt_blk_free(int unit, bcm_pkt_t **pkts, int count)
{
    int i;

    UNIT_VALID(unit);

    if (pkts) {
        for (i = 0; i < count; i++) {
            if (pkts[i]) {
                bcm_pkt_free(unit, pkts[i]);
            }
        }
        sal_free(pkts);
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_pkt_rx_alloc
 * Purpose:
 *      Allocate a packet using RX alloc
 * Parameters:
 *      len    - length of data buffer
 * Returns:
 *      Pointer to packet or NULL if memory error
 * Notes:
 *      Sets up the packet to have a single buffer.
 */

int
bcm_pkt_rx_alloc(int unit, int len, bcm_pkt_t **pkt_buf)
{
    bcm_pkt_t *pkt;
    uint8 *buf;
    int rv;

    UNIT_VALID(unit);

    buf = NULL;
    pkt = sal_alloc(sizeof(bcm_pkt_t), "pkt_rx_alloc");
    if (pkt == NULL) {
        *pkt_buf = NULL;
        return BCM_E_MEMORY;
    }
    sal_memset(pkt, 0, sizeof(bcm_pkt_t));

    if (len > 0) {
        rv = bcm_rx_alloc(unit, len, 0, (void*)&buf);
        if (rv != BCM_E_NONE) {
            sal_free(pkt);
            return rv;
        }
        pkt->_pkt_data.data = buf;
        pkt->_pkt_data.len = len;
        pkt->pkt_len = len;
        pkt->pkt_data = &pkt->_pkt_data;
        pkt->blk_count = 1;
    }

    /* Set up flags here if possible */

    *pkt_buf = pkt;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_pkt_rx_free
 * Purpose:
 *      Free a packet allocated using bcm_pkt_rx_alloc
 * Parameters:
 *      pkt    - packet to free
 * Returns:
 * Notes:
 *      Checks for null pkt and buffer
 */

int
bcm_pkt_rx_free(int unit, bcm_pkt_t *pkt)
{
    UNIT_VALID(unit);

    if (pkt) {
        if (pkt->_pkt_data.data) {
            bcm_rx_free(unit, pkt->_pkt_data.data);
        }
        sal_free(pkt);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_rx_reasons_t_init
 * Purpose:
 *      Initialize RX reasons structure.
 * Parameters:
 *      reasons - (INOUT) Pointer to the structure to be initialized.
 * Returns:
 *      NONE
 */
void
bcm_rx_reasons_t_init(bcm_rx_reasons_t *reasons)
{
    if (reasons != NULL) {
        sal_memset(reasons, 0, sizeof (*reasons));
    }
    return;
}
char*
  bcm_pkt_dnx_type_to_string(
    bcm_pkt_dnx_type_t dnx_hdr_type
  )
{
  char* str = NULL;
  switch(dnx_hdr_type)
  {
    case bcmPktDnxTypePtch1:
        str = "ptch1";
        break;
    case bcmPktDnxTypePtch2:
        str = "ptch2";
        break;
    case bcmPktDnxTypeItmh:
        str = "itmh";
        break;
    case bcmPktDnxTypeFtmh:
        str = "ftmh";
        break;
    case bcmPktDnxTypePph:
        str = "pph";
        break;
    case bcmPktDnxTypeOtsh:
        str = "otsh";
        break;
    case bcmPktDnxTypeOtmh:
        str = "otmh";
        break;
    case bcmPktDnxTypeRaw:
        str = "raw";
        break;
    default:
        str = " Unknown";
  }
  return str;
}
void bcm_pkt_dnx_ptch1_dump(
   bcm_pkt_dnx_ptch1_t  *ptch1
   )
{

    LOG_CLI((BSL_META("src_gport: %u\n\r"),ptch1->src_gport));
    LOG_CLI((BSL_META("opaque_attr: %u\n\r"),ptch1->opaque_attr));
    LOG_CLI((BSL_META("is_port_header_type: %u\n\r"),ptch1->is_port_header_type));

}
void bcm_pkt_dnx_ptch2_dump(
   bcm_pkt_dnx_ptch2_t  *ptch2
   )
{

    LOG_CLI((BSL_META("src_gport: %u\n\r"),ptch2->src_local_port));
    LOG_CLI((BSL_META("opaque_attr: %u\n\r"),ptch2->opaque_attr));
    LOG_CLI((BSL_META("is_port_header_type: %u\n\r"),ptch2->is_port_header_type));

}
char*
  bcm_pkt_dnx_itmh_dest_type_to_string(
     bcm_pkt_dnx_type_t dest_type
  )
{
  char* str = NULL;
  switch(dest_type)
  {
    case bcmPktDnxItmhDestTypeMulticast:
        str = "Multicast";
        break;
    case bcmPktDnxItmhDestTypeFlow:
        str = "Flow";
        break;
    case bcmPktDnxItmhDestTypeIngressShapingFlow:
        str = "IngressShapingFlow";
        break;
    case bcmPktDnxItmhDestTypeVport:
        str = "Vport";
        break;
    case bcmPktDnxItmhDestTypeSystemPort:
        str = "SystemPort";
        break;
    default:
        str = " Unknown";
  }
  return str;
}

void bcm_pkt_dnx_itmh_dest_dump(
                        bcm_pkt_dnx_itmh_dest_t *itmh_dest
                        )
{

    LOG_CLI((BSL_META("dest_type: %s \n\r"), bcm_pkt_dnx_itmh_dest_type_to_string(itmh_dest->dest_type)));
    LOG_CLI((BSL_META("dest_extension_type: %s \n\r"), bcm_pkt_dnx_itmh_dest_type_to_string(itmh_dest->dest_extension_type)));
    LOG_CLI((BSL_META("destination: %u \n\r"), itmh_dest->destination));
    LOG_CLI((BSL_META("multicast_id: %u \n\r"), itmh_dest->multicast_id));
    LOG_CLI((BSL_META("destination_ext: %u \n\r"), itmh_dest->destination_ext));


}
void bcm_pkt_dnx_itmh_dump(
   bcm_pkt_dnx_itmh_t  *itmh
   )
{

    LOG_CLI((BSL_META("inbound_mirror_disable: %u\n\r"),itmh->inbound_mirror_disable));
    LOG_CLI((BSL_META("snoop_cmnd: %u\n\r"),itmh->snoop_cmnd));
    LOG_CLI((BSL_META("prio: %u\n\r"),itmh->prio));
    LOG_CLI((BSL_META("color: %u\n\r"),itmh->color));

    bcm_pkt_dnx_itmh_dest_dump(&(itmh->dest));


}
char*
 bcm_pkt_dnx_ftmh_action_type_to_string(
      bcm_pkt_dnx_ftmh_action_type_t action_type
  )
{
  char* str = NULL;
  switch(action_type)
  {
    case bcmPktDnxFtmhActionTypeForward:
        str = "Forward";
        break;
    case bcmPktDnxFtmhActionTypeSnoop:
        str = "Snoop";
        break;
    case bcmPktDnxFtmhActionTypeInboundMirror:
        str = "InboundMirror";
        break;
    case bcmPktDnxFtmhActionTypeOutboundMirror:
        str = "OutboundMirror";
        break;
    default:
        str = "Unknown";
  }
  return str;
}


void bcm_pkt_dnx_ftmh_lb_extension_dump(
   bcm_pkt_dnx_ftmh_lb_extension_t  *lb_extension
   )
{

    LOG_CLI((BSL_META("valid: %s\n\r"),"lb_extension"));
    LOG_CLI((BSL_META("valid: %u\n\r"),lb_extension->valid));
    LOG_CLI((BSL_META("lb_key: %u\n\r"),lb_extension->lb_key));


}

void bcm_pkt_dnx_ftmh_dest_extension_dump(
   bcm_pkt_dnx_ftmh_dest_extension_t  *dest_extension
   )
{

    LOG_CLI((BSL_META("dest_extension:\n\r")));
    LOG_CLI((BSL_META("valid: %u\n\r"),dest_extension->valid));
    LOG_CLI((BSL_META("lb_key: %u\n\r"),dest_extension->dst_sysport));


}
void bcm_pkt_dnx_ftmh_stack_extension_dump(
   bcm_pkt_dnx_ftmh_stack_extension_t  *stack_extension
   )
{

    LOG_CLI((BSL_META("stack_extension:\n\r")));
    LOG_CLI((BSL_META("valid: %u\n\r"),stack_extension->valid));
    LOG_CLI((BSL_META("stack_route_history_bmp: %u\n\r"),stack_extension->stack_route_history_bmp));


}

void bcm_pkt_dnx_ftmh_dump(
   bcm_pkt_dnx_ftmh_t  *ftmh
   )
{

    LOG_CLI((BSL_META("packet_size: %u\n\r"),ftmh->packet_size));
    LOG_CLI((BSL_META("prio: %u\n\r"),ftmh->prio));
    LOG_CLI((BSL_META("src_sysport: %u\n\r"),ftmh->src_sysport));
    LOG_CLI((BSL_META("dst_port: %u\n\r"),ftmh->dst_port));
    LOG_CLI((BSL_META("ftmh_dp: %u\n\r"),ftmh->ftmh_dp));
    LOG_CLI((BSL_META("action_type: %s \n\r"), bcm_pkt_dnx_ftmh_action_type_to_string(ftmh->action_type)));
    LOG_CLI((BSL_META("out_mirror_disable: %u\n\r"),ftmh->out_mirror_disable));
    LOG_CLI((BSL_META("is_mc_traffic: %u\n\r"),ftmh->is_mc_traffic));
    LOG_CLI((BSL_META("multicast_id: %u\n\r"),ftmh->multicast_id));
    LOG_CLI((BSL_META("out_vport: %u\n\r"),ftmh->out_vport));
    LOG_CLI((BSL_META("cni: %u\n\r"),ftmh->cni));

    bcm_pkt_dnx_ftmh_lb_extension_dump(&(ftmh->lb_ext));
    bcm_pkt_dnx_ftmh_dest_extension_dump(&(ftmh->dest_ext));
    bcm_pkt_dnx_ftmh_stack_extension_dump(&(ftmh->stack_ext));

}
void bcm_pkt_dnx_pph_eei_extension_dump(
   bcm_pkt_dnx_pph_eei_extension_t  *ext
   )
{

    LOG_CLI((BSL_META("pph_eei_extension:\n\r")));
    LOG_CLI((BSL_META("valid: %u\n\r"),ext->valid));
    LOG_CLI((BSL_META("is_mim: %u\n\r"),ext->is_mim));
    LOG_CLI((BSL_META("i_sid: %u\n\r"),ext->i_sid));
    LOG_CLI((BSL_META("command: %u\n\r"),ext->command));
    LOG_CLI((BSL_META("data: %u\n\r"),ext->data));


}
void bcm_pkt_dnx_pph_learn_extension_dump(
   bcm_pkt_dnx_pph_learn_extension_t  *ext
   )
{

    LOG_CLI((BSL_META("pph_learn_extension:\n\r")));
    LOG_CLI((BSL_META("valid: %u\n\r"),ext->valid));
    LOG_CLI((BSL_META("is_mim: %u\n\r"),ext->phy_gport));
    LOG_CLI((BSL_META("i_sid: %u\n\r"),ext->is_eei_valid));
    bcm_pkt_dnx_pph_eei_extension_dump(&(ext->eei));
    LOG_CLI((BSL_META("is_encap_id_valid: %u\n\r"),ext->is_encap_id_valid));
    /*LOG_CLI((BSL_META("raw_data: %u\n\r"),ext->raw_data));*/ 


}
 char*
 bcm_pkt_dnx_pph_fhei_type_to_string(
      bcm_pkt_dnx_pph_fhei_type_t fhei_type
  )
{
  char* str = NULL;
  switch(fhei_type)
  {
    case bcmPktDnxPphFheiTypeBridge:
        str = "Bridge";
        break;
    case bcmPktDnxPphFheiTypeTrap:
        str = "Trap";
        break;
    case bcmPktDnxPphFheiTypeIp:
        str = "Ip";
        break;
    case bcmPktDnxPphFheiTypeMpls:
        str = "Mpls";
        break;
  case bcmPktDnxPphFheiTypeTrill:
      str = "Trill";
      break;
    default:
        str = "Unknown";
  }
  return str;
}
void bcm_pkt_dnx_pph_fhei_bridge_extension_dump(
   bcm_pkt_dnx_pph_fhei_bridge_extension_t  *ext
   )
{

    LOG_CLI((BSL_META("pph_fhei_bridge_extension:\n\r")));
    LOG_CLI((BSL_META("ive_cmd: %u\n\r"),ext->ive_cmd));
    LOG_CLI((BSL_META("pcp1: %u\n\r"),ext->pcp1));
    LOG_CLI((BSL_META("dei1: %u\n\r"),ext->dei1));
    LOG_CLI((BSL_META("vlan1: %u\n\r"),ext->vlan1));
    LOG_CLI((BSL_META("pcp2: %u\n\r"),ext->pcp2));
    LOG_CLI((BSL_META("dei2: %u\n\r"),ext->dei2));
    LOG_CLI((BSL_META("vlan2: %u\n\r"),ext->vlan2));


}
void bcm_pkt_dnx_pph_fhei_trap_extension_dump(
   bcm_pkt_dnx_pph_fhei_trap_extension_t  *ext
   )
{
    LOG_CLI((BSL_META("pph_fhei_trap_extension:\n\r")));
    LOG_CLI((BSL_META("trap_qualifier: %u\n\r"),ext->trap_qualifier));
    LOG_CLI((BSL_META("trap_id: %u\n\r"),ext->trap_id));

}

void bcm_pkt_dnx_pph_fhei_ip_extension_dump(
   bcm_pkt_dnx_pph_fhei_ip_extension_t  *ext
   )
{

    LOG_CLI((BSL_META("pph_fhei_ip_extension:\n\r")));
    LOG_CLI((BSL_META("ive_cmd: %u\n\r"),ext->ive_cmd));
    LOG_CLI((BSL_META("pri: %u\n\r"),ext->pri));
    LOG_CLI((BSL_META("in_ttl: %u\n\r"),ext->in_ttl));


}

void bcm_pkt_dnx_pph_fhei_mpls_extension_dump(
   bcm_pkt_dnx_pph_fhei_mpls_extension_t  *ext
   )
{
    LOG_CLI((BSL_META("pph_fhei_mpls_extension:\n\r")));
    LOG_CLI((BSL_META("tpid_profile: %u\n\r"),ext->tpid_profile));
    LOG_CLI((BSL_META("label: %u\n\r"),ext->label));
    LOG_CLI((BSL_META("upper_layer_protocol: %u\n\r"),ext->upper_layer_protocol));
    LOG_CLI((BSL_META("is_pipe_model: %u\n\r"),ext->is_pipe_model));
    LOG_CLI((BSL_META("cw: %u\n\r"),ext->cw));
    LOG_CLI((BSL_META("label_cmd: %u\n\r"),ext->label_cmd));
    LOG_CLI((BSL_META("in_exp: %u\n\r"),ext->in_exp));
    LOG_CLI((BSL_META("in_ttl: %u\n\r"),ext->in_ttl));


}
void bcm_pkt_dnx_pph_fhei_trill_extension_dump(
   bcm_pkt_dnx_pph_fhei_trill_extension_t  *ext
   )
{

    LOG_CLI((BSL_META("pph_fhei_trill_extension:\n\r")));
    LOG_CLI((BSL_META("in_ttl: %u\n\r"),ext->in_ttl));


}
void bcm_pkt_dnx_pph_fhei_extension_dump(
   bcm_pkt_dnx_pph_fhei_extension_t  *ext
   )
{

    LOG_CLI((BSL_META("pph_fhei_extension:\n\r")));
    LOG_CLI((BSL_META("fhei_type: %s \n\r"), bcm_pkt_dnx_type_to_string(ext->fhei_type)));
    bcm_pkt_dnx_pph_fhei_bridge_extension_dump(&(ext->bridge));
    bcm_pkt_dnx_pph_fhei_trap_extension_dump(&(ext->trap));
    bcm_pkt_dnx_pph_fhei_ip_extension_dump(&(ext->ip));
    bcm_pkt_dnx_pph_fhei_mpls_extension_dump(&(ext->mpls));
    bcm_pkt_dnx_pph_fhei_trill_extension_dump(&(ext->trill));


}
void bcm_pkt_dnx_pph_dump(
   bcm_pkt_dnx_pph_t  *pph
   )
{

    LOG_CLI((BSL_META("fhei_size: %u\n\r"),pph->fhei_size));
    LOG_CLI((BSL_META("forwarding_type: %u\n\r"),pph->forwarding_type));
    LOG_CLI((BSL_META("forwarding_header_offset: %u\n\r"),pph->forwarding_header_offset));
    LOG_CLI((BSL_META("bypass_filter: %u\n\r"),pph->bypass_filter));
    LOG_CLI((BSL_META("snoop_cmnd: %u\n\r"),pph->snoop_cmnd));
    LOG_CLI((BSL_META("vport_orientation: %u\n\r"),pph->vport_orientation));
    LOG_CLI((BSL_META("unknown_address: %u\n\r"),pph->unknown_address));
    LOG_CLI((BSL_META("learn_allowed: %u\n\r"),pph->learn_allowed));
    LOG_CLI((BSL_META("vswitch: %u\n\r"),pph->vswitch));
    LOG_CLI((BSL_META("vrf: %u\n\r"),pph->vrf));
    LOG_CLI((BSL_META("in_vport: %u\n\r"),pph->in_vport));
    bcm_pkt_dnx_pph_fhei_extension_dump(&(pph->fhei));
    bcm_pkt_dnx_pph_eei_extension_dump(&(pph->eei));
    bcm_pkt_dnx_pph_learn_extension_dump(&(pph->learn));


}

 char*
 bcm_pkt_dnx_otsh_type_to_string(
      bcm_pkt_dnx_otsh_type_t otsh_type
  )
{
  char* str = NULL;
  switch(otsh_type)
  {
    case bcmPktDnxOtshTypeOam:
        str = "Oam";
        break;
    case bcmPktDnxOtshTypeL588v2:
        str = "L588v2";
        break;
    case bcmPktDnxOtshTypeLatency:
        str = "Latency";
        break;
    default:
        str = "Unknown";
  }
  return str;
}

 char*
 bcm_pkt_dnx_otsh_oam_subtype_to_string(
     bcm_pkt_dnx_otsh_oam_subtype_t otsh_subtype
  )
{
  char* str = NULL;
  switch(otsh_subtype)
  {
    case bcmPktDnxOtshOamSubtypeNone:
        str = "None";
        break;
    case bcmPktDnxOtshOamSubtypeLm:
        str = "Lm";
        break;
    case bcmPktDnxOtshOamSubtypeDm1588:
        str = "Dm1588";
        break;
    case bcmPktDnxOtshOamSubtypeDmNtp:
        str = "DmNtp";
        break;
    case bcmPktDnxOtshOamSubtypeOamDefault:
        str = "Default";
        break;
    case bcmPktDnxOtshOamSubtypeLoopback:
        str = "Loopback";
        break;
    case bcmPktDnxOtshOamSubtypeEcn:
        str = "Ecn";
        break;
    default:
        str = "Unknown";
  }
  return str;
}
void bcm_pkt_dnx_otsh_dump(
   bcm_pkt_dnx_otsh_t  *otsh
   )
{

    LOG_CLI((BSL_META("otsh_type: %s \n\r"), bcm_pkt_dnx_type_to_string(otsh->otsh_type)));
    LOG_CLI((BSL_META("oam_sub_type: %s \n\r"), bcm_pkt_dnx_otsh_oam_subtype_to_string(otsh->oam_sub_type)));

    LOG_CLI((BSL_META("oam_up_mep: %u\n\r"),otsh->oam_up_mep));
    LOG_CLI((BSL_META("tp_cmd: %u\n\r"),otsh->tp_cmd));
    LOG_CLI((BSL_META("ts_encap: %u\n\r"),otsh->ts_encap));
    /* LOG_CLI((BSL_META("oam_ts_data: %u\n\r"),otsh->oam_ts_data));*/ 
    LOG_CLI((BSL_META("latency_flow_ID: %u\n\r"),otsh->latency_flow_ID));
    LOG_CLI((BSL_META("offset: %u\n\r"),otsh->offset));

}

void bcm_pkt_dnx_otmh_src_sysport_extension_dump(
   bcm_pkt_dnx_otmh_src_sysport_extension_t  *ext
   )
{
    LOG_CLI((BSL_META("valid: %u\n\r"),ext->valid));
    LOG_CLI((BSL_META("src_sysport: %u\n\r"),ext->src_sysport));


}
void bcm_pkt_dnx_otmh_vport_extension_dump(
   bcm_pkt_dnx_otmh_vport_extension_t  *ext
   )
{

    LOG_CLI((BSL_META("valid: %u\n\r"),ext->valid));
    LOG_CLI((BSL_META("out_vport: %u\n\r"),ext->out_vport));


}
void bcm_pkt_dnx_otmh_dump(
   bcm_pkt_dnx_otmh_t  *otmh
   )
{
    LOG_CLI((BSL_META("action_type: %s \n\r"), bcm_pkt_dnx_ftmh_action_type_to_string(otmh->action_type)));

    LOG_CLI((BSL_META("ftmh_dp: %u\n\r"),otmh->ftmh_dp));
    LOG_CLI((BSL_META("is_mc_traffic: %u\n\r"),otmh->is_mc_traffic));
    LOG_CLI((BSL_META("prio: %u\n\r"),otmh->prio));
    LOG_CLI((BSL_META("dst_port: %u\n\r"),otmh->dst_port));

    bcm_pkt_dnx_otmh_src_sysport_extension_dump(&otmh->src_sysport_ext);
    bcm_pkt_dnx_otmh_vport_extension_dump(&otmh->out_vport_ext);


}


void bcm_pkt_dnx_raw_dump(
    bcm_pkt_dnx_raw_t *pkt
   )
{  
    int i;
    LOG_CLI((BSL_META("data len: %u\n\r"),pkt->len));

    for (i = 0; i < pkt->len; i++) {
        LOG_CLI((BSL_META("%u "),pkt->data[i]));

    }
    LOG_CLI((BSL_META("\n\r")));

}
void bcm_pkt_dnx_dump(
    bcm_pkt_t *pkt
   )
{
    int i;
    bcm_pkt_dnx_ptch1_t *ptch1;
    bcm_pkt_dnx_t       *dnx_pkt;

    LOG_CLI((BSL_META("dnx_header_count: %u\n\r"),pkt->dnx_header_count));

    for (i = 0; i < pkt->dnx_header_count; i++) {
        dnx_pkt = (bcm_pkt_dnx_t *)&(pkt->dnx_header_stack[i]);
 
        LOG_CLI((BSL_META("Header : %u \n\r"), i));

        LOG_CLI((BSL_META("type: %s \n\r"), bcm_pkt_dnx_type_to_string(pkt->dnx_header_stack[i].type)));

        switch (pkt->dnx_header_stack[i].type) {
        case bcmPktDnxTypePtch1:
                ptch1 = (bcm_pkt_dnx_ptch1_t *)&(dnx_pkt->ptch1);
                bcm_pkt_dnx_ptch1_dump(ptch1);
                break;
            case bcmPktDnxTypePtch2:
                bcm_pkt_dnx_ptch2_dump(&(dnx_pkt->ptch2));
                break;        
            case bcmPktDnxTypeItmh:
                bcm_pkt_dnx_itmh_dump((bcm_pkt_dnx_itmh_t *)&(pkt->dnx_header_stack[i].itmh));
                break;
            case bcmPktDnxTypeFtmh:
                bcm_pkt_dnx_ftmh_dump((bcm_pkt_dnx_ftmh_t *)&(pkt->dnx_header_stack[i].ftmh));
                break;
            case bcmPktDnxTypePph:
                bcm_pkt_dnx_pph_dump((bcm_pkt_dnx_pph_t *)&(pkt->dnx_header_stack[i].pph));
                break;
            case bcmPktDnxTypeOtsh:
                bcm_pkt_dnx_otsh_dump(&(pkt->dnx_header_stack[i].otsh));
                break;
            case bcmPktDnxTypeOtmh:
                bcm_pkt_dnx_otmh_dump(&(pkt->dnx_header_stack[i].otmh));
                break;
            default:
                bcm_pkt_dnx_raw_dump(&(pkt->dnx_header_stack[i].raw));
                break;
        }

    }

}






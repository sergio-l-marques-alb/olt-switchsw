/*
    Copyright 2001, Broadcom Corporation
    All Rights Reserved.
    
    This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
    the contents of this file may not be disclosed to third parties, copied or
    duplicated in any form, in whole or in part, without the prior written
    permission of Broadcom Corporation.
*/
/*
 * Common [OS-independent] portion of
 * Broadcom Home Networking Division 10/100 Mbit/s Ethernet
 * Device Driver.
 *
 * $Id: etc.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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

#include <shared/et/osl.h>
#include <shared/et/bcmendian.h>
#include <shared/et/bcmutils.h>
#include <shared/et/proto/ethernet.h>
#include <shared/et/bcmenetmib.h>
#include <shared/et/bcmenetrxh.h>
#include <shared/et/et_dbg.h>
#include <shared/et/et_export.h>
#include <soc/etc.h>

#if defined(VXWORKS) || defined(__ECOS)
extern int et_msg_level;
#else
int et_msg_level =
#ifdef BCMDBG
	1;
#else
	0;
#endif
#endif

/* local prototypes */
static void etc_soc_loopback(etc_soc_info_t *etc, int on);
static void etc_soc_dumpetc(etc_soc_info_t *etc, uchar *buf);

/* find the chip opsvec for this chip */
struct chops*
etc_soc_chipmatch(uint vendor, uint device)
{
#ifdef BCM4413_CHOPS
    {
    extern struct chops bcm4413_chops;
    if (bcm4413_chops.id(vendor, device))
        return (&bcm4413_chops);
    }
#endif
#ifdef BCM47XX_CHOPS
    {
    extern struct chops bcm47xx_et_soc_chops;
    if (bcm47xx_et_soc_chops.id(vendor, device))
        return (&bcm47xx_et_soc_chops);
    }
#endif
    return (NULL);
}

void*
etc_soc_attach(void *et, uint vendor, uint device, uint unit, 
    void *pdev, void *regsva)
{
    etc_soc_info_t *etc;

    ET_TRACE(("et%d: etc_soc_attach: vendor 0x%x device 0x%x\n", 
        unit, vendor, device));

    /* some code depends on packed structures */
    ASSERT(sizeof (struct ether_addr) == ETHER_ADDR_LEN);
    ASSERT(sizeof (struct ether_header) == ETHER_HDR_LEN);

    /* allocate etc_soc_info_t state structure */
    if ((etc = (etc_soc_info_t*) ET_MALLOC(sizeof (etc_soc_info_t))) == NULL) {
        ET_ERROR(("et%d: etc_soc_attach malloc failed\n", unit));
        return (NULL);
    }
    bzero((char*)etc, sizeof (etc_soc_info_t));


    /* perform any osl specific init */
    osl_init();

    etc->et = et;
    etc->unit = unit;
    etc->vendorid = (uint16) vendor;
    etc->deviceid = (uint16) device;
#ifdef _BCM5365_FPGA_
    etc->forcespeed = ET_10FULL;
    etc->linkstate = TRUE;
#else
    etc->forcespeed = ET_AUTO;
    etc->linkstate = FALSE;
#endif

    /* set chip opsvec */
    etc->chops = etc_soc_chipmatch(vendor, device);
    ASSERT(etc->chops);

    /* chip attach */
    if ((etc->ch = (*etc->chops->attach)(etc, pdev, regsva)) == NULL) {
        ET_ERROR(("et%d: chipattach error\n", unit));
        goto fail;
    }

    return ((void*)etc);

fail:
    etc_soc_detach(etc);
    return (NULL);
}

void
etc_soc_detach(etc_soc_info_t *etc)
{
    if (etc == NULL)
    	return;

    /* free chip private state */
    if (etc->ch) {
        (*etc->chops->detach)(etc->ch);
        etc->chops = etc->ch = NULL;
    }

    ET_MFREE(etc, sizeof (etc_soc_info_t));
}

void
etc_soc_reset(etc_soc_info_t *etc)
{
    ET_TRACE(("et%d: etc_soc_reset\n", etc->unit));

    etc->reset++;

    /* reset the chip */
    (*etc->chops->reset)(etc->ch);

    /* free any posted tx packets */
    (*etc->chops->txreclaim)(etc->ch, TRUE);

#ifdef DMA
    /* free any posted rx packets */
    (*etc->chops->rxreclaim)(etc->ch);
#endif
}

void
etc_soc_debug(etc_soc_info_t *etc)
{
    uchar buf[4096];   
    (*etc->chops->dump)(etc->ch, (char *) buf);
    soc_cm_print(" %s", buf);
}


void
etc_soc_init(etc_soc_info_t *etc, bool full)
{
    ET_TRACE(("et%d: etc_soc_init\n", etc->unit));

    ASSERT(etc->pioactive == NULL);
    ASSERT(!ETHER_ISNULLADDR(&etc->cur_etheraddr));
    ASSERT(!ETHER_ISMULTI(&etc->cur_etheraddr));

    /* init the chip */
    (*etc->chops->init)(etc->ch, full);
}

/* mark interface up */
void
etc_soc_up(etc_soc_info_t *etc)
{
    etc->up = TRUE;

    et_soc_init(etc->et, FALSE);
}

/* mark interface down */
uint
etc_soc_down(etc_soc_info_t *etc, int reset)
{
    uint callback;

    callback = 0;

    etc->up = FALSE;
    if (reset)
        et_soc_reset(etc->et);

    /* suppress link state changes during power management mode changes */
    if (etc->linkstate) { 
        etc->linkstate = FALSE;
        if (!etc->pm_modechange)
            et_soc_link_down(etc->et);
    }

    return (callback);
}

/* common ioctl handler.  return: 0=ok, -1=error */
int
etc_soc_ioctl(etc_soc_info_t *etc, int cmd, void *arg)
{
    int error;
    int val;
    int *vec = (int*)arg;

    error = 0;

    val = arg? *(int*)arg: 0;

    ET_TRACE(("et%d: etc_soc_ioctl: cmd 0x%x\n", etc->unit, cmd));

    switch (cmd) {
    case ETCUP:
        et_soc_up(etc->et);
        break;

    case ETCDOWN:
        et_soc_down(etc->et, TRUE);
        break;

    case ETCLOOP:
        etc_soc_loopback(etc, val);
        break;

    case ETCDUMP:
        if (et_msg_level & 0x10000) {
            if (arg) {
                bcmdumplog((uchar*)arg, 4096);
            }
        }
        break;

    case ETCSETMSGLEVEL:
        et_msg_level = val;
        break;

    case ETCPROMISC:
        etc_soc_promisc(etc, val);
        break;

    case ETCSPEED:
        if ((val != ET_AUTO) && (val != ET_10HALF) && (val != ET_10FULL)
            && (val != ET_100HALF) && (val != ET_100FULL))
            goto err;
        etc->forcespeed = val;

        /* explicitly reset the phy */
        (*etc->chops->phyreset)(etc->ch, etc->phyaddr);

        /* request restart autonegotiation if we're reverting to adv mode */
        if ((etc->forcespeed == ET_AUTO) & etc->advertise)
            etc->needautoneg = TRUE;

        et_soc_init(etc->et, FALSE);
        break;

    case ETCPHYRD:
        if (vec)
            vec[1] = (*etc->chops->phyrd)(etc->ch, etc->phyaddr, vec[0]);
        break;

    case ETCPHYWR:
        if (vec)
            (*etc->chops->phywr)(etc->ch, etc->phyaddr, vec[0], 
            (uint16) vec[1]);
        break;
    default:
    err:
        error = -1;
    }

    return (error);
}

/* called once per second */
void
etc_soc_watchdog(etc_soc_info_t *etc)
{
    uint16 status;
    uint16 adv;
    uint16 lpa;
    
    etc->now++;

    if (etc->phyaddr == PHY_NOMDC) {	    
        etc->linkstate = TRUE;
        etc->speed = 100;
        etc->duplex = 1;
        return;
    }



    status = (*etc->chops->phyrd)(etc->ch, etc->phyaddr, 1);
    adv = (*etc->chops->phyrd)(etc->ch, etc->phyaddr, 4);
    lpa = (*etc->chops->phyrd)(etc->ch, etc->phyaddr, 5);

    /* check for bad mdio read */
    if (status == 0xffff) {
        ET_ERROR(("et%d: etc_soc_watchdog: bad mdio read: phyaddr %d mdcport %d\n",
            etc->unit, etc->phyaddr, etc->mdcport));
        return;
    }

    /* monitor link state */
    if (!etc->linkstate && (status & STAT_LINK)) {
        etc->linkstate = TRUE;

        if (etc->pm_modechange)
            etc->pm_modechange = FALSE;
        else
            et_soc_link_up(etc->et);
    }
    else if (etc->linkstate && !(status & STAT_LINK)) {
        etc->linkstate = FALSE;
        if (!etc->pm_modechange)
            et_soc_link_down(etc->et);
    }

    /* update current speed and duplex */
    if ( (adv & ADV_100FULL) && (lpa & LPA_100FULL) ) {
        etc->speed = 100;
        etc->duplex = 1;
    } else if ( (adv & ADV_100HALF) && (lpa & LPA_100HALF) ) {
        etc->speed = 100;
        etc->duplex = 0;
    } else if ( (adv & ADV_10FULL) && (lpa & LPA_10FULL) ) {
        etc->speed = 10;
        etc->duplex = 1;
    } else {
        etc->speed = 10;
        etc->duplex = 0;
    }

    /* keep emac txcontrol duplex bit consistent with current phy duplex */
    (*etc->chops->duplexupd)(etc->ch);

    /* check for remote fault error */
    if (status & STAT_REMFAULT) {
        ET_ERROR(("et%d: remote fault\n", etc->unit));
    }

    /* check for jabber error */
    if (status & STAT_JAB) {
        ET_ERROR(("et%d: jabber\n", etc->unit));
    }

    /*
     * Read chip mib counters occationally before the 16bit ones can wrap.
     * We don't use the high-rate mib counters.
     */
    if ((etc->now % 30) == 0)
        (*etc->chops->statsupd)(etc->ch);
}

static void
etc_soc_loopback(etc_soc_info_t *etc, int on)
{
    ET_TRACE(("et%d: etc_soc_loopback: %d\n", etc->unit, on));

    etc->loopbk = (bool) on;
    et_soc_init(etc->et, FALSE);
}

void
etc_soc_promisc(etc_soc_info_t *etc, uint on)
{
    ET_TRACE(("et%d: etc_soc_promisc: %d\n", etc->unit, on));

    etc->promisc = (bool) on;
    et_soc_init(etc->et, FALSE);
}

void
etc_soc_dump(etc_soc_info_t *etc, uchar *buf, int size)
{
    /* big enough */
    if (size < 3700)
        return;

    etc_soc_dumpetc(etc, buf);
    (*etc->chops->dump)(etc->ch, (char *)buf + strlen((char *) buf));
}

static void
etc_soc_dumpetc(etc_soc_info_t *etc, uchar *buf)
{
    char perm[32], cur[32];
    uint i;

    buf += sprintf((char *) buf, 
        "etc 0x%x et 0x%x unit %d msglevel %d speed/duplex %d%s\n",
        (uint)etc, (uint)etc->et, etc->unit, et_msg_level,
        etc->speed, (etc->duplex? "full": "half"));
    buf += sprintf((char *) buf, 
    "up %d promisc %d loopbk %d forcespeed %d advertise 0x%x needautoneg %d\n",
        etc->up, etc->promisc, etc->loopbk, 
        etc->forcespeed, etc->advertise, etc->needautoneg);
    buf += sprintf((char *) buf,
                   "piomode %d pioactive 0x%x nmulticast %d allmulti %d\n",
        etc->piomode, (uint)etc->pioactive, etc->nmulticast, etc->allmulti);
    buf += sprintf((char *) buf, 
        "vendor 0x%x device 0x%x rev %d coreunit %d phyaddr %d mdcport %d\n",
        etc->vendorid, etc->deviceid, etc->chiprev,
        etc->coreunit, etc->phyaddr, etc->mdcport);

    buf += sprintf((char *) buf, "perm_etheraddr %s cur_etheraddr %s\n",
        bcm_ether_ntoa((char*)&etc->perm_etheraddr, perm),
        bcm_ether_ntoa((char*)&etc->cur_etheraddr, cur));

    if (etc->nmulticast) {
        buf += sprintf((char *) buf, "multicast: ");
        for (i = 0; i < etc->nmulticast; i++)
            buf += sprintf((char *) buf, "%s ", 
            bcm_ether_ntoa((char*)&etc->multicast[i], cur));
        buf += sprintf((char *) buf, "\n");
    }

    buf += sprintf((char *) buf, "linkstate %d\n", etc->linkstate);
    buf += sprintf((char *) buf, "\n");

    /* refresh stat counters */
    (*etc->chops->statsupd)(etc->ch);

    /* summary stat counter line */
    /* use sw frame and byte counters -- */
    /* hw mib counters wrap too quickly to be useful */
    buf += sprintf((char *) buf, 
    "txframe %d txbyte %d txerror %d rxframe %d rxbyte %d rxerror %d\n",
        etc->txframe, etc->txbyte, etc->txerror,
        etc->rxframe, etc->rxbyte, etc->rxerror);

    /* transmit stat counters */
    /* hardware mib pkt and octet counters wrap too quickly to be useful */
    buf += sprintf((char *) buf, 
    "tx_broadcast_pkts %d tx_multicast_pkts %d \
    tx_jabber_pkts %d tx_oversize_pkts %d\n",
        etc->mib.tx_broadcast_pkts, etc->mib.tx_multicast_pkts, 
        etc->mib.tx_jabber_pkts, etc->mib.tx_oversize_pkts);
    buf += sprintf((char *) buf, "tx_fragment_pkts %d tx_underruns %d\n",
        etc->mib.tx_fragment_pkts, etc->mib.tx_underruns);
    buf += sprintf((char *) buf, 
        "tx_total_cols %d tx_single_cols %d tx_multiple_cols %d \
        tx_excessive_cols %d\n",
        etc->mib.tx_total_cols, etc->mib.tx_single_cols, 
        etc->mib.tx_multiple_cols, etc->mib.tx_excessive_cols);
    buf += sprintf((char *) buf, 
        "tx_late_cols %d tx_defered %d tx_carrier_lost %d tx_pause_pkts %d\n",
        etc->mib.tx_late_cols, etc->mib.tx_defered, 
        etc->mib.tx_carrier_lost, etc->mib.tx_pause_pkts);
    buf += sprintf((char *) buf,
                   "txnobuf %d reset %d dmade %d dmada %d dmape %d\n",
        etc->txnobuf, etc->reset, etc->dmade, etc->dmada, etc->dmape);

    /* receive stat counters */
    /* hardware mib pkt and octet counters wrap too quickly to be useful */
    buf += sprintf((char *) buf, "rx_broadcast_pkts %d rx_multicast_pkts %d \
    rx_jabber_pkts %d rx_oversize_pkts %d\n",
        etc->mib.rx_broadcast_pkts, etc->mib.rx_multicast_pkts, 
        etc->mib.rx_jabber_pkts, etc->mib.rx_oversize_pkts);
    buf += sprintf((char *) buf, "rx_fragment_pkts %d rx_missed_pkts %d \
        rx_crc_align_errs %d rx_undersize %d\n",
        etc->mib.rx_fragment_pkts, etc->mib.rx_missed_pkts, 
        etc->mib.rx_crc_align_errs, etc->mib.rx_undersize);
    buf += sprintf((char *) buf, 
        "rx_crc_errs %d rx_align_errs %d rx_symbol_errs %d rx_pause_pkts %d\n",
        etc->mib.rx_crc_errs, etc->mib.rx_align_errs, 
        etc->mib.rx_symbol_errs, etc->mib.rx_pause_pkts);
    buf += sprintf((char *) buf, 
        "rx_nonpause_pkts %d rxnobuf %d rxdmauflo %d rxoflo %d rxbadlen %d\n",
        etc->mib.rx_nonpause_pkts, etc->rxnobuf, 
        etc->rxdmauflo, etc->rxoflo, etc->rxbadlen);
    buf += sprintf((char *) buf, "\n");
}

uint
etc_soc_control_totlen(etc_soc_info_t *etc, void *p)
{
    uint total;

    total = 0;
    for (; p; p = ET_PKTNEXT(etc->unit, p))
        total += ET_PKTLEN(etc->unit, p);
    return (total);
}

#ifdef BCMDBG
void
etc_soc_prhdr(char *msg, struct ether_header *eh, uint len)
{
    char da[32], sa[32];

    soc_cm_print("%s: dst %s src %s type 0x%x len %d\n",
        msg,
        bcm_ether_ntoa(eh->ether_dhost, da),
        bcm_ether_ntoa(eh->ether_shost, sa),
        ntoh16(eh->ether_type),
        len);
}
#endif /* BCMDBG */

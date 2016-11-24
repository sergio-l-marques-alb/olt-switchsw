/*
 * $Id: hnddma.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
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
 *
 * Generic Broadcom Home Networking Division (HND) DMA module.
 * This supports the following chips: BCM42xx, 44xx, 47xx .
 *
 * Note: This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Broadcom Corporation.
 */

#include <shared/et/typedefs.h>
#include <shared/et/osl.h>
#include <shared/et/bcmendian.h>
#include <shared/et/bcmutils.h>

struct dma_info;	/* forward declaration */
#define di_t struct dma_info
#include <soc/hnddma.h>

/* debug/trace */
#ifdef BCMDBG
#define	DMA_ERROR(args) if (!(*di->msg_level & 1)) ; else soc_cm_print args
#define	DMA_TRACE(args) if (!(*di->msg_level & 2)) ; else soc_cm_print args
#else
#define	DMA_ERROR(args)
#define	DMA_TRACE(args)
#endif

/* default dma message level(if input msg_level pointer */
/* is null in dma_attach()) */
static uint dma_soc_msg_level = 0;


#define	MAXNAMEL	8
#define	MAXDD		(DMAMAXRINGSZ / sizeof (dmadd_soc_t))

/* dma engine software state */
typedef struct dma_soc_info {
    hnddma_soc_t	hnddma;		/* exported structure */
    uint		*msg_level;	/* message level pointer */

    char		name[MAXNAMEL];	/* callers name for diag msgs */
    void		*drv;		/* driver handle */
    void		*dev;		/* device handle */
    dmaregs_soc_t	*regs;		/* dma engine registers */

    dmadd_soc_t		*txd; /* pointer to chip-specific tx descriptor ring */
    uint		txin;		/* index of next descriptor to reclaim */
    uint		txout;		/* index of next descriptor to post */
    uint		txavail;	/* # free tx descriptors */
    void		*txp[MAXDD];	/* parallel array of pointers to packets */
    void		*txdpa;		/* physical address of descriptor ring */
    uint		txdalign; /* #bytes added to alloc'd mem to align txd */

    dmadd_soc_t		*rxd; /* pointer to chip-specific rx descriptor ring */
    uint		rxin;		/* index of next descriptor to reclaim */
    uint		rxout;		/* index of next descriptor to post */
    void		*rxp[MAXDD]; /* parallel array of pointers to packets */
    void		*rxdpa;		/* physical address of descriptor ring */
    uint		rxdalign; /* #bytes added to alloc'd mem to align rxd */

    /* tunables */
    uint		ntxd;		/* # tx descriptors */
    uint		nrxd;		/* # rx descriptors */
    uint		rxbufsize;	/* rx buffer size in bytes */
    uint		nrxpost;	/* # rx buffers to keep posted */
    uint		rxoffset;	/* rxcontrol offset */
    uint		ddoffset; /* add to get dma address of descriptor ring */
    uint		dataoffset; /* add to get dma address of data buffer */
} dma_soc_info_t;

/* descriptor bumping macros */
#define	NEXTTXD(i)	((i + 1) & (di->ntxd - 1))
#define	PREVTXD(i)	((i - 1) & (di->ntxd - 1))
#define	NEXTRXD(i)	((i + 1) & (di->nrxd - 1))
#define	NTXDACTIVE(h, t)	((t - h) & (di->ntxd - 1))
#define	NRXDACTIVE(h, t)	((t - h) & (di->nrxd - 1))

/* macros to convert between byte offsets and indexes */
#define	B2I(bytes)	((bytes) / sizeof (dmadd_soc_t))
#define	I2B(index)	((index) * sizeof (dmadd_soc_t))

void*
dma_soc_attach(void *drv, void *dev, char *name, dmaregs_soc_t *regs, 
    uint ntxd, uint nrxd, uint rxbufsize, uint nrxpost, uint rxoffset, 
    uint ddoffset, uint dataoffset, uint *msg_level)
{
    dma_soc_info_t *di;
    void *va;

    ASSERT(ntxd <= MAXDD);
    ASSERT(nrxd <= MAXDD);

    /* allocate private info structure */
    if ((di = ET_MALLOC(sizeof (dma_soc_info_t))) == NULL)
        return (NULL);
    bzero((char*)di, sizeof (dma_soc_info_t));

    /* set message level */
    di->msg_level = msg_level ? msg_level : &dma_soc_msg_level;

    DMA_TRACE(("%s: dma_soc_attach: drv 0x%x dev 0x%x regs 0x%x ntxd %d nrxd %d \
        rxbufsize %d nrxpost %d rxoffset %d ddoffset 0x%x dataoffset 0x%x\n", 
        name, (uint)drv, (uint)dev, (uint)regs, ntxd, nrxd, rxbufsize, nrxpost, 
        rxoffset, ddoffset, dataoffset));

    /* make a private copy of our callers name */
    strncpy(di->name, name, MAXNAMEL);
    di->name[MAXNAMEL-1] = '\0';

    di->drv = drv;
    di->dev = dev;
    di->regs = regs;

    /* allocate transmit descriptor ring */
    if (ntxd) {
        if ((va = DMA_ALLOC_CONSISTENT(dev, 
            (DMAMAXRINGSZ + DMARINGALIGN), &di->txdpa)) == NULL)
            goto fail;
        di->txdpa = (void *)DMA_MAP(dev, va);
        di->txd = (dmadd_soc_t*) ROUNDUP(va, DMARINGALIGN);
        di->txdalign = ((uint)di->txd - (uint)va);
        di->txdpa = (void*) ((uint)di->txdpa + di->txdalign);
        ASSERT(ISALIGNED(di->txd, DMARINGALIGN));
    }

    /* allocate receive descriptor ring */
    if (nrxd) {
        if ((va = DMA_ALLOC_CONSISTENT(dev, 
            (DMAMAXRINGSZ + DMARINGALIGN), &di->rxdpa)) == NULL)
            goto fail;
        di->rxdpa = (void *)DMA_MAP(dev, va);
        di->rxd = (dmadd_soc_t*) ROUNDUP(va, DMARINGALIGN);
        di->rxdalign = ((uint)di->rxd - (uint)va);
        di->rxdpa = (void*) ((uint)di->rxdpa + di->rxdalign);
        ASSERT(ISALIGNED(di->rxd, DMARINGALIGN));
    }

    /* save tunables */
    di->ntxd = ntxd;
    di->nrxd = nrxd;
    di->rxbufsize = rxbufsize;
    di->nrxpost = nrxpost;
    di->rxoffset = rxoffset;
    di->ddoffset = ddoffset;
    di->dataoffset = dataoffset;

    return ((void*)di);

fail:
    dma_soc_detach((void*)di);
    return (NULL);
}

/* may be called with core in reset */
void
dma_soc_detach(void *di_soc)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    if (di == NULL)
        return;

    DMA_TRACE(("%s: dma_soc_detach\n", di->name));

    /* shouldn't be here if descriptors are unreclaimed */
    ASSERT(di->txin == di->txout);
    ASSERT(di->rxin == di->rxout);

    /* free dma descriptor rings */
    if (di->txd)
        DMA_FREE_CONSISTENT(di->dev, (void *)(di->txd - di->txdalign), 
            (DMAMAXRINGSZ + DMARINGALIGN), di->txdpa);
    if (di->rxd)
        DMA_FREE_CONSISTENT(di->dev, (void *)(di->rxd - di->rxdalign), 
            (DMAMAXRINGSZ + DMARINGALIGN), di->rxdpa);

    /* free our private info structure */
    ET_MFREE((void*)di, sizeof (dma_soc_info_t));
}

/* 
 * PR2414 WAR: When the DMA channel is in the FetchDescriptor state,
 * it does not notice that the enable bit has been turned off. If the
 * enable bit is turned back on before the descriptor fetch completes,
 * at least some of the DMA channel does not get reset. In particular,
 * it will fetch a descriptor from the address it was trying to fetch
 * from when it was disabled. 
 *
 * For all cores other than USB, the workaround is simply to clear the
 * enable bit, and then read back status until the state shows up as
 * Disabled before re-enabling the channel.
 */

void
dma_soc_txreset(void *di_soc)
{
    uint32 status;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    DMA_TRACE(("%s: dma_soc_txreset\n", di->name));

    /* address PR8249/PR7577 issue */
    /* suspend tx DMA first */
    W_REG(di->dev, &di->regs->xmtcontrol, XC_SE);
    SPINWAIT((status = (R_REG(di->dev, &di->regs->xmtstatus) 
        & XS_XS_MASK)) != XS_XS_DISABLED &&
        status != XS_XS_IDLE &&
        status != XS_XS_STOPPED,
        10000);

    /* PR2414 WAR: DMA engines are not disabled until transfer finishes */
    W_REG(di->dev, &di->regs->xmtcontrol, 0);
    SPINWAIT((status = (R_REG(di->dev, &di->regs->xmtstatus) 
        & XS_XS_MASK)) != XS_XS_DISABLED,
        10000);

    if (status != XS_XS_DISABLED) {
        DMA_ERROR(("%s: dma_soc_txreset: dma cannot be stopped\n", di->name));
    }

    /* wait for the last transaction to complete */
    OSL_DELAY(300);
}

void
dma_soc_rxreset(void *di_soc)
{
    uint32 status;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    DMA_TRACE(("%s: dma_soc_rxreset\n", di->name));

    /* PR2414 WAR: DMA engines are not disabled until transfer finishes */
    W_REG(di->dev, &di->regs->rcvcontrol, 0);
    SPINWAIT((status = (R_REG(di->dev, &di->regs->rcvstatus) 
        & RS_RS_MASK)) != RS_RS_DISABLED,
        10000);

    if (status != RS_RS_DISABLED) {
        DMA_ERROR(("%s: dma_soc_rxreset: dma cannot be stopped\n", di->name));
    }
}

void
dma_soc_txinit(void *di_soc)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    DMA_TRACE(("%s: dma_soc_txinit\n", di->name));

    di->txin = di->txout = 0;
    di->txavail = di->ntxd - 1;

    /* clear tx descriptor ring */
    BZERO_SM((void*)di->txd, (di->ntxd * sizeof (dmadd_soc_t)));

    W_REG(di->dev, &di->regs->xmtcontrol, XC_XE);
    W_REG(di->dev, &di->regs->xmtaddr, ((uint32)di->txdpa + di->ddoffset));
}

bool
dma_soc_txenabled(void *di_soc)
{
    uint32 xc;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    /* If the chip is dead, it is not enabled :-) */
    xc = R_REG(di->dev, &di->regs->xmtcontrol);
    return ((xc != 0xffffffff) && (xc & XC_XE));
}

void
dma_soc_txsuspend(void *di_soc)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    DMA_TRACE(("%s: dma_soc_txsuspend\n", di->name));
    OR_REG(di->dev, &di->regs->xmtcontrol, XC_SE);
}

void
dma_soc_txresume(void *di_soc)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    DMA_TRACE(("%s: dma_soc_txresume\n", di->name));
    AND_REG(di->dev, &di->regs->xmtcontrol, ~XC_SE);
}

bool
dma_soc_txsuspended(void *di_soc)
{
    uint32 xc;
    uint32 xs;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    xc = R_REG(di->dev, &di->regs->xmtcontrol);
    if (xc & XC_SE) {
        xs = R_REG(di->dev, &di->regs->xmtstatus);
        return ((xs & XS_XS_MASK) == XS_XS_IDLE);
    }
    return 0;
}

bool
dma_soc_txstopped(void *di_soc)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    return ((R_REG(di->dev, &di->regs->xmtstatus) & XS_XS_MASK) 
        == XS_XS_STOPPED);
}

bool
dma_soc_rxstopped(void *di_soc)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    return ((R_REG(di->dev, &di->regs->rcvstatus) & RS_RS_MASK) 
        == RS_RS_STOPPED);
}

void
dma_soc_fifoloopbackenable(void *di_soc)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    DMA_TRACE(("%s: dma_soc_fifoloopbackenable\n", di->name));
    OR_REG(di->dev, &di->regs->xmtcontrol, XC_LE);
}

void
dma_soc_rxinit(void *di_soc)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    DMA_TRACE(("%s: dma_soc_rxinit\n", di->name));

    di->rxin = di->rxout = 0;

    /* clear rx descriptor ring */
    BZERO_SM((void*)di->rxd, (di->nrxd * sizeof (dmadd_soc_t)));

    dma_soc_rxenable(di);
    W_REG(di->dev, &di->regs->rcvaddr, ((uint32)di->rxdpa + di->ddoffset));
}

void
dma_soc_rxenable(void *di_soc)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    DMA_TRACE(("%s: dma_soc_rxenable\n", di->name));
    W_REG(di->dev, &di->regs->rcvcontrol, 
        ((di->rxoffset << RC_RO_SHIFT) | RC_RE));
}

bool
dma_soc_rxenabled(void *di_soc)
{
    uint32 rc;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    rc = R_REG(di->dev, &di->regs->rcvcontrol);
    return ((rc != 0xffffffff) && (rc & RC_RE));
}

/*
 * The BCM47XX family supports full 32bit dma engine buffer addressing so
 * dma buffers can cross 4 Kbyte page boundaries.
 */
int
dma_soc_txfast(void *di_soc, void *p0, uint32 coreflags)
{
    void *p, *next;
    uchar *data;
    uint len;
    uint txout;
    uint32 ctrl;
    uint32 pa;
    int chain;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    DMA_TRACE(("%s: dma_soc_txfast\n", di->name));

    txout = di->txout;
    ctrl = 0;

    /*
     * Walk the chain of packet buffers
     * allocating and initializing transmit descriptor entries.
     */
    for (p = p0, chain = 0; p; p = next, chain++) {
        data = ET_PKTDATA(di->dev, p);
        len = ET_PKTLEN(di->dev, p);
        next = ET_PKTNEXT(di->dev, p);

        /* return nonzero if out of tx descriptors */
        if (NEXTTXD(txout) == di->txin)
            goto outoftxd;

        /* PR988 - skip zero length buffers */
        if (len == 0)
            continue;

        /* Flush the cached data to physical memory for DMA to process */
        soc_cm_sflush((int)di->dev, data, len);

        /* get physical address of buffer start */
        pa = (uint32) DMA_MAP(di->dev, data);
        DMA_TRACE(("%s: dma_soc_txfast len=%d pa=%x\n", di->name, len, pa));

        /* build the descriptor control value */
        ctrl = len & CTRL_BC_MASK;

        /* PR3697: Descriptor flags are not ignored for */
        /* descriptors where SOF is clear */
        ctrl |= coreflags;

        if ((p == p0) && !chain)
        	ctrl |= CTRL_SOF;
        if (next == NULL)
        	ctrl |= (CTRL_IOC | CTRL_EOF);
        if (txout == (di->ntxd - 1))
        	ctrl |= CTRL_EOT;

        DMA_TRACE(("%s: dma_soc_txfast ctrl=%x\n", di->name, ctrl));
        /* init the tx descriptor */
        W_SM(&di->txd[txout].ctrl, BUS_SWAP32(ctrl));
        W_SM(&di->txd[txout].addr, BUS_SWAP32(pa + di->dataoffset));

        ASSERT(di->txp[txout] == NULL);

        txout = NEXTTXD(txout);
    }

    /* if last txd eof not set, fix it */
    if (!(ctrl & CTRL_EOF))
        W_SM(&di->txd[PREVTXD(txout)].ctrl, 
        BUS_SWAP32(ctrl | CTRL_IOC | CTRL_EOF));

    /* save the packet */
    di->txp[di->txout] = p0;

    /* bump the tx descriptor index */
    di->txout = txout;

    /* kick the chip */
    W_REG(di->dev, &di->regs->xmtptr, I2B(txout));

    /* tx flow control */
    di->txavail = di->ntxd - NTXDACTIVE(di->txin, di->txout) - 1;

    return (0);

outoftxd:
    DMA_ERROR(("%s: dma_soc_txfast: out of txds\n", di->name));
    ET_PKTFREE(di->dev, p0, TRUE);
    di->txavail = 0;
    di->hnddma.txnobuf++;
    return (-1);
}

#define	PAGESZ		4096
#define	PAGEBASE(x)	((uint)(x) & ~4095)

/*
 * Just like above except go through the extra effort of splitting
 * buffers that cross 4Kbyte boundaries into multiple tx descriptors.
 */
int
dma_soc_tx(void *di_soc, void *p0, uint32 coreflags)
{
    void *p, *next;
    uchar *data;
    uint plen, len;
    uchar *page, *start, *end;
    uint txout;
    uint32 ctrl;
    uint32 pa;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    DMA_TRACE(("%s: dma_soc_tx\n", di->name));

    txout = di->txout;
    ctrl = 0;

    /*
     * Walk the chain of packet buffers
     * splitting those that cross 4 Kbyte boundaries
     * allocating and initializing transmit descriptor entries.
     */
    for (p = p0; p; p = next) {
        data = ET_PKTDATA(di->dev, p);
        plen = ET_PKTLEN(di->dev, p);
        next = ET_PKTNEXT(di->dev, p);

        /* PR988 - skip zero length buffers */
        if (plen == 0)
            continue;

        for (page = (uchar*)PAGEBASE(data);
            page <= (uchar*)PAGEBASE(data + plen - 1);
            page += PAGESZ) {
                
            /* return nonzero if out of tx descriptors */
            if (NEXTTXD(txout) == di->txin)
                goto outoftxd;
            
            start = (page == (uchar*)PAGEBASE(data))?  data: page;
            end = (page == (uchar*)PAGEBASE(data + plen))?
                (data + plen): (page + PAGESZ);
            len = end - start;

            /* build the descriptor control value */
            ctrl = len & CTRL_BC_MASK;

            /* PR3697: Descriptor flags are not ignored for */
            /* escriptors where SOF is clear */
            ctrl |= coreflags;

            if ((p == p0) && (start == data))
                ctrl |= CTRL_SOF;
            if ((next == NULL) && (end == (data + plen)))
                ctrl |= (CTRL_IOC | CTRL_EOF);
            if (txout == (di->ntxd - 1))
                ctrl |= CTRL_EOT;

            /* get physical address of buffer start */
            pa = (uint32) DMA_MAP(di->dev, start);

            /* init the tx descriptor */
            W_SM(&di->txd[txout].ctrl, BUS_SWAP32(ctrl));
            W_SM(&di->txd[txout].addr, BUS_SWAP32(pa + di->dataoffset));

            ASSERT(di->txp[txout] == NULL);

            txout = NEXTTXD(txout);
        }
    }

    /* if last txd eof not set, fix it */
    if (!(ctrl & CTRL_EOF))
        W_SM(&di->txd[PREVTXD(txout)].ctrl, 
        BUS_SWAP32(ctrl | CTRL_IOC | CTRL_EOF));

    /* save the packet */
    di->txp[di->txout] = p0;

    /* bump the tx descriptor index */
    di->txout = txout;

    /* kick the chip */
    W_REG(di->dev, &di->regs->xmtptr, I2B(txout));

    /* tx flow control */
    di->txavail = di->ntxd - NTXDACTIVE(di->txin, di->txout) - 1;

    return (0);

outoftxd:
    DMA_ERROR(("%s: dma_soc_tx: out of txds\n", di->name));
    ET_PKTFREE(di->dev, p0, TRUE);
    di->txavail = 0;
    di->hnddma.txnobuf++;
    return (-1);
}

/* returns a pointer to the next frame received, or NULL if there are no more */
void*
dma_soc_rx(void *di_soc)
{
    void *p;
    uint len;
    int skiplen = 0;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    while ((p = dma_soc_getnextrxp(di, FALSE))) {
        /* skip giant packets which span multiple rx descriptors */
        if (skiplen > 0) {
            skiplen -= di->rxbufsize;
            if (skiplen < 0)
                skiplen = 0;
            ET_PKTFREE(di->dev, p, FALSE);
            continue;
        }
        
        /* Invalidate the cache so that the fresh hot data can be accessed */
        soc_cm_sinval((int)di->dev, ET_PKTDATA(di->dev, p), di->rxbufsize);
        
        len = ltoh16(*(uint16*)(ET_PKTDATA(di->dev, p)));
        DMA_TRACE(("%s: dma_soc_rx len %d\n", di->name, len));
        /* bad frame length check */
        if (len > (di->rxbufsize - di->rxoffset)) {
            DMA_ERROR(("%s: dma_soc_rx: bad frame length (%d)\n", di->name, len));
            if (len > 0)
                skiplen = len - (di->rxbufsize - di->rxoffset);
            ET_PKTFREE(di->dev, p, FALSE);
            di->hnddma.rxgiants++;
            continue;
        }

        /* set actual length */
        ET_PKTSETLEN(di->dev, p, (di->rxoffset + len));

        break;
    }

    return (p);
}

/* post receive buffers */
void
dma_soc_rxfill(void *di_soc)
{
    void *p;
    uint rxin, rxout;
    uint ctrl;
    uint n;
    uint i;
    uint32 pa;
    uint rxbufsize;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    /*
    * Determine how many receive buffers we're lacking
    * from the full complement, allocate, initialize,
    * and post them, then update the chip rx lastdscr.
    */

    rxin = di->rxin;
    rxout = di->rxout;
    rxbufsize = di->rxbufsize;

    n = di->nrxpost - NRXDACTIVE(rxin, rxout);

    DMA_TRACE(("%s: dma_soc_rxfill: post %d\n", di->name, n));

    for (i = 0; i < n; i++) {
        if ((p = ET_PKTGET(di->dev, rxbufsize, FALSE)) == NULL) {
            DMA_ERROR(("%s: dma_soc_rxfill: out of rxbufs\n", di->name));
            di->hnddma.rxnobuf++;
            break;
        }

        /* PR3263 & PR3387 & PR4642 war: */
        /* rxh.len=0 means dma writes not complete */
        *(uint32*)(OSL_UNCACHED(ET_PKTDATA(di->dev, p))) = 0;

        pa = (uint32) DMA_MAP(di->dev, ET_PKTDATA(di->dev, p));
        ASSERT(ISALIGNED(pa, 4));

        /* save the free packet pointer */
        ASSERT(di->rxp[rxout] == NULL);
        di->rxp[rxout] = p;

        /* paranoia */
        ASSERT(R_SM(&di->rxd[rxout].addr) == 0);

        /* prep the descriptor control value */
        ctrl = rxbufsize;
        if (rxout == (di->nrxd - 1))
            ctrl |= CTRL_EOT;

        /* init the rx descriptor */
        W_SM(&di->rxd[rxout].ctrl, BUS_SWAP32(ctrl));
        W_SM(&di->rxd[rxout].addr, BUS_SWAP32(pa + di->dataoffset));

        rxout = NEXTRXD(rxout);
    }

    DMA_TRACE(("%s: dma_soc_rxfill %d\n", di->name, i));
    di->rxout = rxout;

    /* update the chip lastdscr pointer */
    W_REG(di->dev, &di->regs->rcvptr, I2B(rxout));
    DMA_TRACE(("%s: dma_soc_rxfill return\n", di->name));
}

void
dma_soc_txreclaim(void *di_soc, bool forceall)
{
    void *p;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    DMA_TRACE(("%s: dma_soc_txreclaim %s\n", di->name, forceall ? "all" : ""));

    while ((p = dma_soc_getnexttxp(di, forceall))) {
        ET_PKTFREE(di->dev, p, TRUE);
    }
}

/*
 * Reclaim next completed txd (txds if using chained buffers) and
 * return associated packet.
 * If 'force' is true, reclaim txd(s) and return associated packet
 * regardless of the value of the hardware "curr" pointer.
 */
void*
dma_soc_getnexttxp(void *di_soc, bool forceall)
{
    uint start, end, i;
    void *txp;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    DMA_TRACE(("%s: dma_soc_getnexttxp %s\n", di->name, forceall ? "all" : ""));

    txp = NULL;

    /* if forcing, dma engine must be disabled */
    ASSERT(!forceall || !dma_soc_txenabled(di));

    start = di->txin;
    if (forceall)
        end = di->txout;
    else
        end = B2I(R_REG(di->dev, &di->regs->xmtstatus) & XS_CD_MASK);

    /* PR4738 - xmt disable/re-enable does not clear CURR */
    if ((start == 0) && (end > di->txout))
        goto bogus;

    for (i = start; i != end && !txp; i = NEXTTXD(i)) {
        DMA_UNMAP(di->dev, 
            (BUS_SWAP32(R_SM(&di->txd[i].addr)) - di->dataoffset));
        W_SM(&di->txd[i].addr, 0);
        txp = di->txp[i];
        di->txp[i] = NULL;
    }

    di->txin = i;

    /* tx flow control */
    di->txavail = di->ntxd - NTXDACTIVE(di->txin, di->txout) - 1;

    return (txp);

bogus:
    return (NULL);
}

void
dma_soc_rxreclaim(void *di_soc)
{
    void *p;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    DMA_TRACE(("%s: dma_soc_rxreclaim\n", di->name));

    while ((p = dma_soc_getnextrxp(di, TRUE)))
        ET_PKTFREE(di->dev, p, FALSE);
}

void *
dma_soc_getnextrxp(void *di_soc, bool forceall)
{
    uint i;
    void *rxp;
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;

    /* if forcing, dma engine must be disabled */
    ASSERT(!forceall || !dma_soc_rxenabled(di));

    i = di->rxin;

    /* return if no packets posted */
    if (i == di->rxout)
        return (NULL);

    /* ignore curr if forceall */
    if (!forceall && (i == B2I(R_REG(di->dev, &di->regs->rcvstatus) 
        & RS_CD_MASK)))
        return (NULL);

    /* get the packet pointer that corresponds to the rx descriptor */
    rxp = di->rxp[i];
    ASSERT(rxp);
    di->rxp[i] = NULL;

    /* clear this packet from the descriptor ring */
    DMA_UNMAP(di->dev, 
        (BUS_SWAP32(R_SM(&di->rxd[i].addr)) - di->dataoffset));
    W_SM(&di->rxd[i].addr, 0);

    di->rxin = NEXTRXD(i);

    return (rxp);
}

char*
dma_soc_dump(void *di_soc, char *buf)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    buf += sprintf(buf, "txd 0x%lx txdpa 0x%lx txp 0x%lx txin %d txout %d\n",
        (ulong)di->txd, (ulong)di->txdpa, (ulong)di->txp, di->txin, di->txout);
    buf += sprintf(buf, "rxd 0x%lx rxdpa 0x%lx rxp 0x%lx rxin %d rxout %d\n",
        (ulong)di->rxd, (ulong)di->rxdpa, (ulong)di->rxp, di->rxin, di->rxout);
    buf += sprintf(buf, 
        "xmtcontrol 0x%x xmtaddr 0x%x xmtptr 0x%x xmtstatus 0x%x\n",
        R_REG(di->dev, &di->regs->xmtcontrol),
        R_REG(di->dev, &di->regs->xmtaddr),
        R_REG(di->dev, &di->regs->xmtptr),
        R_REG(di->dev, &di->regs->xmtstatus));
    buf += sprintf(buf, 
        "rcvcontrol 0x%x rcvaddr 0x%x rcvptr 0x%x rcvstatus 0x%x\n",
        R_REG(di->dev, &di->regs->rcvcontrol),
        R_REG(di->dev, &di->regs->rcvaddr),
        R_REG(di->dev, &di->regs->rcvptr),
        R_REG(di->dev, &di->regs->rcvstatus));
    return (buf);
}

uint
dma_soc_getvar(void *di_soc, char *name)
{
    dma_soc_info_t *di;

    di = (dma_soc_info_t *)di_soc;
    if (!strcmp(name, "&txavail"))
        return ((uint) &di->txavail);
    else {
        ASSERT(0);
    }
    return (0);
}

void
dma_soc_rxrecycle(void *di_soc)
{
    void *p;
    dma_soc_info_t *di;
    uint i;
    void *rxp;
    uint32 pa;
    uint rxbufsize;
    uint ctrl;
    uint rxout;
    void **pkts;
    uint pktc = 0;

    di = (dma_soc_info_t *)di_soc;
    rxbufsize = di->rxbufsize;
    
    /*
     * Re-write descriptor table address
     */
    W_REG(di->dev, &di->regs->rcvaddr, ((uint32)di->rxdpa + di->ddoffset));

    /*
     * Make sure EOT flag is there for the last descriptor
     */
    ctrl = rxbufsize;
    ctrl |= CTRL_EOT;
    W_SM(&di->rxd[di->nrxd - 1].ctrl, BUS_SWAP32(ctrl));

    /*
     * Allocate buffer to store extracted pkts
     */
    pkts = ET_MALLOC(di->nrxd * sizeof(void *));
    if (pkts == NULL) {
        /*
         * Since we're out of memory, just do it the old way.
         */
        dma_soc_rxreclaim(di_soc);
        dma_soc_rxfill(di_soc);
        return;
    }
    
    /*
     * Extract pkts for recycling
     */
    for(i=0; i<di->nrxd; i++) {
        rxp = di->rxp[i];
        if (rxp != NULL) {
            di->rxp[i] = NULL;
            
            pkts[pktc++] = rxp;
        
            DMA_UNMAP(di->dev, 
                     (BUS_SWAP32(R_SM(&di->rxd[i].addr)) - di->dataoffset));
            W_SM(&di->rxd[i].addr, 0);
        }
    }
    
    /*
     * Fill pkts in table from the beginning (top down)
     */
    for(rxout=0; rxout<pktc; rxout++) {
        
        /* Get a pkt from the extracted buffer */
        p = pkts[rxout];
        
        /* rxh.len = 0 means dma writes not complete */
        *(uint32*)(OSL_UNCACHED(ET_PKTDATA(di->dev, p))) = 0;

        /* Get the physical address */
        pa = (uint32) DMA_MAP(di->dev, ET_PKTDATA(di->dev, p));

        /* Save the free pkt in table */
        di->rxp[rxout] = p;

        /* Init the rx descriptor */
        ctrl = rxbufsize;
        W_SM(&di->rxd[rxout].ctrl, BUS_SWAP32(ctrl));
        W_SM(&di->rxd[rxout].addr, BUS_SWAP32(pa + di->dataoffset));

    }
    
    /* Try Filling pkts from rx queue (if we can) */
    for (; rxout < di->nrxd - 1; rxout++) {
            
        if ((p = ET_PKTGET(di->dev, rxbufsize, FALSE)) == NULL) {
            break;
        }
        
        /* We got one more pkt */
        pktc++;
        
        /* rxh.len = 0 means dma writes not complete */
        *(uint32*)(OSL_UNCACHED(ET_PKTDATA(di->dev, p))) = 0;

        /* Get the physical address */
        pa = (uint32) DMA_MAP(di->dev, ET_PKTDATA(di->dev, p));

        /* Save the free pkt in table */
        di->rxp[rxout] = p;

        /* Init the rx descriptor */
        ctrl = rxbufsize;
        W_SM(&di->rxd[rxout].ctrl, BUS_SWAP32(ctrl));
        W_SM(&di->rxd[rxout].addr, BUS_SWAP32(pa + di->dataoffset));
    }
    
    /* Now we have <pktc> descriptors. */
    di->rxin = 0;
    di->rxout = pktc;
    
    /* Update the chip lastdscr pointer */
    W_REG(di->dev, &di->regs->rcvptr, I2B(di->rxout));
    
    /* Free the extraction buffer */
    ET_MFREE(pkts, di->nrxd * sizeof(void *));
}

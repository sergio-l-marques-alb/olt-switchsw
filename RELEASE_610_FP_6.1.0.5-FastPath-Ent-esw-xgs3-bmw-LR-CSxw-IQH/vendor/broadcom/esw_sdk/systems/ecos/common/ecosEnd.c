// Copyright (c) 2004-2005 RealNet Solutions, Inc., All Rights Reserved
//==========================================================================
// 
//      ecosEnd.c
// 
//      Broadcom  ethernet driver
// 
//==========================================================================
#include <sal/types.h>

#if 1
#define TYPEDEF_UINT8
#define TYPEDEF_UINT16
#define TYPEDEF_UINT32
#define TYPEDEF_UINT64
#define TYPEDEF_INT8
#define TYPEDEF_INT16
#define TYPEDEF_INT32
#include <cyg/hal/typedefs.h>
#undef TYPEDEF_UINT8
#undef TYPEDEF_UINT16
#undef TYPEDEF_UINT32
#undef TYPEDEF_UINT64
#undef TYPEDEF_INT8
#define TYPEDEF_INT16
#define TYPEDEF_INT32
#endif

#include <cyg/io/eth/netdev.h>

#define _KERNEL
#include <cyg/io/eth/eth_drv.h>
#undef _KERNEL

#include <cyg/hal/osl.h>

#define BCM_ESW_SUPPORT
#include <bcm/types.h>
#include <bcm/l2.h>
#include <bcm/rx.h>
#include <bcm/mcast.h>
#include <bcm/error.h>
#include <soc/types.h>
#include <soc/enet.h>
#include <soc/drv.h>
#include <soc/debug.h>

#define MAX_TX_DESCRIPTORS 	128
#define dprintf				  
#define DATAHIWAT				50
#define ETH_STATS_INIT( p )

#define ERROR       -1
#define OK              0

/* sgpkt queue */
struct sgpktq {
    struct sgpkt*  head;
    struct sgpkt*  tail;
    uint32    len;
};

typedef struct end_info {
    void            *ndp;	    /* backpoint to network device tab */
    int             index;
    struct sgpktq   txq;
    unsigned long   tx_keys[MAX_TX_DESCRIPTORS];
                                 /*keys for tx q management	*/
    struct sgpkt    *rxpkt;      /* pointer to current received packet */
    uint32            tx_desc_add;
    uint32            tx_desc_remove;
    cyg_handle_t    alarm_hdl;     /* handle to ecos alarm */
    cyg_alarm       alarm_obj;
    cyg_handle_t    counter_hdl;
    bcm_mac_t		mac;		/* SENS: Interface Mac address */
    bcm_vlan_t		vlan;	/* SENS: Interface VLAN ID */
    int			unit;	/* SOC Unit # */
    struct end_info  *next;		/* pointer to next end_info_t in chain */
} end_info_t;

end_info_t end0_priv_data;

ETH_DRV_SC(end_sc0,
    &end0_priv_data,    // Driver specific data
    "eth1",             // Name for this interface
    end_start,
    end_stop,
    end_ioctl,
    end_can_send,
    end_send,
    end_recv,
    end_deliver,       // "pseudoDSR" called from fast net thread
    end_poll,
    end_int_vector);

NETDEVTAB_ENTRY(end_netdev0, 
    "dev_end0", 
    end_drv_init, 
    &end_sc0);

static end_info_t* end_info_array[1] = {
    &end0_priv_data,
}; 

static end_info_t *end_list = NULL;

extern int diag_printf(const char *fmt, ...);

bcm_rx_t end_receive(int unit, bcm_pkt_t *pkt, void *cookie);
void end_send_done(int unit, bcm_pkt_t *pkt, void *cookie);

/* static functions */
static void end_untag(struct sgpkt *sg);
static void end_sendup(end_info_t *et, struct sgpkt *sg);
static void end_enq(struct sgpktq *sgq, struct sgpkt *sg);
static struct sgpkt* end_deq(struct sgpktq *sgq);
static void end_txdone(end_info_t *et);
static void end_sendnext(end_info_t *et);
static void end_init_alarm(end_info_t *et);
static int end_l2_lookup(int unit, bcm_mac_t mac, vlan_id_t vid,
        bcm_pkt_t *pkt);


static void
end_init_drv_priv(end_info_t *et)
{
    int i;

    et->tx_desc_add = 0;
    et->tx_desc_remove = 0;

    for (i=0; i < MAX_TX_DESCRIPTORS; i++)
        et->tx_keys[i] = 0;	
}

static bool
end_drv_init(struct cyg_netdevtab_entry * ndp)
{
    struct eth_drv_sc *sc;
    end_info_t *et;
    bcm_mac_t mac;

    mac[0]=0; 
    mac[1]=0; 
    mac[2]=1; 
    mac[3]=1; 
    mac[4]=2; 
    mac[5]=2;         

    dprintf("end_init1: device initialization start\n");

    /* device instance */
    sc = (struct eth_drv_sc *)ndp->device_instance;
    /* driver private structure */
    et = (end_info_t *) sc->driver_private;

    memset (et, '\0', sizeof(end_info_t));

    et->ndp = ndp;

    et->next = end_list;
    end_list = et;

    end_init_drv_priv(et);
    end_init_alarm(et);
    /* Initialize upper level driver*/
    (sc->funs->eth_drv->init)(sc, &mac);

    bcopy(&mac, &sc->sc_arpcom.ac_enaddr, ETHER_ADDR_LEN);

    dprintf("end_init: device initialization end\n");

    return 1;	
}

void
end_up(end_info_t *et)
{
    return;
}

static void
end_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
#define     ECOS_END_PK_SZ       1600
#define     ECOS_END_HANDLER_PRIO_BASE      100
    int     rv;
    bcm_l2_addr_t	l2addr;
    int                 my_modid;

    end_info_t *et;

#ifdef CYGPKG_NET
	struct ifnet *ifp = &sc->sc_arpcom.ac_if;
#endif

	
    dprintf("end_start: device start begin\n");

    et = sc->driver_private;

    if (!bcm_rx_active(et->unit)) {

        bcm_rx_cfg_t rx_cfg;

        end_up(et);
	
#if 1 /* FIXME : set vlan and mac temporarily */
        {
            et->unit=0;
            et->vlan=1;
            et->mac[0]=0; 
            et->mac[1]=0; 
            et->mac[2]=1; 
            et->mac[3]=1; 
            et->mac[4]=2; 
            et->mac[5]=2;         
        }
#endif

        bcm_rx_cfg_get(et->unit, &rx_cfg);
        rx_cfg.pkt_size = ECOS_END_PK_SZ;
        /* rx_cfg.rx_alloc = socend_packet_alloc;
        rx_cfg.rx_free = socend_packet_free;  */

        if ((rv= bcm_rx_start(et->unit, &rx_cfg)) < 0) {
            dprintf("end_start: Error: Cannot start RX: %s.\n", bcm_errmsg(rv));
            return;
        }
    }else {
        dprintf("RX Thread is already running\n");
    }

    dprintf("socend_start: adding l2 addr\n");
    bcm_l2_addr_t_init(&l2addr, et->mac, et->vlan);
    l2addr.port = CMIC_PORT(et->unit);
    l2addr.flags = BCM_L2_STATIC;
    bcm_stk_my_modid_get(et->unit, &my_modid);
    l2addr.modid = my_modid;
    rv = bcm_l2_addr_add(et->unit, &l2addr);
    if (rv < 0) {
        printk("end_start: bcm_l2_addr_add failed: %s\n", bcm_errmsg(rv));
        return;
    }

    /* Register call back */
    rv = bcm_rx_register(et->unit, "ECOSEND", end_receive, ECOS_END_HANDLER_PRIO_BASE,
            (void *)et, BCM_RCO_F_ALL_COS);

    if (rv < 0) {
        dprintf("Warning:  Could not register socend_receive from RX\n");
        return;
    }

    /*cyg_alarm_initialize (et->alarm_hdl, (cyg_current_time()+300), 300);*/
    dprintf("end_start: device start end\n");
}

void
end_down(end_info_t *et, int reset)
{
    return;
}

static void
end_stop(struct eth_drv_sc *sc)
{
    end_info_t *et;

    dprintf("end_stop: begin\n");

    et = sc->driver_private;	

    dprintf("end_stop: end\n");
}	
	

static int
end_set_mac_address(end_info_t *et, char *addr)

{
    return 0;
}

#ifdef ETH_DRV_GET_MAC_ADDRESS
static int
end_get_mac_address(end_info_t *et, char *addr)
{
    return 0;
}
#endif


static void
end_set_multicast_list(end_info_t *et, struct eth_drv_mc_list *mcl)
{
    return;
}

static int
end_ioctl(struct eth_drv_sc *sc, unsigned long key,
             void *data, int data_length)
{
    end_info_t *et;

    et = (end_info_t *) sc->driver_private;
    struct ifnet *ifp = &sc->sc_arpcom.ac_if;

    switch ( key ) {

#ifdef ETH_DRV_SET_MAC_ADDRESS
        case ETH_DRV_SET_MAC_ADDRESS:
            if ( 6 != data_length )
                return -2;
            return end_set_mac_address( et, data );
#endif

#ifdef ETH_DRV_GET_MAC_ADDRESS
        case ETH_DRV_GET_MAC_ADDRESS:
            return end_get_mac_address( et, data );
#endif

#ifdef ETH_DRV_GET_IF_STATS_UD
        case ETH_DRV_GET_IF_STATS_UD: // UD == UPDATE
            ETH_STATS_INIT( sc );    // so UPDATE the statistics structure
#endif
        // drop through
#ifdef ETH_DRV_GET_IF_STATS
        case ETH_DRV_GET_IF_STATS:
#endif

#ifdef ETH_DRV_SET_MC_LIST
        case ETH_DRV_SET_MC_LIST:    
        {
            struct eth_drv_mc_list *mcl = (struct eth_drv_mc_list *)data;
            end_set_multicast_list(et, mcl);

            return 0;
        }
#endif // ETH_DRV_SET_MC_LIST

#ifdef ETH_DRV_SET_MC_ALL
        case ETH_DRV_SET_MC_ALL:
            return 0;
#endif // ETH_DRV_SET_MC_ALL
    		
        default:
            break;
    }
    return 0;
}

void
end_deliver(struct eth_drv_sc *sc)
{
    return;
}

static void
end_sendnext(end_info_t *et)
{
    struct sgpkt *sg;

    int rv;
    enet_hdr_t		*eh;
    bcm_pkt_t *bcm_pkt;
    char *packet;
    int i;
    const uint16 add_length = sizeof(eh->en_tag_ctrl)+sizeof(eh->en_tag_len);

    /* dequeue and send each packet */
    while (sg = end_deq(&et->txq)) {
        if (NULL == (bcm_pkt = sal_alloc(sizeof(bcm_pkt_t), "bcm pkt"))) {
            dprintf("end_send : memory allocate failed for bcm_pkt\n");
            return;
        }

        if (NULL == (packet = sal_alloc((sg->len+8), "packet"))) {
            sal_free(bcm_pkt);
            dprintf("end_send : allocate memory failed for packet\n");
            return;
        }

        memset(bcm_pkt, 0, sizeof(bcm_pkt_t));
        memset(packet, 0, (sg->len+8));

        eh = (enet_hdr_t *)sg->data;
        bcm_pkt->pkt_len = sg->len+4;

        /* copy mac address */
        memcpy(packet, sg->data, sizeof(sal_mac_addr_t) * 2);

#if 0 /* add tag or not */
        if (!ENET_TAGGED(eh)) { /* If not tagged - add tag and copy */
            bcm_pkt->pkt_len  += add_length;
            for (i=(sizeof(eh->en_dhost)+sizeof(eh->en_shost)) ; i < sg->len ; i++) {
                packet[i+add_length] = sg->data[i];
            }
            /* Set VLAN info */
            eh = (enet_hdr_t *)packet;
            eh->en_tag_tpid = htons(ENET_DEFAULT_TPID);
            eh->en_tag_ctrl = htons(VLAN_CTRL(0,0,et->vlan));
        }
        else { /* If tagged - copy only */
            for (i=(sizeof(eh->en_dhost)+sizeof(eh->en_shost)) ; i < sg->len ; i++) {
                packet[i] = sg->data[i];
            }
        }
#else
        for (i=(sizeof(eh->en_dhost)+sizeof(eh->en_shost)) ; i < sg->len ; i++) {
            packet[i] = sg->data[i];
        }
#endif

        dprintf("#########end_send : bcm_pkt->pkt_len = %d\n", bcm_pkt->pkt_len);
        bcm_pkt->pkt_data = &(bcm_pkt->_pkt_data);
        bcm_pkt->_pkt_data.len = bcm_pkt->pkt_len;
        bcm_pkt->_pkt_data.data = packet;
        bcm_pkt->blk_count = 1;
        bcm_pkt->unit = et->unit;
        SOC_PBMP_CLEAR(bcm_pkt->tx_pbmp);
        SOC_PBMP_CLEAR(bcm_pkt->tx_upbmp); 
        SOC_PBMP_CLEAR(bcm_pkt->tx_l3pbmp);

        /* get tx_pbmp, tx_upbmp */
        eh = (enet_hdr_t *)sg->data;
        rv = end_l2_lookup(et->unit,
                      eh->en_dhost,
                      VLAN_CTRL_ID(et->vlan),
                      bcm_pkt);
        if(rv == ERROR)
            diag_printf("##########end_l2_lookup : failed\n");

        dprintf("##########end_send : et->unit =%d et->vlan=%d.\n", et->unit, et->vlan);
        dprintf("         da =0x%x: 0x%x: 0x%x: 0x%x: 0x%x: 0x%x\n", eh->en_dhost[0], 
        eh->en_dhost[1], eh->en_dhost[2], eh->en_dhost[3], eh->en_dhost[4], eh->en_dhost[5]);
        dprintf("         sa =0x%x: 0x%x: 0x%x: 0x%x: 0x%x: 0x%x\n", eh->en_shost[0], 
        eh->en_shost[1], eh->en_shost[2], eh->en_shost[3], eh->en_shost[4], eh->en_shost[5]);

        dprintf("         bcm_pkt->tx_pbmp.pbits[0]=0x%x bcm_pkt->tx_pbmp.pbits[1]=0x%x bcm_pkt->flags=0x%x\n", bcm_pkt->tx_pbmp.pbits[0], bcm_pkt->tx_pbmp.pbits[1], bcm_pkt->flags);
        dprintf("#############end_send3 : bcm_pkt->_pkt_data.len=%d\n", bcm_pkt->_pkt_data.len);
        for (i=0; i< 50 ; i++)
            dprintf("0x%x ", *(bcm_pkt->_pkt_data.data+i));
        dprintf("\n");
        bcm_pkt->call_back = end_send_done;
        bcm_pkt->cookie = et;

        bcm_pkt->flags &= ~BCM_TX_CRC_FLD;
        bcm_pkt->flags |= BCM_TX_CRC_APPEND;

        if ((rv = bcm_tx(et->unit, bcm_pkt, et)) != BCM_E_NONE) {
            dprintf("bcm_tx failed: Unit %d: %s\n",
            et->unit, bcm_errmsg(rv));
            sal_free(bcm_pkt->_pkt_data.data);
            sal_free(bcm_pkt);
            return;
        } 
    }
}

static void
end_sendup(end_info_t *et, struct sgpkt *sg)
{
    uint16 flags;
    uchar eabuf[32];
    struct cyg_netdevtab_entry *ndp;
    struct eth_drv_sc *sc;

    ndp = (struct cyg_netdevtab_entry *)(et->ndp);
    sc = (struct eth_drv_sc *)(ndp->device_instance);

    /* Remove vlan tag if packet has it */
    end_untag(sg);

    /* eh should now be aligned 2-mod-4 */
    ASSERT(((uint32)sgpkt.data & 3) == 2);

    sg->len -= ETHER_CRC_LEN;	

    et->rxpkt = sg;

    (sc->funs->eth_drv->recv)( sc, sg->len);	

    return;
}

void
end_send_done(int unit, bcm_pkt_t *pkt, void *cookie)
/*
 * Function: 	end_send_done
 * Purpose:	Process a completion of a send request
 * Parameters:	unit - unit #
 *		pkt - The tx packet structure
 *		cookie - pointer to mBlk structure to free
 *                       If null, assume we free the cluster directly.
 * Returns:	Nothing
 * Notes:
 * Assumption:  This function is called from a non-interrupt context.
 */
{
    debugk(DK_END, "Socend send done, in. pkt %p. ck %p.  ", pkt->_pkt_data.data, cookie);
    sal_free(pkt->_pkt_data.data);
    sal_free(pkt);
    debugk(DK_END, "out.\n");
}

static void 
end_send(struct eth_drv_sc *sc,
            struct eth_drv_sg *sg_list, int sg_len, int total_len,
            unsigned long key)
{
    end_info_t *et;
    struct sgpkt *pkt;
    struct eth_drv_sg *last_sg;
    cyg_uint8 *to_p;

    et = (end_info_t *) sc->driver_private;
    /*dprintf("end_send: inside ..\n");*/

    dprintf("end_send: Tx %d: %d sg's, %d bytes, KEY %x\n", et->index,
            sg_len, total_len, key);

    /* discard if transmit queue is too full */
    if (et->txq.len > DATAHIWAT)
        goto qfull;

    et->tx_keys[et->tx_desc_add] = key;

    pkt = PKTGET (0, total_len, 0);
    to_p = (cyg_uint8 *)(pkt->data);
    for ( last_sg = &sg_list[sg_len]; sg_list < last_sg; sg_list++ ) {
        cyg_uint8 *from_p;
        int l;

        from_p = (cyg_uint8 *)(sg_list->buf);
        l = sg_list->len;

        if ( l > total_len )
            l = total_len;

        memcpy(to_p, from_p, l );
        to_p += l;
        total_len -= l;

        if ( 0 > total_len ) 
            break; // Should exit via sg_last normally
    }	

    // Next descriptor
    if ( ++(et->tx_desc_add) >= MAX_TX_DESCRIPTORS)
        et->tx_desc_add = 0;
    end_enq(&et->txq, pkt);
    end_sendnext(et);
    end_txdone(et);
    return;

qfull:
    end_txdone(et);
    return;	
}

/* enqueue sgpkt on queue */
static void
end_enq(struct sgpktq *sgq, struct sgpkt *sg)
{
    ASSERT(sg->next == NULL);

    if (sgq->tail == NULL) {
        ASSERT(sgq->head == NULL);
        sgq->head = sgq->tail = sg;
    }
    else {
        ASSERT(sgq->head);
        ASSERT(sgq->tail->next == NULL);
        sgq->tail->next = sg;
        sgq->tail = sg;
    }
    sgq->len++;
}

/* dequeue sgpkt from queue */
static struct sgpkt*
end_deq(struct sgpktq *sgq)
{
    struct sgpkt *sg;

    if ((sg = sgq->head)) {
        ASSERT(sgq->tail);
        sgq->head = sg->next;
        sg->next = NULL;
        sgq->len--;
        if (sgq->head == NULL)
            sgq->tail = NULL;
    }
    else {
        ASSERT(sgq->tail == NULL);
    }

    return (sg);
}

static void
end_txdone(end_info_t *et)
{
    struct cyg_netdevtab_entry *ndp;
    struct eth_drv_sc *sc;

    ndp = (struct cyg_netdevtab_entry *)(et->ndp);
    sc = (struct eth_drv_sc *)(ndp->device_instance);

    while ( 1 ) {
        unsigned long key = et->tx_keys[ et->tx_desc_remove ];

        if ( (et->tx_desc_remove == et->tx_desc_add))
            break;

        // Zero the key in global state before the callback:
        et->tx_keys[et->tx_desc_remove ] = 0;

        dprintf("TxDone %d : KEY %x \n", et->index, key);

        // tx_done() can now cope with a NULL key, no guard needed here
        (sc->funs->eth_drv->tx_done)( sc, key, 1 /* status */ );

        if ( ++(et->tx_desc_remove) >= MAX_TX_DESCRIPTORS )
            et->tx_desc_remove = 0;
    }	
}

static void 
end_recv( struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len )
{
    end_info_t *et;
    struct eth_drv_sg *last_sg;
    volatile cyg_uint8 *from_p;
    unsigned long total_len;

    et = (end_info_t *) sc->driver_private;

    from_p = (cyg_uint8*)(et->rxpkt->data);
    total_len = et->rxpkt->len;

    for ( last_sg = &sg_list[sg_len]; sg_list < last_sg; sg_list++ ) {
        cyg_uint8 *to_p;
        int l;

        to_p = (cyg_uint8 *)(sg_list->buf);
        l = sg_list->len;

        CYG_ASSERT( 0 <= l, "sg length -ve" );

        if ( 0 == to_p )
        {
#if 0 /* FIXME : it could cause memory shortage by masking here */
            PKTFREE(0, et->rxpkt,0);            
#endif
            return; // caller was out of mbufs
        }
        if ( l > total_len )
            l = total_len;

        memcpy( to_p, (unsigned char *)from_p, l );
        from_p += l;
        total_len -= l;
    }
#if 0 /* FIXME : it could cause memory shortage by masking here */
    PKTFREE(0,et->rxpkt,0);	
#endif
}

//
// This routine is called to see if it is possible to send another packet.
// It will return non-zero if a transmit is possible, zero otherwise.
//
static int
end_can_send(struct eth_drv_sc *sc)
{
    return 1;
}

void
end_poll(struct eth_drv_sc *sc)
{
    return;
}

// ------------------------------------------------------------------------
// Determine interrupt vector used by a device - for attaching GDB stubs
// packet handler.
static int
end_int_vector(struct eth_drv_sc *sc)
{
    return 0;
}


cyg_alarm_t end_watchdog;

static void
end_init_alarm(end_info_t *et)
{
    cyg_handle_t sys_clk;

    sys_clk = cyg_real_time_clock();

    cyg_clock_to_counter(sys_clk, &et->counter_hdl);

    cyg_alarm_create (et->counter_hdl, end_watchdog,
            (cyg_addrword_t)et, &et->alarm_hdl, &et->alarm_obj);

}

void
end_watchdog(cyg_handle_t alarm_handle, cyg_addrword_t data)
{
    end_info_t *et;

    et = (end_info_t*)data;
}

/*
 * Function: 	end_receive
 * Purpose:	Called Address mux when packet matches MAC address.
 * Parameters:	unit - StrataSwitch unit #
 *		port - StrataSwitch port #
 *		cookie
 * Returns:
 *	BCM_RX_HANDLED - packet received for unknown reason
 *	BCM_RX_HANDLED_OWNED - packet owned.
 */

bcm_rx_t
end_receive(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int rv;
    struct sgpkt spkt;
    int i;

    spkt.len = pkt->pkt_len;
    spkt.data = pkt->pkt_data->data; 

    dprintf("ecosEnd_receive1 : spkt.len=%d pkt->pkt_len=%d pkt->tot_len=%d\n", spkt.len, pkt->pkt_len, pkt->tot_len);
    for (i=0; i< 50 ; i++)
        dprintf("0x%x ", *(spkt.data+i));
    dprintf("\n");

    end_sendup(cookie, &spkt);
    return BCM_RX_HANDLED;
}

/*
 * Function:
 *	end_l2_lookup
 * Purpose:
 *	Determine packets egress port(s) based on MAC and VLAN ID
 * Parameters:
 *	unit - StrataSwitch Unit #.
 *	mac - mac address of destination.
 *	vid - VLAN ID of destination.
 *	pkt - Where the port bitmaps are stored
 * Returns:
 *	OK/ERROR
 * Notes:
 */

static int
end_l2_lookup(int unit, bcm_mac_t mac, vlan_id_t vid,
                    bcm_pkt_t *pkt)
{
    int                 rv = OK;
    bcm_port_t          port = 0;
    pbmp_t              raw_pbm;
    pbmp_t              l_pbm;
    pbmp_t              l_upbm;
    pbmp_t              l_l3pbm;     /* To be implemented */
    bcm_mcast_addr_t    mcaddr;
    bcm_module_t        modid = 0;
    int                 my_modid;

    BCM_PBMP_CLEAR(raw_pbm);
    BCM_PBMP_CLEAR(l_pbm);
    BCM_PBMP_CLEAR(l_upbm);
    BCM_PBMP_CLEAR(l_l3pbm);

    bcm_stk_my_modid_get(unit, &my_modid);
    if (ENET_MACADDR_UNICAST(mac)) {
        bcm_l2_addr_t l2addr;
        
        pkt->opcode = BCM_HG_OPCODE_UC;	
        rv = bcm_l2_addr_get(unit, mac, vid, &l2addr);
        modid = l2addr.modid;
        port = (modid == my_modid) ? l2addr.port : IPIC_PORT(unit);

        BCM_PBMP_ASSIGN(pkt->dest_mod, modid);
        BCM_PBMP_ASSIGN(pkt->dest_port, l2addr.port);

        if (rv >= 0) { /* Determine if it's going out untagged */
            /* rv = bcm_vlan_port_get(unit, vid, &l_pbm, &l_upbm);
	    BCM_PBMP_CLEAR(l_pbm); */
	    BCM_PBMP_PORT_ADD(l_pbm, port);

        } else {  /* combine all error return values */
            rv = ERROR;
        }
    } else if (ENET_MACADDR_MULTICAST(mac)) {
#if 1 /* FIXME : set port 0 for multicast */
        pkt->opcode = BCM_HG_OPCODE_MC;	
        rv = bcm_mcast_port_get(unit, mac, vid, &mcaddr);
        if (rv >= 0) {
        } else {  /* combine all error return values */
            rv = ERROR;
        }
	BCM_PBMP_ASSIGN(raw_pbm, mcaddr.pbmp);
	BCM_PBMP_ASSIGN(l_pbm, mcaddr.pbmp);
	BCM_PBMP_ASSIGN(l_upbm, mcaddr.ubmp);
#else
        modid = 0;
        port = 0;
        BCM_PBMP_ASSIGN(pkt->dest_mod, modid);
        BCM_PBMP_ASSIGN(pkt->dest_port, port);
        BCM_PBMP_PORT_ADD(l_pbm, port);        
#endif	
    } else if (ENET_MACADDR_BROADCAST(mac)) {
        pkt->opcode = BCM_HG_OPCODE_BC;	    
        rv = ERROR;
    } else {
        rv = ERROR;
    }

    switch(rv) {
    case ERROR:
        /*
         * If no match was found, then spray out all ports in vlan.
         */
#if 1 /* FIXME : set port 0 for broadcast */
        rv = bcm_vlan_port_get(unit, vid, &l_pbm, &l_upbm);
#else
        modid = 0;
        port = 0;
        BCM_PBMP_ASSIGN(pkt->dest_mod, modid);
        BCM_PBMP_ASSIGN(pkt->dest_port, port);
        BCM_PBMP_PORT_ADD(l_pbm, port);        
#endif

        if (rv >= 0) {
            rv = OK;
        } else {  /* combine all error return values */
            rv = ERROR;
        }
        break;
    default:
        break;
    }

    BCM_PBMP_PORT_REMOVE(l_pbm, CMIC_PORT(unit));	/* Never back to us */

    pkt->flags |= BCM_TX_SRC_MOD;
    BCM_PBMP_ASSIGN(pkt->src_mod, my_modid);
    pkt->flags |= BCM_TX_SRC_PORT;
    BCM_PBMP_ASSIGN(pkt->src_port, CMIC_PORT(unit));
    BCM_PBMP_ASSIGN(pkt->tx_pbmp, l_pbm);
    BCM_PBMP_ASSIGN(pkt->tx_upbmp, l_upbm);
    BCM_PBMP_ASSIGN(pkt->tx_l3pbmp, l_l3pbm);	/* Always 0 for now */
    
    return(rv);
}

/*
 * Function: 	end_untag
 * Purpose:	Untag an ethernet packet (if tagged)
 * Parameters:	sg 
 * Returns:	Nothing
 */
static	void
end_untag(struct sgpkt *sg)
{
    enet_hdr_t	*th;		/* Tagged header pointers */
    const uint16 dec_length = sizeof(th->en_tag_ctrl)+sizeof(th->en_tag_len);
    int i;

    th = (enet_hdr_t *)sg->data;

    if (ENET_TAGGED(th)) {		/* If tagged - strip */
        sg->len  -= dec_length;
        for (i=(sizeof(th->en_dhost)+sizeof(th->en_shost)) ; i < sg->len ; i++) {
            sg->data[i] = sg->data[i+dec_length];
        }
    }
    return;
}


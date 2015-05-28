/*
 * $Id: port.c,v 1.36 Broadcom SDK $
 * 
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 */


#include <soc/defs.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/mem.h>

#include <sal/core/time.h>
#include <shared/bsl.h>
#include <bcm/port.h>
#include <bcm/tx.h>
#include <bcm/error.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/saber2.h>
#include <bcm_int/esw/cosq.h>

#include <soc/katana2.h>
#include <soc/saber2.h>
#include <soc/phyreg.h>
#include <soc/ll.h>

#if defined(BCM_SABER2_SUPPORT)
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/virtual.h>

static uint32   new_tdm[SB2_TDM_MAX_SIZE] = {0};
extern int      block_max_startaddr[SB2_MAX_BLOCKS];
extern int      block_max_endaddr[SB2_MAX_BLOCKS];
extern sb2_port_to_block_subports_t *sb2_port_to_block_subports[SOC_MAX_NUM_DEVICES];

#define SB2_SPEED_MODE_LINK_1G          2
#define SB2_SPEED_MODE_LINK_2G5         3
#define SB2_SPEED_MODE_LINK_10G_PLUS    4

int
_bcm_sb2_port_lanes_set_post_operation(int unit, bcm_port_t port)
{
    soc_error_t        rv = SOC_E_NONE;
    uint8              loop = 0;
    uint8              block = 0;
    uint8              block_port = 0;
    int                mode = 0;
    int                new_lanes = 0;
    int                port_enable = 0;
    int                speed=0, max_speed=0;


    SOC_IF_ERROR_RETURN(soc_saber2_get_port_block(
                unit,port,&block));
    SOC_IF_ERROR_RETURN(_bcm_sb2_port_lanes_get(
                unit,port,&new_lanes));

#ifdef BCM_PORT_DEFAULT_DISABLE
        port_enable = FALSE;
#else
            port_enable = TRUE;
#endif  /* BCM_PORT_DEFAULT_DISABLE */


    for (loop = 0;loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit,
                        block_port, port_enable));

            speed = (*sb2_port_speeds[unit])[block][new_lanes-1];
            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_max(unit,
                        block_port,  &max_speed));
            speed = speed > max_speed ? max_speed : speed;
            rv = bcm_esw_port_speed_set(unit, block_port, speed);
            SOC_IF_ERROR_RETURN(rv);

            SOC_IF_ERROR_RETURN(bcm_esw_linkscan_mode_get(
                        unit, block_port, &mode));

            if (mode == BCM_LINKSCAN_MODE_NONE) {
                BCM_IF_ERROR_RETURN(bcm_esw_linkscan_mode_set(
                            unit, block_port, BCM_LINKSCAN_MODE_SW));
                BCM_IF_ERROR_RETURN(bcm_esw_port_learn_set(unit,
                            block_port,
                            BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
                BCM_IF_ERROR_RETURN(bcm_esw_port_stp_set(
                            unit, block_port, BCM_STG_STP_FORWARD));
            }
        } /* if SOC_PBMP_MEMBER */
    } /* For loop */

    return SOC_E_NONE;
}

void soc_saber2_pbmp_add(int unit,soc_port_t port,int block,int speed) {
    int         blk=0;
    soc_info_t  *si = &SOC_INFO(unit);
    soc_pbmp_t  my_pbmp_xport_xe;

    blk = SOC_DRIVER(unit)->port_info[port].blk;
    SOC_PBMP_PORT_ADD(si->block_bitmap[blk],port);

    if (block < 6) {
         /* mxq ports */
         if (speed == 10000) {
             SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
             SOC_PBMP_PORT_REMOVE(si->xe.disabled_bitmap, port);
         } else {
             SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
             SOC_PBMP_PORT_REMOVE(si->ge.disabled_bitmap, port);
         }
         SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->ether.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->mxq.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->mxq.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->port.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->port.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->all.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);

         SOC_PORT_TYPE(unit,port)=SOC_BLK_MXQPORT;
         si->port_speed_max[port] = speed;
    } else if (block == 6) {
        /* xl ports */
        SOC_PBMP_CLEAR(my_pbmp_xport_xe);
        my_pbmp_xport_xe = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_XE,
                                                     my_pbmp_xport_xe);

        SOC_PBMP_PORT_ADD(si->xl.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->xl.disabled_bitmap, port);

        if (SOC_PBMP_MEMBER(my_pbmp_xport_xe, port)) {
             if (speed == 10000) {
                 SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
                 SOC_PBMP_PORT_REMOVE(si->xe.disabled_bitmap, port);
             } else {
                 SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
                 SOC_PBMP_PORT_REMOVE(si->ge.disabled_bitmap, port);
             }
        } else {
             SOC_PBMP_PORT_ADD(si->hg.bitmap, port);
             SOC_PBMP_PORT_ADD(si->st.bitmap, port);
             SOC_PBMP_PORT_REMOVE(si->hg.disabled_bitmap, port);
             SOC_PBMP_PORT_REMOVE(si->st.disabled_bitmap, port);
            
             bcm_esw_port_encap_set(unit, port, BCM_PORT_ENCAP_HIGIG);
        }

         SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->ether.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->port.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->port.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->all.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);
         
         SOC_PORT_TYPE(unit,port)=SOC_BLK_XLPORT;
         si->port_speed_max[port] = speed;
    }
}

int _bcm_sb2_update_port_mode(int unit,uint8 port,int speed) {
    uint32      rval; 
    uint8       block;
    bcmMxqPhyPortMode_t phy_mode;
    bcmXlCorePortMode_t core_mode_xl;
    bcmXlPhyPortMode_t  phy_mode_xl;
     
    SOC_IF_ERROR_RETURN(soc_saber2_get_port_block(
                                unit, port, &block));
    
    SOC_IF_ERROR_RETURN(soc_saber2_get_mxq_phy_port_mode(
                unit, port, speed, &phy_mode));
    if (SOC_REG_PORT_VALID(unit, XPORT_MODE_REGr, port)) {
        SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
        soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, PHY_PORT_MODEf, phy_mode);
        soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                PORT_GMII_MII_ENABLEf, (speed >= 10000) ? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
    }

    SOC_IF_ERROR_RETURN(soc_saber2_get_xl_phy_core_port_mode(unit, port,
            &phy_mode_xl, &core_mode_xl));
    if (SOC_REG_PORT_VALID(unit, XLPORT_MODE_REGr, port)) {
        SOC_IF_ERROR_RETURN(READ_XLPORT_MODE_REGr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                EGR_1588_TIMESTAMPING_CMIC_48_ENf, 1);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                XPORT0_CORE_PORT_MODEf,core_mode_xl);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                XPORT0_PHY_PORT_MODEf, phy_mode_xl);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));
    }
    
    return SOC_E_NONE;
}

soc_error_t soc_saber2_port_detach(int unit,uint8 block_port) {
    soc_linkscan_pause(unit);
    
    SOC_IF_ERROR_RETURN(soc_phyctrl_enable_set(unit, block_port, 1));
    SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, block_port));
    PHY_FLAGS_CLR_ALL(unit, block_port);
    bcm_esw_port_enable_set(unit, block_port, 0);

    soc_linkscan_continue(unit);
    return SOC_E_NONE;
}

soc_error_t soc_saber2_port_attach(
        int unit, uint8 block, uint8 block_port ) {
    uint16 phy_addr=0;
    uint16 phy_addr_int=0;
    int okay = 0;

    soc_linkscan_pause(unit);
    _saber2_phy_addr_default(unit, block_port,
                              &phy_addr, &phy_addr_int);
    SOC_IF_ERROR_RETURN
        (soc_phy_cfg_addr_set(unit,block_port,0, phy_addr));
    SOC_IF_ERROR_RETURN
        (soc_phy_cfg_addr_set(unit,block_port,SOC_PHY_INTERNAL,
                              phy_addr_int));
    PHY_ADDR(unit, block_port)     = phy_addr;
    PHY_ADDR_INT(unit, block_port) = phy_addr_int;
    SOC_IF_ERROR_RETURN(_bcm_port_probe(unit, block_port, &okay));
    if (!okay) {
        soc_linkscan_continue(unit);
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(_bcm_port_mode_setup(unit, block_port, FALSE));

    soc_linkscan_continue(unit);
    return SOC_E_NONE;
}

int _bcm_sb2_port_lanes_set(int unit, bcm_port_t port, int lanes)
{
    uint32              start_addr=0;
    uint32              end_addr=0;
    soc_pbmp_t          link_pbmp;
    uint32              time_multiplier=1;
    uint64              rval64 ;
    uint32              rval = 0, rval1 = 0;
    uint32              cell_cnt = 0;
    uint32              try = 0 , try_count = 0;
    soc_timeout_t       to = {0};
    sal_usecs_t         timeout_usec = 100000;
    int                 min_polls = 100;
    uint32             pfc_enable=0;
    uint8              old_ports[][SB2_MAX_PORTS_PER_BLOCK]=
                                  {{1,0,0,0},{1,0,3,0},{0,0,0,0},{1,2,3,4}};
    uint8              new_ports[][SB2_MAX_PORTS_PER_BLOCK]=
                                  {{1,0,0,0},{1,0,3,0},{0,0,0,0},{1,2,3,4}};
    uint8              new_port_mode[5]={0,1,2,0,3};
    soc_info_t         *si = &SOC_INFO(unit);
    uint8              block = 0;
    uint8              block_port = 0;
    uint8              try_loop = 0;
    uint8              loop = 0;
    int                old_lanes = 0;
    int                new_lanes = lanes;
    soc_field_t        port_intf_reset_fld[] = {
                       XQ1_PORT_INTF_RESETf,
                       XQ2_PORT_INTF_RESETf, XQ3_PORT_INTF_RESETf,
                       XQ4_PORT_INTF_RESETf, XQ5_PORT_INTF_RESETf,
                       XQ6_PORT_INTF_RESETf, XQ7_PORT_INTF_RESETf,
                       XQ8_PORT_INTF_RESETf, XQ9_PORT_INTF_RESETf};
    soc_field_t        mmu_intf_reset_fld[] = {
                       XQ1_MMU_INTF_RESETf,
                       XQ2_MMU_INTF_RESETf, XQ3_MMU_INTF_RESETf,
                       XQ4_MMU_INTF_RESETf, XQ5_MMU_INTF_RESETf,
                       XQ6_MMU_INTF_RESETf, XQ7_MMU_INTF_RESETf,
                       XQ8_MMU_INTF_RESETf, XQ9_MMU_INTF_RESETf};
    soc_field_t        new_port_mode_fld[] = {
                       XQ1_NEW_PORT_MODEf,
                       XQ2_NEW_PORT_MODEf, XQ3_NEW_PORT_MODEf,
                       XQ4_NEW_PORT_MODEf, XQ5_NEW_PORT_MODEf,
                       XQ6_NEW_PORT_MODEf, XQ7_NEW_PORT_MODEf,
                       XQ8_NEW_PORT_MODEf, XQ9_NEW_PORT_MODEf};
    uint8              speed_value = 0;
    int                port_speed = 0;
    uint8              tdm_ports[4] = {0};
    uint8              index = 0;
    uint32             pos = 0;
    uint32             egr_fifo_depth = 0;
    int                block_max_nxtaddr;
#if defined(SABER2_DEBUG)
    char               config_str[80];
    uint8              lane_incr[SB2_MAX_PORTS_PER_BLOCK]={4,2,0,1};
    int                auto_portgroup=0;
    int                portgroup=0;
#endif
   soc_field_t          port_enable_field[SB2_MAX_PORTS_PER_BLOCK]=
                            {PORT0f, PORT1f, PORT2f , PORT3f};
    int                init_mode = 0;
    int                port_enable = 0;
    egr_enable_entry_t egr_enable_entry = {{0}}; 
    int                block_speed=0, max_speed=0;

    epc_link_bmap_entry_t epc_link_bmap_entry={{0}};
    txlp_port_addr_map_table_entry_t txlp_port_addr_map_table_entry={{0}};

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (SAL_BOOT_QUICKTURN) {
        time_multiplier=1000;
    }
    
    if (IS_MXQ_PORT(unit, port)) {
        if ((new_lanes != 1) && (new_lanes != 4)) {
            return BCM_E_PARAM;
        }
    } else if(IS_XL_PORT(unit, port)) {
        if ((new_lanes != 1) && (new_lanes != 4)) {
            return BCM_E_PARAM;
        }
    }

    SOC_IF_ERROR_RETURN(soc_saber2_get_port_block(
                unit,port,&block));
    SOC_IF_ERROR_RETURN(_bcm_sb2_port_lanes_get(
                unit,port,&old_lanes));

    if(old_lanes == new_lanes) {
        return SOC_E_NONE;
    }

    if (block == 0) {
        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            if ((IS_LP_PORT(unit, (*sb2_block_ports[unit])[block][loop])) ||
                    (IS_SUBTAG_PORT(unit, (*sb2_block_ports[unit])[block][loop]))) {
                LOG_CLI((BSL_META_U(unit,
                                "HotSwap is not supported for \
                                Subport Coe/LinkPhy Ports\n \
                                Disable CoE and run the API again.\n")));
                return BCM_E_PARAM;
            }
        }
    }
    
    block_speed = 0;
    for (loop = 0;loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_max(unit,
                        block_port,  &max_speed));
            block_speed += max_speed;
        }
    }
    if (block_speed < (*sb2_port_speeds[unit])[block][new_lanes-1]) {
        LOG_CLI((BSL_META_U(unit,
                        "Total BW of block %d is less than %d\n"),block_speed,
                    (*sb2_port_speeds[unit])[block][new_lanes-1]));
        return BCM_E_PARAM;
    }

    if (new_lanes == 1) {
        if(!(IS_XL_PORT(unit, port))) {
            tdm_ports[0] = (*sb2_block_ports[unit])[block][0];
            tdm_ports[1] = (*sb2_block_ports[unit])[block][0];
            tdm_ports[2] = (*sb2_block_ports[unit])[block][0];
            tdm_ports[3] = (*sb2_block_ports[unit])[block][0];
        }
    }

    if (new_lanes == 2) {
        if(!(IS_XL_PORT(unit, port))) {
            tdm_ports[0] = (*sb2_block_ports[unit])[block][0];
            tdm_ports[1] = (*sb2_block_ports[unit])[block][2];
            tdm_ports[2] = (*sb2_block_ports[unit])[block][0];
            tdm_ports[3] = (*sb2_block_ports[unit])[block][2];
        }
    }

    if (new_lanes == 4) {
        tdm_ports[0] = (*sb2_block_ports[unit])[block][0];
        tdm_ports[1] = (*sb2_block_ports[unit])[block][1];
        tdm_ports[2] = (*sb2_block_ports[unit])[block][2];
        tdm_ports[3] = (*sb2_block_ports[unit])[block][3];
    }

    sal_memcpy(&new_tdm[0],&sb2_current_tdm[0],
            sb2_current_tdm_size *sizeof(sb2_current_tdm[0]));

    if(!(IS_XL_PORT(unit, port))) {
        for (index=0,loop=0; loop < sb2_tdm_pos_info[block].total_slots; loop++) {
            pos = sb2_tdm_pos_info[block].pos[loop];
            new_tdm[pos]=tdm_ports[index];
            index = (index + 1) % 4 ;
        }
    }

    BCM_IF_ERROR_RETURN(bcm_esw_linkscan_mode_get(unit, port, &init_mode));

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            /* 1. Stop linkscan on the port */
            bcm_esw_kt2_port_unlock(unit);
            BCM_IF_ERROR_RETURN(bcm_esw_linkscan_mode_set(
                        unit, block_port, BCM_LINKSCAN_MODE_NONE));
            bcm_esw_kt2_port_lock(unit);

            /* 2. To stop the incoming traffic, disable XLMAC Rx */
            BCM_IF_ERROR_RETURN(soc_mac_xl.md_enable_set(unit, block_port, FALSE));
        }
    }

    sal_udelay((10 * time_multiplier));
    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            /* 3. Brings the port down. */
            BCM_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(
                        unit, MEM_BLOCK_ANY, 0, &epc_link_bmap_entry));
            soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm,
                    &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
            BCM_PBMP_PORT_REMOVE(link_pbmp,block_port);
            soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm,
                    &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
            SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(
                        unit, MEM_BLOCK_ALL, 0, &epc_link_bmap_entry));
        }
    }

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {

#ifdef PORT_FLUSH_DEBUG
            /* 4. Issues MMU port flush command */ 
            /* PortFlushing creating some issue with L0 node movement.
             * So moving to Q Flush.
             */
            BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &flush_reg));
            soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, 
                    FLUSH_ACTIVEf,1);
            /* PORT FLUSHING not QUEUE FLUSHING */
            soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, FLUSH_TYPEf,1);
            soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, FLUSH_NUMf,1);
            soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, 
                    FLUSH_ID0f, block_port);
            BCM_IF_ERROR_RETURN(WRITE_TOQ_FLUSH0r(unit, flush_reg));
#endif


            /* 5. Save the current configuration of PAUSE/PFC flow control */ 
            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(
                        unit, block_port, &port_speed));
            if (!(SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) { 
                if (port_speed == 10000) {
                    BCM_IF_ERROR_RETURN(READ_XPORT_TO_MMU_BKPr(
                                unit, block, &pfc_enable));
                    BCM_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(
                                unit, block, 0));
                }
             }
           
            /* 6. flushes any pending egress packets in MXQPORT */
            if (SOC_REG_PORT_VALID(unit, IECELL_TXFIFO_PKT_DROP_CTLr, block_port)) {
                READ_IECELL_TXFIFO_PKT_DROP_CTLr(unit, block_port, &rval);
                soc_reg_field_set(unit, IECELL_TXFIFO_PKT_DROP_CTLr, &rval, 
                        DROP_ENf, 1);
                WRITE_IECELL_TXFIFO_PKT_DROP_CTLr(unit, block_port, rval);
            }

            if (SOC_REG_PORT_VALID(unit, XP_TXFIFO_PKT_DROP_CTLr, block_port)) {
                READ_XP_TXFIFO_PKT_DROP_CTLr(unit, block_port, &rval);
                soc_reg_field_set(unit, XP_TXFIFO_PKT_DROP_CTLr, &rval, 
                        DROP_ENf, 1);
                WRITE_XP_TXFIFO_PKT_DROP_CTLr(unit, block_port, rval);
            }

#ifndef PORT_FLUSH_DEBUG
            BCM_IF_ERROR_RETURN(bcm_kt2_cosq_port_flush(
                        unit, block_port, bcmCosqFlushTypeQueue));
#endif

#ifdef PORT_FLUSH_DEBUG
            /* 7. Waits until all egress port packets are drained. */
            try_count=0;
            if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
                BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval));
                soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_ACTIVEf, 0);
                BCM_IF_ERROR_RETURN(WRITE_TOQ_FLUSH0r(unit, rval));
            }

            do {
                soc_timeout_init(&to, timeout_usec, min_polls);
                if(soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                            (BSL_META_U(unit,
                                        "%s:%d:TimeOut InternalError\n"),
                             FUNCTION_NAME(),__LINE__));
                    return BCM_E_INTERNAL;
                }
                try_count++;
                BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval));
                if (soc_reg_field_get(
                            unit, TOQ_FLUSH0r, rval, FLUSH_ACTIVEf) == 0) {
                    break;
                }
            } while (try_count != 3);

            if (try_count == 3) {
                return BCM_E_TIMEOUT;
            }
#endif

            try_count=0;
            if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
                if (SOC_REG_PORT_VALID(unit, XP_TXFIFO_CELL_CNTr, block_port)) {
                    BCM_IF_ERROR_RETURN(READ_XP_TXFIFO_CELL_CNTr(
                                unit, block_port, &rval));
                    soc_reg_field_set(unit, XP_TXFIFO_CELL_CNTr,
                            &rval, CELL_CNTf, 0);
                    BCM_IF_ERROR_RETURN(WRITE_XP_TXFIFO_CELL_CNTr(
                                unit, block_port, rval));
                }

                if (SOC_REG_PORT_VALID(unit, IECELL_TXFIFO_CELL_CNTr, block_port)) {
                    BCM_IF_ERROR_RETURN(READ_IECELL_TXFIFO_CELL_CNTr(
                                unit, block_port, &rval));
                    soc_reg_field_set(unit, IECELL_TXFIFO_CELL_CNTr,
                            &rval, CELL_CNTf, 0); 
                    BCM_IF_ERROR_RETURN(WRITE_IECELL_TXFIFO_CELL_CNTr(
                                unit, block_port, rval));
                }   
 
            }
            soc_timeout_init(&to, timeout_usec, min_polls);
            for (try=0; try<100 && try_count < 10 ; try++) {
                if(soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                            (BSL_META_U(unit,
                                        "%s:%d:TimeOut InternalError\n"),
                             FUNCTION_NAME(),__LINE__));
                    return SOC_E_INTERNAL; 
                }

                BCM_IF_ERROR_RETURN(READ_XP_TXFIFO_CELL_CNTr (
                            unit,block_port, &rval));
                cell_cnt=soc_reg_field_get(unit, XP_TXFIFO_CELL_CNTr,     
                        rval, CELL_CNTf);
                if (cell_cnt == 0) {
                    try_count++;
                    break;     
                }
            }

            if (try == 100) {
                return SOC_E_TIMEOUT;
            }
        }
    }

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
        
            /* 8. Powerdown Vipercore/Eaglecore serdes */
            if (SOC_REG_PORT_VALID(unit, XPORT_XGXS_CTRLr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit, block_port, &rval));
                soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval,
                        IDDQf,1);
                soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval,
                        PWRDWNf,1);
                WRITE_XPORT_XGXS_CTRLr(unit, block_port,rval);
            }


            if (SOC_REG_PORT_VALID(unit, XLPORT_XGXS0_CTRL_REGr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XLPORT_XGXS0_CTRL_REGr(unit, 
                            block_port, &rval));
                soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, 
                        PWRDWNf, 1);
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_XGXS0_CTRL_REGr(unit, 
                            block_port, rval));
            }

            /* 9. Disable egress cell request generation. */
            if(block > 0) {
                sal_memset(&egr_enable_entry, 0, sizeof(egr_enable_entry_t));
                soc_mem_field32_set(unit, EGR_ENABLEm, &egr_enable_entry, PRT_ENABLEf, 0);
                SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, block_port,
                            &egr_enable_entry));
            } else {
                SOC_IF_ERROR_RETURN(READ_TXLP_PORT_ENABLEr(unit, port,
                            &rval));
                port_enable = soc_reg_field_get(unit, TXLP_PORT_ENABLEr, rval,
                        PORT_ENABLEf);
                port_enable &= ~(1 << (loop));
                soc_reg_field_set(unit, TXLP_PORT_ENABLEr, &rval,
                        PORT_ENABLEf, port_enable);
                BCM_IF_ERROR_RETURN(WRITE_TXLP_PORT_ENABLEr(unit, port, rval));
            }

            /* 10. disable the MXQPORT flush. */
            if (SOC_REG_PORT_VALID(unit, XP_TXFIFO_PKT_DROP_CTLr, block_port)) {
                READ_XP_TXFIFO_PKT_DROP_CTLr(unit, block_port,&rval);
                soc_reg_field_set(unit, XP_TXFIFO_PKT_DROP_CTLr, &rval, 
                        DROP_ENf,0);
                WRITE_XP_TXFIFO_PKT_DROP_CTLr(unit, block_port,rval);
            }
            
            if (SOC_REG_PORT_VALID(unit, IECELL_TXFIFO_PKT_DROP_CTLr, block_port)) {
                READ_IECELL_TXFIFO_PKT_DROP_CTLr(unit, block_port,&rval);
                soc_reg_field_set(unit, IECELL_TXFIFO_PKT_DROP_CTLr, &rval, 
                        DROP_ENf,0);
                WRITE_IECELL_TXFIFO_PKT_DROP_CTLr(unit, block_port,rval);
            }
            /* 11. Restore PFC */
            if (!(SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
                if (port_speed == 10000) {
                    BCM_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(
                                unit, block, pfc_enable));
                }
            }
            /* PLEASE NOTE THAT PORT IS STILL DOWN */

        } /* If SOC_PBMP_MEMBER */
    } /* for loop */

    /* Update local pbmp's */
    soc_linkscan_pause(unit);
    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if ((old_ports[old_lanes-1][loop] == 0) &&
                (new_ports[new_lanes-1][loop] == 0)) {
            /* Either not used  */
            continue;
        }

        si->port_speed_max[block_port]=(*sb2_port_speeds[unit])[block]
            [new_lanes-1];
        if (old_ports[old_lanes-1][loop] == new_ports[new_lanes-1][loop] ) {
            /* same port was used so only speed change */
            soc_katana2_pbmp_remove(unit,block_port);
            soc_saber2_pbmp_add(unit,block_port,block,
                    si->port_speed_max[block_port]);
        }
        if (old_ports[old_lanes-1][loop]  > new_ports[new_lanes-1][loop]) {
            /* This port is not applicable so remove it from pbmp list */
            soc_katana2_pbmp_remove(unit,block_port);
            continue;
        }
        if (new_ports[new_lanes-1][loop]  > old_ports[old_lanes-1][loop]) {
            /* This port is new so add it in pbmp list first */
            soc_saber2_pbmp_add(unit,block_port, block,
                    si->port_speed_max[block_port]);
        }
    }
    soc_katana2_pbmp_all_resync(unit) ;
    soc_esw_dport_init(unit);
    soc_linkscan_continue(unit);
    SOC_IF_ERROR_RETURN(_soc_saber2_mmu_reconfigure(unit)); 

    /* ReConfigure H/W */
    /* 1. Soft-reset relevant MXQPORT(s) */
    if (block < 6) { 
        soc_saber2_block_reset(unit, block, 0);
    } else {
        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            block_port=(*sb2_block_ports[unit])[block][loop];
            if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
                if (SOC_REG_PORT_VALID(unit, IECELL_CONFIGr, block_port)) {
                    SOC_IF_ERROR_RETURN(READ_IECELL_CONFIGr(unit, block_port, &rval));
                    soc_reg_field_set(unit, IECELL_CONFIGr, &rval, SOFT_RESETf, 1);
                    SOC_IF_ERROR_RETURN(WRITE_IECELL_CONFIGr(unit, block_port, rval));
                }

                if (SOC_REG_PORT_VALID(unit, XLPORT_XGXS0_CTRL_REGr, block_port)) {
                    SOC_IF_ERROR_RETURN(READ_XLPORT_XGXS0_CTRL_REGr(unit, block_port, &rval));
                    soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, RSTB_HWf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_XLPORT_XGXS0_CTRL_REGr(unit, block_port, rval));
                }
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0xF));
    }

    /* ReConfigure TDM */
    soc_saber2_reconfigure_tdm(unit, sb2_current_tdm_size, new_tdm); 
    sal_memcpy(&sb2_current_tdm[0],&new_tdm[0],
            sb2_current_tdm_size *sizeof(sb2_current_tdm[0]));

    block_max_nxtaddr = block_max_startaddr[block];
    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            BCM_IF_ERROR_RETURN(READ_DEQ_EFIFO_CFGr(unit,block_port,&rval));

            if (si->port_speed_max[block_port] <= 2500) {
                egr_fifo_depth = 11;
            } else if (si->port_speed_max[block_port] <= 13000) {
                egr_fifo_depth = 44;
            } else {
                LOG_ERROR(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit, "Invalid Speed config\n")));
                return SOC_E_FAIL;
            }

            soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                    EGRESS_FIFO_START_ADDRESSf,
                    block_max_nxtaddr);

            if ((block_max_nxtaddr + egr_fifo_depth) > block_max_endaddr[block]) {
                egr_fifo_depth = (block_max_endaddr[block] -
                        block_max_nxtaddr)+1;
                block_max_nxtaddr = block_max_endaddr[block];
            } else {
                block_max_nxtaddr += egr_fifo_depth;
            }
            if (egr_fifo_depth == 1) {
                LOG_CLI((BSL_META_U(unit,
                                "WARN: EGR_FIFO_DEPTH IS ZERO for port=%d\n"),
                            block_port));
            }
            soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                    EGRESS_FIFO_DEPTHf, egr_fifo_depth);
            BCM_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFGr(
                        unit,block_port,rval));
        } /* if SOC_PBMP_MEMBER  ... block_port */
    } /* for loop */

    /* WORK-AROUND for port-flushing i.e. need to repeat below step twice */
    for (try_loop = 0; try_loop < 2; try_loop++) { 
        /* EP Reset */
        if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
            BCM_IF_ERROR_RETURN(WRITE_DYNAMIC_PORT_RECFG_VECTOR_CFG_29_00r(
                        unit, 0));
        }

        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            block_port=(*sb2_block_ports[unit])[block][loop];
            if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
                rval1 |= (1 << block_port);
            }
        }

        BCM_IF_ERROR_RETURN(WRITE_DYNAMIC_PORT_RECFG_VECTOR_CFG_29_00r(
                    unit, rval1));

        if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
            BCM_IF_ERROR_RETURN(WRITE_DYNAMIC_PORT_RECFG_VECTOR_CFG_29_00r(
                        unit, 0));
        }

        try_count = 0;
        do {
            soc_timeout_init(&to, timeout_usec, min_polls);
            if(soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                                    "%s:%d:TimeOut InternalError\n"),
                         FUNCTION_NAME(),__LINE__));
                return BCM_E_INTERNAL;
            }
            try_count++;

            BCM_IF_ERROR_RETURN(READ_DYNAMIC_PORT_RECFG_VECTOR_CFG_29_00r(
                        unit, &rval1));
            if (rval1 == 0 ) {
                break;
            }
        } while (try_count != 3);

        if (try_count == 3) {
            return BCM_E_TIMEOUT;
        }

    } /* for try_loop */

    if ((block > 0) && (block < 6)) {
        READ_EDATABUF_DBG_PORT_INTF_RESETr(unit, &rval);
        soc_reg_field_set(unit, EDATABUF_DBG_PORT_INTF_RESETr,
                &rval,  port_intf_reset_fld[block - 1], 1);
        WRITE_EDATABUF_DBG_PORT_INTF_RESETr(unit, rval);
        READ_EDATABUF_DBG_MMU_INTF_RESETr(unit, &rval);
        soc_reg_field_set(unit, EDATABUF_DBG_MMU_INTF_RESETr,
                &rval,  mmu_intf_reset_fld[block - 1], 1);
        soc_reg_field_set(unit, EDATABUF_DBG_MMU_INTF_RESETr,
                &rval,  new_port_mode_fld[block - 1], 
                new_port_mode[new_lanes]);
        WRITE_EDATABUF_DBG_MMU_INTF_RESETr(unit, rval);
    } else if (block == 6) {
        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            READ_EDATABUF_DBG_PORT_INTF_RESETr(unit, &rval);
            soc_reg_field_set(unit, EDATABUF_DBG_PORT_INTF_RESETr,
                    &rval,  port_intf_reset_fld[block + loop - 1], 1);
            WRITE_EDATABUF_DBG_PORT_INTF_RESETr(unit, rval);
            READ_EDATABUF_DBG_MMU_INTF_RESETr(unit, &rval);
            soc_reg_field_set(unit, EDATABUF_DBG_MMU_INTF_RESETr,
                    &rval,  mmu_intf_reset_fld[block + loop - 1], 1);
            soc_reg_field_set(unit, EDATABUF_DBG_MMU_INTF_RESETr,
                    &rval,  new_port_mode_fld[block + loop - 1],
                    new_port_mode[new_lanes]);
            WRITE_EDATABUF_DBG_MMU_INTF_RESETr(unit, rval);
        }
    } else if (block == 0){
        WRITE_TXLP_PORT_CREDIT_RESETr(unit, port,0xf);
        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            sal_memset(&txlp_port_addr_map_table_entry,0,
                    sizeof(txlp_port_addr_map_table_entry_t));
            block_port=(*sb2_block_ports[unit])[block][loop];
            if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
                
                if (si->port_speed_max[port] <= 2500) {
                    end_addr = start_addr + (( 8 * 4) - 1); /* 8 cells */
                } else if(si->port_speed_max[port] <= 10000) {
                    end_addr = start_addr + (( 16 * 4) - 1);
                } else  {
                    return SOC_E_PARAM;
                }

                soc_TXLP_PORT_ADDR_MAP_TABLEm_field_set(unit,
                        &txlp_port_addr_map_table_entry,START_ADDRf,&start_addr);
                soc_TXLP_PORT_ADDR_MAP_TABLEm_field_set(unit,
                        &txlp_port_addr_map_table_entry,END_ADDRf,&end_addr);
                start_addr = end_addr+1;
            }

            BCM_IF_ERROR_RETURN(WRITE_TXLP_PORT_ADDR_MAP_TABLEm(
                        unit,SOC_INFO(unit).txlp_block[block],
                        loop, &txlp_port_addr_map_table_entry));
        } /* for loop */
    } /* if else block != 0 */


    /* Begin: Link up sequence */
    sal_udelay((10*time_multiplier));
    
    /* 1. Bring edatabuf port and mmu intf out of reset */
    if ((block > 0) && (block < 6)) {
        READ_EDATABUF_DBG_PORT_INTF_RESETr(unit, &rval);
        soc_reg_field_set(unit, EDATABUF_DBG_PORT_INTF_RESETr,
                &rval,  port_intf_reset_fld[block - 1], 0);
        WRITE_EDATABUF_DBG_PORT_INTF_RESETr(unit, rval);
        READ_EDATABUF_DBG_MMU_INTF_RESETr(unit, &rval);
        soc_reg_field_set(unit, EDATABUF_DBG_MMU_INTF_RESETr,
                &rval,  mmu_intf_reset_fld[block - 1], 0);
        WRITE_EDATABUF_DBG_MMU_INTF_RESETr(unit, rval);
    } else if(block == 6) {
        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            READ_EDATABUF_DBG_PORT_INTF_RESETr(unit, &rval);
            soc_reg_field_set(unit, EDATABUF_DBG_PORT_INTF_RESETr,
                    &rval,  port_intf_reset_fld[block + loop - 1], 0);
            WRITE_EDATABUF_DBG_PORT_INTF_RESETr(unit, rval);
            READ_EDATABUF_DBG_MMU_INTF_RESETr(unit, &rval);
            soc_reg_field_set(unit, EDATABUF_DBG_MMU_INTF_RESETr,
                    &rval,  mmu_intf_reset_fld[block + loop - 1], 0);
            WRITE_EDATABUF_DBG_MMU_INTF_RESETr(unit, rval);
        }
    }

    if (block == 0) {
        WRITE_TXLP_PORT_CREDIT_RESETr(unit, port,0);
    }

    /* 2. Enable port */
    BCM_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(
                unit, MEM_BLOCK_ANY, 0, &epc_link_bmap_entry));
    soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm,
            &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            BCM_PBMP_PORT_ADD(link_pbmp,block_port);
        }
    } 
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm,
            &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(
                unit, MEM_BLOCK_ALL, 0, &epc_link_bmap_entry));
    
    sal_udelay((10*time_multiplier));

    /* Bring out of reset */
    if (block < 6) {
        soc_saber2_block_reset(unit, block, 1);
    } else {
        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            block_port=(*sb2_block_ports[unit])[block][loop];
            if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
                if (SOC_REG_PORT_VALID(unit, IECELL_CONFIGr, block_port)) {
                    SOC_IF_ERROR_RETURN(READ_IECELL_CONFIGr(unit, block_port, &rval));
                    soc_reg_field_set(unit, IECELL_CONFIGr, &rval, SOFT_RESETf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_IECELL_CONFIGr(unit, block_port, rval));
                }
            }
        }
    }

    if (block == 6) {
        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            block_port=(*sb2_block_ports[unit])[block][loop];
            if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
                if(SOC_REG_PORT_VALID(unit, XLPORT_MAC_CONTROLr, block_port)) {
                    SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, block_port, &rval));
                    soc_reg_field_set(unit, XLPORT_MAC_CONTROLr,  &rval, XMAC0_RESETf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, block_port, rval));
                }
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0));
    }
    
    if (SOC_REG_PORT_VALID(unit, XLPORT_XGXS0_CTRL_REGr, port)) {
        SOC_IF_ERROR_RETURN(READ_XLPORT_XGXS0_CTRL_REGr(unit, port, &rval));

        soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, PWRDWNf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_XGXS0_CTRL_REGr(unit, port, rval));
        
        soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, IDDQf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_XGXS0_CTRL_REGr(unit, port, rval));

        soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, RSTB_HWf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_XGXS0_CTRL_REGr(unit, port, rval));

    } else if (SOC_REG_PORT_VALID(unit, XPORT_XGXS_CTRLr, port)) {
        SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit, port, &rval));
        
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, PWRDWNf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));

        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, IDDQf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
        
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_HWf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
  
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_MDIOREGSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));
        
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_PLLf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));

        for (loop=0;loop<SB2_MAX_PORTS_PER_BLOCK;loop++) {
             block_port=(*sb2_block_ports[unit])[block][loop];
             if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
                 SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(
                                     unit, block_port, &rval));
                 if (si->port_speed_max[block_port] > 2500) {
                     soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval,
                                       TXD10G_FIFO_RSTBf, 1);
                 } else {
                     soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval,
                                       TXD10G_FIFO_RSTBf,0);
                 }
                 soc_reg_field_set(unit,XPORT_XGXS_CTRLr, &rval,
                                   TXD1G_FIFO_RSTBf, 0xF);
                 SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(
                                     unit,block_port, rval));
             }
        }
    }

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            BCM_IF_ERROR_RETURN(_bcm_sb2_update_port_mode(
                        unit,block_port,
                        si->port_speed_max[block_port]));
            break;
        }
    }

    if (block == 6) {
        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            block_port=(*sb2_block_ports[unit])[block][loop];
            if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
                if(SOC_REG_PORT_VALID(unit, XLPORT_MAC_CONTROLr, block_port)) {
                    SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, block_port, &rval));
                    soc_reg_field_set(unit, XLPORT_MAC_CONTROLr,  &rval, XMAC0_RESETf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, block_port, rval));
                }
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0));
    }

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            if(SOC_REG_PORT_VALID(unit, XPORT_XMAC_CONTROLr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XPORT_XMAC_CONTROLr(unit, block_port, &rval));
                soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf, 1);
                SOC_IF_ERROR_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, block_port, rval));
            }
        }
    }
    sal_udelay(10);

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            if(SOC_REG_PORT_VALID(unit, XPORT_XMAC_CONTROLr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XPORT_XMAC_CONTROLr(unit, block_port, &rval));
                soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf, 0);
                SOC_IF_ERROR_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, block_port, rval));
            }
        }
    }

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            if(SOC_REG_PORT_VALID(unit, MAC_RSV_MASKr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_MAC_RSV_MASKr(unit,block_port,
                            &rval));
                soc_reg_field_set(unit, MAC_RSV_MASKr, &rval, MASKf, 0x78);
                SOC_IF_ERROR_RETURN(WRITE_MAC_RSV_MASKr(unit,block_port,
                            rval));
            } /* SOC_REG_PORT_VALID MAC_RSV_MASK */

            if(SOC_REG_PORT_VALID(unit, XLPORT_MAC_RSV_MASKr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_RSV_MASKr(unit,
                            block_port, &rval));
                if (IS_ST_PORT(unit, block_port)) {
                    soc_reg_field_set(unit, XLPORT_MAC_RSV_MASKr,
                            &rval, MASKf, 0x58);
                } else {
                    soc_reg_field_set(unit, XLPORT_MAC_RSV_MASKr,
                            &rval, MASKf, 0x78);
                }
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_RSV_MASKr(unit,
                            block_port, rval));
            } /* SOC_REG_PORT_VALID MAC_RSV_MASK */
        } /* if SOC_PBMP_MEMBER */
    } /* for loop */

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {

            if (SOC_REG_PORT_VALID(unit, XPORT_MIB_RESETr, block_port)) {
                SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit, block_port, 0xf));
                sal_usleep(1000);
                SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit, block_port, 0x0));
            }

            if (SOC_REG_PORT_VALID(unit, XLPORT_MIB_RESETr, block_port)) {
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, block_port, 0xf));
                sal_usleep(1000);
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, block_port, 0x0));
            }
        }
    }

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            if (SOC_REG_PORT_VALID(unit, XLMAC_MODEr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XLMAC_MODEr(unit,block_port,
                            &rval64));
                if (si->port_speed_max[block_port] ==  1000) {
                    speed_value = SB2_SPEED_MODE_LINK_1G;
                } else if (si->port_speed_max[block_port] == 2500) {
                    speed_value = SB2_SPEED_MODE_LINK_2G5;
                } else {
                    speed_value = SB2_SPEED_MODE_LINK_10G_PLUS;
                }

                soc_reg64_field32_set(unit, XLMAC_MODEr, &rval64,
                        SPEED_MODEf,speed_value);
                if (IS_ST_PORT(unit, block_port)) {
                    soc_reg64_field32_set(unit, XLMAC_MODEr, &rval64,
                            HDR_MODEf, 2);
                } else {
                    soc_reg64_field32_set(unit, XLMAC_MODEr, &rval64,
                            HDR_MODEf, 0);
                }
                SOC_IF_ERROR_RETURN(WRITE_XLMAC_MODEr(unit,block_port,
                            rval64));
             } /* if SOC_REG_PORT_VALID XLMAC_MODE */
        } /* if SOC_PBMP_MEMBER block_port */
    } /* for loop */

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            if (SOC_REG_PORT_VALID(unit, XLMAC_RX_CTRLr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(
                            unit,block_port, &rval64));
                if (IS_ST_PORT(unit, block_port)) {
                    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval64,
                            RUNT_THRESHOLDf, 76);
                } else {
                    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval64,
                            RUNT_THRESHOLDf, 64);
                }
                soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval64,
                        STRIP_CRCf, 0);
                SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_CTRLr(
                            unit,block_port, rval64));
            } /* if SOC_REG_PORT_VALID XLMAC_RX_CTRL */
        } /* if SOC_PBMP_MEMBER block_port */
    } /* for loop */

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            if (SOC_REG_PORT_VALID(unit, XLMAC_TX_CTRLr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(
                        unit, block_port, &rval64));
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval64,
                        CRC_MODEf,3);
                SOC_IF_ERROR_RETURN(WRITE_XLMAC_TX_CTRLr(
                        unit,block_port, rval64));
            } /* if SOC_REG_PORT_VALID XLMAC_TX_CTRL */
        } /* if SOC_PBMP_MEMBER block_port */
    } /* for loop */

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            if (SOC_REG_PORT_VALID(unit, XLMAC_RX_MAX_SIZEr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XLMAC_RX_MAX_SIZEr(
                            unit,block_port, &rval64));
                soc_reg64_field32_set(unit, XLMAC_RX_MAX_SIZEr, &rval64,
                        RX_MAX_SIZEf,(12*1024));
                SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_MAX_SIZEr(
                            unit,block_port, rval64));
            } /* if SOC_REG_PORT_VALID XLMAC_RX_MAX_SIZE */
        } /* if SOC_PBMP_MEMBER block_port */
    } /* for loop */

    if (SOC_REG_PORT_VALID(unit, XLPORT_ECC_CONTROLr, port)) {
        SOC_IF_ERROR_RETURN(READ_XLPORT_ECC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_ECC_CONTROLr, &rval,
                MIB_TSC_MEM_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_ECC_CONTROLr(unit, port, rval));
    }

#if defined(SABER2_DEBUG)
    if (block == 0) {
        auto_portgroup = soc_property_get(unit, "auto_portgroup", 0);
        switch(auto_portgroup) {
            case 0:
                bsl_warn(BSL_BCM_PORT, unit,"auto_portgroup not set! \n");
                for (loop = 0;
                        loop < SB2_MAX_PORTS_PER_BLOCK;
                        loop += lane_incr[new_lanes-1] ) {
                    block_port=(*sb2_block_ports[unit])[block][loop];
                    portgroup = soc_property_port_get(unit, block_port, 
                            spn_PORTGROUP, 0);
                    if (portgroup == 0) {
                        bsl_warn(BSL_BCM_PORT, unit,
                                "WARNING:porgroup config variable not set \n"
                                "Please set portgroup config variable \n"
                                "Continuing but Correct behavior "
                                "no longer guaranteed \n");
                    } else {
                        bsl_warn(BSL_BCM_PORT, unit,
                                "Portgroup setting:%d  for port:%d \n",
                                portgroup, block_port);
                    } 
                }
                break;
            case 1:
            default: 
                bsl_warn(BSL_BCM_PORT, unit,
                        "auto_portgroup set so taking self decision! \n");
                for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
                    block_port=(*sb2_block_ports[unit])[block][loop];
                    sal_sprintf(config_str,"portgroup_%d",block_port);
                    soc_mem_config_save(unit, config_str, NULL);
                }
                switch(new_lanes) {
                    case 1:
                        sal_sprintf(config_str,"portgroup_%d",port);
                        soc_mem_config_save(unit, config_str, "4");
                        bsl_warn(BSL_BCM_PORT, unit,
                                "Generated Portgroup setting as %s=4\
                                for port:%d \n",
                                config_str, port);
                        break;
                    case 2:
                        for (loop=0;loop<SB2_MAX_PORTS_PER_BLOCK;loop+=2) {
                            block_port=(*sb2_block_ports[unit])[block][loop];
                            sal_sprintf(config_str,"portgroup_%d",block_port);
                            soc_mem_config_save(unit, config_str, "2");
                            bsl_warn(BSL_BCM_PORT, unit,
                                    "Generated Portgroup setting as %s=2\
                                    for port:%d\n",
                                    config_str, block_port);
                        }
                        break;
                    case 4:
                        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
                            block_port=(*sb2_block_ports[unit])[block][loop];
                            sal_sprintf(config_str,"portgroup_%d",block_port);
                            soc_mem_config_save(unit, config_str, "1");
                            bsl_warn(BSL_BCM_PORT, unit,
                                    "Generated Portgroup setting as %s=1\
                                    for port:%d\n",
                                    config_str, block_port);
                        }
                        break;
                } /* switch new_lanes */
                break;
        } /* switch auto_portgroup */ 
    } /* if block == 0 */
#endif

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if ((old_ports[old_lanes-1][loop] == 0) &&
                (new_ports[new_lanes-1][loop] == 0)) {
            continue;
        }
        if (old_ports[old_lanes-1][loop] == new_ports[new_lanes-1][loop] ) {
            /* same port was used so only speed change */
            SOC_IF_ERROR_RETURN(soc_saber2_port_detach(unit,block_port));
        }
        if (old_ports[old_lanes-1][loop]  > new_ports[new_lanes-1][loop]) {
            /* This port is not applicable so remove it from pbmp list */
            SOC_IF_ERROR_RETURN(soc_saber2_port_detach(unit,block_port));
            continue;
        }
        if (new_ports[new_lanes-1][loop]  > old_ports[old_lanes-1][loop]) {
            /* This port is new continue */
            continue;
        }
    }

    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
        
            if (SOC_REG_PORT_VALID(unit, XPORT_PORT_ENABLEr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XPORT_PORT_ENABLEr(unit, block_port,
                            &rval));
                soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &rval,
                        port_enable_field[loop], 1);
                SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(unit, block_port,
                            rval));
           }        
                            
           if (SOC_REG_PORT_VALID(unit, XLPORT_ENABLE_REGr, block_port)) {
                SOC_IF_ERROR_RETURN(READ_XLPORT_ENABLE_REGr(unit, block_port,
                            &rval));
                soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval,
                        port_enable_field[loop], 1);
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, block_port,
                            rval));
           }        

        }/* if SOC_PBMP_MEMBER block_port */
    } /* for loop */

    sal_udelay((10*time_multiplier));
    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            BCM_IF_ERROR_RETURN(soc_mac_xl.md_enable_set(
                        unit, block_port, TRUE));
        } /* if SOC_PBMP_MEMBER block_port */
    } /* for loop */

    if (SOC_REG_PORT_VALID(unit, THDO_PORT_DISABLE_CFG1r, port)) {
        for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
            block_port=(*sb2_block_ports[unit])[block][loop];
            SOC_IF_ERROR_RETURN(READ_THDO_PORT_DISABLE_CFG1r(unit,&rval));

            soc_reg_field_set(unit, THDO_PORT_DISABLE_CFG1r, &rval,
                    PORT_WRf,1);
            soc_reg_field_set(unit, THDO_PORT_DISABLE_CFG1r, &rval,
                    PORT_WR_TYPEf,0);
            soc_reg_field_set(unit, THDO_PORT_DISABLE_CFG1r, &rval,
                    PORT_IDf,block_port);
            SOC_IF_ERROR_RETURN(WRITE_THDO_PORT_DISABLE_CFG1r(unit,rval));

            sal_udelay(10*time_multiplier);

            if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
                soc_reg_field_set(unit, THDO_PORT_DISABLE_CFG1r, &rval,
                        PORT_WRf,0);
                SOC_IF_ERROR_RETURN(WRITE_THDO_PORT_DISABLE_CFG1r(
                            unit,rval));
            }
            SOC_IF_ERROR_RETURN(READ_THDO_PORT_DISABLE_CFG1r(unit,&rval));

            if (soc_reg_field_get(unit, THDO_PORT_DISABLE_CFG1r, rval,
                    PORT_WRf) != 0) {
            return BCM_E_TIMEOUT;
            }
        } /* for loop */
    } /* if SOC_REG_PORT_VALID THDO_PORT_DISABLE_CFG1 */

    sal_udelay((10*time_multiplier));
    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            if(block > 0) {
                sal_memset(&egr_enable_entry, 0, sizeof(egr_enable_entry_t));
                soc_mem_field32_set(unit, EGR_ENABLEm, &egr_enable_entry, PRT_ENABLEf, 1);
                SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, block_port,
                            &egr_enable_entry));
            } else {
                 SOC_IF_ERROR_RETURN(READ_TXLP_PORT_ENABLEr(
                                     unit, port, &rval));
                 port_enable = soc_reg_field_get(unit, TXLP_PORT_ENABLEr,
                                     rval, PORT_ENABLEf);
                 port_enable |= ( 1<< ((*sb2_port_to_block_subports[unit])
                                      [block_port-1]));
                 soc_reg_field_set(unit, TXLP_PORT_ENABLEr,
                                     &rval, PORT_ENABLEf, port_enable);
                 SOC_IF_ERROR_RETURN(WRITE_TXLP_PORT_ENABLEr(
                                     unit, port, rval));
            }
        }
    }

    /* Update port_num_lanes in soc_info_config */
    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port)) {
            si->port_num_lanes[block_port] = SB2_MAX_PORTS_PER_BLOCK - lanes + 1;
        } else {
            si->port_num_lanes[block_port] = 0;
        }
    }
 
    for (loop = 0; loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        block_port=(*sb2_block_ports[unit])[block][loop];
        if ((old_ports[old_lanes-1][loop] == 0) &&
                (new_ports[new_lanes-1][loop] == 0)) {
            continue;
        }
        if (old_ports[old_lanes-1][loop] == new_ports[new_lanes-1][loop] ) {
            /* same port was used so only speed change */
            SOC_IF_ERROR_RETURN(soc_saber2_port_attach(
                        unit,block ,block_port));
        }
        if (old_ports[old_lanes-1][loop]  > new_ports[new_lanes-1][loop]) {
            /* This port is not applicable so remove it from pbmp list */
            continue;
        }
        if (new_ports[new_lanes-1][loop]  > old_ports[old_lanes-1][loop]) {
            /* This port is new so add it in pbmp list first */
            SOC_IF_ERROR_RETURN(soc_saber2_port_attach(
                        unit,block ,block_port));
        }
    }

    bcm_esw_kt2_port_unlock(unit);
    BCM_IF_ERROR_RETURN(bcm_esw_linkscan_mode_set(unit, port, init_mode));
    bcm_esw_kt2_port_lock(unit);
    
    /* End: Link up sequence */
   
    return SOC_E_NONE;
}

int
_bcm_sb2_port_lanes_get(int unit, bcm_port_t port, int *lanes)
{
    uint8 loop=0, block=0;

    *lanes=0;
    SOC_IF_ERROR_RETURN(soc_saber2_get_port_block(
                unit, port, &block));

    for (loop = 0;loop < SB2_MAX_PORTS_PER_BLOCK; loop++) {
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)),
                    (*sb2_block_ports[unit])[block][loop])) {
            (*lanes)++;
        }
    }
    return SOC_E_NONE;
}

int
bcm_sb2_flexio_pbmp_update(int unit, bcm_pbmp_t *pbmp)
{
    bcm_port_t blk_port;
    int        blk, blk_idx;

    if (NULL == pbmp) {
        return BCM_E_PORT;
    }

    for (blk = 0; blk < SB2_MAX_BLOCKS; blk++) {

        if (blk < 6) {
            /* Add the port at index 1, 2, & 3 to gven pbmp and update the
             * port_type so that it is treated as valid port
             */
            for (blk_idx = 1; 
                    blk_idx < SB2_MAX_PORTS_PER_BLOCK; blk_idx++) {

                blk_port = (*sb2_block_ports[unit])[blk][blk_idx];
                if (blk_port < SOC_INFO(unit).lb_port) {
                    BCM_PBMP_PORT_ADD(*pbmp, blk_port);
                    SOC_PORT_TYPE(unit, blk_port) = SOC_BLK_MXQPORT;
                }

            } /* for blk_idx */
        } else if (blk == 6) {

            for (blk_idx = 1; 
                    blk_idx < SB2_MAX_PORTS_PER_BLOCK; blk_idx++) {

                blk_port = (*sb2_block_ports[unit])[blk][blk_idx];
                if (blk_port < SOC_INFO(unit).lb_port) {
                    BCM_PBMP_PORT_ADD(*pbmp, blk_port);
                    SOC_PORT_TYPE(unit, blk_port) = SOC_BLK_XLPORT;
                }

            } /* for blk_idx */
        } /* if SOC_PORT_TYPE */
    } /* For blk */
    return BCM_E_NONE;
} /* _bcm_sb2_flexio_pbmp_update */

#endif

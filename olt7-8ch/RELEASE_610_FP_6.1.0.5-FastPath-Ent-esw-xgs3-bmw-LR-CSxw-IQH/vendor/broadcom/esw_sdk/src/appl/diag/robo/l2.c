/*
 * $Id: l2.c,v 1.1 2011/04/18 17:11:00 mruas Exp $
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

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/l23x.h>

#include <soc/arl.h>
#include <soc/l2x.h>
#include <soc/l2u.h>
#include <soc/debug.h>
#include <soc/hash.h>

#include <bcm/error.h>
#include <bcm/l2.h>
#include <bcm/mcast.h>
#include <bcm/debug.h>
#include <bcm/stack.h>


const mac_addr_t _mac_all_zeroes =
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* CB function to print L2 Entry */
int _robo_l2addr_dump(int unit, bcm_l2_addr_t *l2addr, void *user_data)
{
    bcm_module_t        local_modid;
    char                pfmt[SOC_PBMP_FMT_LEN];
    char                bmstr[FORMAT_PBMP_MAX];

    if (bcm_stk_my_modid_get(unit, &local_modid) != BCM_E_NONE) {
        local_modid = -1;
    } 

    printk("mac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d",
           l2addr->mac[0], l2addr->mac[1], l2addr->mac[2],
           l2addr->mac[3], l2addr->mac[4], l2addr->mac[5], l2addr->vid);

    if (BCM_GPORT_IS_MPLS_PORT(l2addr->port)) {
        printk(" port=%d(mpls)", l2addr->port);
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(l2addr->port)) {
        printk(" port=%d(subport)", l2addr->port);
    } else {
        printk(" modid=%d port=%d%s%s", l2addr->modid, l2addr->port, 
              ((local_modid) == l2addr->modid) ? "/" : " ",
              ((local_modid == l2addr->modid) ? SOC_PORT_NAME(unit, l2addr->port) : " "));
    }

    if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
        printk(" Trunk=%d Rtag=%d", l2addr->tgid, l2addr->rtag);
    }

    if (l2addr->flags & BCM_L2_STATIC) {
        printk(" Static");
    }

    if (l2addr->flags & BCM_L2_HIT) {
        printk(" Hit");
    }

    if (l2addr->cos_src != 0 || l2addr->cos_dst != 0) {
#ifdef BCM_ROBO_SUPPORT        
        if(SOC_IS_ROBO(unit)) {
            if (SOC_IS_ROBO_GE_SWITCH(unit)) {
                printk(" Priority=%d", l2addr->cos_dst);
            } else {
                printk(" COS=%d", l2addr->cos_dst);
            }
        } else {
#endif /* BCM_ROBO_SUPPORT */        
            printk(" COS(src=%d,dst=%d)", l2addr->cos_src, l2addr->cos_dst);
#ifdef BCM_ROBO_SUPPORT
        }
#endif /* BCM_ROBO_SUPPORT */        
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        printk(" SCP");
    }

    if ((l2addr->flags & BCM_L2_COPY_TO_CPU) ||
        (l2addr->port == CMIC_PORT(unit))) {
        printk(" CPU");
    }

    if (l2addr->flags & BCM_L2_L3LOOKUP) {
        printk(" L3");
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        printk(" DiscardSrc");
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        printk(" DiscardDest");
    }

    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (l2addr->flags & BCM_L2_SETPRI) {
            printk(" ReplacePriority");
        }
    }

    if (SOC_IS_EASYRIDER(unit)) {
        if (l2addr->flags & BCM_L2_REMOTE_LOOKUP) {
            printk(" RemoteLookup");
        }
    }

    if (l2addr->flags & BCM_L2_MCAST) {
        printk(" MCast=%d", l2addr->l2mc_index);
    }

    if (SOC_PBMP_NOT_NULL(l2addr->block_bitmap)) {
        format_pbmp(unit, bmstr, sizeof (bmstr), l2addr->block_bitmap);
        printk(" MAC blocked port bitmap=%s: %s",
               SOC_PBMP_FMT(l2addr->block_bitmap, pfmt), bmstr);
    }

    if (l2addr->group) {
        printk(" Group=%d", l2addr->group);
    }

    printk("\n");

    return BCM_E_NONE;
}

static void
_robo_dump_l2_cache_addr(int unit, char *pfx, bcm_l2_cache_addr_t *l2caddr)
{
    printk("%sMport_mac=%02x:%02x:%02x:%02x:%02x:%02x",
           pfx,
           l2caddr->mac[0], l2caddr->mac[1], l2caddr->mac[2],
           l2caddr->mac[3], l2caddr->mac[4], l2caddr->mac[5]);

    printk(" Mport_vctr=0x%x", SOC_PBMP_WORD_GET(l2caddr->dest_ports, 0));

    printk("\n");
}

/*
 * AGE timer
 */

cmd_result_t
cmd_robo_age(int unit, args_t *a)
{
    int seconds;
    int r;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }


    if (!ARG_CNT(a)) {          /* Display settings */
        if ((r = bcm_l2_age_timer_get(unit, &seconds)) != BCM_E_NONE) {
            printk("%s ERROR: could not get age time: %s\n",
                   ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

        printk("Current age timer is %d.\n", seconds);

        return CMD_OK;
    }

    seconds = sal_ctoi(ARG_GET(a), 0);

    if ((r = bcm_l2_age_timer_set(unit, seconds)) != BCM_E_NONE) {
        printk("%s ERROR: could not set age time: %s\n",
               ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    printk("Set age timer to %d. %s\n", seconds, seconds ? "":"(disabled)");

    return CMD_OK;
}


cmd_result_t
if_robo_bpdu(int unit, args_t *a)
{
    int idx, count;
    char *subcmd = NULL;
    static int initted = 0;
    static mac_addr_t arg_macaddr, maczero, mac;
    static int		arg_index = 0;
    int rv;
    parse_table_t pt;
    char buf[SAL_MACADDR_STR_LEN];
    
    /*
     * Initialize MAC address field for the user to the first real
     * address which does not conflict
     */
    if (! initted) {
        maczero[0] = 0;
        maczero[1] = 0;
        maczero[2] = 0;
        maczero[3] = 0;
        maczero[4] = 0;
        maczero[5] = 0;
        arg_macaddr[0] = 0x01;
        arg_macaddr[1] = 0x80;
        arg_macaddr[2] = 0xc2;
        arg_macaddr[3] = 0x00;
        arg_macaddr[4] = 0x00;
        arg_macaddr[5] = 0x00;
        initted = 1;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    /* Check valid device to operation on ...*/
    if (! sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!sal_strcasecmp(subcmd, "add") ||
	!sal_strcasecmp(subcmd, "+")) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "Index", 	PQ_DFL|PQ_INT,
			(void *)(0), &arg_index, NULL);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
		    0, &arg_macaddr,NULL);

	if (!parseEndOk(a, &pt, &rv)) {
	    return rv;
	}
       count = bcm_l2_bpdu_count(unit);
       if (arg_index > (count - 1) ) {       
	    printk("%s ERROR: support %d BUPUs\n", ARG_CMD(a),count);
	    return CMD_FAIL;
       } else {
        	rv = bcm_l2_bpdu_set(unit, arg_index, arg_macaddr);
       }
	if (rv != BCM_E_NONE) {
	    printk("1. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }

    if (!sal_strcasecmp(subcmd, "del") ||
	!sal_strcasecmp(subcmd, "-")) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "Index", 	PQ_DFL|PQ_INT,
			(void *)(0), &arg_index, NULL);

	if (!parseEndOk(a, &pt, &rv)) {
	    return rv;
	}
       count = bcm_l2_bpdu_count(unit);
       if (arg_index > (count - 1) ) {       
	    printk("%s ERROR: support %d BUPUs\n", ARG_CMD(a),count);
	    return CMD_FAIL;
       } else {
          rv = bcm_l2_bpdu_set(unit, arg_index, maczero);
       }		
	if (rv != BCM_E_NONE) {
	    printk("2. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }
    if (! sal_strcasecmp(subcmd, "show") || ! sal_strcasecmp(subcmd, "-d")) {
        count = bcm_l2_bpdu_count(unit);
        printk("unit %d has %d BPDU entries\n", unit, count);
        for (idx = 0; idx < count; idx++) {
            rv = bcm_l2_bpdu_get(unit, idx, &mac);
            if (rv != BCM_E_NONE) {
                printk("3. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            format_macaddr(buf, mac);
            printk("\tBPDU %d: %s\n", idx, buf);
        }
        return CMD_OK ;
    }
    return CMD_USAGE;
}

/* add below extern for bcm_l2_addr_init() is disattchable*/
extern void 
bcm_l2_addr_init(bcm_l2_addr_t *l2addr, const bcm_mac_t mac_addr, bcm_vlan_t vid);
extern void 
_bcm_robo_l2_from_arl(int unit, bcm_l2_addr_t *l2addr, l2_arl_sw_entry_t *arl_entry);

/*
 * Used with the 'l2 watch' command below.
 */
static void
_l2_watch_cb(int unit,
	     bcm_l2_addr_t *l2addr,
	     int insert,
	     void *userdata)
{
    int s;
    sal_thread_t main_thread;

    s = sal_splhi();

    main_thread = sal_thread_main_get();
    sal_thread_main_set(sal_thread_self());

    if(insert) {
	dump_l2_addr(unit, "L2 ADD: ", l2addr);
    }
    else {
	dump_l2_addr(unit, "L2 DEL: ", l2addr);
    }

    sal_thread_main_set(main_thread);

    sal_spl(s);
}


cmd_result_t
if_robo_l2(int unit, args_t *a)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int idx;
    char *subcmd = NULL;
    static pbmp_t arg_pbmp;
    pbmp_t pbmp;
    static int arg_static = 0, arg_trunk = 0, arg_l3if = 0,
	arg_scp = 0, arg_ds = 0, arg_dd = 0, arg_count = 1, arg_modid = 0,
	arg_vlan = VLAN_ID_DEFAULT, arg_tgid = 0, arg_cbit = 0,
	arg_port, arg_cos = -1, arg_replace = 0;
    int arg_newmodid = 0, arg_newport = 0, arg_newtgid=0,arg_del=0;
    soc_port_t p;
    int rv = CMD_OK;
    parse_table_t pt;
    cmd_result_t ret_code;
    bcm_l2_addr_t l2addr;
    uint32  flags, port, fld_value;
    uint64  temp_mac_field;
    uint8   temp_mac_addr[6];
    l2_arl_sw_entry_t l2_sw_entry, output, output1;
    
    /*
     * Initialize MAC address field for the user to the first real
     * address which does not conflict
     */
    mac_addr_t   default_macaddr = {0, 0, 0, 0, 0, 0X1};
    mac_addr_t   arg_macaddr = {0, 0, 0, 0, 0, 0X1};

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    /* Check valid device to operation on ...*/
    if (! sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!sal_strcasecmp(subcmd, "replace")) {
        uint32 flags = 0;

        arg_modid = arg_port = arg_vlan = arg_tgid = arg_newport = arg_newtgid = -1;
        arg_newmodid = arg_static = arg_del =0;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Module",          PQ_DFL|PQ_INT,
                0, &arg_modid,  NULL);
        parse_table_add(&pt, "Port",            PQ_DFL|PQ_PORT,
                0, &arg_port,   NULL);
        parse_table_add(&pt, "MACaddress", 	    PQ_DFL|PQ_MAC,
                0, &arg_macaddr, NULL);
        parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
                0, &arg_vlan,	NULL);
        parse_table_add(&pt, "Trunk",           PQ_DFL|PQ_BOOL,
                0, &arg_trunk,	NULL);
        parse_table_add(&pt, "TrunkGroupID",    PQ_DFL|PQ_INT,
                0, &arg_tgid,	NULL);
        parse_table_add(&pt, "STatic", 	        PQ_DFL|PQ_BOOL,
                0, &arg_static,	NULL);
        parse_table_add(&pt, "Delete", 	        PQ_DFL|PQ_BOOL,
                0, &arg_del,	NULL);
        parse_table_add(&pt, "NewModule",       PQ_DFL|PQ_INT,
                0, &arg_newmodid,  NULL);
        parse_table_add(&pt, "NewPort",         PQ_DFL|PQ_PORT,
                0, &arg_newport,   NULL);
        parse_table_add(&pt, "NewTrunkGroupID", PQ_DFL|PQ_INT,
                0, &arg_newtgid,	NULL);
        if (!parseEndOk(a, &pt, &ret_code)) {
            return ret_code;
        }

        bcm_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);
        if (arg_static) {
            flags |= BCM_L2_REPLACE_MATCH_STATIC;
        }
        if (arg_del) {
            flags |= BCM_L2_REPLACE_DELETE;
        }
        if (ENET_CMP_MACADDR(arg_macaddr, default_macaddr)) {
            flags |= BCM_L2_REPLACE_MATCH_MAC;
        }
        if (arg_vlan != -1) {
            flags |= BCM_L2_REPLACE_MATCH_VLAN;
        }
        if (arg_newtgid != - 1) {
            flags |= BCM_L2_REPLACE_NEW_TRUNK; 
        }
        if ((arg_port != - 1) && (arg_modid != - 1)) {
            flags |= BCM_L2_REPLACE_MATCH_DEST;
            l2addr.modid = arg_modid;
            l2addr.port = arg_port;
        }
        if (arg_trunk && (arg_tgid != -1)) {
            l2addr.tgid = arg_tgid;
        }

        rv = bcm_l2_replace(unit, flags, &l2addr, arg_newmodid, 
                                arg_newport, arg_newtgid); 

        if (BCM_FAILURE(rv)) {
            printk("4. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        return CMD_OK;
    } else if (! sal_strcasecmp(subcmd, "add") || ! sal_strcasecmp(subcmd, "+")) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP,
                (void *)(0), &arg_pbmp, NULL);
        parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC,
                0, &arg_macaddr, NULL);
        parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX,
                0, &arg_vlan, NULL);
        parse_table_add(&pt, "PRIority", PQ_DFL|PQ_HEX,
                0, &arg_cos, NULL);
        parse_table_add(&pt, "STatic", PQ_DFL|PQ_BOOL,
                0, &arg_static, NULL);
        parse_table_add(&pt, "Replace",         PQ_DFL|PQ_BOOL,
                0, &arg_replace,NULL);

        if (!parseEndOk( a, &pt, &ret_code)) {
            return ret_code;
        }
        pbmp = arg_pbmp;

        if (BCM_PBMP_IS_NULL(pbmp)) {
            printk("%s ERROR: empty port bitmap\n", ARG_CMD(a));
            return CMD_FAIL;
        }

        /*
         * If we are setting the range, the MAC address is incremented by
         * 1 for each port.
         */
        PBMP_ITER(pbmp, p) {
            bcm_l2_addr_init(&l2addr, arg_macaddr, arg_vlan);

            l2addr.tgid = arg_tgid;
            
            if (SOC_IS_ROBO_GE_SWITCH(unit)) {
                if (arg_cos < 0) {
                    arg_cos = 0;
                }
            } else {
                if ((arg_cos < 0) || (arg_cos > 3)) {
                      arg_cos = 0; /* If out of range, set to 0 */
                }
            }
            l2addr.cos_dst = arg_cos;
            /* Configure flags for SDK call */
            if (arg_static)
                l2addr.flags |= BCM_L2_STATIC;
            if (arg_scp)
                l2addr.flags |= BCM_L2_COS_SRC_PRI;
            if (arg_dd)
                l2addr.flags |= BCM_L2_DISCARD_DST;
            if (arg_ds)
                l2addr.flags |= BCM_L2_DISCARD_SRC;
            if (arg_l3if)
                l2addr.flags |= BCM_L2_L3LOOKUP;
            if (arg_trunk)
                l2addr.flags |= BCM_L2_TRUNK_MEMBER;
            if (arg_cbit)
                l2addr.flags |= BCM_L2_COPY_TO_CPU;
            if (arg_replace)
                l2addr.flags |= BCM_L2_REPLACE_DYNAMIC;

            l2addr.port = p;

            dump_l2_addr(unit, "ADD: ", &l2addr);

            if ((rv = bcm_l2_addr_add(unit, &l2addr))!= BCM_E_NONE) {
                printk("5. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            /* Set up for next call */
            increment_macaddr(arg_macaddr, 1);
        }

        return CMD_OK;
    } else if (! sal_strcasecmp(subcmd, "del") 
            || ! sal_strcasecmp(subcmd, "-")) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
                0, &arg_macaddr,NULL);
        parse_table_add(&pt, "Count", 	PQ_DFL|PQ_INT,
                (void *)(1), &arg_count, NULL);
        parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
                0, &arg_vlan,	NULL);
        if (!parseEndOk( a, &pt, &ret_code)) {
            return ret_code;
        }

        for (idx = 0; idx < arg_count; idx++) {
            rv = bcm_l2_addr_get(unit, arg_macaddr, arg_vlan, &l2addr);

            if (rv < 0) {
                printk("6. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            dump_l2_addr(unit, "DEL: ", &l2addr);

            if ((rv = bcm_l2_addr_delete(unit, arg_macaddr, arg_vlan)) < 0) {
                printk("7. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            increment_macaddr(arg_macaddr, 1);
        }

        return CMD_OK;
    } else if (! sal_strcasecmp(subcmd, "show") 
            || ! sal_strcasecmp(subcmd, "-d")) {
        /*
         * If no port specified, show all ports.
         */

        if (ARG_CNT(a) == 0) {
            pbmp = PBMP_ALL(unit);
        } else {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP,
                    (void *)(0), &arg_pbmp, NULL);
        
            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }
        
            SOC_PBMP_ASSIGN(pbmp, arg_pbmp);
            SOC_PBMP_AND(pbmp, PBMP_ALL(unit));
        }
        if(soc->arl_table){
  
            rv = bcm_l2_traverse(unit, _robo_l2addr_dump, NULL);

            if (rv < 0) {
                printk("%s: ERROR: bcm_l2_traverse failed %s\n",
                       ARG_CMD(a), bcm_errmsg(rv));
            }
            
        }else {
        sal_memset(&l2_sw_entry, 0, sizeof(l2_arl_sw_entry_t));
        PBMP_ITER(pbmp, port) {
            /* step 1: start the search of valid ARL entries */
            flags = DRV_MEM_OP_SEARCH_VALID_START;
            (DRV_SERVICES(unit)->mem_search)(unit, DRV_MEM_ARL, 
                NULL, NULL, NULL, flags);

            /* step 2: get the valid ARL entries if found */
            /* write the port number into arl entry */
            (DRV_SERVICES(unit)->mem_field_set)
                 (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT, 
                 (uint32 *)&l2_sw_entry, &port);
            flags = DRV_MEM_OP_SEARCH_VALID_GET | DRV_MEM_OP_SEARCH_PORT;
	    rv = SOC_E_EXISTS;
	    while (rv == SOC_E_EXISTS) {
            	sal_memset(&output, 0, sizeof(l2_arl_sw_entry_t));
            	sal_memset(&output1, 0, sizeof(l2_arl_sw_entry_t));
            	rv  = (DRV_SERVICES(unit)->mem_search)(unit, DRV_MEM_ARL, 
                    (uint32 *)&l2_sw_entry, (uint32 *)&output, (uint32 *)&output1, flags);
            	if (rv == SOC_E_EXISTS) {
            	    /* Entry 0 */
            	    (DRV_SERVICES(unit)->mem_field_get)
            	        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, 
            	        (uint32 *)&output, &fld_value);
            	    if (fld_value){
            	        _bcm_robo_l2_from_arl(unit, &l2addr, &output);
            	        if (!SOC_PBMP_MEMBER(pbmp, l2addr.port)){
            	            continue;
            	        }
            	        dump_l2_addr(unit, "", &l2addr);
            	    }
            	    /* Entry 1 */
            	    (DRV_SERVICES(unit)->mem_field_get)
            	        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, 
            	        (uint32 *)&output1, &fld_value);
            	    if (fld_value){
            	        _bcm_robo_l2_from_arl(unit, &l2addr, &output1);
            	        if (!SOC_PBMP_MEMBER(pbmp, l2addr.port)){
            	            continue;
            	        }
            	        dump_l2_addr(unit, "", &l2addr);
            	    }
            	}
            	
            }
        }

        }
        soc_cm_print("\n");
        return CMD_OK;
    } else if (! sal_strcasecmp(subcmd, "clear") 
            || ! sal_strcasecmp(subcmd, "clr")) {

        char *static_str;
        int old_modid, old_port, old_vlan, old_tgid, old_static;
        enum {
            MAC_PRESENT         = 0x00000001,
            MODID_PRESENT       = 0x00000002,
            PORT_PRESENT        = 0x00000004,
            VLANID_PRESENT      = 0x00000008,
            TGID_PRESENT        = 0x00000010
        } arg_combination;
        
        /*
	 * Clear the ARL on a per port or per switch basis. Walk ARL
	 * memory finding matching ports such as the user specified,
	 * issue BCM L2 SDK calls for clearing the entries.
	 */

        /* Save current arguments */
        old_modid = arg_modid;
        old_port = arg_port;
	 old_vlan = arg_vlan;
	 old_tgid = arg_tgid;
	 old_static = arg_static;

	 arg_modid = -1;
	 arg_port = -1;
	 arg_vlan = -1;
	 arg_tgid = -1;
	 arg_static = TRUE;
        ENET_SET_MACADDR(arg_macaddr, _mac_all_zeroes);
        arg_combination = 0;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Module",          PQ_DFL|PQ_INT,
			0, &arg_modid,  NULL);
        parse_table_add(&pt, "Port",            PQ_DFL|PQ_PORT,
			0, &arg_port,   NULL);
        parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &arg_vlan,	NULL);
        parse_table_add(&pt, "TrunkGroupID", 	PQ_DFL|PQ_INT,
			0, &arg_tgid,	NULL);
        parse_table_add(&pt, "MACaddress",      PQ_DFL|PQ_MAC,
                        0, &arg_macaddr,NULL);
        parse_table_add(&pt, "STatic", 	        PQ_DFL|PQ_BOOL,
			0, &arg_static,	NULL);

        ret_code = CMD_OK;

        if (!ARG_CNT(a)) {
            /*
             * Restore arguments for parseEndOk below to print
             * correct settings.
             */
            if (arg_modid == -1) arg_modid = old_modid;
            if (arg_port == -1) arg_port = old_port;
            if (arg_vlan == -1) arg_vlan = old_vlan;
            if (arg_tgid == -1) arg_tgid = old_tgid;
            if (arg_static == -1) arg_static = old_static;
        }

	if (!parseEndOk(a, &pt, &ret_code)) {
	    goto done;
        }

        /*
         * Notice which arguments were supplied
         */
        if (arg_modid >=0) {
            arg_combination |= MODID_PRESENT;
        }

        if (arg_port >= 0) {
            arg_combination |= PORT_PRESENT;
        }

        if (ENET_CMP_MACADDR(arg_macaddr, _mac_all_zeroes)) {
            arg_combination |= MAC_PRESENT;
        }

        if (arg_tgid >= 0) {
            arg_combination |= TGID_PRESENT;
        }

        if (arg_vlan >= 0) {
            arg_combination |= VLANID_PRESENT;
        }

        static_str = arg_static ? "static and non-static" : "non-static";
        arg_static = arg_static ? BCM_L2_DELETE_STATIC : 0;
        switch (arg_combination) {
        case PORT_PRESENT:
	    printk("%s: Deleting %s addresses by port, local module ID\n",
		   ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_port(unit,
					    -1, arg_port,
					    arg_static);
            break;

        case PORT_PRESENT | MODID_PRESENT:
	    printk("%s: Deleting %s addresses by module/port\n",
		   ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_port(unit,
					    arg_modid, arg_port,
					    arg_static);
            break;

        case VLANID_PRESENT:
	    printk("%s: Deleting %s addresses by VLAN\n",
		   ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_vlan(unit,
					    arg_vlan,
					    arg_static);
            break;

        case TGID_PRESENT:
	    printk("%s: Deleting %s addresses by trunk ID\n",
		   ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_trunk(unit,
					     arg_tgid,
					     arg_static);

	    break;
        
        case MAC_PRESENT:
            printk("%s: Deleting %s addresses by MAC\n",
                   ARG_CMD(a), static_str);
            rv = bcm_l2_addr_delete_by_mac(unit, 
                                           arg_macaddr, 
                                           arg_static);
            break;

        case MAC_PRESENT | VLANID_PRESENT:
            printk("%s: Deleting an address by MAC and VLAN\n",
                   ARG_CMD(a));
            rv = bcm_l2_addr_delete(unit, 
                                    arg_macaddr, 
                                    arg_vlan);

            break;

        case MAC_PRESENT | PORT_PRESENT:
            printk("%s: Deleting %s addresses by MAC and port\n",
                   ARG_CMD(a), static_str);
            rv = bcm_l2_addr_delete_by_mac_port(unit, 
                                                arg_macaddr, 
                                                -1, arg_port,
                                                arg_static);
            break;

        case MAC_PRESENT | PORT_PRESENT | MODID_PRESENT:
            printk("%s: Deleting %s addresses by MAC and module/port\n",
                   ARG_CMD(a), static_str);
            rv = bcm_l2_addr_delete_by_mac_port(unit, 
                                                arg_macaddr, 
                                                arg_modid, arg_port,
                                                arg_static);
            break;

        case VLANID_PRESENT | PORT_PRESENT:
	    printk("%s: Deleting %s addresses by VLAN and port\n",
		   ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_vlan_port(unit,
						 arg_vlan,
						 -1, arg_port,
						 arg_static);
            break;

        case VLANID_PRESENT | PORT_PRESENT | MODID_PRESENT:
	    printk("%s: Deleting %s addresses by VLAN and module/port\n",
		   ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_vlan_port(unit,
						 arg_vlan,
						 arg_modid, arg_port,
						 arg_static);
            break;

        case VLANID_PRESENT |TGID_PRESENT:
	       printk("%s: Deleting %s addresses by trunk ID\n",
		   ARG_CMD(a), static_str);
	        rv = bcm_l2_addr_delete_by_vlan_trunk(unit,
                                        arg_vlan,
					     arg_tgid,
					     arg_static);

	    break;
        default:
	    printk("%s: Unknown argument combination\n", ARG_CMD(a));
	    ret_code = CMD_USAGE;
            break;
	}

done:
        /* Restore unused arguments */
        if (arg_modid == -1) arg_modid = old_modid;
	if (arg_port == -1) arg_port = old_port;
	if (arg_vlan == -1) arg_vlan = old_vlan;
	if (arg_tgid == -1) arg_tgid = old_tgid;
	if (arg_static == -1) arg_static = old_static;

        if ((ret_code == CMD_OK) && (rv < 0)) {
            printk("ERROR: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
        }

	return ret_code;
    } else if (! sal_strcasecmp(subcmd, "dump")) {
        if(soc->arl_table){
            int index_min, index_max, index_count;
            uint32 idx,valid;
            uint32 count=0;

            index_min = SOC_MEM_BASE(unit, L2_ARLm);
            index_max = SOC_MEM_BASE(unit, L2_ARLm) + SOC_MEM_SIZE(unit, L2_ARLm);
            index_count = SOC_MEM_SIZE(unit, L2_ARLm);
            for (idx = index_min; idx < index_count; idx++) {
                sal_memset(&output, 0, sizeof(l2_arl_sw_entry_t));
                if(!ARL_ENTRY_NULL(&soc->arl_table[idx])) {
                    ARL_SW_TABLE_LOCK(soc);
                    sal_memcpy(&output, &soc->arl_table[idx], sizeof(l2_arl_sw_entry_t));
                    ARL_SW_TABLE_UNLOCK(soc);
                } else{
                    continue;
                }
                (DRV_SERVICES(unit)->mem_field_get)
             		(unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, 
            		(uint32 *)&output, &valid);
                if (valid){
                    count++;
            	      (DRV_SERVICES(unit)->mem_field_get)
            		    (unit, DRV_MEM_ARL, DRV_MEM_FIELD_MAC, 
                		(uint32 *)&output, (uint32 *)&temp_mac_field);
                	    SAL_MAC_ADDR_FROM_UINT64(temp_mac_addr, temp_mac_field);
                	    soc_cm_print("\nMac = %02x-%02x-%02x-%02x-%02x-%02x ",
                            temp_mac_addr[0], temp_mac_addr[1],
                            temp_mac_addr[2], temp_mac_addr[3],
                            temp_mac_addr[4], temp_mac_addr[5]);
                	    (DRV_SERVICES(unit)->mem_field_get)
                		(unit, DRV_MEM_ARL, DRV_MEM_FIELD_VLANID, 
                		(uint32 *)&output, &fld_value);
                	    soc_cm_print("VLAN ID = %d, ",fld_value);
                	    if (temp_mac_addr[0] & 0x01) { /* mcast address */
                        /* The the multicast format didn't define, we need 
                            collect 3 fields to get the multicast index value.
                            multicast index : bit 55~48
                            age : bit 55
                            priority : bit 54~53
                            port id : bit 52~48
                        */
                            (DRV_SERVICES(unit)->mem_field_get)
                       	    (unit, DRV_MEM_ARL, DRV_MEM_FIELD_DEST_BITMAP, 
                                (uint32 *)&output, &fld_value);
                            soc_cm_print("Multicast Index = %d, ", fld_value);
                            (DRV_SERVICES(unit)->mem_field_get)
                                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_STATIC, 
                                (uint32 *)&output, &fld_value);
                            if (fld_value) {
                                soc_cm_print("STATIC");
                            }
                        } else { /* unicast address */
                            (DRV_SERVICES(unit)->mem_field_get)
                                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT, 
                                (uint32 *)&output, &fld_value);
                            soc_cm_print("Port = %d, ", fld_value);
                            (DRV_SERVICES(unit)->mem_field_get)
                                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_PRIORITY, 
                                (uint32 *)&output, &fld_value);
                            if (SOC_IS_ROBO_GE_SWITCH(unit)) {
                                soc_cm_print("Priority = %d", fld_value);
                            } else {
                                soc_cm_print("COS Queue = %d", fld_value);
                            }

                            (DRV_SERVICES(unit)->mem_field_get)
                                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_STATIC, 
                                (uint32 *)&output, &fld_value);
                            if (fld_value) {
                                soc_cm_print(", STATIC");
                            }
                            (DRV_SERVICES(unit)->mem_field_get)
                                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_AGE, 
                                (uint32 *)&output, &fld_value);
                            if (fld_value) {
                                soc_cm_print(", Hit");
                            }
                        }
            	    }
            }
            soc_cm_debug(DK_L2TABLE,"\n dump %d entries\n",count);
        }else {
        
	/* step 1: start the search of valid ARL entries */
        flags = DRV_MEM_OP_SEARCH_VALID_START;
        (DRV_SERVICES(unit)->mem_search)(unit, DRV_MEM_ARL, NULL, 
            NULL, NULL, flags);

	/* step 2: get valid ARL entries */
	flags = DRV_MEM_OP_SEARCH_VALID_GET;
        rv = SOC_E_EXISTS;
        while (rv == SOC_E_EXISTS) {
            sal_memset(&output, 0, sizeof(l2_arl_sw_entry_t));
           sal_memset(&output1, 0, sizeof(l2_arl_sw_entry_t));
            rv  = (DRV_SERVICES(unit)->mem_search)(unit, DRV_MEM_ARL, 
                NULL, (uint32 *)&output, (uint32 *)&output1, flags);
            if (rv == SOC_E_EXISTS) {
        	 /* Entry 0 */
                (DRV_SERVICES(unit)->mem_field_get)
                    (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, 
                    (uint32 *)&output, &fld_value);
                if (fld_value){
                    _bcm_robo_l2_from_arl(unit, &l2addr, &output);
                    dump_l2_addr(unit, "", &l2addr);
                }
                /* Entry 1 */
                (DRV_SERVICES(unit)->mem_field_get)
                    (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, 
                    (uint32 *)&output1, &fld_value);
                if (fld_value){
                    _bcm_robo_l2_from_arl(unit, &l2addr, &output1);
                    dump_l2_addr(unit, "", &l2addr);
                }
           } 
        	
        }
        }

	soc_cm_print("\n");
        return CMD_OK;
    }
    else if (!sal_strcasecmp(subcmd, "conflict")) {
	bcm_l2_addr_t		addr;
	bcm_l2_addr_t		cf[2];
	int			cf_count, i;

	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &arg_macaddr,NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &arg_vlan,	NULL);
	if (!parseEndOk(a, &pt, &ret_code))
	    return ret_code;

	bcm_l2_addr_t_init(&addr, arg_macaddr, arg_vlan);

	if ((rv = bcm_l2_conflict_get(unit, &addr,
				      cf, sizeof (cf) / sizeof (cf[0]),
				      &cf_count)) < 0) {
	    printk("%s: bcm_l2_conflict_get failed: %s\n",
		   ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}

	for (i = 0; i < cf_count; i++) {
	    dump_l2_addr(unit, "conflict: ", &cf[i]);
	}

	return CMD_OK;
    }
    else if (!sal_strcasecmp(subcmd, "watch")) {
	static int watch = 0;
	char* opt = ARG_GET(a);
	if(opt == NULL) {
	    printk("L2 watch is%srunning.\n",
		   (watch) ? " " : " not ");
	    return CMD_OK;
	}
	else if(!sal_strcasecmp(opt, "start")) {
	    watch = 1;
	    bcm_l2_addr_register(unit, _l2_watch_cb, NULL);
	    return CMD_OK;
	}
	else if(!sal_strcasecmp(opt, "stop")) {
	    watch = 0;
	    bcm_l2_addr_unregister(unit, _l2_watch_cb, NULL);
	    return CMD_OK;
	}
	else {
	    return CMD_USAGE;
	}
    }
    else if (!sal_strcasecmp(subcmd, "cache") ||
	     !sal_strcasecmp(subcmd, "c") ) {
        int cidx;
        char *cachecmd = NULL;
        char str[16];
        mac_addr_t arg_macaddr;
        int arg_cidx, arg_ccount, idx_max, arg_dstports = 0;
        bcm_l2_cache_addr_t l2caddr;

        if ((cachecmd = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        sal_memset(arg_macaddr, 0, sizeof (mac_addr_t));
        BCM_PBMP_CLEAR(arg_pbmp);

        arg_cidx = -1;
        arg_ccount = 1;

        if (!sal_strcasecmp(cachecmd, "add") ||
            !sal_strcasecmp(cachecmd, "+") ) {

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "CacheIndex",          PQ_DFL|PQ_INT,
                            0, &arg_cidx,       NULL);
            parse_table_add(&pt, "MACaddress",          PQ_DFL|PQ_MAC,
                            0, &arg_macaddr,    NULL);
            parse_table_add(&pt, "pbmp",                PQ_DFL|PQ_PBMP,
                            0, &arg_pbmp,      NULL);
            parse_table_add(&pt, "DestPorts",         PQ_DFL|PQ_BOOL,
                            0, &arg_dstports,	NULL);

            if (!parseEndOk(a, &pt, &ret_code)) {
                return ret_code;
            }

            if (arg_cidx == -1) {
                printk("%s ERROR: no index specified\n", ARG_CMD(a));
                return CMD_FAIL;
            }

            bcm_l2_cache_addr_t_init(&l2caddr);

            ENET_COPY_MACADDR(arg_macaddr, l2caddr.mac);

            /* Configure flags for SDK call */
            if (arg_dstports) {
                l2caddr.flags |= BCM_L2_CACHE_DESTPORTS;
                l2caddr.dest_ports = arg_pbmp;
            }

            _robo_dump_l2_cache_addr(unit, "ADD CACHE: ", &l2caddr);

            if ((rv = bcm_l2_cache_set(unit, arg_cidx, &l2caddr, 
                                       &cidx)) != BCM_E_NONE) {
                printk("8. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            printk(" => using index %d\n", cidx);

            return CMD_OK;
        }

        else if (!sal_strcasecmp(cachecmd, "del") ||
                 !sal_strcasecmp(cachecmd, "-")) {

            if (ARG_CNT(a)) {
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "CacheIndex",      PQ_DFL|PQ_INT,
                                0, &arg_cidx,   NULL);
                parse_table_add(&pt, "Count",           PQ_DFL|PQ_INT,
                                0, &arg_ccount, NULL);
                if (!parseEndOk(a, &pt, &ret_code)) {
                    return ret_code;
                }
            }

            if (arg_cidx == -1) {
                printk("%s ERROR: no index specified\n", ARG_CMD(a));
                return CMD_FAIL;
            }

            for (idx = 0; idx < arg_ccount; idx++) {
                if ((rv = bcm_l2_cache_get(unit, arg_cidx, &l2caddr)) < 0) {
                    printk("9. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }

                _robo_dump_l2_cache_addr(unit, "DEL CACHE: ", &l2caddr);

                printk(" => index %d\n", arg_cidx);

                if ((rv = bcm_l2_cache_delete(unit, arg_cidx)) != BCM_E_NONE) {
                    printk("10. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                arg_cidx++;
            }

            return CMD_OK;
        }

        else if (!sal_strcasecmp(cachecmd, "show") ||
                 !sal_strcasecmp(cachecmd, "-d")) {

	    if ((rv = bcm_l2_cache_size_get(unit, &idx_max)) < 0) {
		printk("11. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
		return CMD_FAIL;
	    }

	    for (idx = 0; idx <= idx_max; idx++) {
                if (bcm_l2_cache_get(unit, idx, &l2caddr) == BCM_E_NONE) {
                    sal_sprintf(str, " %4d : ", idx);
                    _robo_dump_l2_cache_addr(unit, str, &l2caddr);
                }
            }

            return CMD_OK;
        }

        else if (!sal_strcasecmp(cachecmd, "clear") ||
                 !sal_strcasecmp(cachecmd, "clr")) {

            if (ARG_CNT(a)) {
                parse_table_init(unit, &pt); 
                if (!parseEndOk(a, &pt, &ret_code)) {
                    return ret_code;
                }
            }

            if ((rv = bcm_l2_cache_delete_all(unit)) != BCM_E_NONE) {
                printk("12. %s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            } 

            return CMD_OK;
        }
        else {
            return CMD_USAGE;
        }
    }
    else {
        return CMD_USAGE;
    }

    return CMD_FAIL;
}


/*
 * Multicast command
 */

cmd_result_t
if_robo_mcast(int unit, args_t *a)
{
    char *subcmd;
    int r;
    static mac_addr_t mac_address;
    static int initted = 0;
    static int vid=1;
    static int mcindex = -1;
    static pbmp_t pbmp;
    parse_table_t pt;
    cmd_result_t ret_code;
    bcm_mcast_addr_t mcaddr;
    pbmp_t rtrpbmp;
    int port;

    if (! initted) {
        mac_address[0] = 0x1;
        mac_address[1] = 0;
        mac_address[2] = 0;
        mac_address[3] = 0;
        mac_address[4] = 0;
        mac_address[5] = 0x1;
        initted = 1;
    }

    if (! sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    SOC_PBMP_AND(pbmp, PBMP_ALL(unit));
    mcindex = -1;
    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC,
                0, &mac_address,NULL);
        parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX,
                0, &vid, NULL);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP,
                (void *)(0), &pbmp, NULL);
        parse_table_add(&pt, "Index", 	PQ_DFL|PQ_INT,
			0, &mcindex,	NULL);
        if (!parseEndOk( a, &pt, &ret_code)) {
            return ret_code;
        }
        bcm_mcast_addr_init(&mcaddr, mac_address, vid);
        mcaddr.pbmp = pbmp;
        mcaddr.l2mc_index = mcindex;
        if (mcindex < 0) {
	    r = bcm_mcast_addr_add(unit, &mcaddr);
        } else {
	    r = bcm_mcast_addr_add_w_l2mcindex(unit, &mcaddr);
        }
        if (r < 0) {
	    printk("bcm_mcast_addr_add failed ERROR: %s\n", bcm_errmsg(r));
	    return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (sal_strncasecmp(subcmd, "del", 3) == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC,
                0, &mac_address,NULL);
        parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX,
                0, &vid, NULL);
        if (!parseEndOk( a, &pt, &ret_code)) {
            return ret_code;
        }
        if ((r = bcm_mcast_addr_remove(unit, mac_address, vid)) < 0) {
            printk("bcm_mcast_addr_remove failed ERROR: %s\n",
                bcm_errmsg(r));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "join") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC,
                0, &mac_address,NULL);
        parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX,
                0, &vid, NULL);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP,
                (void *)(0), &pbmp, NULL);
        if (!parseEndOk( a, &pt, &ret_code)) {
            return ret_code;
        }
        PBMP_ITER(pbmp, port) {
            r = bcm_mcast_join(unit, mac_address, vid, port,
                       &mcaddr, &rtrpbmp);
            if (r < 0) {
                printk("ERROR: %s %s port %s failed: %s\n",
                       ARG_CMD(a), subcmd, SOC_PORT_NAME(unit, port),
                       bcm_errmsg(r));
                return CMD_FAIL;
            }
            switch (r) {
                case BCM_MCAST_JOIN_ADDED:
                    printk("port %s added\n", SOC_PORT_NAME(unit, port));
                    break;
                case BCM_MCAST_JOIN_UPDATED:
                    printk("port %s updated\n", SOC_PORT_NAME(unit, port));
                    break;
                default:
                    printk("port %s complete, return %d\n",
                           SOC_PORT_NAME(unit, port), r);
                    break;
            }
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "leave") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC,
                0, &mac_address,NULL);
        parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX,
                0, &vid, NULL);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP,
                (void *)(0), &pbmp, NULL);
        if (!parseEndOk( a, &pt, &ret_code)) {
            return ret_code;
        }
        PBMP_ITER(pbmp, port) {
            r = bcm_mcast_leave(unit, mac_address, vid, port);
            if (r < 0) {
                printk("ERROR: %s %s port %s failed: %s\n",
                       ARG_CMD(a), subcmd, SOC_PORT_NAME(unit, port),
                       bcm_errmsg(r));
                return CMD_FAIL;
            }
            switch (r) {
                case BCM_MCAST_LEAVE_NOTFOUND:
                    printk("port %s not found\n", SOC_PORT_NAME(unit, port));
                    break;
                case BCM_MCAST_LEAVE_DELETED:
                    printk("port %s deleted\n", SOC_PORT_NAME(unit, port));
                    break;
                case BCM_MCAST_LEAVE_UPDATED:
                    printk("port %s updated\n", SOC_PORT_NAME(unit, port));
                    break;
                default:
                    printk("port %s complete, return %d\n",
                           SOC_PORT_NAME(unit, port), r);
                    break;
            }
        }
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "padd") == 0) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &mac_address, NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &vid,	NULL);
	parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP,
			(void *)(0), &pbmp, NULL);
	if (!parseEndOk(a, &pt, &ret_code))
	    return ret_code;
	bcm_mcast_addr_t_init(&mcaddr, mac_address, vid);
	SOC_PBMP_ASSIGN(mcaddr.pbmp, pbmp);
	r = bcm_mcast_port_add(unit, &mcaddr);
	if (r < 0) {
	    printk("ERROR: %s %s ports failed: %s\n",
		   ARG_CMD(a), subcmd, bcm_errmsg(r));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "premove") == 0) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &mac_address, NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &vid,	NULL);
	parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP,
			(void *)(0), &pbmp, NULL);
	if (!parseEndOk(a, &pt, &ret_code))
	    return ret_code;
	bcm_mcast_addr_t_init(&mcaddr, mac_address, vid);
	SOC_PBMP_ASSIGN(mcaddr.pbmp, pbmp);
	r = bcm_mcast_port_remove(unit, &mcaddr);
	if (r < 0) {
	    printk("ERROR: %s %s ports failed: %s\n",
		   ARG_CMD(a), subcmd, bcm_errmsg(r));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "pget") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
                        0, &mac_address, NULL);
        parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
                        0, &vid,	NULL);
        if (!parseEndOk(a, &pt, &ret_code))
            return ret_code;
        bcm_mcast_addr_t_init(&mcaddr, mac_address, vid);
        SOC_PBMP_ASSIGN(mcaddr.pbmp, pbmp);
        r = bcm_mcast_port_get(unit, mac_address, vid, &mcaddr);
        if (r < 0) {
            printk("ERROR: %s %s ports failed: %s\n",
                   ARG_CMD(a), subcmd, bcm_errmsg(r));
            return CMD_FAIL;
        }

        printk("mac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d",
               mcaddr.mac[0], mcaddr.mac[1], mcaddr.mac[2],
               mcaddr.mac[3], mcaddr.mac[4], mcaddr.mac[5], mcaddr.vid);

        if (SOC_INFO(unit).port_num > 32) {
            printk(" mc_pbmp=0x%x 0x%x\n", SOC_PBMP_WORD_GET(mcaddr.pbmp, 1), 
                SOC_PBMP_WORD_GET(mcaddr.pbmp, 0));
        } else {
            printk(" mc_pbmp=0x%x\n", SOC_PBMP_WORD_GET(mcaddr.pbmp, 0));
        }
    
        return CMD_OK;
    }

    return CMD_USAGE;
}



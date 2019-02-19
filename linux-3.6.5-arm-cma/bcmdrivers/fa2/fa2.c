/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * This file implements fa+ device driver initialization, table access and debug
 * functionality
 *
 */
#include <linux/module.h>
#include <linux/if_ether.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include "mach/iproc_regs.h"
#include "mach/io_map.h"
#include "mach/irqs.h"
#include "fa2_regs.h"
#include "fa2_defs.h"
#include "fa2_if.h"
//#include <linux/bootmem.h>

/* Extern declarations */
int fa2_init_flow_cache(void);

/* Forward declarations */
static int fa2_open(struct inode *inode, struct file *fp);
static irqreturn_t fa2_interrupt_handler(int irq, void *dev_id);
int fa2_callback(fa2_ioctl_t callback, unsigned long arg);
static int fa2_hw_init(void);
/* static int fa2_dbg_test_rtmac_tbl(void); */

static int fa2_initialized = false;
static int fa2_drv_reg_flag = 0;
static bool fa2_hw_mem_init_done = false;

volatile void __iomem *fa2_virt_addr;

fa2_flentry_t *fa2_flcache;
spinlock_t fa2_lock;
uint8_t fa2_dbg_log_lvl = FA2_DBG_DEFAULT_LVL;
uint8_t fa2_enable_aging = false;

/* Structures maintained by driver, so that we do not need to initiate indirect 
 * accesses to get values
 */

struct fa2_rtmac_entry {
    struct fa2_rtmac_tbl_entry rtme;
    uint8_t valid; /* If s/w programmed this entry, valid is set to 1, else 0 */
};

struct fa2_tunltbl_entry {
    struct fa2_tunnel_tbl_entry tte;
    uint8_t valid; /* If s/w programmed this entry, valid is set to 1, else 0 */
};

struct fa2_nhtbl_entry {
    struct fa2_nh_tbl_entry nhte;
    uint8_t valid; /* If s/w programmed this entry, valid is set to 1, else 0 */
};

/* MTU table, accessed directly by  'mtu_idx' field */
uint16_t fa2_mtu_tbl[FA2_MTUTBL_SIZE];

 /* One bit for one port, port# determined by 'src_pid' field */
uint32_t fa2_porttyp_tbl;

/* To keep track of which bits were actually written to h/w. Since port type
 * entries are only 1-bit wide, there is no way to distinguish if the value
 * has a meaning (that is, whether it was explicitly programmed, or it just 
 * happens to be 0. The var below keeps track of this.
 */
uint32_t fa2_porttyp_entry_valid;

static struct fa2_rtmac_entry fa2_drv_rtmac_tbl[FA2_RTMACTBL_SIZE];
static struct fa2_tunltbl_entry fa2_drv_tunnel_tbl[FA2_TUNLTBL_SIZE];
static struct fa2_nhtbl_entry fa2_drv_nxthop_tbl[FA2_NEXTHOPTBL_SIZE];

const char *fa2_ioctl_name[] = {
    "FA2IOCTL_STATUS",
	"FA2IOCTL_RESET",
	"FA2IOCTL_INIT",
	"FA2IOCTL_ENABLE",
	"FA2IOCTL_DISABLE",
	"FA2IOCTL_REGDUMP",
	"FA2IOCTL_DUMPPT",
	"FA2IOCTL_DUMPNHT",
	"FA2IOCTL_CFGLOG",
	"FA2IOCTL_ADDFE",
    "FA2IOCTL_ADDPT",
    "FA2IOCTL_ADDNHT",
	"FA2IOCTL_INVALID"
};

/* 
 * This function searches driver's internal database for a matching source MAC 
 * address in router mac table. It returns index of the matching location, if the mac address
 * is found. Otherwise -1 is returned
 */
int fa2_search_rtmac_tbl(uint8_t *ma)
{
    int i;
    int found = false;

    if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
        printk(KERN_INFO "\n%s: Mac input: 0x%02X%02X%02X%02X%02X%02X\n",
               __func__, ma[0], ma[1], ma[2], ma[3], ma[4], ma[5]);
    }

    for (i = 0; i < FA2_RTMACTBL_SIZE; i++) {

        if (fa2_drv_rtmac_tbl[i].valid && 
            (memcmp(ma, fa2_drv_rtmac_tbl[i].rtme.mac_addr, FA2_MAC_ADDR_SIZE)
             == 0)) {

            found = true;

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
                printk(KERN_INFO "%s: Mac found in drv rtmac table at:"
                       " %d\n", __func__, i);
            }

            break;

        }
    }

    if (found == false) {
        i = -1;

        if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {

            printk(KERN_INFO "%s: Mac not found in drv rtmac table\n",
                   __func__);
        }
    }

    return i;
}

/* 
 * This function searches driver's internal database for a matching destination
 * MAC address in next hop table. It returns index of the matching location, 
 * if the mac address is found. Otherwise -1 is returned
 */
int fa2_search_nh_tbl(uint8_t *ma)
{
    int i;
    int found = false;

    if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
        printk(KERN_INFO "\n%s: Mac input: 0x%02X%02X%02X%02X%02X%02X\n",
               __func__, ma[0], ma[1], ma[2], ma[3], ma[4], ma[5]);
    }

    for (i = 0; i < FA2_NEXTHOPTBL_SIZE; i++) {

        if (fa2_drv_nxthop_tbl[i].valid && 
            (memcmp(ma, fa2_drv_nxthop_tbl[i].nhte.da, FA2_MAC_ADDR_SIZE) == 0)) {

            found = true;

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
                printk(KERN_INFO "%s: Mac found in drv nh table at:"
                       " %d\n", __func__, i);
            }

            break;

        }
    }

    if (found == false) {
        i = -1;

        if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
            printk(KERN_INFO "%s: Mac not found in drv nh table\n", __func__);
        }
    }

    return i;
}

/* 
 * This function searches driver's internal database for a matching source
 * MAC address and session id in tunnel table.
 * It returns index of the matching location, if a match is found.
 * Otherwise -1 is returned
 */
int fa2_search_tunl_tbl(uint8_t *ma, uint16_t sid)
{
    int i;
    int found = false;

    if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
        printk(KERN_INFO "\n%s: Mac input: 0x%02X%02X%02X%02X%02X%02X,"
               " session_id = %d(0x%X)\n",
               __func__, ma[0], ma[1], ma[2], ma[3], ma[4], ma[5], sid, sid);
    }

    for (i = 0; i < FA2_TUNLTBL_SIZE; i++) {

        if (fa2_drv_tunnel_tbl[i].valid &&
            (fa2_drv_tunnel_tbl[i].tte.session_id == sid) &&
            (memcmp(ma, fa2_drv_tunnel_tbl[i].tte.smac, FA2_MAC_ADDR_SIZE) == 0)) {

            found = true;

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
                    printk(KERN_INFO "%s: Match found in drv tunnel table at:"
                           " %d\n", __func__, i);
            }

            break;

        }
    }

    if (found == false) {

        i = -1;

        if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
            printk(KERN_INFO "%s: No matching entry found in drv tunnel"
                   " table\n", __func__);
        }
    }

    return i;
}

/* 
 * This function adds router mac entry to hardware. Also, it updates
 * its internal database. Caller fills all information. This function 
 * searches for matching mac address. If a match is found h/w entry is
 * not updated, else h/w entry is programmed.
 * If the caller wants to directly write to a given location, 
 * dir_write must be set to '1', and tbl_idx should contain a valid index value
 * Note, for such a direct write, we do not check state of the entry being
 * written, so use this option with caution
 * In normal cases, caller should set dir_write to 0
 */
int fa2_add_rtmac_entry(struct fa2_rtmac_tbl_entry *rtm, int *tbl_idx,
                        uint8_t dir_write)
{
    int ret = FA2_SUCCESS;
    int idx;
    int rtmac_tbl_ldx = 0;

    if (dir_write == 1) {
        rtmac_tbl_ldx = *tbl_idx % FA2_RTMACTBL_SIZE;
        goto FA2_RTMACTBL_WR;
    }

    /* Search drivers' internal database */
    idx = fa2_search_rtmac_tbl(rtm->mac_addr);

    if (idx != -1) {

        /* Entry found, return index to caller */
        *tbl_idx = idx; 

        return(FA2_SUCCESS);
    }

    /* Entry not present, proceed with adding a new entry */

    /* Check if there is space in the table to add an entry. If none, inform
     * caller
     */
    for (rtmac_tbl_ldx = 0; rtmac_tbl_ldx < FA2_RTMACTBL_SIZE; rtmac_tbl_ldx++){
        
        /* Break on first free available location */
        if (!fa2_drv_rtmac_tbl[rtmac_tbl_ldx].valid) {
            /* Free entry found, index in rtmac_tbl_ldx */
            break;
        }
    }

    if (rtmac_tbl_ldx >= FA2_RTMACTBL_SIZE) {

        printk(KERN_DEBUG "\n%s: Rtmac table full, new entry cannot be added\n",
               __func__);

        return(FA2_ABORT);
    }

FA2_RTMACTBL_WR:
    ret = fa2_set_tbl_entry(FA2_RTMACTBL, rtmac_tbl_ldx, (void *)rtm);

    if (ret == FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Rtmac entry added at %d\n",
               __func__, rtmac_tbl_ldx);

        /* Update drv database */
        fa2_drv_rtmac_tbl[rtmac_tbl_ldx].rtme.host = rtm->host;
        fa2_drv_rtmac_tbl[rtmac_tbl_ldx].rtme.l4_checksum_check = 
                                         rtm->l4_checksum_check;
        fa2_drv_rtmac_tbl[rtmac_tbl_ldx].rtme.external = rtm->external;

        memcpy(fa2_drv_rtmac_tbl[rtmac_tbl_ldx].rtme.mac_addr, rtm->mac_addr,
               FA2_MAC_ADDR_SIZE);

        fa2_drv_rtmac_tbl[rtmac_tbl_ldx].valid = 1;

        *tbl_idx = rtmac_tbl_ldx; 
    }
    else {
        printk(KERN_DEBUG "\n%s: Could not add rtmac entry at %d\n",
               __func__, rtmac_tbl_ldx);

        ret = FA2_ERROR;
    }

    return(ret);
}

/* 
 * This function adds next hop entry to hardware. Also, it updates
 * its internal database. Caller fills all information. This function 
 * searches for matching mac address. If a match is found h/w entry is
 * not updated, else h/w entry is programmed.
 * If the caller wants to directly write to a given location, 
 * dir_write must be set to '1', and tbl_idx should contain a valid index value
 * Note, for such a direct write, we do not check state of the entry being
 * written, so use this option with caution
 * In normal cases, caller should set dir_write to 0
 */
int fa2_add_nh_entry(struct fa2_nh_tbl_entry *nh, int *tbl_idx,
                     uint8_t dir_write)
{
    int ret = FA2_SUCCESS;
    int idx;
    int nh_tbl_ldx = 0;

    if (dir_write == 1) {
        nh_tbl_ldx = *tbl_idx % FA2_NEXTHOPTBL_SIZE;
        goto FA2_NHTBL_WR;
    }

    /* Search drivers' internal database */
    idx = fa2_search_nh_tbl(nh->da);

    if (idx != -1) {

        /* Entry found, return index to caller */
        *tbl_idx = idx; 

        return(FA2_SUCCESS);
    }

    /* Entry not present, proceed with adding a new entry */

    /* Check if there is space in the table to add an entry. If none, inform
     * caller
     */
    for (nh_tbl_ldx = 0; nh_tbl_ldx < FA2_NEXTHOPTBL_SIZE; nh_tbl_ldx++) {
        
        /* Break on first available location */
        if (!fa2_drv_nxthop_tbl[nh_tbl_ldx].valid) {
            /* Free entry found, index in nh_tbl_ldx */
            break;
        }
    }

    if (nh_tbl_ldx >= FA2_NEXTHOPTBL_SIZE) {

        printk(KERN_DEBUG "\n%s: Next hop table full, new entry cannot be added\n",
               __func__);

        return(FA2_ABORT);
    }

FA2_NHTBL_WR:
    ret = fa2_set_tbl_entry(FA2_NXTHOPTBL, nh_tbl_ldx, (void *)nh);

    if (ret == FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Next hop entry added at %d\n",
               __func__, nh_tbl_ldx);

        /* Update drv database */
        fa2_drv_nxthop_tbl[nh_tbl_ldx].nhte.vlan = nh->vlan;
        fa2_drv_nxthop_tbl[nh_tbl_ldx].nhte.op = nh->op;
        fa2_drv_nxthop_tbl[nh_tbl_ldx].nhte.l2_frmtype = nh->l2_frmtype;

        memcpy(fa2_drv_nxthop_tbl[nh_tbl_ldx].nhte.da, nh->da,
               FA2_MAC_ADDR_SIZE);

        fa2_drv_nxthop_tbl[nh_tbl_ldx].valid = 1;

        *tbl_idx = nh_tbl_ldx; 
    }
    else {
        printk(KERN_DEBUG "\n%s: Could not add next hop entry at %d\n",
               __func__, nh_tbl_ldx);

        ret = FA2_ERROR;
    }

    return(ret);
}

/* 
 * This function adds tunnel entry to hardware. Also, it updates
 * its internal database. Caller fills all information. This function 
 * searches for matching mac address. If a match is found h/w entry is
 * not updated, else h/w entry is programmed.
 * If the caller wants to directly write to a given location, 
 * dir_write must be set to '1', and tbl_idx should contain a valid index value
 * Note, for such a direct write, we do not check state of the entry being
 * written, so use this option with caution
 * In normal cases, caller should set dir_write to 0
 */
int fa2_add_tunl_entry(struct fa2_tunnel_tbl_entry *tt, int *tbl_idx,
                       uint8_t dir_write)
{
    int ret = FA2_SUCCESS;
    int idx;
    int tnl_tbl_ldx = 0;

    if (dir_write == 1) {
        tnl_tbl_ldx = *tbl_idx % FA2_TUNLTBL_SIZE;
        goto FA2_TUNLTBL_WR;
    }

    /* Search drivers' internal database */
    idx = fa2_search_tunl_tbl(tt->smac, tt->session_id);

    if (idx != -1) {

        /* Entry found, return index to caller */
        *tbl_idx = idx; 

        return(FA2_SUCCESS);
    }

    /* Entry not present, proceed with adding a new entry */

    /* Check if there is space in the table to add an entry. If none, inform
     * caller
     */
    for (tnl_tbl_ldx = 0; tnl_tbl_ldx < FA2_TUNLTBL_SIZE; tnl_tbl_ldx++) {
        
        /* Break on first available location */
        if (!fa2_drv_tunnel_tbl[tnl_tbl_ldx].valid) {
            /* Free entry found, index in tnl_tbl_ldx */
            break;
        }
    }

    if (tnl_tbl_ldx >= FA2_TUNLTBL_SIZE) {

        printk(KERN_DEBUG "\n%s: Tunnel table full, new entry cannot be added\n",
               __func__);

        return(FA2_ABORT);
    }

FA2_TUNLTBL_WR:
    ret = fa2_set_tbl_entry(FA2_TUNLTBL, tnl_tbl_ldx, (void *)tt);

    if (ret == FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Tunnel entry added at %d\n",
               __func__, tnl_tbl_ldx);

        /* Update drv database */
        fa2_drv_tunnel_tbl[tnl_tbl_ldx].tte.action = tt->action;
        fa2_drv_tunnel_tbl[tnl_tbl_ldx].tte.dmac_idx = tt->dmac_idx;
        fa2_drv_tunnel_tbl[tnl_tbl_ldx].tte.session_id = tt->session_id;

        memcpy(fa2_drv_tunnel_tbl[tnl_tbl_ldx].tte.smac, tt->smac,
               FA2_MAC_ADDR_SIZE);

        fa2_drv_tunnel_tbl[tnl_tbl_ldx].valid = 1;

        *tbl_idx = tnl_tbl_ldx; 
    }
    else {
        printk(KERN_DEBUG "\n%s: Could not add tunnel entry at %d\n",
               __func__, tnl_tbl_ldx);

        ret = FA2_ERROR;
    }

    return(ret);
}

/*
 * This function deletes a router mac entry in h/w, and clears driver's internal
 * database. If 'index' is a valid value, the entry at that location is 
 * deleted. Else, the function searches driver database based on mac addr
 */
int fa2_del_rtmac_entry(uint8_t *ma, int index)
{
    int ret = FA2_SUCCESS;
    int idx;
    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];

    /* Search based on mac addr and session id */
    if (index == -1) {

        /* Search driver's internal database */
        idx = fa2_search_rtmac_tbl(ma);

        if (idx == -1) {

            printk(KERN_DEBUG "\nError: %s: Valid entry not found for given"
                   " mac\n Check if mac is correct. Or, check if table is"
                   " populated correctly\n", __func__);

            return(FA2_ABORT);
        }
    }
    else {
        /* Caller passed valid index value */
        idx = index;
    }

    /* Clear array, so that 0s are written in h/w entry */
    memset(val, 0x0, sizeof(val));

    /* Note, to speed up operation, we call indirect access function directly */
    ret = fa2_indirect_write(FA2_RTMACTBL, idx, val);

    if (ret == FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Router Mac entry %d deleted \n", __func__, idx);

        /* Update drv database. Note: sets 'valid' flag to 0 for this entry */
        memset((void *)(&fa2_drv_rtmac_tbl[idx]), 0x0,
               sizeof(struct fa2_rtmac_entry));

    }
    else {
        printk(KERN_DEBUG "\n%s: Could not delete router mac entry at %d\n",
               __func__, idx);

        ret = FA2_ERROR;
    }

    return(ret);
}

/*
 * This function deletes a next hop entry in h/w, and clears driver's internal
 * database. If 'index' is a valid value, the entry at that location is 
 * deleted. Else, the function searches driver database based on mac addr
 */
int fa2_del_nh_entry(uint8_t *ma, int index)
{
    int ret = FA2_SUCCESS;
    int idx;
    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];

    /* Search based on mac addr and session id */
    if (index == -1) {

        /* Search driver's internal database */
        idx = fa2_search_nh_tbl(ma);

        if (idx == -1) {

            printk(KERN_DEBUG "\nError: %s: Valid entry not found for given"
                   " mac\n Check if mac is correct. Or, check if table is"
                   " populated correctly\n", __func__);

            return(FA2_ABORT);
        }
    }
    else {
        /* Caller passed valid index value */
        idx = index;
    }

    /* Clear array, so that 0s are written in h/w entry */
    memset(val, 0x0, sizeof(val));

    /* Note, to speed up operation, we call indirect access function directly */
    ret = fa2_indirect_write(FA2_NXTHOPTBL, idx, val);

    if (ret == FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Next hop entry %d deleted \n", __func__, idx);

        /* Update drv database. Note: sets 'valid' flag to 0 for this entry */
        memset((void *)(&fa2_drv_nxthop_tbl[idx]), 0x0,
               sizeof(struct fa2_nhtbl_entry));

    }
    else {
        printk(KERN_DEBUG "\n%s: Could not delete next hop entry at %d\n",
               __func__, idx);

        ret = FA2_ERROR;
    }

    return(ret);
}

/*
 * This function deletes a tunnel entry in h/w, and clears driver's internal
 * database. If 'index' is a valid value, the entry at that location is 
 * deleted. Else, the function searches driver database based on mac addr and
 *  session id  
 */
int fa2_del_tunl_entry(uint8_t *smac, uint16_t session_id, int index)
{
    int ret = FA2_SUCCESS;
    int idx;
    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];

    /* Search based on mac addr and session id */
    if (index == -1) {

        /* Search driver's internal database */
        idx = fa2_search_tunl_tbl(smac, session_id);

        if (idx == -1) {
            printk(KERN_DEBUG "\nError: %s: Valid entry not found for given"
                   " smac and session id.\n Check if these values are"
                   " correct. Or, check if table is populated correctly\n",
               __func__);

            return(FA2_ABORT);
        }
    }
    else {
        /* Caller passed valid index value */
        idx = index;
    }

    /* Clear array, so that 0s are written in h/w entry */
    memset(val, 0x0, sizeof(val));

    /* Note, to speed up operation, we call indirect access function directly */
    ret = fa2_indirect_write(FA2_TUNLTBL, idx, val);

    if (ret == FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Tunnel entry %d deleted \n", __func__, idx);

        /* Update drv database. Note: sets 'valid' flag to 0 for this entry */
        memset((void *)(&fa2_drv_tunnel_tbl[idx]), 0x0,
               sizeof(struct fa2_tunltbl_entry));

    }
    else {
        printk(KERN_DEBUG "\n%s: Could not delete tunnel entry at %d\n",
               __func__, idx);

        ret = FA2_ERROR;
    }

    return(ret);
}

/* 
 * This function is used to directly set a value in a given table. It is 
 * useful for smaller tables like mtu and port type tables. Other use may 
 * include clearing a entry (that is, deletion). Caller must carefully
 * construct values, as per field definitions and ranges in Uarch doc, 
 * else h/w will behave unpredicatbly.
 *
 *  table : one of the tables where the values needs to be written to
 *  index : the location where the data needs to be written. Caller must
 *        pass a valid index value (matching range of a given table)
 *  buffer :  contains value(s) to be programmed in to a given table
 *  buff_size : the valid # 32-bit words contained in 'buffer'
 */
static int fa2_set_table_value(enum fa2_tbl_num table, int index,
                               uint32_t *buffer, int buff_size)
{
    int ret = FA2_SUCCESS;
    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];
    int i;

    /* Clear array, so that 0s are written in h/w entry */
    memset(val, 0x0, sizeof(val));

    for (i = 0; i < buff_size; i++) {
        val[i] = buffer[i];
    }

    ret = fa2_indirect_write(table, index, val);

    return(ret);
}

/* 
 * Function to read raw table. See description for set function above
 */
static int fa2_get_table_value(enum fa2_tbl_num table, int index,
                               uint32_t *buffer, int buff_size)
{
    int ret = FA2_SUCCESS;
    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];
    int i;

    /* Clear array, so that 0s are written in h/w entry */
    memset(val, 0x0, sizeof(val));

    ret = fa2_indirect_read(table, index, val);

    for (i = 0; i < buff_size; i++) {
        buffer[i] = val[i]; /* b[0->7] = v[0->7]*/
    }

    return(ret);
}

/*
 * Sets a value in the port type table
 * val is either 0 or 1
 * index is the location where the value needs to be written (range: 0-31)
 * op tells whether the caller intends to clear (delete) the entry. Needed
 * since each entry is only 1-bit wide.
 * op = 1 means add operation,
 * op = 0 means delete operation. Note we do not program hw table for delete
 *        since there is no way to figure out if the entry is deleted, since
 *        it is a 1-bit field
 * Note: Since we keep track of each port type entry, theres no need to read
 * hardware for reading value 
 */
int fa2_set_port_type_entry(uint8_t val, int index, int op)
{
    int ret = FA2_SUCCESS;
    uint32_t v;

    /* One bit for one port, set appropriate bit */
    if (op) {
        v = val & 0x1;

        ret = fa2_set_table_value(FA2_PORTTYPTBL, index % FA2_PORTYPTBL_SIZE,
                                  &v, FA2_PORTYPTBL_ENTRY_SIZE);

        if (ret == FA2_SUCCESS) {

            fa2_porttyp_tbl |= (v << (index % FA2_PORTYPTBL_SIZE));
            /* Set valid bit */
            fa2_porttyp_entry_valid |= (1 << (index % FA2_PORTYPTBL_SIZE));
        }
        else {
            /* Set operation failed */
            ret = FA2_ERROR;
        }
    }
    else {
        /* Reset valid bit */
        //fa2_porttyp_tbl &= ~(1 << (index % FA2_PORTYPTBL_SIZE));
        fa2_porttyp_entry_valid &= ~(1 << (index % FA2_PORTYPTBL_SIZE));
    }

    return(ret);
}

/*
 * Sets a value in the mtu table
 * val is the mtu value (12-bit wide)
 * index is the location where the value needs to be written (range: 0-7)
 */
int fa2_set_mtu_entry(uint32_t val, int index)
{
    int ret = FA2_SUCCESS;

    val &= FA2_MTUENT_MTU_MASK;

    ret = fa2_set_table_value(FA2_MTUTBL, index % FA2_MTUTBL_SIZE,
                              &val, FA2_MTUTBL_ENTRY_SIZE);

    if (ret == FA2_SUCCESS) {
        fa2_mtu_tbl[index % FA2_MTUTBL_SIZE] = val;
    }
    else {
        /* Set operation failed, set an invalid value which caller may use, 
         * if needed
         */
        fa2_mtu_tbl[index % FA2_MTUTBL_SIZE] = FA2_INVALID_MTU_VAL;
        ret = FA2_ERROR;
    }

    return(ret);
}

/*
 * Gets a value from port type table. The function checks if the location being
 * accessed has a valid value (that is, the entry was programmed in to h/w).
 * If not valid, 0xFF is returned. Caller must check the return values. 
 * index : offset where the value is stored (range: 0-31)
 * loc : 0 = read driver's cached value, preferred unless it's really necessary.
 *           Its faster and avoids additional indirect read op 
 *       1 = read h/w mtu table's entry
 * val is buffer for storing port type value read (1-bit wide)
 * 
 */
int fa2_get_port_type_entry(int index, uint8_t loc, uint8_t *val)
{
    int ret = FA2_SUCCESS;
    uint32_t v;


    /* Check if the entry being accessed is valid, that is, programmed in to
     * the h/w. If not valid, return invalid value (0xFF).
     * Caller must check the return values.
     */
    if (!(fa2_porttyp_entry_valid & (1 << (index % FA2_PORTYPTBL_SIZE)))) {

        *val = 0xFF;

        return(FA2_SUCCESS);

    }

    if (!loc) {
        /* Read driver's value for the bit */
        *val = (fa2_porttyp_tbl & (1 << (index % FA2_PORTYPTBL_SIZE))) ? 0x1 : 0x0;

        //printk(KERN_INFO "\n%s: fa2_porttyp_tbl = 0x%08X, fa2_porttyp_entry_valid=0x%08X, *val = %u\n", __func__, fa2_porttyp_tbl, fa2_porttyp_entry_valid, *val);
    }
    else {

        /* Read h/w */
        ret = fa2_get_table_value(FA2_PORTTYPTBL, index % FA2_PORTYPTBL_SIZE,
                                  &v, FA2_PORTYPTBL_ENTRY_SIZE);

        if (ret == FA2_SUCCESS) {
            *val = v & 0x1;
        }
        else {
            ret = FA2_ERROR;
        }
    }

    return(ret);
}

/*
 * Gets a value in the mtu table
 * index : offset where the value is stored (range: 0-7)
 * loc : 0 = read driver's cached value, preferred unless it's really necessary.
 *           Its faster and avoids additional indirect read op 
 *       1 = read h/w mtu table's entry
 * val is buffer for storing mtu value read (12-bit wide)
 */
int fa2_get_mtu_entry(int index, uint8_t loc, uint32_t *val)
{
    int ret = FA2_SUCCESS;

    if (!loc) {
        /* Read driver's internal value */
        *val = fa2_mtu_tbl[index % FA2_MTUTBL_SIZE];
    }
    else {
 
        /* Read value from h/w */
        ret = fa2_get_table_value(FA2_MTUTBL, index % FA2_MTUTBL_SIZE,
                                  val, FA2_MTUTBL_ENTRY_SIZE);

        if (ret == FA2_SUCCESS) {
            *val &= FA2_MTUENT_MTU_MASK;

        }
        else {
            /* Get operation failed */
            ret = FA2_ERROR;
        }
    }

    return(ret);
}


/*
 * Function Name: fa2_handle_ioctl
 * Description  : Main entry point to handle user applications IOCTL requests
 * Returns      : 0 - success or error
 */
static int fa2_handle_ioctl( struct file *filep,
				unsigned int command, unsigned long arg)
{
	unsigned int cmd;

	int ret = FA2_SUCCESS;


    /* If needed, implement cmds coming 'before' *invalid below */
	if (command >= FA2IOCTL_INVALID)
		cmd = FA2IOCTL_INVALID;
	else
		cmd = command;

    /*printk("%s: cmd = %u(0x%X)\n", __func__, cmd, cmd); */

	/* printk(KERN_DEBUG "%s: cmd<%d> %s arg<%lu>", __func__, command,
	 	fa2_ioctl_name[command - 1], arg);  'command' is 1-based */


	switch (cmd) {
	case FA2IOCTL_STATUS:
		ret = fa2_callback(FA2CMDIOCTL_STATUS, arg);
		break;

	case FA2IOCTL_RESET:
		ret += fa2_callback(FA2CMDIOCTL_RESET, 0);
		break;

	case FA2IOCTL_INIT:
		ret = fa2_callback(FA2CMDIOCTL_INIT, 0);
		break;

	case FA2IOCTL_ENABLE:
		ret += fa2_callback(FA2CMDIOCTL_ENABLE, 0);
		break;

	case FA2IOCTL_DISABLE:
		ret = fa2_callback(FA2CMDIOCTL_DISABLE, 0);
		break;

	case FA2IOCTL_REGDUMP:
		ret += fa2_callback(FA2CMDIOCTL_REGDUMP, 0);
		break;

	case FA2IOCTL_CFGLOG:
		ret = fa2_callback(FA2CMDIOCTL_CFGLOG, arg);
		break;

	default:
		printk(KERN_DEBUG "Invalid cmd[0x%X]", cmd);
		return FA2_ERROR;
	}

	return ret;
}

/* File ops for registering driver with the kernel */
static struct file_operations fa2_fops = {
	.unlocked_ioctl  = (void *) fa2_handle_ioctl,
	.open   = fa2_open,
    .owner = THIS_MODULE,
};

/*
 * Function Name: iproc_nsp_fa2_probe
 * Description  : Initial function that is called at system startup that
 *                registers this device.
 * Returns      : FA2DRV_MAJOR (success).
 *		  err (error)
 */
static int __devinit iproc_nsp_fa2_probe(struct platform_device *pdev)
{
	char 	*dev_name = FA2DRV_NAME; /* "FA2"; */
    int rc = 0, irq;
    struct resource *iomem;
    struct resource *ioarea;

	printk(KERN_INFO "FA+ driver probe");

	/* Get register memory resource */
    iomem = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    if (!iomem) {

        printk(KERN_ERR "%s: No mem resource\n", __func__);

        return -ENODEV;
    }

    printk(KERN_DEBUG "\nFA+: Got iomem 0x%p\n", iomem);

	/* Get the interrupt number */
    irq = platform_get_irq(pdev, 0);

    if (irq == -ENXIO) {

        printk(KERN_ERR "%s: No irq resource\n", __func__);

        return -ENODEV;
    }

    printk(KERN_DEBUG "\nFA+: Got irqnum %d\n", irq);

	/* Mark the memory region as used */
    ioarea = request_mem_region(iomem->start, resource_size(iomem),
                                pdev->name);
    if (!ioarea) {

        printk(KERN_ERR "%s: Error: FA+ device region already claimed\n",
               __func__);

        return -EBUSY;
    }

    printk(KERN_DEBUG "\nFA+: Got ioarea 0x%p\n", ioarea);

    fa2_virt_addr = ioremap(iomem->start, resource_size(iomem));

    if (!fa2_virt_addr) {

        printk(KERN_ERR "%s: ioremap of register space failed\n", __func__);

        rc = -ENOMEM;

        goto err_fa2_rel_mem_region;

    }

    printk(KERN_DEBUG "\nFA+:  ==== Got block_base_addr=0x%08X\n",
          (unsigned int)fa2_virt_addr);

	/*
	 * Register an interrupt handler and disable interrupts.
	 */
	rc = request_irq(irq, fa2_interrupt_handler,
			  IRQF_DISABLED | IRQF_PERCPU,
			  dev_name, NULL);

	if (rc) {
		printk(KERN_ERR "%s: Failed to register %s irq %d\n",
			__func__, dev_name, irq);

		goto err_fa2_deinit;
	}

    printk(KERN_DEBUG "\nFA+: request_irq succeeded\n");

#if 0  /* boot time allocation - works but gives WARN_ON_ONCE
          from mm/bootmem.c 582 */
    fa2_flcache = (fa2_flentry_t *)alloc_bootmem(80 * 4096);

    if (fa2_flcache == NULL) {
        printk(KERN_ERR "%s Mem allocation failed, size=%u\n", __func__, 80*4096);
        return -ENOMEM;
    }
    else {
        printk(KERN_ERR "%s Allocated memory, size=%u bytes, at 0x%p\n", __func__, 80*4096, fa2_flcache);
        memset(fa2_flcache, 0x0, 80*4096);
    }
#endif 

    /* We are allocating memory for max # entries. If h/w table size should be 
     * programmed to be less than max, we can reduce this amount too
     */
    fa2_flcache = (fa2_flentry_t *)kmalloc_array(FA2_FLOWTBL_SIZE, 
                                           sizeof(fa2_flentry_t),
                                           GFP_KERNEL);

    /* If memory is not avaibale, do not continue with initialization */
    if (fa2_flcache == NULL) {
        printk(KERN_ERR "%s Mem allocation failed for size %u\n",
               __func__, sizeof(fa2_flentry_t) * FA2_FLOWTBL_SIZE);


        rc = -ENOMEM;
        goto err_fa2_deinit;
    }
    else {
        printk(KERN_INFO "%s allocated memory, %u bytes at 0x%p\n",
               __func__, sizeof(fa2_flentry_t) * FA2_FLOWTBL_SIZE, 
               fa2_flcache);
    }

	rc = register_chrdev(FA2DRV_MAJOR, dev_name, &fa2_fops);

	if (rc < 0) {
		printk(KERN_ERR "%s Unable to get major number %d, rc = %d\n",
			__FUNCTION__, FA2DRV_MAJOR, rc);

		goto err_fa2_deinit;
	}

	printk(KERN_INFO "FA+ char driver registered, major %d, minor %d, "
           "dev_name: %s\n", FA2DRV_MAJOR, 0, dev_name);

    /* Initialize spin lock */ 
    spin_lock_init(&fa2_lock);
 
    rc = fa2_hw_init();

    if (rc != FA2_SUCCESS) {
	    printk(KERN_INFO "%s: Error: FA2 h/w initialization failed, rc = %d\n",
               __func__, rc);

        rc = -EIO;

		goto err_fa2_deinit;
    }

	printk(KERN_INFO "%s: FA+ h/w initialized\n", __func__);

	fa2_drv_reg_flag = 1;

	printk(KERN_INFO "%s: FA+ driver registered\n", __func__);

	return 0;

err_fa2_deinit:
    iounmap(fa2_virt_addr);
    platform_set_drvdata(pdev, NULL);

err_fa2_rel_mem_region:
    release_mem_region(iomem->start, resource_size(iomem));

    printk(KERN_ERR "%s: probe failed, error=%d", __func__, rc);

    return (rc);
}

/*
 * Function Name: fa2_drv_exit
 * Description  : Function called when module is unloaded.
 * Returns      : Nothing
 */
void __exit fa2_drv_exit(void)
{

	unregister_chrdev(FA2DRV_MAJOR, FA2DRV_NAME );

	fa2_drv_reg_flag = 0;

	printk(KERN_INFO "FA2 Driver Unregistered<%d>\n", FA2DRV_MAJOR);
}

static irqreturn_t fa2_interrupt_handler(int irq, void *dev_id)
{
	volatile void __iomem		*reg_addr;
	volatile uint32_t	        ctlstatus;
	volatile uint32_t	        errstatus;
	volatile uint32_t	        eccstatus;

	reg_addr = fa2_virt_addr + CTF_STATUS_REG_OFF;
	ctlstatus = ioread32(reg_addr);
    if (ctlstatus) {
        /* Clear ctf status (0x68) interrupts (w1tc) */
        iowrite32(ctlstatus, reg_addr);
    }

	reg_addr = fa2_virt_addr + CTF_ERROR_REG_REG_OFF;
    errstatus = ioread32(reg_addr);
    if (errstatus) {
        /* Clear ctf error (0x90) interrupts (w1tc). Check if we need to handle
         * these intrs 
         */
        iowrite32(errstatus, reg_addr);
    }

	reg_addr = fa2_virt_addr + CTF_SER_EVENT_REG_OFF;
    eccstatus = ioread32(reg_addr);
    if (eccstatus) {
        /* Clear ctf error (0xD4) interrupts (w1tc). Check if we need to handle
         * these intrs
         */
        iowrite32(eccstatus, reg_addr);
    }

    /* Memory init done */
    if (ctlstatus & FA2_CTF_STATUS_INIT_DONE) {
        fa2_hw_mem_init_done = true;
    }

    /* If all *status values are 0, then this handler was called in error, for
     * event(s) that do not belong to this(fa+) block; should not happen
     */
    if (!ctlstatus && !errstatus && !eccstatus) {
        return IRQ_NONE;
    }

	return IRQ_HANDLED;
}

/*
 * This function enables/disables SPU block
 * To enable, pass val = 1,
 * To disable, pass val = 0;
 */
void fa2_spu_en_dis(int val)
{
    volatile void __iomem		*reg_addr;
    volatile uint32_t	        reg_val;

	reg_addr = fa2_virt_addr + CTF_CONTROL_REG_OFF;
	reg_val = ioread32(reg_addr);
    reg_val |= ((val & 0x1) ? FA2_SPU_ENABLE : 0);
    iowrite32(reg_val, reg_addr);

    printk(KERN_INFO "\n%s: SPU block %s\n", __func__,
          (val & 0x1) ? "enabled" : "disabled");

    return;
}

/*
 * This function enables/disables PAE interface
 * To enable, pass val = 1,
 * To disable, pass val = 0;
 */
void fa2_pae_en_dis(int val)
{
    volatile void __iomem		*reg_addr;
    volatile uint32_t	        reg_val;

	reg_addr = fa2_virt_addr + CTF_CONTROL_REG_OFF;
	reg_val = ioread32(reg_addr);
    reg_val |= ((val & 0x1) ? FA2_PAE_ENABLE : 0);
    iowrite32(reg_val, reg_addr);

    printk(KERN_INFO "\n%s: PAE i/f %s\n", __func__,
           (val & 0x1) ? "enabled" : "disabled");

    return;
}


/* 
 * This function is called during initialization or exit to clear all
 * driver table configuration
 */
static void fa2_clear_drv_tbl_config(void)
{
    int i;

    for (i = 0; i < FA2_MTUTBL_SIZE; i++) {
        fa2_mtu_tbl[i] = FA2_INVALID_MTU_VAL;
    }

    fa2_porttyp_tbl = 0;
    fa2_porttyp_entry_valid = 0;

    memset((void *)fa2_drv_rtmac_tbl, 0x0, sizeof(fa2_drv_rtmac_tbl));
    memset((void *)fa2_drv_tunnel_tbl, 0x0, sizeof(fa2_drv_tunnel_tbl));
    memset((void *)fa2_drv_nxthop_tbl, 0x0, sizeof(fa2_drv_nxthop_tbl));

    return;
}

void fa2_dump_regs(void)
{
	volatile void __iomem		*reg_addr;
	volatile uint32_t	        reg_val;
    uint32_t                    reg_offset;
    int i;
    char v[100], *p = v;

    v[0] = '\0';

    printk(KERN_INFO "\nReg Addr   +0         +4         +8         +C");
    printk(KERN_INFO 
           "\n------------------------------------------------------\n");
    for (reg_offset = CTF_CONTROL_REG_OFF, i = 0; 
         reg_offset <= CTF_MEM_ACC_DATA6_REG_OFF; 
         reg_offset += 4, i++) {

            reg_addr = fa2_virt_addr + reg_offset;

            /* Register 0x58 fields are clear-on-read. When aging is enabled,
             * and if we read this reg, the aging handler will receive wrong
             * values. So we do not read the register at all in this debug
             * command. Shouldn't be done using devmem or any other means as 
             * well
             */
            if (reg_offset == CTF_FLOW_TIMEOUT_CONTROL_REG_OFF) {
                reg_val = 0x0;
            } else {
                reg_val = ioread32(reg_addr);
            }

            if ((i > 0) && !(i % 4)) {
                printk(KERN_INFO "0x%08X%s\n", 
                       (unsigned int)(HW_IO_VIRT_TO_PHYS((unsigned int)reg_addr - 0x10)), v);
                v[0] = '\0';
                p = v;
            }
            snprintf(p, 100 , " 0x%08X", reg_val);
            p += 11;
           
    }
    printk(KERN_INFO "0x%08X%s", (unsigned int)(HW_IO_VIRT_TO_PHYS((unsigned int)reg_addr)), v);
    printk(KERN_INFO
          "------------------------------------------------------\n");
    printk(KERN_INFO "\n\n");

    return;
}

/*
 * This function programs default parameters in aging related registers. This 
 * function is called only if aging is enabled 
 */
static int fa2_cfg_aging(void)
{
	volatile void __iomem		*reg_addr;
	volatile uint32_t	        reg_val;

    reg_addr = fa2_virt_addr + CTF_FLOW_TIMEOUT_CONTROL_REG_OFF;
    reg_val = ioread32(reg_addr);
    /* Let host handle syn packets */
    reg_val |= FA2_CTF_FLOW_TIMEOUT_CONTROL_TCP_SYN_BYPASS_LKUP;
    /* Enable messaging by h/w when it closes one or more flows */
    reg_val |= FA2_CTF_FLOW_TIMEOUT_CONTROL_HW_TIMEOUT_MSG_ENABLE;
    iowrite32(reg_val, reg_addr);

    /* Set up time bases for udp, tcp */
    reg_addr = fa2_virt_addr + CTF_FLOW_TIMER_CONFIG0_REG_OFF;
    reg_val = ioread32(reg_addr);

    /* Use timebase 2^31 (0x1) for all flow types */
    FA2_SETBITS(reg_val, 
                CTF_FLOW_TIMER_CONFIG0__TCP_FINISHED_TIMEBASE_TWO_EXP31,
                FA2_CTF_FLOW_TIMER_CONFIG0_TCP_FINISH_TBASE,
                CTF_FLOW_TIMER_CONFIG0__TCP_FINISHED_TIMEBASE_R);

    FA2_SETBITS(reg_val, 
                CTF_FLOW_TIMER_CONFIG0__TCP_ESTABLISHED_TIMEBASE_TWO_EXP31,
                FA2_CTF_FLOW_TIMER_CONFIG0_TCP_EST_TBASE, 
                CTF_FLOW_TIMER_CONFIG0__TCP_ESTABLISHED_TIMEBASE_R);

    FA2_SETBITS(reg_val,
                CTF_FLOW_TIMER_CONFIG0__UDP_ESTABLISHED_TIMEBASE_TWO_EXP31,
                FA2_CTF_FLOW_TIMER_CONFIG0_UDP_EST_TBASE, 
                CTF_FLOW_TIMER_CONFIG0__UDP_ESTABLISHED_TIMEBASE_R);

    iowrite32(reg_val, reg_addr);

    /* Set init timesout values 180s for udp, 240s for tcp. Close tcp flow after
     * 30s (giving sufficient time for both ends to close their (half) 
     * connection).
     * Note: if the values programmed in reg 0x5C above are changed, the values
     * in this register (0x60) will change correspondingly
     */
    reg_addr = fa2_virt_addr + CTF_FLOW_TIMER_CONFIG1_REG_OFF;
    reg_val = ioread32(reg_addr);

    /* 180/8.59 (21), 240/8.59(28), 30/8.59(4) */

    FA2_SETBITS(reg_val, FA2_TCP_FIN_FL_DEF_TOUT,
                FA2_CTF_FLOW_TIMER_CONFIG1_TCP_FINISHED_TOUT, 
                CTF_FLOW_TIMER_CONFIG1__TCP_FINISHED_TIMEOUT_R);

    FA2_SETBITS(reg_val, FA2_TCP_EST_FL_DEF_TOUT, 
                FA2_CTF_FLOW_TIMER_CONFIG1_TCP_ESTABLISHED_TOUT,
                CTF_FLOW_TIMER_CONFIG1__TCP_ESTABLISHED_TIMEOUT_R);

    FA2_SETBITS(reg_val, FA2_UDP_EST_FL_DEF_TOUT, 
                FA2_CTF_FLOW_TIMER_CONFIG1_UDP_ESTABLISHED_TOUT,
                CTF_FLOW_TIMER_CONFIG1__UDP_ESTABLISHED_TIMEOUT_R);

    iowrite32(reg_val, reg_addr);

    return FA2_SUCCESS;
}

static int fa2_hw_init(void)
{
	volatile void __iomem		*reg_addr;
	volatile uint32_t	        reg_val;
    int ret = FA2_SUCCESS;
    volatile int retries;

    /* Check fa2 block was initialized previously */
    if (fa2_initialized	== true) {

        printk(KERN_DEBUG "%s: FA2 h/w is already initialized\n", __func__);

        return FA2_SUCCESS;
    }


    /* Clear any spurious errors before initialization (w1tc) */
    reg_addr = fa2_virt_addr + CTF_SER_EVENT_REG_OFF;
    reg_val = ioread32(reg_addr);
    iowrite32(reg_val, reg_addr);

	reg_addr = fa2_virt_addr + CTF_CONTROL_REG_OFF;

	/* Read intr status and clear it */
	reg_val = ioread32(reg_addr);

    /*reg_val |= FA2_CRC_OWRT_CONFIG;  Check if it should be set,
                                       was set in FA. */
    reg_val |= FA2_DISABLE_MAC_DA_CHK; /* Disable mac da check */
    reg_val |= FA2_CTF_MODE;
    reg_val |= FA2_MEM_INIT; /* Start memory initialization */
    /* Note: Mac DA check is enabled by default. Hit will result in counting
     * packets (bit #4)
     */

    /* Check if SPU, PAE also need to be enabled by default. Or, whether it 
     * should be done later on
     */

    iowrite32(reg_val, reg_addr);

	reg_addr = fa2_virt_addr + CTF_STATUS_REG_OFF;

    /* Poll for mem init completion */
    retries = 0;
    do {
        msleep(1);
	    reg_val = ioread32(reg_addr);
        retries++;
    } while (!(reg_val & FA2_CTF_STATUS_INIT_DONE) && (retries <= 50));

    if (retries > 50)  {
    
	    printk("%s: ERROR: Hardware initialization did not complete after %d"
               " attempts. Reg 0x68 = 0x%08X. Exiting. \n",
               __func__, retries, reg_val);

        return FA2_ERROR;
    }

    printk(KERN_DEBUG "\n%s: Writing 0x%X to 0x%X\n",
           __func__, reg_val, (unsigned int)reg_addr);
    /* Clear the interrupt(s) */
    iowrite32(reg_val, reg_addr);

	reg_addr = fa2_virt_addr + CTF_L2_SKIP_CONTROL_REG_OFF;
    reg_val = FA2_ETH2_TO_SNAP_CONVERSION; /* Enable eth2 to SNAP conversion */
    iowrite32(reg_val, reg_addr);

	reg_addr = fa2_virt_addr + CTF_HASH_SEED_REG_OFF;
    reg_val = 0x3021; /* A random value for seed */
    iowrite32(reg_val, reg_addr);

    /* Reg 0x94 - enable reporting of error status */
    /* TODO: Ask design/DV if these are interrupt bits or simply to enable
     * status
     */

	reg_addr = fa2_virt_addr + CTF_DRR_CONFIG_REG_OFF;
    reg_val = 0x30; /* Value obtained from DV's config */
    reg_val |= (0x20 << CTF_DRR_CONFIG__MAC_WEIGHT_R); 
    iowrite32(reg_val, reg_addr);


    if (fa2_enable_aging) {
        fa2_cfg_aging();
    }

    fa2_spu_en_dis(1);
    fa2_pae_en_dis(0);

    /* Set all internal shadow (caches/drv database) for all tables to default
     * values
     */
    fa2_clear_drv_tbl_config();

    /* This portion can be moved afer call to hw_init succeeds */
    if (fa2_init_flow_cache() != FA2_SUCCESS) {

       printk(KERN_ERR "\nError: Flow cache initialization failed\n");

       ret = FA2_ERROR;
    }

    fa2_initialized = true;

    printk(KERN_INFO "\n==== %s: fa2 block initialized\n", __func__);

    return ret;
}

int is_fa2_initialized(void)
{
    return(fa2_initialized);
}

/* TODO:  Check if the block can be enabled or disabled. Putting it in
 * coprocessor mode is not the correct way, as was done for FA
 * Possible steps may be: start/stop traffic through back pressure control,
 * enable/disable interrupts, initing memory through MEM_INIT bit
 * Check if there any dependency on spu, pae  
 */
int fa2_enable(void)
{
    return(FA2_SUCCESS);
}

int fa2_disable(void)
{
    return(FA2_SUCCESS);
}

/*
 * Function   : fa2_reset_tables
 * Description: Resets tables associated with CTF
 *
 * Parameters :
 *      t :	Table to clear
 *
 * Returns    :
 *    success : FA2_SUCCESS, 0
 *    failure : FA2_ERROR, -1
 */
int fa2_reset_tables(enum fa2_tbl_num t)
{
	volatile void __iomem		*reg_addr;
	volatile uint32_t	        reg_val;
    int ret = FA2_SUCCESS;
    volatile int retries;
    int i;
    uint32_t val = 0;

    switch(t) {

        case FA2_FLTBL_V6_W1:
        case FA2_FLTBL_V6_W0:
        case FA2_FLTBL_V4:
        case FA2_NXTHOPTBL:
            /* Note: mem_init resets both flow and next hop tables */ 
	        reg_addr = fa2_virt_addr + CTF_CONTROL_REG_OFF;

	        reg_val = ioread32(reg_addr);

            reg_val |= FA2_MEM_INIT; /* Start memory initialization */

            iowrite32(reg_val, reg_addr);

	        reg_addr = fa2_virt_addr + CTF_STATUS_REG_OFF;

            /* Poll for mem init completion */
            retries = 0;
            do {
                msleep(1);
	            reg_val = ioread32(reg_addr);
                retries++;
            } while (!(reg_val & FA2_CTF_STATUS_INIT_DONE) && (retries <= 50));

            if (retries > 50)  {
    
	            printk("%s: ERROR: Hardware initialization did not complete "
                       "after %d attempts. Reg 0x68 = 0x%08X. Exiting. \n",
                       __func__, retries, reg_val);

                ret = FA2_ERROR;
            }
            break;

        case FA2_MTUTBL:
            for (i = 0; i < FA2_MTUTBL_SIZE; i++) {
                fa2_set_tbl_entry(FA2_MTUTBL, i, (void *)(&val));
            }

            break;
        case FA2_PORTTYPTBL:
            for (i = 0; i < FA2_PORTYPTBL_SIZE; i++) {
                fa2_set_tbl_entry(FA2_PORTTYPTBL, i, (void *)(&val));
            }
            break;

        case FA2_RTMACTBL:
            {
                struct fa2_rtmac_tbl_entry s;

                memset((void *)&s, 0x0, sizeof(s));

                for (i = 0; i < FA2_RTMACTBL_SIZE; i++) {
                    fa2_set_tbl_entry(FA2_RTMACTBL, i, (void *)(&s));
                }
            }
            break;

        case FA2_TUNLTBL:
            {
                struct fa2_tunnel_tbl_entry s;

                memset((void *)&s, 0x0, sizeof(s));

                for (i = 0; i < FA2_TUNLTBL_SIZE; i++) {
                    fa2_set_tbl_entry(FA2_TUNLTBL, i, (void *)(&s));
                }
            }
            break;

        default:
		    printk(KERN_DEBUG "ERROR: %s - invalid table id passed %d",
                   __func__, t);
		    ret = FA2_ERROR;
            break;

    }

    return ret;
}



static int fa2_dbg_get_nh_tbl(void)
{
    struct fa2_nh_tbl_entry nht;
    int r = FA2_SUCCESS;
    int idx;

    printk(KERN_INFO "\nNext hop h/w table:\n");

    for (idx = 0; idx < FA2_NEXTHOPTBL_SIZE; idx++) {
        if (fa2_drv_nxthop_tbl[idx].valid) {
            memset(&nht, 0, sizeof(nht));

            r = fa2_get_tbl_entry(FA2_NXTHOPTBL, idx, (void *)(&nht));

            if (r == FA2_SUCCESS) {
                printk(KERN_INFO "\nEntry %d", idx);
                fa2_display_nh_entry(&nht);
            } else {
                printk(KERN_INFO "\n%s: Call to fa2_get_tbl_entry failed for "
                       "index %d. Error is %d\n", __func__, idx, r);
                return(r);
            }
        }
    }

    printk(KERN_INFO "\nDriver next hop table:\n");

    for (idx = 0; idx < FA2_NEXTHOPTBL_SIZE; idx++) {
        if (fa2_drv_nxthop_tbl[idx].valid) {
            printk(KERN_INFO "\nvalid = %u, vlan=%u, op=%u, l2_frmtype=%u,"
                   " da[0-5]=%02X %02X %02X %02X %02X %02X\n", 
                   fa2_drv_nxthop_tbl[idx].valid,
                   fa2_drv_nxthop_tbl[idx].nhte.vlan, 
                   fa2_drv_nxthop_tbl[idx].nhte.op, 
                   fa2_drv_nxthop_tbl[idx].nhte.l2_frmtype, 
                   fa2_drv_nxthop_tbl[idx].nhte.da[0], 
                   fa2_drv_nxthop_tbl[idx].nhte.da[1], 
                   fa2_drv_nxthop_tbl[idx].nhte.da[2], 
                   fa2_drv_nxthop_tbl[idx].nhte.da[3], 
                   fa2_drv_nxthop_tbl[idx].nhte.da[4], 
                   fa2_drv_nxthop_tbl[idx].nhte.da[5]);
        }
    }

    return(r);
}

static int fa2_dbg_get_rtmac_tbl(void)
{
    struct fa2_rtmac_tbl_entry rt;
    int r = FA2_SUCCESS;
    int idx;

    printk(KERN_INFO "\nRtmac h/w table:\n");

    for (idx = 0; idx < FA2_RTMACTBL_SIZE; idx++) {
        if (fa2_drv_rtmac_tbl[idx].valid) {
            memset(&rt, 0, sizeof(rt));

            r = fa2_get_tbl_entry(FA2_RTMACTBL, idx, (void *)(&rt));

            if (r == FA2_SUCCESS) {
                printk(KERN_INFO "\nEntry %d", idx);
                fa2_display_rtmac_entry(&rt);
            } else {
                printk(KERN_INFO "\n%s: Call to fa2_get_tbl_entry failed for "
                       "index %d. Error is %d\n", __func__, idx, r);
                return(r);
            }
        }
    }

    printk(KERN_INFO "\nDriver rtmac table:\n");

    for (idx = 0; idx < FA2_RTMACTBL_SIZE; idx++) {
        if (fa2_drv_rtmac_tbl[idx].valid) {
            printk(KERN_INFO "\nvalid = %u, host=%u, ext=%u, l4_cs=%u,"
                   " mac[0-5]=%02X %02X %02X %02X %02X %02X\n", 
                   fa2_drv_rtmac_tbl[idx].valid,
                   fa2_drv_rtmac_tbl[idx].rtme.host, 
                   fa2_drv_rtmac_tbl[idx].rtme.external, 
                   fa2_drv_rtmac_tbl[idx].rtme.l4_checksum_check, 
                   fa2_drv_rtmac_tbl[idx].rtme.mac_addr[0], 
                   fa2_drv_rtmac_tbl[idx].rtme.mac_addr[1], 
                   fa2_drv_rtmac_tbl[idx].rtme.mac_addr[2], 
                   fa2_drv_rtmac_tbl[idx].rtme.mac_addr[3], 
                   fa2_drv_rtmac_tbl[idx].rtme.mac_addr[4], 
                   fa2_drv_rtmac_tbl[idx].rtme.mac_addr[5]);
        }
    }

    return(r);
}

static int fa2_dbg_get_tunl_tbl(void)
{
    struct fa2_tunnel_tbl_entry tt;
    int r = FA2_SUCCESS;
    int idx;

    printk(KERN_INFO "\nTunnel (PPPoE) h/w table:\n");

    for (idx = 0; idx < FA2_TUNLTBL_SIZE; idx++) {
        if (fa2_drv_tunnel_tbl[idx].valid) {
            memset(&tt, 0, sizeof(tt));

            r = fa2_get_tbl_entry(FA2_TUNLTBL, idx, (void *)(&tt));

            if (r == FA2_SUCCESS) {
                printk(KERN_INFO "\nEntry %d", idx);
                fa2_display_tunltbl_entry(&tt);
            } else {
                printk(KERN_INFO "\n%s: Call to fa2_get_tbl_entry failed for "
                       "index %d. Error is %d\n", __func__, idx, r);
                return(r);
            }
        }
    }

    printk(KERN_INFO "\nDriver tunnel (PPPoE) table:\n");

    for (idx = 0; idx < FA2_TUNLTBL_SIZE; idx++) {
        if (fa2_drv_tunnel_tbl[idx].valid) {
            printk(KERN_INFO "\nvalid = %u, action=%u, dmac_idx=%u, session_id=%u,"
                   " smac[0-5]=%02X %02X %02X %02X %02X %02X\n", 
                   fa2_drv_tunnel_tbl[idx].valid,
                   fa2_drv_tunnel_tbl[idx].tte.action, 
                   fa2_drv_tunnel_tbl[idx].tte.dmac_idx, 
                   fa2_drv_tunnel_tbl[idx].tte.session_id, 
                   fa2_drv_tunnel_tbl[idx].tte.smac[0], 
                   fa2_drv_tunnel_tbl[idx].tte.smac[1], 
                   fa2_drv_tunnel_tbl[idx].tte.smac[2], 
                   fa2_drv_tunnel_tbl[idx].tte.smac[3], 
                   fa2_drv_tunnel_tbl[idx].tte.smac[4], 
                   fa2_drv_tunnel_tbl[idx].tte.smac[5]);
        }
    }

    return(r);
}

/*
 * Function   : fa2_callback
 * Description: Callback entry point into FA2 driver
 *
 * Returns    :
 *    Success : 0
 *    Failure : -1
 */
int fa2_callback(fa2_ioctl_t callback, unsigned long arg)
{
	int ret = FA2_SUCCESS;

	printk(KERN_DEBUG "%s: callback=0x%X, arg = %lu", __func__, callback, arg);

	switch (callback) {
	    case FA2CMDIOCTL_STATUS:
            switch (arg) {
                case FA2_GET_OPT_COUNTS:
                    break;
                case FA2_GET_OPT_CACHE_ENTRIES:
                    fa2_dump_flow_cache(-1, 0);
                    break;
                case FA2_GET_OPT_HW_ENTRIES:
                    fa2_dbg_get_fltable();
                    break;
                case FA2_GET_OPT_CACHE_AND_HW_ENTRIES:
                    fa2_dbg_get_fltable();
                    fa2_dump_flow_cache(-1, 0);
                    break;
                case FA2_GET_OPT_RTMAC:
                    ret = fa2_dbg_get_rtmac_tbl();
                    ret = fa2_dbg_get_tunl_tbl();
                    //ret = fa2_dbg_test_rtmac_tbl();
                    break;
                case FA2_GET_OPT_NHT:
                    ret = fa2_dbg_get_nh_tbl();
                    break;
                default:
                    printk(KERN_INFO "%s: Invalid arg %lu(0x%X) for "
                           "ioctlcmd %u", __func__, arg, (unsigned int)(arg), callback);
                    break;
            }
		    break;

	case FA2CMDIOCTL_RESET:
		//ret = fa2_reset_tables((enum fa2_tbl_num)arg);
		break;

	case FA2CMDIOCTL_INIT:
		ret = fa2_hw_init();
		break;

	case FA2CMDIOCTL_ENABLE:
		//ret = fa2_enable();
		break;
	case FA2CMDIOCTL_DISABLE:
		//ret = fa2_disable();
		break;

	case FA2CMDIOCTL_REGDUMP:
        fa2_dump_regs();
        ret = FA2_SUCCESS;
		break;

	case FA2CMDIOCTL_CFGLOG:
        if (arg <= FA2_DBG_MAX_LVL) {

            fa2_dbg_log_lvl = arg;

            printk(KERN_INFO "\n%s: Log level set to %u\n",
                   __func__, fa2_dbg_log_lvl);
        }
        else {
            printk(KERN_INFO "\nValid values are between %d and %d\n",
                   FA2_DBG_DEFAULT_LVL, FA2_DBG_MAX_LVL);
        }
		break;

	default:
		printk(KERN_INFO "%s: ERROR: callback(%d)", __func__, callback);
		ret = FA2_ERROR;
	}

	return ret;
}

/*
 * Function   : fa2_reset_init
 * Description: This function applies reset to the FA2 block and initializes it
 *              to default state
 * Returns    :
 *    Success : 0
 *    Failure : -1
 */
extern int fa2_reset_init(void)
{
	printk(KERN_DEBUG "Initializing FA2 Hardware Please wait...\n");

	if (fa2_callback(FA2CMDIOCTL_RESET, 0) == FA2_ERROR) {
		printk(KERN_DEBUG "%s ERROR: FA2_RESET\n", __FUNCTION__ );
		return FA2_ERROR;
	}
	if (fa2_callback(FA2CMDIOCTL_INIT, 0) == FA2_ERROR) {
		printk(KERN_DEBUG "%s ERROR: FA2_INIT\n",__FUNCTION__ );
		return FA2_ERROR;
	}
	if (fa2_callback(FA2CMDIOCTL_ENABLE, 0) == FA2_ERROR) {
		printk(KERN_DEBUG"%s ERROR: FA2_ENABLE", __FUNCTION__);
		return FA2_ERROR;
	}

	return FA2_SUCCESS;
}

static struct platform_driver iproc_nsp_fa2_driver = {
   .driver = {
      .name = "fa2",
      .owner = THIS_MODULE,
   },
   .probe   = iproc_nsp_fa2_probe,
   .remove  = NULL,
   .suspend = NULL,
   .resume  = NULL,
};

/*
 * Function   : fa2_load_init
 * Description: Module loading constructor.
 *
 * On module loading FA2 driver is registered.
 */
int __init fa2_load_init(void)
{
    int rc;

	printk(KERN_DEBUG "Registering FA2 Driver...\n");


    rc = platform_driver_register(&iproc_nsp_fa2_driver);
    if (rc < 0) {
        printk(KERN_ERR "%s: Driver registration failed, error %d\n",
               __func__, rc);
    }


	return 0;
}

/*
 * Function   : fa2_unload_exit
 * Description: Module unloading destructor.
 *      On module unloding the character driver is destroyed
 *      after FA2 hardware is disabled.
 */
void __exit fa2_unload_exit(void)
{
	printk(KERN_DEBUG "Unloading FA2 Driver...\n");

	/*
	 * Disable FA2 hardware
	 */
	fa2_disable();

	/*
	 * Delete the char driver
	 */
	fa2_drv_exit();

    fa2_initialized	= false;
}


/*
 * Function Name: fa2_open
 * Description  : Called when an user application opens this device.
 * Returns      : 0 - success
 */
static int fa2_open(struct inode *inode, struct file *fp)
{
	return FA2_SUCCESS;
}

EXPORT_SYMBOL(fa2_callback);

//TODO: enable later
//EXPORT_SYMBOL(fa2_reset_init);

module_init(fa2_load_init);
module_exit(fa2_unload_exit);

MODULE_DESCRIPTION(FA2_MODNAME);
MODULE_VERSION(FA2_VERSION);
MODULE_LICENSE("Proprietary");

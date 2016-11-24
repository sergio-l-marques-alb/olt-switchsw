/*
 * $Id: vlan.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:        vlan.c
 * Purpose: VLAN management
 */
#include <sal/types.h>
#include <sal/appl/io.h>

#include <shared/pbmp.h>
#include <shared/types.h>

#include <soc/types.h>
#include <soc/debug.h>
#include <soc/feature.h>
#include <soc/robo.h>
#include <soc/drv.h>
#include <soc/mcm/robo/memregs.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/vlan.h>
#include <bcm/stg.h>
#include <bcm/port.h>
#include <bcm/trunk.h>
#include <bcm/field.h>

#include <bcm_int/common/lock.h>
#include <bcm_int/robo/vlan.h>
#include <bcm_int/robo/port.h>

/*
 * The entire vlan_info structure is protected by BCM_LOCK.
 */

typedef struct bcm_robo_vlan_info_s {
    int         init;       /* TRUE if VLAN module has been inited */
    bcm_vlan_t  defl;       /* Default VLAN */
    vlist_t     *list;      /* Bitmap of existing VLANs */
    int         count;      /* Number of existing VLANs */
} bcm_robo_vlan_info_t;

static bcm_robo_vlan_info_t robo_vlan_info[BCM_MAX_NUM_UNITS];

#ifndef BCM_VLAN_VID_MAX
#define BCM_VLAN_VID_MIN        0
#define BCM_VLAN_VID_MAX        4095
#define BCM_VID_VALID_MIN        1
#define BCM_VID_VALID_MAX        4095
#endif
#define CHECK_INIT(unit)                    \
        if (!robo_vlan_info[unit].init)             \
        return BCM_E_INIT

#define CHECK_VID(unit, vid)                    \
        if ((vid) < BCM_VID_VALID_MIN || (vid) > BCM_VID_VALID_MAX) \
        return (BCM_E_PARAM)

#ifdef WAN_PORT_SUPPORT
#if defined(BCM_5397_A0)
#define WAN_PORT_NUMBER 7
#endif /* BCM_5397_A0 */
#endif /* WAN_PORT_SUPPORT */

#define VLAN_VTMODE_TRANSPATENT 0   /* vlan translation mode at traspatent */
#define VLAN_VTMODE_MAPPING     1   /* vlan translation mode at mapping */

/* 
 *  vlan_1q_entry_t is 8 bytes length structure, should be enough
 *      for further appilication to other Robo Chips
 */
typedef vlan_1q_entry_t   vlan_entry_t;

/*
 *  =============== Internal VLAN variable/macro =====================
 */

#if defined(BCM_53115)  
/* flag for keeping the init status on VLAN translation used CFP.
 *  - bcm53115 used only.
 */
int flag_vt_cfp_init = FALSE;

/* to avoid CFP create action for the Field API init sequence is not proceeded
 * yet.
 */
static  int flag_skip_cfp_create = FALSE;

/* keep the information about the created group_id and entry_id in CFP 
 * when the iDT_Mode enabled.
 */
drv_idt_mode_cfp_info_t     idt_mode_cfp_info;

/* database to represent the created VLAN translation entry relation in CFP */
drv_vt_cfp_db_t             vt_cfp_db;

static sal_mutex_t vt_cfp_mutex = NULL;

#define VT_CFP_LOCK     \
            sal_mutex_take(vt_cfp_mutex, sal_mutex_FOREVER)
#define VT_CFP_UNLOCK   \
            sal_mutex_give(vt_cfp_mutex)

#define DRV_EVR_CFP_PORT_FLAG_ISP       0x0
#define DRV_EVR_CFP_PORT_FLAG_NONEISP   0x1

#endif  /* BCM_53115 */
#ifdef LVL7_FIXUP
#define BCM_EVR_OP_FLAG_MASK    0x0F
#define BCM_EVR_OP_FLAG_ASIS    0x01
#define BCM_EVR_OP_FLAG_ASRX    0x02
#define BCM_EVR_OP_FLAG_REMOVE  0x04
#define BCM_EVR_OP_FLAG_MODIFY  0x08

#define CHECK_OP(op) \
        if (((op) & BCM_EVR_OP_FLAG_MASK) <= 0) { \
          return BCM_E_PARAM;                    \
        }                                        
#endif
/*
 *  =============== Internal VLAN routines =====================
 */
#if defined(BCM_53115)  
/* 
 *  Function : _vtcfp_db_dump
 *      - routine for the debug usage to dump the SW database about VT-CFP.
 */
void _vtcfp_db_dump(void){
    int i,start=0,this;
    drv_vt_cfp_entry_db_t ent;
    
    soc_cm_print("\n---- SW_VTCFP_DB information ----\n");
    start = vt_cfp_db.vt_cfp_db_start_id;
    soc_cm_print(">> .valid_entry_count=%d, .start_entry_id=%d\n\n",
        vt_cfp_db.vt_cfp_db_valid_count, start);
    if (start == VTCFP_NULL_INDEX)    return;
    this = start;
    
    for (i=0; i<vt_cfp_db.vt_cfp_db_valid_count; i++){
        ent = vt_cfp_db.vt_cfp_entry_db[this];
        soc_cm_print("%d.ent[%d]{vid=%d,vt_mode=%d,nni_cfpid=%d,uni_cfp_id=%d}{prev=%d,next=%d}\n",
            i+1, this, ent.vid,ent.vt_mode,ent.nni_field_id,ent.uni_field_id,ent.prev,ent.next);
        this = ent.next;
        if (IS_VALID_VTCFP_DB_ENTRY_ID(this)){
            soc_cm_print("......\n");
        } else {
            soc_cm_print("==== END ====\n\n");
        }
    }
}
#endif    /* BCM_53115 */  
 
/* =============== VT_CFP section START =================== */
/*
 *  Function : _bcm_robo_vlan_vtcfp_vid_search
 *      - search if a entry with given vid is existed in VT_CFP sw database.
 *
 *  Parameter :
 *      - vtcfp_db_id  : the target entry_id(sw) if existed. the most close
 *                  entry_id(sw) if not existed.
 *      - free_id   : the first found free entry_id(sw). if no entry been free
 *                  this value will = DRV_EVRID_VT_SW_MAX_CNT.
 *                  (the free_entry.next and free_entry.prev will be "-1")
 *  Return :
 *      TRUE(1)     : search result is existed.
 *      FALSE(0)    : search result is not existed. 
 *  Note : 
 *  1. return 0(False) is not found. and 
 *      a. vtcfp_db_id = VTCFP_SUPPORT_VLAN_CNT when the table on this port
 *          is full. else
 *      b. vtcfp_db_id = (valid entry index) to point to the most close item 
 *          in this sorted table on this port. The real case for the search 
 *          result might be one of below:
 *          - (vtcfp_db_id).vid > in_vid(not full yet and this entry_id 
 *              indicating the fisrt entry within vid large than given vid
 *          - (vtcfp_db_id).vid < in_vid(not full yet and all exist entries' 
 *              vid are smaller than vid. 
 *      c. vtcfp_db_id = -1. no entry created on this port.
 *  2. if entry is found, the vtcfp_db_id indicate the match one(invid). 
 *
 */
int _bcm_robo_vlan_vtcfp_vid_search(int unit, bcm_vlan_t in_vid, 
                int *vtcfp_db_id, int *free_id){
#if defined(BCM_53115)  
    int     sw_db_head;
    int     i, found = FALSE;
    bcm_vlan_t  temp_vid;
    
    drv_vt_cfp_entry_db_t   *temp_vtcfp_db_entry;     
#endif    
    
    if (!SOC_IS_ROBO53115(unit)){
        return FALSE;
    }
#if defined(BCM_53115)  
    /* check if VID is valid */
    if (in_vid < BCM_VID_VALID_MIN || in_vid > BCM_VID_VALID_MAX){
        soc_cm_debug(DK_VLAN, "%s: invalid VID=%d\n", __func__, in_vid);
        return FALSE;
    }
    
    VT_CFP_LOCK;

    sw_db_head = vt_cfp_db.vt_cfp_db_start_id;
    
    /* check if sw_db is empty */
    if (vt_cfp_db.vt_cfp_db_valid_count == 0){
        *vtcfp_db_id = -1;
        *free_id = 0;
        VT_CFP_UNLOCK;
        return FALSE;
    }
    
    for (i = sw_db_head; i < VTCFP_SUPPORT_VLAN_CNT; 
                    i = temp_vtcfp_db_entry->next){
        
        temp_vtcfp_db_entry = vt_cfp_db.vt_cfp_entry_db + i;
        *vtcfp_db_id = i;
        
        temp_vid = temp_vtcfp_db_entry->vid;
        if (temp_vid == in_vid){
            found = TRUE;
            break;
        } else if(temp_vid > in_vid) {
            found = FALSE;
            break;
        }
    }
    
    /* get the free entry index */
    *free_id = -1;
    for (i = 0; i < VTCFP_SUPPORT_VLAN_CNT; i++){
        temp_vtcfp_db_entry = vt_cfp_db.vt_cfp_entry_db + i;
        /* next = -1 and prev = -1 means this node is free */
        if ((temp_vtcfp_db_entry->next == -1) && 
                    (temp_vtcfp_db_entry->prev == -1)){
            *free_id = i;
            break;
        }
    }
    
    /* check if sw_db is full */
    if (!found){
        if (vt_cfp_db.vt_cfp_db_valid_count == VTCFP_SUPPORT_VLAN_CNT){
            *vtcfp_db_id = VTCFP_SUPPORT_VLAN_CNT;
            *free_id = VTCFP_SUPPORT_VLAN_CNT;
        }
    }
       
    VT_CFP_UNLOCK;
    return found;
#else    /* bcm53115 */
    return FALSE;
#endif   /* bcm53115 */
}

#if defined(BCM_53115)  
/*
 *  Function : _bcm_robo_vlan_vtcfp_sw_db_update
 *      - update the VT_CFP sw database for different operation.
 *  Parmeter :
 *      op      :   insert | delete | reset
 *      vid     :   vid
 *      nni_field_id : field entry id for NNI
 *      uni_field_id : field entry id for UNI
 *      vt_mode :   mapping | trasparent
 *      fast_id :   the most closed index
 *      this_id :   the operating index
 *      
 *  Note : 
 *
 */
void _bcm_robo_vlan_vtcfp_sw_db_update(int op, bcm_vlan_t  ori_vid, 
                    int nni_field_id, int uni_field_id,
                    int vt_mode, int fast_id, int this_id){
    int temp_id;
    
    if (!(IS_VALID_VTCFP_DB_ENTRY_ID(fast_id))){
        if (fast_id != -1) {
            soc_cm_debug(DK_ERR, "%s: invalid case on fast_id \n", __func__);
            return;
        }
    }

    if (!(IS_VALID_VTCFP_DB_ENTRY_ID(this_id))){
        soc_cm_debug(DK_ERR, "%s: invalid case on this_id \n", __func__);
        return;   
    }
    
    if (!flag_vt_cfp_init){
        soc_cm_debug(DK_VLAN, "%s: VT_CFP is not init!\n", __func__);
        return ;
    }
    
    soc_cm_debug(DK_VLAN, 
            "%s,%d,Start(op=%d,vid=%d,fast_id=%d,this_id=%d)!\n", 
            __func__,__LINE__,op, ori_vid, fast_id, this_id);
    switch(op){
    case VTCFP_SWDB_OP_FLAG_INSERT :
        if (fast_id == VTCFP_NULL_INDEX){     /* means the first node */
            vt_cfp_db.vt_cfp_db_start_id = this_id;
            vt_cfp_db.vt_cfp_entry_db[this_id].prev = VTCFP_NULL_INDEX;
            vt_cfp_db.vt_cfp_entry_db[this_id].next = 
                            VTCFP_SUPPORT_VLAN_CNT;
        } else {
            /* insert to the front of fast_id node */
            if (vt_cfp_db.vt_cfp_entry_db[fast_id].vid > ori_vid){
                /* check if head */
                if (vt_cfp_db.vt_cfp_entry_db[fast_id].prev == 
                                VTCFP_NULL_INDEX){
                    /* insert to the head */
                    vt_cfp_db.vt_cfp_db_start_id = this_id;
                    
                    vt_cfp_db.vt_cfp_entry_db[this_id].prev = 
                                    VTCFP_NULL_INDEX;
                    vt_cfp_db.vt_cfp_entry_db[this_id].next = fast_id;
                    
                    vt_cfp_db.vt_cfp_entry_db[fast_id].prev = this_id;
                } else {
                    /* insert to normal */
                    temp_id = vt_cfp_db.vt_cfp_entry_db[fast_id].prev;
                    
                    vt_cfp_db.vt_cfp_entry_db[temp_id].next = this_id;
                    
                    vt_cfp_db.vt_cfp_entry_db[this_id].prev = temp_id;
                    vt_cfp_db.vt_cfp_entry_db[this_id].next = fast_id;
                    
                    vt_cfp_db.vt_cfp_entry_db[fast_id].prev = this_id;
                    
                }
            } else {    /* insert to the end of fast_id node */
                temp_id = vt_cfp_db.vt_cfp_entry_db[fast_id].next;
            
                vt_cfp_db.vt_cfp_entry_db[fast_id].next = this_id;
                
                vt_cfp_db.vt_cfp_entry_db[this_id].prev = fast_id;
                vt_cfp_db.vt_cfp_entry_db[this_id].next = temp_id;
                
                if (temp_id != VTCFP_SUPPORT_VLAN_CNT){
                    /* this case should not be happened */
                    soc_cm_debug(DK_ERR, 
                            "%s: invalid case on fast_id=%d \n", 
                            __func__, fast_id);
                    vt_cfp_db.vt_cfp_entry_db[temp_id].prev = this_id;
                }
            }
        }
        vt_cfp_db.vt_cfp_entry_db[this_id].vid = ori_vid;
        vt_cfp_db.vt_cfp_entry_db[this_id].vt_mode = vt_mode;
        vt_cfp_db.vt_cfp_entry_db[this_id].nni_field_id = nni_field_id;
        vt_cfp_db.vt_cfp_entry_db[this_id].uni_field_id = uni_field_id;
        
        (vt_cfp_db.vt_cfp_db_valid_count)++;
        break;
    case VTCFP_SWDB_OP_FLAG_DELETE : 
        /* the first node */
        if (vt_cfp_db.vt_cfp_entry_db[this_id].prev == VTCFP_NULL_INDEX){ 
            temp_id = vt_cfp_db.vt_cfp_entry_db[this_id].next;
            vt_cfp_db.vt_cfp_db_start_id = temp_id;
            
            vt_cfp_db.vt_cfp_entry_db[temp_id].prev = VTCFP_NULL_INDEX;
        
        /* the last node */
        } else if(vt_cfp_db.vt_cfp_entry_db[this_id].next == 
                        VTCFP_SUPPORT_VLAN_CNT){
            temp_id = vt_cfp_db.vt_cfp_entry_db[this_id].prev;
            vt_cfp_db.vt_cfp_entry_db[temp_id].next = VTCFP_SUPPORT_VLAN_CNT;                
        
        /* normal node */
        } else {  
            temp_id = vt_cfp_db.vt_cfp_entry_db[this_id].prev;
            vt_cfp_db.vt_cfp_entry_db[temp_id].next = 
                        vt_cfp_db.vt_cfp_entry_db[this_id].next;
                        
            temp_id = vt_cfp_db.vt_cfp_entry_db[this_id].next;
            vt_cfp_db.vt_cfp_entry_db[temp_id].prev = 
                        vt_cfp_db.vt_cfp_entry_db[this_id].prev;
        }
        vt_cfp_db.vt_cfp_entry_db[this_id].vid = 0;
        vt_cfp_db.vt_cfp_entry_db[this_id].vt_mode = 0;
        vt_cfp_db.vt_cfp_entry_db[this_id].nni_field_id = VTCFP_NULL_INDEX;
        vt_cfp_db.vt_cfp_entry_db[this_id].uni_field_id = VTCFP_NULL_INDEX;
        vt_cfp_db.vt_cfp_entry_db[this_id].next = VTCFP_NULL_INDEX;
        vt_cfp_db.vt_cfp_entry_db[this_id].prev = VTCFP_NULL_INDEX;
        
        (vt_cfp_db.vt_cfp_db_valid_count)--;
        break;
    case VTCFP_SWDB_OP_FLAG_RESET :
        /* -------- TBD -------- */
        break;
    default :
        break;
    }
    
    if (soc_cm_debug_check(DK_VLAN)){
        _vtcfp_db_dump();
    }
    soc_cm_debug(DK_VLAN, "%s,%d,Done!\n", __func__,__LINE__);
}
#endif    
    
/*
 *  Function : _bcm_robo_vlan_vtcfp_create
 *      - to create a field entry for device basis VLAN translation.
 *
 *  Parameters :
 *      - ori_vid : original VID.
 *      - vt_mode : vlan translation mode (mapping/transparent).
 *  Note : 
 */
int _bcm_robo_vlan_vtcfp_create(int unit, bcm_vlan_t ori_vid, 
                    bcm_vlan_t new_vid, int vt_mode){
                    
    int rv = BCM_E_NONE;
    
#if defined(BCM_53115)  
    bcm_port_t  port = 0;
    uint32      temp32 = 0;
    pbmp_t      nni_bmp, uni_bmp, pbm_mask;
    int         vtcfp_db_id, idt_mode;
    int         evr_entry_id = 0, free_id;
    
    bcm_field_group_t cfp_vt_group;
    bcm_field_entry_t nni_cfp_vt_entry, uni_cfp_vt_entry;


    if (!SOC_IS_ROBO53115(unit)){
        return BCM_E_UNAVAIL;
    }
    
    soc_cm_debug(DK_VLAN, "%s, ori_vid=%d, new_vid=%d,vt_mode=%d!\n",
            __func__,ori_vid, new_vid,vt_mode);
    /* 1. check if iDT_Mode enabled, and retrive the NNI/UNI portbitmap */
    (DRV_SERVICES(unit)->vlan_property_get)
                    (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE,
                     (uint32 *) &idt_mode);
    if (!idt_mode){
        soc_cm_debug(DK_VERBOSE | DK_VLAN, "%s, not in iDT_Mode!\n",__func__);
        return BCM_E_CONFIG;
    }
    BCM_PBMP_CLEAR(nni_bmp);
    BCM_PBMP_CLEAR(uni_bmp);
    BCM_PBMP_CLEAR(pbm_mask);
    
    port = ~port;   /* means get the device basis value */
    BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->vlan_prop_port_enable_get)
                    (unit, DRV_VLAN_PROP_ISP_PORT, 
                    port,  &temp32));
    SOC_PBMP_WORD_SET(nni_bmp, 0, temp32);

    BCM_PBMP_ASSIGN(pbm_mask, PBMP_ALL(unit));
    BCM_PBMP_ASSIGN(uni_bmp, PBMP_PORT_ALL(unit));
    BCM_PBMP_REMOVE(uni_bmp, nni_bmp);
    
    VT_CFP_LOCK;
    /* 2. search vid if existed :
     *  a. if exist, should return by error code.(check esw for detail)
     *  b. if no exist, 
     *      - if entry full, return by error code!
     *      - else accept the creating request.
     *  c. search result can retrive the most close node for this insertion.
     */
    vtcfp_db_id = -1;
    if (_bcm_robo_vlan_vtcfp_vid_search(unit, ori_vid, 
                    &vtcfp_db_id, &free_id)){
        
        VT_CFP_UNLOCK;
        return BCM_E_EXISTS;
    } else {
        if (IS_VTCFP_DB_FULL){
            VT_CFP_UNLOCK;
            return BCM_E_FULL;
        }
    }
     
    /* 3. if creating request was accept, create EVR etnry.
     *  a. if EVR can be created, retrive the EVR entry_id.
     *  b. else return the returned error code.
     */
    rv = ((DRV_SERVICES(unit)->vlan_vt_add)
                (unit, DRV_VLAN_XLAT_EVR, 0, ori_vid, new_vid, 
                0, vt_mode));
    if (rv){
        VT_CFP_UNLOCK;
        soc_cm_debug(DK_WARN, 
                    "%s: faild on add VT entry with vid=%d !!\n", 
                    __func__, ori_vid);
        return rv;
    }
    
    rv = ((DRV_SERVICES(unit)->vlan_property_get)
                (unit, DRV_VLAN_PROP_EVR_VT_NEW_ENTRY_ID,
                 (uint32 *) &evr_entry_id));
    if (rv){
        VT_CFP_UNLOCK;
        soc_cm_debug(DK_WARN, 
                    "%s: faild on get created VT entry-=id for vid=%d !!\n", 
                    __func__, ori_vid);
        return rv;
    }
    if (evr_entry_id == 0){
        soc_cm_debug(DK_ERR, 
                    "%s,%d, invalid Classification ID!\n",__func__,__LINE__);
    }
     
    /* 4. create the field entry and assing the qualifier based on NNI/UNI
     *      ports. That means there are 2 entries will be created for this 
     *      VLAN translation.
     *  a. if both field entries can't be created properly, return the error 
     *      code and remove the related created EVR table also. Than return 
     *      error code.
     *  b. else assign the EVR_id to the action and install CFP entries.
     *      - if both field entries can't be installed properly, remvoe the 
     *          installed entries and remove the created EVR entries.
     */
    cfp_vt_group = idt_mode_cfp_info.vt_cfp_group_id;

    /* for NNI ports */
    rv = bcm_field_entry_create(unit, cfp_vt_group, &nni_cfp_vt_entry);
    if (rv < 0) {
        soc_cm_debug(DK_WARN, 
                "%s:rv=%d,faild on creating field entry for NNI ports!!\n", 
                __func__,rv);
        
        goto vt_cfp_error_action;
    }
    rv = bcm_field_entry_prio_set(unit, nni_cfp_vt_entry, 
                BCM_FIELD_GROUP_PRIO_ANY);
    if (rv < 0) {
        soc_cm_debug(DK_WARN, 
                "%s: rv=%d,faild on set field entry priority!!\n", 
                __func__,rv);
        goto vt_cfp_error_action;
    }
    rv = bcm_field_qualify_InPorts
                (unit, nni_cfp_vt_entry, nni_bmp, pbm_mask);
    rv |= bcm_field_qualify_OuterVlan
                (unit, nni_cfp_vt_entry, ori_vid, BCM_VLAN_VID_MAX);
    if (rv < 0) {
        soc_cm_debug(DK_WARN, 
                    "%s: faild on set field qualify!!\n", 
                    __func__);
        goto vt_cfp_error_action;
    }
    
    /* for UNI ports */
    rv = bcm_field_entry_create(unit, cfp_vt_group, &uni_cfp_vt_entry);
    if (rv < 0) {
        soc_cm_debug(DK_WARN, 
                    "%s: faild on creating field entry for NNI ports !!\n", 
                    __func__);
        
        goto vt_cfp_error_action;
    }
    rv = bcm_field_entry_prio_set(unit, uni_cfp_vt_entry, 
                BCM_FIELD_GROUP_PRIO_ANY);
    if (rv < 0) {
        soc_cm_debug(DK_WARN, 
                    "%s: rv=%d,faild on set field entry priority!!########\n", 
                    __func__,rv);
        goto vt_cfp_error_action;
    }
    rv = bcm_field_qualify_InPorts
                (unit, uni_cfp_vt_entry, uni_bmp, pbm_mask);
    rv |= bcm_field_qualify_InnerVlan
                (unit, uni_cfp_vt_entry, ori_vid, BCM_VLAN_VID_MAX);
    if (rv < 0) {
        soc_cm_debug(DK_WARN, 
                    "%s: faild on set field qualify!!\n", 
                    __func__);
        goto vt_cfp_error_action;
    }

    /* field entries installing */
    rv = bcm_field_action_add(unit, nni_cfp_vt_entry, 
                bcmFieldActionNewClassId, evr_entry_id, 0);
    rv |= bcm_field_action_add(unit, uni_cfp_vt_entry, 
                bcmFieldActionNewClassId, evr_entry_id, 0);
    rv |= bcm_field_counter_create(unit, nni_cfp_vt_entry);
    rv |= bcm_field_counter_create(unit, uni_cfp_vt_entry);
    rv |= bcm_field_entry_install(unit, nni_cfp_vt_entry);
    rv |= bcm_field_entry_install(unit, uni_cfp_vt_entry);
    if (rv < 0) {
        soc_cm_debug(DK_WARN, 
                    "%s: rv=%d,faild on installing field entry!!\n", 
                    __func__, rv);
        goto vt_cfp_error_action;
    }
        
    /* 5. maintain the VT_CFP database. */
    _bcm_robo_vlan_vtcfp_sw_db_update(VTCFP_SWDB_OP_FLAG_INSERT, ori_vid, 
                    nni_cfp_vt_entry, uni_cfp_vt_entry, 
                    vt_mode, vtcfp_db_id, free_id);
    
    VT_CFP_UNLOCK;
    return BCM_E_NONE;

vt_cfp_error_action :
    /* remove the created EVR entry */
    if((DRV_SERVICES(unit)->vlan_vt_delete)
                (unit, DRV_VLAN_XLAT_EVR, 0, ori_vid)){
        soc_cm_debug(DK_WARN, 
                    "%s: faild on remove VT entry with vid=%d !!\n", 
                    __func__, ori_vid);
    }
    VT_CFP_UNLOCK;
    
#endif  /* BCM_53115 */
    return rv;
}

/*
 *  Function : _bcm_robo_vlan_vtcfp_delete
 *      - to create a field entry  
 *
 *  Parameters :
 *      - entry_id : created field entry id.
 *      - isp_flag : flag to indication ISP/None-ISP 
 *      - c_id : means classification ID on indicating to EVR table entry.
 *  Note : 
 *  1. init mem and the sw database.
 */
int _bcm_robo_vlan_vtcfp_delete(int unit, bcm_vlan_t ori_vid, int vt_mode){
                    
#if defined(BCM_53115)  
    int         vtcfp_db_id, free_id, idt_mode;
    int         temp_vtcfp_entry_id;
    drv_vt_cfp_entry_db_t   *temp_vtcfp_ent;
#endif  /* BCM_53115 */
    int rv = BCM_E_NONE;
    
    if (!SOC_IS_ROBO53115(unit)){
        return BCM_E_UNAVAIL;
    }
#if defined(BCM_53115)  
   
    soc_cm_debug(DK_VLAN, 
            "%s,%d,Start(vid=%d, vt_mode=%d)!\n", 
            __func__,__LINE__, ori_vid, vt_mode);
    
    /* 1. check if iDT_Mode enabled */
    (DRV_SERVICES(unit)->vlan_property_get)
                    (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE,
                     (uint32 *) &idt_mode);
    if (!idt_mode){
        soc_cm_debug(DK_VLAN, "%s,%d not in iDT_Mode!\n",__func__,__LINE__);
        return BCM_E_CONFIG;
    }
    
    VT_CFP_LOCK;
    /* 2. search vt_cfp entry */
    vtcfp_db_id = -1;
    if ((_bcm_robo_vlan_vtcfp_vid_search(unit, ori_vid, 
                    &vtcfp_db_id, &free_id)) == FALSE){
        
        VT_CFP_UNLOCK;
        return BCM_E_NOT_FOUND;
    } else {
        if (!IS_VALID_VTCFP_DB_ENTRY_ID(vtcfp_db_id)){
            VT_CFP_UNLOCK;
            return BCM_E_INTERNAL;
        }
    }
    temp_vtcfp_ent = vt_cfp_db.vt_cfp_entry_db + vtcfp_db_id;
    
    soc_cm_debug(DK_VLAN,"Processing delete action!\n"); 
    if (soc_cm_debug_check(DK_VLAN)){
        _vtcfp_db_dump();
    }
    /* 3. check vt_mode, return not found if vt_mode not match */
    if (temp_vtcfp_ent->vt_mode != vt_mode){
        
        VT_CFP_UNLOCK;
        return BCM_E_NOT_FOUND;
    }
    
    /* 4. remove vt_cfp entry (sw DB and device mem)
     *   - delete NNI / UNI entries
     */
    temp_vtcfp_entry_id = temp_vtcfp_ent->nni_field_id;
    rv = bcm_field_entry_destroy(unit, temp_vtcfp_entry_id);
    if (rv){
        VT_CFP_UNLOCK;
        soc_cm_debug(DK_WARN, 
                "%s: faild on remove NNU used CFP entry with id=%d !!\n", 
                __func__, temp_vtcfp_entry_id);
        return rv;
    }
    temp_vtcfp_entry_id = temp_vtcfp_ent->uni_field_id;
    rv = bcm_field_entry_destroy(unit, temp_vtcfp_entry_id);
    if (rv){
        VT_CFP_UNLOCK;
        soc_cm_debug(DK_WARN, 
                "%s: faild on remove UNI used CFP entry with id=%d !!\n", 
                __func__, temp_vtcfp_entry_id);
        return rv;
    }
    
    /* 5. maintain vt_cfp sw database */
    _bcm_robo_vlan_vtcfp_sw_db_update(VTCFP_SWDB_OP_FLAG_DELETE, ori_vid, 
                    0, 0, vt_mode, 0, vtcfp_db_id);
    
    /* 6. remove associated EVR entries on all ports */
    rv = ((DRV_SERVICES(unit)->vlan_vt_delete)
                (unit, DRV_VLAN_XLAT_EVR, 0, ori_vid));
    if (rv){
        VT_CFP_UNLOCK;
        soc_cm_debug(DK_WARN, 
                "%s: faild to remove associated VT entroies at vid=%d!!\n", 
                __func__, ori_vid);
        return rv;
    }
    
    VT_CFP_UNLOCK;
    soc_cm_debug(DK_VLAN, 
            "%s,%d,Done!\n", __func__,__LINE__);
#endif  /* BCM_53115 */
    return rv;
}

/*
 *  Function : _bcm_robo_vlan_vtcfp_delete_all
 *      - to create a field entry  
 *
 *  Parameters :
 *      - entry_id : created field entry id.
 *      - isp_flag : flag to indication ISP/None-ISP 
 *      - c_id : means classification ID on indicating to EVR table entry.
 *  Note : 
 *  1. init mem and the sw database.
 */
int _bcm_robo_vlan_vtcfp_delete_all(int unit, int vt_mode){
                    
#if defined(BCM_53115)  
    int         vtcfp_db_id;
    int         temp_vtcfp_entry_id, temp_next;
    bcm_vlan_t  temp_vid;
    drv_vt_cfp_entry_db_t   *temp_vtcfp_ent;
#endif  /* BCM_53115 */
    int rv = SOC_E_NONE;
    
    if (!SOC_IS_ROBO53115(unit)){
        return BCM_E_UNAVAIL;
    }
#if defined(BCM_53115)  

    /* 1. No check about iDT_Mode in this routine */

    VT_CFP_LOCK;
    soc_cm_debug(DK_VLAN, 
            "%s,%d,Start!\n", __func__,__LINE__);
    /* 2. check if vt_cfp is empty */
    if (vt_cfp_db.vt_cfp_db_valid_count == 0){
        VT_CFP_UNLOCK;
        return BCM_E_NONE;
    }

    /* 3. loop on each vt_cfp entry and remove if vt_mode is match 
     *  - delete NNI / UNI entries
     */
    temp_next = VTCFP_SUPPORT_VLAN_CNT;
    for (vtcfp_db_id = vt_cfp_db.vt_cfp_db_start_id; 
            vtcfp_db_id < VTCFP_SUPPORT_VLAN_CNT; 
            vtcfp_db_id = temp_next){

        /* vtcfp_db_id could be -1 when reach the latest node. */        
        if (!IS_VALID_VTCFP_DB_ENTRY_ID(vtcfp_db_id)){
            break;
        }

        temp_vtcfp_ent = vt_cfp_db.vt_cfp_entry_db + vtcfp_db_id;
        
        if (temp_vtcfp_ent->vt_mode == vt_mode){
            temp_vtcfp_entry_id = temp_vtcfp_ent->nni_field_id;
            rv = bcm_field_entry_destroy(unit, temp_vtcfp_entry_id);
            if (rv == BCM_E_NOT_FOUND){
                /* in init routine on calling this funciton, such case might 
                 * be happened.
                 */
                rv = BCM_E_NONE;
            }
            temp_vtcfp_entry_id = temp_vtcfp_ent->uni_field_id;
            rv = bcm_field_entry_destroy(unit, temp_vtcfp_entry_id);
            if (rv == BCM_E_NOT_FOUND){
                /* in init routine on calling this funciton, such case might 
                 * be happened.
                 */
                rv = BCM_E_NONE;
            }
            
            temp_next = temp_vtcfp_ent->next;
            temp_vid = temp_vtcfp_ent->vid;
            /* 4. maintain vt_cfp sw database */
            _bcm_robo_vlan_vtcfp_sw_db_update(VTCFP_SWDB_OP_FLAG_DELETE, 
                            temp_vid, 
                            0, 0, vt_mode, 0, vtcfp_db_id);
                            
            /* 5. remove associated EVR entries on all ports */
            rv = ((DRV_SERVICES(unit)->vlan_vt_delete)
                    (unit, DRV_VLAN_XLAT_EVR, 0, temp_vid));
            if (rv){
                VT_CFP_UNLOCK;
                soc_cm_debug(DK_WARN, 
                        "%s: faild to remove VT entroies at vid=%d!!\n", 
                        __func__, temp_vid);
                return rv;
            }
        }
    }
    
    VT_CFP_UNLOCK;
    soc_cm_debug(DK_VLAN, 
            "%s,%d,Done!\n", __func__,__LINE__);
#endif  /* BCM_53115 */
    return rv;
}


/* 
 * Function:
 *  _bcm_robo_vlan_vtcfp_init
 * 
 * Purpose:
 *  init process on CFP for VLAN translation usage.
 *  1. create one default CFP entry for iDT_Mode on ISP/None-ISP port.
 *      - field group and entry create
 *  2. create the default CFP group for VLAN translation.
 *      - field group created only
 *  3. init VT_CFP sw database.
 * 
 * Note : 
 *  1. this is for bcm53115 only currently.
 */
int _bcm_robo_vlan_vtcfp_init(int unit){

#if defined(BCM_53115)  
    int     i, rv = BCM_E_NONE;

    bcm_field_qset_t qset;
    bcm_field_group_t group;

    
    if (!SOC_IS_ROBO53115(unit)){
        return BCM_E_UNAVAIL;
    }

    /* mutex create */
    if (vt_cfp_mutex != NULL) {
        sal_mutex_destroy(vt_cfp_mutex);
        vt_cfp_mutex = NULL;
    }
    vt_cfp_mutex = sal_mutex_create("EGR_VLAN_XLATE");
    if (vt_cfp_mutex == NULL) {
        return SOC_E_INTERNAL;
    }
    
    /* clear the sw info */
    sal_memset(&idt_mode_cfp_info, 0, sizeof(idt_mode_cfp_info));
    
    if (IS_VT_CFP_INIT){
        /* if current init status is existed, reset all exsiting VT_CFP 
         *  - clear by VT_MODE
         */
        rv = _bcm_robo_vlan_vtcfp_delete_all(
                        unit, VLAN_VTMODE_TRANSPATENT);
        rv |= _bcm_robo_vlan_vtcfp_delete_all(
                        unit, VLAN_VTMODE_MAPPING);
        if (rv){
            soc_cm_debug(DK_WARN, 
                    "%s: rv=%d,faild on reset the existing VT entries!!\n", 
                    __func__,rv);
            return BCM_E_INIT;
        }

        soc_cm_debug(DK_VLAN, 
                "%s: VT_CFP init flag reset again!\n", 
                __func__);
        flag_vt_cfp_init = FALSE;   /* reset the init flag */
    } else {
        
        /* processing the VT related CFP configuration */
        
        if (flag_skip_cfp_create) {
            flag_vt_cfp_init = FALSE;
        } else {
           
            /* 1. create CFP group for VLAN translation. 
             *  - qualify field at port, S-Tag and C-Tag.
             *  - no entry been created in this section.
             */
            BCM_FIELD_QSET_INIT(qset);
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlan);
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInnerVlan);
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyClassId);
            rv = bcm_field_group_create(unit, qset, 
                BCM_FIELD_GROUP_PRIO_ANY, &group);
            if (rv < 0) {
                return rv;
            }
    
            /* 2. save the cfp info */
            idt_mode_cfp_info.vt_cfp_group_id = group;
            
            /* 3. set CFP init flag */
            flag_vt_cfp_init = TRUE;
            
        }
    }

    /* 4. init VT_CFP_DB */
    sal_memset(&vt_cfp_db, 0, sizeof(drv_vt_cfp_db_t));
    vt_cfp_db.vt_cfp_db_start_id = VTCFP_NULL_INDEX;
    for (i = 0; i < VTCFP_SUPPORT_VLAN_CNT; i++){
        vt_cfp_db.vt_cfp_entry_db[i].nni_field_id = VTCFP_NULL_INDEX;
        vt_cfp_db.vt_cfp_entry_db[i].uni_field_id = VTCFP_NULL_INDEX;
        vt_cfp_db.vt_cfp_entry_db[i].prev = VTCFP_NULL_INDEX;
        vt_cfp_db.vt_cfp_entry_db[i].next = VTCFP_NULL_INDEX;
    }
    vt_cfp_db.vt_cfp_db_valid_count = 0;
    
#endif  /* BCM_53115 */

    return BCM_E_NONE;
}
    

/*
 * Function:
 *  _bcm_robo_vlan_vtcfp_isp_change
 * 
 * Purpose:
 *  rebuild the CFP entry when isp/none-isp port changed.
 * 
 * Note : 
 *  1. this is for bcm53115 only currently.
 */
int _bcm_robo_vlan_vtcfp_isp_change(int unit){
    
#if defined(BCM_53115)  
    if (SOC_IS_ROBO53115(unit)){
        bcm_port_t  port = 0;
        uint32      temp32;
        pbmp_t      nni_bmp, uni_bmp, pbm_mask;
        int         vtcfp_cnt, vtcfp_head, i, rv, idt_mode;
        drv_vt_cfp_entry_db_t   *temp_vtcfp_ent;
        
        bcm_field_entry_t   entry;
        
        soc_cm_debug(DK_VLAN, "%s, VTCFP rebuild!\n",__func__);
        /* check iDT_Mode first */
        (DRV_SERVICES(unit)->vlan_property_get)
                        (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE,
                         (uint32 *) &idt_mode);
        if (!idt_mode){
            soc_cm_debug(DK_VLAN, "%s, not in iDT_Mode!\n",__func__);
            return BCM_E_NONE;
        }
        /* get the ISP port bitmap */
        BCM_PBMP_CLEAR(nni_bmp);
        BCM_PBMP_CLEAR(uni_bmp);
        BCM_PBMP_CLEAR(pbm_mask);
        
        port = ~port;   /* means get the device basis value */
        BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->vlan_prop_port_enable_get)
                        (unit, DRV_VLAN_PROP_ISP_PORT, 
                        port,  &temp32));
        SOC_PBMP_WORD_SET(nni_bmp, 0, temp32);

        BCM_PBMP_ASSIGN(pbm_mask, PBMP_PORT_ALL(unit));
        BCM_PBMP_ASSIGN(uni_bmp, PBMP_PORT_ALL(unit));
        BCM_PBMP_REMOVE(uni_bmp, nni_bmp);
        
        VT_CFP_LOCK;
        /* get all the created CFP entry_id to rebuild */
        vtcfp_cnt = vt_cfp_db.vt_cfp_db_valid_count;
        vtcfp_head = vt_cfp_db.vt_cfp_db_start_id;
        for(i = vtcfp_head; IS_VALID_VTCFP_DB_ENTRY_ID(i); 
                        i = temp_vtcfp_ent->next){
            
            temp_vtcfp_ent = vt_cfp_db.vt_cfp_entry_db + i;
            /* for NNI ports */
            entry = temp_vtcfp_ent->nni_field_id;
            if ((rv = (bcm_field_qualify_InPorts(
                    unit, entry, nni_bmp, pbm_mask))) != BCM_E_NONE){
                soc_cm_debug(DK_VLAN, 
                        "%s,%d,(err=%d) failed on field(new InPorts)!\n",
                        __func__, __LINE__, rv);
            }
            if ((rv = (bcm_field_entry_install(unit, entry))) != BCM_E_NONE){
                soc_cm_debug(DK_VLAN, 
                        "%s,%d,(err=%d) failed on field(Action install)!\n",
                        __func__, __LINE__, rv);
            }
            
            /* for UNI ports */
            entry = temp_vtcfp_ent->uni_field_id;
            if ((rv = (bcm_field_qualify_InPorts(
                    unit, entry, uni_bmp, pbm_mask))) != BCM_E_NONE){
                soc_cm_debug(DK_VLAN, 
                        "%s,%d,(err=%d) failed on field(new InPorts)!\n",
                        __func__, __LINE__, rv);
            }
            if ((rv = (bcm_field_entry_install(unit, entry))) != BCM_E_NONE){
                soc_cm_debug(DK_VLAN, 
                        "%s,%d,(err=%d) failed on field(Action install)!\n",
                        __func__, __LINE__, rv);
            }
          
        }
        VT_CFP_UNLOCK;
        soc_cm_debug(DK_VLAN, "%s, VTCFP rebuild DONE!\n",__func__);
        
    } else {
        return BCM_E_UNAVAIL;
    }
    
#endif /* BCM_53115 */
    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_robo_vlan_vtevr_isp_change
 * 
 * Purpose:
 *  rebuild the EVR(egress vlan remark) entry when isp/none-isp port changed.
 *  1. include the entry for None-ISP untag and VT used entries.
 * 
 * Note : 
 *  1. this is for bcm53115 only currently.
 */
int _bcm_robo_vlan_vtevr_isp_change(int unit, pbmp_t changed_pbm){
    
#if defined(BCM_53115)  
    if (SOC_IS_ROBO53115(unit)){
        uint32  temp32 = 0;
        
        temp32 = SOC_PBMP_WORD_GET(changed_pbm, 0);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                (unit, DRV_VLAN_PROP_EVR_VT_ISP_CHANGE, temp32));

    } else {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_53115 */
    
    return BCM_E_NONE;
}

/* =============== VT_CFP section END =================== */

#if QVLAN_UTBMP_BACKUP
/*
 * Function:
 *  _bcm_robo_vlist_utbmp_set
 * Purpose:
 *  set the untag portbitmap in a specific VLAN list to keep the logical VLAN 
 *      untag port bitmap.
 * Note:
 *  1. this unatag bitmap is not allowed to be set out of VLAN API scope.
 */
STATIC void 
_bcm_robo_vlist_utbmp_set(vlist_t **list, bcm_vlan_t vid, bcm_pbmp_t ut_pbmp)
{

    /* check if the indicated vid is valid */
    while (*list != NULL) {
        if ((*list)->vid == vid) {
            break;
        }
        list = &(*list)->next;
    }

    /* overwrite the untag bitmap*/
    assert(&ut_pbmp != NULL);
    if ((*list)->vid == vid) {
        BCM_PBMP_CLEAR((*list)->ubmp);
        BCM_PBMP_OR((*list)->ubmp, ut_pbmp);
    }

}

/*
 * Function:
 *  _bcm_robo_vlist_utbmp_get
 * Purpose:
 *  get the untag portbitmap in a specific VLAN list.
 */
void 
_bcm_robo_vlist_utbmp_get(vlist_t **list, bcm_vlan_t vid, bcm_pbmp_t *ut_pbmp)
{

    /* check if the indicated vid is valid */
    while (*list != NULL) {
        if ((*list)->vid == vid) {
            break;
        }
        list = &(*list)->next;
    }

    /* get the untag bitmap*/
    assert(ut_pbmp != NULL);
    if ((*list)->vid == vid) {
        BCM_PBMP_CLEAR(*ut_pbmp);
        BCM_PBMP_OR(*ut_pbmp, (*list)->ubmp);
    }

}

/*  
 * Function:
 *  _bcm_robo_vlan_utbmp_dt_rebuild
 * Purpose:
 *  reporgram the untag bitmap in whole 1Q_VLAN table due to the device 
 *      double tagging mode is turned on/off.
 * Note:
 *   1. each valid VLAN untag bitmap should be reprogramed by port dtag mode 
 *      (NNI or UNI) when DT_MODE is changed to enabled.
 *   2. each valid VLAN untag bitmap should be bakcuped to original bitmap 
 *      (logical bitmap) when DT_MODE is changed to disabled.
 *
 */
int _bcm_robo_vlan_utbmp_dt_rebuild(int unit, int dt_mode)
{
    vlist_t     *v;
    int         action = 0;
    pbmp_t      isp_bmp, v_bmp, temp_bmp;
    bcm_port_t  port = 0;
    bcm_vlan_t  vid = 0;
    vlan_entry_t    vt;
    uint64      temp64;
    uint32      temp32;

    /* action = 0, means restore | action = 1, means rebuild */
    action =  (dt_mode == BCM_PORT_DTAG_MODE_NONE) ? 0 : 1;

    /* get  isp_bmp when DT_MODE is enabled */
    if (dt_mode != BCM_PORT_DTAG_MODE_NONE ){
        port = ~port;
        BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->vlan_prop_port_enable_get)
                        (unit, DRV_VLAN_PROP_ISP_PORT, 
                        port,  (uint32 *)&temp64));
        soc_robo_64_val_to_pbmp(unit, &isp_bmp, temp64);
    }
    
    v = robo_vlan_info[unit].list;
    while (v != NULL) {

        if (action){
            vid = v->vid;

            /* all the vlan member port will be set as 
             * 1. UNI ports in untag bitmap are set.
             * 2. NNI ports in untag bitmap are reset.
             */
            BCM_IF_ERROR_RETURN(
                        bcm_vlan_port_get(unit, vid, &v_bmp, &temp_bmp));
            BCM_PBMP_CLEAR(temp_bmp);
            BCM_PBMP_REMOVE(v_bmp, isp_bmp);
            BCM_PBMP_OR(temp_bmp, v_bmp);
        } else {
            /* get the backup untag bitmap */
            BCM_PBMP_CLEAR(temp_bmp);
            BCM_PBMP_OR(temp_bmp, v->ubmp);
        }

        /* write to memory */
        sal_memset(&vt, 0, sizeof (vt));
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_read)
                        (unit, DRV_MEM_VLAN,
                        (uint32)vid, 1, (uint32 *)&vt));

        if (SOC_INFO(unit).port_num > 32) {
            soc_robo_64_pbmp_to_val(unit, &temp_bmp, &temp64);
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                            (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                            (uint32 *)&vt, (uint32 *)&temp64));
        } else {
            temp32 = SOC_PBMP_WORD_GET(temp_bmp, 0);
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                            (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                            (uint32 *)&vt, &temp32));
        }
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_write)
                        (unit, DRV_MEM_VLAN,
                        (uint32)vid, 1, (uint32 *)&vt));
            
        v = v->next;
    }
    
    return  BCM_E_NONE;
}
#endif

int _bcm_robo_flow2vlan_init(int unit)
{
    int table_size = SOC_MEM_SIZE(unit, FLOW2VLANm);
    flow2vlan_entry_t f2v_t;
    uint32 tmp = 0;
    int i = 0;
    int rv = BCM_E_NONE;

    /* init FLOW2VLAN table */
    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        sal_memset(&f2v_t, 0, sizeof (f2v_t));
        tmp = 0xfff;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_FLOWVLAN, DRV_MEM_FIELD_VLANID,
                        (uint32 *)&f2v_t, &tmp));
    
        for (i = 0; i < table_size; i++) {
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_write)
                (unit, DRV_MEM_FLOWVLAN, i, 1,
                (uint32 *)&f2v_t));
        }
    } else {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *  _bcm_robo_vlist_insert
 * Purpose:
 *  Add a VLAN ID to a vlist
 */

STATIC int
_bcm_robo_vlist_insert(vlist_t **list, bcm_vlan_t vid)
{
    vlist_t     *v;

    if ((v = sal_alloc(sizeof (vlist_t), "vlist")) == NULL) {
        return BCM_E_MEMORY;
    }

    v->vid = vid;

#if QVLAN_UTBMP_BACKUP
    /* reset the untag bitmap when create */
    BCM_PBMP_CLEAR(v->ubmp);
#endif

    v->next = *list;
    *list = v;

    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_robo_vlist_remove
 * Purpose:
 *  Delete a VLAN ID from a vlist
 */

STATIC int
_bcm_robo_vlist_remove(vlist_t **list, bcm_vlan_t vid)
{
    vlist_t     *v;

    while (*list != NULL) {
        if ((*list)->vid == vid) {
            v = *list;
            *list = v->next;
            sal_free(v);
            return BCM_E_NONE;
        }
        list = &(*list)->next;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *  _bcm_robo_vlist_lookup
 * Purpose:
 *  Return TRUE if a VLAN ID exists in a vlist, FALSE otherwise
 */

STATIC int
_bcm_robo_vlist_lookup(vlist_t **list, bcm_vlan_t vid)
{
    while (*list != NULL) {
        if ((*list)->vid == vid) {
            return TRUE;
        }
        list = &(*list)->next;
    }

    return FALSE;
}

/*
 * Function:
 *  _bcm_robo_vlist_destroy
 * Purpose:
 *  Free all memory used by a VLAN list
 */

STATIC int
_bcm_robo_vlist_destroy(vlist_t **list)
{
    while (*list != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_robo_vlist_remove(list, (*list)->vid));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *  _bcm_robo_vdata_compar
 * Purpose:
 *  Internal utility routine for sorting on VLAN ID.
 */

STATIC int 
_bcm_robo_vdata_compar(void *a, void *b)
{
    bcm_vlan_data_t     *d1 = a;
    bcm_vlan_data_t     *d2 = b;

    return (d1->vlan_tag < d2->vlan_tag ? -1 :
            d1->vlan_tag > d2->vlan_tag ?  1 : 0);
}

/*
 * Function:
 *  _bcm_robo_vlan_list
 * Purpose:
 *      Main body of bcm_vlan_list() and bcm_vlan_list_by_pbmp().
 *  Assumes locking already done.
 * Parameters:
 *  list_all - if TRUE, lists all ports and ignores list_pbmp.
 *  list_pbmp - if list_all is FALSE, lists only VLANs containing
 *      any of the ports in list_pbmp.
 */

STATIC int
_bcm_robo_vlan_list(int unit, bcm_vlan_data_t **listp, int *countp,
           int list_all, pbmp_t list_pbmp)
{
    bcm_vlan_data_t *list;
    vlist_t     *v;
    int         count, i, rv;

    *countp = 0;
    *listp = NULL;

    if (!list_all && SOC_PBMP_IS_NULL(list_pbmp)) { /* Empty list */
        return BCM_E_NONE;
    }

    count = robo_vlan_info[unit].count;

    if (count == 0) {
        return BCM_E_NONE;          /* Empty list */
    }

    if ((list = sal_alloc(count * sizeof (list[0]), "vlan_list")) == NULL) {
        return BCM_E_MEMORY;
    }

    i = 0;

    for (v = robo_vlan_info[unit].list; v != NULL; v = v->next) {
        pbmp_t      pbmp, ubmp, tbmp;
        
        if ((rv = bcm_vlan_port_get(unit, v->vid, &pbmp, &ubmp)) < 0) {
            sal_free(list);
            return rv;
        }
        
        SOC_PBMP_ASSIGN(tbmp, list_pbmp);
        SOC_PBMP_AND(tbmp, pbmp);
        if (list_all || SOC_PBMP_NOT_NULL(tbmp)) {
            assert(i < count);
        
            list[i].vlan_tag = v->vid;
            BCM_PBMP_ASSIGN(list[i].port_bitmap, pbmp);
            BCM_PBMP_ASSIGN(list[i].ut_port_bitmap, ubmp);
        
            i++;
        }
    }

    assert(!list_all || i == count);  /* If list_all, make sure all listed */

    *countp = i;
    *listp = list;

    _shr_sort(*listp, *countp, sizeof (bcm_vlan_data_t), 
                    _bcm_robo_vdata_compar);

    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_robo_vlan_default_set
 * Purpose:
 *  Main part of bcm_vlan_default_set; assumes locking already done.
 */

STATIC int
_bcm_robo_vlan_default_set(int unit, bcm_vlan_t vid)
{
    if (!_bcm_robo_vlist_lookup(&robo_vlan_info[unit].list, vid)) {
        return BCM_E_NOT_FOUND;
    }

    robo_vlan_info[unit].defl = vid;

    return BCM_E_NONE;
}


/*
 * Function:
 *  _bcm_robo_vlan_1st (internal)
 * Purpose:
 *  Return the first defined VLAN ID that is not the default VLAN.
 */

STATIC INLINE bcm_vlan_t
_bcm_robo_vlan_1st(int unit)
{
    vlist_t     *vl;

    for (vl = robo_vlan_info[unit].list; vl != NULL; vl = vl->next) {
        if (vl->vid != robo_vlan_info[unit].defl) {
            return vl->vid;
        }
    }

    return BCM_VLAN_INVALID;
}
/*
 * Function:
 *  _bcm_robo_vlan_init
 * Purpose:
 *  Initialize the VLAN related tables with the default entry in it.
 * Note :
 *  1. In earily version(for bcm5338), initial routine will enable below items, 
 *      need verified if there is any side effect without these setting:
 *      - enable reserve mcast tagging
 *      - enable GMRP/GVRP tagging
 *      - Rx GVRP/GMRP packet(to CPU)
 *  2. 1Q.enable and Default Vlan Tag = 1
 *  3. No memory clear been process, we assume all entry is cleared 
 *      after power on.
 */

STATIC int 
_bcm_robo_vlan_init(int unit, bcm_vlan_data_t *vd)
{
    vlan_entry_t        ve, ve_drop;
    bcm_pbmp_t          pbm;
    uint64              fld_value;
    uint32              fld_val32;
    int bcm5389a1 = 0;
    int len = 0;
    uint32 addr = 0, model = 0, rev = 0;
    
    /* ---- clear VLAN table ---- */
    /* skipped VLAN table clear (assuming hardware did it) */
    soc_cm_debug(DK_VLAN,
        "[vlan]: assuming hardware vlan table is cleared after power on\n");

    /* Check if chip is BCM53489A1 */
    if (SOC_IS_ROBO5389(unit)){
        len = (DRV_SERVICES(unit)->reg_length_get)
            (unit, MODEL_IDr);
        addr = (DRV_SERVICES(unit)->reg_addr)
            (unit, MODEL_IDr, 0, 0);
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
            (unit, addr, &model, len));
        len = (DRV_SERVICES(unit)->reg_length_get)
            (unit, CHIP_REVIDr);
        addr = (DRV_SERVICES(unit)->reg_addr)
            (unit, CHIP_REVIDr, 0, 0);
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
            (unit, addr, &rev, len));
        if ((model == BCM5389_A1_DEVICE_ID) &&
            (rev == BCM5389_A1_REV_ID)) {
            bcm5389a1 = 1;
        }
    }
    
    /* ---- set default entry into vlan table ---- */
    sal_memset(&ve, 0, sizeof (ve));
    sal_memset(&ve_drop, 0, sizeof (ve_drop));
    
    BCM_PBMP_ASSIGN(pbm, vd->ut_port_bitmap);
    BCM_PBMP_AND(pbm, PBMP_ALL(unit));


    if (SOC_IS_ROBO5397(unit)) {
#ifdef WAN_PORT_SUPPORT
        /*
         * WAN port of BCM5397 is reside at port #7.
         * In routing mode, WAN port should be added into default vlan table.
         * So that packets CPU sends to WAN port won't be dropped.
         */
        BCM_PBMP_PORT_ADD(vd->port_bitmap, WAN_PORT_NUMBER);
        BCM_PBMP_PORT_ADD(pbm, WAN_PORT_NUMBER);
#endif
    }
    

    if (SOC_INFO(unit).port_num > 32) {
        soc_robo_64_pbmp_to_val(unit, &vd->port_bitmap, &fld_value);
        (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                    (uint32 *)&ve, 
                    (uint32 *)&fld_value);
        soc_robo_64_pbmp_to_val(unit, &pbm, &fld_value);
        (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                    (uint32 *)&ve, 
                    (uint32 *)&fld_value);
    } else {
        fld_val32 = SOC_PBMP_WORD_GET(vd->port_bitmap, 0);
        (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                    (uint32 *)&ve, 
                    (uint32 *)&fld_val32);
        fld_val32 = SOC_PBMP_WORD_GET(pbm, 0);
        (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                    (uint32 *)&ve, 
                    (uint32 *)&fld_val32);
    }   
    /* bcm5389 does not support mspt */
    if(SOC_IS_ROBO5324(unit) || SOC_IS_ROBO5396(unit)
        || SOC_IS_ROBO5398(unit) || SOC_IS_ROBO5348(unit) || 
        SOC_IS_ROBO5397(unit) || SOC_IS_ROBO5347(unit) ||
        SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53242(unit) ||
        SOC_IS_ROBO53262(unit) || SOC_IS_ROBO53115(unit) ||
        SOC_IS_ROBO53118(unit)){
        fld_val32 = BCM_STG_DEFAULT;
        (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_SPT_GROUP_ID,
                    (uint32 *)&ve, 
                    &fld_val32);
    } 
     
    if(SOC_IS_ROBODINO(unit)){
        fld_val32 = 1;
        (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_VALID,
                    (uint32 *)&ve, 
                    &fld_val32);
        if (bcm5389a1) {
            (DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_VALID,
                        (uint32 *)&ve_drop, 
                        &fld_val32);
        }
    }

    
    /* write to VLAN table */               
    /* Create default vlan */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_write)
                    (unit, DRV_MEM_VLAN, BCM_VLAN_DEFAULT, 1, 
                    (uint32 *)&ve));

    /* Create vlan 4095 with empty members to drop packets of vid=4095 */
    if (bcm5389a1) {
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_write)
                        (unit, DRV_MEM_VLAN, BCM_VLAN_INVALID, 1, 
                        (uint32 *)&ve_drop));
    }
                    
    /* ---- set to QVLAN mode (enable 1Q vlan) ---- */
    /* set 1Q vlan mode */               
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_mode_set)
                    (unit, DRV_VLAN_MODE_TAG));
    
    /* ------- non 1Q-VLAN init -------- */
    if(SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)){
        BCM_PBMP_CLEAR(pbm);
        BCM_PBMP_ASSIGN(pbm, PBMP_ALL(unit));
        /* disable all ports' VLAN translation */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_prop_port_enable_set)
                            (unit, DRV_VLAN_PROP_V2V_PORT,pbm ,FALSE));
        /* disabled all ports' VLAN translation */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_prop_port_enable_set)
                            (unit, DRV_VLAN_PROP_MAC2V_PORT,pbm ,TRUE));
        /* disable all ports' VLAN translation */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_prop_port_enable_set)
                            (unit, DRV_VLAN_PROP_PROTOCOL2V_PORT,pbm ,TRUE));
        /* enable port based trust VLAN */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_prop_port_enable_set)
                            (unit, DRV_VLAN_PROP_TRUST_VLAN_PORT,pbm ,TRUE));
        
        /* initialize MAC VLAN */
        BCM_IF_ERROR_RETURN(bcm_vlan_mac_delete_all(unit));
    
        /* initialize Protocol VLAN (assigning port=0 but won't be used in ROBO chip)*/
        BCM_IF_ERROR_RETURN(bcm_port_protocol_vlan_delete_all(unit, 0));
        
        /* initialize FLOW2VLAN table. Value of unused entries should be 0xfff. */
        BCM_IF_ERROR_RETURN(_bcm_robo_flow2vlan_init(unit));
        
        /* inittialize VLAN2VLAN table.
         * - special reset routine for performance issue. Check the detail 
         *  in service driver.
         */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_V2V_INIT, TRUE));
        
#if defined(BCM_53115)  
    } else if (SOC_IS_ROBO53115(unit)) {
        int rv = BCM_E_NONE;
        
        /* init the bcm53115 VT requirred SW information or re-init 
         * existing VT related CFP configuration. 
         */
        flag_skip_cfp_create = TRUE;
        rv = _bcm_robo_vlan_vtcfp_init(unit);
        flag_skip_cfp_create = FALSE;
        BCM_IF_ERROR_RETURN(rv);

        /* clear all EVR table entry on each port. and the related 
         *      port basis database.
         */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_EVR_INIT, TRUE));

        /* reset the iDT_Mode */ 
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                        (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, FALSE));
        
        flag_vt_cfp_init = FALSE;
#endif  /* BCM_53115 */                         
    }
    
    if (SOC_IS_ROBO5395(unit)) {
        /* Disable MAC VLAN  for all ports */
        BCM_IF_ERROR_RETURN(
            bcm_vlan_control_set(unit, bcmVlanPreferMAC, FALSE));
        /* Clear MAC based VLAN table */
        BCM_IF_ERROR_RETURN(bcm_vlan_mac_delete_all(unit));
    }

    return BCM_E_NONE;

}

int bcm_robo_vlan_init(int unit)
{
    bcm_vlan_data_t     vd;

    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_init()..\n");
    sal_memset(&vd, 0, sizeof (vd));

   /*
     * Initialize hardware tables
     */
    vd.vlan_tag = BCM_VLAN_DEFAULT;
    BCM_PBMP_ASSIGN(vd.port_bitmap, PBMP_ALL(unit));
    BCM_PBMP_ASSIGN(vd.ut_port_bitmap, PBMP_ALL(unit));
    BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_CMIC(unit));

    /*
     * A compile-time application policy may prefer to not add
     * Ethernet or CPU ports to the default VLAN.
     */
#ifdef  BCM_VLAN_NO_DEFAULT_ETHER
    BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_E_ALL(unit));
    BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_E_ALL(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_ETHER */
#ifdef  BCM_VLAN_NO_DEFAULT_CPU
    BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_CMIC(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_CPU */

    /*
     * Init VLAN related hw reg/mem.
     */    
    BCM_IF_ERROR_RETURN(_bcm_robo_vlan_init(unit, &vd));

    /*
     * Initialize software structures
     */
    robo_vlan_info[unit].defl = BCM_VLAN_DEFAULT;    

    /* In case bcm_vlan_init is called more than once */
    BCM_IF_ERROR_RETURN(
        _bcm_robo_vlist_destroy(&robo_vlan_info[unit].list));

    BCM_IF_ERROR_RETURN(
        _bcm_robo_vlist_insert(&robo_vlan_info[unit].list, vd.vlan_tag));

#if QVLAN_UTBMP_BACKUP
    /* backup the untage bitmap */
    _bcm_robo_vlist_utbmp_set(&robo_vlan_info[unit].list, vd.vlan_tag, vd.ut_port_bitmap);
    
#endif
    robo_vlan_info[unit].count = 1;
    robo_vlan_info[unit].init = TRUE;

    return BCM_E_NONE;

}

/*
 * Function:
 *  _bcm_robo_vlan_create
 * Purpose:
 *  Main body of bcm_vlan_create; assumes locking already done;
 */

STATIC int
_bcm_robo_vlan_create(int unit, bcm_vlan_t vid)
{
    bcm_stg_t       stg_defl;
    int             rv;
    vlan_entry_t    vt;
    uint64          field_value;
    uint32          field_val32;

    sal_memset(&vt, 0, sizeof (vt));

    rv = bcm_stg_default_get(unit, &stg_defl);
    if (rv == BCM_E_UNAVAIL) {
        stg_defl = -1;
    } else if (rv < 0) {
        return rv;
    }

    if (_bcm_robo_vlist_lookup(&robo_vlan_info[unit].list, vid)) {
        return BCM_E_EXISTS;
    }

    /* clear all port bitmap for creating process */
    if (SOC_INFO(unit).port_num > 32) {
        COMPILER_64_ZERO(field_value);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_value));

        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_value));
    } else {
        field_val32 = 0;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_val32));

        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_val32));
    }
    
    /*
     * The STG value doesn't matter much; it will get changed
     * immediately after this routine is called.
     */
    if(SOC_IS_ROBO5324(unit) || SOC_IS_ROBO5396(unit)||
        SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)){
        field_val32 = BCM_STG_DEFAULT;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_SPT_GROUP_ID,
                    (uint32 *)&vt, (uint32 *)&field_val32));
    }

    if(SOC_IS_ROBODINO(unit)){
        field_val32 = 1;
        (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_VALID,
                    (uint32 *)&vt, 
                    (uint32 *)&field_val32);    
    }

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_write)
                    (unit, DRV_MEM_VLAN, 
                    (uint32)vid, 1, (uint32 *)&vt));

    BCM_IF_ERROR_RETURN
    (_bcm_robo_vlist_insert(&robo_vlan_info[unit].list, vid));

    robo_vlan_info[unit].count++;

    if (stg_defl >= 0) {
        /* Must be after vlist insert */
        BCM_IF_ERROR_RETURN
            (bcm_stg_vlan_add(unit, stg_defl, vid));
    }

    return BCM_E_NONE;
}
    
/*
 * Function:
 *  bcm_robo_vlan_create
 * Purpose:
 *  Allocate and configure a VLAN on RoboSwitch.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to create.
 * Returns:
 *  BCM_E_NONE - Success.
 *  BCM_E_INTERNAL - Chip access failure.
 *  BCM_E_EXISTS - VLAN ID already in use.
 * Notes:
 *  VLAN is placed in the default STG and can be reassigned later.
 */
int bcm_robo_vlan_create(int unit, bcm_vlan_t vid)
{
    int         rv;

    CHECK_INIT(unit);
    CHECK_VID(unit, vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_create()..\n");
    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_create(unit, vid);
    BCM_UNLOCK(unit);

    return rv;
}               



/*
 * Function:
 *  _bcm_robo_vlan_destroy
 * Purpose:
 *  Main body of bcm_vlan_destroy; assumes locking done.
 */

STATIC int
_bcm_robo_vlan_destroy(int unit, bcm_vlan_t vid)
{
    bcm_stg_t       stg;
    int             rv;
    vlan_entry_t    vt;

    /* Cannot destroy default VLAN */
    if (vid == robo_vlan_info[unit].defl) {
        return BCM_E_BADID;
    }

    if (!_bcm_robo_vlist_lookup(&robo_vlan_info[unit].list, vid)) {
        return BCM_E_NOT_FOUND;
    }

    /* Remove VLAN from its spanning tree group */
    BCM_IF_ERROR_RETURN(bcm_vlan_stg_get
                    (unit, vid, &stg));

    rv = bcm_stg_vlan_remove(unit, stg, vid);
    if (rv < 0 && rv != BCM_E_UNAVAIL) {
        return rv;
    }   

    sal_memset(&vt, 0, sizeof(vlan_entry_t));
    /* set this VLAN entry been invalid */   
    if ((rv = ((DRV_SERVICES(unit)->mem_write)
                (unit, DRV_MEM_VLAN, 
                (uint32)vid, 1, (uint32 *)&vt))) < 0){
        return rv;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_robo_vlist_remove
                    (&robo_vlan_info[unit].list, vid));

    robo_vlan_info[unit].count--;

    return BCM_E_NONE;
}
    
/*
 * Function:
 *  bcm_robo_vlan_destroy
 * Purpose:
 *  Deallocate VLAN from RoboSwitch.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to affect.
 * Returns:
 *  BCM_E_NONE      Success.
 *  BCM_E_INTERNAL      Chip access failure.
 *  BCM_E_BADID           Cannot remove default VLAN
 *  BCM_E_NOT_FOUND VLAN ID not in use.
 * Notes:
 *  None.
 */
int 
bcm_robo_vlan_destroy(int unit, bcm_vlan_t vid)
{
    int     rv;

    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_destroy()..\n");
    CHECK_VID(unit, vid);

    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_destroy(unit, vid);
    BCM_UNLOCK(unit);

    return rv;
}               
    
/*
 * Function:
 *  bcm_robo_vlan_destroy_all
 * Purpose:
 *  Destroy all VLANs except the default VLAN
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 * Returns:
 *  BCM_E_NONE - Success.
 *  BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *  None.
 */
int 
bcm_robo_vlan_destroy_all(int unit)
{
    int             rv = BCM_E_NONE;
    bcm_vlan_t      vid;

    CHECK_INIT(unit);

    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_destroy_all()..\n");
    BCM_LOCK(unit);

    while ((vid = _bcm_robo_vlan_1st(unit)) != BCM_VLAN_INVALID) {
        if ((rv = bcm_vlan_destroy(unit, vid)) < 0) {
            break;
        }
    }

    BCM_UNLOCK(unit);

    return rv;
}               

/*
 * Function:
 *  _bcm_robo_vlan_port_get
 * Purpose:
 *  Read the port bitmap from a VLAN_TAB entry.
 */
int
_bcm_robo_vlan_port_get(int unit, bcm_vlan_t vid, pbmp_t *pbmp, pbmp_t *ubmp)
{
    vlan_entry_t    vt;
    uint64          field_value;
    uint32          field_val32;

#if QVLAN_UTBMP_BACKUP
    pbmp_t      backup_ubmp;
#endif
    /* Check if this VLAN is exist */
    if (!(_bcm_robo_vlist_lookup(&robo_vlan_info[unit].list, vid))) {
        return BCM_E_NOT_FOUND;
    }
    
    /* Upper layer already checks that vid is valid */
    sal_memset(&vt, 0, sizeof (vt));
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_read)
                    (unit, DRV_MEM_VLAN, 
                    (uint32)vid, 1, (uint32 *)&vt));

    if (SOC_INFO(unit).port_num > 32) {
        COMPILER_64_ZERO(field_value);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_value));

        if (pbmp != NULL) {
            BCM_PBMP_CLEAR(*pbmp);
            soc_robo_64_val_to_pbmp(unit, pbmp, field_value);
        }

#if QVLAN_UTBMP_BACKUP
        if (ubmp != NULL) {
            BCM_PBMP_CLEAR(*ubmp);
            BCM_PBMP_OR(*ubmp, backup_ubmp);
        }
#else
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_value));
        if (ubmp != NULL) {
            BCM_PBMP_CLEAR(*ubmp);
            soc_robo_64_val_to_pbmp(unit, ubmp, field_value);
        }
#endif
    } else {
        field_val32 = 0;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_val32));
        if (pbmp != NULL) {
            BCM_PBMP_CLEAR(*pbmp);
            SOC_PBMP_WORD_SET(*pbmp, 0, field_val32);
        }

#if QVLAN_UTBMP_BACKUP
        if (ubmp != NULL) {
            BCM_PBMP_CLEAR(*ubmp);
            BCM_PBMP_OR(*ubmp, backup_ubmp);
        }
#else
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_val32));
        if (ubmp != NULL) {
            BCM_PBMP_CLEAR(*ubmp);
            SOC_PBMP_WORD_SET(*ubmp, 0, field_val32);
        }
#endif
    }

    if (SOC_IS_ROBO5397(unit)) {
#ifdef WAN_PORT_SUPPORT
        /*
         * WAN port of BCM5397 is reside at port #7.
         * Although WAN port is added in default vlan table. 
         * This port should not be returned when port member of default vlan is
         * requested.
         */
        if (vid == BCM_VLAN_DEFAULT) {
            BCM_PBMP_PORT_REMOVE(*pbmp, WAN_PORT_NUMBER);
            BCM_PBMP_PORT_REMOVE(*ubmp, WAN_PORT_NUMBER);
        }
#endif
    }

    return BCM_E_NONE;
}

    
/*
 * Function:
 *  _bcm_robo_vlan_port_add
 * Purpose:
 *  Main part of bcm_robo_vlan_port_add; assumes locking already done.
 */

STATIC int
_bcm_robo_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp)
{
    pbmp_t          vlan_pbmp, vlan_ubmp;
    vlan_entry_t    vt;
    uint64          field_value, temp64;
    uint32          field_val32, temp32;
    
#if QVLAN_UTBMP_BACKUP
    pbmp_t      backup_ubmp, tag_bmp;
    int         dt_mode;
    bcm_port_t  isp_port;
#endif

    /* Check if this VLAN is exist */
    if (!(_bcm_robo_vlist_lookup(&robo_vlan_info[unit].list, vid))) {
        return BCM_E_NOT_FOUND;
    }

    /* No such thing as untagged CPU */
    BCM_PBMP_REMOVE(ubmp, PBMP_CMIC(unit));

    /* --- Don't add ports that are already there --- */
    BCM_IF_ERROR_RETURN(_bcm_robo_vlan_port_get(unit, vid,
                           &vlan_pbmp, &vlan_ubmp));

    BCM_PBMP_OR(pbmp, vlan_pbmp);
    BCM_PBMP_OR(ubmp, vlan_ubmp);
    
    /* Only allow untagged ports belong to portbitmap on the vlan. */
    BCM_PBMP_AND(ubmp, pbmp);

#if QVLAN_UTBMP_BACKUP
    /* backup untag bitmap */
    BCM_PBMP_CLEAR(backup_ubmp);
    BCM_PBMP_OR(backup_ubmp, ubmp);
    BCM_PBMP_REMOVE(backup_ubmp, PBMP_CMIC(unit));

    /* get the tagged bitmap */
    BCM_PBMP_CLEAR(tag_bmp);
    BCM_PBMP_OR(tag_bmp, pbmp);
    
    /* get device DT_MODE :
     *  - currently in ROBO chips(bcm5348/53242), if a port dtag mode was get 
     *      as "BCM_PORT_DTAG_MODE_NONE" value means the device double 
     *      tagging mode is turned off.
     */
    BCM_IF_ERROR_RETURN(bcm_port_dtag_mode_get(unit, 0, &dt_mode));
#endif

    BCM_PBMP_REMOVE(pbmp, vlan_pbmp);
    BCM_PBMP_REMOVE(ubmp, vlan_ubmp);
    
    BCM_PBMP_REMOVE(ubmp, PBMP_CMIC(unit));
    
    sal_memset(&vt, 0, sizeof (vt));
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_read)
                    (unit, DRV_MEM_VLAN, 
                    (uint32)vid, 1, (uint32 *)&vt));

    if (SOC_INFO(unit).port_num > 32) {
        COMPILER_64_ZERO(field_value);                    
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_value));
                        
        soc_robo_64_pbmp_to_val(unit, &pbmp, &temp64);
        COMPILER_64_OR(field_value, temp64);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_value));
                        
        /* process ut_pbmp */
#if QVLAN_UTBMP_BACKUP
        COMPILER_64_ZERO(field_value);
        if (dt_mode != BCM_PORT_DTAG_MODE_NONE){    /* dt_mode is enabled */
            /* ------ special process when DT_MODE is enabled ------ 
             *      1. NNI port must be excluded in untag bitmap.
             *      2. UNI port must be existed in untag bitmap.
             */
            BCM_PBMP_CLEAR(ubmp);
            BCM_PBMP_ITER(tag_bmp, isp_port){
                if (IS_CPU_PORT(unit, isp_port)){
                    continue;
                }
                BCM_IF_ERROR_RETURN(
                            bcm_port_dtag_mode_get(unit, isp_port, &dt_mode));
                switch (dt_mode){
                case BCM_PORT_DTAG_MODE_INTERNAL :      /* NNI port */
                    BCM_PBMP_PORT_REMOVE(ubmp, isp_port);
                    break;
                case BCM_PORT_DTAG_MODE_EXTERNAL :      /* UNI port */
                    BCM_PBMP_PORT_ADD(ubmp, isp_port);
                    break;
                default :
                    continue;
                    break;
                }
            }   /* BCM_PBMP_ITER(tag_bmp) */
        } else {
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                            (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                            (uint32 *)&vt, (uint32 *)&field_value));
         }
        
#else
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_value));
#endif
        soc_robo_64_pbmp_to_val(unit, &ubmp, &temp64);
        COMPILER_64_OR(field_value, temp64);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_value));
    } else {
        field_val32 = 0;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_val32));
                        
        temp32 = SOC_PBMP_WORD_GET(pbmp, 0);
        field_val32 |= temp32;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_val32));
                        
        /* process ut_pbmp */
#if QVLAN_UTBMP_BACKUP
        field_val32 = 0;
        if (dt_mode != BCM_PORT_DTAG_MODE_NONE){    /* dt_mode is enabled */
            /* ------ special process when DT_MODE is enabled ------ 
             *      1. NNI port must be excluded in untag bitmap.
             *      2. UNI port must be existed in untag bitmap.
             */
            BCM_PBMP_CLEAR(ubmp);
            BCM_PBMP_ITER(tag_bmp, isp_port){
                if (IS_CPU_PORT(unit, isp_port)){
                    continue;
                }
                BCM_IF_ERROR_RETURN(
                            bcm_port_dtag_mode_get(unit, isp_port, &dt_mode));
                switch (dt_mode){
                case BCM_PORT_DTAG_MODE_INTERNAL :      /* NNI port */
                    BCM_PBMP_PORT_REMOVE(ubmp, isp_port);
                    break;
                case BCM_PORT_DTAG_MODE_EXTERNAL :      /* UNI port */
                    BCM_PBMP_PORT_ADD(ubmp, isp_port);
                    break;
                default :
                    break;
                }
            }   /* BCM_PBMP_ITER(tag_bmp) */
            
        } else {
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                            (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                            (uint32 *)&vt, (uint32 *)&field_value));
         }

#else
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_val32));
#endif
        temp32 = SOC_PBMP_WORD_GET(ubmp, 0);
        field_val32 |= temp32;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_val32));
    }
                    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_write)
                    (unit, DRV_MEM_VLAN, 
                    (uint32)vid, 1, (uint32 *)&vt));

#if QVLAN_UTBMP_BACKUP
    /* ------ backup untag bitmap ------ */
    _bcm_robo_vlist_utbmp_set(&robo_vlan_info[unit].list, vid, backup_ubmp);
    /* ------ */
#endif
    
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_vlan_port_add
 * Purpose:
 *      Add ports to the specified vlan.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to add port to as a member.
 *      pbmp - port bitmap for members of VLAN
 *      ubmp - untagged members of VLAN
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */
int 
bcm_robo_vlan_port_add(int unit,
                 bcm_vlan_t vid,
                 bcm_pbmp_t pbmp, bcm_pbmp_t ubmp)
{
    int         rv;

    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_port_add()..\n");
    CHECK_VID(unit, vid);

    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_port_add(unit, vid, pbmp, ubmp);
    BCM_UNLOCK(unit);

    return rv;
}               

/*
 * Function:
 *  _bcm_robo_vlan_port_remove
 * Purpose:
 *  Main part of bcm_vlan_port_remove; assumes locking already done.
 */
static int
_bcm_robo_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    pbmp_t          vlan_pbmp, vlan_ubmp;
    vlan_entry_t    vt;
    uint64          field_value, temp64;
    uint32          field_val32, temp32;
    
#if QVLAN_UTBMP_BACKUP
    pbmp_t      temp_bmp;
    int         dt_mode;
    bcm_port_t  c_port;     /* customer port */
#endif

    /* Check if this VLAN is exist */
    if (!(_bcm_robo_vlist_lookup(&robo_vlan_info[unit].list, vid))) {
        return BCM_E_NOT_FOUND;
    }

    /* Don't remove ports that are not there */
    BCM_IF_ERROR_RETURN(_bcm_robo_vlan_port_get(unit, vid,
                           &vlan_pbmp, &vlan_ubmp));

    BCM_PBMP_AND(pbmp, vlan_pbmp);

    /* Remove ports from the VLAN bitmap in a VLAN_TAB entry.*/
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_read)
                    (unit, DRV_MEM_VLAN, 
                    (uint32)vid, 1, (uint32 *)&vt));

#if QVLAN_UTBMP_BACKUP
    /* get DT_MODE */
    BCM_IF_ERROR_RETURN(
                bcm_port_dtag_mode_get(unit, 0, &dt_mode));

    /* kept the remove bitmap */
    BCM_PBMP_CLEAR(temp_bmp);
    BCM_PBMP_OR(temp_bmp, pbmp);
    
#endif

    if (SOC_INFO(unit).port_num > 32) {
        COMPILER_64_ZERO(field_value);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_value));

        /* set new port bitmap */
        soc_robo_64_pbmp_to_val(unit, &pbmp, &temp64);
        COMPILER_64_NOT(temp64);
        COMPILER_64_AND(field_value, temp64);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_value));

#if QVLAN_UTBMP_BACKUP
        if (dt_mode == BCM_PORT_DTAG_MODE_NONE){
            /* 1. assuming that the untag bitmap from real memory read is well 
             *      processed by the rule of NNI/UNI port mode setting. 
             * 2. based on the assumptions above, here we can exclude the port  
             *      which was set as UNI Port and is existed in this removed 
             *      port bitmap. (UNI port must not been 0 in untag_bitmap.)
             * 3. after this process
             */
            BCM_PBMP_CLEAR(temp_bmp);
            BCM_PBMP_OR(temp_bmp, pbmp);
            
            BCM_PBMP_ITER(temp_bmp, c_port){
                if (IS_CPU_PORT(unit, c_port)){
                    continue;
                }
                BCM_IF_ERROR_RETURN(
                            bcm_port_dtag_mode_get(unit, c_port, &dt_mode));
                switch (dt_mode){
                case BCM_PORT_DTAG_MODE_EXTERNAL :      /* UNI port */
                    BCM_PBMP_PORT_REMOVE(pbmp, c_port);
                    break;
                default :
                    continue;
                    break;
                }
            }   /* BCM_PBMP_ITER(pbmp) */
        }
#endif

        /* get/set new untag pbmp */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_value));
        soc_robo_64_pbmp_to_val(unit, &pbmp, &temp64);
        COMPILER_64_NOT(temp64);
        COMPILER_64_AND(field_value, temp64);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_value));
    } else {
        field_val32 = 0;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_val32));

        /* set new port bitmap */
        temp32 = SOC_PBMP_WORD_GET(pbmp, 0);
        field_val32 &= ~temp32;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_PORT_BITMAP,
                        (uint32 *)&vt, (uint32 *)&field_val32));
                        
        /* get/set new untag pbmp */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_val32));
        field_val32 &= ~temp32;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_OUTPUT_UNTAG,
                        (uint32 *)&vt, (uint32 *)&field_val32));
    }

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_write)
                    (unit, DRV_MEM_VLAN, 
                    (uint32)vid, 1, (uint32 *)&vt));
    
    return BCM_E_NONE;
}

                 
/*
 * Function:
 *  bcm_robo_vlan_port_remove
 * Purpose:
 *      Remove ports from a specified vlan.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to remove port(s) from.
 *      pbmp - port bitmap for ports to remove.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 * Notes:
 *      None.
 */
int 
bcm_robo_vlan_port_remove(int unit,
                bcm_vlan_t vid,
                bcm_pbmp_t pbmp)
{
    int         rv;

    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_port_remove()..\n");
    CHECK_VID(unit, vid);

    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_port_remove(unit, vid, pbmp);
    BCM_UNLOCK(unit);

    return rv;
}               
                
/*
 * Function:
 *  bcm_robo_vlan_port_get
 * Purpose:
 *      Retrieves a list of the member ports of an existing VLAN.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to lookup.
 *      tag_pbmp - (output) member port bitmap (ignored if NULL)
 *      untag_pbmp - (output) untagged port bitmap (ignored if NULL)
 * Returns:
 *      BCM_E_NONE - Success (port bitmaps filled in).
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - No such VLAN defined.
 */

int 
bcm_robo_vlan_port_get(int unit,
                 bcm_vlan_t vid,
                 bcm_pbmp_t *pbmp, bcm_pbmp_t *ubmp)
{
    int         rv;

    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_port_get()..\n");
    CHECK_VID(unit, vid);

    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_port_get(unit, vid, pbmp, ubmp);
    BCM_UNLOCK(unit);

    return rv;
}               
                 

/*
 * Function:
 *  bcm_robo_vlan_list
 * Purpose:
 *      Returns an array of all defined VLANs and their port bitmaps.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number
 *      listp - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero VLANs defined.
 *      countp - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero VLANs defined.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_MEMORY - Out of system memory.
 * Notes:
 *  The caller is responsible for freeing the memory that is
 *  returned, using bcm_vlan_list_destroy.
 */
int 
bcm_robo_vlan_list(int unit, bcm_vlan_data_t **listp, int *countp)
{
    int     rv;
    pbmp_t  empty_pbm;

    CHECK_INIT(unit);

    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_list()..\n");
    SOC_PBMP_CLEAR(empty_pbm);
    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_list(unit, listp, countp, TRUE, empty_pbm);
    BCM_UNLOCK(unit);

    return rv;
}               

/*
 * Function:
 *  bcm_robo_vlan_list_by_pbmp
 * Purpose:
 *      Returns an array of defined VLANs and port bitmaps.
 *  Only VLANs that containing any of the specified ports are listed.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number
 *  pbmp - Bitmap of ports
 *      listp - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero VLANs defined.
 *      countp - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero VLANs defined.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 *      BCM_E_MEMORY            Out of system memory.
 * Notes:
 *  The caller is responsible for freeing the memory that is
 *  returned, using bcm_vlan_list_destroy.
 */

int 
bcm_robo_vlan_list_by_pbmp(int unit, pbmp_t pbmp,
                 bcm_vlan_data_t **listp, int *countp)
{
    int     rv;

    CHECK_INIT(unit);

    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_list_by_pbmp()..\n");
    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_list(unit, listp, countp, FALSE, pbmp);
    BCM_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        _shr_sort(*listp, *countp, sizeof (bcm_vlan_data_t), 
                _bcm_robo_vdata_compar);
    }

    return rv;
}               
                 
/*
 * Function:
 *  bcm_robo_vlan_list_destroy
 * Purpose:
 *      Destroy a list returned by bcm_vlan_list.
 *      Also works for the zero-VLAN case (NULL list).
 * Parameters:
 *      unit - RoboSwitch PCI device unit number
 *      list - List returned by bcm_vlan_list
 *      count - Count returned by bcm_vlan_count
 * Returns:
 *      BCM_E_NONE              Success.
 * Notes:
 *      None.
 */
int 
bcm_robo_vlan_list_destroy(int unit, bcm_vlan_data_t *list, int count)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_list_destroy()..\n");
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    if (list != NULL) {
        sal_free(list);
    }

    return BCM_E_NONE;
}               
    

/*
 * Function:
 *  bcm_robo_vlan_default_get
 * Purpose:
 *  Get the default VLAN ID
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  vid_ptr - (OUT) Target to receive the VLAN ID.
 * Returns:
 *  BCM_E_NONE - Success.
 */
int 
bcm_robo_vlan_default_get(int unit, bcm_vlan_t *vid_ptr)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_default_get()..\n");
    *vid_ptr = robo_vlan_info[unit].defl;

    return BCM_E_NONE;
}               

/*
 * Function:
 *  bcm_robo_vlan_default_set
 * Purpose:
 *  Set the default VLAN ID
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  vid - The new default VLAN
 * Returns:
 *  BCM_E_NONE - Success.
 * Notes:
 *  The new default VLAN must already exist.
 */
int 
bcm_robo_vlan_default_set(int unit, bcm_vlan_t vid)
{
    int         rv;

    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_default_set()..\n");
    CHECK_VID(unit, vid);

    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_default_set(unit, vid);
    BCM_UNLOCK(unit);

    return rv;
}               
    

/*
 * Function:
 *  bcm_robo_vlan_stg_get
 * Purpose:
 *  Retrieve the VTABLE STG for the specified vlan
 * Parameters:
 *  unit - RoboSwitch PCI device unit number.
 *      vid - VLAN ID.
 *  stg_ptr - (OUT) Pointer to store stgid for return.
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_vlan_stg_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg_ptr)
{
    uint32          field_value = 0;

    CHECK_INIT(unit);

    soc_cm_debug(DK_VLAN, 
                "BCM API : bcm_robo_vlan_stg_get()..\n");
    
    /* Upper layer already checks that vid is valid */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mstp_config_get)
                (unit, (uint32)vid, &field_value));
    
    *stg_ptr = field_value;

    return BCM_E_NONE;
}               
    
/*
 * Function:
 *  bcm_robo_vlan_stg_set
 * Purpose:
 *  Update the VTABLE STG for the specified vlan
 * Parameters:
 *  unit  - RoboSwitch PCI device unit number
 *      vid - VLAN ID
 *  stg - New spanning tree group number for VLAN
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_vlan_stg_set(int unit, bcm_vlan_t vid, bcm_stg_t stg)
{
    int     rv;
    bcm_stg_t *stg_list;
    int     stg_count, i, stg_existed = 0;

    CHECK_INIT(unit);

    /* Check if this VLAN is exist */
    if (!(_bcm_robo_vlist_lookup(&robo_vlan_info[unit].list, vid))) {
        return BCM_E_NOT_FOUND;
    }

    /* Check if this STG is exist */
    BCM_IF_ERROR_RETURN(bcm_stg_list(unit, &stg_list, &stg_count));
    for (i = 0; i < stg_count; i++) {
        if (stg_list[i] == stg) {
            stg_existed = 1;
            break;
        }
    }
    BCM_IF_ERROR_RETURN(bcm_stg_list_destroy(unit, stg_list, stg_count));
    if (!stg_existed) {
        return BCM_E_BADID;
    }

    soc_cm_debug(DK_VLAN, 
                "BCM API : bcm_robo_vlan_stg_set()..\n");
    /* Upper layer already checks that vid is valid */

    if ((rv = (DRV_SERVICES(unit)->mstp_config_set)
        (unit, (uint32)vid, (uint32)stg)) < 0) {
        return rv;
    }

    return BCM_E_NONE;
}               

/*
 * Function:
 *  _bcm_robo_vlan_stp_set
 * Purpose:
 *  Main part of bcm_vlan_stp_set; assumes locking already done.
 */

STATIC int
_bcm_robo_vlan_stp_set(int unit, bcm_vlan_t vid, 
                bcm_port_t port, int stp_state)
{
    bcm_stg_t   stgid;

    BCM_IF_ERROR_RETURN(bcm_vlan_stg_get(unit, vid, &stgid));
    BCM_IF_ERROR_RETURN(bcm_stg_stp_set(unit, stgid, port, stp_state));

    return BCM_E_NONE;
}
    
/*
 * Function:
 *  bcm_robo_vlan_stp_set
 * Purpose:
 *  Set the spanning tree state for a port in the whole spanning
 *  tree group that contains the specified VLAN.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *      vid - VLAN ID
 *      port - Port
 *  stp_state - State to set
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_vlan_stp_set(int unit, bcm_vlan_t vid,
                bcm_port_t port, int stp_state)
{
    int     rv;

    soc_cm_debug(DK_VLAN, "BCM API : bcm_robo_vlan_stp_set()..\n");
    rv = _bcm_robo_vlan_stp_set(unit, vid, port, stp_state);

    return rv;
}               

/*
 * Function:
 *  bcm_robo_vlan_stp_get
 * Purpose:
 *  Get the spanning tree state for a port in the whole spanning
 *  tree group that contains the specified VLAN.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number.
 *      vid - VLAN ID.
 *      port - Port
 *  stp_state - (OUT) State to return.
 * Returns:
 *  BCM_E_XXX
 */

int 
bcm_robo_vlan_stp_get(int unit, bcm_vlan_t vid,
                bcm_port_t port, int *stp_state)
{
    int     rv;
    uint32 port_status;

    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_stp_get()..\n");
    if ((rv = (DRV_SERVICES(unit)->mstp_port_get)
        (unit, (uint32)vid, (uint32)port, &port_status)) < 0) {
        return rv;
    }
    *stp_state = (port_status == DRV_PORTST_DISABLE) ? BCM_STG_STP_DISABLE :
             (port_status == DRV_PORTST_BLOCK) ? BCM_STG_STP_BLOCK :
             (port_status == DRV_PORTST_LEARN) ? BCM_STG_STP_LEARN :
             (port_status == DRV_PORTST_FORWARD) ? BCM_STG_STP_FORWARD :
             BCM_STG_STP_LISTEN;

    return BCM_E_NONE;
}               
                

/* MAC based vlan selection */
int 
bcm_robo_vlan_mac_add(int unit, bcm_mac_t mac, bcm_vlan_t vid, int prio)
{
    int rv = BCM_E_NONE;
    mac2vlan_entry_t  vment;
    uint64 mac_field;
    uint32  temp;
    
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_mac_add()\n");

    prio &= ~BCM_PRIO_DROP_FIRST;       /* no cng setting in ROBO */
    if ((prio & ~BCM_PRIO_MASK) != 0) {
        return BCM_E_PARAM;
    }

    sal_memset(&vment, 0, sizeof(vment));

    /* set Valid field */
    temp = 1;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_VALID, 
        (uint32 *)&vment, &temp));
    
    /* set priority field */
    temp = prio;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_PRIORITY, 
        (uint32 *)&vment, &temp));
    /* set VID field */
    temp = vid;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_VLANID, 
        (uint32 *)&vment, &temp));
    /* set MAC field */
    SAL_MAC_ADDR_TO_UINT64(mac, mac_field);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_MAC, 
        (uint32 *)&vment, (uint32 *)&mac_field));

    rv = (DRV_SERVICES(unit)->mem_insert)
        (unit, DRV_MEM_MACVLAN, (uint32 *)&vment, 0);
    return rv;
}               

int 
bcm_robo_vlan_mac_delete(int unit, bcm_mac_t mac)
{
    int rv = BCM_E_NONE;
    mac2vlan_entry_t  vment;
    uint64 mac_field;
    uint32  temp;
    
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_mac_delete()\n");
    
    sal_memset(&vment, 0, sizeof(vment));

    /* set Valid field */
    temp = 1;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_VALID, 
        (uint32 *)&vment, &temp));
    
    /* set MAC field */
    SAL_MAC_ADDR_TO_UINT64(mac, mac_field);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_MAC, 
        (uint32 *)&vment, (uint32 *)&mac_field));

    rv = (DRV_SERVICES(unit)->mem_delete)
        (unit, DRV_MEM_MACVLAN, (uint32 *)&vment, 0);

    if (rv == SOC_E_NOT_FOUND) {
        rv = SOC_E_NONE;
    }

    return rv;
}               
    
int bcm_robo_vlan_mac_delete_all(int unit)
{
    int rv = BCM_E_NONE;
    
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_mac_delete_all()\n");
    
    rv = (DRV_SERVICES(unit)->mem_clear)
        (unit, DRV_MEM_MACVLAN);
    return rv;
}               
    

/*
 * Function:
 *      bcm_robo_vlan_translate_get
 * Purpose:
 *      Get vlan translation
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      port - port numebr (ignored in ROBO53242/53262/53115)
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority (ignored in ROBO53242/53262/53115)
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */

int 
bcm_robo_vlan_translate_get (int unit, bcm_port_t port, bcm_vlan_t old_vid,
                            bcm_vlan_t *new_vid, int *prio)
{
    uint32 vt_mode = 0, vt_new_vid = 0;
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id;
    
    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s(unit=%d,port=%d,vid=%d)\n",
            __func__, unit, port, old_vid);
            
    /* check valid port only but doing nothing for ROBO */
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }

    /* The VLAN translation in bcm53243/bcm53262/bcm53115 support VID key 
     * only. The priority key is ignored in ROBO vlan translation.
     */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_get)
            (unit, DRV_VLAN_PROP_ING_VT_SPVID, old_vid, port, &vt_new_vid));
    *new_vid = (bcm_vlan_t)vt_new_vid;

    /* check the target VID's VT_Mode(transparant/mapping)
     *  - dtag is VLAN_XLATE transparant mode.
     *  - if the target VID is assinged as transparent mode, the return value 
     *      will still be "BCM_E_NOTFOUND".
     */    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_get)
            (unit, DRV_VLAN_PROP_VT_MODE, old_vid, port, &vt_mode));
    if (vt_mode != VLAN_VTMODE_MAPPING){
        
        soc_cm_debug(DK_VLAN, 
                "%s,unit=%d,port=%d,vid=%d is not at mapping mode!\n",
                __func__, unit, port, old_vid);
        return BCM_E_NOT_FOUND;
    }
    
    return BCM_E_NONE;
}

/* VLAN translation selection, add VT as Mapping mode 
 *  
 * Note :
 *  1. Vulcan support device basis egress VT only, no port basis and no 
 *      ingress basis.
 */
int 
bcm_robo_vlan_translate_add(int unit, bcm_port_t port, bcm_vlan_t old_vid, 
                  bcm_vlan_t new_vid, int prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id; 
        
    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);
    CHECK_VID(unit, new_vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s\n", __func__);

    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }

    /* check valid priority only but doing nothing for ROBO */
    if ((prio != -1) && ((prio & ~BCM_PRIO_MASK) != 0)) {
        return BCM_E_PARAM;
    }

    /* In robo chips so far, there are only bcm53242/bcm53262/bcm53115 
     *  support VLAN translation. 
     * ----------
     * 1. bcm53242/bcm53262 : support ingress basis vlan_translation only.
     * 2. bcm53115 : support egress basis vlan_translation only.
     *          - actually, bcm53115 is working by ingress VID filtering and 
     *              egress VID translate.
     */ 
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_add)
            (unit, DRV_VLAN_XLAT_INGRESS, port, old_vid, new_vid, 
             prio, VLAN_VTMODE_MAPPING));

    return BCM_E_NONE;

}

/*
 * Function:
 *  bcm_robo_vlan_translate_delete
 * Purpose:
 *  Delete an entry or entries from the VLAN Translation table.
 * Parameters:
 *  unit - Switch chip PCI device unit number.
 *      old_vid - VLAN ID.
 *      port - Port
 * Returns:
 *      BCM_E_NONE   0 or more entries were deleted
 *  BCM_E_XXX    Internal error
 *
 * Note :
 *  1. Vulcan support device basis egress VT only, no port basis and no 
 *      ingress basis.
 */

int 
bcm_robo_vlan_translate_delete(int unit, bcm_port_t port, bcm_vlan_t old_vid)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id; 
    int rv = BCM_E_NONE;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s\n", __func__);
            
    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }
    
    /* In robo chips so far, there are only bcm53242/bcm53262/bcm53115 
     *  support VLAN translation. 
     * ----------
     * 1. bcm53242/bcm53262 : support ingress basis vlan_translation only.
     * 2. bcm53115 : support egress basis vlan_translation only.
     *          - actually, bcm53115 is working by ingress VID filtering and 
     *              egress VID translate.
     */ 
    rv = (DRV_SERVICES(unit)->vlan_vt_delete)
                (unit, DRV_VLAN_XLAT_INGRESS_MAP, port, old_vid);

    if (rv == BCM_E_NOT_FOUND){
        /* the same reason with esw easyrider to return E_NONE to replace
         * E_NOT_FOUND.
         */
        soc_cm_debug(DK_VLAN, 
                "%s, force return from E_Not_Found to E_None!\n", 
                __func__);
        return BCM_E_NONE;
    }
    
    return rv;
}

int
bcm_robo_vlan_translate_delete_all(int unit)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : %s\n", __func__);

    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_delete_all)
                (unit, DRV_VLAN_XLAT_INGRESS_MAP));

    return BCM_E_NONE;
}

/* Egress VLAN translation selection, add VT as Mapping mode 
 *  
 * Note :
 *  1. Vulcan support device basis egress VT only, no port basis and no 
 *      ingress basis.
 */
int
bcm_robo_vlan_translate_egress_add(int unit,bcm_port_t port, 
        bcm_vlan_t old_vid, bcm_vlan_t new_vid, int prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id;
    
    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);
    CHECK_VID(unit, new_vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s\n", __func__);
    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }

    /* check valid priority only but doing nothing for ROBO */
    if ((prio != -1) && ((prio & ~BCM_PRIO_MASK) != 0)) {
        return BCM_E_PARAM;
    }

    /* bcm53115, support egress basis vlan translation through 
     *  1. CFP : for ingress VID filtering and 
     *  2. EVR (Egress Vlan Remark) table for egress VID process.
     */
    if (SOC_IS_ROBO53115(unit)){
        int idt_mode;
        
        /* re-init VT_CFP and EVR table, add this seciton is for the 
         *  regression test will not call bcm_vlan_control_set() to enable 
         *  VLAN translation feature.
         */
        if ((!IS_VT_CFP_INIT)){
            
            /* make sure the requirred CFP was init already */
            BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_init(unit));
    
            /* clear all EVR table entry on each port. and the related 
             *      port basis database.
             */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                                (unit, DRV_VLAN_PROP_EVR_INIT, TRUE));
        }

        /* enable iDT_Mode, add this seciton is for the regression test 
         *  will not call bcm_vlan_control_set() to enable VLAN translation 
         *  feature.
         */
        (DRV_SERVICES(unit)->vlan_property_get)
                        (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE,
                         (uint32 *) &idt_mode);
        if (!idt_mode){
            idt_mode = TRUE;
            (DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, idt_mode);
        }

        /* Vulcan VLAN translation can support global VT only(no port basis)
         * so the port parameter will be ignore here.
         */
        BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_create(
                    unit, old_vid, new_vid, VLAN_VTMODE_MAPPING));
        
    } else {

        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_add)
                (unit, DRV_VLAN_XLAT_EGRESS, port, old_vid, new_vid, 
                 prio, VLAN_VTMODE_MAPPING));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_vlan_translate_egress_get
 * Purpose:
 *      Get vlan egress translation
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      port - port numebr (ignored in ROBO53242/53262/53115)
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority (ignored in ROBO53242/53262/53115)
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int bcm_robo_vlan_translate_egress_get (int unit, bcm_port_t port,
                                       bcm_vlan_t old_vid,
                                       bcm_vlan_t *new_vid,
                                       int *prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id;
    uint32 vt_mode = 0, vt_new_vid = 0;
    
    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s(unit=%d,port=%d,vid=%d)\n",
            __func__, unit, port, old_vid);
            
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }

    /* The VLAN translation in bcm53243/bcm53262/bcm53115 support VID key 
     * only. The priority key is ignored in ROBO vlan translation.
     */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_get)
            (unit, DRV_VLAN_PROP_EGR_VT_SPVID, old_vid, port, &vt_new_vid));
    *new_vid = vt_new_vid;

    /* check the target VID's VT_Mode(transparant/mapping)
     *  - dtag is VLAN_XLATE transparant mode.
     *  - if the target VID is assinged as transparent mode, the return value 
     *      will still be "BCM_E_NOTFOUND".
     */    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_get)
            (unit, DRV_VLAN_PROP_VT_MODE, old_vid, port, &vt_mode));
    if (vt_mode != VLAN_VTMODE_MAPPING){
        
        soc_cm_debug(DK_VLAN, 
                "%s,unit=%d,port=%d,vid=%d is not at mapping mode!\n",
                __func__, unit, port, old_vid);
        return BCM_E_NOT_FOUND;
    }
    
    return BCM_E_NONE;
    
}

/* Egress VLAN translation delection
 *  
 * Note :
 *  1. Vulcan support device basis egress VT only, no port basis and no 
 *      ingress basis.
 */
int
bcm_robo_vlan_translate_egress_delete(int unit,bcm_port_t port, 
            bcm_vlan_t old_vid)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s\n", __func__);
    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }

    /* bcm53115, support egress basis vlan translation through 
     *  1. CFP : for ingress VID filtering and 
     *  2. EVR (Egress Vlan Remark) table for egress VID process.
     */
    if (SOC_IS_ROBO53115(unit)){
        
        int rv = BCM_E_NONE;
        int idt_mode;
        
        if (!IS_VT_CFP_INIT) {
            
            /* make sure the requirred CFP was init already */
            BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_init(unit));
    
            /* clear all EVR table entry on each port. and the related 
             *      port basis database.
             */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                                (unit, DRV_VLAN_PROP_EVR_INIT, TRUE));
        }

        (DRV_SERVICES(unit)->vlan_property_get)
                        (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE,
                         (uint32 *) &idt_mode);
        if (!idt_mode) {
            idt_mode = TRUE;
            (DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, idt_mode);
        }

        /* Vulcan VLAN translation can support global VT only(no port basis)
         * so the port parameter will be ignore here.
         */
        rv = _bcm_robo_vlan_vtcfp_delete(
                        unit, old_vid, VLAN_VTMODE_MAPPING);
        
        if (rv){
            if (rv == BCM_E_NOT_FOUND){
                /* the same reason with esw easyrider to return E_NONE to replace
                 * E_NOT_FOUND.
                 */
                soc_cm_debug(DK_VLAN, 
                        "%s, force return from E_Not_Found to E_None!\n", 
                        __func__);
                return BCM_E_NONE;
            }
            return rv;
        }
    } else {
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_delete)
                    (unit, DRV_VLAN_XLAT_EGRESS, port, old_vid));
    }

    return BCM_E_NONE;
}
    
int
bcm_robo_vlan_translate_egress_delete_all(int unit)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : %s\n", __func__);
            
    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }
    
    /* bcm53115, support egress basis vlan translation through 
     *  1. CFP : for ingress VID filtering and 
     *  2. EVR (Egress Vlan Remark) table for egress VID process.
     */
    if (SOC_IS_ROBO53115(unit)){
        uint32  idt_mode;
        
        if (!IS_VT_CFP_INIT) {
            
            /* make sure the requirred CFP was init already */
            BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_init(unit));
    
            /* clear all EVR table entry on each port. and the related 
             *      port basis database.
             */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                                (unit, DRV_VLAN_PROP_EVR_INIT, TRUE));
        }

        (DRV_SERVICES(unit)->vlan_property_get)
                        (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, &idt_mode);
        if (!idt_mode) {
            idt_mode = TRUE;
            (DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, idt_mode);
        }
        
        /* Vulcan VLAN translation can support global VT only(no port basis)
         * so the port parameter will be ignore here.
         */
        BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_delete_all(
                        unit, VLAN_VTMODE_MAPPING));
    } else {
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_delete_all)
                    (unit, DRV_VLAN_XLAT_EGRESS));
    }

    return BCM_E_NONE;
}

int
bcm_robo_vlan_translate_range_add(int unit, int port, bcm_vlan_t old_vid_low,
                                 bcm_vlan_t old_vid_high, bcm_vlan_t new_vid,
                                 int int_prio)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_translate_range_add()..unavailable\n");
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_vlan_translate_range_get (int unit, bcm_port_t port,
                                      bcm_vlan_t old_vlan_low,
                                      bcm_vlan_t old_vlan_high,
                                      bcm_vlan_t *new_vid, int *prio)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_translate_range_get()..unavailable\n");
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_range_traverse
 * Description :
 *   Traverses over VLAN translate table and call provided callback 
 *   with valid entries.
 * Parameters :
 *      unit            (IN) BCM unit number
 *      cb              (IN) User callback function
 *      user_data       (IN) Pointer to user specific data
 * Return:
 *
 *      BCM_E_XXX
 */
int 
bcm_robo_vlan_translate_range_traverse(int unit, 
                                  bcm_vlan_translate_range_traverse_cb cb,
                                  void *user_data)
{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_vlan_translate_range_delete(int unit, int port,
                                    bcm_vlan_t old_vid_low,
                                    bcm_vlan_t old_vid_high)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_translate_range_delete()..unavailable\n");
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_translate_range_delete_all(int unit)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_translate_range_delete_all()..unavailable\n");
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_translate_action_add(int unit, uint32 port_class,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : %s..unavailable\n",__func__);
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_translate_action_delete(int unit, uint32 port_class,
                                     bcm_vlan_t outer_vlan,
                                     bcm_vlan_t inner_vlan)
{
    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : %s..unavailable\n",__func__);
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_translate_action_delete_all(int unit)
{
    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : %s..unavailable\n",__func__);
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_robo_vlan_translate_action_range_get
 * Description :
 *   Get an entry to the VLAN Translation table, which assigns
 *   VLAN actions for packets matching within the VLAN range(s).
 * Parameters :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress gport (generic port)
 *      outer_vlan_low  (IN) Outer VLAN ID Low
 *      outer_vlan_high (IN) Outer VLAN ID High
 *      inner_vlan_low  (IN) Inner VLAN ID Low
 *      inner_vlan_high (IN) Inner VLAN ID High
 *      action          (OUT) Action for outer and inner tag
 *
 */
int 
bcm_robo_vlan_translate_action_range_get (int unit, bcm_port_t port,
                                         bcm_vlan_t outer_vlan_low,
                                         bcm_vlan_t outer_vlan_high,
                                         bcm_vlan_t inner_vlan_low,
                                         bcm_vlan_t inner_vlan_high,
                                         bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : %s..unavailable\n",__func__);
    return BCM_E_UNAVAIL;
}


int
bcm_robo_vlan_translate_egress_action_add(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : %s..unavailable\n",__func__);
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_translate_egress_action_delete(int unit, int port_class,
                                            bcm_vlan_t outer_vlan,
                                            bcm_vlan_t inner_vlan)
{
    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : %s..unavailable\n",__func__);
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_translate_egress_action_delete_all(int unit)
{
    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN, 
            "BCM API : %s..unavailable\n",__func__);
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_dtag_add
 * Purpose:
 *      Set VLAN translation by trasparent mode
 * Parameters:
 *      unit - RoboSwitch device unit number (driver internal).
 *      port - port id (zero based)
 *      inner_vid - Old VID
 *      outer_vid - New VID
 *      prio - priority
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      1. prot = -1 means this is system basis setting. 
 *          This is differnet with ESW.
 *      2. Per different VLAN translation designing mechanism with ESW chips, 
 *          please check the responsible routines for each chip real behavior.
 */

int
bcm_robo_vlan_dtag_add(int unit,int port, bcm_vlan_t inner_vid,
        bcm_vlan_t outer_vid, int prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id;
    
    CHECK_INIT(unit);
    CHECK_VID(unit, inner_vid);
    CHECK_VID(unit, outer_vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s(unit=%d,port=%d,vid1=%d,vid2=%d,prio=%d)\n",
            __func__, unit, port, inner_vid, outer_vid, prio);
    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }

    /* check valid priority only but doing nothing for ROBO */
    if ((prio != -1) && ((prio & ~BCM_PRIO_MASK) != 0)) {
        return BCM_E_PARAM;
    }

    /* In robo chips so far, there are only bcm53242/bcm53262/bcm53115 
     *  support VLAN translation. 
     * ----------
     * 1. bcm53242/bcm53262 : support ingress basis vlan_translation only.
     * 2. bcm53115 : support egress basis vlan_translation only.
     *          - actually, bcm53115 is working by ingress VID filtering and 
     *              egress VID translate.
     */ 
    if (SOC_IS_ROBO53115(unit)){
        int idt_mode;
        
        /* re-init VT_CFP and EVR table, add this seciton is for the 
         *  regression test will not call bcm_vlan_control_set() to enable 
         *  VLAN translation feature.
         */
        if ((!IS_VT_CFP_INIT)){
            
            /* make sure the requirred CFP was init already */
            BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_init(unit));
    
            /* clear all EVR table entry on each port. and the related 
             *      port basis database.
             */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                                (unit, DRV_VLAN_PROP_EVR_INIT, TRUE));
        }

        /* enable iDT_Mode, add this seciton is for the regression test 
         *  will not call bcm_vlan_control_set() to enable VLAN translation 
         *  feature.
         */
        (DRV_SERVICES(unit)->vlan_property_get)
                        (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE,
                         (uint32 *) &idt_mode);
        if (!idt_mode){
            idt_mode = TRUE;
            (DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, idt_mode);
        }
        
        /* Vulcan VLAN translation can support global VT only(no port basis)
         * so the port parameter will be ignore here.
         */
        BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_create(
                    unit, inner_vid, outer_vid, 
                    VLAN_VTMODE_TRANSPATENT));
        
    } else {

        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_add)
                (unit, DRV_VLAN_XLAT_INGRESS, port, inner_vid, outer_vid, 
                 prio, VLAN_VTMODE_TRANSPATENT));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_vlan_dtag_get
 * Purpose:
 *      Get vlan translation for double tagging
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      port - port numebr (ignored in ROBO53242/53262/53115)
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority (ignored in ROBO53242/53262/53115)
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid and prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *  1.
 *  
 */
int 
bcm_robo_vlan_dtag_get (int unit, bcm_port_t port,
                       bcm_vlan_t old_vid,
                       bcm_vlan_t *new_vid,
                       int *prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id;
    uint32 vt_mode = 0, vt_new_vid = 0;
    
    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s(unit=%d,port=%d,vid=%d)\n",
            __func__, unit, port, old_vid);
            
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }
    
    /* The VLAN translation in bcm53243/bcm53262/bcm53115 support VID key 
     * only. The priority key is ignored in ROBO vlan translation.
     *  - bcm53115 is egress basis design.
     *  - bcm53242 & bcm53262 is ingress basis design.
     */
    if(SOC_IS_ROBO53115(unit)){
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_get)
            (unit, DRV_VLAN_PROP_EGR_VT_SPVID, old_vid, port, &vt_new_vid));

    } else if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_get)
            (unit, DRV_VLAN_PROP_ING_VT_SPVID, old_vid, port, &vt_new_vid));
    } else {
        return BCM_E_UNAVAIL;
    }
    *new_vid = vt_new_vid;

    /* check the target VID's VT_Mode(transparant/mapping)
     *  - dtag is VLAN_XLATE transparant mode.
     *  - if the target VID is assinged as mapping mode, the return value 
     *      will still be "BCM_E_NOTFOUND".
     */    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_get)
            (unit, DRV_VLAN_PROP_VT_MODE, old_vid, port, &vt_mode));
    if (vt_mode != VLAN_VTMODE_TRANSPATENT){
        
        soc_cm_debug(DK_VLAN, 
                "%s,unit=%d,port=%d,vid=%d is not at transparent mode!\n",
                __func__, unit, port, old_vid);
        return BCM_E_NOT_FOUND;
    }
    
    
    return BCM_E_NONE;
}

int
bcm_robo_vlan_dtag_delete(int unit, int port, bcm_vlan_t inner_vid)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id;
    int rv = BCM_E_NONE;
   
    CHECK_INIT(unit);
    CHECK_VID(unit, inner_vid);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s(unit=%d,port=%d,vid=%d)\n",
            __func__, unit, port, inner_vid);
    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }

    /* In robo chips so far, there are only bcm53242/bcm53262/bcm53115 
     *  support VLAN translation. 
     * ----------
     * 1. bcm53242/bcm53262 : support ingress basis vlan_translation only.
     * 2. bcm53115 : support egress basis vlan_translation only.
     *          - actually, bcm53115 is working by ingress VID filtering and 
     *              egress VID translate.
     */ 
    if (SOC_IS_ROBO53115(unit)){
        int idt_mode;
        
        if (!IS_VT_CFP_INIT) {
            
            /* make sure the requirred CFP was init already */
            BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_init(unit));
    
            /* clear all EVR table entry on each port. and the related 
             *      port basis database.
             */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                                (unit, DRV_VLAN_PROP_EVR_INIT, TRUE));
        }

        (DRV_SERVICES(unit)->vlan_property_get)
                        (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE,
                         (uint32 *) &idt_mode);
        if (!idt_mode) {
            idt_mode = TRUE;
            (DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, idt_mode);
        }

        /* Vulcan VLAN translation can support global VT only(no port basis)
         * so the port parameter will be ignore here.
         */
        rv = _bcm_robo_vlan_vtcfp_delete(
                        unit, inner_vid, VLAN_VTMODE_TRANSPATENT);

    } else if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)){
        rv = (DRV_SERVICES(unit)->vlan_vt_delete)
                    (unit, DRV_VLAN_XLAT_INGRESS_TRAN, port, inner_vid);
    } else {
        return BCM_E_UNAVAIL;
    }

    if (rv == BCM_E_NOT_FOUND){
        /* the same with esw easyrider to return E_NONE to replace
         * E_NOT_FOUND.
         */
        soc_cm_debug(DK_VLAN, 
                "%s, force return from E_Not_Found to E_None!\n", 
                __func__);
        return BCM_E_NONE;
    }

    return rv;
}

int
bcm_robo_vlan_dtag_delete_all(int unit)
{
    CHECK_INIT(unit);

    soc_cm_debug(DK_VLAN, 
            "BCM API : %s\n", __func__);
    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    /* In robo chips so far, there are only bcm53242/bcm53262/bcm53115 
     *  support VLAN translation. 
     * ----------
     * 1. bcm53242/bcm53262 : support ingress basis vlan_translation only.
     * 2. bcm53115 : support egress basis vlan_translation only.
     *          - actually, bcm53115 is working by ingress VID filtering and 
     *              egress VID translate.
     */ 
    if (SOC_IS_ROBO53115(unit)){
        int idt_mode;

        if (!IS_VT_CFP_INIT) {
            
            /* make sure the requirred CFP was init already */
            BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_init(unit));
    
            /* clear all EVR table entry on each port. and the related 
             *      port basis database.
             */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                                (unit, DRV_VLAN_PROP_EVR_INIT, TRUE));
        }

        (DRV_SERVICES(unit)->vlan_property_get)
                        (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE,
                         (uint32 *) &idt_mode);
        if (!idt_mode) {
            idt_mode = TRUE;
            (DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, idt_mode);
        }
        
        /* Vulcan VLAN translation can support global VT only(no port basis)
         * so the port parameter will be ignore here.
         */
        BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_delete_all(
                        unit, VLAN_VTMODE_TRANSPATENT));
        
    } else if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)){

        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_vt_delete_all)
                    (unit, DRV_VLAN_XLAT_INGRESS_TRAN));

    } else {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

int 
bcm_robo_vlan_dtag_range_add(int unit, int port,
                            bcm_vlan_t old_vid_low, 
                            bcm_vlan_t old_vid_high,
                            bcm_vlan_t new_vid, int int_prio)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_dtag_range_add()..unavailable\n");
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_vlan_dtag_range_get (int unit, bcm_port_t port,
                             bcm_vlan_t old_vid_low,
                             bcm_vlan_t old_vid_high,
                             bcm_vlan_t *new_vid,
                             int *prio)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_dtag_range_get()..unavailable\n");
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_vlan_dtag_range_delete(int unit, int port, 
                               bcm_vlan_t old_vid_low,
                               bcm_vlan_t old_vid_high)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_dtag_range_delete()..unavailable\n");
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_vlan_dtag_range_delete_all(int unit)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_dtag_range_delete_all()..unavailable\n");
    return BCM_E_UNAVAIL;
}


/* IP4 subnet based vlan selection */
int 
bcm_robo_vlan_ip4_add(int unit, bcm_ip_t ipaddr, bcm_ip_t netmask,
                bcm_vlan_t vid, int prio)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_ip4_add()..unavailable\n");
    return BCM_E_UNAVAIL;
}               
                
int 
bcm_robo_vlan_ip4_delete(int unit, bcm_ip_t ipaddr, bcm_ip_t netmask)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_ip4_delete()..unavailable\n");
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_vlan_ip4_delete_all(int unit)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_ip4_delete_all()..unavailable\n");
    return BCM_E_UNAVAIL;
}               

int 
bcm_robo_vlan_ip_add(int unit, bcm_vlan_ip_t * vlan_ip)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_ip_add()..unavailable\n");
    return BCM_E_UNAVAIL;
}               
                
int 
bcm_robo_vlan_ip_delete(int unit, bcm_vlan_ip_t * vlan_ip)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_ip_delete()..unavailable\n");
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_vlan_ip_delete_all(int unit)
{
    soc_cm_debug(DK_VLAN, 
            "BCM API : bcm_robo_vlan_ip_delete_all()..unavailable\n");
    return BCM_E_UNAVAIL;
}               

/*
 * Function:
 *     bcm_robo_vlan_control_set
 *
 * Purpose:
 *     Set miscellaneous VLAN-specific chip options
 *
 * Parameters:
 *     unit - RoboSwitch device unit number (driver internal).
 *     type - A value from bcm_vlan_control_t enumeration list
 *     arg  - state whose meaning is dependent on 'type'
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_INTERNAL - Chip access failure.
 *     BCM_E_UNAVAIL  - type not supported on unit
 *
 * Note :
 *  In bcm53242, we implement the VLAN prefer MAC2VLAN
 *      (i.e. type=bcmVlanPreferMAC) by disable the port TRUST_VLAN.
 *  This is not quite the same with enterprise switch. To disable port
 *      TRUST_VLAN can perform the prefer MAC2VLAN feature only when the
 *      incoming frame is untagged/1P tagged. And such feature can't work
 *      correctly when the incoming frame is tagged and DA is mist in
 *      MAC2VLAN table.
 *
 */
int
bcm_robo_vlan_control_set(int unit, bcm_vlan_control_t type, int arg)
{
    bcm_pbmp_t  set_bmp;
    int port = 0;

    soc_cm_debug(DK_VLAN,
            "BCM API : bcm_robo_vlan_control_set()..\n");
    switch (type) {
      case bcmVlanDropUnknown:

        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                (unit, DRV_VLAN_PROP_DROP_VTABLE_MISS,
                 (arg ? TRUE :FALSE)));
        break;
      case bcmVlanTranslate:

        if (soc_feature(unit, soc_feature_vlan_translation)) {

            int current_en = 0;
            
            arg = (arg) ? TRUE : FALSE;
            BCM_IF_ERROR_RETURN(bcm_vlan_control_get(unit, 
                   bcmVlanTranslate , &current_en));
            if (current_en == arg) {   /* config no change */
                return BCM_E_NONE;
            }
            
            if (SOC_IS_ROBO53115(unit)){
                if (arg){
                    /* make sure the requirred CFP was init already */
                    BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_init(unit));
    
                    /* clear all EVR table entry on each port. and the related 
                     *      port basis database.
                     */
                    BCM_IF_ERROR_RETURN(
                            (DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_EVR_INIT, TRUE));
                }
                BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->vlan_property_set)
                        (unit, DRV_VLAN_PROP_V2V,(arg ? TRUE :FALSE)));
            } else if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)){
                BCM_IF_ERROR_RETURN(
                            (DRV_SERVICES(unit)->vlan_property_set)
                                (unit, DRV_VLAN_PROP_V2V,
                                (arg ? TRUE :FALSE)));
            } else {
                return BCM_E_UNAVAIL;
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
      case bcmVlanPerPortTranslate:
        if (soc_feature(unit, soc_feature_vlan_translation)) {
            BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->vlan_property_set)
                            (unit, DRV_VLAN_PROP_PER_PORT_TRANSLATION, 
                             (arg ? TRUE :FALSE)));
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
      case bcmVlanIgnorePktTag:
        BCM_PBMP_CLEAR(set_bmp);
        BCM_PBMP_ASSIGN(set_bmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(set_bmp, PBMP_CMIC(unit));

        BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->vlan_prop_port_enable_set)
                        (unit, DRV_VLAN_PROP_TRUST_VLAN_PORT, set_bmp,
                         (arg ? FALSE :TRUE)));
        break;
      case bcmVlanShared:
        /* user change VLAN mode between SVL and IVL on the fly is allowed but 
         * user must take the action to clear all existed L2 entries to 
         * prevent the unexpect forwarding behavior.
         */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                (unit, DRV_VLAN_PROP_VLAN_LEARNING_MODE,
                 (arg ? TRUE :FALSE)));
        break;
      case bcmVlanPreferMAC:
        BCM_PBMP_CLEAR(set_bmp);
        BCM_PBMP_ASSIGN(set_bmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(set_bmp, PBMP_CMIC(unit));
        PBMP_ITER(set_bmp, port) {
            BCM_IF_ERROR_RETURN(
                bcm_vlan_control_port_set(unit, port, 
                bcmVlanPortPreferMAC, (arg ? TRUE:FALSE)));
        }
        break;
      case bcmVlanIntelligentDT:    /* specific type for bcm53115 only */
        /* set bcm53115 into iDT_mode :
         *  
         * Note : 
         *  1. Such setting force bcm53115 at iDT mode, customer need to 
         *      handle some other related configuration to ensure the packet 
         *      learning and forwarding properly. 
         *    - Check processing flow in bcm_robo_port_dtag_mode_set().
         */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_set)
                (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE,(arg ? TRUE :FALSE)));
        break;
      case bcmVlanPreferEgressTranslate:
        /* The definition in header file on this control type is :
         * >> Do egress translation even if ingress FP changes the outer/inner 
         *      VLAN tag(s). 
         * 
         * In current ESW devices, only TRX device can support this specific 
         *  feature.
         *
         * In current robo devices, only BCM53115 can support egress basis 
         *  VT and only egress basis VT supported on bcm53115. There is no 
         *  such preference can be turn on or turn off on bcm53115.
         */
        return BCM_E_UNAVAIL;
      case bcmVlanPreferIP4:
      case bcmVlanSharedID:
      default:
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
    
}

/*
 * Function:
 *     bcm_robo_vlan_control_get
 *
 * Purpose:
 *     Get miscellaneous VLAN-specific chip options
 *
 * Parameters:
 *     unit - RoboSwitch device unit number (driver internal).
 *     type - A value from bcm_vlan_control_t enumeration list
 *     arg  - (OUT) state whose meaning is dependent on 'type'
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_PARAM    - arg points to NULL
 *     BCM_E_INTERNAL - Chip access failure.
 *     BCM_E_UNAVAIL  - type not supported on unit
 */
int 
bcm_robo_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg)
{
    uint32  field_value;
    uint32  port = 0;
    pbmp_t      pbmp;
    uint32  temp;

    soc_cm_debug(DK_VLAN,
            "BCM API : bcm_robo_vlan_control_get()..\n");
    switch (type) {
      case bcmVlanDropUnknown:

        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_get)
                (unit, DRV_VLAN_PROP_DROP_VTABLE_MISS, 
                  &field_value));
        *arg = (field_value) ? TRUE : FALSE;
        break;
      case bcmVlanTranslate:

        if (soc_feature(unit, soc_feature_vlan_translation)) {

            if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit) || 
                    SOC_IS_ROBO53115(unit)){
                uint32 prop_val = 0;
                
                BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->vlan_property_get)
                            (unit, DRV_VLAN_PROP_V2V, &prop_val));
                *arg = (prop_val) ? TRUE : FALSE;
            } else {
                /* special port id to get the device basis value. */
                port = ~port;
                BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->vlan_prop_port_enable_get)
                            (unit, DRV_VLAN_PROP_V2V_PORT, port,  
                            (uint32 *)&pbmp));
                *arg = (BCM_PBMP_IS_NULL(pbmp)) ? FALSE : TRUE;
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
      case bcmVlanPerPortTranslate:
        if (soc_feature(unit, soc_feature_vlan_translation)) {
            BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->vlan_property_get)
                            (unit, DRV_VLAN_PROP_PER_PORT_TRANSLATION, 
                             &temp));
            *arg = temp ? TRUE : FALSE;
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
      case bcmVlanIgnorePktTag:


        /* special assignment on port id to get the device basis value. */
        port = ~port;
        BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->vlan_prop_port_enable_get)
                    (unit, DRV_VLAN_PROP_TRUST_VLAN_PORT,
                    port,  (uint32 *)&pbmp));
        BCM_PBMP_REMOVE(pbmp, PBMP_CMIC(unit));
        *arg = (BCM_PBMP_IS_NULL(pbmp)) ? TRUE : FALSE;
        break;
        
      case bcmVlanShared:
   
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_get)
                (unit, DRV_VLAN_PROP_VLAN_LEARNING_MODE, 
                  &field_value));
        *arg = (field_value) ? FALSE : TRUE;
        break;
      case bcmVlanPreferMAC:
        
        /* robo chips currently have no device based preferMAC setting */
        /* special assignment on port id to get the device basis value. */
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(pbmp, PBMP_CMIC(unit));
        *arg = FALSE;
        PBMP_ITER(pbmp, port) {
            BCM_IF_ERROR_RETURN(
                bcm_vlan_control_port_get(unit, port, 
                bcmVlanPortPreferMAC, (int *) &temp));
            if (temp) {
                *arg = TRUE;
                break;
            }
        }
        break;
      case bcmVlanIntelligentDT:    /* specific type for bcm53115 only */
        /* set bcm53115 into iDT_mode :
         *  
         * Note : 
         *  1. Such setting force bcm53115 at iDT mode, customer need to 
         *      handle some other related configuration to ensure the packet 
         *      learning and forwarding properly. 
         *    - Check processing flow in bcm_robo_port_dtag_mode_set().
         */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_property_get)
                (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, &temp));
        *arg = temp ? TRUE : FALSE;
        break;
      case bcmVlanPreferEgressTranslate:
        /* The definition in header file on this control type is :
         * >> Do egress translation even if ingress FP changes the outer/inner 
         *      VLAN tag(s). 
         * 
         * In current ESW devices, only TRX device can support this specific 
         *  feature.
         *
         * In current robo devices, only BCM53115 can support egress basis 
         *  VT and only egress basis VT supported on bcm53115. There is no 
         *  such preference can be turn on or turn off on bcm53115.
         */
        return BCM_E_UNAVAIL;
      case bcmVlanPreferIP4:
      case bcmVlanSharedID:
      default:
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
} 
  
int
bcm_robo_vlan_control_port_set(int unit, bcm_port_t port,
              bcm_vlan_control_port_t type, int arg)
{
    int rv = BCM_E_NONE;
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id;
    pbmp_t  bmp;

    soc_cm_debug(DK_VLAN,
            "BCM API : bcm_robo_vlan_control_set...()");

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }
    
    BCM_PBMP_CLEAR(bmp);
    BCM_PBMP_PORT_ADD(bmp, port);
            
    switch (type) {
        case bcmVlanPortPreferMAC:
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->port_set)
                (unit, bmp, DRV_PORT_PROP_MAC_BASE_VLAN, 
                (arg ? TRUE:FALSE)));
            break;

        case bcmVlanTranslateIngressEnable:

            BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->vlan_prop_port_enable_set)
                            (unit, DRV_VLAN_PROP_V2V_PORT, bmp,
                            (arg ? TRUE :FALSE)));
            break;
           
        case bcmVlanPortIgnorePktTag:
        
            BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->vlan_prop_port_enable_set)
                            (unit, DRV_VLAN_PROP_TRUST_VLAN_PORT, bmp,
                             (arg ? FALSE :TRUE)));
            break;
        case bcmVlanPortPreferIP4:
        case bcmVlanTranslateIngressMissDrop:
        case bcmVlanTranslateEgressEnable:
        case bcmVlanTranslateEgressMissDrop:
        case bcmVlanTranslateEgressMissUntaggedDrop:
        case bcmVlanLookupMACEnable:
        case bcmVlanLookupIPEnable:
        case bcmVlanPortUseInnerPri:
        case bcmVlanPortVerifyOuterTpid:
        case bcmVlanPortOuterTpidSelect:
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }
    return rv;
}               

int 
bcm_robo_vlan_control_port_get(int unit, bcm_port_t port,
              bcm_vlan_control_port_t type, int * arg)
{
    int rv = BCM_E_NONE;
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id;
    uint32  temp;

    soc_cm_debug(DK_VLAN,
            "BCM API : bcm_robo_vlan_control_port_get()\n");

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
    }

    switch (type) {
        case bcmVlanPortPreferMAC:
            BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->port_get)
                (unit, port, DRV_PORT_PROP_MAC_BASE_VLAN, &temp));
            *arg = (temp) ? TRUE:FALSE;
            break;
        case bcmVlanTranslateIngressEnable:
            BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->vlan_prop_port_enable_get)
                        (unit, DRV_VLAN_PROP_V2V_PORT, port,  &temp));
            *arg = (temp) ? TRUE : FALSE;
            break;
        case bcmVlanPortIgnorePktTag:
            /* special assignment on port id to get the device basis value. */
            BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->vlan_prop_port_enable_get)
                        (unit, DRV_VLAN_PROP_TRUST_VLAN_PORT,
                        port, &temp));
            /* temp==TRUE means VLAN-Tag is trust and won't ignore */
            *arg = (temp) ? FALSE : TRUE;
            break;
 
        case bcmVlanPortPreferIP4:
        case bcmVlanTranslateIngressMissDrop:
        case bcmVlanTranslateEgressEnable:
        case bcmVlanTranslateEgressMissDrop:
        case bcmVlanTranslateEgressMissUntaggedDrop:
        case bcmVlanLookupMACEnable:
        case bcmVlanLookupIPEnable:
        case bcmVlanPortUseInnerPri:
        case bcmVlanPortVerifyOuterTpid:
        case bcmVlanPortOuterTpidSelect:
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }
    return rv;
}
int bcm_robo_vlan_mcast_flood_set(int unit,
                             bcm_vlan_t vlan,
                             bcm_vlan_mcast_flood_t mode)
{
    return BCM_E_UNAVAIL;
}
int bcm_robo_vlan_mcast_flood_get(int unit,
                             bcm_vlan_t vlan,
                             bcm_vlan_mcast_flood_t *mode)
{
    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_robo_vlan_control_vlan_set(int unit, bcm_vlan_t vid,
                          bcm_vlan_control_vlan_t control)
{
    int rv = BCM_E_NONE;
    int disable_mask;
    vlan_entry_t    vt;
    uint32 field_val32;

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        disable_mask = (BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU |
                        BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU |
                        BCM_VLAN_IP4_DISABLE |
                        BCM_VLAN_IP6_DISABLE |
                        BCM_VLAN_IP4_MCAST_DISABLE |
                        BCM_VLAN_IP6_MCAST_DISABLE |
                        BCM_VLAN_IP4_MCAST_L2_DISABLE |
                        BCM_VLAN_IP6_MCAST_L2_DISABLE |
                        BCM_VLAN_MPLS_DISABLE |
                        BCM_VLAN_COSQ_ENABLE |
                        BCM_VLAN_IGMP_SNOOP_DISABLE |
                        BCM_VLAN_PIM_SNOOP_DISABLE |
                        BCM_VLAN_USE_FABRIC_DISTRIBUTION);
        if (control.flags & disable_mask) { 
            return SOC_E_PARAM;
        }

        /* Check if this VLAN is exist */
        if (!(_bcm_robo_vlist_lookup(&robo_vlan_info[unit].list, vid))) {
            return BCM_E_NOT_FOUND;
        }
    
        sal_memset(&vt, 0, sizeof (vt));
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_read)
                        (unit, DRV_MEM_VLAN,
                        (uint32)vid, 1, (uint32 *)&vt));
    
        field_val32 = (control.flags & BCM_VLAN_LEARN_DISABLE)? 1 : 0;
    
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_FWD_MODE,
                        (uint32 *)&vt, (uint32 *)&field_val32));
    
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_write)
                        (unit, DRV_MEM_VLAN,
                        (uint32)vid, 1, (uint32 *)&vt));
    } else {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

int
bcm_robo_vlan_control_vlan_set(int unit, bcm_vlan_t vid, 
                          bcm_vlan_control_vlan_t control)
{
    int         rv;

    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN,
            "BCM API : bcm_robo_vlan_control_vlan_set()..\n");
    CHECK_VID(unit, vid);

    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_control_vlan_set(unit, vid, control);
    BCM_UNLOCK(unit);

    return rv;
}

STATIC int
_bcm_robo_vlan_control_vlan_get(int unit, bcm_vlan_t vid,
                          bcm_vlan_control_vlan_t *control)
{
    int rv = BCM_E_NONE;
    vlan_entry_t    vt;
    uint32 field_value;

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        /* Check if this VLAN is exist */
        if (!(_bcm_robo_vlist_lookup(&robo_vlan_info[unit].list, vid))) {
            return BCM_E_NOT_FOUND;
        }
        /* Upper layer already checks that vid is valid */
        sal_memset(&vt, 0, sizeof (vt));
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_read)
                        (unit, DRV_MEM_VLAN,
                        (uint32)vid, 1, (uint32 *)&vt));
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_VLAN, DRV_MEM_FIELD_FWD_MODE,
                        (uint32 *)&vt, (uint32 *)&field_value));
        control->flags |= (field_value ? BCM_VLAN_LEARN_DISABLE : 0);
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    return rv;
}

int
bcm_robo_vlan_control_vlan_get(int unit, bcm_vlan_t vid,
                          bcm_vlan_control_vlan_t *control)
{
    int         rv;

    CHECK_INIT(unit);
    soc_cm_debug(DK_VLAN,
            "BCM API : bcm_robo_vlan_port_get()..\n");
    CHECK_VID(unit, vid);

    BCM_LOCK(unit);
    rv = _bcm_robo_vlan_control_vlan_get(unit, vid, control);
    BCM_UNLOCK(unit);

    return rv;
}

int
bcm_robo_vlan_ip_action_add(int unit, bcm_vlan_ip_t *vlan_ip,
                           bcm_vlan_action_set_t *action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_mac_action_add(int unit, bcm_mac_t mac, bcm_vlan_action_set_t *action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_mac_action_get(int unit, bcm_mac_t mac, bcm_vlan_action_set_t *action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_mac_action_delete(int unit, bcm_mac_t mac)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_vlan_mac_action_delete_all(int unit)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_vlan_mac_action_traverse(int unit, 
                                 bcm_vlan_mac_action_traverse_cb cb, 
                                 void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * Port-based VLAN actions
 */
/* 
 * Function:
 *      bcm_vlan_port_default_action_set
 * Purpose: 
 *      Set the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_vlan_port_default_action_set(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{   
    return BCM_E_UNAVAIL;
}

/*  
 * Function:
 *      bcm_vlan_port_default_action_get
 * Purpose:
 *      Get the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (OUT) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_vlan_port_default_action_get(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_set
 * Purpose:
 *      Set the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_vlan_port_egress_default_action_set(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_get
 * Purpose:
 *      Get the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (OUT) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */     
int
bcm_robo_vlan_port_egress_default_action_get(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    return BCM_E_UNAVAIL;
}

/*
 * Protocol-based VLAN actions
 */
/*
 * Function   :
 *      bcm_vlan_port_protocol_action_add
 * Description   :
 *      Add protocol based VLAN with specified action.
 *      If the entry already exists, update the action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type
 *      ether     (IN) 16 bit ether type
 *      action    (IN) Action for outer tag and inner tag
 * Note:
 * 
 */
int
bcm_robo_vlan_port_protocol_action_add(int unit,
                                      bcm_port_t port,
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    BCM_IF_ERROR_RETURN(
            _bcm_robo_vlan_port_protocol_action_add(unit, port, frame,
                                                 ether, action));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_add
 * Purpose:
 *      Add a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 *      port_1     - First port in the cross-connect
 *      port_2     - Second port in the cross-connect
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_robo_vlan_cross_connect_add(int unit,
                               bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                               bcm_gport_t port_1, bcm_gport_t port_2)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete
 * Purpose:
 *      Delete a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_robo_vlan_cross_connect_delete(int unit,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete_all
 * Purpose:
 *      Delete all VLAN cross connect entries
 * Parameters:
 *      unit       - Device unit number
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_robo_vlan_cross_connect_delete_all(int unit)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_traverse
 * Purpose:
 *      Walks through the valid cross connect entries and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_robo_vlan_cross_connect_traverse(int unit,
                                    bcm_vlan_cross_connect_traverse_cb cb,
                                    void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_add
 * Description :
 *   Add an entry to the VLAN Translation table, which assigns
 *   VLAN actions for packets matching within the VLAN range(s).
 * Parameters :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress gport (global port)
 *      outer_vlan_low  (IN) Outer VLAN ID Low
 *      outer_vlan_high (IN) Outer VLAN ID High
 *      inner_vlan_low  (IN) Inner VLAN ID Low
 *      inner_vlan_high (IN) Inner VLAN ID High
 *      action          (IN) Action for outer and inner tag
 *
 * Notes :
 *   For translation of double-tagged packets, specify a valid
 *   VLAN ID value for outer_vlan_low/high and inner_vlan_low/high.
 *   For translation of single outer-tagged packets, specify a
 *   valid VLAN ID for outer_vlan_low/high and BCM_VLAN_INVALID
 *   for inner_vlan_low/high. For translation of single inner-tagged
 *   packets, specify a valid VLAN ID for inner_vlan_low/high and
 *   BCM_VLAN_INVALID for outer_vlan_low/high.
 */

int
bcm_robo_vlan_translate_action_range_add(int unit, bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low,
                                        bcm_vlan_t outer_vlan_high,
                                        bcm_vlan_t inner_vlan_low,
                                        bcm_vlan_t inner_vlan_high,
                                        bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);
    CHECK_VID(unit, outer_vlan_low);
    CHECK_VID(unit, outer_vlan_high);
    CHECK_VID(unit, inner_vlan_low);
    CHECK_VID(unit, inner_vlan_high);

    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_delete
 * Description :
 *   Delete an entry from the VLAN Translation table for the
 *   specified VLAN range(s).
 * Parameters :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress gport (global port)
 *      outer_vlan_low  (IN) Outer VLAN ID Low
 *      outer_vlan_high (IN) Outer VLAN ID High
 *      inner_vlan_low  (IN) Inner VLAN ID Low
 *      inner_vlan_high (IN) Inner VLAN ID High
 *
 * Notes :
 *   For translation of double-tagged packets, specify a valid
 *   VLAN ID value for outer_vlan_low/high and inner_vlan_low/high.
 *   For translation of single outer-tagged packets, specify a
 *   valid VLAN ID for outer_vlan_low/high and BCM_VLAN_INVALID
 *   for inner_vlan_low/high. For translation of single inner-tagged
 *   packets, specify a valid VLAN ID for inner_vlan_low/high and
 *   BCM_VLAN_INVALID for outer_vlan_low/high.
 */
int
bcm_robo_vlan_translate_action_range_delete(int unit, bcm_gport_t port,
                                           bcm_vlan_t outer_vlan_low,
                                           bcm_vlan_t outer_vlan_high,
                                           bcm_vlan_t inner_vlan_low,
                                           bcm_vlan_t inner_vlan_high)
{
    CHECK_INIT(unit);
    CHECK_VID(unit, outer_vlan_low);
    CHECK_VID(unit, outer_vlan_high);
    CHECK_VID(unit, inner_vlan_low);
    CHECK_VID(unit, inner_vlan_high);

    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_delete_all
 * Description :
 *     Delete all VLAN range entries from the VLAN Translation table.
 * Parameters :
 *      unit            (IN) BCM unit number
 */

int
bcm_robo_vlan_translate_action_range_delete_all(int unit)
{
    CHECK_INIT(unit);

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_action_get
 * Description   :
 *      Get an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (OUT) Action for outer and inner tag
 */
int 
bcm_robo_vlan_translate_action_get (int unit, bcm_gport_t port,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);

    CHECK_VID(unit, outer_vlan);
    CHECK_VID(unit, inner_vlan);

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_traverse
 * Description   :
 *      Traverse over all translate entries and call given call back with 
 *      new vid and prio.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_robo_vlan_translate_traverse(int unit, bcm_vlan_translate_traverse_cb cb, 
                            void *user_data)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function   :
 *      bcm_vlan_translate_egress_traverse
 * Description   :
 *      Traverse over all translate entries and call given call back with 
 *      new vid and prio.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_robo_vlan_translate_egress_traverse(int unit, 
                                   bcm_vlan_translate_egress_traverse_cb cb, 
                                   void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_traverse
 * Description   :
 *      Traverse over all translate entries and call given call back with 
 *      new vid and prio.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_robo_vlan_dtag_traverse(int unit, 
                       bcm_vlan_dtag_traverse_cb cb, 
                       void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_dtag_range_traverse
 * Description :
 *   Traverses over VLAN double tagging table and call provided callback 
 *   with valid entries.
 * Parameters :
 *      unit            (IN) BCM unit number
 *      cb              (IN) User callback function
 *      user_data       (IN) Pointer to user specific data
 * Return:
 *
 *      BCM_E_XXX
 */
int 
bcm_robo_vlan_dtag_range_traverse(int unit, bcm_vlan_dtag_range_traverse_cb cb,
                             void *user_data)
{  
    return BCM_E_UNAVAIL;
}



/*
 * Function   :
 *      bcm_vlan_translate_action_traverse
 * Description   :
 *      Traverse over all translate entries and call given callback with 
 *      action structure
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
bcm_robo_vlan_translate_action_traverse(int unit, 
                                   bcm_vlan_translate_action_traverse_cb cb, 
                                   void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_traverse
 * Description :
 *   Traverses over VLAN Translation table and call provided callback 
 *   with valid entries.
 * Parameters :
 *      unit            (IN) BCM unit number
 *      cb              (IN) User callback function
 *      user_data       (IN) Pointer to user specific data
 * Return:
 *
 *      BCM_E_XXX
 */
int 
bcm_robo_vlan_translate_action_range_traverse(int unit,
    bcm_vlan_translate_action_range_traverse_cb cb, void *user_data)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function   :
 *      bcm_vlan_translate_egress_action_traverse
 * Description   :
 *      Traverse over all translate entries and call given callback with 
 *      action structure
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
bcm_robo_vlan_translate_egress_action_traverse(int unit, 
                            bcm_vlan_translate_egress_action_traverse_cb cb, 
                            void *user_data)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function   :
 *      bcm_vlan_translate_egress_action_get
 * Description   :
 *      Get an entry to egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (OUT) Action for outer and inner tag
 */
int 
bcm_robo_vlan_translate_egress_action_get (int unit, int port_class,
                                          bcm_vlan_t outer_vlan,
                                          bcm_vlan_t inner_vlan,
                                          bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);
    CHECK_VID(unit, outer_vlan);
    CHECK_VID(unit, inner_vlan);

    return BCM_E_UNAVAIL;
}

/* Create a VLAN queue map entry. */
int
bcm_robo_vlan_queue_map_create(int unit,
                               uint32 flags,
                               int *qmid)
{
    return BCM_E_UNAVAIL;
}

/* Delete a VLAN queue map entry. */
int
bcm_robo_vlan_queue_map_destroy(int unit,
                                int qmid)
{
    return BCM_E_UNAVAIL;
}

/* Delete all VLAN queue map entries. */
int
bcm_robo_vlan_queue_map_destroy_all(int unit)
{
    return BCM_E_UNAVAIL;
}

/* Set a VLAN queue map entry. */
int
bcm_robo_vlan_queue_map_set(int unit,
                            int qmid,
                            int pkt_pri,
                            int cfi,
                            int queue,
                            int color)
{
    return BCM_E_UNAVAIL;
}

/* Get a VLAN queue map entry. */
int
bcm_robo_vlan_queue_map_get(int unit,
                            int qmid,
                            int pkt_pri,
                            int cfi,
                            int *queue,
                            int *color)
{
    return BCM_E_UNAVAIL;
}

/* Attach a queue map object to a VLAN or VFI. */
int
bcm_robo_vlan_queue_map_attach(int unit,
                               bcm_vlan_t vlan,
                               int qmid)
{
    return BCM_E_UNAVAIL;
}

/* Get the queue map object which is attached to a VLAN or VFI. */
int
bcm_robo_vlan_queue_map_attach_get(int unit,
                                   bcm_vlan_t vlan,
                                   int *qmid)
{
    return BCM_E_UNAVAIL;
}

/* Detach a queue map object from a VLAN or VFI. */
int
bcm_robo_vlan_queue_map_detach(int unit,
                               bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}

/* Detach queue map objects from all VLAN or VFI. */
int
bcm_robo_vlan_queue_map_detach_all(int unit)
{
    return BCM_E_UNAVAIL;
}
#ifdef LVL7_FIXUP
int bcm_robo_vlan_evt_entry_add(int unit, int port, int *evr_id,
                                int out_op, bcm_vlan_t out_vid, int in_op, bcm_vlan_t in_vid)
{
  int idt_mode;

  CHECK_INIT(unit);

  CHECK_OP(out_op);
  CHECK_OP(in_op);

  if (out_op == BCM_EVR_OP_FLAG_MODIFY)
    CHECK_VID(unit, out_vid);

  if (in_op == BCM_EVR_OP_FLAG_MODIFY)
    CHECK_VID(unit, in_vid);

  soc_cm_debug(DK_VLAN,
               "BCM API : %s unit %d port %d out_vid %d in_vid %d\n",
               __func__,unit,port, out_vid,in_vid);

  if (!SOC_IS_ROBO53115(unit) || !soc_feature(unit, soc_feature_vlan_translation
))
  {
    return BCM_E_UNAVAIL;
  }

  /* check valid port only but doing nothing for ROBO */
  if ( !SOC_PORT_VALID(unit, port)) {
      return BCM_E_PORT;
  }

  /*  Add if iDT_Mode is enabled, add this seciton is for the regression test
   *  will not call bcm_vlan_control_set() to enable VLAN translation
   *  feature.
   */
  (DRV_SERVICES(unit)->vlan_property_get)
               (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, &idt_mode);
   if (!idt_mode)
   {

     idt_mode = TRUE;
     soc_cm_debug(DK_VLAN, "Enabling iDT mode\n");
     (DRV_SERVICES(unit)->vlan_property_set)
                         (unit, DRV_VLAN_PROP_IDT_MODE_ENABLE, idt_mode);
   }
   BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_evt_entry_add)
            (unit, port, evr_id, out_op, out_vid, in_op, in_vid));

  return BCM_E_NONE;
}

int bcm_robo_vlan_evt_entry_get(int unit, int port, int evr_id,
                                int *out_op, bcm_vlan_t *out_vid, int *in_op, bcm_vlan_t *in_vid)
{
  CHECK_INIT(unit);

  soc_cm_debug(DK_VLAN,
               "BCM API : %s unit %d port %d evr_id %d\n",
               __func__,unit,port,evr_id);

  if (!SOC_IS_ROBO53115(unit) || !soc_feature(unit, soc_feature_vlan_translation
))
  {
    return BCM_E_UNAVAIL;
  }

  /* check valid port only but doing nothing for ROBO */
  if ( !SOC_PORT_VALID(unit, port))
  {
      return BCM_E_PORT;
  }

  BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_evt_entry_get)
            (unit, port, evr_id, out_op, out_vid, in_op, in_vid));
  return BCM_E_NONE;
}

int bcm_robo_vlan_evt_entry_delete(int unit, int port, int evr_id)
{
  CHECK_INIT(unit);

  soc_cm_debug(DK_VLAN,
               "BCM API : %s unit %d port %d evr_id %d\n",
               __func__,unit,port,evr_id);

  if (!SOC_IS_ROBO53115(unit) || !soc_feature(unit, soc_feature_vlan_translation
))
  {
    return BCM_E_UNAVAIL;
  }

  /* check valid port only but doing nothing for ROBO */
  if ( !SOC_PORT_VALID(unit, port))
  {
      return BCM_E_PORT;
  }

  BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_evt_entry_delete)
            (unit, port, evr_id));
  return BCM_E_NONE;
}

int bcm_robo_vlan_evt_entry_modify(int unit, int port, int evr_id, 
                                int out_op, bcm_vlan_t out_vid, int in_op, bcm_vlan_t in_vid)
{
  CHECK_INIT(unit);

  soc_cm_debug(DK_VLAN,
               "BCM API : %s unit %d port %d evr_id %d\n",
               __func__,unit,port,evr_id);

  if (!SOC_IS_ROBO53115(unit) || !soc_feature(unit, soc_feature_vlan_translation))
  {
    return BCM_E_UNAVAIL;
  }
  
  /* check valid port only but doing nothing for ROBO */
  if ( !SOC_PORT_VALID(unit, port)) 
  {
      return BCM_E_PORT;
  }

  BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->vlan_evt_entry_modify)
            (unit, port, evr_id,out_op,out_vid,in_op,in_vid));
  return BCM_E_NONE;
}

#endif

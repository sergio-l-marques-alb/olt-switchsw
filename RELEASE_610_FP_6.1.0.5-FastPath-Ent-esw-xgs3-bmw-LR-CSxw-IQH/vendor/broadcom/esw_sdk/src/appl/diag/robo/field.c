/*
 * $Id: field.c,v 1.1 2011/04/18 17:11:00 mruas Exp $
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
 * Field Processor related CLI commands
 */

#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/core/libc.h>

#include <soc/debug.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>

#include <bcm/init.h>
#include <bcm/field.h>
#include <bcm/error.h>

#include <bcm_int/robo/field.h>


#ifdef BCM_FIELD_SUPPORT

/*
 * Macro:
 *     FP_CHECK_RETURN
 * Purpose:
 *     Check the return value from an API call. Output either a failed
 *     message or okay along with the function name.
 */
#define FP_CHECK_RETURN(retval, funct_name) \
    if ((retval) != BCM_E_NONE) { \
        printk("%s() failed: %s\n", (funct_name), bcm_errmsg(retval)); \
        return CMD_FAIL; \
    } \
    printk("%s(unit=%d) okay\n", (funct_name), unit);

/*
 * Macro:
 *     FP_GET_NUMB
 * Purpose:
 *     Get a numerical value from stdin.
 */
#define FP_GET_NUMB(numb, str, args) \
    if (((str) = ARG_GET(args)) == NULL) { \
        return CMD_USAGE; \
    } \
    (numb) = parse_integer(str);

/*
 * Macro:
 *     FP_GET_PORT
 * Purpose:
 *     Get a numerical value from stdin.
 */
#define FP_GET_PORT(_unit, _port, _str, _args)                   \
    if (((_str) = ARG_GET(_args)) == NULL) {                     \
        return CMD_USAGE;                                        \
    }                                                            \
    if (parse_port((_unit), (_str), &(_port)) < 0) {             \
       FP_ERR(("ERROR: invalid port string: \"%s\"\n", (_str))); \
       return CMD_FAIL;                                          \
    }

/*
 * Marker for last element in qualification table 
 */
#define FP_TABLE_END_STR "tbl_end"

#define FP_STAT_STR_SZ 256
/*
 * local function prototypes
 */

STATIC char *_robo_fp_qual_name(bcm_field_qualify_t qid);
STATIC char *_robo_fp_qual_stage_name(bcm_field_stage_t stage);
STATIC char *_robo_fp_qual_IpType_name(bcm_field_IpType_t type);
STATIC char *_robo_fp_action_name(bcm_field_action_t action);
STATIC char *_robo_fp_control_name(bcm_field_control_t control);
STATIC char *_robo_fp_group_mode_name(bcm_field_group_mode_t mode);

STATIC int robo_fp_action(int unit, args_t *args);
STATIC int robo_fp_action_ports(int unit, args_t *args);
STATIC int robo_fp_action_add(int unit, args_t *args);
STATIC int robo_fp_action_ports_add(int unit, args_t *args);
STATIC int robo_fp_lookup_color(const char *qual_str);
STATIC int robo_fp_action_get(int unit, args_t *args);
STATIC int robo_fp_action_ports_get(int unit, args_t *args);
STATIC int robo_fp_action_remove(int unit, args_t *args);

STATIC int robo_fp_control(int unit, args_t *args);

STATIC int robo_fp_counter(int unit, args_t *args);
STATIC int robo_fp_counter_create(int unit, args_t *args);
STATIC int robo_fp_counter_destroy(int unit, args_t *args);
STATIC int robo_fp_counter_get(int unit, args_t *args);
STATIC int robo_fp_counter_set(int unit, args_t *args);
STATIC int robo_fp_counter_share(int unit, args_t *args);

STATIC int robo_fp_entry(int unit, args_t *args);
STATIC int robo_fp_entry_create(int unit, args_t *args);
STATIC int robo_fp_entry_copy(int unit, args_t *args);
STATIC int robo_fp_entry_destroy(int unit, args_t *args);
STATIC int robo_fp_entry_install(int unit, args_t *args);
STATIC int robo_fp_entry_reinstall(int unit, args_t *args);
STATIC int robo_fp_entry_remove(int unit, args_t *args);
STATIC int robo_fp_entry_prio(int unit, args_t *args);

STATIC int robo_fp_group(int unit, args_t *args, bcm_field_qset_t *qset);
STATIC int robo_fp_group_create(int unit, args_t *args,
                           bcm_field_qset_t *qset);

STATIC void robo_fp_lookup_mode(const char *mode_str, bcm_field_group_mode_t *mode);
STATIC int robo_fp_group_get(int unit, args_t *args);
STATIC int robo_fp_group_destroy(int unit, args_t *args);
STATIC int robo_fp_group_set(int unit, args_t *args, bcm_field_qset_t *qset);
STATIC int robo_fp_group_status_get(int unit, args_t *args);
STATIC int robo_fp_group_mode_get(int unit, args_t *args);

STATIC int robo_fp_range(int unit, args_t *args);
STATIC int robo_fp_range_create(int unit, args_t *args);
STATIC int robo_fp_range_group_create(int unit, args_t *args);
STATIC int robo_fp_range_get(int unit, args_t *args);
STATIC int robo_fp_range_destroy(int unit, args_t *args);
STATIC int robo_fp_group_lookup(int unit, args_t *args);
STATIC int robo_fp_group_enable_set(int unit, bcm_field_group_t gid, int enable);

STATIC int robo_fp_meter(int unit, args_t *args);
STATIC int robo_fp_meter_create(int unit, args_t *args);
STATIC int robo_fp_meter_destroy(int unit, args_t *args);
STATIC int robo_fp_meter_getc(int unit, args_t *args);
STATIC int robo_fp_meter_getp(int unit, args_t *args);
STATIC int robo_fp_meter_setc(int unit, args_t *args);
STATIC int robo_fp_meter_setp(int unit, args_t *args);
STATIC int robo_fp_meter_share(int unit, args_t *args);

STATIC int robo_fp_qset(int unit, args_t *args, bcm_field_qset_t *qset);
STATIC int robo_fp_qset_add(int unit, args_t *args, bcm_field_qset_t *qset);
STATIC int robo_fp_qset_show(bcm_field_qset_t *qset);
/* Lookup functions given string equivalent for type. */
STATIC void robo_fp_lookup_qual(const char *qual_str, bcm_field_qualify_t *qual);
STATIC void robo_fp_lookup_action(const char *act_str, bcm_field_action_t *act);
STATIC void robo_fp_lookup_control(const char *control_str,
                              bcm_field_control_t *control);

STATIC bcm_field_IpType_t robo_fp_lookup_IpType(const char *type_str);
STATIC bcm_field_stage_t  robo_fp_lookup_stage(const char *stage_str);

/* Qualify related functions */
STATIC int robo_fp_qual(int unit, args_t *args);
STATIC int robo_fp_qual_InPorts(int unit, bcm_field_entry_t eid, args_t *args);

/* bcm_field_qualify_XXX exercise functions. */
STATIC int robo_fp_qual_module(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_module_t, bcm_module_t),
              char *qual_str);
STATIC int robo_fp_qual_port(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_port_t, bcm_port_t),
              char *qual_str);
STATIC int robo_fp_qual_modport(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_module_t, bcm_module_t,
                       bcm_port_t, bcm_port_t),
              char *qual_str);
STATIC int robo_fp_qual_trunk(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_trunk_t, bcm_trunk_t),
              char *qual_str);
STATIC int robo_fp_qual_l4port(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_l4_port_t, bcm_l4_port_t),
              char *qual_str);
STATIC int robo_fp_qual_rangecheck(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_field_range_t, int),
              char *qual_str);
STATIC int robo_fp_qual_vlan(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_vlan_t, bcm_vlan_t),
              char *qual_str);
STATIC int robo_fp_qual_ip(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_ip_t, bcm_ip_t),
              char *qual_str);
STATIC int robo_fp_qual_ip6(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_ip6_t, bcm_ip6_t),
              char *qual_str);
STATIC int robo_fp_qual_mac(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_mac_t, bcm_mac_t),
              char *qual_str);
STATIC int robo_fp_qual_udf(int unit, bcm_field_entry_t eid, 
              args_t *args,
              int func(int, bcm_field_entry_t, bcm_field_udf_t, 
                       uint8*, uint8*),
              char *qual_str);
STATIC int robo_fp_qual_snap(int unit, bcm_field_entry_t eid, args_t *args,
                      int func(int, bcm_field_entry_t, bcm_field_snap_header_t, 
                      bcm_field_snap_header_t), char *qual_str);
STATIC int robo_fp_qual_8(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, uint8, uint8),
              char *qual_str);
STATIC int robo_fp_qual_16(int unit, bcm_field_entry_t eid, args_t *args,
                      int func(int, bcm_field_entry_t, uint16, uint16),
                      char *qual_str);
STATIC int robo_fp_qual_32(int unit, bcm_field_entry_t eid, args_t *args,
                      int func(int, bcm_field_entry_t, uint32, uint32),
                      char *qual_str);
STATIC int robo_fp_qual_Decap(int unit, bcm_field_entry_t eid, args_t *args);
STATIC int robo_fp_qual_Stage(int unit, bcm_field_entry_t eid, args_t *args);
STATIC int robo_fp_qual_L2Format(int unit, bcm_field_entry_t eid, args_t *args);
STATIC int robo_fp_qual_IpType(int unit, bcm_field_entry_t eid, args_t *args);
STATIC int robo_fp_qual_IpProtocolCommon(int unit, bcm_field_entry_t eid, args_t *args);
STATIC int robo_fp_qual_L3IntfGroup(int unit, bcm_field_entry_t eid, args_t *args);
STATIC int robo_fp_qual_same(int unit, bcm_field_entry_t eid, args_t *args,
               int func(int, bcm_field_entry_t, uint32),
               char *qual_str);
STATIC int robo_fp_qual_IpFrag(int unit, bcm_field_entry_t eid, args_t *args);

STATIC int robo_fp_user(int unit, args_t *args, bcm_field_udf_spec_t *udf_spec);
STATIC int robo_fp_user_init(int unit, args_t *args, 
                       bcm_field_udf_spec_t *udf_spec);
STATIC int robo_fp_user_set(int unit, args_t *args, 
                       bcm_field_udf_spec_t *udf_spec);
STATIC int robo_fp_user_get(int unit, args_t *args,
                       bcm_field_udf_spec_t *udf_spec);
STATIC int robo_fp_user_create(int unit, args_t *args,
                          bcm_field_udf_spec_t *udf_spec);
STATIC int robo_fp_user_post_ethertype(int unit, args_t *args);

STATIC int robo_fp_thread(int unit, args_t *args);

STATIC bcm_field_udf_t Last_Robo_Created_UDF_Id = 0;

/*
 * Function:
 *      if_field_proc
 * Purpose:
 *      Manage Field Processor (FP)
 * Parameters:
 *      unit - SOC unit #
 *      args - pointer to command line arguments      
 * Returns:
 *    CMD_OK
 */

cmd_result_t
if_robo_field_proc(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    static bcm_field_qset_t     qset;
    static bcm_field_udf_spec_t udf_spec;
#ifdef BROADCOM_DEBUG
    bcm_field_group_t           gid;
    bcm_field_entry_t           eid = 0;
#endif /* BROADCOM_DEBUG */
    bcm_field_status_t          status;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fp action ... */
    if(!sal_strcasecmp(subcmd, "action")) {
        return robo_fp_action(unit, args);
    }
 
    /* BCM.0> fp control ... */
    if(!sal_strcasecmp(subcmd, "control")) {
        return robo_fp_control(unit, args);
    }
 
   /* BCM.0> fp counter ... */
    if(!sal_strcasecmp(subcmd, "counter")) {
        return robo_fp_counter(unit, args);
    }

    /* BCM.0> fp detach */
    if(!sal_strcasecmp(subcmd, "detach")) {
        retval = bcm_field_detach(unit);
        FP_CHECK_RETURN(retval, "bcm_field_detach");
        return CMD_OK;
    }

    /* BCM.0> fp entry ... */
    if(!sal_strcasecmp(subcmd, "entry")) {
        return robo_fp_entry(unit, args);
    }
 
    /* BCM.0> fp group ... */
    if(!sal_strcasecmp(subcmd, "group")) {
        return robo_fp_group(unit, args, &qset);
    }

    /* BCM.0> fp init */
    if(!sal_strcasecmp(subcmd, "init")) {
        retval = bcm_field_init(unit);
        FP_CHECK_RETURN(retval, "bcm_field_init");
        BCM_FIELD_QSET_INIT(qset);
        return CMD_OK;
    }

    /* BCM.0> fp install - deprecated, use fp entry install */
    if(!sal_strcasecmp(subcmd, "install")) {
        return robo_fp_entry_install(unit, args);
    }

    /* BCM.0> fp range ... */
    if(!sal_strcasecmp(subcmd, "range")) {
        return robo_fp_range(unit, args);
    }

    /* BCM.0> fp meter ... */
    if(!sal_strcasecmp(subcmd, "meter")) {
        return robo_fp_meter(unit, args);
    }

    /* BCM.0> fp status*/
    if(!sal_strcasecmp(subcmd, "status")) {
        FP_VERB(("FP status\n"));
        retval = bcm_field_status_get(unit, &status);
        FP_CHECK_RETURN(retval, "bcm_field_status_get");
        printk("field_status={group_total=%d, group_free=%d, flags=0x%x}\n",
               status.group_total, status.group_free, status.flags);
        return CMD_OK;
    }

    /* BCM.0> fp qual ... */
    if(!sal_strcasecmp(subcmd, "qual")) {
        return robo_fp_qual(unit, args);
    }

    /* BCM.0> fp qset ... */
    if(!sal_strcasecmp(subcmd, "qset")) {
        return robo_fp_qset(unit, args, &qset);
    }

#ifdef BROADCOM_DEBUG
    /* BCM.0> fp show ...*/
    if(!sal_strcasecmp(subcmd, "show")) {
        if ((subcmd = ARG_GET(args)) != NULL) {
            /* BCM.0> fp show entry ...*/
            if(!sal_strcasecmp(subcmd, "entry")) {
                if ((subcmd = ARG_GET(args)) == NULL) {
                    return CMD_USAGE;
                } else {
                    /* BCM.0> fp show entry 'eid' */ 
                    eid = parse_integer(subcmd);
                    bcm_field_entry_dump(unit, eid);
                    return CMD_OK;
                }
            }
            /* BCM.0> fp show group ...*/
            if(!sal_strcasecmp(subcmd, "group")) {
                FP_GET_NUMB(gid, subcmd, args);
                bcm_field_group_dump(unit, gid);
                return CMD_OK;
            }
            /* BCM.0> fp show qset */
            if(!sal_strcasecmp(subcmd, "qset")) {
                robo_fp_qset_show(&qset);
                return CMD_OK;
            }
            return CMD_NFND;
        } else {
            /* BCM.0> fp show */
            bcm_field_show(unit, "FP");
            return CMD_OK;
        }
    }
#endif /* BROADCOM_DEBUG */
    /* BCM.0> fp user ... */
    if(!sal_strcasecmp(subcmd, "user")) {
        return robo_fp_user(unit, args, &udf_spec);
    }

    if(!sal_strcasecmp(subcmd, "thread")) {
        return robo_fp_thread(unit, args);
    }

    return CMD_USAGE;
}

/*
 * Function:
 *     _robo_fp_qual_name
 * Purpose:
 *     Translate a Qualifier enum value to a text string.
 * Parameters:
 *     Enum value from bcm_field_qualify_e. (ex.bcmFieldQualifyInPorts)
 * Returns:
 *     Text name of indicated qualifier enum value.
 */
STATIC char *
_robo_fp_qual_name(bcm_field_qualify_t qid)
{
    /* Text names of the enumerated qualifier IDs. */
    char *qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;

    return (qid >= bcmFieldQualifyCount ? "??" : qual_text[qid]);
}

/*
 * Function:
 *     _robo_fp_qual_stage_name
 * Purpose:
 *     Translate a stage qualifier enum value to a text string.
 * Parameters:
 *     Enum value from bcm_field_stage_e. (ex.bcmFieldStageIngressEarly)
 * Returns:
 *     Text name of indicated stage qualifier enum value.
 */
STATIC char *
_robo_fp_qual_stage_name(bcm_field_stage_t stage)
{
    /* Text names of the enumerated qualifier stage values. */
    /* All these are prefixed with "bcmFieldStage" */
    char *stage_text[] = BCM_FIELD_STAGE_STRINGS;

    assert(COUNTOF(stage_text) == bcmFieldStageCount);

    return (stage >= bcmFieldStageCount ? "??" : stage_text[stage]);
}

/*
 * Function:
 *     _robo_fp_qual_L2Format_name
 * Purpose:
 *     Translate L2Format enum value to a text string.
 * Parameters:
 *     Enum value from bcm_field_L2Format_e. (ex.bcmFieldL2FormatSnap)
 * Returns:
 *     Text name of indicated L2Format qualifier enum value.
 */
STATIC char *
_robo_fp_qual_L2Format_name(bcm_field_L2Format_t type)
{
    /* Text names of the enumerated qualifier L2Format values. */
    /* All these are prefixed with "bcmFieldL2Format" */
    char *L2Format_text[bcmFieldL2FormatCount] = BCM_FIELD_L2FORMAT_STRINGS;

    assert(COUNTOF(L2Format_text) == bcmFieldL2FormatCount);

    return (type >= bcmFieldL2FormatCount ? "??" : L2Format_text[type]);
}

/*
 * Function:
 *     _robo_fp_qual_IpType_name
 * Purpose:
 *     Translate IpType enum value to a text string.
 * Parameters:
 *     Enum value from bcm_field_IpType_e. (ex.bcmFieldIpTypeNonIp)
 * Returns:
 *     Text name of indicated IpType qualifier enum value.
 */
STATIC char *
_robo_fp_qual_IpType_name(bcm_field_IpType_t type)
{
    /* Text names of the enumerated qualifier IpType values. */
    /* All these are prefixed with "bcmFieldIpType" */
    char *IpType_text[bcmFieldIpTypeCount] = BCM_FIELD_IPTYPE_STRINGS;

    assert(COUNTOF(IpType_text) == bcmFieldIpTypeCount);

    return (type >= bcmFieldIpTypeCount ? "??" : IpType_text[type]);
}

/*
 * Function:
 *     _robo_fp_qual_IpProtocolCommon_name
 * Purpose:
 *     Translate IpProtocolCommon enum value to a text string.
 * Parameters:
 *     Enum value from bcm_field_IpProtocolCommon_e. (ex.bcmFieldIpProtocolCommonTcp)
 * Returns:
 *     Text name of indicated IpProtocolCommon qualifier enum value.
 */
STATIC char *
_robo_fp_qual_IpProtocolCommon_name(bcm_field_IpProtocolCommon_t type)
{
    /* Text names of the enumerated qualifier IpProtocolCommon values. */
    /* All these are prefixed with "bcmFieldIpProtocolCommon" */
    char *IpProtocolCommon_text[bcmFieldIpProtocolCommonCount] = BCM_FIELD_IPPROTOCOLCOMMON_STRINGS;

    assert(COUNTOF(IpProtocolCommon_text) == bcmFieldIpProtocolCommonCount);

    return (type >= bcmFieldIpProtocolCommonCount ? "??" : IpProtocolCommon_text[type]);
}

/*
 * Function:
 *     _robo_fp_action_name
 * Purpose:
 *     Return text name of indicated action enum value.
 */
STATIC char *
_robo_fp_action_name(bcm_field_action_t action)
{
    /* Text names of Actions. These are used for debugging output and CLIs.
     * Note that the order needs to match the bcm_field_action_t enum order.
     */
    char *action_text[] = BCM_FIELD_ACTION_STRINGS;
    assert(COUNTOF(action_text)     == bcmFieldActionCount);

    return (action >= bcmFieldActionCount ? "??" : action_text[action]);
}

/*
 * Function:
 *     _robo_fp_control_name
 * Purpose:
 *     Return text name of indicated control enum value.
 */
STATIC char *
_robo_fp_control_name(bcm_field_control_t control)
{
    /* Text names of Controls. These are used for debugging output and CLIs.
     * Note that the order needs to match the bcm_field_control_t enum order.
     */
    char *control_text[] = BCM_FIELD_CONTROL_STRINGS;
    assert(COUNTOF(control_text)     == bcmFieldControlCount);

    return (control >= bcmFieldControlCount ? "??" : control_text[control]);
}

/*
 * Function:
 *     _robo_fp_group_mode_name
 * Purpose:
 *     Return text name of indicated group mode enum value.
 */
STATIC char *
_robo_fp_group_mode_name(bcm_field_group_mode_t mode)
{
    char *mode_text[bcmFieldGroupModeCount] = BCM_FIELD_GROUP_MODE_STRINGS;

    return (mode >= bcmFieldGroupModeCount ? "??" : mode_text[mode]);
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_action(int unit, args_t *args)
{
    char*               subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    /* BCM.0> fp action ports ... */
    if(!sal_strcasecmp(subcmd, "ports")) {
        return robo_fp_action_ports(unit, args);
    }
    /* BCM.0> fp action add ... */
    if(!sal_strcasecmp(subcmd, "add")) {
        return robo_fp_action_add(unit, args);
    }
    /* BCM.0> fp action get ... */
    if(!sal_strcasecmp(subcmd, "get")) {
        return robo_fp_action_get(unit, args);
    }
    /* BCM.0> fp action remove... */
    if(!sal_strcasecmp(subcmd, "remove")) {
        return robo_fp_action_remove(unit, args);
    }

    return CMD_USAGE;
}

/*
 * Function:
 *     robo_fp_action_ports
 * Purpose: 
 *     FP action which takes pbmp as argument
 * Parmameters:
 *     unit - BCM unit number
 *     args - command line arguments
 * Returns:
 */
STATIC int
robo_fp_action_ports(int unit, args_t *args)
{
    char*               subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    /* BCM.0> fp action ports add ... */
    if(!sal_strcasecmp(subcmd, "add")) {
        return robo_fp_action_ports_add(unit, args);
    }
    /* BCM.0> fp action ports get ... */
    if(!sal_strcasecmp(subcmd, "get")) {
        return robo_fp_action_ports_get(unit, args);
    }

    return CMD_USAGE;
}

/*
 * Function:
 *     robo_fp_action_add
 * Purpose:
 *     Add an action to an entry. Can take action either in the form
 *     of a string or digit corresponding to action order in 
 *     bcm_field_action_t enum.
 * Parmameters:
 *     unit - BCM unit number
 *     args - command line arguments
 * Returns:
 */
STATIC int
robo_fp_action_add(int unit, args_t *args)
{
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    bcm_field_action_t  action;
    int                 retval;
    int                 p0 = 0, p1 = 0;

    FP_GET_NUMB(eid, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 

    if (isint(subcmd)) {
        action = parse_integer(subcmd);
    } else {
        robo_fp_lookup_action(subcmd, &action);
        if (action == bcmFieldActionCount) {
            printk("Unknown action: %s\n", subcmd);
            return CMD_FAIL;
        }
    }

    /* Read the action parameters (p0 and p1).*/ 
    switch (action) {
        case bcmFieldActionRedirect:
        case bcmFieldActionRpRedirectPort:
        case bcmFieldActionGpRedirectPort:
        case bcmFieldActionMirrorIngress:
        case bcmFieldActionRpMirrorIngress:
        case bcmFieldActionGpMirrorIngress:
        case bcmFieldActionMirrorEgress:
            FP_GET_NUMB(p0, subcmd, args);
            if ((subcmd  = ARG_GET(args)) == NULL) { 
                return CMD_USAGE; 
            } 
            if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
                if (isint(subcmd)) {
                    /* 
                     * Port id 0x3f is a magic number of BCM53242.
                     * It represents flood to all ports.
                     * The magic number need to be processed seperately
                     * and should not do port id validation.
                     */
                    p1 = parse_integer(subcmd);
                    if ((p1 != FP_ACT_CHANGE_FWD_ALL_PORTS) &&
                        (p1 != FP_ACT_CHANGE_FWD_MIRROT_TO_PORT)) {
                        if (parse_port(unit, subcmd, &p1) < 0) {
                            printk("ERROR: invalid port string: \"%s\"\n", subcmd);
                            return CMD_FAIL;
                        }
                    }
                } else if (parse_port(unit, subcmd, &p1) < 0) {
                    printk("ERROR: invalid port string: \"%s\"\n", subcmd);
                    return CMD_FAIL;
                }
            } else if (parse_port(unit, subcmd, &p1) < 0) {
                printk("ERROR: invalid port string: \"%s\"\n", subcmd);
                return CMD_FAIL;
            }
            break;
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionEgressMask:
            if ((subcmd  = ARG_GET(args)) == NULL) { 
                return CMD_USAGE; 
            } 
            if (parse_pbmp(unit, subcmd, (soc_pbmp_t*)&p0) < 0) {
                printk("%s: Error: unrecognized port bitmap: %s\n",
                ARG_CMD(args), subcmd);
                return CMD_FAIL;
            }
            break;
        case bcmFieldActionDropPrecedence:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionRpDropPrecedence:
            if ((subcmd  = ARG_GET(args)) == NULL) { 
                return CMD_USAGE; 
            }
            if (isint(subcmd)) {
                p0 = parse_integer(subcmd);
        } else {
                p0 = robo_fp_lookup_color(subcmd);
        }
            break;
        default:
            FP_GET_NUMB(p0, subcmd, args);
            FP_GET_NUMB(p1, subcmd, args);
    }

    FP_VERB(("FP action add eid=%d, action=%s, p0=0x%x, p1=0x%x\n", eid,
             _robo_fp_action_name(action), p0, p1));
    retval = bcm_field_action_add(unit, eid, action, p0, p1);
    FP_CHECK_RETURN(retval, "bcm_field_action_add");

    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_action_ports_add
 * Purpose: 
 *     FP action add which takes pbmp as argument
 * Parmameters:
 *     unit - BCM unit number
 *     args - command line arguments
 * Returns:
 */
STATIC int
robo_fp_action_ports_add(int unit, args_t *args)
{
    char               *subcmd = NULL;
    bcm_field_entry_t   eid;
    bcm_field_action_t  action;
    int                 retval;
    bcm_pbmp_t          pbmp;
#ifdef BROADCOM_DEBUG
    char                buf[BCM_FIELD_ACTION_WIDTH_MAX];
    char                buf_pbmp[SOC_PBMP_FMT_LEN];
#endif /* BROADCOM_DEBUG */

    FP_GET_NUMB(eid, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 

    if (isint(subcmd)) {
        action = parse_integer(subcmd);
    } else {
        action = parse_field_action(subcmd);
        if (action == bcmFieldActionCount) {
            printk("Unknown action: %s\n", subcmd);
            return CMD_FAIL;
        }
    }

    if (action != bcmFieldActionRedirectPbmp) {
        printk("Error: Unrecognized action\n");
        return CMD_FAIL;
    }

    /* Read the action parameters pbmp.*/ 
    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    } else if (parse_pbmp(unit, subcmd, &pbmp) < 0) {
        printk("%s: Error: unrecognized port bitmap: %s\n",
                ARG_CMD(args), subcmd);
        return CMD_FAIL;
    }

    FP_VERB(("FP action ports add eid=%d, action=%s, pbmp=%s\n", eid,
             format_field_action(buf, action, 1), 
             format_pbmp(unit, buf_pbmp, sizeof(buf_pbmp), pbmp)));
    retval = bcm_field_action_ports_add(unit, eid, action, pbmp);
    FP_CHECK_RETURN(retval, "bcm_field_action_ports_add");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_lookup_color(const char *qual_str) {
    assert(qual_str != NULL);
    
    if (!sal_strcasecmp(qual_str, "preserve")) {
        return BCM_FIELD_COLOR_PRESERVE;
    } else if (!sal_strcasecmp(qual_str, "green")) {
        return BCM_FIELD_COLOR_GREEN;
    } else if (!sal_strcasecmp(qual_str, "yellow")) {
        return BCM_FIELD_COLOR_YELLOW;
    } else if (!sal_strcasecmp(qual_str, "red")) {
        return BCM_FIELD_COLOR_RED;
    }

    return -1;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_action_get(int unit, args_t *args)
{
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    int                 retval;
    bcm_field_action_t  action;
    uint32              p0 = 0, p1 = 0;

    FP_GET_NUMB(eid, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 

    if (isint(subcmd)) {
        action = parse_integer(subcmd);
    } else {
        robo_fp_lookup_action(subcmd, &action);
        if (action == bcmFieldActionCount) {
            printk("Unknown action: %s\n", subcmd);
            return CMD_FAIL;
        }
    }

    retval = bcm_field_action_get(unit, eid, action, &p0, &p1);
    FP_CHECK_RETURN(retval, "bcm_field_action_get");
    printk("FP action get: action=%s, p0=%d, p1=%d\n", 
           _robo_fp_action_name(action), p0, p1);

    return CMD_OK;
}

/*
 * Function: 
 *     robo_fp_action_ports_get
 * Purpose: 
 *     FP action get which takes pbmp as argument
 * Parmameters:
 *     unit - BCM unit number
 *     args - command line arguments
 * Returns:
 */
STATIC int
robo_fp_action_ports_get(int unit, args_t *args)
{
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    int                 retval;
    bcm_field_action_t  action;
    bcm_pbmp_t          pbmp;
    char                buf[BCM_FIELD_ACTION_WIDTH_MAX];
    char                buf_pbmp[SOC_PBMP_FMT_LEN];

    FP_GET_NUMB(eid, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 

    if (isint(subcmd)) {
        action = parse_integer(subcmd);
    } else {
        action = parse_field_action(subcmd);
        if (action == bcmFieldActionCount) {
            printk("Unknown action: %s\n", subcmd);
            return CMD_FAIL;
        }
    }

    if (action != bcmFieldActionRedirectPbmp) {
        printk("Error: Unrecognized action\n");
        return CMD_FAIL;
    }

    retval = bcm_field_action_ports_get(unit, eid, action, &pbmp);
    FP_CHECK_RETURN(retval, "bcm_field_action_ports_get");
    printk("FP action ports get: action=%s, pbmp=%s\n", 
           format_field_action(buf, action, 1), 
           SOC_PBMP_FMT(pbmp, buf_pbmp));

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_action_remove(int unit, args_t *args)
{
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    int                 retval;
    bcm_field_action_t  action;
    

    FP_GET_NUMB(eid, subcmd, args);

    if ((subcmd = ARG_GET(args)) == NULL) {
        retval = bcm_field_action_remove_all(unit, eid);
        FP_CHECK_RETURN(retval, "bcm_field_action_remove_all");
    } else {
        if (isint(subcmd)) {
            action = parse_integer(subcmd);
        } else {
            robo_fp_lookup_action(subcmd, &action);
            if (action == bcmFieldActionCount) {
                printk("Unknown action: %s\n", subcmd);
                return CMD_FAIL;
            }
        }
        retval = bcm_field_action_remove(unit, eid, action);
        FP_CHECK_RETURN(retval, "bcm_field_action_remove");
    }

    return CMD_OK;
}

/*
 * Function:
 *    robo_fp_control
 * Purpose:
 *    Set/Get field control values.
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_control(int unit, args_t *args)
{
    char*               subcmd = NULL;
    bcm_field_control_t element;
    uint32              status = 0;
    int                 retval = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        element = parse_integer(subcmd);
    } else {
        robo_fp_lookup_control(subcmd, &element);
        if (element == bcmFieldControlCount) {
            printk("Unknown control: %s\n", subcmd);
            return CMD_FAIL;
        }
    }

    switch (element) {
        case bcmFieldControlStage:
        case bcmFieldControlColorIndependent:
            break;
        default:
            printk("Control %d unknown\n", element);
            return CMD_OK;
    }

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fp control <control_number>*/
        FP_VERB(("bcm_field_control_get(element=%d)\n", element));
        retval = bcm_field_control_get(unit, element, &status);
        FP_CHECK_RETURN(retval, "bcm_field_control_get");
        printk("FP element=%d: status=%d\n", element, status);
    } else {
        /* BCM.0> fp control <control_number> <status>*/
        if (element == bcmFieldControlStage && !isint(subcmd)) {
            status = robo_fp_lookup_stage(subcmd);
            if (status == bcmFieldStageCount) {
                printk("Unknown stage: %s\n", subcmd);
                return CMD_FAIL;
            }
    } else {
            status = parse_integer(subcmd);
        }
        FP_VERB(("bcm_field_control_set(element=%d, status=%d)\n", element,
                 status));
        retval = bcm_field_control_set(unit, element, status);
        FP_CHECK_RETURN(retval, "bcm_field_control_set");
    }
    return CMD_OK;
}

/*
 * Function: robo_fp_counter
 * Purpose:
 *     Handle ">fp counter {create|set|get|share|destroy}..." commands.
 *
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_counter(int unit, args_t *args)
{
    char*               subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fp counter create ... */
    if(!sal_strcasecmp(subcmd, "create")) {
        return robo_fp_counter_create(unit, args);
    }
    /* BCM.0> fp counter set ... */
    if(!sal_strcasecmp(subcmd, "set")) {
        return robo_fp_counter_set(unit, args);
    }
    /* BCM.0> fp counter get ... */
    if(!sal_strcasecmp(subcmd, "get")) {
        return robo_fp_counter_get(unit, args);
    }
    /* BCM.0> fp counter share ... */
    if(!sal_strcasecmp(subcmd, "share")) {
        return robo_fp_counter_share(unit, args);
    }
    /* BCM.0> fp counter destroy ... */
    if(!sal_strcasecmp(subcmd, "destroy")) {
        return robo_fp_counter_destroy(unit, args);
    }
 
    return CMD_USAGE;
}


/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_counter_create(int unit, args_t *args)
{
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    int                 retval;

    FP_GET_NUMB(eid, subcmd, args);

    retval = bcm_field_counter_create(unit, eid);
    FP_CHECK_RETURN(retval, "bcm_field_counter_create");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_counter_destroy(int unit, args_t *args)
{
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    int                 retval;

    FP_GET_NUMB(eid, subcmd, args);

    retval = bcm_field_counter_destroy(unit, eid);
    FP_CHECK_RETURN(retval, "bcm_field_counter_destroy");

    return CMD_OK;
}


/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_counter_get(int unit, args_t *args)
{
    char*               subcmd = NULL;
    char                buf_val[32]; /* format string */ 
    bcm_field_entry_t   eid;
    int                 retval;
    int                 counter_num;
    uint64              val;

    FP_GET_NUMB(eid, subcmd, args);
    FP_GET_NUMB(counter_num, subcmd, args);

    retval = bcm_field_counter_get(unit, eid, counter_num, &val);
    FP_CHECK_RETURN(retval, "bcm_field_counter_get");
    format_uint64_decimal(buf_val, val, ',');
    printk("FP counter get: eid=%d, counter_num=%d, val=%s\n", 
           eid, counter_num, buf_val);
    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_counter_set
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_counter_set(int unit, args_t *args)
{
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    int                 counter_num;
    int                 retval;
    uint64              val;

    FP_GET_NUMB(eid, subcmd, args);
    FP_GET_NUMB(counter_num, subcmd, args);

    if ((subcmd = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 
    val = parse_uint64(subcmd);

    FP_VERB(("_counter_set(eid=%d, counter_num=%d, val=%ld)\n",
            eid, counter_num, val));
    retval = bcm_field_counter_set(unit, eid, counter_num, val);
    FP_CHECK_RETURN(retval, "bcm_field_counter_set");
    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_counter_share(int unit, args_t *args)
{
    int                 retval = CMD_OK;
    char*               subcmd = NULL;
    int                 src_eid;
    int                 dst_eid;

    FP_GET_NUMB(src_eid, subcmd, args);
    FP_GET_NUMB(dst_eid, subcmd, args);

    retval = bcm_field_counter_share(unit, src_eid, dst_eid);
    FP_CHECK_RETURN(retval, "bcm_field_counter_share");

    return CMD_OK;
}
 
/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_entry(int unit, args_t *args)
{
    char*               subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fp entry create ... */
    if(!sal_strcasecmp(subcmd, "create")) {
        return robo_fp_entry_create(unit, args);
    }
    /* BCM.0> fp entry copy ... */
    if(!sal_strcasecmp(subcmd, "copy")) {
        return robo_fp_entry_copy(unit, args);
    }
    /* BCM.0> fp entry destroy ... */
    if(!sal_strcasecmp(subcmd, "destroy")) {
        return robo_fp_entry_destroy(unit, args);
    }
    /* BCM.0> fp entry install ... */
    if(!sal_strcasecmp(subcmd, "install")) {
        return robo_fp_entry_install(unit, args);
    }
    /* BCM.0> fp entry reinstall ... */
    if(!sal_strcasecmp(subcmd, "reinstall")) {
        return robo_fp_entry_reinstall(unit, args);
    }
    /* BCM.0> fp entry remove ... */
    if(!sal_strcasecmp(subcmd, "remove")) {
        return robo_fp_entry_remove(unit, args);
    }
    /* BCM.0> fp entry prio ... */
    if(!sal_strcasecmp(subcmd, "prio")) {
        return robo_fp_entry_prio(unit, args);
    }

    return CMD_USAGE;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_entry_create(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_group_t           gid;
    bcm_field_entry_t           eid;

    FP_GET_NUMB(gid, subcmd, args);
 
    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fp entry create 'gid'  */
        FP_VERB(("_entry_create unit=%d, gid=%d\n", unit, gid));
        retval = bcm_field_entry_create(unit, gid, &eid);
        FP_CHECK_RETURN(retval, "bcm_field_entry_create");
    } else {
        /* BCM.0> fp entry create 'gid' 'eid' */
        eid = parse_integer(subcmd);
        FP_VERB(("_entry_create unit=%d, gid=%d, eid=%d\n", unit, gid, eid));
        retval = bcm_field_entry_create_id(unit, gid, eid);
        FP_CHECK_RETURN(retval, "bcm_field_entry_create_id");
    }
    printk("EID %d created!\n", eid);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_entry_copy(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_entry_t           src_eid, dst_eid = -111;

    FP_GET_NUMB(src_eid, subcmd, args);
    subcmd = ARG_GET(args);

    if (subcmd ) {
        /* BCM.0> fp entry copy 'src_eid' 'dst_eid'  */
        dst_eid = parse_integer(subcmd);
        FP_VERB(("bcm_field_entry_copy_id(src_eid=%d, dst_eid=%d)\n",
                 src_eid, dst_eid));
        retval = bcm_field_entry_copy_id(unit, src_eid, dst_eid);
        FP_CHECK_RETURN(retval, "bcm_field_entry_copy_id");
    } else {
        /* BCM.0> fp entry copy 'src_eid' */
        FP_VERB(("bcm_field_entry_copy(src_eid=%d)\n", src_eid));
        retval = bcm_field_entry_copy(unit, src_eid, &dst_eid);
        FP_CHECK_RETURN(retval, "bcm_field_entry_copy");
        printk("EID %d created!\n", dst_eid);
    }

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_entry_destroy(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_entry_t           eid;

    if ((subcmd = ARG_GET(args)) == NULL) {
        FP_VERB(("bcm_field_entry_destroy_all(unit=%d)\n", unit));
        retval = bcm_field_entry_destroy_all(unit);
        FP_CHECK_RETURN(retval, "bcm_field_entry_destroy_all");
        return CMD_OK;
    } else {
        eid = parse_integer(subcmd);
        FP_VERB(("bcm_field_entry_destroy(unit=%d, eid=%d)\n", unit, eid));
        retval = bcm_field_entry_destroy(unit, eid);
        FP_CHECK_RETURN(retval, "bcm_field_entry_destroy");
        return CMD_OK;
    }
    return CMD_USAGE;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_entry_install(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_entry_t           eid;
 
    /* BCM.0> fp detach 'eid' */
    FP_GET_NUMB(eid, subcmd, args);

    FP_VERB(("_entry_install eid=%d\n", eid));
    retval = bcm_field_entry_install(unit, eid);

    /* Check return status. */
    printk("bcm_field_entry_install(unit=%d, entry=%d) ", unit, eid);
    if ((retval) != BCM_E_NONE) {
        printk("failed: %s\n", bcm_errmsg(retval));
        return CMD_FAIL;
    }
    printk("okay\n");
    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_entry_reinstall
 * Purpose:
 * Parmameters:
 * Returns:
 *     CMD_OK 
 */
STATIC int
robo_fp_entry_reinstall(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_entry_t           eid;

    FP_GET_NUMB(eid, subcmd, args);
            
    FP_VERB(("_entry_reinstall eid=%d\n", eid));
    retval = bcm_field_entry_reinstall(unit, eid);
    FP_CHECK_RETURN(retval, "bcm_field_entry_reinstall");
    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_entry_remove
 * Purpose:
 * Parmameters:
 * Returns:
 *     CMD_OK 
 */
STATIC int
robo_fp_entry_remove(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_entry_t           eid;

    FP_GET_NUMB(eid, subcmd, args);
            
    FP_VERB(("_entry_remove eid=%d\n", eid));
    retval = bcm_field_entry_remove(unit, eid);
    FP_CHECK_RETURN(retval, "bcm_field_entry_remove");
    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_entry_prio
 * Purpose:
 *     CLI interface to bcm_field_entry_prio_get/set()
 * Parmameters:
 * Returns:
 *     CMD_OK 
 */
STATIC int
robo_fp_entry_prio(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    int                         prio;
    char*                       subcmd = NULL;
    bcm_field_entry_t           eid;

    FP_GET_NUMB(eid, subcmd, args);

    /* BCM.0> fp entry prio <eid> */
    if ((subcmd = ARG_GET(args)) == NULL) {
        FP_VERB(("bcm_field_entry_prio_get(eid=%d)\n", eid));
        retval = bcm_field_entry_prio_get(unit, eid, &prio);
        FP_CHECK_RETURN(retval, "bcm_field_entry_prio_get");
        printk("FP entry=%d: prio=%d\n", eid, prio);
    } else {
        /* BCM.0> fp entry prio <eid> [prio] */
        if (isint(subcmd)) {
            prio = parse_integer(subcmd);
        } else {
            if(!sal_strcasecmp(subcmd, "highest")) {
                prio = BCM_FIELD_ENTRY_PRIO_HIGHEST;
            } else if(!sal_strcasecmp(subcmd, "lowest")) {
                prio = BCM_FIELD_ENTRY_PRIO_LOWEST;
            } else if(!sal_strcasecmp(subcmd, "dontcare")) {
                prio = BCM_FIELD_ENTRY_PRIO_DONT_CARE;
            } else if(!sal_strcasecmp(subcmd, "default")) {
                prio = BCM_FIELD_ENTRY_PRIO_DEFAULT;
            } else {
                return CMD_USAGE;
            }
        }

        FP_VERB(("bcm_field_entry_prio_set(eid=%d, prio=%d)\n", eid, prio));
        retval = bcm_field_entry_prio_set(unit, eid, prio);
        FP_CHECK_RETURN(retval, "bcm_field_entry_prio_set");
    }

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_group(int unit, args_t *args, bcm_field_qset_t *qset)
{
    char*               subcmd = NULL;
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    /* BCM.0> fp group create ... */
    if(!sal_strcasecmp(subcmd, "create")) {
        return robo_fp_group_create(unit, args, qset);
    }
    /* BCM.0> fp group destroy ... */
    if(!sal_strcasecmp(subcmd, "destroy")) {
        return robo_fp_group_destroy(unit, args);
    }

    /* BCM.0> fp group get ... */
    if(!sal_strcasecmp(subcmd, "get")) {
        return robo_fp_group_get(unit, args);
    }

    /* BCM.0> fp group set ... */
    if(!sal_strcasecmp(subcmd, "set")) {
        return robo_fp_group_set(unit, args, qset);
    }

    /* BCM.0> fp group status ... */
    if(!sal_strcasecmp(subcmd, "status")) {
        return robo_fp_group_status_get(unit, args);
    }

    /* BCM.0> fp group mode ... */
    if(!sal_strcasecmp(subcmd, "mode")) {
        return robo_fp_group_mode_get(unit, args);
    }

    /* BCM.0> fp group lookup ... */
    if(!sal_strcasecmp(subcmd, "lookup")) {
        return robo_fp_group_lookup(unit, args);
    }

    return CMD_USAGE;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_group_create(int unit, args_t *args, bcm_field_qset_t *qset)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    int                         pri;
    bcm_field_group_t           gid;
    bcm_field_group_mode_t      mode;
 
    FP_GET_NUMB(pri, subcmd, args);

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fp group create 'prio'  */
        FP_VERB(("_group_create pri=%d\n", pri));
        retval = bcm_field_group_create(unit, *qset, pri, &gid);
        FP_CHECK_RETURN(retval, "bcm_field_group_create");
    } else {
        gid = parse_integer(subcmd);
        if ((subcmd = ARG_GET(args)) == NULL) {
            /* BCM.0> fp group create 'prio' 'gid' */
            FP_VERB(("_group_create_id pri=%d gid=%d\n", pri, gid));
            retval = bcm_field_group_create_id(unit, *qset, pri, gid);
            FP_CHECK_RETURN(retval, "bcm_field_group_create_id");
        } else {
            /* BCM.0> fp group create 'prio' 'gid' 'mode' */
            if (isint(subcmd)) {
                mode = parse_integer(subcmd);
            } else {
                robo_fp_lookup_mode(subcmd, &mode);
                if (mode == bcmFieldGroupModeCount) {
                    printk("Unknown mode: %s\n", subcmd);
                    return CMD_FAIL;
                }
            }
            FP_VERB(("_group_create_id pri=%d gid=%d, mode=%d\n", pri, gid,
                     mode));
            retval = bcm_field_group_create_mode_id(unit, *qset, pri, mode,
                                                    gid);
            FP_CHECK_RETURN(retval, "bcm_field_group_create_mode_id");
        }
    }
    printk("GID %d created!\n", gid);

    return CMD_OK;
}

/*
 * Function:
 *    robo_fp_lookup_mode
 * Purpose:
 *    Lookup a action from a user string.
 * Parmameters:
 * Returns:
 */
STATIC void
robo_fp_lookup_mode(const char *mode_str, bcm_field_group_mode_t *mode)
{
    char                   tbl_str[FP_STAT_STR_SZ];
    char                   lng_str[FP_STAT_STR_SZ];

    assert(mode_str != NULL);
    assert(strlen(mode_str) < FP_STAT_STR_SZ - 1);
    assert(mode != NULL);

    for (*mode = 0; *mode < bcmFieldGroupModeCount; (*mode)++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        /* Test for the suffix only */
        strncpy(tbl_str, _robo_fp_group_mode_name(*mode), FP_STAT_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, mode_str)) {
            break;
        }
        /* Test for whole name of the mode*/
        strncpy(lng_str, "bcmFieldGroupMode",
                strlen("bcmFieldGroupMode") + 1);
        strncat(lng_str, tbl_str,
                FP_STAT_STR_SZ - 1 - strlen("bcmFieldGroupMode"));
        if (!sal_strcasecmp(lng_str, mode_str)) {
            break;
        }
    }

    /* If not found, result is bcmFieldGroupModeCount */
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_group_destroy(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_group_t           gid;
 
    FP_GET_NUMB(gid, subcmd, args);

    /* BCM.0> fp group destroy 'gid' */
    FP_VERB(("_group_destroy gid=%d\n", gid));
    retval = bcm_field_group_destroy(unit, gid);
    FP_CHECK_RETURN(retval, "bcm_field_group_destroy");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_group_get(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_group_t           gid;
    bcm_field_qset_t            qset;
 
    FP_GET_NUMB(gid, subcmd, args);

    /* BCM.0> fp group create 'prio'  */
    FP_VERB(("_group_get gid=%d\n", gid));
    retval = bcm_field_group_get(unit, gid, &qset);
    FP_CHECK_RETURN(retval, "bcm_field_group_get");
    robo_fp_qset_show(&qset);
    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_range(int unit, args_t *args)
{
    char*               subcmd = NULL;
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    /* BCM.0> fp range group create ... */
    if(!sal_strcasecmp(subcmd, "group")) {
        if ((subcmd = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        if(!sal_strcasecmp(subcmd, "create")) {
            return robo_fp_range_group_create(unit, args);
        }
    }
    /* BCM.0> fp range create ... */
    if(!sal_strcasecmp(subcmd, "create")) {
        return robo_fp_range_create(unit, args);
    }
    /* BCM.0> fp range get ... */
    if(!sal_strcasecmp(subcmd, "get")) {
        return robo_fp_range_get(unit, args);
    }
    /* BCM.0> fp range destroy ... */
    if(!sal_strcasecmp(subcmd, "destroy")) {
        return robo_fp_range_destroy(unit, args);
    }
    return CMD_USAGE;
}

/*
 * Function:
 *    FP CLI function to create an FP range
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_range_create(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_range_t           rid;
    bcm_l4_port_t               min, max;
    uint32                      flags;
    uint32                      param[4];

    FP_GET_NUMB(param[0], subcmd, args);
    FP_GET_NUMB(param[1], subcmd, args);
    FP_GET_NUMB(param[2], subcmd, args);

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fp range create 'flags' 'min' 'max' */
        flags = param[0];
        min   = param[1];
        max   = param[2];
        FP_VERB(("_range_create unit=%d, flags=0x%x, min=%d, max=%d \n",
                 unit, flags, min, max));
        retval = bcm_field_range_create(unit, &rid, flags, min, max);
        FP_CHECK_RETURN(retval, "bcm_field_range_create");
    } else {
        /* BCM.0> fp range create 'rid' 'flags' 'min' 'max' */
        rid   = param[0];
        flags = param[1];
        min   = param[2];
        max   = parse_integer(subcmd);
        FP_VERB(
            ("_range_create_id unit=%d, rid=%d, flags=0x%x, min=%d, max=%d \n",
                unit, rid, flags, min, max));
        retval = bcm_field_range_create_id(unit, rid, flags, min, max);
        FP_CHECK_RETURN(retval, "bcm_field_range_create_id");
    }
    printk("RID %d created!\n", rid);

    return CMD_OK;
}

/*
 * Function:
 *    FP CLI function to create an FP range group
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_range_group_create(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_range_t           rid;
    bcm_l4_port_t               min, max;
    uint32                      flags;
    bcm_if_group_t              group;
    uint32                      param[5];

    FP_GET_NUMB(param[0], subcmd, args);
    FP_GET_NUMB(param[1], subcmd, args);
    FP_GET_NUMB(param[2], subcmd, args);
    FP_GET_NUMB(param[3], subcmd, args);

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fp range group create 'flags' 'min' 'max' */
        flags = param[0];
        min   = param[1];
        max   = param[2];
        group = param[3];
        FP_VERB(("_range_group_create unit=%d, flags=0x%x, min=%d, max=%d group=%d\n",
                 unit, flags, min, max, group));
        retval = bcm_field_range_group_create(unit, &rid, flags, min, max, group);
        FP_CHECK_RETURN(retval, "bcm_field_range_group_create");
    } else {
        /* BCM.0> fp range group create 'rid' 'flags' 'min' 'max' */
        rid   = param[0];
        flags = param[1];
        min   = param[2];
        max   = param[3];
        group = parse_integer(subcmd);
        FP_VERB(
            ("_range_group_create_id unit=%d, rid=%d, flags=0x%x, min=%d, max=%d group=%d\n",
                unit, rid, flags, min, max, group));
        retval = bcm_field_range_group_create_id(unit, rid, flags, min, max, group);
        FP_CHECK_RETURN(retval, "bcm_field_range_group_create_id");
    }
    FP_OUT(("RID %d created!\n", rid));

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_range_get(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    int                         rid;
    bcm_l4_port_t               min, max;
    uint32                      flags;
 
    FP_GET_NUMB(rid, subcmd, args);

    /* BCM.0> fp range get 'rid'  */
    FP_VERB(("robo_fp_range_get 'rid=%d'\n", rid));
    retval = bcm_field_range_get(unit, rid, &flags, &min, &max);
    FP_CHECK_RETURN(retval, "bcm_field_range_get");
    printk("FP range get: rid=%d, min=%d max=%d ", rid, min, max);
    printk("flags=0x%x%s%s\n",
           flags,
           flags & BCM_FIELD_RANGE_SRCPORT ? " SRCPORT" : "",
           flags & BCM_FIELD_RANGE_DSTPORT ? " DSTPORT" : "");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_range_destroy(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    int                         rid;
 
    FP_GET_NUMB(rid, subcmd, args);

    /* BCM.0> fp range destroy 'rid'  */
    FP_VERB(("robo_fp_range_destroy 'rid=%d'\n", rid));
    retval = bcm_field_range_destroy(unit, rid);
    FP_CHECK_RETURN(retval, "bcm_field_range_destroy");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_group_set(int unit, args_t *args, bcm_field_qset_t *qset)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_group_t           gid;
 
    FP_GET_NUMB(gid, subcmd, args);

    /* BCM.0> fp group set 'gid' */
    retval = bcm_field_group_set(unit, gid, *qset);
    FP_CHECK_RETURN(retval, "bcm_field_group_set");
 
    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_group_status_get(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_group_t           gid;
    bcm_field_group_status_t    gstat;
 
    FP_GET_NUMB(gid, subcmd, args);

    /* BCM.0> fp group status 'gid' */
    FP_VERB(("_group_status_get gid=%d\n", gid));
    retval = bcm_field_group_status_get(unit, gid, &gstat);
    if (retval >= 0) {
        printk("group_status={\t");
        printk("prio_min=%d,       \t",  gstat.prio_min);
        printk("prio_max=%d,       \t",  gstat.prio_max);
        printk("entries_total=%d,\t",    gstat.entries_total);
        printk("entries_free=%d,\n\t\t", gstat.entries_free);
        printk("counters_total=%d,\t",   gstat.counters_total);
        printk("counters_free=%d,\t",    gstat.counters_free);
        printk("meters_total=%d,\t",     gstat.meters_total);
        printk("meters_free=%d",         gstat.meters_free);
        printk("}\n");
    }
    FP_CHECK_RETURN(retval, "bcm_field_group_status_get");
 
    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_group_mode_get(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_field_group_t           gid;
    bcm_field_group_mode_t      mode;

    FP_GET_NUMB(gid, subcmd, args);

    /* BCM.0> fp group mode 'gid' */
    FP_VERB(("bcm_field_group_mode_get gid=%d\n", gid));
    retval = bcm_field_group_mode_get(unit, gid, &mode);
    printk("group mode=%s\n", _robo_fp_group_mode_name(mode));
    FP_CHECK_RETURN(retval, "bcm_field_group_mode_get");
 
    return CMD_OK;
} 

/*
 * Function:
 *     robo_fp_group_lookup
 * Purpose:
 *     Test getting/setting the FP group packet lookup enable/disable APIs.
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_group_lookup(int unit, args_t *args)
{
    char*                       subcmd = NULL;
    int                         retval = CMD_OK;
    bcm_field_group_t           gid;
    int                         enable;
    
    FP_GET_NUMB(gid, subcmd, args);

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fp group lookup 'gid' */
        FP_VERB(("bcm_field_group_enable_get(gid=%d)\n", gid));
        retval = bcm_field_group_enable_get(unit, gid, &enable);
        FP_CHECK_RETURN(retval, "bcm_field_group_enable_get");
        if (enable) {
            printk("GID %d: lookup=Enabled\n", gid);
        } else {
            printk("GID %d: lookup=Disabled\n", gid);
        }
 
        return CMD_OK;
    }

    /* BCM.0> fp group lookup 'gid' enable */
    if(!sal_strcasecmp(subcmd, "enable")) {
        return robo_fp_group_enable_set(unit, gid, 1);
    }

    /* BCM.0> fp group lookup 'gid' disable */
    if(!sal_strcasecmp(subcmd, "disable")) {
        return robo_fp_group_enable_set(unit, gid, 0);
    }
    return CMD_USAGE;
} 

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_group_enable_set(int unit, bcm_field_group_t gid, int enable)
{
    int                         retval = CMD_OK;

    /* BCM.0> fp group enable/disable 'gid' */
    FP_VERB(("bcm_field_group_enable_set(gid=%d, enable=%d)\n", gid, enable));
    retval = bcm_field_group_enable_set(unit, gid, enable);
    FP_CHECK_RETURN(retval, "bcm_field_group_enable_set");
 
    return CMD_OK;
} 

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_meter(int unit, args_t *args)
{
    char*               subcmd = NULL;
 
    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fp meter create ... */
    if(!sal_strcasecmp(subcmd, "create")) {
        return robo_fp_meter_create(unit, args);
    }

    /* BCM.0> fp meter destroy ... */
    if(!sal_strcasecmp(subcmd, "destroy")) {
        return robo_fp_meter_destroy(unit, args);
    }

    /* BCM.0> fp meter getc ... */
    if(!sal_strcasecmp(subcmd, "getc")) {
        return robo_fp_meter_getc(unit, args);
    }

    /* BCM.0> fp meter getp ... */
    if(!sal_strcasecmp(subcmd, "getp")) {
        return robo_fp_meter_getp(unit, args);
    }

    /* BCM.0> fp meter setc ... */
    if(!sal_strcasecmp(subcmd, "setc")) {
        return robo_fp_meter_setc(unit, args);
    }

    /* BCM.0> fp meter setp ... */
    if(!sal_strcasecmp(subcmd, "setp")) {
        return robo_fp_meter_setp(unit, args);
    }

    /* BCM.0> fp meter share ... */
    if(!sal_strcasecmp(subcmd, "share")) {
        return robo_fp_meter_share(unit, args);
    }

    return CMD_USAGE;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_meter_create(int unit, args_t *args)
{
    int                 retval = CMD_OK;
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
 
    FP_GET_NUMB(eid, subcmd, args);

    /* BCM.0> fp meter create 'eid' */
    FP_VERB(("_meter_create eid=%d\n", eid));
    retval = bcm_field_meter_create(unit, eid);
    FP_CHECK_RETURN(retval, "bcm_field_meter_create");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_meter_destroy(int unit, args_t *args)
{
    int                 retval = CMD_OK;
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
 
    FP_GET_NUMB(eid, subcmd, args);

    FP_VERB(("_meter_destroy eid=%d\n", eid));
    /* BCM.0> fp meter destroy 'gid' */
    retval = bcm_field_meter_destroy(unit, eid);
    FP_CHECK_RETURN(retval, "bcm_field_meter_destroy");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_meter_getc(int unit, args_t *args)
{
    int                 retval = CMD_OK;
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    uint32              rate, burst;

    FP_GET_NUMB(eid, subcmd, args);

    FP_VERB(("_meter_get committed eid=%d\n", eid));
    retval = bcm_field_meter_get(unit, eid, 2, &rate, &burst);
    printk("FP meter get committed eid=%d: rate=%d, burst=%d\n", eid,
            rate, burst);
    FP_CHECK_RETURN(retval, "bcm_field_meter_get");
    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_meter_getp(int unit, args_t *args)
{
    int                 retval = CMD_OK;
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    uint32              rate, burst;

    FP_GET_NUMB(eid, subcmd, args);

    FP_VERB(("_meter_get peak eid=%d\n", eid));
    retval = bcm_field_meter_get(unit, eid, 1, &rate, &burst);
    printk("FP meter get peak eid=%d: rate=%d, burst=%d\n", eid,
            rate, burst);
    FP_CHECK_RETURN(retval, "bcm_field_meter_get");
    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_meter_setc(int unit, args_t *args)
{
    int                 retval = CMD_OK;
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    int                 rate, burst;

    FP_GET_NUMB(eid, subcmd, args);
    FP_GET_NUMB(rate, subcmd, args);
    FP_GET_NUMB(burst, subcmd, args);

    FP_VERB(("_meter_set committed eid=%d, %d, rate=%d, burst=%d)\n", eid,
            BCM_FIELD_METER_COMMITTED, rate, burst));
    retval = bcm_field_meter_set(unit, eid, BCM_FIELD_METER_COMMITTED, rate,
                                 burst);
    FP_CHECK_RETURN(retval, "bcm_field_meter_set");
    
    return CMD_OK;
}


/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_meter_setp(int unit, args_t *args)
{
    int                 retval = CMD_OK;
    char*               subcmd = NULL;
    bcm_field_entry_t   eid;
    int                 rate, burst;

    FP_GET_NUMB(eid, subcmd, args);
    FP_GET_NUMB(rate, subcmd, args);
    FP_GET_NUMB(burst, subcmd, args);

    FP_VERB(("_meter_set peak eid=%d, %d, rate=%d, burst=%d)\n", eid,
            BCM_FIELD_METER_PEAK, rate, burst));
    retval = bcm_field_meter_set(unit, eid, BCM_FIELD_METER_PEAK, rate,
                                 burst);
    FP_CHECK_RETURN(retval, "bcm_field_meter_set");
    
    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_meter_share(int unit, args_t *args)
{
    int                 retval = CMD_OK;
    char*               subcmd = NULL;
    bcm_field_entry_t   src_eid, dst_eid;
 
    FP_GET_NUMB(src_eid, subcmd, args);
    FP_GET_NUMB(dst_eid, subcmd, args);

    FP_VERB(("_meter_share src_eid=%d, dst_eid=%d\n", src_eid, dst_eid));
    retval = bcm_field_meter_share(unit, src_eid, dst_eid);
    FP_CHECK_RETURN(retval, "bcm_field_meter_share");

    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_qset
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qset(int unit, args_t *args, bcm_field_qset_t *qset)
{
    char*                 subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fp qset clear */
    if(!sal_strcasecmp(subcmd, "clear")) {
        BCM_FIELD_QSET_INIT(*qset);
        printk("BCM_FIELD_QSET_INIT() okay\n");
        return CMD_OK;
    }
    /* BCM.0> fp qset add ...*/
    if(!sal_strcasecmp(subcmd, "add")) {
        return robo_fp_qset_add(unit, args, qset);
    }
    /* BCM.0> fp qset show */
    if(!sal_strcasecmp(subcmd, "show")) {
        return robo_fp_qset_show(qset);
    }

    return CMD_USAGE;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qset_add(int unit, args_t *args, bcm_field_qset_t *qset)
{
    char                  *qual_str = NULL;
    bcm_field_qualify_t   qual;
    bcm_field_udf_t       udf_id;
    char                  *subcmd   = NULL;
    int                   retval; 

    if ((qual_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* 
     * Argument exception: "UserDefined" does not correspond directly to 
     * any bcmFieldQualify* enum.
     */
    if(!sal_strcasecmp(qual_str, "UserDefined")) {
        FP_GET_NUMB(udf_id, subcmd, args);
        FP_VERB(("robo_fp_qset_add: udf_id=%d\n", udf_id));
        retval = bcm_field_qset_add_udf(unit, qset, udf_id);
        FP_CHECK_RETURN(retval, "bcm_field_qset_add_udf");
        return CMD_OK;
    }

    if (isint(qual_str)) {
        qual = parse_integer(qual_str);
    } else {
        robo_fp_lookup_qual(qual_str, &qual);

        if (qual == bcmFieldQualifyCount) {
            printk("Unknown qualifier: %s\n", _robo_fp_qual_name(qual));
            return CMD_FAIL;
        }
    }

    BCM_FIELD_QSET_ADD(*qset, qual);
    printk("BCM_FIELD_QSET_ADD(%s) okay\n", _robo_fp_qual_name(qual));
    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qset_show(bcm_field_qset_t *qset)
{
#ifdef BROADCOM_DEBUG
    _robo_field_qset_dump("qset=", *qset, "\n");
#endif /* BROADCOM_DEBUG */

    return CMD_OK;
}

/*
 * Function:
 *    robo_fp_lookup_qual
 * Purpose:
 *    Lookup a qualification from a user string.
 * Parmameters:
 * Returns:
 */
STATIC void
robo_fp_lookup_qual(const char *qual_str, bcm_field_qualify_t *qual)
{
    char                   tbl_str[FP_STAT_STR_SZ];
    char                   lng_str[FP_STAT_STR_SZ];

    assert(qual_str != NULL);
    assert(strlen(qual_str) < FP_STAT_STR_SZ - 1);
    assert(qual != NULL);

    for (*qual = 0; *qual < bcmFieldQualifyCount; (*qual)++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        /* Test for the suffix only */
        strncpy(tbl_str, _robo_fp_qual_name(*qual), FP_STAT_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, qual_str)) {
            break;
        }
        /* Test for whole name of the qualifier */
        sal_strcpy(lng_str, "bcmFieldQualify");
        strncat(lng_str, tbl_str,
                FP_STAT_STR_SZ - 1 - strlen("bcmFieldQualify"));
        if (!sal_strcasecmp(lng_str, qual_str)) {
            break;
        }
    }

    /* If not found, result is bcmFieldQualifyCount */
}

/*
 * Function:
 *    robo_fp_lookup_action
 * Purpose:
 *    Lookup a action from a user string.
 * Parmameters:
 * Returns:
 */
STATIC void
robo_fp_lookup_action(const char *act_str, bcm_field_action_t *act)
{
    char                   tbl_str[FP_STAT_STR_SZ];
    char                   lng_str[FP_STAT_STR_SZ];

    assert(act_str != NULL);
    assert(strlen(act_str) < FP_STAT_STR_SZ - 1);
    assert(act != NULL);

    for (*act = 0; *act < bcmFieldActionCount; (*act)++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        /* Test for the suffix only */
        strncpy(tbl_str, _robo_fp_action_name(*act), FP_STAT_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, act_str)) {
            break;
        }
        /* Test for whole name of the action */
        strncpy(lng_str, "bcmFieldAction",
                strlen("bcmFieldAction") + 1);
        strncat(lng_str, tbl_str,
                FP_STAT_STR_SZ - 1 - strlen("bcmFieldAction"));
        if (!sal_strcasecmp(lng_str, act_str)) {
            break;
        }
    }

    /* If not found, result is bcmFieldActionCount */
}
/*
 * Function:
 *    robo_fp_lookup_control
 * Purpose:
 *    Lookup a control from a user string.
 * Parmameters:
 * Returns:
 */
STATIC void
robo_fp_lookup_control(const char *control_str, bcm_field_control_t *control)
{
    char                   tbl_str[FP_STAT_STR_SZ];
    char                   lng_str[FP_STAT_STR_SZ];

    assert(control_str != NULL);
    assert(strlen(control_str) < FP_STAT_STR_SZ - 1);
    assert(control != NULL);

    for (*control = 0; *control < bcmFieldControlCount; (*control)++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        /* Test for the suffix only */
        strncpy(tbl_str, _robo_fp_control_name(*control), FP_STAT_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, control_str)) {
            break;
        }
        /* Test for whole name of the control */
        strncpy(lng_str, "bcmFieldControl",
                strlen("bcmFieldControl") + 1);
        strncat(lng_str, tbl_str,
                FP_STAT_STR_SZ - 1 - strlen("bcmFieldControl"));
        if (!sal_strcasecmp(lng_str, control_str)) {
            break;
        }
    }

    /* If not found, result is bcmFieldActionCount */
}

/*
 * Function:
 *    robo_fp_lookup_L2Format
 * Purpose:
 *    Lookup a field L2Format value from a user string.
 * Parmameters:
 *     type_str - search string
 * Returns:
 *     corresponding L2Format value
 *    
 */
STATIC bcm_field_L2Format_t
robo_fp_lookup_L2Format(const char *type_str)
{
    char                   tbl_str[FP_STAT_STR_SZ];
    char                   lng_str[FP_STAT_STR_SZ];
    const char             *prefix = "bcmFieldL2Format";
    bcm_field_L2Format_t    type;

    assert(type_str != NULL);
    assert(sal_strlen(type_str) < FP_STAT_STR_SZ - 1);

    for (type = 0; type < bcmFieldL2FormatCount; (type)++) {
        /* Test for the suffix only */
        sal_memset(tbl_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        strncpy(tbl_str, _robo_fp_qual_L2Format_name(type), FP_STAT_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, type_str)) {
            break;
        }
        /* Test for whole name of the L2Format*/
        sal_memset(lng_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        strncpy(lng_str, prefix, sal_strlen(prefix));
        strncat(lng_str, tbl_str, FP_STAT_STR_SZ - 1 - sal_strlen(prefix));
        if (!sal_strcasecmp(lng_str, type_str)) {
            break;
        }
    }

    /* If not found, result is bcmFieldL2FormatCount */
    return type;
}


/*
 * Function:
 *    robo_fp_lookup_IpType
 * Purpose:
 *    Lookup a field IpType value from a user string.
 * Parmameters:
 *     type_str - search string
 * Returns:
 *     corresponding IpType value
 *    
 */
STATIC bcm_field_IpType_t
robo_fp_lookup_IpType(const char *type_str)
{
    char                   tbl_str[FP_STAT_STR_SZ];
    char                   lng_str[FP_STAT_STR_SZ];
    const char             *prefix = "bcmFieldIpType";
    bcm_field_IpType_t     type;

    assert(type_str != NULL);
    assert(strlen(type_str) < FP_STAT_STR_SZ - 1);

    for (type = 0; type < bcmFieldIpTypeCount; (type)++) {
        /* Test for the suffix only */
        sal_memset(tbl_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        strncpy(tbl_str, _robo_fp_qual_IpType_name(type), FP_STAT_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, type_str)) {
            break;
        }
        /* Test for whole name of the IpType*/
        sal_memset(lng_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        strncpy(lng_str, prefix, strlen(prefix));
        strncat(lng_str, tbl_str, FP_STAT_STR_SZ - 1 - strlen(prefix));
        if (!sal_strcasecmp(lng_str, type_str)) {
            break;
        }
    }

    /* If not found, result is bcmFieldIpTypeCount */
    return type;
}

/*
 * Function:
 *    robo_fp_lookup_IpProtocolCommon
 * Purpose:
 *    Lookup a field IpProtocolCommon value from a user string.
 * Parmameters:
 *     type_str - search string
 * Returns:
 *     corresponding IpProtocolCommon value
 *    
 */
STATIC bcm_field_IpProtocolCommon_t
robo_fp_lookup_IpProtocolCommon(const char *type_str)
{
    char                   tbl_str[FP_STAT_STR_SZ];
    char                   lng_str[FP_STAT_STR_SZ];
    const char             *prefix = "bcmFieldIpProtocolCommon";
    bcm_field_IpProtocolCommon_t     type;

    assert(type_str != NULL);
    assert(strlen(type_str) < FP_STAT_STR_SZ - 1);

    for (type = 0; type < bcmFieldIpProtocolCommonCount; (type)++) {
        /* Test for the suffix only */
        sal_memset(tbl_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        strncpy(tbl_str, _robo_fp_qual_IpProtocolCommon_name(type), FP_STAT_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, type_str)) {
            break;
        }
        /* Test for whole name of the IpProtocolCommon*/
        sal_memset(lng_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        strncpy(lng_str, prefix, strlen(prefix));
        strncat(lng_str, tbl_str, FP_STAT_STR_SZ - 1 - strlen(prefix));
        if (!sal_strcasecmp(lng_str, type_str)) {
            break;
        }
    }

    /* If not found, result is bcmFieldIpProtocolCommonCount */
    return type;
}


/*
 * Function:
 *    robo_fp_lookup_stage
 * Purpose:
 *    Lookup a field stage value from a user string.
 * Parmameters:
 *     stage_str - search string
 *     stage     - (OUT) corresponding stage value
 * Returns:
 *    
 */
STATIC bcm_field_stage_t
robo_fp_lookup_stage(const char *stage_str) 
{
    char                   tbl_str[FP_STAT_STR_SZ];
    char                   lng_str[FP_STAT_STR_SZ];
    bcm_field_stage_t      stage;

    assert(stage_str != NULL);
    assert(strlen(stage_str) < FP_STAT_STR_SZ - 1);

    for (stage = 0; stage < bcmFieldStageCount; stage++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_STAT_STR_SZ);
        /* Test for the suffix only */
        strncpy(tbl_str, _robo_fp_qual_stage_name(stage), FP_STAT_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, stage_str)) {
            break;
        }
        /* Test for whole name of the Stage */
        sal_strcpy(lng_str, "bcmFieldStage");
        strncat(lng_str, tbl_str,
                FP_STAT_STR_SZ - 1 - strlen("bcmFieldStage"));
        if (!sal_strcasecmp(lng_str, stage_str)) {
            break;
        }
    }

    /* If not found, result is bcmFieldStageCount */
    return stage;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual(int unit, args_t *args)
{
    char                  *subcmd   = NULL;
    char                  *qual_str = NULL;
    bcm_field_qualify_t   qual;
    bcm_field_entry_t     eid;
    int                   rv;
 
    FP_GET_NUMB(eid, subcmd, args);
    /* > fp qual 'eid' ...*/

    if ((qual_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* 
     * Argument exception: "clear" does not correspond directly to 
     * any bcmFieldQualify* enum.
     */
    if(!sal_strcasecmp(qual_str, "clear")) {
        /* BCM.0> fp qual 'eid' clear  */
        FP_VERB(("robo_fp_qual_clear 'eid=%d'\n", eid));
        rv = bcm_field_qualify_clear(unit, eid);
        FP_CHECK_RETURN(rv , "bcm_field_qualify_clear");

        return rv; 
    }

    /* 
     * Argument exception: "UserDefined" does not correspond directly to 
     * any bcmFieldQualify* enum.
     */
    if(!sal_strcasecmp(qual_str, "UserDefined")) {
        return robo_fp_qual_udf(unit, eid, args, 
                           bcm_field_qualify_UserDefined, "UserDefined");
    }

    robo_fp_lookup_qual(qual_str, &qual);

    rv = CMD_OK;

    /* > fp qual 'eid' bcmFieldQualifyXXX ...*/
    switch (qual) {
    case bcmFieldQualifyInPort:
        rv = robo_fp_qual_port(unit, eid, args, bcm_field_qualify_InPort,
                     "InPort");
        break;
    case bcmFieldQualifyInPorts:
        rv = robo_fp_qual_InPorts(unit, eid, args);
        break;
    case bcmFieldQualifyDrop:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_Drop,
                  "Drop");
        break;
    case bcmFieldQualifySrcModid:
        rv = robo_fp_qual_module(unit, eid, args, bcm_field_qualify_SrcModid,
                       "SrcModid");
        break;
    case bcmFieldQualifySrcPortTgid:
        rv = robo_fp_qual_port(unit, eid, args, bcm_field_qualify_SrcPortTgid,
                     "SrcPortTgid");
        break;
    case bcmFieldQualifySrcPort:
        rv = robo_fp_qual_modport(unit, eid, args, bcm_field_qualify_SrcPort,
                     "SrcPort");
        break;
    case bcmFieldQualifySrcTrunk:
        rv = robo_fp_qual_trunk(unit, eid, args, bcm_field_qualify_SrcTrunk,
                     "SrcTrunk");
        break;
    case bcmFieldQualifyDstModid:
        rv = robo_fp_qual_module(unit, eid, args, bcm_field_qualify_DstModid,
                       "DstModid");
        break;
    case bcmFieldQualifyDstPortTgid:
        rv = robo_fp_qual_port(unit, eid, args, bcm_field_qualify_DstPortTgid,
                     "DstPortTgid");
        break;
    case bcmFieldQualifyDstPort:
        rv = robo_fp_qual_modport(unit, eid, args, bcm_field_qualify_DstPort,
                     "DstPort");
        break;
    case bcmFieldQualifyDstTrunk:
        rv = robo_fp_qual_trunk(unit, eid, args, bcm_field_qualify_DstTrunk,
                     "DstTrunk");
        break;
    case bcmFieldQualifyL4SrcPort:
        rv = robo_fp_qual_l4port(unit, eid, args, bcm_field_qualify_L4SrcPort,
                       "L4SrcPort");
        break;
    case bcmFieldQualifyL4DstPort:
        rv = robo_fp_qual_l4port(unit, eid, args, bcm_field_qualify_L4DstPort,
                       "L4DstPort");
        break;
    case bcmFieldQualifyOuterVlan:
        rv = robo_fp_qual_vlan(unit, eid, args, bcm_field_qualify_OuterVlan,
                     "OuterVlan");
        break;
    case bcmFieldQualifyOuterVlanId:
        rv = robo_fp_qual_vlan(unit, eid, args, bcm_field_qualify_OuterVlanId,
                     "OuterVlanId");
        break;
    case bcmFieldQualifyOuterVlanPri:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_OuterVlanPri,
                     "OuterVlanPri");
        break;
    case bcmFieldQualifyOuterVlanCfi:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_OuterVlanCfi,
                     "OuterVlanCfi");
        break;
    case bcmFieldQualifyInnerVlan:
        rv = robo_fp_qual_vlan(unit, eid, args, bcm_field_qualify_InnerVlan,
                     "InnerVlan");
        break;
    case bcmFieldQualifyInnerVlanId:
        rv = robo_fp_qual_vlan(unit, eid, args, bcm_field_qualify_InnerVlanId,
                     "InnerVlanId");
        break;
    case bcmFieldQualifyInnerVlanPri:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_InnerVlanPri,
                     "InnerVlanPri");
        break;
    case bcmFieldQualifyInnerVlanCfi:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_InnerVlanCfi,
                     "InnerVlanCfi");
        break;
    case bcmFieldQualifyEtherType:
        rv = robo_fp_qual_16(unit, eid, args, bcm_field_qualify_EtherType,
                   "EtherType");
        break;
    case bcmFieldQualifyIpProtocol:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_IpProtocol,
                  "IpProtocol");
        break;
    case bcmFieldQualifyLookupStatus:
        rv = robo_fp_qual_32(unit, eid, args, bcm_field_qualify_LookupStatus,
                   "LookupStatus");
        break;
    case bcmFieldQualifyIpInfo:
        rv = robo_fp_qual_32(unit, eid, args, bcm_field_qualify_IpInfo,
                   "IpInfo");
        break;
    case bcmFieldQualifyPacketRes:
        rv = robo_fp_qual_32(unit, eid, args, bcm_field_qualify_PacketRes,
                   "PacketRes");
        break;
    case bcmFieldQualifySrcIp:
        rv = robo_fp_qual_ip(unit, eid, args, bcm_field_qualify_SrcIp,
                   "SrcIp");
        break;
    case bcmFieldQualifyDstIp:
        rv = robo_fp_qual_ip(unit, eid, args, bcm_field_qualify_DstIp,
                   "DstIp");
        break;
    case bcmFieldQualifyDSCP:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_DSCP,
                  "DSCP");
        break;
    case bcmFieldQualifyIpFlags:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_IpFlags,
                  "IpFlags");
        break;
    case bcmFieldQualifyTcpControl:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_TcpControl,
                  "TcpControl");
        break;
    case bcmFieldQualifyTtl:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_Ttl,
                  "TTL");
        break;
    case bcmFieldQualifyRangeCheck:
        rv = robo_fp_qual_rangecheck(unit, eid, args, bcm_field_qualify_RangeCheck,
                       "RangeCheck");
        break;
    case bcmFieldQualifySrcIp6:
        rv = robo_fp_qual_ip6(unit, eid, args, bcm_field_qualify_SrcIp6,
                    "SrcIp6");
        break;
    case bcmFieldQualifySrcIp6High:
        rv = robo_fp_qual_ip6(unit, eid, args, bcm_field_qualify_SrcIp6High,
                    "SrcIp6High");
        break;
    case bcmFieldQualifyDstIp6:
        rv = robo_fp_qual_ip6(unit, eid, args, bcm_field_qualify_DstIp6, "DstIp6");
        break;
    case bcmFieldQualifyDstIp6High:
        rv = robo_fp_qual_ip6(unit, eid, args, bcm_field_qualify_DstIp6High,
                    "DstIp6High");
        break;
    case bcmFieldQualifyIp6FlowLabel:
        rv = robo_fp_qual_32(unit, eid, args, bcm_field_qualify_Ip6FlowLabel,
                   "Ip6FlowLabel");
        break;
    case bcmFieldQualifySrcMac:
        rv = robo_fp_qual_mac(unit, eid, args, bcm_field_qualify_SrcMac, "SrcMac");
        break;
    case bcmFieldQualifyDstMac:
        rv = robo_fp_qual_mac(unit, eid, args, bcm_field_qualify_DstMac, "DstMac");
        break;
    case bcmFieldQualifyPacketFormat:
        rv = robo_fp_qual_32(unit, eid, args, bcm_field_qualify_PacketFormat,
                   "PacketFormat");
        break;
    case bcmFieldQualifyIpType:
        rv = robo_fp_qual_IpType(unit, eid, args);
        break;
    case bcmFieldQualifyL2Format:
        rv = robo_fp_qual_L2Format(unit, eid, args);
        break;
    case bcmFieldQualifyMHOpcode:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_MHOpcode,
                  "MHOpcode");
        break;
    case bcmFieldQualifyDecap:
        rv = robo_fp_qual_Decap(unit, eid, args);
        break;
    case bcmFieldQualifyHiGig:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_HiGig, "HiGig");
        break;
    case bcmFieldQualifyStage:
        rv = robo_fp_qual_Stage(unit, eid, args);
        break;
    case bcmFieldQualifyL3IntfGroup:
        rv = robo_fp_qual_L3IntfGroup(unit, eid, args);
        break;
    case bcmFieldQualifySrcIpEqualDstIp:
        rv = robo_fp_qual_same(unit, eid, args, bcm_field_qualify_SrcIpEqualDstIp, 
                       "SrcIpEqualDstIp");
        break;
    case bcmFieldQualifyEqualL4Port:
        rv = robo_fp_qual_same(unit, eid, args, bcm_field_qualify_EqualL4Port, 
                       "EqualL4Port");
        break;
    case bcmFieldQualifyTcpSequenceZero:
        rv = robo_fp_qual_same(unit, eid, args, bcm_field_qualify_TcpSequenceZero, 
                       "TcpSequenceZero");
        break;
    case bcmFieldQualifyTcpHeaderSize:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_TcpHeaderSize, 
                       "TcpHeaderSize");
        break;
    case bcmFieldQualifyVlanFormat:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_VlanFormat,
                  "VlanFormat");
        break;
    case bcmFieldQualifyIpFrag:
        rv = robo_fp_qual_IpFrag(unit, eid, args);
        break;
    case bcmFieldQualifySnap:
        rv = robo_fp_qual_snap(unit, eid, args, bcm_field_qualify_Snap,
                   "Snap");
       break;
    case bcmFieldQualifyIpAuth:
        rv = robo_fp_qual_8(unit, eid, args, bcm_field_qualify_IpAuth,
                   "IpAuth");
       break;
    case bcmFieldQualifyIpProtocolCommon:
        rv = robo_fp_qual_IpProtocolCommon(unit, eid, args);
        break;
    case bcmFieldQualifyBigIcmpCheck:
        rv = robo_fp_qual_32(unit, eid, args, bcm_field_qualify_BigIcmpCheck,
                   "BigIcmpCheck");
        break;
    case bcmFieldQualifyIcmpTypeCode:
        rv = robo_fp_qual_16(unit, eid, args, bcm_field_qualify_IcmpTypeCode,
                   "IcmpTypeCode");
        break;
    case bcmFieldQualifyIgmpTypeMaxRespTime:
        rv = robo_fp_qual_16(unit, eid, args, bcm_field_qualify_IgmpTypeMaxRespTime,
                   "IgmpTypeMaxRespTime");
        break;
    case bcmFieldQualifyCount:
    default:
        printk("Unknown qualifier: %s\n", qual_str);
        rv = CMD_FAIL;
    }

    return rv;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_same(int unit, bcm_field_entry_t eid, args_t *args,
               int func(int, bcm_field_entry_t, uint32),
               char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    uint32                      data;
    char                        str[FP_STAT_STR_SZ];

    sal_memset(str, 0, sizeof(char) * FP_STAT_STR_SZ);
    FP_GET_NUMB(data, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'flag' */
    retval = func(unit, eid, data);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_IpFrag(int unit, bcm_field_entry_t eid, args_t *args)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_field_IpFrag_t              mode;

    FP_GET_NUMB(mode, subcmd, args);

    /* BCM.0> fp qual <eid> InterfaceClass <data> <mask> */
    FP_VERB(("robo_fp_qual_IpFrag(unit=%d, entry=%d, mode=%d)\n", unit,
             eid, mode));
    retval = bcm_field_qualify_IpFrag(unit, eid, mode);
    FP_CHECK_RETURN(retval, "bcm_field_qualify_IpFrag");

    return CMD_OK;
}


STATIC int
robo_fp_qual_InPorts(int unit, bcm_field_entry_t eid, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_pbmp_t                  data, mask;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    } else if (parse_pbmp(unit, subcmd, &data) < 0) {
        printk("%s: Error: unrecognized port bitmap: %s\n",
               ARG_CMD(args), subcmd);
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(args)) == NULL) {
        BCM_PBMP_ASSIGN(mask, PBMP_ALL(unit));
    } else if (parse_pbmp(unit, subcmd, &mask) < 0) {
        return CMD_FAIL;
    }

    /* BCM.0> fp qual 'eid' InPorts 'data' 'mask' */
    retval = bcm_field_qualify_InPorts(unit, eid, data, mask);
    FP_CHECK_RETURN(retval, "bcm_field_qualify_InPorts");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_module(int unit, bcm_field_entry_t eid, args_t *args,
               int func(int, bcm_field_entry_t, bcm_module_t, bcm_module_t),
               char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_module_t                data, mask;
    char                        str[FP_STAT_STR_SZ];

    FP_GET_NUMB(data, subcmd, args);
    FP_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_port(int unit, bcm_field_entry_t eid, args_t *args,
             int func(int, bcm_field_entry_t, bcm_port_t, bcm_port_t),
             char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_port_t                  data, mask;
    char                        str[FP_STAT_STR_SZ];
   
    sal_memset(str, 0, sizeof(char) * FP_STAT_STR_SZ);

    FP_GET_PORT(unit, data, subcmd, args);
    FP_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_modport(int unit, bcm_field_entry_t eid, args_t *args,
             int func(int, bcm_field_entry_t, bcm_module_t, bcm_module_t,
                      bcm_port_t, bcm_port_t),
             char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_module_t                data_modid, mask_modid;
    bcm_port_t                  data_port, mask_port;
    char                        str[FP_STAT_STR_SZ];
   
    sal_memset(str, 0, sizeof(char) * FP_STAT_STR_SZ);

    FP_GET_NUMB(data_modid, subcmd, args);
    FP_GET_NUMB(mask_modid, subcmd, args);
    FP_GET_PORT(unit, data_port, subcmd, args);
    FP_GET_NUMB(mask_port, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data_modid, mask_modid, data_port, mask_port);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_trunk(int unit, bcm_field_entry_t eid, args_t *args,
             int func(int, bcm_field_entry_t, bcm_trunk_t, bcm_trunk_t),
             char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_port_t                  data, mask;
    char                        str[FP_STAT_STR_SZ];
   
    sal_memset(str, 0, sizeof(char) * FP_STAT_STR_SZ);

    FP_GET_NUMB(data, subcmd, args);
    FP_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_l4port(int unit, bcm_field_entry_t eid, args_t *args,
               int func(int, bcm_field_entry_t, bcm_l4_port_t, bcm_l4_port_t),
               char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_l4_port_t               data, mask;
    char                        str[FP_STAT_STR_SZ];

    sal_memset(str, 0, sizeof(char) * FP_STAT_STR_SZ);
    FP_GET_NUMB(data, subcmd, args);
    FP_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_rangecheck(int unit, bcm_field_entry_t eid, args_t *args,
                   int func(int, bcm_field_entry_t, bcm_field_range_t, int),
                   char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    int                         range, result;
    char                        str[FP_STAT_STR_SZ];

    sal_memset(str, 0, sizeof(char) * FP_STAT_STR_SZ);
    FP_GET_NUMB(range, subcmd, args);
    FP_GET_NUMB(result, subcmd, args);

    /* BCM.0> fp qual 'eid' RangeCheck 'range' 'result' */
    retval = func(unit, eid, range, result);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_vlan(int unit, bcm_field_entry_t eid, args_t *args,
              int func(int, bcm_field_entry_t, bcm_vlan_t, bcm_vlan_t),
             char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_vlan_t                  data, mask;
    char                        str[FP_STAT_STR_SZ];

    FP_GET_NUMB(data, subcmd, args);
    FP_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_ip(int unit, bcm_field_entry_t eid, args_t *args,
           int func(int, bcm_field_entry_t, bcm_ip_t, bcm_ip_t),
           char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_ip_t                    data, mask;
    char                        stat_str[FP_STAT_STR_SZ];


    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if (parse_ipaddr(subcmd, &data) < 0) { 
       printk("ERROR: invalid ip4 addr string: \"%s\"\n", subcmd); \
       return CMD_FAIL; \
    }

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if (parse_ipaddr(subcmd, &mask) < 0) { 
       printk("ERROR: invalid ip4 addr string: \"%s\"\n", subcmd); \
       return CMD_FAIL; \
    }

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(stat_str, "bcm_field_qualify_");
    strncat(stat_str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, stat_str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_ip6(int unit, bcm_field_entry_t eid, args_t *args,
            int func(int, bcm_field_entry_t, bcm_ip6_t, bcm_ip6_t),
            char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_ip6_t                   data, mask;
    char                        str[FP_STAT_STR_SZ];

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    SOC_IF_ERROR_RETURN(parse_ip6addr(subcmd, data));

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    SOC_IF_ERROR_RETURN(parse_ip6addr(subcmd, mask));

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_mac(int unit, bcm_field_entry_t eid, args_t *args,
            int func(int, bcm_field_entry_t, bcm_mac_t, bcm_mac_t),
            char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_mac_t                   data, mask;
    char                        stat_str[FP_STAT_STR_SZ];

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if ((retval = parse_macaddr(subcmd, data)) < 0) {
       printk("ERROR: invalid mac string: \"%s\" (error=%d)\n", subcmd, retval);
       return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if ((retval = parse_macaddr(subcmd, mask)) < 0) {
       printk("ERROR: invalid mac string: \"%s\" (error=%d)\n", subcmd, retval);
       return CMD_FAIL;
    }
 
    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(stat_str, "bcm_field_qualify_");
    strncat(stat_str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, stat_str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_udf(int unit, bcm_field_entry_t eid, 
            args_t *args,
            int func(int, bcm_field_entry_t, bcm_field_udf_t, uint8*, uint8*),
            char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    int                         i;
    uint8                       data[BCM_FIELD_USER_FIELD_SIZE];
    uint8                       mask[BCM_FIELD_USER_FIELD_SIZE];
    char                        stat_str[FP_STAT_STR_SZ];
    bcm_field_udf_t             udf_id;
    uint32                      data32, mask32;

    FP_GET_NUMB(udf_id,     subcmd, args);
    FP_GET_NUMB(data32,     subcmd, args);
    FP_GET_NUMB(mask32,     subcmd, args);

    /* Convert uint32 to left-to-right byte ordering */
    for (i=0; i<4; i++) {
        data[i] = (data32 >> ((3-i)*8)) & 0xff;
        mask[i] = (mask32 >> ((3-i)*8)) & 0xff;
    }
 
    if (udf_id == 0) {
        udf_id = Last_Robo_Created_UDF_Id;
    } 

    /* BCM.0> fp qual 'eid' UserDefined udf_id 'data' 'mask' */
    FP_VERB(("robo_fp_qual_udf: udf_id=%d, data=0x%x mask=0x%x\n", udf_id, data,
             mask));
    retval = func(unit, eid, udf_id, data, mask);
    strcpy(stat_str, "bcm_field_qualify_");
    strncat(stat_str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, stat_str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_snap(int unit, bcm_field_entry_t eid, args_t *args,
           int func(int, bcm_field_entry_t, bcm_field_snap_header_t, 
           bcm_field_snap_header_t), char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_field_snap_header_t snap_data, snap_mask;
    char                        str[FP_STAT_STR_SZ];

    sal_memset(&snap_data, 0, sizeof(bcm_field_snap_header_t));
    sal_memset(&snap_mask, 0, sizeof(bcm_field_snap_header_t));
    FP_GET_NUMB(snap_data.org_code, subcmd, args);
    FP_GET_NUMB(snap_mask.org_code, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, snap_data, snap_mask);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}


/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_8(int unit, bcm_field_entry_t eid, args_t *args,
          int func(int, bcm_field_entry_t, uint8, uint8),
          char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    uint8                       data, mask;
    char                        stat_str[FP_STAT_STR_SZ];

    FP_GET_NUMB(data, subcmd, args);
    FP_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(stat_str, "bcm_field_qualify_");
    strncat(stat_str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, stat_str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_16(int unit, bcm_field_entry_t eid, args_t *args,
           int func(int, bcm_field_entry_t, uint16, uint16),
           char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    uint16                      data, mask;
    char                        str[FP_STAT_STR_SZ];

    FP_GET_NUMB(data, subcmd, args);
    FP_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}


/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_32(int unit, bcm_field_entry_t eid, args_t *args,
           int func(int, bcm_field_entry_t, uint32, uint32),
           char *qual_str)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    uint32                      data, mask;
    char                        str[FP_STAT_STR_SZ];

    FP_GET_NUMB(data, subcmd, args);
    FP_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fp qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(str, "bcm_field_qualify_");
    strncat(str, qual_str, FP_STAT_STR_SZ - 1 - strlen("bcm_field_qualify_"));
    FP_CHECK_RETURN(retval, str);

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_Decap(int unit, bcm_field_entry_t eid, args_t *args)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_field_decap_t           decap;

    if ((subcmd  = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 

    if (isint(subcmd)) {
        decap = parse_integer(subcmd);
    } else {
        decap = parse_field_decap(subcmd);
        if (decap == bcmFieldDecapCount) {
            printk("Unknown decap value: %s\n", subcmd);
            return CMD_FAIL;
        }
    }

    /* BCM.0> fp qual 'eid' Decap 'decap' */
    retval = bcm_field_qualify_Decap(unit, eid, decap);
    FP_CHECK_RETURN(retval, "bcm_field_qualify_Decap");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_Stage(int unit, bcm_field_entry_t eid, args_t *args)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_field_stage_t           stage;

    if ((subcmd  = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 

    if (isint(subcmd)) {
        stage = parse_integer(subcmd);
    } else {
        stage = robo_fp_lookup_stage(subcmd);
    }

    if (stage == bcmFieldStageCount) {
        printk("Unknown Stage value: %s\n", subcmd);
        return CMD_FAIL;
    }

    /* BCM.0> fp qual <eid> Stage <stage> */
    FP_VERB(("bcm_field_qualify_Stage(unit=%d, entry=%d, stage=%s)\n", unit,
             eid, _robo_fp_qual_stage_name(stage)));
    retval = bcm_field_qualify_Stage(unit, eid, stage);
    FP_CHECK_RETURN(retval, "bcm_field_qualify_Stage");

    return CMD_OK;
}

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_L3IntfGroup(int unit, bcm_field_entry_t eid, args_t *args)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_if_group_t              data, mask;

    FP_GET_NUMB(data, subcmd, args);
    FP_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fp qual <eid> L3IntfGroup <data> <mask> */
    FP_VERB(("bcm_field_qualify_L3IntfGroup(unit=%d, entry=%d, data=%d, mask=%d)\n", unit,
             eid, data, mask));
    retval = bcm_field_qualify_L3IntfGroup(unit, eid, data, mask);
    FP_CHECK_RETURN(retval, "bcm_field_qualify_L3IntfGroup");

    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_qual_L2Format
 * Purpose:
 *     Qualify on L2Format.
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_L2Format(int unit, bcm_field_entry_t eid, args_t *args)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_field_L2Format_t          type;

    if ((subcmd  = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 

    if (isint(subcmd)) {
        type = parse_integer(subcmd);
    } else {
        type = robo_fp_lookup_L2Format(subcmd);
        if (type == bcmFieldL2FormatCount) {
            printk("Unknown L2Format value: %s\n", subcmd);
            return CMD_FAIL;
        }
    }

    /* BCM.0> fp qual 'eid' L2Format 'type' */
    retval = bcm_field_qualify_L2Format(unit, eid, type);
    FP_CHECK_RETURN(retval, "bcm_field_qualify_L2Format");

    return CMD_OK;
}


/*
 * Function:
 *     robo_fp_qual_IpType
 * Purpose:
 *     Qualify on IpType.
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_IpType(int unit, bcm_field_entry_t eid, args_t *args)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_field_IpType_t          type;

    if ((subcmd  = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 

    if (isint(subcmd)) {
        type = parse_integer(subcmd);
    } else {
        type = robo_fp_lookup_IpType(subcmd);
        if (type == bcmFieldIpTypeCount) {
            printk("Unknown IpType value: %s\n", subcmd);
            return CMD_FAIL;
        }
    }

    /* BCM.0> fp qual 'eid' IpType 'type' */
    retval = bcm_field_qualify_IpType(unit, eid, type);
    FP_CHECK_RETURN(retval, "bcm_field_qualify_IpType");

    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_qual_IpProtocolCommon
 * Purpose:
 *     Qualify on IpProtocolCommon.
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_qual_IpProtocolCommon(int unit, bcm_field_entry_t eid, args_t *args)
{
    int                         retval = CMD_OK;
    char                        *subcmd = NULL;
    bcm_field_IpType_t          protocol;

    if ((subcmd  = ARG_GET(args)) == NULL) { 
        return CMD_USAGE; 
    } 

    if (isint(subcmd)) {
        protocol = parse_integer(subcmd);
    } else {
        protocol = robo_fp_lookup_IpProtocolCommon(subcmd);
        if (protocol == bcmFieldIpProtocolCommonCount) {
            printk("Unknown IpProtocolCommon value: %s\n", subcmd);
            return CMD_FAIL;
        }
    }

    /* BCM.0> fp qual 'eid' IpProtocolcommon 'type' */
    retval = bcm_field_qualify_IpProtocolCommon(unit, eid, protocol);
    FP_CHECK_RETURN(retval, "bcm_field_qualify_IpProtocolCommon");

    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_user
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_user(int unit, args_t *args, bcm_field_udf_spec_t *udf_spec)
{
    char*               subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fp user get ... */
    if(!sal_strcasecmp(subcmd, "get")) {
        return robo_fp_user_get(unit, args, udf_spec);
    }

    /* BCM.0> fp user init ... */
    if(!sal_strcasecmp(subcmd, "init")) {
        return robo_fp_user_init(unit, args, udf_spec);
    }

    /* BCM.0> fp user set ... */
    if(!sal_strcasecmp(subcmd, "set")) {
        return robo_fp_user_set(unit, args, udf_spec);
    }

    /* BCM.0> fp user create ... */
    if(!sal_strcasecmp(subcmd, "create")) {
        return robo_fp_user_create(unit, args, udf_spec);
    }

    /* BCM.0> fp user post_ethertype ... */
    if(!sal_strcasecmp(subcmd, "post_ethertype")) {
        return robo_fp_user_post_ethertype(unit, args);
    }

    return CMD_USAGE;
}

/*
 * Function:
 *     robo_fp_user_get
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_user_get(int unit, args_t *args, bcm_field_udf_spec_t *udf_spec)
{
    int                  retval = CMD_OK;
    char*                subcmd = NULL;
    bcm_field_udf_t      udf_id;

    FP_GET_NUMB(udf_id, subcmd, args);

    if (udf_id == 0) {
        udf_id = Last_Robo_Created_UDF_Id;
    }

    FP_VERB(("_user_get: udf_id=%d\n", udf_id));
    retval = bcm_field_udf_get(unit, udf_spec, udf_id);
    FP_CHECK_RETURN(retval, "bcm_field_udf_get");
    
    printk("FP user get: udf_id=%d!\n", 
           udf_id);
    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_user_init
 * Purpose:
 *     Clear the static UDF spec.
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_user_init(int unit, args_t *args, bcm_field_udf_spec_t *udf_spec)
{
    FP_VERB(("robo_fp_user_init: udf cleared\n"));
    sal_memset(udf_spec, 0, sizeof(*udf_spec));
    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_user_set
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_user_set(int unit, args_t *args, bcm_field_udf_spec_t *udf_spec)
{
    int                  retval = CMD_OK;
    char*                subcmd = NULL;
    uint32               flags, offset;

    FP_GET_NUMB(flags, subcmd, args);
    FP_GET_NUMB(offset, subcmd, args);

    FP_VERB(("bcm_field_udf_spec_set(flags=%d offset=%d)\n", flags, offset));
    retval = bcm_field_udf_spec_set(unit, udf_spec, flags, offset);
    FP_CHECK_RETURN(retval, "bcm_field_udf_spec_set");
 
    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_user_create
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
robo_fp_user_create(int unit, args_t *args, bcm_field_udf_spec_t *udf_spec)
{
    int                  retval = CMD_OK;
    char*                subcmd = NULL;
    bcm_field_udf_t      udf_id;

    FP_GET_NUMB(udf_id, subcmd, args);

    if (udf_id == 0) {
        FP_VERB(("bcm_field_udf_create()\n"));
        retval = bcm_field_udf_create(unit, udf_spec, &udf_id);
        FP_CHECK_RETURN(retval, "bcm_field_udf_create");
        Last_Robo_Created_UDF_Id = udf_id;
    }
    else {
        FP_VERB(("bcm_field_udf_create_id(udf_id=%d)\n", 
                 udf_id));
        retval = bcm_field_udf_create_id(unit, udf_spec, udf_id);
        FP_CHECK_RETURN(retval, "bcm_field_udf_create_id");
    }
 
    return CMD_OK;
}

/*
 * Function:
 *     robo_fp_user_post_ethertype
 * Purpose:
 *     Test the internal function that sets up the 4 bytes following the
 *     ethertype as a UDF.
 * Parmameters:
 * Returns:
 */

STATIC int
robo_fp_user_post_ethertype(int unit, args_t *args) {

    return CMD_NOTIMPL;
}

STATIC int robo_fp_thread(int unit, args_t *args) 
{
    char*   subcmd = NULL;
    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if(!sal_strcasecmp(subcmd, "off")) {
        _robo_field_thread_stop(unit);
        return CMD_OK;
    }

    return CMD_USAGE;
}

#endif  /* BCM_FIELD_SUPPORT */

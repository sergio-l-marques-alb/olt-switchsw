/*
 * $Id: mem.c,v 1.1 2011/04/18 17:11:00 mruas Exp $
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
 * socdiag memory commands
 */

#include <sal/core/libc.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <soc/error.h>

#include <appl/diag/system.h>

/*
 * Utility routine to concatenate the first argument ("first"), with
 * the remaining arguments, with commas separating them.
 */

static void
collect_comma_args(args_t *a, char *valstr, char *first)
{
    char *s;

    strcpy(valstr, first);

    while ((s = ARG_GET(a)) != 0) {
        strcat(valstr, ",");
        strcat(valstr, s);
    }
}

/*
 * modify_mem_fields
 *
 *   Verify similar to modify_reg_fields (see reg.c) but works on
 *   memory table entries instead of register values.  Handles fields
 *   of any length.
 *
 *   "mod_value" is the raw data of the entry. "mod_str" is the string 
 *   containing field name and the desired field vlaue.
 *   Either mod_value or mod_str can be assigned in one function call.
 *
 *   Values may be specified with optional increment or decrement
 *   amounts; for example, a MAC address could be 0x1234+2 or 0x1234-1
 *   to specify an increment of +2 or -1, respectively.
 *
 *   If incr is FALSE, the increment is ignored and the plain value is
 *   stored in the field (e.g. 0x1234).
 *
 *   If incr is TRUE, the value portion is ignored.  Instead, the
 *   increment value is added to the existing value of the field.  The
 *   field value wraps around on overflow.
 *
 *   Returns -1 on failure, 0 on success.
 */

static int
modify_mem_fields(int unit, soc_mem_t mem, uint32 *entry,
          uint32 *mod_value, char *mod_str, int incr)
{
    soc_field_info_t *fld;
    char *fmod, *fval, *s;
    char modstr[1024];
    uint32 fvalue[SOC_ROBO_MAX_MEM_FIELD_WORDS];
    uint32 fincr[SOC_ROBO_MAX_MEM_FIELD_WORDS];
    int i;
    int entry_dw;
    soc_mem_info_t *m = &SOC_MEM_INFO(unit, mem);

    uint32 table_name;
    uint32 field_name;
    int arl_macaddress = FALSE;
    uint32 entry_value[SOC_ROBO_MAX_MEM_WORDS]={0,0};
    
   switch(mem) {
    	case GEN_MEMORYm:
    		table_name = DRV_MEM_GEN;
    		break;
    	case L2_ARLm:
    	case L2_ARL_SWm:
    	case L2_MARL_SWm:
    		table_name = DRV_MEM_ARL_HW;
    		break;
    	case MARL_PBMPm:
    		table_name = DRV_MEM_MCAST;
    		break;
    	case MSPT_TABm:
    		table_name = DRV_MEM_MSTP;
    		break;
    	case STSEC_MAC0m:
    	case STSEC_MAC1m:
    	case STSEC_MAC2m:
    		table_name = DRV_MEM_SECMAC;
    		break;
    	case VLAN_1Qm:
    		table_name = DRV_MEM_VLAN;
    		break;
    	case VLAN2VLANm:
    		table_name = DRV_MEM_VLANVLAN;
    		break;
    	case MAC2VLANm:
    		table_name = DRV_MEM_MACVLAN;
    		break;
    	case PROTOCOL2VLANm:
    		table_name = DRV_MEM_PROTOCOLVLAN;
    		break;
    	case FLOW2VLANm:
    		table_name = DRV_MEM_FLOWVLAN;
    		break;
       case CFP_TCAM_S0m:
    	case CFP_TCAM_S1m:
    	case CFP_TCAM_S2m:
    	case CFP_TCAM_S3m:
    	case CFP_TCAM_S4m:
    	case CFP_TCAM_S5m:
       case CFP_TCAM_IPV4_SCm:
       case CFP_TCAM_IPV6_SCm:
       case CFP_TCAM_NONIP_SCm:
       case CFP_TCAM_CHAIN_SCm:
    	    table_name = DRV_MEM_TCAM_DATA;
    	    break;
    	case CFP_TCAM_MASKm:
       case CFP_TCAM_IPV4_MASKm:
       case CFP_TCAM_IPV6_MASKm:
       case CFP_TCAM_NONIP_MASKm:
       case CFP_TCAM_CHAIN_MASKm:
    	    table_name = DRV_MEM_TCAM_MASK;
    	    break;
    	case CFP_ACT_POLm:
    	    table_name = DRV_MEM_CFP_ACT;
    	    break;
    	case CFP_METERm:
    	    table_name = DRV_MEM_CFP_METER;
    	    break;
    	case CFP_STAT_IBm:
    	    table_name = DRV_MEM_CFP_STAT_IB;
    	    break;
    	case CFP_STAT_OBm:
    	    table_name = DRV_MEM_CFP_STAT_OB;
    	    break;
		default:
			printk("Unsupport memory table.\n");
			return -1;
    }
    
    if (mod_value) {
        memcpy(entry_value,mod_value,sizeof(uint32)*SOC_ROBO_MAX_MEM_FIELD_WORDS);
        (DRV_SERVICES(unit)->mem_write)
                     (unit, table_name, *entry, 1, entry_value);
        goto done;
    }
    
    entry_dw = BYTES2WORDS(m->bytes);

    strncpy(modstr, mod_str, sizeof (modstr));   /* Don't destroy input string */
    modstr[sizeof (modstr) - 1] = 0;
    mod_str = modstr;

    while ((fmod = strtok(mod_str, ",")) != 0) {
        mod_str = NULL;         /* Pass strtok NULL next time */
        fval = strchr(fmod, '=');
        if (fval != NULL) {     /* Point fval to arg, NULL if none */
            *fval++ = 0;        /* Now fmod holds only field name. */
        }
        if (fmod[0] == 0) {
            printk("Null field name\n");
            return -1;
        }
        if (!sal_strcasecmp(fmod, "clear")) {
            memset(entry, 0, entry_dw * sizeof (*entry));
            continue;
        }
        for (fld = &m->fields[0]; fld < &m->fields[m->nFields]; fld++) {
            if (!sal_strcasecmp(fmod, soc_robo_fieldnames[fld->field])) {
                break;
            }
        }
        if (table_name == DRV_MEM_ARL) {
           	if (!sal_strcasecmp(fmod, "macaddr") || \
           	    !sal_strcasecmp(fmod, "macaddress")) {
       	    	arl_macaddress = TRUE;
       	    }
        }
    
        if ((fld == &m->fields[m->nFields]) && (!arl_macaddress)) {
            printk("No such field \"%s\" in memory \"%s\".\n",
               fmod, SOC_ROBO_MEM_UFNAME(unit, mem));
            return -1;
        }
        if (!fval) {
            printk("Missing %d-bit value to assign to \"%s\" field \"%s\".\n",
               fld->len, SOC_ROBO_MEM_UFNAME(unit, mem), soc_robo_fieldnames[fld->field]);
            return -1;
        }
    
        if ((table_name == DRV_MEM_TCAM_DATA) ||
            (table_name == DRV_MEM_TCAM_MASK) ||
            (table_name == DRV_MEM_CFP_ACT) ||
            (table_name == DRV_MEM_CFP_METER) ||
            (table_name == DRV_MEM_CFP_STAT_IB) ||
            (table_name == DRV_MEM_CFP_STAT_OB)) {
            switch (fld->field) {
                case VALID_Rf:
                    field_name = DRV_CFP_FIELD_VALID;
                    break;
                case SLICEIDf:
                    field_name = DRV_CFP_FIELD_SLICE_ID;
                    break;
                case SRC_Pf:
                    field_name = DRV_CFP_FIELD_SRC_PORT;
                    break;
                case VLANTAGGEDf:
                    field_name = DRV_CFP_FIELD_1QTAGGED;
                    break;
                case SPTAGGEDf:
                    field_name = DRV_CFP_FIELD_SPTAGGED;
                    break;
                case EII_OR_8023f:
                    field_name = DRV_CFP_FIELD_EII_OR_8023;
                    break;
                case BRCM_TAGGEDf:
                    field_name = DRV_CFP_FIELD_BRCM_TAGGED;
                    break;
                case IEEE_802_2_LLCf:
                    field_name = DRV_CFP_FIELD_IEEE_LLC;
                    break;
                case IEEE_802_2_SNAPf:
                    field_name = DRV_CFP_FIELD_IEEE_SNAP;
                    break;
                case MAC_DAf:
                    field_name = DRV_CFP_FIELD_MAC_DA;
                    break;
                case MAC_SAf:
                    field_name = DRV_CFP_FIELD_MAC_SA;
                    break;
                case SP_VIDf:
                    field_name = DRV_CFP_FIELD_SP_VID;
                    break;
                case SP_CFIf:
                    field_name = DRV_CFP_FIELD_SP_CFI;
                    break;
                case SP_PRIf:
                    field_name = DRV_CFP_FIELD_SP_PRI;
                    break;
                case USR_VIDf:
                    field_name = DRV_CFP_FIELD_USR_VID;
                    break;
                case USR_CFIf:
                    field_name = DRV_CFP_FIELD_USR_CFI;
                    break;
                case USR_PRIf:
                    field_name = DRV_CFP_FIELD_USR_PRI;
                    break;
                case ETYPEf:
                    field_name = DRV_CFP_FIELD_ETYPE;
                    break;
                case UDF0_VLDf:
                    field_name = DRV_CFP_FIELD_UDF0_VALID;
                    break;
                case UDF0f:
                    field_name = DRV_CFP_FIELD_UDF0;
                    break;
                case IPV4_VALIDf:
                    field_name = DRV_CFP_FIELD_IPV4_VALID;
                    break;
                case IP_DAf:
                    field_name = DRV_CFP_FIELD_IP_DA;
                    break;
                case IP_SAf:
                    field_name = DRV_CFP_FIELD_IP_SA;
                    break;
                case SAMEIPADDRf:
                    field_name = DRV_CFP_FIELD_SAME_IP;
                    break;
                case TCP_UDP_VALIDf:
                    field_name = DRV_CFP_FIELD_TCPUDP_VALID;
                    break;
                case L4DSTf:
                    field_name = DRV_CFP_FIELD_L4DST;
                    break;
                case L4SRCf:
                    field_name = DRV_CFP_FIELD_L4SRC;
                    break;
                case SAMEL4PORTf:
                    field_name = DRV_CFP_FIELD_SAME_L4PORT;
                    break;
                case L4SRC_LESS_1024f:
                    field_name = DRV_CFP_FIELD_L4SRC_LESS1024;
                    break;
                case TCP_FRAMEf:
                    field_name = DRV_CFP_FIELD_TCP_FRAME;
                    break;
                case TCP_SEQUENCE_ZEROf:
                    field_name = DRV_CFP_FIELD_TCP_SEQ_ZERO;
                    break;
                case TCP_HEADER_LENf:
                    field_name = DRV_CFP_FIELD_TCP_HDR_LEN;
                    break;
                case TCP_FLAGf:
                    field_name = DRV_CFP_FIELD_TCP_FLAG;
                    break;
                case UDF2_VLDf:
                    field_name = DRV_CFP_FIELD_UDF2_VALID;
                    break;
                 case UDF2f:
                    field_name = DRV_CFP_FIELD_UDF2;
                    break;
                case IP_PROTOCOLf:
                    field_name = DRV_CFP_FIELD_IP_PROTO;
                    break;
                case IP_VERSIONf:
                    field_name = DRV_CFP_FIELD_IP_VER;
                    break;
                case IP_TOSf:
                    field_name = DRV_CFP_FIELD_IP_TOS;
                    break;
                case IP_TTLf:
                    field_name = DRV_CFP_FIELD_IP_TTL;
                    break;
                case UDF3A_VLDf:
                    field_name = DRV_CFP_FIELD_UDF3A_VALID;
                    break;
                case UDF3Af:
                    field_name = DRV_CFP_FIELD_UDF3A;
                    break;
                case UDF3B_VLDf:
                    field_name = DRV_CFP_FIELD_UDF3B_VALID;
                    break;
                case UDF3Bf:
                    field_name = DRV_CFP_FIELD_UDF3B;
                    break;
                case UDF3C_VLDf:
                    field_name = DRV_CFP_FIELD_UDF3C_VALID;
                    break;
                case UDF3Cf:
                    field_name = DRV_CFP_FIELD_UDF3C;
                    break;
                case UDF4A_VLDf:
                    field_name = DRV_CFP_FIELD_UDF4A_VALID;
                    break;
                case UDF4Af:
                    field_name = DRV_CFP_FIELD_UDF4A;
                    break;
                case UDF4B_VLDf:
                    field_name = DRV_CFP_FIELD_UDF4B_VALID;
                    break;
                case UDF4Bf:
                    field_name = DRV_CFP_FIELD_UDF4B;
                    break;
                case UDF4C_VLDf:
                    field_name = DRV_CFP_FIELD_UDF4C_VALID;
                    break;
                case UDF4Cf:
                    field_name = DRV_CFP_FIELD_UDF4C;
                    break;
                case UDF4D_VLDf:
                    field_name = DRV_CFP_FIELD_UDF4D_VALID;
                    break;
                case UDF4Df:
                    field_name = DRV_CFP_FIELD_UDF4D;
                    break;
                case UDF4E_VLDf:
                    field_name = DRV_CFP_FIELD_UDF4E_VALID;
                    break;
                case UDF4Ef:
                    field_name = DRV_CFP_FIELD_UDF4E;
                    break;
                case UPD_PRI_IBf:
                    field_name = DRV_CFP_FIELD_MOD_PRI_EN_IB;
                    break;
                case PRI_MAP_IBf:
                    field_name = DRV_CFP_FIELD_MOD_PRI_MAP_IB;
                    break;
                case REDIRECT_EN_IBf:
                    field_name = DRV_CFP_FIELD_REDIRECT_EN_IB;
                    break;
                case ADD_CHANGE_DEST_IBf:
                    field_name = DRV_CFP_FIELD_ADD_CHANGE_DEST_IB;
                    break;
                case NEW_DEST_IBf:
                    field_name = DRV_CFP_FIELD_NEW_DEST_IB;
                    break;
                case UPD_PRI_OBf:
                    field_name = DRV_CFP_FIELD_MOD_PRI_EN_OB;
                    break;
                case PRI_MAP_OBf:
                    field_name = DRV_CFP_FIELD_MOD_PRI_MAP_OB;
                    break;
                case REDIRECT_EN_OBf:
                    field_name = DRV_CFP_FIELD_REDIRECT_EN_OB;
                    break;
                case ADD_CHANGE_DEST_OBf:
                    field_name = DRV_CFP_FIELD_ADD_CHANGE_DEST_OB;
                    break;
                case NEW_DEST_OBf:
                    field_name = DRV_CFP_FIELD_NEW_DEST_OB;
                    break;
                case CURR_QUOTAf:
                    field_name = DRV_CFP_FIELD_CURR_QUOTA;
                    break;
                case RATE_REFRESH_ENf:
                    field_name = DRV_CFP_FIELD_RATE_REFRESH_EN;
                    break;
                case REF_CAPf:
                    field_name = DRV_CFP_FIELD_REF_CAP;
                    break;
                case TOKEN_NUMf:
                    field_name = DRV_CFP_FIELD_RATE;
                    break;
                case IN_BAND_CNTf:
                    field_name = DRV_CFP_FIELD_IB_CNT;
                    break;
                case OUT_BAND_CNTf:
                    field_name = DRV_CFP_FIELD_OB_CNT;
                    break;
                /* Add for BCM5395 memory field */
                case SRC_PMAPf:
                    field_name = DRV_CFP_FIELD_IN_PBMP;
                    break;
                case IP_FRAGf:
                    field_name = DRV_CFP_FIELD_IP_FRAG;
                    break;
                case NON_FIRST_FRAGf:
                    field_name = DRV_CFP_FIELD_IP_NON_FIRST_FRAG;
                    break;
                default :
                    printk("No such field \"%s\" in memory \"%s\".\n",
                           fmod, SOC_ROBO_MEM_UFNAME(unit, mem));
                    return -1;
            }
        } else {
        switch(fld->field) {
        	case VID_Rf:
        		field_name = DRV_MEM_FIELD_VLANID;
        		break;
        	case PORTID_Rf:
        		field_name = DRV_MEM_FIELD_SRC_PORT;
        		break;
        	case MARL_PBMP_IDXf:
        		field_name = DRV_MEM_FIELD_DEST_BITMAP;
        		break;
        	case PRIORITY_Rf:
        		field_name = DRV_MEM_FIELD_PRIORITY;
        		break;
        	case AGEf:
        		field_name = DRV_MEM_FIELD_AGE;
        		break;
        	case STATICf:
        		field_name = DRV_MEM_FIELD_STATIC;
        		break;
        	case VALID_Rf:
        		field_name = DRV_MEM_FIELD_VALID;
        		break;
        	case MSPT_IDf:
        		field_name = DRV_MEM_FIELD_SPT_GROUP_ID;
        		break;
        	case UNTAG_MAPf:
        		field_name = DRV_MEM_FIELD_OUTPUT_UNTAG;
        		break;
        	case FORWARD_MAPf:
        		field_name = DRV_MEM_FIELD_PORT_BITMAP;
        		break;
        	case EVEN_PBMPf:
        		field_name = DRV_MEM_FIELD_DEST_BITMAP;
        		break;
        	case ODD_PBMPf:
        		field_name = DRV_MEM_FIELD_DEST_BITMAP1;
        		break;
        	case EVEN_VALIDf:
        		field_name = DRV_MEM_FIELD_VALID;
        		break;
        	case ODD_VALIDf:
        		field_name = DRV_MEM_FIELD_VALID1;
        		break;
        	case MACADDRf:
        		field_name = DRV_MEM_FIELD_MAC;
        		break;
        	case VLAIDf:
        		field_name = DRV_MEM_FIELD_VALID;
        		break;
        	default:
        		if (arl_macaddress) {
            		field_name = DRV_MEM_FIELD_MAC;
        		} else if ((fld->field >= MSP_TREE_PORT0f) && \
        		           (fld->field<=MSP_TREE_PORT26f)) {
        		    /* fld->field falls in port0 to port26 */
        			field_name = DRV_MEM_FIELD_MSTP_PORTST;
        		} else {
                    printk("No such field \"%s\" in memory \"%s\".\n",
                       fmod, SOC_ROBO_MEM_UFNAME(unit, mem));
                    return -1;
        		}
            }
        }

        s = strchr(fval, '+');
        if (s == NULL) {
            s = strchr(fval, '-');
        }
        if (s == fval) {
            s = NULL;
        }
        if (incr) {
            if (s != NULL) {
            parse_long_integer(fincr, SOC_ROBO_MAX_MEM_FIELD_WORDS,
                       s[1] ? &s[1] : "1");
            if (*s == '-') {
                neg_long_integer(fincr, SOC_ROBO_MAX_MEM_FIELD_WORDS);
            }
            if (fld->len & 31) {
                /* Proper treatment of sign extension */
                fincr[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
            }
            (DRV_SERVICES(unit)->mem_field_get)(unit, table_name, field_name,
                    entry, fvalue);
            add_long_integer(fvalue, fincr, SOC_ROBO_MAX_MEM_FIELD_WORDS);
            if (fld->len & 31) {
                /* Proper treatment of sign extension */
                fvalue[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
            }
            (DRV_SERVICES(unit)->mem_read)
                         (unit, table_name, *entry, 1, entry_value);
            (DRV_SERVICES(unit)->mem_field_set)
                         (unit, table_name, field_name, entry_value, fvalue);
            (DRV_SERVICES(unit)->mem_write)
                         (unit, table_name, *entry, 1, entry_value);
            }
        } else {
            if (s != NULL) {
                *s = 0;
            }
            parse_long_integer(fvalue, SOC_ROBO_MAX_MEM_FIELD_WORDS, fval);
            for (i = fld->len; i < SOC_ROBO_MAX_MEM_FIELD_BITS; i++) {
                if (fvalue[i / 32] & 1 << (i & 31)) {
                    printk("Value \"%s\" too large for "
                       "%d-bit field \"%s\".\n",
                       fval, fld->len, soc_robo_fieldnames[fld->field]);
                    return -1;
                }
            }
            (DRV_SERVICES(unit)->mem_read)
                         (unit, table_name, *entry, 1, entry_value);
            (DRV_SERVICES(unit)->mem_field_set)
                         (unit, table_name, field_name, entry_value, fvalue);
            (DRV_SERVICES(unit)->mem_write)
                         (unit, table_name, *entry, 1, entry_value);
        }
        
    }
done:
    return 0;
}

static int
parse_dwords(int count, uint32 *dw, args_t *a)
{
    char *s;
    int i;

    for (i = 0; i < count; i++) {
        if ((s = ARG_GET(a)) == NULL) {
            printk("Not enough data values (have %d, need %d)\n",
                   i, count);
            return -1;
        }

        dw[i] = parse_integer(s);
    }

    if (ARG_CNT(a) > 0) {
        printk("Ignoring extra data on command line "
               "(only %d words needed)\n",
               count);
    }

    return 0;
}

cmd_result_t
cmd_robo_mem_write(int unit, args_t *a)
{
    int i, start, count, copyno;
    char *tab, *idx, *cnt, *s;
    soc_mem_t mem;
    uint32 entry[SOC_ROBO_MAX_MEM_WORDS];
    int entry_dw;
    char copyno_str[8];
    int update;
    int rv = CMD_FAIL;
    char valstr[1024];
    uint32 index;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        goto done;
    }

    tab = ARG_GET(a);
    idx = ARG_GET(a);
    cnt = ARG_GET(a);
    s = ARG_GET(a);

    /* you will need at least one value and all the args .. */
    if (!tab || !idx || !cnt || !s || !isint(cnt)) {
        return CMD_USAGE;
    }

    if (parse_memory_name(unit, &mem, tab, &copyno) < 0) {
        printk("ERROR: unknown table \"%s\"\n",tab);
        goto done;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        debugk(DK_ERR, "Error: Memory %s not valid for chip %s.\n",
        	SOC_ROBO_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }

    if (soc_mem_is_readonly(unit, mem)) {
        debugk(DK_ERR, "ERROR: Table %s is read-only\n",
	           SOC_ROBO_MEM_UFNAME(unit, mem));
        goto done;
    }

    start = parse_memory_index(unit, mem, idx);
    count = parse_integer(cnt);

    if (copyno == COPYNO_ALL) {
        copyno_str[0] = 0;
    } else {
        sprintf(copyno_str, ".%d", copyno);
    }

    entry_dw = soc_mem_entry_words(unit, mem);

    /*
     * If a list of fields were specified, generate the entry from them.
     * Otherwise, generate it by reading raw dwords from command line.
     */

    if (!isint(s)) {
        /* List of fields */
        collect_comma_args(a, valstr, s);

        memset(entry, 0, sizeof (entry));

        update = TRUE;
    } else {
        /* List of numeric values */

        ARG_PREV(a);

        if (parse_dwords(entry_dw, entry, a) < 0) {
            goto done;
        }

        update = FALSE;
    }

    if (debugk_check(DK_SOCMEM)) {
	    printk("WRITE[%s%s], DATA:", 
	           SOC_ROBO_MEM_UFNAME(unit, mem),
	    	copyno_str);
	    for (i = 0; i < entry_dw; i++) {
	        printk(" 0x%x", entry[i]);
	    }
	    printk("\n");
    }

    /*
     * Take lock to ensure atomic modification of memory.
     */

    soc_mem_lock(unit, mem);

    /*
     * Created entry, now write it
     */
    for (index = start; index < start + count; index++) {
        if (update) {
            modify_mem_fields(unit, mem, \
            	              (uint32 *)&index, NULL, valstr, FALSE);
        }
        else {
            modify_mem_fields(unit, mem, \
            	              (uint32 *)&index, entry, NULL, FALSE);
        }
    }
    soc_mem_unlock(unit, mem);

    rv = CMD_OK;

 done:
    return rv;
}


/*
 * Modify the fields of a table entry
 */

cmd_result_t
cmd_robo_mem_modify(int unit, args_t *a)
{
    int start, count, copyno;
    uint32 index;
    char *tab, *idx, *cnt, *s;
    soc_mem_t mem;
    char valstr[1024];
    int rv;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    tab = ARG_GET(a);
    idx = ARG_GET(a);
    cnt = ARG_GET(a);
    s = ARG_GET(a);

    /* you will need at least one dword and all the args .. */
    if (!tab || !idx || !cnt || !s || !isint(cnt)) {
        return CMD_USAGE;
    }

    collect_comma_args(a, valstr, s);

    if (parse_memory_name(unit, &mem, tab, &copyno) < 0) {
        printk("ERROR: unknown table \"%s\"\n",tab);
        return CMD_FAIL;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        debugk(DK_ERR, "Error: Memory %s not valid for chip %s.\n",
        	SOC_ROBO_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        return CMD_FAIL;
    }

    if (soc_mem_is_readonly(unit, mem)) {
    	printk("ERROR: Table %s is read-only\n", SOC_ROBO_MEM_UFNAME(unit, mem));
        return CMD_FAIL;
    }
    start = parse_memory_index(unit, mem, idx);
    count = parse_integer(cnt);

    /*
     * Take lock to ensure atomic modification of memory.
     */

    soc_mem_lock(unit, mem);

    rv = CMD_OK;
    for (index = start; index < start + count; index++) {
        modify_mem_fields(unit, mem, (uint32 *)&index, NULL, valstr, FALSE);
    }
    soc_mem_unlock(unit, mem);

    return rv;
}

/*
 * Print out help for all the memory types.
 *
 * If substr_match is non-NULL, only prints out info for memories with
 * this substring in their name or user-friendly name.
 */

static void
do_help_socmem(int unit, char *substr_match)
{
    soc_mem_t mem;
    char buf[80];
    int i, copies;
    int found = 0;

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }

        if (substr_match != NULL &&
            strcaseindex(SOC_ROBO_MEM_NAME(unit, mem), substr_match) == NULL &&
            strcaseindex(SOC_ROBO_MEM_UFNAME(unit, mem), substr_match) == NULL) {
            continue;
        }

    	strncpy(buf, SOC_ROBO_MEM_UFNAME(unit, mem), sizeof(buf)-1);

        copies = 0;
        SOC_MEM_BLOCK_ITER(unit, mem, i) {
            copies += 1;
        }
        if (copies > 1) {
            sprintf(buf + strlen(buf), "/%d", copies);
        }
    
        if (!found) {
            printk("   Flags    %-16s  MIN -  MAX   Words  %s\n",
               "Name/Copies", "Description");
            found = 1;
        }

        printk("  %c%c%c%c%c%c%c  %-16s %4d - %5d  %2d     %s\n",
               soc_mem_is_readonly(unit, mem) ? 'r' : '-',
               soc_mem_is_debug(unit, mem) ? 'd' : '-',
               soc_mem_is_sorted(unit, mem) ? 's' :
               soc_mem_is_hashed(unit, mem) ? 'h' : '-',
               soc_mem_is_cam(unit, mem) ? 'A' : '-',
               soc_mem_is_cbp(unit, mem) ? 'c' : '-',
               (soc_mem_is_bistepic(unit, mem) ||
                soc_mem_is_bistffp(unit, mem) ||
                soc_mem_is_bistcbp(unit, mem)) ? 'b' : '-',
                soc_mem_is_cachable(unit, mem) ? 'C' : '-',
               buf,
               soc_robo_mem_index_min(unit, mem),
               soc_robo_mem_index_max(unit, mem),
               soc_mem_entry_words(unit, mem),
               SOC_ROBO_MEM_DESC(unit, mem));
    }

    if (found) {
        printk("Flags: (r)eadonly, (d)ebug, (s)orted, (h)ashed\n"
               "       C(A)M, (c)bp, (b)ist-able, (C)achable\n");
    } else if (substr_match != NULL) {
        printk("No memory found with the substring '%s' in its name.\n",
               substr_match);
    }
}

/*
 * List the tables, or fields of a table entry
 */

cmd_result_t
cmd_robo_mem_list(int unit, args_t *a)
{
    soc_mem_info_t *m;
    soc_field_info_t *fld;
    char *tab, *s;
    soc_mem_t mem;
    uint32 entry[SOC_ROBO_MAX_MEM_WORDS];
    uint32 mask[SOC_ROBO_MAX_MEM_WORDS];
    int have_entry, i, dw, copyno;
    uint32 flags;
    int minidx, maxidx;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    tab = ARG_GET(a);

    if (!tab) {
        do_help_socmem(unit, NULL);
        return CMD_OK;
    }

    if (parse_memory_name(unit, &mem, tab, &copyno) < 0) {
        if ((s = strchr(tab, '.')) != NULL) {
            *s = 0;
        }
        do_help_socmem(unit, tab);
        return CMD_OK;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        printk("ERROR: Memory \"%s\" not valid for this unit\n", tab);
        return CMD_FAIL;
    }

    m = &SOC_MEM_INFO(unit, mem);
    flags = m->flags;

    dw = BYTES2WORDS(m->bytes);

    if ((s = ARG_GET(a)) == 0) {
        have_entry = 0;
    } else {
        for (i = 0; i < dw; i++) {
            if (s == 0) {
                printk("Not enough data specified (%d words needed)\n", dw);
                return CMD_FAIL;
            }
            entry[i] = parse_integer(s);
            s = ARG_GET(a);
        }
        if (s) {
            printk("Extra data specified (ignored)\n");
        }
        have_entry = 1;
    }

    printk("Memory: %s", SOC_ROBO_MEM_UFNAME(unit, mem));
    s = SOC_ROBO_MEM_UFALIAS(unit, mem);
    if (s && *s && strcmp(SOC_ROBO_MEM_UFNAME(unit, mem), s) != 0) {
        printk(" alias %s", s);
    }

    printk("Flags:");
    if (flags & SOC_MEM_FLAG_READONLY) {
        printk(" read-only");
    }
    if (flags & SOC_MEM_FLAG_VALID) {
        printk(" valid");
    }
    if (flags & SOC_MEM_FLAG_DEBUG) {
        printk(" debug");
    }
    if (flags & SOC_MEM_FLAG_SORTED) {
        printk(" sorted");
    }
    if (flags & SOC_MEM_FLAG_CBP) {
        printk(" cbp");
    }
    if (flags & SOC_MEM_FLAG_CACHABLE) {
        printk(" cachable");
    }
    if (flags & SOC_MEM_FLAG_BISTCBP) {
        printk(" bist-cbp");
    }
    if (flags & SOC_MEM_FLAG_BISTEPIC) {
        printk(" bist-epic");
    }
    if (flags & SOC_MEM_FLAG_BISTFFP) {
        printk(" bist-ffp");
    }
    if (flags & SOC_MEM_FLAG_UNIFIED) {
        printk(" unified");
    }
    if (flags & SOC_MEM_FLAG_HASHED) {
        printk(" hashed");
    }
    if (flags & SOC_MEM_FLAG_WORDADR) {
        printk(" word-addressed");
    }
    if (flags & SOC_MEM_FLAG_MONOLITH) {
        printk(" monolithic");
    }
    if (flags & SOC_MEM_FLAG_BE) {
        printk(" big-endian");
    }
    printk("\n");

    minidx = soc_robo_mem_index_min(unit, mem);
    maxidx = soc_robo_mem_index_max(unit, mem);
    printk("Entries: %d with indices %d-%d (0x%x-0x%x)",
           maxidx - minidx + 1,
           minidx,
           maxidx,
           minidx,
           maxidx);
    printk(", each %d bytes %d words\n", m->bytes, dw);

    printk("Entry mask:");
    soc_mem_datamask_get(unit, mem, mask);
    for (i = 0; i < dw; i++) {
        if (mask[i] == 0xffffffff) {
            printk(" -1");
        } else if (mask[i] == 0) {
            printk(" 0");
        } else {
            printk(" 0x%08x", mask[i]);
        }
    }
    printk("\n");

    s = SOC_ROBO_MEM_DESC(unit, mem);
    
    if (s && *s) {
        printk("Description: %s\n", s);
    }

    for (fld = &m->fields[m->nFields - 1]; fld >= &m->fields[0]; fld--) {
        printk("  %s<%d", soc_robo_fieldnames[fld->field], fld->bp + fld->len - 1);
        if (fld->len > 1) {
            printk(":%d", fld->bp);
        }
        if (have_entry) {
            uint32 fval[SOC_ROBO_MAX_MEM_FIELD_WORDS];
            char tmp[132];
        
            memset(fval, 0, sizeof (fval));
            format_long_integer(tmp, fval, SOC_ROBO_MAX_MEM_FIELD_WORDS);
            printk("> = %s\n", tmp);
        } else {
            printk(">\n");
        }
    }

    return CMD_OK;
}

/*
 * Turn on/off software caching of hardware tables
 */


cmd_result_t
mem_robo_cache(int unit, args_t *a)
{
    soc_mem_t		mem;
    int			copyno;
    char		*c;
    uint32	enable;
    uint32      table_name;
    int         rv = CMD_OK;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
    	return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {
    	printk("Memory table cache status:\n");
        (DRV_SERVICES(unit)->mem_cache_get)(unit, DRV_MEM_VLAN, &enable);
    	printk("VLAN    : cache %s.\n", enable ? "Enabled" : "Disabled");
        (DRV_SERVICES(unit)->mem_cache_get)(unit, DRV_MEM_SECMAC, &enable);
    	printk("SEC_MAC : cache %s.\n", enable ? "Enabled" : "Disabled");
    
    	return CMD_OK;
    }

    while ((c = ARG_GET(a)) != 0) {
    	switch (*c) {
        	case '+':
        	    enable = 1;
        	    c++;
        	    break;
        	case '-':
        	    enable = 0;
        	    c++;
        	    break;
        	default:
        	    enable = 1;
        	    break;
	    }

    	if (parse_memory_name(unit, &mem, c, &copyno) < 0) {
    	    printk("%s: Unknown table \"%s\"\n", ARG_CMD(a), c);
    	    return CMD_FAIL;
    	}
    
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            debugk(DK_ERR, "Error: Memory %s not valid for chip %s.\n",
                   SOC_ROBO_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            continue;
        }

        switch(mem) {
         	case STSEC_MAC0m:
         	case STSEC_MAC1m:
         	case STSEC_MAC2m:
         		table_name = DRV_MEM_SECMAC;
         		break;
         	case VLAN_1Qm:
         		table_name = DRV_MEM_VLAN;
         		break;
     		default:
     			printk("%s: Memory %s is not cachable\n",
     				    ARG_CMD(a), SOC_ROBO_MEM_UFNAME(unit, mem));
     			return CMD_FAIL;
        }
        rv = (DRV_SERVICES(unit)->mem_cache_set)(unit, table_name, enable);
        if (rv < 0) {
            return rv;
    	}
    }

    return CMD_OK;
}


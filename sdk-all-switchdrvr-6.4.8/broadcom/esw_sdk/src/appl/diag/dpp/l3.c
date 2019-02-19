/* $Id: l3.c,v 1.2 Broadcom SDK $
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



#include <shared/bsl.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/debug.h>


STATIC cmd_result_t _cmd_dpp_l3_intf(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l3_egr(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l3_host(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l3_route(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l3_mpath(int unit, args_t *args);

#define CMD_L3_INTF_USAGE                                  \
    "    intf [add | update | delete | find | get ]\n"     \
    "         [SMAC=<mac>] [vid=<id>]\n"                   \
    "         [vrf=<vrf>] [Id=<ifid>]\n"                   \
    "         [ttl=<ttl>] [mtu=<mtu>]\n"                   

#define CMD_L3_EGR_USAGE                                    \
    "    egr  [add | update | delete | find | get ]\n"      \
    "         [Id=<ifid>] [DMAC=<mac>]\n"                   \
    "         [module=<modid>] [port=<port>]\n"             \
    "         [trunk=<tid>] [label=<label>]\n"              \
    "         [encap=<encap>] [fec=<fec>]\n"                    

#define CMD_L3_HOST_USAGE                                  \
    "    host  [add | delete | update | find ]\n"            \
    "          [vrf=<vrf>] ( ip=<ipaddr> ) | \n" \
    "          (v6 ip6=<ipaddr> )\n"          \
    "          [fec=<fec>]\n"

#define CMD_L3_ROUTE_USAGE                                  \
    "    route [add | delete | update | get ]\n"            \
    "          [vrf=<vrf>] ( ip=<ipaddr> mask=<mask>) | \n" \
    "          (v6 ip6=<ipaddr> ip6mask=<mask> )\n"          \
    "          [fec=<fec>]\n"

#define CMD_L3_MPATH_USAGE                                      \
    "    mpath [create | add | delete | destroy | get ]\n"      \
    "          [vrf=<vrf>] [fec1=<fec1>] [fec2=<fec2>]\n"       \
    "          [fec3=<fec3>] [fec4=<fec4>] [mpbase=<mpbase>]\n"

static cmd_t _cmd_dpp_l3_list[] = {
    {"intf",        _cmd_dpp_l3_intf,         "\n" CMD_L3_INTF_USAGE, NULL},
    {"egr",         _cmd_dpp_l3_egr,          "\n" CMD_L3_EGR_USAGE, NULL},
    {"host",        _cmd_dpp_l3_host,         "\n" CMD_L3_HOST_USAGE, NULL},
    {"route",       _cmd_dpp_l3_route,        "\n" CMD_L3_ROUTE_USAGE, NULL},
    {"mpath",       _cmd_dpp_l3_mpath,        "\n" CMD_L3_MPATH_USAGE, NULL}
};

char cmd_dpp_l3_usage[] =
    "\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "    l3 <option> [args...]\n"
#else
    CMD_L3_INTF_USAGE         "\n"
    CMD_L3_EGR_USAGE          "\n"
    CMD_L3_HOST_USAGE         "\n"
    CMD_L3_ROUTE_USAGE        "\n"
    CMD_L3_MPATH_USAGE        "\n"
#endif
    ;
/*
 * Local global variables to remember last values in arguments.
 */
static sal_mac_addr_t   _l3_macaddr = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};
static int _l3_vid = VLAN_ID_DEFAULT;
static int _l3_modid = 0, _l3_label, _l3_encap, _l3_port, _l3_tgid = 0;
static int _l3_fec;
static int _l3_vrf = 0, _l3_ttl = 0,  _l3_ifid = 0, _l3_mtu = 0;

 
#define _DPP_L3_ECMP_MAX 8

STATIC cmd_result_t
_cmd_dpp_l3_intf(int unit, args_t *args)
{
    cmd_result_t       retCode;
    int                rv;
    parse_table_t      pt;
    bcm_l3_intf_t      bcm_intf;
    char	      *subcmd;

    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    _l3_vid   = 0;
    _l3_ifid  = 0;
    _l3_vrf   = 0;
    _l3_ttl   = 0;
    _l3_mtu   = 0;

    rv        = BCM_E_NONE;
    sal_memset(_l3_macaddr, 0, 6);
    
    parse_table_init(unit, &pt);
    bcm_l3_intf_t_init(&bcm_intf);
    if ((sal_strcasecmp(subcmd, "add") == 0) ||
        (sal_strcasecmp(subcmd, "update") == 0)) {

        /* Parse command option arguments */
        parse_table_add(&pt, "SMAC", PQ_DFL|PQ_MAC, 0, &_l3_macaddr, NULL);
        parse_table_add(&pt, "vid", PQ_DFL|PQ_HEX, 0, &_l3_vid, NULL);
        parse_table_add(&pt, "Id",  PQ_DFL|PQ_HEX, 0, &_l3_ifid, NULL);
        parse_table_add(&pt, "vrf", PQ_DFL|PQ_INT, 0, &_l3_vrf, NULL);
        parse_table_add(&pt, "ttl", PQ_DFL|PQ_INT, 0, &_l3_ttl, NULL);
        parse_table_add(&pt, "mtu", PQ_DFL|PQ_INT, 0, &_l3_mtu, NULL);

        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }

        if (sal_strcasecmp(subcmd, "update") == 0) {
            bcm_intf.l3a_flags   |= BCM_L3_REPLACE;
        }
        
        if (_l3_ifid) {
            bcm_intf.l3a_flags   |= BCM_L3_WITH_ID;
            bcm_intf.l3a_intf_id  = _l3_ifid;
        }

        if (_l3_vid) {
            bcm_intf.l3a_vid      = _l3_vid;
        }

        if (_l3_vrf) {
            bcm_intf.l3a_vrf      = _l3_vrf;
        }

        bcm_intf.l3a_ttl          = 2;
        if (_l3_ttl) {
            bcm_intf.l3a_ttl      = _l3_ttl;
        }

        bcm_intf.l3a_mtu         = 9216;
        if (_l3_mtu) {
            bcm_intf.l3a_mtu     = _l3_mtu;
        }
        sal_memcpy(bcm_intf.l3a_mac_addr,_l3_macaddr, 6);
    
        rv = bcm_l3_intf_create(unit, &bcm_intf);
        if (rv == BCM_E_NONE) {
            cli_out("Created if_id 0x%x\n", bcm_intf.l3a_intf_id);
        }
    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        parse_table_add(&pt, "SMAC", PQ_DFL|PQ_MAC, 0, &_l3_macaddr, NULL);
        parse_table_add(&pt, "vid", PQ_DFL|PQ_HEX, 0, &_l3_vid, NULL);
        parse_table_add(&pt, "Id",  PQ_DFL|PQ_HEX, 0, &_l3_ifid, NULL);
        parse_table_add(&pt, "vrf", PQ_DFL|PQ_INT, 0, &_l3_vrf, NULL);

        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }
        
        if (_l3_ifid) {
            bcm_intf.l3a_flags   |= BCM_L3_WITH_ID;
            bcm_intf.l3a_intf_id  = _l3_ifid;
        } else {
            bcm_intf.l3a_vid      = _l3_vid;
            sal_memcpy(bcm_intf.l3a_mac_addr,
                       _l3_macaddr, 6);            
        }
        bcm_intf.l3a_vrf      = _l3_vrf;

        rv = bcm_l3_intf_delete(unit, &bcm_intf);
        if (rv == BCM_E_NONE) {
            if (_l3_ifid)
                cli_out("Deleted if_id 0x%x, %s\n",
                        bcm_intf.l3a_intf_id, bcm_errmsg(rv));
            else
                cli_out("Deleted vid=%d mac=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x %s\n",
                        bcm_intf.l3a_vid,
                        bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                        bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                        bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                        bcm_errmsg(rv));
        }
        
    } else if (sal_strcasecmp(subcmd, "find") == 0) {

        /* Parse command option arguments */
        parse_table_add(&pt, "SMAC", PQ_DFL|PQ_MAC, 0, &_l3_macaddr, NULL);
        parse_table_add(&pt, "vid", PQ_DFL|PQ_HEX, 0, &_l3_vid, NULL);

        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }
        
        sal_memcpy(bcm_intf.l3a_mac_addr,_l3_macaddr, 6);
        bcm_intf.l3a_vid      = _l3_vid;
        rv = bcm_l3_intf_find(unit, &bcm_intf);
        if (rv == BCM_E_NONE) {
            cli_out("MacAddr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "Id=0x%x, Vid=%d, vrf=%d, ttl=%d, mtu=%d\n",
                    bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                    bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                    bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                    bcm_intf.l3a_intf_id, bcm_intf.l3a_vid,
                    bcm_intf.l3a_vrf, bcm_intf.l3a_ttl, bcm_intf.l3a_mtu);
        } else {
            cli_out("MacAddr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "Vid=%d bcm_l3_intf_find() failure: %s\n",
                    bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                    bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                    bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                    bcm_intf.l3a_vid, bcm_errmsg(rv));
        }
    } else if (sal_strcasecmp(subcmd, "get") == 0) {  
        parse_table_add(&pt, "Id",  PQ_DFL|PQ_HEX, 0, &_l3_ifid, NULL);

        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }
        
        bcm_intf.l3a_flags   |= BCM_L3_WITH_ID;
        bcm_intf.l3a_intf_id  = _l3_ifid;
        rv = bcm_l3_intf_get(unit, &bcm_intf);
        if (rv == BCM_E_NONE) {
            cli_out("MacAddr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "Id=0x%x, Vid=%d, vrf=%d, ttl=%d, mtu=%d\n",
                    bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                    bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                    bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                    bcm_intf.l3a_intf_id, bcm_intf.l3a_vid,
                    bcm_intf.l3a_vrf, bcm_intf.l3a_ttl, bcm_intf.l3a_mtu);
        } else {
            cli_out("MacAddr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "Vid=%d bcm_l3_intf_get() failure: %s\n",
                    bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                    bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                    bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                    bcm_intf.l3a_vid, bcm_errmsg(rv));
        }
    } else {
        cli_out("%s ERROR: Invalid option %s\n", FUNCTION_NAME(), subcmd);
	parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}


STATIC cmd_result_t
_cmd_dpp_l3_egr(int unit, args_t *args)
{
    cmd_result_t       retCode;
    int                rv;
    parse_table_t      pt;
    bcm_l3_egress_t    bcm_egr;
    char	      *subcmd;
    bcm_if_t           fec_idx;
    uint32             flags;
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    flags          = 0;
    _l3_ifid       = 0;
    _l3_tgid       = 0;
    _l3_modid      = 0;
    _l3_port       = 0;
    _l3_encap        = 0;
    _l3_label      = 0;
    _l3_fec        = 0;
    rv             = BCM_E_NONE;
    sal_memset(_l3_macaddr, 0, 6);
    
    parse_table_init(unit, &pt);
    bcm_l3_egress_t_init(&bcm_egr);
    
    if ((sal_strcasecmp(subcmd, "add") == 0) ||
        (sal_strcasecmp(subcmd, "update") == 0)) {

        /* Parse command option arguments */
        parse_table_add(&pt, "Id",     PQ_DFL|PQ_HEX, 0, &_l3_ifid, NULL);
        parse_table_add(&pt, "DMAC",   PQ_DFL|PQ_MAC, 0, &_l3_macaddr, NULL);
        parse_table_add(&pt, "module", PQ_DFL|PQ_INT, 0, &_l3_modid, NULL);
        parse_table_add(&pt, "port",   PQ_DFL|PQ_INT, 0, &_l3_port, NULL);
        parse_table_add(&pt, "trunk",  PQ_DFL|PQ_INT, 0, &_l3_tgid, NULL);
        parse_table_add(&pt, "label",  PQ_DFL|PQ_HEX, 0, &_l3_label, NULL);
        parse_table_add(&pt, "encap",  PQ_DFL|PQ_HEX, 0, &_l3_encap, NULL);
        parse_table_add(&pt, "fec",    PQ_DFL|PQ_HEX, 0, &_l3_fec, NULL);
        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }

        if (sal_strcasecmp(subcmd, "update") == 0) {
            flags = bcm_egr.flags = BCM_L3_REPLACE;
        }
        
        bcm_egr.intf              = _l3_ifid;
        sal_memcpy(bcm_egr.mac_addr,_l3_macaddr, 6);
        bcm_egr.module            = _l3_modid;
        bcm_egr.port              = _l3_port;
        bcm_egr.trunk             = _l3_tgid;
        bcm_egr.mpls_label        = _l3_label;
        bcm_egr.encap_id          = _l3_encap;
        
        rv = bcm_l3_egress_create(unit, flags, &bcm_egr, &fec_idx);
        if (rv == BCM_E_NONE) {
            cli_out("Created fec 0x%x encap=0x%x\n", fec_idx, bcm_egr.encap_id);
        }
    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        parse_table_add(&pt, "fec",    PQ_DFL|PQ_HEX, 0, &_l3_fec, NULL);
        
        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }
        fec_idx = (uint32)_l3_fec;
        rv = bcm_l3_egress_get (unit,
                                fec_idx,
                                &bcm_egr) ;
        if (rv == BCM_E_NONE) {
            cli_out("\n\tDMAC=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "\n\tInterfaceId=0x%x, module=%d, port=%d, trunk=%d "
                    "\n\tlabel=0x%x encap=0x%x\n",
                    bcm_egr.mac_addr[0],bcm_egr.mac_addr[1],
                    bcm_egr.mac_addr[2],bcm_egr.mac_addr[3],
                    bcm_egr.mac_addr[4],bcm_egr.mac_addr[5],
                    bcm_egr.intf, bcm_egr.module,
                    bcm_egr.port, bcm_egr.trunk, bcm_egr.mpls_label,
                    bcm_egr.encap_id);
        }
    } else {
        cli_out("%s ERROR: Invalid option %s\n", FUNCTION_NAME(), subcmd);
	parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

STATIC cmd_result_t
_cmd_dpp_l3_host(int unit, args_t *args)
{
    cmd_result_t       retCode;
    int                rv;
    parse_table_t      pt;
    bcm_l3_host_t      bcm_host;
    char              *subcmd;
    ip_addr_t          ipaddr;
    ip6_addr_t         ip6addr;
    int                v6 = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    _l3_fec        = 0;
    _l3_vrf        = 0;
    rv             = BCM_E_NONE;
    sal_memset(&ipaddr, 0, sizeof(ipaddr));
    sal_memset(&ip6addr, 0, sizeof(ip6_addr_t));

    parse_table_init(unit, &pt);
    bcm_l3_host_t_init(&bcm_host);

    parse_table_add(&pt, "ip",      PQ_DFL | PQ_IP,  0, &ipaddr, NULL);
    parse_table_add(&pt, "ip6",     PQ_DFL | PQ_IP6, 0, &ip6addr, NULL);
    parse_table_add(&pt, "vrf",    PQ_DFL | PQ_INT, 0, &_l3_vrf, NULL);
    parse_table_add(&pt, "fec",    PQ_DFL | PQ_HEX, 0, &_l3_fec, NULL);
    parse_table_add(&pt, "v6",  PQ_DFL|PQ_BOOL|PQ_NO_EQ_OPT, 0, &v6, NULL);

    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    if (v6) {
        bcm_host.l3a_flags |= BCM_L3_IP6;
    }

    bcm_host.l3a_vrf      = _l3_vrf;
    bcm_host.l3a_ip_addr  = ipaddr;
    sal_memcpy(&bcm_host.l3a_ip6_addr, ip6addr, sizeof(ip6_addr_t));

    if (sal_strcasecmp(subcmd, "add") == 0) {
        bcm_host.l3a_intf    = _l3_fec;
        rv = bcm_l3_host_add(unit, &bcm_host);

        cli_out("Host add returned: %s\n", bcm_errmsg(rv));

    } else if (sal_strcasecmp(subcmd, "update") == 0) {
        bcm_host.l3a_intf    = _l3_fec;
        bcm_host.l3a_flags  |= BCM_L3_REPLACE;
        rv = bcm_l3_host_add(unit, &bcm_host);

        cli_out("Host update returned: %s\n", bcm_errmsg(rv));

    } else if (sal_strcasecmp(subcmd, "find") == 0) {
        rv = bcm_l3_host_find(unit, &bcm_host);
        cli_out("Host find returned: %s\n", bcm_errmsg(rv));

    } else {
        cli_out("%s ERROR: Invalid option %s", FUNCTION_NAME(), subcmd);
        return CMD_FAIL;
    }

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

STATIC cmd_result_t
_cmd_dpp_l3_route(int unit, args_t *args)
{
    cmd_result_t       retCode;
    int                rv;
    parse_table_t      pt;
    bcm_l3_route_t     bcm_route;
    char	      *subcmd;
    ip_addr_t          ipaddr, mask;
    ip6_addr_t         ip6addr, ip6mask;
    int                v6 = 0;
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    _l3_fec        = 0;
    _l3_vrf        = 0;
    rv             = BCM_E_NONE;
    sal_memset(&ipaddr, 0, sizeof(ipaddr));
    sal_memset(&mask, 0, sizeof(mask));
    sal_memset(&ip6addr, 0, sizeof(ip6_addr_t));
    sal_memset(&ip6mask, 0, sizeof(ip6_addr_t));
    
    parse_table_init(unit, &pt);
    bcm_l3_route_t_init(&bcm_route);

    parse_table_add(&pt, "ip",      PQ_DFL | PQ_IP,  0, &ipaddr, NULL);
    parse_table_add(&pt, "ip6",     PQ_DFL | PQ_IP6, 0, &ip6addr, NULL);
    parse_table_add(&pt, "mask",    PQ_DFL | PQ_IP,  0, &mask,   NULL);
    parse_table_add(&pt, "ip6mask", PQ_DFL | PQ_IP6, 0, &ip6mask,   NULL);
    parse_table_add(&pt, "vrf",    PQ_DFL | PQ_INT, 0, &_l3_vrf, NULL);
    parse_table_add(&pt, "fec",    PQ_DFL | PQ_HEX, 0, &_l3_fec, NULL);
    parse_table_add(&pt, "v6",  PQ_DFL|PQ_BOOL|PQ_NO_EQ_OPT, 0, &v6, NULL);
        
    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    if (v6) {
        bcm_route.l3a_flags |= BCM_L3_IP6;
    }
    
    bcm_route.l3a_vrf     = _l3_vrf;
    bcm_route.l3a_subnet  = ipaddr;
    bcm_route.l3a_ip_mask = mask;
    sal_memcpy(&bcm_route.l3a_ip6_net, ip6addr, sizeof(ip6_addr_t));
    sal_memcpy(&bcm_route.l3a_ip6_mask, ip6mask, sizeof(ip6_addr_t));
    
    if (sal_strcasecmp(subcmd, "add") == 0) {
        bcm_route.l3a_intf    = _l3_fec;
        rv = bcm_l3_route_add(unit, &bcm_route);
        
        cli_out("Route add returned: %s\n", bcm_errmsg(rv));

    } else if (sal_strcasecmp(subcmd, "update") == 0) {
        bcm_route.l3a_intf    = _l3_fec;
        bcm_route.l3a_flags  |= BCM_L3_REPLACE;
        rv = bcm_l3_route_add(unit, &bcm_route);

        cli_out("Route update returned: %s\n", bcm_errmsg(rv));

    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        rv = bcm_l3_route_get(unit, &bcm_route);
        cli_out("Route get returned: %s\n", bcm_errmsg(rv));

    } else {
        cli_out("%s ERROR: Invalid option %s", FUNCTION_NAME(), subcmd);
        return CMD_FAIL;
    }
    
    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

STATIC cmd_result_t
_cmd_dpp_l3_mpath(int unit, args_t *args)
{
    cmd_result_t       retCode;
    int                rv, count;
    parse_table_t      pt;
    uint32             i, flags;
    char	      *subcmd;
    bcm_if_t           mpbase, fecs[_DPP_L3_ECMP_MAX];
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }
    
    _l3_vrf        = 0;
    mpbase         = 0;
    count          = 0;
    flags          = 0;
    rv             = BCM_E_NONE;
    for (i = 0; i < _DPP_L3_ECMP_MAX; i++) {
        fecs[i] = -1;
    }
        
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "vrf",    PQ_DFL | PQ_INT, 0, &_l3_vrf, NULL);
    parse_table_add(&pt, "fec1",   PQ_DFL | PQ_HEX, 0, &fecs[0], NULL);
    parse_table_add(&pt, "fec2",   PQ_DFL | PQ_HEX, 0, &fecs[1], NULL);
    parse_table_add(&pt, "fec3",   PQ_DFL | PQ_HEX, 0, &fecs[2], NULL);
    parse_table_add(&pt, "fec4",   PQ_DFL | PQ_HEX, 0, &fecs[3], NULL);
    parse_table_add(&pt, "mpbase", PQ_DFL | PQ_HEX, 0, &mpbase,  NULL);
    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    for (i = 0; i < _DPP_L3_ECMP_MAX; i++) {
        if (fecs[i] != -1) {
            count++;
        }
    }
    
    if (sal_strcasecmp(subcmd, "create") == 0) {
        /*
         * Initial creation
         */
        rv = bcm_l3_egress_multipath_create(unit,
                                            flags,
                                            count,
                                            fecs,
                                            &mpbase);
        if (rv == BCM_E_NONE) {
            cli_out("Multipath add success: 0x%x\n", mpbase);
        }
    } else if (sal_strcasecmp(subcmd, "add") == 0) {
        /*
         * add a member to the set
         */
        for (i = 0; i < (_DPP_L3_ECMP_MAX-1); i++) {
            if (fecs[i] != -1) {
                break;
            }
        }
        rv = bcm_l3_egress_multipath_add(unit,
                                         mpbase,
                                         fecs[i]);
        if (rv == BCM_E_NONE) {
            cli_out("multipath add successfull\n");
        }
    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        /*
         * remove a member from the set
         */
        for (i = 0; i < (_DPP_L3_ECMP_MAX-1); i++) {
            if (fecs[i] != -1) {
                break;
            }
        }
        rv = bcm_l3_egress_multipath_delete(unit,
                                            mpbase,
                                            fecs[i]);
        if (rv == BCM_E_NONE) {
            cli_out("multipath delete successfull\n");
        }
    } else if (sal_strcasecmp(subcmd, "destroy") == 0) {
        /*
         * destroy the whole group
         */
        rv = bcm_l3_egress_multipath_destroy(unit,
                                             mpbase) ;
        if (rv == BCM_E_NONE) {
            cli_out("multipath destroy success\n");
        }
    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        rv = bcm_l3_egress_multipath_get(unit,
                                         mpbase,
                                         _DPP_L3_ECMP_MAX,
                                         fecs,
                                         &count);
        if (rv == BCM_E_NONE) {
            cli_out("multipath get successfull\n");
            for (i = 0; i < count; i++) {
                cli_out("fec[%d] = 0x%x\n", i, fecs[i]);
            }
        }
    } else {
        cli_out("%s ERROR: Invalid option %s", FUNCTION_NAME(), subcmd);
        return CMD_FAIL;
    }
    
    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

cmd_result_t
cmd_dpp_l3(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
                              _cmd_dpp_l3_list, COUNTOF(_cmd_dpp_l3_list));
}


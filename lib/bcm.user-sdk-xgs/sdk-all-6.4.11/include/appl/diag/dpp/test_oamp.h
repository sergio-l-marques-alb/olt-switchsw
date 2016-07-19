/*
 * $Id: $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

/* Defines */
#define _ERR_MSG_MODULE_NAME BSL_BCM_OAM
#define TEST_BFD_VLAN                       10          /* VID = 10 */
#define TEST_BFD_DETEC_MULT_3               3           /* Detect Multiplier = 3 */
#define TEST_BFD_DETECTION_TIME_10MS        100000      /* Detection time = 10ms */
#define TEST_BFD_REMOTE_IP                  0x09010164  /* Destination IP*/
#define TEST_BFD_MAX_NUM_PERIODS            7           /* Maximum number of allowed periods*/
#define TEST_BFD_NUM_DEF_PERIODS            6           /* Number of predifined values for EP periods*/
#define TEST_BFD_MAX_NUM_INTERVALS          8           /* Maximum number of values for Required Min RX and Desire Min TX Interval */
#define TEST_BFD_NUM_IP                     16          /* Maximum number of IPs */

#define OAMP_TEST_DEF_PORT_1                13          /* Default value for port_1 */
#define OAMP_TEST_DEF_PORT_2                14          /* Default value for port_2*/
#define OAMP_TEST_DEF_NUM_EP_PAIRS          1024        /* Default value for number pairs of endpoint*/
#define OAMP_TEST_DEF_SEED                  0           /* Default value for seed number */
#define OAMP_TEST_MAX_NUM_DEVICES           2           
#define OAMP_TEST_DEF_NUM_STATES            0           /* Default value for number of states to be changed */
#define OAMP_TEST_DEF_NUM_UPDATE            0           /* Default value for number update of period */
#define TEST_OAM_NUM_LIFS                   1024

typedef struct oamp_test_init_param_s {
    char *option;                                       /* Test type (BFD/OAM) */
    int port1;                                          /* port on which endpoints will be defined*/
    int port2;                                          /* port on which endpoints will be defined*/
    int numEndpoints;                                   /* Number of endpoints to be created*/
    int seed;                                           /* Seed of the test*/
    int numStates;                                      /* The number of times that the state will be change */
    int numUpdate;                                      /* The number of random MEPS to be updated */
} oamp_test_init_param_t;

typedef struct _bcm_dpp_oam_bfd_sw_db_lock_s {
    sal_mutex_t lock;
    int is_init;
} test_bfd_lock_t;

typedef struct endpoint_create_info_s {
    int vlan;
    bcm_gport_t vlan_port_id;
    int name;
    int level;
    int port;
} endpoint_create_info_t;

#define TEST_BFD_INFO_LOCK \
    if (sal_mutex_take(test_bfd_info_lock.lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d."), FUNCTION_NAME(), unit)); \
    }

#define TEST_BFD_INFO_UNLOCK  \
   if (sal_mutex_give(test_bfd_info_lock.lock)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d."), FUNCTION_NAME(), unit)); \
    }
extern int oamp_usage_parse(int unit, args_t *a, oamp_test_init_param_t *init_param);

extern int test_run_oam(int unit, int port_1, int port_2, int numEndpoints, int seed, int numUpdate);
extern int test_oam_create_vlan_gport(int unit, int port, int vlan, int * vlan_port_id);
extern int test_oam_create_endpoint_with_rmep(int unit, endpoint_create_info_t *ep1, endpoint_create_info_t *ep2, int mep_index, int group);
extern int test_oam_updateLMEP(int unit,int index);
extern int test_oam_updateRMEP(int unit,int index);
extern int test_oam_init_and_surroundings(int unit, int * group_id);
extern int test_oam_register_events(int unit);

extern int test_run_bfd(int unit, int port_1, int port_2, int numEndpoints, int seed,int numStates);
extern int test_bfd_Multithreding(int unit, int endpoint_id);
extern int test_bfd_update_state(int unit, int endpoint_id);
extern int test_bfd_createEndpoint(int unit,int local_port, int encap_id, int src_ip,int remote_ip, int local_discr, int remote_discr);
extern int test_bfd_updateEndpoint(int unit, int endpoint_1_id, int endpoint_2_id );
extern int test_bfd_preDefinedUpdate(int unit, int endpoint_1_id, int endpoint_2_id);
extern int test_bfd_changeState(int unit, int endpoint_id);
extern int test_bfd_add_host(int unit, int addr, int vrf, int intf);
extern int test_bfd_register_events(int unit);
extern int test_bfd_init_ports(int unit,int local_port, int remote_port, int vlan) ;
extern void test_bfd_set_global_values(void);

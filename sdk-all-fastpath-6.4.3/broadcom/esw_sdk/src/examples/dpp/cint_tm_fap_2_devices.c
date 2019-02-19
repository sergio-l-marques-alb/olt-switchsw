/* $Id: cint_tm_fap_2_devices.c,v 1.2 Broadcom SDK $
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
*/

cint_reset();


struct info_t {
    /* Number of devices in the system */
    int         nof_devices;
    /* Number of physical ports per device */
    int         num_ports;
    /* Number of internal ports (OLP,ERP) per device */
    int         num_internal_ports;
    /* Number of cos levels (COSQ) */
    int         num_cos;
    /* CPU system port */
    int         cpu_sys_port;
    /* Offset given for a system port on each device */
    int         offset_start_of_sys_port;
    /* Offset given for a voq */
    int         offset_start_of_voq;
    /* Offset given for a voq connector */    
    int         offset_start_of_voq_connector;
    /* Present local ports on each device */
    bcm_gport_t dest_local_gport[80];    
    /* 
     * Present all VOQ connectors on egress device 
     * For each dest local port create 2 voq connectors, for each device 
     */
    bcm_gport_t gport_ucast_voq_connector_group[2*80];
    /* 
     * Present all VOQ on ingress device, 
     * For each system port create VOQ 
     */
    bcm_gport_t gport_ucast_queue_group[2*80];
    /* Present all FQ scheduling on egress device, one for each local port */
    bcm_gport_t gport_ucast_scheduler[80*8];    
    
    /* If true, a single CPU control two units, and no need to allocate resources with ID.
       Otherwise, this script runs on two CPUs, and we allocate with-ID in order to know how
       to bind ingress to egress resources. */
    int central_cpu;
};

int MAX_COS = 8;
int MAX_NUM_PORTS = 80;
info_t g_info; /* global info */

/* 
 * Utils function: Converting bewtween IDs
 */
/* Convert (module id,port) id to sysport id */
int
convert_modport_to_sysport_id(int modid,int port)
{
    /* ITMH cannot be sent to system port 0, so we're using system port
               100 instead (for CPU) */
    if (port == 0) {
        port = g_info.cpu_sys_port;
    }
    return g_info.offset_start_of_sys_port * modid + port;
}

/* 
 * Convert sysport to base VOQ id 
 * The conversation includes: 
 * VOQ ID = offset + (sysport-1)*num_cos. 
 * offset specify the start of the VOQ range (right after the FMQ range) by default 4.
 */
int
convert_sysport_id_to_base_voq_id(int sysport)
{
    /* Assume, no system port 0 */
    if (sysport == 0) {
        return -1;
    }
    return g_info.offset_start_of_voq + (sysport-1)*g_info.num_cos;
}

/* 
 * Convert (local port,ingress_modid) to VOQ connector base 
 * VOQ connector ID  = offset + egress_local_port * nof_devices * max_cos + ingress_modid*max_cos
 * specify offset start of the VOQ connector ID, by default 32. 
 * max_cos is 8
 */

int
convert_modport_to_base_voq_connector(int local_port,int ingress_modid)
{
    return g_info.offset_start_of_voq_connector + local_port*g_info.nof_devices*MAX_COS
        + ingress_modid*MAX_COS;
}

/* 
 * Purpose: Initializes Looks at current configuration and populates
 *          g_info based on that
 */
int
global_info_init (int unit, int nof_devices)
{
    int         rv = BCM_E_NONE;
    
    g_info.num_cos = 4;    
    g_info.nof_devices = nof_devices;    
    g_info.num_ports = 8;
    g_info.cpu_sys_port = 99;
    /* Offset sysport per device */
    g_info.offset_start_of_sys_port = 100;
    /* Offset VOQ */
    g_info.offset_start_of_voq = 4;
    /* Offset VOQ connector */
    g_info.offset_start_of_voq_connector = 32;
    
    g_info.central_cpu = 0;

    return rv;
}

/* This function create system ports of the system */
int
stk_init(int unit, int mymodid)
{
    int rv = BCM_E_NONE;
    int idx, port, sys_port, intern_idx, modid;
    bcm_gport_t internal_gport[80];
    bcm_gport_t modport_gport,sysport_gport;
    int int_flags;

    /* Set modid */
    rv = bcm_stk_my_modid_set(unit, mymodid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_my_modid_set failed. Error:%d (%s)\n",  
                 rv, bcm_errmsg(rv));
        return rv;
    }

    /* Get number of internal ports */
    /* Assuming all devices have the same amount internal ports */    
    int_flags =
        BCM_PORT_INTERNAL_EGRESS_REPLICATION | BCM_PORT_INTERNAL_OLP;
    rv = bcm_port_internal_get(unit, int_flags, MAX_NUM_PORTS,
                               internal_gport,&g_info.num_internal_ports);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_internal_get failed. Error:%d (%s)\n",  
                 rv, bcm_errmsg(rv));
        return rv;
    }
    
    /* 
     * Creation of system ports in the system 
     * Iterate over port + internal ports. After the iteration, num_ports will
     *  be incremented by internal_num_ports 
     * Assuming each device has the same num ports + internal ports  
     */    
    for (modid = 0; modid < g_info.nof_devices; modid++) {       
        intern_idx = 0;
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            if (idx <= g_info.num_ports) {
                port = idx;            
            } else {
                /* Internal port */
                if (BCM_GPORT_IS_LOCAL(internal_gport[intern_idx])) {
                    port = BCM_GPORT_LOCAL_GET(internal_gport[intern_idx]);
                } else if (BCM_GPORT_IS_MODPORT(internal_gport[intern_idx])) {
                    port = BCM_GPORT_MODPORT_PORT_GET(internal_gport[intern_idx]);                        
                } else {
                    printf("bcm_port_internal_get internal gport type unsupported gport(0x%08x), unit=%d, modid=%d, idx=%d, intern_ndx=%d. Error:%d (%s)\n",
                             internal_gport[intern_idx], unit, modid, idx, intern_idx, rv, bcm_errmsg(rv));
                    return rv;
                }
                   
                intern_idx++;
            }
                
            BCM_GPORT_LOCAL_SET(g_info.dest_local_gport[idx],port);
            /* System port id depends on modid + port id */ 
            sys_port = convert_modport_to_sysport_id(modid,port);                                  
            BCM_GPORT_MODPORT_SET(modport_gport, modid, port);
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);
                
            rv = bcm_stk_sysport_gport_set(unit, sysport_gport, 
                                           modport_gport);
            if (BCM_FAILURE(rv)) {
                printf("bcm_stk_sysport_gport_set(%d, %d, %d) failed. Error:%d (%s)\n", 
                         unit, sysport_gport, modport_gport, rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }

    rv = bcm_stk_module_enable(unit, mymodid, g_info.num_ports+g_info.num_internal_ports, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_petra_stk_module_enable failed. Error:%d (%s)\n",  
                 rv, bcm_errmsg(rv));
        return rv;
    }
    
    return rv;
}

/* This function create the scheduling scheme */
int
cosq_init_ing_egr_alloc(int unit) {
    int                         rv = BCM_E_NONE;
    int                         idx, cos, port;
    uint32                      flags;
    int queue_range = 0;
    int modid, sysport_id, voq_base_id, voq_connector_id;
    bcm_gport_t sysport_gport,modport_gport, e2e_gport;

    /* 
     * Before creating VOQs, User must specify the range of the FMQs in
     * the device.
     * In most cases, where fabric multicast is only defined by packet tc,
     * range should set between 0-3. 
     * Set range that is different than 0-3, need to change fabric scheduler
     * mode. 
     * Please see more details in the UM, Cint example: 
     * cint_enhance_application.c 
     * and API: 
     * bcm_fabric_control_set type: bcmFabricMulticastSchedulerMode. 
     */     
    queue_range = 0;
    rv = bcm_fabric_control_set(unit,bcmFabricMulticastQueueMin,queue_range);
    if (BCM_FAILURE(rv)) {
        printf("bcm_fabric_control_set queue_id :%d failed. Error:%d (%s)\n", queue_range, rv, bcm_errmsg(rv));
        return rv;
    }
    
    queue_range = 3;
    rv = bcm_fabric_control_set(unit,bcmFabricMulticastQueueMax,queue_range);
    if (BCM_FAILURE(rv)) {
        printf("bcm_fabric_control_set queue_id :%d failed. Error:%d (%s)\n", queue_range, rv, bcm_errmsg(rv));
        return rv;
    }
    
    /* 
     * Creating Schedling Scheme 
     * This is done by the following steps: 
     * 1. Egress: Create for each local port: following FQs, VOQ connectors. 
     * 2. Ingress: Create VOQs for each system port. 
     * 3. Connect Ingress VOQs <=> Egress VOQ connectors. 
     * Pay attention the scheduling scheme assumes static IDs for VOQ connectors,VOQs. 
     * This is depended by the local and system ports in the system.
     * Conversion is done static using utils functions. 
     */
    /* Stage I: Egress Create FQs */
    for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
        /* Replace HR mode to enhance */
        flags = (BCM_COSQ_GPORT_SCHEDULER |
                BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED |
                 BCM_COSQ_GPORT_REPLACE);
        
        /* e2e gport */
        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]));        
        rv = bcm_cosq_gport_add(unit, g_info.dest_local_gport[idx], 1, flags,
                                &e2e_gport);

        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_gport_add replace hr scheduler idx:%d failed. Error:%d (%s)\n", idx, rv, bcm_errmsg(rv));
            return rv;
        }

        /* Create FQ per traffic class i, attach it to HR SPi */
        for (cos = 0; cos < g_info.num_cos; cos++) 
        {
            flags = (BCM_COSQ_GPORT_SCHEDULER | 
                BCM_COSQ_GPORT_SCHEDULER_FQ);

            rv = bcm_cosq_gport_add(unit, e2e_gport, 1, flags, 
                                    &g_info.gport_ucast_scheduler[idx*MAX_COS+cos]);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_add fq scheduler idx:%d cos: %d failed. Error:%d (%s)\n", idx, cos, rv, bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_cosq_gport_sched_set(unit, 
                         g_info.gport_ucast_scheduler[idx*MAX_COS+cos], 0, 
                         BCM_COSQ_SP0 + cos,0);


            /* attach hr scheduler to fq */
            rv = bcm_cosq_gport_attach(unit, e2e_gport,
                             g_info.gport_ucast_scheduler[idx*MAX_COS+cos], 0);

            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_attach hr scheduler-fq idx:%d cos:%d failed. Error:%d (%s)\n", 
                         idx, cos, rv, bcm_errmsg(rv));
                return rv;
            }
        }        
    }

    /* Stage I: Egress Create VOQ connectors */
    for (modid = 0; modid < g_info.nof_devices; modid++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            /* create voq connector - with ID */
            flags = BCM_COSQ_GPORT_VOQ_CONNECTOR; 
            
            /* If both devices are control by a central CPU, no need to allocate with ID,
               since we use the ID allocated by the BCM */
            if (!g_info.central_cpu) {
                flags |= BCM_COSQ_GPORT_WITH_ID; 
                port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);
                voq_connector_id  = convert_modport_to_base_voq_connector(port,modid);
                BCM_COSQ_GPORT_VOQ_CONNECTOR_SET(g_info.gport_ucast_voq_connector_group[modid*MAX_NUM_PORTS+idx],voq_connector_id);
            }
                        
            rv = bcm_cosq_gport_add(unit, g_info.dest_local_gport[idx], g_info.num_cos, 
                              flags, &g_info.gport_ucast_voq_connector_group[modid*MAX_NUM_PORTS+idx]);
    
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_add connector idx:%d failed. Error %d (%s)\n", idx, rv, bcm_errmsg(rv));
                return rv;
            }
    
            for (cos = 0; cos < g_info.num_cos; cos++) {
                /* Each VOQ connector attach suitable FQ traffic class */
                rv = bcm_cosq_gport_sched_set(unit, 
                             g_info.gport_ucast_voq_connector_group[modid*MAX_NUM_PORTS+idx], cos, 
                             BCM_COSQ_SP0,0);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_cosq_gport_sched_set connector idx:%d cos:%d failed. Error:%d(%s)\n", 
                             idx, cos, rv, bcm_errmsg(rv));
                    return rv;
                }
                /* attach fq scheduler to connecter */
                rv = bcm_cosq_gport_attach(unit, g_info.gport_ucast_scheduler[idx*MAX_COS+cos],
                                 g_info.gport_ucast_voq_connector_group[modid*MAX_NUM_PORTS+idx], cos);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_cosq_gport_attach fq scheduler-connector idx:%d cos:%d failed. Error:%d (%s)\n", 
                             idx, cos, rv, bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }
    
    /* Stage 2: Ingress: Create VOQs for each system port. */
    for (modid = 0; modid < g_info.nof_devices; modid++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
                       
            BCM_GPORT_MODPORT_SET(modport_gport, modid, BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]));

            if (!g_info.central_cpu) {
                flags |= BCM_COSQ_GPORT_WITH_ID;
                
                rv = bcm_stk_gport_sysport_get(unit,modport_gport,&sysport_gport);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_stk_gport_sysport_get get sys port failed. Error:%d (%s) \n", 
                             rv, bcm_errmsg(rv));
                   return rv;
                }
                
                sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);
                voq_base_id = convert_sysport_id_to_base_voq_id(sysport_id);
             
                BCM_GPORT_UNICAST_QUEUE_GROUP_SET(g_info.gport_ucast_queue_group[modid*MAX_NUM_PORTS+idx],voq_base_id);
            }

            rv = bcm_cosq_gport_add(unit, modport_gport, g_info.num_cos, 
                                    flags, &g_info.gport_ucast_queue_group[modid*MAX_NUM_PORTS+idx]);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_add UC queue failed. Error:%d (%s) \n", 
                         rv, bcm_errmsg(rv));
               return rv;
            }            
        }
    }
    
    return rv;
}

int
cosq_init_ing_egr_connect(int unit, int mymodid) {
    int                         rv = BCM_E_NONE;
    int                         idx, port;    
    bcm_cosq_gport_connection_t connection;
    int modid, sysport_id, voq_base_id, voq_connector_id;
    bcm_gport_t voq_connector_gport,sysport_gport,modport_gport, voq_base_gport;    

    /* Stage 3: Connect Ingress VOQs <=> Egress VOQ connectors. */
    /* Ingress: connect voqs to voq connectors */
    for (modid = 0; modid < g_info.nof_devices; modid++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
            connection.remote_modid = modid;
            connection.voq = g_info.gport_ucast_queue_group[modid*MAX_NUM_PORTS+idx];
            connection.voq_connector = g_info.gport_ucast_voq_connector_group[mymodid*MAX_NUM_PORTS+idx];
            
            rv = bcm_cosq_gport_connection_set(unit, &connection);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_connection_set ingress modid: %d, idx:%d failed. Error:%d (%s)\n", modid, idx, rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }            

    /* Egress: connect voq connectors to voqs */
    for (modid = 0; modid < g_info.nof_devices; modid++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
            connection.remote_modid = modid;
            connection.voq_connector = g_info.gport_ucast_voq_connector_group[modid*MAX_NUM_PORTS+idx];
            connection.voq = g_info.gport_ucast_queue_group[mymodid*MAX_NUM_PORTS+idx];

            rv = bcm_cosq_gport_connection_set(unit, &connection);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_connection_set egress modid: %d, idx:%d failed. Error:%d (%s)\n", modid, idx, rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }       

    return rv;
}

/*
 * Purpose: Initialize basic components of Soc_petra-B on GFA-BI card.
 * Note:    This is intended to configure only using BCM API.
 * This is the main function of this cint example. The following steps are being done:
 * 1. Setup the diag init structure. (global_info_init)
 * 2. Create system ports in the system, set device id, enable traffic. (stk_init)
 * 3. Run the Cosq Application (Scheduling scheme) (cosq_init)
 * Parameters:
 *  - unit:
 *  - nof_units: Number of devices in the system.
 *  - unit_2nd: unit id of 2nd unit controlled by this CPU. If -1, each CPU controls
 *              a single unit.
 */
int
bcm_pb_init(int unit, int modid, int nof_units, int unit_2nd, int modid_2nd)
{
    int rv = BCM_E_NONE;
    int cfg_2nd_unit;    

    /*
    if ((nof_units == 2) && (unit_2nd < 0)) {
      printf("Number of units is 2, but 2nd unit id (%d) is negative ", unit_2nd);
      return BCM_E_PARAM;
    }
    */
    /* misc init should be called first */
    rv = global_info_init(unit, nof_units);
    if (BCM_FAILURE(rv)) {
        printf("global_info_init: failed. Error:%d (%s) \n", 
                 rv, bcm_errmsg(rv));
        return rv;
    }
    
    cfg_2nd_unit = (g_info.central_cpu && (nof_units > 1));

    rv = stk_init(unit, modid);
    if (BCM_FAILURE(rv)) {
        printf("stk_init: failed. Error:%d (%s) \n", 
                 rv, bcm_errmsg(rv));
        return rv;
    }
    
    if (cfg_2nd_unit) {
        rv = stk_init(unit_2nd, modid_2nd);
        if (BCM_FAILURE(rv)) {
            printf("stk_init: failed. Error:%d (%s) \n", 
                     rv, bcm_errmsg(rv));
            return rv;
        }
    }
   
    /*
     * COSQ Init
     */
    
    rv = cosq_init_ing_egr_alloc(unit);
    if (BCM_FAILURE(rv)) {
        printf("cosq_init_ing_egr_alloc: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    if (cfg_2nd_unit) {
        rv = cosq_init_ing_egr_alloc(unit_2nd);
        if (BCM_FAILURE(rv)) {
            printf("cosq_init_ing_egr_alloc: failed. unit: %d, Error:%d (%s) \n", unit_2nd, rv, bcm_errmsg(rv));
            return rv;
        }
    }
    
    rv = cosq_init_ing_egr_connect(unit, modid);
    if (BCM_FAILURE(rv)) {
        printf("cosq_init_ing_egr_connect: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    if (cfg_2nd_unit) {
        rv = cosq_init_ing_egr_connect(unit_2nd, modid_2nd);
        if (BCM_FAILURE(rv)) {
            printf("cosq_init_ing_egr_connect: failed. unit: %d, Error:%d (%s) \n", unit_2nd, rv, bcm_errmsg(rv));
            return rv;
        }
    }
    
    return rv;
}

/*
 * $Id: cint_loopback_sr_cell.c,v 1.9 Broadcom SDK $ 
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
 * DCMN send \ receive loopback SR cell example:
 * 
 * The example simulate:
 *  1. define single route and send loopback sr cell to this route
 *  2. receive sr cells
 *
 */

uint32 data_set[16];
uint32 data_get[16];

int
send_route_loopback (int unit, bcm_port_t port, bcm_port_loopback_t lb) {
    int rv;
    bcm_fabric_route_t route;
    int lb_orig;
    bcm_port_t lane;
    bcm_info_t info;

    bcm_fabric_route_t_init(&route);
    
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("error, in bcm_info_get, rv=%d\n", rv);
        return rv;
    }
   
    /* if device is arad, find link corresponding to fabric port*/
    if((info.device & 0xff00) == 0x8600) {
        lane = _SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    }
    else {
        lane = port;
    }


    rv = bcm_port_loopback_get(unit, port, &lb_orig);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_loopback_get, rv=%d\n",rv);
        return rv;
    }

    rv = bcm_port_loopback_set(unit, port, lb);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_loopback_set 1, rv=%d\n",rv);
        return rv;
    }
    

    bshell(unit, "Sleep");

    /*define a single route from FE2 to FE2*/
    route.number_of_hops = 1;
    route.hop_ids = &lane;
    route.pipe_id =  -1; 

    /*build data*/
    data_set[0]  = 0x02002300;
    data_set[1]  = 0x14543656;
    data_set[2]  = 0x22222222;
    data_set[3]  = 0x34243663;
    data_set[4]  = 0x47478444;
    data_set[5]  = 0x55555555;
    data_set[6]  = 0x60890635;
    data_set[7]  = 0x77777777;
    data_set[8]  = 0x23432434;
    data_set[9]  = 0x43545889;
    data_set[10] = 0x423443aa;
    data_set[11] = 0xb2533bbb;
    data_set[12] = 0xcc123cc6;
    data_set[13] = 0xdddddddd;
    data_set[14] = 0xeeee5435;
    data_set[15] = 0xf131ffff;


    /*send source-routed cell*/
    rv = bcm_fabric_route_tx(unit, 0, &route, 16 /*in words*/, data_set);
    if (rv != BCM_E_NONE) {
        printf("Error, in soc_send_sr_cell, rv=%d\n",rv);
        return rv;
    }

    rv = bcm_port_loopback_set(unit, port, lb_orig);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_loopback_set 2, rv=%d\n",rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*receive sr cells*/
int
receive_sr_cell(int unit, int max_messages) {
    int rv, count, i;
    uint32 data_actual_size;

    /* 
     * in case several sr cells received the cells are accumulated in SW 
     * For that reason it's important to read in loop (even if the relevant interrupt is down) 
     * until soc_receive_sr_cell return EMPTY error.
    */
    count = 0;
    while(count < max_messages) {
        /*receive sr cell data*/
        rv = bcm_fabric_route_rx(unit, 0, 16, data_get, &data_actual_size);
         /*all messages was read*/
        if(BCM_E_EMPTY == rv) {
            printf("No more messages to read \n");
            break;
        } else if (rv != BCM_E_NONE) {
            printf("Error, in bcm_fabric_route_rx, rv=%d\n",rv);
            return rv;
        }

         /*print received data*/
        printf("Message received: ");
        for(i=0 ; i<data_actual_size ; i++) {
            printf("0x%x ",data_get[i]);
        }
        printf("\n");

        count++;
    }

    printf("%d messages received \n",count);

   return BCM_E_NONE;
}

int run_sr_cell_loopback(int unit, bcm_port_t port, bcm_port_loopback_t lb) {
  int i;
  int pass = 1;
  int rv;

  /*Clear buffer*/
  receive_sr_cell(unit, 1000);

  rv = send_route_loopback(unit, port, lb); 
  if (rv != BCM_E_NONE) {
      printf("run_sr_cell_loopback: FAILED\n");
      return rv;
  }
  bshell(unit, "Sleep 5");
  rv = receive_sr_cell(unit, 1);
  if (rv != BCM_E_NONE) {
      printf("run_sr_cell_loopback: FAILED\n");
      return rv;
  }
  for (i = 0; i < 16; i++) {
      if (data_set[i] != data_get[i]) {
          pass = 0;
      }
  }
  if (pass) {
      printf("run_sr_cell_loopback: PASS\n");
  }

}


/*
 * $Id: cint_sr_cell.c,v 1.3 Broadcom SDK $
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
 * DCMN send \ receive SR cell example:
 * 
 * The example simulate:
 *  1. define single route and send  sr cell to this route
 *  2. define route group and cell sr cell to the group
 *  3. receive sr cells
 *
 */

uint32 data_set[16];
uint32 data_get[76];


/*define single route and send  sr cell to this route*/
int
send_route(int unit) {
    int rv, route_id;
    int link_ids[2];
    bcm_fabric_route_t route;

    bcm_fabric_route_t_init(&route);

   
    /*define a single route from FE1 to FE3*/
    route.number_of_hops = 2;
    link_ids[0] = 15; /*goto FE through link 15*/
    link_ids[1] = 0; /*goto FAP through link 1*/
    route.hop_ids = link_ids;

    /*build data*/
    data_set[0]  = 0x00000000;
    data_set[1]  = 0x11111111;
    data_set[2]  = 0x22222222;
    data_set[3]  = 0x33333333;
    data_set[4]  = 0x44444444;
    data_set[5]  = 0x55555555;
    data_set[6]  = 0x66666666;
    data_set[7]  = 0x77777777;
    data_set[8]  = 0x88888888;
    data_set[9]  = 0x99999999;
    data_set[10] = 0xaaaaaaaa;
    data_set[11] = 0xbbbbbbbb;
    data_set[12] = 0xcccccccc;
    data_set[13] = 0xdddddddd;
    data_set[14] = 0xeeeeeeee;
    data_set[15] = 0xffffffff;

    /*send source-routed cell*/
    rv = bcm_fabric_route_tx(unit, 0,route, 16, data_set);
      if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_route_tx, rv=%d, \n", rv);
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
        rv = bcm_fabric_route_rx(unit, 0, 76, data_get, &data_actual_size);
         /*all messages was read*/
        if(BCM_E_EMPTY == rv) {
            printf("No more messages to read \n");
            break;
        } else if (rv != BCM_E_NONE) {
            printf("Error, in soc_receive_sr_cell, rv=%d, \n", rv);
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

   printf("%d messages received \n", count);

   return BCM_E_NONE;    
}


int run_sr_cell(int unit) {
  send_route(unit); 
  receive_sr_cell(unit, 1);
}

int cint_sr_cell_system_test(int unit_fap_0, int unit_fap_1) {

  int i;
  int pass = 1;
  int rv;

  /*Clear buffer*/
  receive_sr_cell(unit_fap_1, 1000);

  rv = send_route(unit_fap_0); 
  if (rv != BCM_E_NONE)
  {
      printf("failed to send cell\n");
      return rv;
  }

  rv = receive_sr_cell(unit_fap_1, 1);
  if (rv != BCM_E_NONE)
  {
      printf("failed to send cell\n");
      return rv;
  }

  for (i = 0; i < 16; i++) {
      if (data_set[i] != data_get[i]) {
          pass = 0;
      }
  }
  if (pass) {
      printf("cint_sr_cell_system_test: PASS\n");
  }

}


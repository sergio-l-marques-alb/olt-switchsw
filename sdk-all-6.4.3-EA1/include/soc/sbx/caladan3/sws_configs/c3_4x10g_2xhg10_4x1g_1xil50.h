
/*
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
 * $Id: c3_4x10g_2xhg10_4x1g_1xil50.h,v 1.1.2.4 Broadcom SDK $
 */
#ifdef BCM_CALADAN3_SUPPORT

sws_config_t c3_4x10g_2xhg10_4x1g_1xil50_cfg = {

    /*
     * QM Config
     */
     {
        /* Buffer Thresholds */
         /*
           total_buff_max_pages, total_buff_drop_thres_de1, total_buff_drop_thres_de2,
           fc_total_buffer_xoff_thresh, num_pages_reserved, total_buff_hysteresis_delta,
           ingress_max_pages, ingress_drop_thres_de1, ingress_drop_thres_de2,
           fc_ingress_xoff_thresh, ingress_hysteresis_delta, egress_max_pages,
           egress_drop_thres_de1, egress_drop_thres_de2, fc_egress_xoff_thresh,
           egress_hysteresis_delta, per_queue_drop_hysteresis_delta
         */
        {
            15384, 11538, 7692, 
            11226, 3776, 1539, 
            6818, 5114, 3409,
            10300, 682, 8257,
            6193, 4129, 10300,
            826, 38
        },

        /* Queue Config */
        28, 
        28, 
        {
            /* 
             * Note the Queue ordering here must mirror the Q number space implicitly 
             * defined in the C src!.  
             * 0-11 for XE/GE ports on CLPORT0
             * 12-47 for GE ports on XTPORT0-XTPORT2
             * HG10 appears to start at 12 overlapping number space with XTPORT0
             * and possibly XTPORT1 depending on how many 1G channels on HG10 interface!
             */

            /*
              max_pages, drop_thres_de1, drop_thres_de2,
              flow_ctrl_thresh, min_pages_data, min_pages_header
            */
            {310, 233, 155, 0x3FFF, 68, 68}, /* 0  Reserved Q NOT to be used */
            {310, 233, 155, 0x3FFF, 68, 68}, /* 1  Placeholder*/

             /*GEs */                 
            {64, 48, 32, 0x3FFF, 17, 17}, /* 2 */ 
            {64, 48, 32, 0x3FFF, 17, 17}, /* 3 */ 

            /*XEs */
            {310, 233, 155, 0x3FFF, 68, 68}, /* 4 */
            {310, 233, 155, 0x3FFF, 68, 68}, /* 5 */
            {310, 233, 155, 0x3FFF, 68, 68}, /* 6 */
            {310, 233, 155, 0x3FFF, 68, 68}, /* 7 */

            {310, 233, 155, 0x3FFF, 68, 68}, /* 8   Reserved Q NOT to be used*/
            {310, 233, 155, 0x3FFF, 68, 68}, /* 9   Placeholder*/

            /*GEs */                
            {64, 48, 32, 0x3FFF, 17, 17}, /* 10 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 11 */

            /*HG10 */               
            {64, 48, 32, 0x3FFF, 17, 17}, /* 12 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 13 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 14 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 15 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 16 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 17 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 18 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 19 */
                                    
            /*HG10 */               
            {64, 48, 32, 0x3FFF, 17, 17}, /* 20 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 21 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 22 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 23 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 24 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 25 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 26 */
            {64, 48, 32, 0x3FFF, 17, 17}, /* 27 */

            { 0 }, /* 28 */
            { 0 }, /* 29 */
            { 0 }, /* 30 */
            { 0 }, /* 31 */
            { 0 }, /* 32 */
            { 0 }, /* 33 */
            { 0 }, /* 34 */
            { 0 }, /* 35 */
            { 0 }, /* 36 */
            { 0 }, /* 37 */
            { 0 }, /* 38 */
            { 0 }, /* 39 */
            { 0 }, /* 40 */
            { 0 }, /* 41 */
            { 0 }, /* 42 */
            { 0 }, /* 43 */
            { 0 }, /* 44 */
            { 0 }, /* 45 */
            { 0 }, /* 46 */
            { 0 }, /* 47 */
            { 0 }, /* 48 */
            { 0 }, /* 49 */
            { 0 }, /* 50 */
            { 0 }, /* 51 */
            { 0 }, /* 52 */
            { 0 }, /* 53 */
            { 0 }, /* 54 */
            { 0 }, /* 55 */
            { 0 }, /* 56 */
            { 0 }, /* 57 */
            { 0 }, /* 58 */
            { 0 }, /* 59 */
            { 0 }, /* 60 */
            { 0 }, /* 61 */
            { 0 }, /* 62 */
            { 0 }, /* 63 */

        },

        {
            {271, 204, 136, 121, 17, 17}, /* 0  Phantom XE0 */
            {271, 204, 136, 121, 17, 17}, /* 1  Placeholder */

            {271, 204, 136, 121, 17, 17}, /* 2 */
            {271, 204, 136, 121, 17, 17}, /* 3 */

            {1007, 756, 504, 227, 68, 68}, /* 4 */
            {1007, 756, 504, 227, 68, 68}, /* 5 */
            {1007, 756, 504, 227, 68, 68}, /* 6 */
            {1007, 756, 504, 227, 68, 68}, /* 7 */

            {1007, 756, 504, 227, 68, 68}, /* 8 Phantom XE5 */

            {271, 204, 136, 121, 17, 17}, /* 9  Placeholder not used*/

            {271, 204, 136, 121, 17, 17}, /* 10 */
            {271, 204, 136, 121, 17, 17}, /* 11 */

            {271, 204, 136, 121, 17, 17}, /* 12 */
            {271, 204, 136, 121, 17, 17}, /* 13 */
            {271, 204, 136, 121, 17, 17}, /* 14 */
            {271, 204, 136, 121, 17, 17}, /* 15 */
            {271, 204, 136, 121, 17, 17}, /* 16 */
            {271, 204, 136, 121, 17, 17}, /* 17 */
            {271, 204, 136, 121, 17, 17}, /* 18 */
            {271, 204, 136, 121, 17, 17}, /* 19 */

            {271, 204, 136, 121, 17, 17}, /* 20 */
            {271, 204, 136, 121, 17, 17}, /* 21 */
            {271, 204, 136, 121, 17, 17}, /* 22 */
            {271, 204, 136, 121, 17, 17}, /* 23 */
            {271, 204, 136, 121, 17, 17}, /* 24 */
            {271, 204, 136, 121, 17, 17}, /* 25 */
            {271, 204, 136, 121, 17, 17}, /* 26 */
            {271, 204, 136, 121, 17, 17}, /* 27 */

            { 0 }, /* 28 */
            { 0 }, /* 29 */
            { 0 }, /* 30 */
            { 0 }, /* 31 */
            { 0 }, /* 32 */
            { 0 }, /* 33 */
            { 0 }, /* 34 */
            { 0 }, /* 35 */
            { 0 }, /* 36 */
            { 0 }, /* 37 */
            { 0 }, /* 38 */
            { 0 }, /* 39 */
            { 0 }, /* 40 */
            { 0 }, /* 41 */
            { 0 }, /* 42 */
            { 0 }, /* 43 */
            { 0 }, /* 44 */
            { 0 }, /* 45 */
            { 0 }, /* 46 */
            { 0 }, /* 47 */
            { 0 }, /* 48 */
            { 0 }, /* 49 */
            { 0 }, /* 50 */
            { 0 }, /* 51 */
            { 0 }, /* 52 */
            { 0 }, /* 53 */
            { 0 }, /* 54 */
            { 0 }, /* 55 */
            { 0 }, /* 56 */
            { 0 }, /* 57 */
            { 0 }, /* 58 */
            { 0 }, /* 59 */
            { 0 }, /* 60 */
            { 0 }, /* 61 */
            { 0 }, /* 62 */
            { 0 }, /* 63 */
        }, 

        /* Cmic queues */
        {258, 194, 129, 0, 64, 64},

        /* XL queues */
        { {256, 192, 128, 0, 64, 64}, {256, 192, 128, 0, 64, 64} },

        /* Ingress Redirection Queues */
        { {128, 96, 64, 0, 0, 64}, {128, 96, 64, 0, 0, 64} },

        /* Egress Redirection Queues */
        { {128, 96, 64, 0, 0, 64}, {128, 96, 64, 0, 0, 64} },

        /* Ingress Bubble Queues */
        {128, 96, 64, 0, 0, 64},

        /* Egress Bubble Queues */
        {128, 96, 64, 0, 0, 64},

    },

    /*
     * Line PT Config
     */
    {
        /* Fifo info */
        {
            51,
            {
                {0,0,8,2},    /* Port 0   Phantom Port XE0*/

                /*1 HG10 @ 10G*/
                {1,64,8,2},    /* Port 1 */

                /*2 GEs @ 1G*/
                {2,128,2,1},   /* Port 2 */
                {3,144,2,1},   /* Port 3 */

                /*4 XEs @ 10G*/
                {4,160,8,2},   /* Port 4 */
                {5,224,8,2},  /* Port 5 */
                {6,288,8,2},  /* Port 6 */
                {7,352,8,2},  /* Port 7 */

                {8,416,8,2},    /* Port 8   Phantom Port XE5*/

                /*1 HG10 @ 10G*/
                {9,416,8,2},  /* Port 9 */

                /*2 GEs @ 1G*/
                {10,480,2,1},    /* Port 10 */
                {11,496,2,1},    /* Port 11 */

                {12,0,0,0},   /* Port 12 */
                {13,0,0,0},   /* Port 13 */
                {14,0,0,0},   /* Port 14 */
                {15,0,0,0},   /* Port 15 */
                {16,0,0,0},   /* Port 16 */
                {17,0,0,0},   /* Port 17 */
                {18,0,0,0},   /* Port 18 */
                {19,0,0,0},   /* Port 19 */
                {20,0,0,0},   /* Port 20 */
                {21,0,0,0},   /* Port 21 */
                {22,0,0,0},   /* Port 22 */
                {23,0,0,0},   /* Port 23 */
                {24,0,0,0},   /* Port 24 */
                {25,0,0,0},   /* Port 25 */
                {26,0,0,0},   /* Port 26 */
                {27,0,0,0},   /* Port 27 */
                {28,0,0,0},   /* Port 28 */
                {29,0,0,0},   /* Port 29 */
                {30,0,0,0},   /* Port 30 */
                {31,0,0,0},   /* Port 31 */
                {32,0,0,0},   /* Port 32 */
                {33,0,0,0},   /* Port 33 */
                {34,0,0,0},   /* Port 34 */
                {35,0,0,0},   /* Port 35 */
                {36,0,0,0},   /* Port 36 */
                {37,0,0,0},   /* Port 37 */
                {38,0,0,0},   /* Port 38 */
                {39,0,0,0},   /* Port 39 */
                {40,0,0,0},   /* Port 40 */
                {41,0,0,0},   /* Port 41 */
                {42,0,0,0},   /* Port 42 */
                {43,0,0,0},   /* Port 43 */
                {44,0,0,0},   /* Port 44 */
                {45,0,0,0},   /* Port 45 */
                {46,0,0,0},   /* Port 46 */
                {47,0,0,0},   /* Port 47 */
                {48,0,0,0},   /* Port 48 */
                {49,0,0,0},   /* Port 49 */
                {50,0,0,0}    /* Port 50 */
            }
        },

        /* Client Calendar */
        {
            60,
            {
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT4, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT4, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT5, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, 
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0
            },
        },

        /* Port Calendar */
        {
            51,
            {
                2,3,4,5,6,7,10,11,12,
                2,3,4,5,6,7,10,11,
                2,3,4,5,6,7,10,11,12,
                /*48, 49, 50,*/
                2,3,4,5,6,7,10,11,
                2,3,4,5,6,7,10,11,12,
                2,3,4,5,6,7,10,11,

            },
        },
    },
    /*
     * Fabric PT Config
     */
    {
        /* Fifo info */
        {
             1, 
             {
                 { 0,0,76,20 },    /* Port 0*/
              }
        },

        /* Client Cal */
        {
            76,
            {
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0,
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0,
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0,
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0,
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0,
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0,
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0,
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0,
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0, CLIENT0,
                 CLIENT0, CLIENT0, CLIENT0, CLIENT0,
            },
        },

        /* Port Cal */
        {
            108,
            {
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            },
        },
    },

    /*
     * Line PR config
     */
    {

        /* Rx buf config */
        {
            0,
            {
                {0, 0, 0} 
            },
        },

        /* IDP Thresholds */
        { 255, 192, 128, 0, 0 },
    },

    /*
     * Fabric PR config
     */
    {

        /* Rx buf config */
        {
            0,
            {
                {0, 0, 0}
            }
        },

        /* IDP Thresholds */
        { 255, 192, 128, 0, 196 },
    },


};

#endif

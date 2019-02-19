;;;
;;;   $Id: tmu_taps_ut.asm,v 1.3 Broadcom SDK $
;;;
;;; $Copyright: Copyright 2016 Broadcom Corporation.
;;; This program is the proprietary software of Broadcom Corporation
;;; and/or its licensors, and may only be used, duplicated, modified
;;; or distributed pursuant to the terms and conditions of a separate,
;;; written license agreement executed between you and Broadcom
;;; (an "Authorized License").  Except as set forth in an Authorized
;;; License, Broadcom grants no license (express or implied), right
;;; to use, or waiver of any kind with respect to the Software, and
;;; Broadcom expressly reserves all rights in and to the Software
;;; and all intellectual property rights therein.  IF YOU HAVE
;;; NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
;;; IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
;;; ALL USE OF THE SOFTWARE.  
;;;  
;;; Except as expressly set forth in the Authorized License,
;;;  
;;; 1.     This program, including its structure, sequence and organization,
;;; constitutes the valuable trade secrets of Broadcom, and you shall use
;;; all reasonable efforts to protect the confidentiality thereof,
;;; and to use this information only in connection with your use of
;;; Broadcom integrated circuit products.
;;;  
;;; 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
;;; PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
;;; REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
;;; OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
;;; DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
;;; NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
;;; ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
;;; CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
;;; OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
;;; 
;;; 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
;;; BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
;;; INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
;;; ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
;;; TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
;;; POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
;;; THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
;;; WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
;;; ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$   
;;;   Internal -- latency 19 cycles, shadow 48 cycles
;;;   External -- latency 26 cycles, shadow 48 cycles
;;;
;;; --------------------------------------------------------------------------

        const TSR_ERROR_FLAG    = 0x40000000 
        reg   dcr = tsr[27:24]
        reg   ValidContext = tsr[0]
        reg   tsr_ErrorFlag = tsr[10]
        reg   ErrorMessage = r24

        reg   IPV4_SK0_ActualResults_0 = c20
        reg   IPV4_SK0_ActualResults_1 = c21
        reg   IPV4_SK0_ActualResults_2 = c22
        reg   IPV4_SK0_ActualResults_3 = c23

        reg   IPV4_SK1_ActualResults_0 = c16
        reg   IPV4_SK1_ActualResults_1 = c17
        reg   IPV4_SK1_ActualResults_2 = c18
        reg   IPV4_SK1_ActualResults_3 = c19
       
        reg   IPV4_SubKey0_31to0 = r0
        reg   IPV4_SubKey0_63to32 = r1
   
        reg   Key_63_0    = rr0
        reg   Key_127_64  = rr17
        reg   Key_191_128 = rr18
        reg   Key_255_192 = rr19
        reg   Key_319_256 = rr20
        reg   Key_383_320 = rr21
        reg   Key_447_384 = rr22
        reg   Key_511_448 = rr23

        reg   PEID = pir[6:0]

        stream	0:
        ;; execute only for PE id 0, buddy 0
         > 1. jnz THE_END, PEID
         > 1.   mov ValidContext, 1
         > 1.   load IPV4_SubKey0_31to0, p0.129(0)
         > 1.   load IPV4_SubKey0_63to32, p1.129(0)       
	 ;> 1. mov IPV4_SubKey0_31to0, 0x10000000
	 ;> 1. mov IPV4_SubKey0_63to32, 0
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 1.   key 4.2, Key_511_448, Key_447_384
         > 1.   key 4.1, Key_383_320, Key_319_256, Key_255_192
         > 1.   key 4.0, Key_191_128, Key_127_64,  Key_63_0

         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop

         > 7.   switch
   > (!E)? 1.   j NO_TSR_ERROR_s1
   >  (E)? 1.   mov tsr_ErrorFlag, 0
         > 1.   or ErrorMessage, TSR_ERROR_FLAG, ErrorMessage
         > 0.   nop
         > 1.   j  TSR_ERROR_s1
NO_TSR_ERROR_s1:
         > 1.   jz  THE_END, ValidContext
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop    
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
	 > 0. nop
	 > 0. nop
	 > 0. nop
	 > 0. nop
         > 1.   load  IPV4_SK1_ActualResults_0, p0.130(IPV4_SK1_ActualResults_0)
         > 1.   load  IPV4_SK1_ActualResults_1, p1.130(IPV4_SK1_ActualResults_1)
         > 1.   load  IPV4_SK1_ActualResults_2, p2.130(IPV4_SK1_ActualResults_2)
         > 1.   load  IPV4_SK1_ActualResults_3, p3.130(IPV4_SK1_ActualResults_3)

 
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop    
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop        
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop    
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop
         > 0.   nop             
         > 1.   j THE_END
TSR_ERROR_s1:
         > 1.   mov dcr, 1
THE_END:
         > 0.   nop


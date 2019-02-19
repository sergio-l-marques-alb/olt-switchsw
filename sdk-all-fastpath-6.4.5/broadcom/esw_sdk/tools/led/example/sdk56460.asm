; $Id: sdk56460.asm,v 1.1 Broadcom SDK $
;
; $Copyright: Copyright 2012 Broadcom Corporation.
; This program is the proprietary software of Broadcom Corporation
; and/or its licensors, and may only be used, duplicated, modified
; or distributed pursuant to the terms and conditions of a separate,
; written license agreement executed between you and Broadcom
; (an "Authorized License").  Except as set forth in an Authorized
; License, Broadcom grants no license (express or implied), right
; to use, or waiver of any kind with respect to the Software, and
; Broadcom expressly reserves all rights in and to the Software
; and all intellectual property rights therein.  IF YOU HAVE
; NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
; IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
; ALL USE OF THE SOFTWARE.  
;  
; Except as expressly set forth in the Authorized License,
;  
; 1.     This program, including its structure, sequence and organization,
; constitutes the valuable trade secrets of Broadcom, and you shall use
; all reasonable efforts to protect the confidentiality thereof,
; and to use this information only in connection with your use of
; Broadcom integrated circuit products.
;  
; 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
; PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
; REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
; OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
; DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
; NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
; ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
; CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
; OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
; 
; 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
; BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
; INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
; ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
; TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
; THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
; WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
; ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
; 
;
; This is the program used for testing the BCM56460 SVK with LED board.
; To start it, use the following commands from BCM:
;
;       led 0 load sdk56460.hex
;       led 0 auto on
;       led 0 start
;
; TEST LED board used . 1 LED per port is considered. 
; There are two bits per  LED with the following colors:
;       ZERO, ZERO      Black
;       ZERO, ONE       Amber
;       ONE, ZERO       Green
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       LA01, LA02, ..., LA04
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0xA0 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
;
; Current implementation as below and is being tested with TEST LED board:
;
; LA01 reflects port 1 link/activity status:
;       Black: no link
;       Green: Link
;       Green Blink: Activity
;

TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)

MIN_PORT        EQU     1
MAX_PORT        EQU     28

NUM_PORT        EQU     28

;
; Main Update Routine
;
; This routine is called once per tick.
;

update:
        call    delay
        ld      a, MIN_PORT

up1:
        port    a
        ld      (PORT_NUM),a
        call    delay
        call    delay
        call    activity        

        ld      a,(PORT_NUM)
        inc     a
        cmp     a, MAX_PORT+1 
        jnz     up1

        ; Update various timers

up2:
        send    28 *2 ;  

;
; activity
;
; This routine calculates the activity LED for the current port.
; It extends the activity lights using timers (new activity overrides
; and resets the timers).
;
; Inputs: (PORT_NUM)
; Outputs: Two bits sent to LED stream
;

delay:  ld      a,0x50
delay1: dec     a
        jnz     delay1
        ret

activity:
        ld      a,(PORT_NUM)
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,0

        jnc     led_off



        tst  b,4
        push  cy
        tst  b,5
        push  cy
        tor
        pop
        jnc     clr_blink

        ld    b,(PORT_NUM)
        add   b,0x60
        ld    a,(b)

        inc  a
        ld    (b),a

        tst  a,0
        jc   led_off  
clr_blink:
        ld    a,0
        ld    (b),a

glow_link:

        jmp     led_green


;
; led_off, led_yellow, led_green
;
; Inputs: None
; Outputs: Two bits to the LED stream indicating color
; Destroys: None
;

led_yellow:
        push  1
        pack
        push  0
        pack
        ret

led_green:
        push  0
        pack
        push  1
        pack
        ret

led_off:
        push  0
        pack
        push  0
        pack
        ret

;
; Variables (SDK software initializes LED memory from 0xa0-0xff to 0)
;

TXRX_ALT_COUNT  equ     0xe0
HD_COUNT        equ     0xe1
GIG_ALT_COUNT   equ     0xe2
PORT_NUM        equ     0xe3
PD_TEMP        equ     0xe5

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/esw/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA        equ     0xa0    ; Size 24 + 6? bytes

;
; LED extension timers
;

TXRX_TIMERS     equ     0xe4         ; NUM_PORT bytes C0 - DF
;TX_TIMERS       equ     0xe0           ; NUM_PORT bytes E0 - FF

;
; Symbolic names for the bits of the port status fields
;

RX              equ     0x0     ; received packet
TX              equ     0x1     ; transmitted packet
COLL            equ     0x2     ; collision indicator
SPEED_C         equ     0x3     ; 100 Mbps
SPEED_M         equ     0x4     ; 1000 Mbps
DUPLEX          equ     0x5     ; half/full duplex
FLOW            equ     0x6     ; flow control capable
LINKUP          equ     0x7     ; link down/up status
LINKEN          equ     0x8     ; link disabled/enabled status
ZERO            equ     0xE     ; always 0
ONE             equ     0xF     ; always 1


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_txrx.h
*
* @purpose TACACS+ Client definitions related to transmit and receive
*          functions.
*
* @component tacacs+
*
* @comments This file consists of constant definitions related to 
*           transmit and receive functions.
*
* @create 03/17/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_TACACS_TXRX_H
#define INCLUDE_TACACS_TXRX_H

#define L7_TACACS_SESSION_POLL_TIMEOUT  1 /* 1 second */

L7_RC_t tacacsAuthSessionsPoll(void);

#endif /* INCLUDE_TACACS_TXRX_H */

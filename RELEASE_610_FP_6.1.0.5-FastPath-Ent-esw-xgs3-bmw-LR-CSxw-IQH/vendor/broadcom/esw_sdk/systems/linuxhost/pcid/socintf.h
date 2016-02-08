/*
 * $Id: socintf.h,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        socintf.h
 * Purpose:     
 */

#ifndef   _PCID_SOCINTF_H_
#define   _PCID_SOCINTF_H_

#include "pcid.h"

extern int pcid_setup_socket(int port);
extern int pcid_wait_for_cnxn(int sockfd);
extern void pcid_close_cnxn(int sockfd);
extern int pcid_process_request(pcid_info_t *pcid_info, int sockfd,
                    struct timeval *tmout);




#endif /* _PCID_SOCINTF_H_ */

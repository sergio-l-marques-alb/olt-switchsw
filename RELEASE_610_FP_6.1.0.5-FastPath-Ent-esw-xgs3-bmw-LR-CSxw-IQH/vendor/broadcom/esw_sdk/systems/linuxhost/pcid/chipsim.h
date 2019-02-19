/*
 * $Id: chipsim.h,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        chipsim.h
 * Purpose:     
 */

#ifndef   _CHIPSIM_H_
#define   _CHIPSIM_H_



typedef struct event_s event_t;

typedef void (*event_handler_t)(event_t *ev);

struct event_s {
    sal_usecs_t		abs_time;
    event_handler_t	handler;
    event_t		*next;
    pcid_info_t         *pcid_info;
};

extern void pcid_counter_activity(event_t *ev);
extern void pcid_check_packet_input(event_t *ev);
extern void event_enqueue(pcid_info_t *pcid_info, event_handler_t handler,
                          sal_usecs_t abs_time);
extern void event_init(void);
extern event_t *event_peek(void);
extern void event_process_through(sal_usecs_t abs_time);




#endif /* _CHIPSIM_H_ */

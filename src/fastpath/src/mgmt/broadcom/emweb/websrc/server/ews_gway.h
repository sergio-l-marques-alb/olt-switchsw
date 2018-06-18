/*
 * Product: EmWeb
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb/Server internal definitions
 *
 */

#ifndef _EWS_GWAY_H_
#define _EWS_GWAY_H_

#include "ews_dist.h"
#include "ews.h"

#ifdef EW_CONFIG_OPTION_PRIMARY

typedef struct EwsSecondaryHandle_s {
    EwsLink             link;            /* list of secondaries (ews_state) */
    EwsLink             context_list;    /* queue of waiting contexts */
    EwsContext          context_current; /* current context */
    EwaSecondaryHandle  handle;          /* corresponding application handle */
    boolean             opened;          /* TRUE if opened */
    boolean             eof;             /* TRUE if secondary closed data */
    EwsContext          response_context;/* context for parsing response */
  } EwsSecondaryHandle_t, *EwsSecondaryHandleP;

/*
 * ewsDistRun
 * Process distributed context
 */
extern void ewsDistRun ( EwsContext context );

/*
 * ewsDistShutdown
 * Cleanup resources allocated for primary/secondary distribution
 */
extern void ewsDistShutdown ( void );

/*
 * ewsDistForwardAgent
 * Move secondary response data buffers from secondary context to the
 * primary context for transmission to the user agent.  The more flag
 * indicates that additional data will be forwarded for this request.
 * Specifically, if more is TRUE, we forward all buffers from the beginning
 * of the response chain up to but not including the buffer containing
 * req_str_end.  If more is FALSE, we forward all buffers on the chain.
 * Since requests and responses between the primary and secondary are never
 * pipelined, we don't have to worry about a second response sharing the
 * buffers.
 */
extern void ewsDistForwardAgent ( EwsContext context, boolean more );

/*
 * ewsDistFinish
 */
extern void ewsDistFinish ( EwsSecondaryHandle secondary, EwsStatus status);
#endif /* EW_CONFIG_OPTION_PRIMARY */

#endif /* _EWS_GWAY_H_ */

/*
 * Product: EmWeb/Telnet
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
 */

#ifndef _EWS_TEL_H_
#define _EWS_TEL_H_ 1 /* version */

#include "strlib_common_common.h"
#include "strlib_common_common.h"
#include "strlib_base_common.h"
#include "strlib_base_common.h"
#include "ews_tel.h"
#include "ews_clix.h"

#ifdef EW_CONFIG_OPTION_TELNET

/*
 * Telnet protocol constants
 */
#define TELNET_IAC   ((unsigned char) 255)
#define TELNET_WILL  ((unsigned char) 251)
#define TELNET_WONT  ((unsigned char) 252)
#define TELNET_DO    ((unsigned char) 253)
#define TELNET_DONT  ((unsigned char) 254)

#define TELNET_BREAK ((unsigned char) 243)
#define TELNET_IP    ((unsigned char) 244)
#define TELNET_AO    ((unsigned char) 245)
#define TELNET_AYT   ((unsigned char) 246)
#define TELNET_EC    ((unsigned char) 247)
#define TELNET_EL    ((unsigned char) 248)
#define TELNET_GA    ((unsigned char) 249)

#define TELNET_SE    ((unsigned char) 240)
#define TELNET_SB    ((unsigned char) 250)

/*
 * Telnet options
 */
#define TELNET_ECHO              ((unsigned char) 1)
#define TELNET_SUPRESS_GO_AHEAD  ((unsigned char) 3)

/*
 * Request NVT to enter raw mode.  We will echo, we won't send GAs,
 * so NVT should enter character-at-a-time mode without local echo.
 */
#define TELNET_RAW_MODE { TELNET_IAC      \
                          ,TELNET_WILL     \
                          ,TELNET_ECHO     \
                          ,TELNET_IAC      \
                          ,TELNET_WILL     \
                          ,TELNET_SUPRESS_GO_AHEAD  \
  }

typedef enum EwsTelnetState_e
{
  ewsTelnetStart
  ,ewsTelnetHaveCR
  ,ewsTelnetHaveIAC
  ,ewsTelnetHaveWill
  ,ewsTelnetHaveWont
  ,ewsTelnetHaveDo
  ,ewsTelnetHaveDont
  ,ewsTelnetHaveSB
  ,ewsTelnetHaveSBIAC
} EwsTelnetState;

/*
 * Per-telnet session state
 */
typedef struct EwsTelnet_s
{
  EwsTelnetState state;
  const char * trapString;
  const char * prompt;
  uintf flags;
  boolean supress_ga;
  boolean need_flush;
  boolean ssh;

  boolean logoutDeferred;

#ifdef EW_CONFIG_OPTION_CLI
  struct EwsCliState_s cli;
#endif    /* EW_CONFIG_OPTION_CLI */

} EwsTelnet, * EwsTelnetP;

extern EwsStatus
ewsTelnetSendHeader( EwsContext context
                    , EwsDocument document
                    , const char * status
                    , const char * string
);
extern void
ewsTelnetFinish( EwsContext context );

extern boolean
ewsTelnetParse ( EwsContext context );

#endif /* EW_CONFIG_OPTION_TELNET */

#endif /* _EWS_TEL_H_ */

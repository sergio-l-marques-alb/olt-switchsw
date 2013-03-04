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
 * EmWeb/Server Application Interface
 *
 */
#ifndef _EWS_API_H_
#define _EWS_API_H_

#include "ew_types.h"   /* application-defined generic types */
#include "ew_config.h"  /* application-defined configuration */

#include "ews_def.h"    /* general interface definitions */
#include "ews_sys.h"    /* system interfaces */
#include "ews_net.h"    /* network interfaces */
#include "ews_doc.h"    /* document interfaces */
#include "ews_auth.h"   /* authentication interfaces */
#include "ews_cgi.h"    /* CGI interfaces */
#include "ews_ctxt.h"   /* context access interfaces */
#include "ews_dist.h"   /* distributed EmWeb primary/secondary interfaces */

#ifdef EW_CONFIG_OPTION_SMTP
#include "ews_smtp.h"   /* SMTP mail application interfaces */
#endif

#ifdef EW_CONFIG_OPTION_TELNET
#include "ews_telnet.h"   /* TELNET application interfaces */
#endif

#ifdef EW_CONFIG_OPTION_CLI
#include "ews_cli.h"     /* CLI application interfaces */
#endif

#ifdef EW_CONFIG_OPTION_CLIENT
#include "ews_client.h"  /* EmWeb/Client */
#endif /* EW_CONFIG_OPTION_CLIENT */

#ifdef EW_CONFIG_OPTION_XMLP
#include "ews_xmlp.h"  /* EmWeb/XMLP */
#endif /* EW_CONFIG_OPTION_XMLP */

#ifdef EW_CONFIG_OPTION_UPNP
#include "ews_upnp.h"  /* EmWeb/UPnP */
#endif /* EW_CONFIG_OPTION_UPNP */

#endif /* _EWS_API_H_ */

/* 
 * $Id: qax_fabric_with_packet_tdm.h, Broadcom SDK $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifndef __QAX_FABRIC_WITH_PACKET_TDM_INCLUDED__
/* { */
#define __QAX_FABRIC_WITH_PACKET_TDM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/error.h>

/*************
 * DEFINES   *
 *************/



/*************
 * MACROS    *
 *************/


/*************
 * TYPE DEFS *
 *************/


/*************
 * GLOBALS   *
 *************/


/*************
 * FUNCTIONS *
 *************/
int qax_fabric_with_packet_tdm_loopback_enable(int unit, int quad, int enable);
int qax_fabric_with_packet_tdm_link_get(int unit, int quad, int* link);
int qax_fabric_with_packet_tdm_enable_set(int unit, int quad, int enable);
int qax_fabric_with_packet_tdm_enable_get(int unit, int quad, int *enable);
int qax_fabric_with_packet_tdm_attach(int unit, int quad);


/*
  *  Function to probe and init fabric without fabric interface module
  */
soc_error_t qax_fabric_with_packet_tdm_port_probe(int unit, int quad);


/************
 *  TAIL
 *************/
#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif


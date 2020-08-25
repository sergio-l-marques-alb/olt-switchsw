/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename mirror_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 
*
* @author 
* @end
*
**********************************************************************/

#ifndef __MIRROR_EXPORTS_H_
#define __MIRROR_EXPORTS_H_


/* Port Mirroring Component Feature List */
typedef enum
{
  L7_MIRRORING_FEATURE_SUPPORTED = 0,            /* general support statement */
  L7_MIRRORING_MULT_SRCPORTS_FEATURE_ID,
  L7_MIRRORING_CPU_INTF_SUPPORTED_FEATURE_ID,    /* CPU INTF support for probe port */
  L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID,/* supports specifying teh direction of traffic tobe mirrored per source port*/
  L7_MIRRORING_LAG_INTF_SOURCE_SUPPORTED_FEATURE_ID,    /* LAG INTF support for source port */
  L7_MIRRORING_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_MIRRORING_FEATURE_IDS_t;

/******************************************************************/
/******    Start of Port Mirroring types and defines        *******/
/******************************************************************/

typedef enum
{
  L7_MIRROR_UNCONFIGURED =0,
  L7_MIRROR_BIDIRECTIONAL = 1,
  L7_MIRROR_INGRESS,
  L7_MIRROR_EGRESS
} L7_MIRROR_DIRECTION_t;

#define L7_DEFAULT_MIRROR_SESSION_ID 1

/******************************************************************/
/******    End of Port Mirroring types and defines        *******/
/******************************************************************/



/******************** conditional Override *****************************/

#ifdef INCLUDE_MIRROR_EXPORTS_OVERRIDES
#include "mirror_exports_overrides.h"
#endif

#endif /* __MIRROR_EXPORTS_H_*/

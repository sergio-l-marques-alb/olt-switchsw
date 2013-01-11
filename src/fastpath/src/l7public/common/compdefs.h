/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename compdefs.h
*
* @purpose This file contains defines needed by the UI and layers below for management
*
* @component sysapi
*
* @comments none
*
* @create 9/12/2006
*
* @author W. Jacobs
* @end
*
**********************************************************************/
#ifndef INCLUDE_COMPDEFS
#define INCLUDE_COMPDEFS

/*--------------------------------------*/
/*  System Support (Debug) Constants    */
/*--------------------------------------*/

/*  Identification of categories allows for automatic positioning of debug hooks
    in the user interface.  

    System-related hooks are always presented first. These should be relatively few 
    in number.  These should provide information about the general health of the system.

    Feature specific hooks are always presented next.  These typically provide information
    about the overall health of the feature

    Driver hooks provide for a detailed level of debugging. 

    A miscellaneous, or "other" category is provided for placement of hooks that do not 
    clearly belong elsewhere.

    As an initial start, broad categories are defined along package lines (e.g. switching,
    routing, bgp, etc.).  This does not prohibit the definition of other categories.
    This also does not prohibit the evolution of a tiered hierarchy
    not prohibit the definition o

    */

typedef enum
{
  SUPPORT_CATEGORY_UNUSED = 0,   
  SUPPORT_CATEGORY_SYSTEM,            /* Debugging hooks generally related to the system */
  SUPPORT_CATEGORY_SWITCHING,         /* Debugging hooks applicable to the package as a whole */
  SUPPORT_CATEGORY_ROUTING,           /* Debugging hooks applicable to the package as a whole */
  SUPPORT_CATEGORY_IPMCAST,           /* Debugging hooks applicable to the package as a whole */
  SUPPORT_CATEGORY_BGP,               /* Debugging hooks applicable to the package as a whole */
  SUPPORT_CATEGORY_QOS,               /* Debugging hooks applicable to the package as a whole */
  SUPPORT_CATEGORY_WIRELESS,          /* Debugging hooks applicable the package as a whole */

  SUPPORT_CATEGORY_COMPONENT_SPECIFIC, /*Debugging hooks applicable to a specific feature*/

  SUPPORT_CATEGORY_PORT_CONNECTIVITY, /* Debugging hooks applicable to port connectivity */
  SUPPORT_CATEGORY_STACKING, /* Debugging hooks applicable to stacking transitions */

  SUPPORT_CATEGORY_LAST           /* Always last - End marker */
} SUPPORT_CATEGORIES_t;

#define SUPPORT_CATEGORIES_MAX  SUPPORT_CATEGORY_LAST


/*--------------------------------------*/
/*  dot3ad Constants                    */
/*--------------------------------------*/
typedef enum
{
  L7_DOT3AD_HASHING_MODE_FIRST = 0,
  L7_DOT3AD_HASHING_MODE_SA_VLAN,
  L7_DOT3AD_HASHING_MODE_DA_VLAN,
  L7_DOT3AD_HASHING_MODE_SDA_VLAN,
  L7_DOT3AD_HASHING_MODE_SIP_SPORT,
  L7_DOT3AD_HASHING_MODE_DIP_DPORT,
  L7_DOT3AD_HASHING_MODE_SDIP_DPORT,
  L7_DOT3AD_HASHING_MODE_ENHANCED,
  L7_DOT3AD_HASHING_MODE_LAST

} L7_DOT3AD_HASH_MODE_t;

/*--------------------------------------*/
/*  ISDP Constants                    */
/*--------------------------------------*/

#define L7_ISDP_TIMER_MIN       5
#define L7_ISDP_TIMER_MAX       254

#define L7_ISDP_HOLDTIME_MIN    10
#define L7_ISDP_HOLDTIME_MAX    255

#define L7_ISDP_DEVICE_ID_LEN   65
#define L7_ISDP_PORT_ID_LEN     64
#define L7_ISDP_VERSION_LEN     300
#define L7_ISDP_PLATFORM_LEN    64

#define L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_SERIAL_BITMASK    0x01
#define L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_MAC_BITMASK       0x02
#define L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_OTHER_BITMASK     0x04
#define L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_HOSTNAME_BITMASK  0x08

#endif

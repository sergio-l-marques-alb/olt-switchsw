/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _DNX_SW_STATE_ATTRIBUTRE_PACKED__
#define _DNX_SW_STATE_ATTRIBUTRE_PACKED__

/*************
* DEFINES   *
*************/
/** { */

/*
 * Define the various 'packed' attributes.
 */
#ifdef _MSC_VER
/* { */
/*
 * On MS-Windows platform this attribute is not defined.
 */
#ifndef __ATTRIBUTE_PACKED__
/* { */
#define __ATTRIBUTE_PACKED__
/* } */
#endif

#pragma warning(disable  : 4103)
#pragma warning(disable  : 4127)
#pragma pack(push)
#pragma pack(1)
/* } */
#elif defined(__GNUC__)
/* { */
/*
 * GNUC packing attribute.
 */
#ifndef __ATTRIBUTE_PACKED__
/* { */
#define __ATTRIBUTE_PACKED__ __attribute__ ((packed))
/* } */
#endif
/* } */
#elif defined(GHS)
/* { */
#ifndef __ATTRIBUTE_PACKED__
/* { */
#define __ATTRIBUTE_PACKED__ __attribute__ ((__packed__))
/* } */
#endif
/* } */
#else
/* { */
/*
 * Here add any other definition (custom)
 */
#define __ATTRIBUTE_PACKED__
/* } */
#endif

/*
 * }
 */

#endif /* _DNX_SW_STATE_ATTRIBUTRE_PACKED__ */

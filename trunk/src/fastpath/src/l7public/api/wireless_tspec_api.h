/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename   wireless_tspec_api.h
*
* @purpose    Genric wireless TSPEC APIs accessed outside of the wireless code.
*
* @component  Wireless
*
* @comments   none
*
* @create     01/07/2009
*
* @author     gpaussa
*
* @end
*
*********************************************************************/

#ifndef INCLUDE_WIRELESS_TSPEC_API_H
#define INCLUDE_WIRELESS_TSPEC_API_H

#include "l7_common.h"

/* GET/SET macros to access various tsInfo fields within a TSPEC
 *
 * NOTE: The macros are passed a variable name of type wsTspecTsInfo_t.
 */
#define WIRELESS_TSPEC_TID_OCTET                0
#define WIRELESS_TSPEC_TID_BITS                 0xF
#define WIRELESS_TSPEC_TID_SHIFT                1
#define WIRELESS_TSPEC_TID_MASK                 (WIRELESS_TSPEC_TID_BITS << WIRELESS_TSPEC_TID_SHIFT)

#define WIRELESS_TSPEC_DIR_OCTET                0
#define WIRELESS_TSPEC_DIR_BITS                 0x3
#define WIRELESS_TSPEC_DIR_SHIFT                5
#define WIRELESS_TSPEC_DIR_MASK                 (WIRELESS_TSPEC_DIR_BITS << WIRELESS_TSPEC_DIR_SHIFT)

#define WIRELESS_TSPEC_PSB_OCTET                1
#define WIRELESS_TSPEC_PSB_BITS                 0x1
#define WIRELESS_TSPEC_PSB_SHIFT                2
#define WIRELESS_TSPEC_PSB_MASK                 (WIRELESS_TSPEC_PSB_BITS << WIRELESS_TSPEC_PSB_SHIFT)

#define WIRELESS_TSPEC_UP_OCTET                 1
#define WIRELESS_TSPEC_UP_BITS                  0x7
#define WIRELESS_TSPEC_UP_SHIFT                 3
#define WIRELESS_TSPEC_UP_MASK                  (WIRELESS_TSPEC_UP_BITS << WIRELESS_TSPEC_UP_SHIFT)

#define WIRELESS_TSPEC_TID_GET(_tsi)            ( ((_tsi).octets[WIRELESS_TSPEC_TID_OCTET] & WIRELESS_TSPEC_TID_MASK) \
                                                    >> WIRELESS_TSPEC_TID_SHIFT )
#define WIRELESS_TSPEC_DIR_GET(_tsi)            ( ((_tsi).octets[WIRELESS_TSPEC_DIR_OCTET] & WIRELESS_TSPEC_DIR_MASK) \
                                                    >> WIRELESS_TSPEC_DIR_SHIFT )
#define WIRELESS_TSPEC_PSB_GET(_tsi)            ( ((_tsi).octets[WIRELESS_TSPEC_PSB_OCTET] & WIRELESS_TSPEC_PSB_MASK) \
                                                    >> WIRELESS_TSPEC_PSB_SHIFT )
#define WIRELESS_TSPEC_UP_GET(_tsi)             ( ((_tsi).octets[WIRELESS_TSPEC_UP_OCTET]  & WIRELESS_TSPEC_UP_MASK)  \
                                                    >> WIRELESS_TSPEC_UP_SHIFT  )

#define WIRELESS_TSPEC_TID_SET(_tsi, _val)      ( (_tsi).octets[WIRELESS_TSPEC_TID_OCTET] = \
                                                    (((_tsi).octets[WIRELESS_TSPEC_TID_OCTET] & ~WIRELESS_TSPEC_TID_MASK) | \
                                                     (((_val) & WIRELESS_TSPEC_TID_BITS) << WIRELESS_TSPEC_TID_SHIFT)) )
#define WIRELESS_TSPEC_DIR_SET(_tsi, _val)      ( (_tsi).octets[WIRELESS_TSPEC_DIR_OCTET] = \
                                                    (((_tsi).octets[WIRELESS_TSPEC_DIR_OCTET] & ~WIRELESS_TSPEC_DIR_MASK) | \
                                                     (((_val) & WIRELESS_TSPEC_DIR_BITS) << WIRELESS_TSPEC_DIR_SHIFT)) )
#define WIRELESS_TSPEC_PSB_SET(_tsi, _val)      ( (_tsi).octets[WIRELESS_TSPEC_PSB_OCTET] = \
                                                    (((_tsi).octets[WIRELESS_TSPEC_PSB_OCTET] & ~WIRELESS_TSPEC_PSB_MASK) | \
                                                     (((_val) & WIRELESS_TSPEC_PSB_BITS) << WIRELESS_TSPEC_PSB_SHIFT)) )
#define WIRELESS_TSPEC_UP_SET(_tsi, _val)       ( (_tsi).octets[WIRELESS_TSPEC_UP_OCTET] = \
                                                    (((_tsi).octets[WIRELESS_TSPEC_UP_OCTET]  & ~WIRELESS_TSPEC_UP_MASK)  | \
                                                     (((_val) & WIRELESS_TSPEC_UP_BITS)  << WIRELESS_TSPEC_UP_SHIFT))  )


#define WIRELESS_TSPEC_TS_INFO_SIZE             3

typedef struct wirelessTspecTsInfo_s
{
  L7_uchar8                   octets[WIRELESS_TSPEC_TS_INFO_SIZE];
} wirelessTspecTsInfo_t;

/*******************************************************
*
* TSPEC record contents
*
* NOTE: Only certain fields are supported by WMM 1.1, as
*       indicated by the comments.
*
********************************************************/
typedef struct wirelessTspecRecord_s
{
  L7_uchar8                   rsvd1[1];                     /* (reserved- for alignment) */
  wirelessTspecTsInfo_t       tsInfo;                       /* WMM 1.1 */
  L7_ushort16                 nomMsduSize;                  /* WMM 1.1 */
  L7_ushort16                 maxMsduSize;
  L7_uint32                   minServiceIval;
  L7_uint32                   maxServiceIval;
  L7_uint32                   inactivityIval;
  L7_uint32                   suspensionIval;
  L7_uint32                   serviceStartTime;
  L7_uint32                   minDataRate;
  L7_uint32                   meanDataRate;                 /* WMM 1.1 */
  L7_uint32                   peakDataRate;
  L7_uint32                   burstSize;
  L7_uint32                   delayBound;
  L7_uint32                   minPhyRate;                   /* WMM 1.1 */
  L7_ushort16                 surplusBwAllowance;           /* WMM 1.1 */
  L7_ushort16                 mediumTime;                   /* WMM 1.1 */
} wirelessTspecRecord_t;


/*********************************************************************
* @purpose  Verifies whether a TSPEC access category index is supported or not.
*
* @param    L7_uint32    acindex        @b{(input)}  Access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Per the list above, only the SUPPORTED access categories are 
*           considered here, meaning that the access category can be 
*           configured for mandatory admission control.
*
* @end
*********************************************************************/
L7_RC_t wirelessTspecSupportedACIndexGet(L7_uint32 acindex);

/*********************************************************************
* @purpose  Gets the first supported TSPEC access category index.
*
* @param    L7_uint32    *acindex       @b{(input)}  First access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Per the list above, only the SUPPORTED access categories are 
*           considered here, meaning that the access category can be 
*           configured for mandatory admission control.
*
* @comments In this case, the first supported access category is voice,
*           which can be used as input to a subsequent 'NextGet' API call.
*
* @end
*********************************************************************/
L7_RC_t wirelessTspecSupportedACIndexFirstGet(L7_uint32 *acindex);

/*********************************************************************
* @purpose  Gets the last supported TSPEC access category index.
*
* @param    L7_uint32    *acindex       @b{(input)}  Last access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Per the list above, only the SUPPORTED access categories are 
*           considered here, meaning that the access category can be 
*           configured for mandatory admission control.
*
* @comments In this case, the last supported access category is video,
*           which can be used as input to a subsequent 'NextGet' API call.
*
* @end
*********************************************************************/
L7_RC_t wirelessTspecSupportedACIndexLastGet(L7_uint32 *acindex);

/*********************************************************************
* @purpose  Gets the next supported TSPEC access category index.
*
* @param    L7_uint32    acindex        @b{(input)}  Access category index
* @param    L7_uint32    *nextAcindex   @b{(output)} Next access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments For TSPEC access categories, the "next" value is perceived
*           as the next supported value from the following order:
*           voice, video, best-effort, background.  These are iterated
*           in decreasing numerical order based on their corresponding
*           index value.
*
* @comments Per the list above, only the SUPPORTED access categories are 
*           considered here, meaning that the access category can be 
*           configured for mandatory admission control.
*
* @end
*********************************************************************/
L7_RC_t wirelessTspecSupportedACIndexNextGet(L7_uint32 acindex,
                                             L7_uint32 *nextAcindex);

#endif /* INCLUDE_WIRELESS_TSPEC_API_H */

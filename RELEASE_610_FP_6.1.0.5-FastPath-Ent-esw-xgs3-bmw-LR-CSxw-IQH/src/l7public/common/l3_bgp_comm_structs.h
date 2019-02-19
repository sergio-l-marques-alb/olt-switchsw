/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l3_bgp_comm_strcusts.h
*
* @purpose The purpose of this file is to have a central location for
*          layer 3 common structures to be used by the entire system.
*
* @component sysapi
*
* @comments none
*
* @create 2/18/2002
*
* @author
* @end
*
**********************************************************************/

/**************************@null{*************************************
                     FASTPATH Release 3.0.0
 *******************************}*************************************}
**********************************************************************/

#ifndef INCLUDE_L3_BGP_COMM_STRUCTS
#define INCLUDE_L3_BGP_COMM_STRUCTS

#include "datatypes.h"



/*-------------------
   BGP4  STRUCTURES
 -------------------*/

typedef struct bgpAddrFamily_t
{
      L7_ushort16  Afi;
      L7_uchar8  Safi;
} bgpAddrFamily_t;

typedef unsigned char bgpIpAddr[4];     /* IP Address */

typedef struct bgpSnpa_t
{
   L7_ushort16  SnpaLen;
   bgpIpAddr    SnpaAdr;
}  bgpSnpa_t;


/* BGP range and policy types */

#define L7_BGP4_POLICY_RANGE_STRLEN      64

typedef struct bgpPolicyRange_s
{
    L7_uint32   rangeType;
    L7_uint32   val1;
    L7_uint32   val2;
    L7_char8    valStr[L7_BGP4_POLICY_RANGE_STRLEN];

}bgpPolicyRange_t;

typedef struct bgpPolicyAction_s
{
    L7_uint32   matchType;
    L7_uint32   modifier;
    L7_uint32   val1;
    L7_uint32   val2;

}bgpPolicyAction_t;


/*    BGP-4 Received Path Attribute Table.  This table contains, */
/*    one entry per path to a network, path attributes           */
/*    received from all peers running BGP-4.                     */

typedef struct bgpMapPathAttribute_s
{
   /* ---------------   RFC 1771 standard parameters -------------   */

/*      bgp4PathAttrPeer                                       */
/*          SYNTAX     IpAddress                               */
/*          ACCESS     read-only                               */
/*          DESCRIPTION                                        */
/*                  "The IP address of the peer where the path */
/*                  information was learned."                  */
   bgpIpAddr Peer;

/*      bgp4PathAttrOrigin                                          */
/*          SYNTAX     INTEGER {                                    */
/*                               igp(1),-- networks are interior    */
/*                               egp(2),-- networks learned via EGP */
/*                               incomplete(3) -- undetermined      */
/*                             }                                    */
/*          ACCESS     read-write                                   */
/*          DESCRIPTION                                             */
/*                  "The ultimate origin of the path information."  */
   L7_ushort16     Origin;

/*      bgp4PathAttrASPathSegment                                  */
/*          SYNTAX     OCTET STRING (SIZE (2..255))                */
/*          ACCESS     read-only                                   */
/*          DESCRIPTION                                            */
/*                  "The sequence of AS path segments.  Each AS    */
/*                  path segment is represented by a triple        */
/*                  <type, length, value>.                         */
/*                                                                 */
/*                  The type is a 1-octet field which has two      */
/*                  possible values:                               */
/*                       1      AS_SET: unordered set of ASs a     */
/*                                   route in the UPDATE message   */
/*                                   has traversed                 */
/*                       2      AS_SEQUENCE: ordered set of ASs    */
/*                                   a route in the UPDATE message */
/*                                   has traversed.                */
/*                                                                 */
/*                  The length is a 1-octet field containing the   */
/*                   number of ASs in the value field.              */
/*                                                                 */
/*                  The value field contains one or more AS        */
/*                  numbers, each AS is represented in the octet   */
/*                  string as a pair of octets according to the    */
/*                  following algorithm:                           */
/*                                                                 */
/*                      first-byte-of-pair = ASNumber / 256;       */
/*                      second-byte-of-pair = ASNumber & 255;"     */
   L7_ushort16     ASPathSegmentLen;
   L7_uchar8     *p_ASPathSegment;

/*      bgp4PathAttrNextHop                                       */
/*          SYNTAX     IpAddress                                  */
/*          ACCESS     read-only                                  */
/*          DESCRIPTION                                           */
/*                  "The address of the border router that should */
/*                  be used for the destination network."         */
   bgpIpAddr   NextHop;

/*      bgp4PathAttrMultiExitDisc                                   */
/*          SYNTAX     INTEGER (-1..2147483647)                     */
/*          ACCESS     read-write                                   */
/*          DESCRIPTION                                             */
/*                  "This metric is used to discriminate between    */
/*                  multiple exit points to an adjacent autonomous  */
/*                  system.  A value of -1 indicates the absence of */
/*                  this attribute."                                */
   L7_long32     MultiExitDisc;

/*      bgp4PathAttrLocalPref                                       */
/*          SYNTAX     INTEGER (-1..2147483647)                     */
/*          ACCESS     read-write                                   */
/*          DESCRIPTION                                             */
/*                  "The originating BGP4 speaker's degree of       */
/*                  preference for an advertised route.  A value of */
/*                  -1 indicates the absence of this attribute."    */
   L7_long32     LocalPref;

/*      bgp4PathAttrAtomicAggregate                           */
/*          SYNTAX     INTEGER {                              */
/*                         lessSpecificRrouteNotSelected(1),  */
/*                         lessSpecificRouteSelected(2)       */
/*                     }                                      */
/*          ACCESS     read-write                             */
/*          DESCRIPTION                                       */
/*                  "Whether or not a system has selected     */
/*                  a less specific route without selecting a */
/*                  more specific route."                     */
   L7_ushort16     AtomicAggregate;

/*      bgp4PathAttrAggregatorAS                                      */
/*          SYNTAX     INTEGER (0..65535)                             */
/*          ACCESS     read-only                                      */
/*          DESCRIPTION                                               */
/*                  "The AS number of the last BGP4 speaker that      */
/*                  performed route aggregation.  A value of zero (0) */
/*                  indicates the absence of this attribute."         */
   L7_ushort16     AggregatorAS;

/*      bgp4PathAttrAggregatorAddr                                     */
/*          SYNTAX     IpAddress                                       */
/*          ACCESS     read-only                                       */
/*          DESCRIPTION                                                */
/*                  "The IP address of the last BGP4 speaker that      */
/*                   performed route aggregation.  A value of          */
/*                   0.0.0.0 indicates the absence of this attribute." */
   bgpIpAddr  AggregatorAddr;

/*      bgp4PathAttrUnknown                                         */
/*          SYNTAX     OCTET STRING (SIZE(0..255))                  */
/*          ACCESS     read-only                                    */
/*          DESCRIPTION                                             */
/*                  "One or more path attributes not understood     */
/*                   by this BGP4 speaker.  Size zero (0) indicates */
/*                   the absence of such attribute(s).  Octets      */
/*                   beyond the maximum size, if any, are not       */
/*                   recorded by this object."                      */
   L7_ushort16     UnknownSize;
   L7_uchar8     *p_Unknown;

/*      bgp4PathAttrCalcLocalPref                                   */
/*          SYNTAX     INTEGER (-1..2147483647)                     */
/*          ACCESS     read-only                                    */
/*          DESCRIPTION                                             */
/*                  "The degree of preference calculated by the     */
/*                  receiving BGP4 speaker for an advertised route. */
/*                  A value of -1 indicates the absence of this     */
/*                  attribute."                                     */
   L7_long32     CalcLocalPref;

/*      bgp4PathAttrBest                                        */
/*          SYNTAX     INTEGER {                                */
/*                         false(1),-- not chosen as best route */
/*                         true(2) -- chosen as best route      */
/*                     }                                        */
/*          ACCESS     read-only                                */
/*          DESCRIPTION                                         */
/*                  "An indication of whether or not this route */
/*                  was chosen as the best BGP4 route."         */
   L7_ushort16     Best;

/* ------------------  BGP Route Reflection --------------------    */
   bgpIpAddr OriginatorId;
   L7_uchar8     ClusterNum;
   bgpIpAddr *p_ClusterList;

/* ------------------  BGP Communities -------------------------    */
   L7_uchar8     CommunityNum;
   L7_ulong32     *p_Communities;

   /* ----------- additional protocol parameters ----------------- */

   /* Multiprotocol extension parameters */

   /* Address Family Identifier (read-write) */
   L7_ushort16  Afi;
   /* Subsequent Address Family Identifier (read-write) */
   L7_uchar8  Safi;
   /* Length of Next Hop Network Address (read-write) */
   L7_uchar8  NextHopAdrLen;
   /* Number of SNPAs (read-write) */
   L7_ushort16  SnpaNum;
   /* list of SNPAs info (read-write) */
   bgpSnpa_t **p_Snpa;

}  bgpMapPathAttribute_t;

#endif

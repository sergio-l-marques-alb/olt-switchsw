/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_h323_message_types.h
*
* @purpose H323 Protocol meessage type
*
* @component VOIP | H323
*
* @comments
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#ifndef VOIP_H323_MESSAGE_TYPE_H
#define VOIP_H323_MESSAGE_TYPE_H
#include <stddef.h>
#include "voip_h323.h"

static  asnField_t transportAddrIpAddr[] =
{        /* SEQUENCE */
        {FNAME("ip") ASN_OCTSTR, ASN_FIXD, 4, 0, ASN_DECODE,
         offsetof(transportAddrIpAddr_t, ip),L7_NULLPTR},
        {FNAME("port") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t transportAddrIpSourceRoute_route[] = 
{      /* SEQUENCE OF */
        {FNAME("item") ASN_OCTSTR, ASN_FIXD, 4, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t transportAddrIpSourceRoute_routing[] = 
{       /* CHOICE */
        {FNAME("strict") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0,L7_NULLPTR},
        {FNAME("loose") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0,L7_NULLPTR},
};

static asnField_t transportAddrIpSourceRoute[] = 
{      /* SEQUENCE */
        {FNAME("ip") ASN_OCTSTR, ASN_FIXD, 4, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("port") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("route") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0,
         transportAddrIpSourceRoute_route},
        {FNAME("routing") ASN_CHOICE, 1, 2, 2, ASN_SKIP | ASN_EXT, 0,
         transportAddrIpSourceRoute_routing},
};

static asnField_t transportAddrIpxAddr[] = 
{       /* SEQUENCE */
        {FNAME("node") ASN_OCTSTR, ASN_FIXD, 6, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("netnum") ASN_OCTSTR, ASN_FIXD, 4, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("port") ASN_OCTSTR, ASN_FIXD, 2, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t transportAddrIp6Addr[] = 
{       /* SEQUENCE */
        {FNAME("ip") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("port") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H221NonStandard[] = 
{   /* SEQUENCE */
        {FNAME("t35CountryCode") ASN_INT, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("t35Extension") ASN_INT, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("manufacturerCode") ASN_INT, ASN_WORD, 0, 0,ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t nonStandardIdentifier[] = 
{     /* CHOICE */
        {FNAME("object") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("h221NonStandard") ASN_SEQ, 0, 3, 3, ASN_SKIP | ASN_EXT, 0,
         H221NonStandard},
};

static asnField_t nonStandardParameter[] = 
{      /* SEQUENCE */
        {FNAME("nonStandardIdentifier") ASN_CHOICE, 1, 2, 2, ASN_SKIP | ASN_EXT, 0,
         nonStandardIdentifier},
        {FNAME("data") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t transportAddr[] = 
{  /* CHOICE */
        {FNAME("ipAddress") ASN_SEQ, 0, 2, 2, ASN_DECODE,
         offsetof(transportAddr_t, ipAddr), transportAddrIpAddr},
        {FNAME("ipSourceRoute") ASN_SEQ, 0, 4, 4, ASN_SKIP | ASN_EXT, 0,
         transportAddrIpSourceRoute},
        {FNAME("ipxAddress") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         transportAddrIpxAddr},
        {FNAME("ip6Address") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0,
         transportAddrIp6Addr},
        {FNAME("netBios") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("nsap") ASN_OCTSTR, 5, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("nonStandardAddress") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         nonStandardParameter},
};

static asnField_t aliasAddr[] = 
{      /* CHOICE */
        {FNAME("dialedDigits") ASN_NUMDGT, 7, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("h323-ID") ASN_BMPSTR, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("url-ID") ASN_IA5STR, ASN_WORD, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("transportID") ASN_CHOICE, 3, 7, 7, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("email-ID") ASN_IA5STR, ASN_WORD, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("partyNumber") ASN_CHOICE, 3, 5, 5, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("mobileUIM") ASN_CHOICE, 1, 2, 2, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t setupUUIESourceAddr[] = 
{  /* SEQUENCE OF */
        {FNAME("item") ASN_CHOICE, 1, 2, 7, ASN_SKIP | ASN_EXT, 0, aliasAddr},
};

static asnField_t vendorIdentifier[] = 
{  /* SEQUENCE */
        {FNAME("vendor") ASN_SEQ, 0, 3, 3, ASN_SKIP | ASN_EXT, 0, H221NonStandard},
        {FNAME("productId") ASN_OCTSTR, ASN_BYTE, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("versionId") ASN_OCTSTR, ASN_BYTE, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t gatekeeperInfo[] = 
{    /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
};

static asnField_t H310Caps[] = 
{
  /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("dataRatesSupported") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedPrefixes") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H320Caps[] = 
{  /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("dataRatesSupported") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedPrefixes") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H321Caps[] = 
{  /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("dataRatesSupported") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedPrefixes") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H322Caps[] = 
{  /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("dataRatesSupported") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedPrefixes") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H323Caps[] = 
{  /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("dataRatesSupported") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedPrefixes") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H324Caps[] = 
{  /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("dataRatesSupported") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedPrefixes") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t voiceCaps[] = 
{ /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("dataRatesSupported") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedPrefixes") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t T120OnlyCaps[] = 
{      /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("dataRatesSupported") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedPrefixes") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t supportedProtocols[] = 
{        /* CHOICE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         nonStandardParameter},
        {FNAME("h310") ASN_SEQ, 1, 1, 3, ASN_SKIP | ASN_EXT, 0, H310Caps},
        {FNAME("h320") ASN_SEQ, 1, 1, 3, ASN_SKIP | ASN_EXT, 0, H320Caps},
        {FNAME("h321") ASN_SEQ, 1, 1, 3, ASN_SKIP | ASN_EXT, 0, H321Caps},
        {FNAME("h322") ASN_SEQ, 1, 1, 3, ASN_SKIP | ASN_EXT, 0, H322Caps},
        {FNAME("h323") ASN_SEQ, 1, 1, 3, ASN_SKIP | ASN_EXT, 0, H323Caps},
        {FNAME("h324") ASN_SEQ, 1, 1, 3, ASN_SKIP | ASN_EXT, 0, H324Caps},
        {FNAME("voice") ASN_SEQ, 1, 1, 3, ASN_SKIP | ASN_EXT, 0,voiceCaps},
        {FNAME("t120-only") ASN_SEQ, 1, 1, 3, ASN_SKIP | ASN_EXT, 0, T120OnlyCaps},
        {FNAME("nonStandardProtocol") ASN_SEQ, 2, 3, 3, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("t38FaxAnnexbOnly") ASN_SEQ, 2, 5, 5, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t gatewayInfoProtocol[] = 
{      /* SEQUENCE OF */
        {FNAME("item") ASN_CHOICE, 4, 9, 11, ASN_SKIP | ASN_EXT, 0, supportedProtocols},
};

static asnField_t gatewayInfo[] = {       /* SEQUENCE */
        {FNAME("protocol") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         gatewayInfoProtocol},
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
};

static asnField_t mcuInfo[] = 
{   /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("protocol") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t terminalInfo[] = 
{      /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
};

static asnField_t endpointType[] = 
{      /* SEQUENCE */
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("vendor") ASN_SEQ, 2, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         vendorIdentifier},
        {FNAME("gatekeeper") ASN_SEQ, 1, 1, 1, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         gatekeeperInfo},
        {FNAME("gateway") ASN_SEQ, 2, 2, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0, gatewayInfo},
        {FNAME("mcu") ASN_SEQ, 1, 1, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0, mcuInfo},
        {FNAME("terminal") ASN_SEQ, 1, 1, 1, ASN_SKIP | ASN_EXT | ASN_OPT, 0, terminalInfo},
        {FNAME("mc") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("undefinedNode") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("set") ASN_BITSTR, ASN_FIXD, 32, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedTunnelledProtocols") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT,
         0, L7_NULLPTR},
};

static asnField_t setupUUIEDestinationAddr[] = 
{     /* SEQUENCE OF */
        {FNAME("item") ASN_CHOICE, 1, 2, 7, ASN_SKIP | ASN_EXT, 0, aliasAddr},
};

static asnField_t setupUUIEdestExtraCallInfo[] = 
{      /* SEQUENCE OF */
        {FNAME("item") ASN_CHOICE, 1, 2, 7, ASN_SKIP | ASN_EXT, 0, aliasAddr},
};

static asnField_t setupUUIEDestExtraCRV[] = 
{   /* SEQUENCE OF */
        {FNAME("item")ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t setupUUIEConferenceGoal[] = 
{ /* CHOICE */
        {FNAME("create") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("join") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("invite") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("capability-negotiation") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("callIndependentSupplementaryService") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP,
         0, L7_NULLPTR},
};

static asnField_t Q954Details[] = 
{       /* SEQUENCE */
        {FNAME("conferenceCalling") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("threePartyService") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t QseriesOptions[] = 
{    /* SEQUENCE */
        {FNAME("q932Full") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("q951Full") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("q952Full") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("q953Full") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("q955Full") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("q956Full") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("q957Full") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("q954Info") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0, Q954Details},
};

static asnField_t callType[] = 
{  /* CHOICE */
        {FNAME("pointToPoint") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("oneToN") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("nToOne") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("nToN") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H245NonStdIdentifier_h221NonStd[] = 
{        /* SEQUENCE */
        {FNAME("t35CountryCode") ASN_INT, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("t35Extension") ASN_INT, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("manufacturerCode") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H245NonStdParameter[] = 
{ /* SEQUENCE */
        {FNAME("nonStandardIdentifier") ASN_CHOICE, 1, 2, 2, ASN_SKIP, 0,
         H245NonStdIdentifier_h221NonStd},
        {FNAME("data") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H261VideoCapability[] = 
{       /* SEQUENCE */
        {FNAME("qcifMPI") ASN_INT, 2, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cifMPI") ASN_INT, 2, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("temporalSpatialTradeOffCapability") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0,
         L7_NULLPTR},
        {FNAME("maxBitRate") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("stillImageTransmission") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("videoBadMBsCap") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H262VideoCapability[] = 
{       /* SEQUENCE */
        {FNAME("profileAndLevel-SPatML") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("profileAndLevel-MPatLL") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("profileAndLevel-MPatML") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("profileAndLevel-MPatH-14") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("profileAndLevel-MPatHL") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("profileAndLevel-SNRatLL") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("profileAndLevel-SNRatML") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("profileAndLevel-SpatialatH-14") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0,
         L7_NULLPTR},
        {FNAME("profileAndLevel-HPatML") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("profileAndLevel-HPatH-14") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("profileAndLevel-HPatHL") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("videoBitRate") ASN_INT, ASN_CONS, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("vbvBufferSize") ASN_INT, ASN_CONS, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("samplesPerLine") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("linesPerFrame") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("framesPerSecond") ASN_INT, 4, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("luminanceSampleRate") ASN_INT, ASN_CONS, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("videoBadMBsCap") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};
static asnField_t H263VideoCapability[] = 
{       /* SEQUENCE */
        {FNAME("sqcifMPI") ASN_INT, 5, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("qcifMPI") ASN_INT, 5, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cifMPI") ASN_INT, 5, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cif4MPI") ASN_INT, 5, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cif16MPI") ASN_INT, 5, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("maxBitRate") ASN_INT, ASN_CONS, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("unrestrictedVector") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("arithmeticCoding") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("advancedPrediction") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("pbFrames") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("temporalSpatialTradeOffCapability") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0,
         L7_NULLPTR},
        {FNAME("hrd-B") ASN_INT, ASN_CONS, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("bppMaxKb") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("slowSqcifMPI") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("slowQcifMPI") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("slowCifMPI") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("slowCif4MPI") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("slowCif16MPI") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("errorCompensation") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("enhancementLayerInfo") ASN_SEQ, 3, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("h263Options") ASN_SEQ, 5, 29, 31, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t IS11172VideoCapability[] = 
{    /* SEQUENCE */
        {FNAME("constrainedBitstream") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("videoBitRate") ASN_INT, ASN_CONS, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("vbvBufferSize") ASN_INT, ASN_CONS, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("samplesPerLine") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("linesPerFrame") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("pictureRate") ASN_INT, 4, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("luminanceSampleRate") ASN_INT, ASN_CONS, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("videoBadMBsCap") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t videoCapability[] = 
{   /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         H245NonStdParameter},
        {FNAME("h261VideoCapability") ASN_SEQ, 2, 5, 6, ASN_SKIP | ASN_EXT, 0,
         H261VideoCapability},
        {FNAME("h262VideoCapability") ASN_SEQ, 6, 17, 18, ASN_SKIP | ASN_EXT, 0,
         H262VideoCapability},
        {FNAME("h263VideoCapability") ASN_SEQ, 7, 13, 21, ASN_SKIP | ASN_EXT, 0,
         H263VideoCapability},
        {FNAME("is11172VideoCapability") ASN_SEQ, 6, 7, 8, ASN_SKIP | ASN_EXT, 0,
         IS11172VideoCapability},
        {FNAME("genericVideoCapability") ASN_SEQ, 5, 6, 6, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t audioCapability_g7231[] = 
{     /* SEQUENCE */
        {FNAME("maxAl-sduAudioFrames") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("silenceSuppression") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t IS11172AudioCapability[] = 
{    /* SEQUENCE */
        {FNAME("audioLayer1") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioLayer2") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioLayer3") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioSampling32k") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioSampling44k1") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioSampling48k") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("singleChannel") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("twoChannels") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("bitRate") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t IS13818AudioCapability[] = 
{    /* SEQUENCE */
        {FNAME("audioLayer1") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioLayer2") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioLayer3") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioSampling16k") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioSampling22k05") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioSampling24k") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioSampling32k") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioSampling44k1") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("audioSampling48k") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("singleChannel") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("twoChannels") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("threeChannels2-1") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("threeChannels3-0") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("fourChannels2-0-2-0") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("fourChannels2-2") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("fourChannels3-1") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("fiveChannels3-0-2-0") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("fiveChannels3-2") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("lowFrequencyEnhancement") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("multilingual") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("bitRate") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t audioCapability[] = 
{   /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         H245NonStdParameter},
        {FNAME("g711Alaw64k") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g711Alaw56k") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g711Ulaw64k") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g711Ulaw56k") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g722-64k") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g722-56k") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g722-48k") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g7231") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, audioCapability_g7231},
        {FNAME("g728") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g729") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g729AnnexA") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("is11172AudioCapability") ASN_SEQ, 0, 9, 9, ASN_SKIP | ASN_EXT, 0,
         IS11172AudioCapability},
        {FNAME("is13818AudioCapability") ASN_SEQ, 0, 21, 21, ASN_SKIP | ASN_EXT, 0,
         IS13818AudioCapability},
        {FNAME("g729wAnnexB") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g729AnnexAwAnnexB") ASN_INT, ASN_BYTE, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g7231AnnexCCapability") ASN_SEQ, 1, 3, 3, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("gsmFullRate") ASN_SEQ, 0, 3, 3, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("gsmHalfRate") ASN_SEQ, 0, 3, 3, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("gsmEnhancedFullRate") ASN_SEQ, 0, 3, 3, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("genericAudioCapability") ASN_SEQ, 5, 6, 6, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("g729Extensions") ASN_SEQ, 1, 8, 8, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t dataProtocolCapability[] = {    /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         H245NonStdParameter},
        {FNAME("v14buffered") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("v42lapm") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("hdlcFrameTunnelling") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("h310SeparateVCStack") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("h310SingleVCStack") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("transparent") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("segmentationAndReassembly") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("hdlcFrameTunnelingwSAR") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("v120") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("separateLANStack") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("v76wCompression") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("tcp") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("udp") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t T84Profile_t84Restricted[] = 
{  /* SEQUENCE */
        {FNAME("qcif") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("cif") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("ccir601Seq") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("ccir601Prog") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("hdtvSeq") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("hdtvProg") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g3FacsMH200x100") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g3FacsMH200x200") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g4FacsMMR200x100") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("g4FacsMMR200x200") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("jbig200x200Seq") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("jbig200x200Prog") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("jbig300x300Seq") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("jbig300x300Prog") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("digPhotoLow") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("digPhotoMedSeq") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("digPhotoMedProg") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("digPhotoHighSeq") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("digPhotoHighProg") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t T84Profile[] = 
{        /* CHOICE */
        {FNAME("t84Unrestricted") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("t84Restricted") ASN_SEQ, 0, 19, 19, ASN_SKIP | ASN_EXT, 0,
         T84Profile_t84Restricted},
};

static asnField_t dataApplnCapability_t84[] = 
{ /* SEQUENCE */
        {FNAME("t84Protocol") ASN_CHOICE, 3, 7, 14, ASN_SKIP | ASN_EXT, 0,
         dataProtocolCapability},
        {FNAME("t84Profile") ASN_CHOICE, 1, 2, 2, ASN_SKIP, 0, T84Profile},
};

static asnField_t dataApplnCapability_nlpid[] = 
{       /* SEQUENCE */
        {FNAME("nlpidProtocol") ASN_CHOICE, 3, 7, 14, ASN_SKIP | ASN_EXT, 0,
         dataProtocolCapability},
        {FNAME("nlpidData") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t dataApplnCapabilityAppln[] = 
{     /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         H245NonStdParameter},
        {FNAME("t120") ASN_CHOICE, 3, 7, 14, ASN_DECODE | ASN_EXT,
         offsetof(dataApplnCapabilityAppln_t, t120),
         dataProtocolCapability},
        {FNAME("dsm-cc") ASN_CHOICE, 3, 7, 14, ASN_SKIP | ASN_EXT, 0,
         dataProtocolCapability},
        {FNAME("userData") ASN_CHOICE, 3, 7, 14, ASN_SKIP | ASN_EXT, 0,
         dataProtocolCapability},
        {FNAME("t84") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         dataApplnCapability_t84},
        {FNAME("t434") ASN_CHOICE, 3, 7, 14, ASN_SKIP | ASN_EXT, 0,
         dataProtocolCapability},
        {FNAME("h224") ASN_CHOICE, 3, 7, 14, ASN_SKIP | ASN_EXT, 0,
         dataProtocolCapability},
        {FNAME("nlpid") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         dataApplnCapability_nlpid},
        {FNAME("dsvdControl") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("h222DataPartitioning") ASN_CHOICE, 3, 7, 14, ASN_SKIP | ASN_EXT, 0,
         dataProtocolCapability},
        {FNAME("t30fax") ASN_CHOICE, 3, 7, 14, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("t140") ASN_CHOICE, 3, 7, 14, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("t38fax") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("genericDataCapability") ASN_SEQ, 5, 6, 6, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t dataApplnCapability[] = 
{ /* SEQUENCE */
        {FNAME("application") ASN_CHOICE, 4, 10, 14, ASN_DECODE | ASN_EXT,
         offsetof(dataApplnCapability_t, application),
         dataApplnCapabilityAppln},
        {FNAME("maxBitRate") ASN_INT, ASN_CONS, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t encryptionMode[] = 
{    /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         H245NonStdParameter},
        {FNAME("h233Encryption") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t dataType[] = 
{  /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         H245NonStdParameter},
        {FNAME("nullData") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("videoData") ASN_CHOICE, 3, 5, 6, ASN_SKIP | ASN_EXT, 0, videoCapability},
        {FNAME("audioData") ASN_CHOICE, 4, 14, 22, ASN_SKIP | ASN_EXT, 0,
         audioCapability},
        {FNAME("data") ASN_SEQ, 0, 2, 2, ASN_DECODE | ASN_EXT, offsetof(dataType_t, data),
         dataApplnCapability},
        {FNAME("encryptionData") ASN_CHOICE, 1, 2, 2, ASN_SKIP | ASN_EXT, 0,
         encryptionMode},
        {FNAME("h235Control") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("h235Media") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("multiplexedStream") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t H222LogParameters[] = 
{      /* SEQUENCE */
        {FNAME("resourceID") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("subChannelID") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("pcr-pid") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("programDescriptors") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("streamDescriptors") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};
/* ALT adaption layer type */
static asnField_t H223LogParametersALTAl3[] = 
{      /* SEQUENCE */
        {FNAME("controlFieldOctets") ASN_INT, 2, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("sendBufferSize") ASN_INT, ASN_CONS, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H223LogParametersALT[] = 
{  /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         H245NonStdParameter},
        {FNAME("al1Framed") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("al1NotFramed") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("al2WithoutSequenceNumbers") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("al2WithSequenceNumbers") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("al3") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         H223LogParametersALTAl3},
        {FNAME("al1M") ASN_SEQ, 0, 7, 8, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("al2M") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("al3M") ASN_SEQ, 0, 5, 6, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t H223LogParameters[] = 
{      /* SEQUENCE */
        {FNAME("adaptationLayerType") ASN_CHOICE, 3, 6, 9, ASN_SKIP | ASN_EXT, 0,
         H223LogParametersALT},
        {FNAME("segmentableFlag") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t CRCLength[] = 
{ /* CHOICE */
        {FNAME("crc8bit") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("crc16bit") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("crc32bit") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t V76HDLCParameters[] = 
{ /* SEQUENCE */
        {FNAME("crcLength") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT, 0, CRCLength},
        {FNAME("n401") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("loopbackTestProcedure") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t V76LogParameters_suspendResume[] = 
{ /* CHOICE */
        {FNAME("noSuspendResume") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("suspendResumewAddress") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("suspendResumewoAddress") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t V76LogParameters_modeERMRecovery[] = 
{     /* CHOICE */
        {FNAME("rej") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("sREJ") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("mSREJ") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t V76LogParameters_modeERM[] = 
{      /* SEQUENCE */
        {FNAME("windowSize") ASN_INT, 7, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("recovery") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT, 0,
         V76LogParameters_modeERMRecovery},
};

static asnField_t V76LogParametersMode[] = 
{  /* CHOICE */
        {FNAME("eRM") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0,
         V76LogParameters_modeERM},
        {FNAME("uNERM") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t V75Parameters[] = 
{     /* SEQUENCE */
        {FNAME("audioHeaderPresent") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t V76LogParameters[] = 
{       /* SEQUENCE */
        {FNAME("hdlcParameters") ASN_SEQ, 0, 3, 3, ASN_SKIP | ASN_EXT, 0,
         V76HDLCParameters},
        {FNAME("suspendResume") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT, 0,
         V76LogParameters_suspendResume},
        {FNAME("uIH") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("mode") ASN_CHOICE, 1, 2, 2, ASN_SKIP | ASN_EXT, 0,
         V76LogParametersMode},
        {FNAME("v75Parameters") ASN_SEQ, 0, 1, 1, ASN_SKIP | ASN_EXT, 0, V75Parameters},
};

static asnField_t H2250LogParametersNonStd[] = 
{ /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, H245NonStdParameter},
};

static asnField_t unicastAddrIpAddr[] = 
{  /* SEQUENCE */
        {FNAME("network") ASN_OCTSTR, ASN_FIXD, 4, 0, ASN_DECODE,
         offsetof(unicastAddrIPAddr_t, network), L7_NULLPTR},
        {FNAME("tsapIdentifier") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};
static asnField_t unicastAddrIpxAddr[] = 
{ /* SEQUENCE */
        {FNAME("node") ASN_OCTSTR, ASN_FIXD, 6, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("netnum") ASN_OCTSTR, ASN_FIXD, 4, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("tsapIdentifier") ASN_OCTSTR, ASN_FIXD, 2, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t unicastAddrIp6Addr[] = 
{ /* SEQUENCE */
        {FNAME("network") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("tsapIdentifier") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t unicastAddrIpSourceRouteAddrRouting[] = 
{       /* CHOICE */
        {FNAME("strict") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("loose") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t unicastAddrIpSourceRouteAddrRoute[] = 
{ /* SEQUENCE OF */
        {FNAME("item") ASN_OCTSTR, ASN_FIXD, 4, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t unicastAddrIpSourceRouteAddr[] = 
{       /* SEQUENCE */
        {FNAME("routing") ASN_CHOICE, 1, 2, 2, ASN_SKIP, 0,
         unicastAddrIpSourceRouteAddrRouting},
        {FNAME("network") ASN_OCTSTR, ASN_FIXD, 4, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("tsapIdentifier") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("route") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP, 0,
         unicastAddrIpSourceRouteAddrRoute},
};

static asnField_t unicastAddr[] = 
{    /* CHOICE */
        {FNAME("iPAddress") ASN_SEQ, 0, 2, 2, ASN_DECODE | ASN_EXT,
         offsetof(unicastAddr_t, ipAddr), unicastAddrIpAddr},
        {FNAME("iPXAddress") ASN_SEQ, 0, 3, 3, ASN_SKIP | ASN_EXT, 0,
         unicastAddrIpxAddr},
        {FNAME("iP6Address") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0,
         unicastAddrIp6Addr},
        {FNAME("netBios") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("iPSourceRouteAddress") ASN_SEQ, 0, 4, 4, ASN_SKIP | ASN_EXT, 0,
         unicastAddrIpSourceRouteAddr},
        {FNAME("nsap") ASN_OCTSTR, 5, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("nonStandardAddress") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t multicastAddrIpAddr[] = 
{        /* SEQUENCE */
        {FNAME("network") ASN_OCTSTR, ASN_FIXD, 4, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("tsapIdentifier") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t multicastAddrIp6Addr[] = 
{       /* SEQUENCE */
        {FNAME("network") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("tsapIdentifier") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t multicastAddr[] = 
{  /* CHOICE */
        {FNAME("iPAddress") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0,
         multicastAddrIpAddr},
        {FNAME("iP6Address") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0,
         multicastAddrIp6Addr},
        {FNAME("nsap") ASN_OCTSTR, 5, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("nonStandardAddress") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H245TransportAddr[] = 
{     /* CHOICE */
        {FNAME("unicastAddress") ASN_CHOICE, 3, 5, 7, ASN_DECODE | ASN_EXT,
         offsetof(H245TransportAddr_t, unicastAddr), unicastAddr},
        {FNAME("multicastAddress") ASN_CHOICE, 1, 2, 4, ASN_SKIP | ASN_EXT, 0,
         multicastAddr},
};

static asnField_t H2250LogParameters[] = 
{     /* SEQUENCE */
        {FNAME("nonStandard") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         H2250LogParametersNonStd},
        {FNAME("sessionID") ASN_INT, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("associatedSessionID") ASN_INT, 8, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("mediaChannel") ASN_CHOICE, 1, 2, 2, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(H2250LogParams_t, mediaChannel),
         H245TransportAddr},
        {FNAME("mediaGuaranteedDelivery") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("mediaControlChannel") ASN_CHOICE, 1, 2, 2, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(H2250LogParams_t, mediaControlChannel),
         H245TransportAddr},
        {FNAME("mediaControlGuaranteedDelivery") ASN_BOOL, ASN_FIXD, 0, 0, ASN_STOP | ASN_OPT,
         0, L7_NULLPTR},
        {FNAME("silenceSuppression") ASN_BOOL, ASN_FIXD, 0, 0, ASN_STOP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("destination") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("dynamicRTPPayloadType") ASN_INT, 5, 96, 0,ASN_STOP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("mediaPacketization") ASN_CHOICE, 0, 1, 2, ASN_STOP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("transportCapability") ASN_SEQ, 3, 3, 3, ASN_STOP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("redundancyEncoding") ASN_SEQ, 1, 2, 2, ASN_STOP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("source") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t openLogFwdMpexParameters[] = 
{    /* CHOICE */
        {FNAME("h222LogicalChannelParameters") ASN_SEQ, 3, 5, 5, ASN_SKIP | ASN_EXT, 0,
         H222LogParameters},
        {FNAME("h223LogicalChannelParameters") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0,
         H223LogParameters},
        {FNAME("v76LogicalChannelParameters") ASN_SEQ, 0, 5, 5, ASN_SKIP | ASN_EXT, 0,
         V76LogParameters},
        {FNAME("h2250LogicalChannelParameters") ASN_SEQ, 10, 11, 14, ASN_DECODE | ASN_EXT,
         offsetof
         (openLogFwdMpexParams_t,
          h2250LogParams),H2250LogParameters},
        {FNAME("none") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t openLogFwdParameters[] = 
{        /* SEQUENCE */

        {FNAME("portNumber") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("dataType") ASN_CHOICE, 3, 6, 9, ASN_DECODE | ASN_EXT,
         offsetof(openLogFwdParams_t,
                  dataType), dataType},
        {FNAME("multiplexParameters") ASN_CHOICE, 2, 3, 5, ASN_DECODE | ASN_EXT,
         offsetof(openLogFwdParams_t,
                  multiplexParams),
         openLogFwdMpexParameters},
        {FNAME("forwardLogicalChannelDependency") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT,
         0, L7_NULLPTR},
        {FNAME("replacementFor") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t openLogRevMpexParameters[] = 
{    /* CHOICE */
        {FNAME("h223LogicalChannelParameters") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT, 0,
         H223LogParameters},
        {FNAME("v76LogicalChannelParameters") ASN_SEQ, 0, 5, 5, ASN_SKIP | ASN_EXT, 0,
         V76LogParameters},
        {FNAME("h2250LogicalChannelParameters") ASN_SEQ, 10, 11, 14, ASN_DECODE | ASN_EXT,
         offsetof
         (openLogRevMpexParams_t,
          h2250LogParams), H2250LogParameters},
};

static asnField_t openLogRevParameters[] = 
{        /* SEQUENCE */
        {FNAME("dataType") ASN_CHOICE, 3, 6, 9, ASN_SKIP | ASN_EXT, 0, dataType},
        {FNAME("multiplexParameters") ASN_CHOICE, 1, 2, 3, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(openLogRevParams_t, multiplexParams),
         openLogRevParameters},
        {FNAME("reverseLogicalChannelDependency") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT,
         0, L7_NULLPTR},
        {FNAME("replacementFor") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t networkAccessParametersDistribution[] = 
{      /* CHOICE */
        {FNAME("unicast") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("multicast") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t Q2931AddrAddr[] = 
{      /* CHOICE */
        {FNAME("internationalNumber") ASN_NUMSTR, 4, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("nsapAddress") ASN_OCTSTR, 5, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t Q2931Addr[] = 
{      /* SEQUENCE */
        {FNAME("address") ASN_CHOICE, 1, 2, 2, ASN_SKIP | ASN_EXT, 0,
         Q2931AddrAddr},
        {FNAME("subaddress") ASN_OCTSTR, 5, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t networkAccessParametersNwAddr[] = 
{    /* CHOICE */
        {FNAME("q2931Address") ASN_SEQ, 1, 2, 2, ASN_SKIP | ASN_EXT, 0, Q2931Addr},
        {FNAME("e164Address") ASN_NUMDGT, 7, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("localAreaAddress") ASN_CHOICE, 1, 2, 2, ASN_DECODE | ASN_EXT,
         offsetof(networkAccessAddr_t, localAreaAddr),
         H245TransportAddr},
};

static asnField_t networkAccessParameters[] = 
{   /* SEQUENCE */
        {FNAME("distribution") ASN_CHOICE, 1, 2, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         networkAccessParametersDistribution},
        {FNAME("networkAddress") ASN_CHOICE, 2, 3, 3, ASN_DECODE | ASN_EXT,
         offsetof(networkAccessParams_t, networkAddr),
         networkAccessParametersNwAddr},
        {FNAME("associateConference") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("externalReference") ASN_OCTSTR, 8, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("t120SetupProcedure") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
};

static asnField_t openLogicalChannel[] = 
{        /* SEQUENCE */
        {FNAME("forwardLogicalChannelNumber") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("forwardLogicalChannelParameters") ASN_SEQ, 1, 3, 5, ASN_DECODE | ASN_EXT,
         offsetof(openLogicalChannel_t, fwdLogParams),
         openLogFwdParameters},
        {FNAME("reverseLogicalChannelParameters") ASN_SEQ, 1, 2, 4,
         ASN_DECODE | ASN_EXT | ASN_OPT, offsetof(openLogicalChannel_t,
                                     revLogParams),
         openLogRevParameters},
        {FNAME("separateStack") ASN_SEQ, 2, 4, 5, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(openLogicalChannel_t, separateStack),
         networkAccessParameters},
        {FNAME("encryptionSync") ASN_SEQ, 2, 4, 4, ASN_STOP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t setupUUIEFastStart[] = 
{      /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 1, 3, 5, ASN_DECODE | ASN_OPEN | ASN_EXT,
         sizeof(openLogicalChannel_t), openLogicalChannel}
        ,
};

static asnField_t setupUUIE[] = 
{        /* SEQUENCE */
        {FNAME("protocolIdentifier") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("h245Address") ASN_CHOICE, 3, 7, 7, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(setupUUIE_t, h245Addr), transportAddr},
        {FNAME("sourceAddress") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         setupUUIESourceAddr},
        {FNAME("sourceInfo") ASN_SEQ, 6, 8, 10, ASN_SKIP | ASN_EXT, 0, endpointType},
        {FNAME("destinationAddress") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         setupUUIEDestinationAddr},
        {FNAME("destCallSignalAddress") ASN_CHOICE, 3, 7, 7, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(setupUUIE_t, destCallSignalAddr), transportAddr},
        {FNAME("destExtraCallInfo") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         setupUUIEdestExtraCallInfo},
        {FNAME("destExtraCRV") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         setupUUIEDestExtraCRV},
        {FNAME("activeMC") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("conferenceID") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("conferenceGoal") ASN_CHOICE, 2, 3, 5, ASN_SKIP | ASN_EXT, 0,
         setupUUIEConferenceGoal},
        {FNAME("callServices") ASN_SEQ, 0, 8, 8, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         QseriesOptions},
        {FNAME("callType") ASN_CHOICE, 2, 4, 4, ASN_SKIP | ASN_EXT, 0, callType},
        {FNAME("sourceCallSignalAddress") ASN_CHOICE, 3, 7, 7, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(setupUUIE_t, sourceCallSignalAddr), transportAddr},
        {FNAME("remoteExtensionAddress") ASN_CHOICE, 1, 2, 7, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("callIdentifier") ASN_SEQ, 0, 1, 1, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("h245SecurityCapability") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("tokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cryptoTokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("fastStart") ASN_SEQOF, ASN_SEMI, 0, 30, ASN_DECODE | ASN_OPT,
         offsetof(setupUUIE_t, faststart), setupUUIEFastStart},
        {FNAME("mediaWaitForConnect") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("canOverlapSend") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("endpointIdentifier") ASN_BMPSTR, 7, 1, 0, ASN_STOP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("multipleCalls") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("maintainConnection") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("connectionParameters") ASN_SEQ, 0, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("language") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("presentationIndicator") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("screeningIndicator") ASN_ENUM, 2, 0, 0, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("serviceControl") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("symmetricOperationRequired") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("capacity") ASN_SEQ, 2, 2, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("circuitInfo") ASN_SEQ, 3, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("desiredProtocols") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("neededFeatures") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("desiredFeatures") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("supportedFeatures") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("parallelH245Control") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("additionalSourceAddresses") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         L7_NULLPTR},
};

static asnField_t callUUIEFastStart[] = 
{     /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 1, 3, 5, ASN_DECODE | ASN_OPEN | ASN_EXT,
         sizeof(openLogicalChannel_t), openLogicalChannel}
        ,
};

static asnField_t callUUIE[] = 
{       /* SEQUENCE */
        {FNAME("protocolIdentifier") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("destinationInfo") ASN_SEQ, 6, 8, 10, ASN_SKIP | ASN_EXT, 0,
         endpointType},
        {FNAME("h245Address") ASN_CHOICE, 3, 7, 7, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(callUUIE_t, h245Addr), transportAddr},
        {FNAME("callIdentifier") ASN_SEQ, 0, 1, 1, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("h245SecurityMode") ASN_CHOICE, 2, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("tokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cryptoTokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("fastStart") ASN_SEQOF, ASN_SEMI, 0, 30, ASN_DECODE | ASN_OPT,
         offsetof(callUUIE_t, fastStart),
         callUUIEFastStart},
        {FNAME("multipleCalls") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("maintainConnection") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("fastConnectRefused") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("featureSet") ASN_SEQ, 3, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t connectUUIEFastStart[] = 
{    /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 1, 3, 5, ASN_DECODE | ASN_OPEN | ASN_EXT,
         sizeof(openLogicalChannel_t), openLogicalChannel}
        ,
};

static asnField_t connectUUIE[] = 
{      /* ASN_SEQUENCE */
        {FNAME("protocolIdentifier") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("h245Address") ASN_CHOICE, 3, 7, 7, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(connectUUIE_t, h245Addr), transportAddr},
        {FNAME("destinationInfo") ASN_SEQ, 6, 8, 10, ASN_SKIP | ASN_EXT, 0,
         endpointType},
        {FNAME("conferenceID") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("callIdentifier") ASN_SEQ, 0, 1, 1, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("h245SecurityMode") ASN_CHOICE, 2, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("tokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cryptoTokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("fastStart") ASN_SEQOF, ASN_SEMI, 0, 30, ASN_DECODE | ASN_OPT,
         offsetof(connectUUIE_t, fastStart),connectUUIEFastStart},
        {FNAME("multipleCalls") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("maintainConnection") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("language") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("connectedAddress") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("presentationIndicator") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("screeningIndicator") ASN_ENUM, 2, 0, 0, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("fastConnectRefused") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("serviceControl") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("capacity") ASN_SEQ, 2, 2, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("featureSet") ASN_SEQ, 3, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t alertingUUIEFastStart[] = 
{   /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 1, 3, 5, ASN_DECODE | ASN_OPEN | ASN_EXT,
         sizeof(openLogicalChannel_t), openLogicalChannel}
        ,
};

static asnField_t alertingUUIE[] = 
{     /* SEQUENCE */
        {FNAME("protocolIdentifier") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("destinationInfo") ASN_SEQ, 6, 8, 10, ASN_SKIP | ASN_EXT, 0,
         endpointType},
        {FNAME("h245Address") ASN_CHOICE, 3, 7, 7, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(alertingUUIE_t, h245Addr), transportAddr},
        {FNAME("callIdentifier") ASN_SEQ, 0, 1, 1, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("h245SecurityMode") ASN_CHOICE, 2, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("tokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cryptoTokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("fastStart") ASN_SEQOF, ASN_SEMI, 0, 30, ASN_DECODE | ASN_OPT,
         offsetof(alertingUUIE_t, fastStart), alertingUUIEFastStart},
        {FNAME("multipleCalls") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("maintainConnection") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("alertingAddress") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("presentationIndicator") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("screeningIndicator") ASN_ENUM, 2, 0, 0, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("fastConnectRefused") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("serviceControl") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("capacity") ASN_SEQ, 2, 2, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("featureSet") ASN_SEQ, 3, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t informationUUIEfastStart[] = 
{        /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 1, 3, 5, ASN_DECODE | ASN_OPEN | ASN_EXT,
         sizeof(openLogicalChannel_t), openLogicalChannel}
        ,
};

static asnField_t informationUUIE[] = 
{  /* SEQUENCE */
        {FNAME("protocolIdentifier") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("callIdentifier") ASN_SEQ, 0, 1, 1, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("tokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cryptoTokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("fastStart") ASN_SEQOF, ASN_SEMI, 0, 30, ASN_DECODE | ASN_OPT,
         offsetof(informationUUIE_t, fastStart), informationUUIEfastStart},
        {FNAME("fastConnectRefused") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("circuitInfo") ASN_SEQ, 3, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t releaseCompleteReason[] = 
{     /* CHOICE */
        {FNAME("noBandwidth") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("gatekeeperResources") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("unreachableDestination") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("destinationRejection") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("invalidRevision") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("noPermission") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("unreachableGatekeeper") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("gatewayResources") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("badFormatAddress") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("adaptiveBusy") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("inConf") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("undefinedReason") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("facilityCallDeflection") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("securityDenied") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("calledPartyNotRegistered") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("callerNotRegistered") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("newConnectionNeeded") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("nonStandardReason") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("replaceWithConferenceInvite") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP, 0,
         L7_NULLPTR},
        {FNAME("genericDataReason") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("neededFeatureNotSupported") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("tunnelledSignallingRejected") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t releaseCompleteUUIE[] = 
{      /* SEQUENCE */
        {FNAME("protocolIdentifier") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("reason") ASN_CHOICE, 4, 12, 22, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         releaseCompleteReason},
        {FNAME("callIdentifier") ASN_SEQ, 0, 1, 1, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("tokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cryptoTokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("busyAddress") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("presentationIndicator") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("screeningIndicator") ASN_ENUM, 2, 0, 0, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("capacity") ASN_SEQ, 2, 2, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("serviceControl") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("featureSet") ASN_SEQ, 3, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t facilityUUIEAltAliasAddr[] = 
{     /* SEQUENCE OF */
        {FNAME("item") ASN_CHOICE, 1, 2, 7, ASN_SKIP | ASN_EXT, 0, aliasAddr},
};

static asnField_t facilityReason[] = 
{    /* ASN_CHOICE */
        {FNAME("routeCallToGatekeeper") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("callForwarded") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("routeCallToMC") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("undefinedReason") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("conferenceListChoice") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("startH245") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("noH245") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("newTokens") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("featureSetUpdate") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("forwardedElements") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("transportedInformation") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t facilityUUIEFastStart[] = 
{   /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 1, 3, 5, ASN_DECODE | ASN_OPEN | ASN_EXT,
         sizeof(openLogicalChannel_t), openLogicalChannel}
        ,
};

static asnField_t facilityUUIE[] = 
{     /* SEQUENCE */
        {FNAME("protocolIdentifier") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("alternativeAddress") ASN_CHOICE, 3, 7, 7, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         transportAddr},
        {FNAME("alternativeAliasAddress") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         facilityUUIEAltAliasAddr},
        {FNAME("conferenceID") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("reason") ASN_CHOICE, 2, 4, 11, ASN_DECODE | ASN_EXT,
         offsetof(facilityUUIE_t, reason), facilityReason},
        {FNAME("callIdentifier") ASN_SEQ, 0, 1, 1, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("destExtraCallInfo") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("remoteExtensionAddress") ASN_CHOICE, 1, 2, 7, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("tokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("cryptoTokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("conferences") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("h245Address") ASN_CHOICE, 3, 7, 7, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(facilityUUIE_t, h245Addr), transportAddr},
        {FNAME("fastStart") ASN_SEQOF, ASN_SEMI, 0, 30, ASN_DECODE | ASN_OPT,
         offsetof(facilityUUIE_t, fastStart), facilityUUIEFastStart},
        {FNAME("multipleCalls") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("maintainConnection") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("fastConnectRefused") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("serviceControl") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("circuitInfo") ASN_SEQ, 3, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("featureSet") ASN_SEQ, 3, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("destinationInfo") ASN_SEQ, 6, 8, 10, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("h245SecurityMode") ASN_CHOICE, 2, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         L7_NULLPTR},
};

static asnField_t callIdentifier[] = 
{    /* SEQUENCE */
        {FNAME("guid") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t securityServiceMode[] = 
{       /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, nonStandardParameter},
        {FNAME("none") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("default") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t securityCapabilities[] = 
{      /* SEQUENCE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("encryption") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT, 0,
         securityServiceMode},
        {FNAME("authenticaton") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT, 0,
         securityServiceMode},
        {FNAME("integrity") ASN_CHOICE, 2, 3, 3, ASN_SKIP | ASN_EXT, 0,
         securityServiceMode},
};

static asnField_t H245Security[] = 
{      /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, nonStandardParameter},
        {FNAME("noSecurity") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("tls") ASN_SEQ, 1, 4, 4, ASN_SKIP | ASN_EXT, 0, securityCapabilities},
        {FNAME("ipsec") ASN_SEQ, 1, 4, 4, ASN_SKIP | ASN_EXT, 0, securityCapabilities},
};

static asnField_t dHset[] = 
{     /* SEQUENCE */
        {FNAME("halfkey") ASN_BITSTR, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("modSize") ASN_BITSTR, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("generator") ASN_BITSTR, ASN_WORD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t typedCertificate[] = 
{  /* SEQUENCE */
        {FNAME("type") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("certificate") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t H235NonStandardParameter[] = 
{ /* SEQUENCE */
        {FNAME("nonStandardIdentifier") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("data") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t clearToken[] = 
{        /* SEQUENCE */
        {FNAME("tokenASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("timeStamp") ASN_INT, ASN_CONS, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("password") ASN_BMPSTR, 7, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("dhkey") ASN_SEQ, 0, 3, 3, ASN_SKIP | ASN_EXT | ASN_OPT, 0, dHset},
        {FNAME("challenge") ASN_OCTSTR, 7, 8, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("random") ASN_INT, ASN_UNCO, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("certificate") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         typedCertificate},
        {FNAME("generalID") ASN_BMPSTR, 7, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("nonStandard") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         H235NonStandardParameter},
        {FNAME("eckasdhkey") ASN_CHOICE, 1, 2, 2, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("sendersID") ASN_BMPSTR, 7, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t progressUUIETokens[] = 
{      /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 8, 9, 11, ASN_SKIP | ASN_EXT, 0, clearToken},
};

static asnField_t params[] = 
{    /* SEQUENCE */
        {FNAME("ranInt") ASN_INT, ASN_UNCO, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("iv8") ASN_OCTSTR, ASN_FIXD, 8, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("iv16") ASN_OCTSTR, ASN_FIXD, 16, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t cryptoH323TokenEPPwdHashToken[] = 
{     /* SEQUENCE */
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("hash") ASN_BITSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoH323TokenEPPwdHash[] = 
{   /* SEQUENCE */
        {FNAME("alias") ASN_CHOICE, 1, 2, 7, ASN_SKIP | ASN_EXT, 0, aliasAddr},
        {FNAME("timeStamp") ASN_INT, ASN_CONS, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("token") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoH323TokenEPPwdHashToken},
};

static asnField_t cryptoH323TokenGKPwdHashToken[] = 
{     /* SEQUENCE */
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("hash") ASN_BITSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoH323TokenGKPwdHash[] = 
{   /* SEQUENCE */
        {FNAME("gatekeeperId") ASN_BMPSTR, 7, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("timeStamp") ASN_INT, ASN_CONS, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("token") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoH323TokenGKPwdHashToken},
};

static asnField_t cryptoH323TokenEPPwdEncr[] = 
{   /* SEQUENCE */
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("encryptedData") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoH323TokenGKPwdEncr[] = 
{   /* SEQUENCE */
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("encryptedData") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoH323TokenEPCert[] = 
{      /* SEQUENCE */
        {FNAME("toBeSigned") ASN_SEQ, 8, 9, 11, ASN_SKIP | ASN_OPEN | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("signature") ASN_BITSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoH323TokenGKCert[] = 
{      /* SEQUENCE */
        {FNAME("toBeSigned") ASN_SEQ, 8, 9, 11, ASN_SKIP | ASN_OPEN | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("signature") ASN_BITSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoH323TokenFastStart[] = 
{   /* SEQUENCE */
        {FNAME("toBeSigned") ASN_SEQ, 8, 9, 11, ASN_SKIP | ASN_OPEN | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("signature") ASN_BITSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoTokenEncryptedTokenT[] = 
{    /* SEQUENCE */
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("encryptedData") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoTokenEncryptedToken[] = 
{  /* SEQUENCE */
        {FNAME("tokenASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("token") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoTokenEncryptedTokenT},
};

static asnField_t cryptoTokenSignedTokenT[] = 
{       /* SEQUENCE */
        {FNAME("toBeSigned") ASN_SEQ, 8, 9, 11, ASN_SKIP | ASN_OPEN | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("signature") ASN_BITSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoTokenSignedToken[] = 
{     /* SEQUENCE */
        {FNAME("tokenASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("token") ASN_SEQ, 0, 4, 4, ASN_SKIP, 0,
         cryptoTokenSignedTokenT},
};

static asnField_t cryptoTokenHashedTokenT[] = 
{       /* SEQUENCE */
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("hash") ASN_BITSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoTokenHashedToken[] = 
{     /* SEQUENCE */
        {FNAME("tokenASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("hashedVals") ASN_SEQ, 8, 9, 11, ASN_SKIP | ASN_EXT, 0, clearToken},
        {FNAME("token") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoTokenHashedTokenT},
};

static asnField_t cryptoTokenPwdEncr[] = 
{ /* SEQUENCE */
        {FNAME("algorithmASN_OID") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("paramS") ASN_SEQ, 2, 2, 3, ASN_SKIP | ASN_EXT, 0, params},
        {FNAME("encryptedData") ASN_OCTSTR, ASN_SEMI, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
};

static asnField_t cryptoToken[] = 
{       /* CHOICE */
        {FNAME("cryptoEncryptedToken") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         cryptoTokenEncryptedToken},
        {FNAME("cryptoSignedToken") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0,
         cryptoTokenSignedToken},
        {FNAME("cryptoHashedToken") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoTokenHashedToken},
        {FNAME("cryptoPwdEncr") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoTokenPwdEncr},
};

static asnField_t cryptoH323Token[] = 
{   /* CHOICE */
        {FNAME("cryptoEPPwdHash") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoH323TokenEPPwdHash},
        {FNAME("cryptoGKPwdHash") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoH323TokenGKPwdHash},
        {FNAME("cryptoEPPwdEncr") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoH323TokenEPPwdEncr},
        {FNAME("cryptoGKPwdEncr") ASN_SEQ, 0, 3, 3, ASN_SKIP, 0,
         cryptoH323TokenGKPwdEncr},
        {FNAME("cryptoEPCert") ASN_SEQ, 0, 4, 4, ASN_SKIP, 0,
         cryptoH323TokenEPCert},
        {FNAME("cryptoGKCert") ASN_SEQ, 0, 4, 4, ASN_SKIP, 0,
         cryptoH323TokenGKCert},
        {FNAME("cryptoFastStart") ASN_SEQ, 0, 4, 4, ASN_SKIP, 0,
         cryptoH323TokenFastStart},
        {FNAME("nestedcryptoToken") ASN_CHOICE, 2, 4, 4, ASN_SKIP | ASN_EXT, 0,
         cryptoToken},
};

static asnField_t progressUUIEcryptoTokens[] = 
{        /* SEQUENCE OF */
        {FNAME("item") ASN_CHOICE, 3, 8, 8, ASN_SKIP | ASN_EXT, 0, cryptoH323Token},
};

static asnField_t progressUUIEFastStart[] = 
{   /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 1, 3, 5, ASN_DECODE | ASN_OPEN | ASN_EXT,
         sizeof(openLogicalChannel_t), openLogicalChannel}
        ,
};

static asnField_t progressUUIE[] = 
{     /* SEQUENCE */
        {FNAME("protocolIdentifier") ASN_OID, ASN_BYTE, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("destinationInfo") ASN_SEQ, 6, 8, 10, ASN_SKIP | ASN_EXT, 0,
         endpointType},
        {FNAME("h245Address") ASN_CHOICE, 3, 7, 7, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(progressUUIE_t, h245Addr), transportAddr},
        {FNAME("callIdentifier") ASN_SEQ, 0, 1, 1, ASN_SKIP | ASN_EXT, 0,
         callIdentifier},
        {FNAME("h245SecurityMode") ASN_CHOICE, 2, 4, 4, ASN_SKIP | ASN_EXT | ASN_OPT, 0,
         H245Security},
        {FNAME("tokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         progressUUIETokens},
        {FNAME("cryptoTokens") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         progressUUIEcryptoTokens},
        {FNAME("fastStart") ASN_SEQOF, ASN_SEMI, 0, 30, ASN_DECODE | ASN_OPT,
         offsetof(progressUUIE_t, fastStart), progressUUIEFastStart},
        {FNAME("multipleCalls") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("maintainConnection") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("fastConnectRefused") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t h323MessageBody[] = 
{     /* CHOICE */
        {FNAME("setup") ASN_SEQ, 7, 13, 39, ASN_DECODE | ASN_EXT,
         offsetof(h323MessageBody_t, u.setup) ,setupUUIE},
        {FNAME("callProceeding") ASN_SEQ, 1, 3, 12, ASN_DECODE | ASN_EXT,
         offsetof(h323MessageBody_t,u.call),
         callUUIE},
        {FNAME("connect") ASN_SEQ, 1, 4, 19, ASN_DECODE | ASN_EXT,
         offsetof(h323MessageBody_t, u.connect), connectUUIE},
        {FNAME("alerting") ASN_SEQ, 1, 3, 17, ASN_DECODE | ASN_EXT,
         offsetof(h323MessageBody_t, u.alerting), alertingUUIE},
        {FNAME("information") ASN_SEQ, 0, 1, 7, ASN_DECODE | ASN_EXT,
         offsetof(h323MessageBody_t, u.information),
         informationUUIE},
        {FNAME("releaseComplete") ASN_SEQ, 1, 2, 11, ASN_SKIP | ASN_EXT, 0,
         releaseCompleteUUIE},
        {FNAME("facility") ASN_SEQ, 3, 5, 21, ASN_DECODE | ASN_EXT,
         offsetof(h323MessageBody_t, u.facility), facilityUUIE},
        {FNAME("progress") ASN_SEQ, 5, 8, 11, ASN_DECODE | ASN_EXT,
         offsetof(h323MessageBody_t, u.progress), progressUUIE},
        {FNAME("empty") ASN_NUL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("status") ASN_SEQ, 2, 4, 4, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("statusInquiry") ASN_SEQ, 2, 4, 4, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("setupAcknowledge") ASN_SEQ, 2, 4, 4, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("notify") ASN_SEQ, 2, 4, 4, ASN_SKIP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t requestMessage[] = 
{    /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("masterSlaveDetermination") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("terminalCapabilitySet") ASN_SEQ, 3, 5, 5, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("openLogicalChannel") ASN_SEQ, 1, 3, 5, ASN_DECODE | ASN_EXT,
         offsetof(requestMessage_t, openLogicalChannel), openLogicalChannel},
        {FNAME("closeLogicalChannel") ASN_SEQ, 0, 2, 3, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("requestChannelClose") ASN_SEQ, 0, 1, 3, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("multiplexEntrySend") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("requestMultiplexEntry") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("requestMode") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("roundTripDelayRequest") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("maintenanceLoopRequest") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("communicationModeRequest") ASN_SEQ, 0, 0, 0, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("conferenceRequest") ASN_CHOICE, 3, 8, 16, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("multilinkRequest") ASN_CHOICE, 3, 5, 5, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("logicalChannelRateRequest") ASN_SEQ, 0, 3, 3, ASN_STOP | ASN_EXT, 0,
         L7_NULLPTR},
};

static asnField_t openLogAckRevMpexParams[] = 
{ /* CHOICE */
        {FNAME("h222LogicalChannelParameters") ASN_SEQ, 3, 5, 5, ASN_SKIP | ASN_EXT, 0,
         H222LogParameters},
        {FNAME("h2250LogicalChannelParameters") ASN_SEQ, 10, 11, 14, ASN_DECODE | ASN_EXT,
         offsetof
         (openLogAckRevMpexParams_t,
          h2250LogParams),H2250LogParameters},
};

static asnField_t openLogAckRevParams[] = 
{     /* SEQUENCE */
        {FNAME("reverseLogicalChannelNumber") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("portNumber") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("multiplexParameters") ASN_CHOICE, 0, 1, 2, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(openLogAckRevParams_t,multiplexParams),
         openLogAckRevMpexParams},
        {FNAME("replacementFor") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t H2250LogAckParametersNonStd[] = 
{      /* SEQUENCE OF */
        {FNAME("item") ASN_SEQ, 0, 2, 2, ASN_SKIP, 0, H245NonStdParameter},
};

static asnField_t H2250LogAckParameters[] = 
{  /* SEQUENCE */
        {FNAME("nonStandard") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         H2250LogAckParametersNonStd},
        {FNAME("sessionID") ASN_INT, 8, 1, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("mediaChannel") ASN_CHOICE, 1, 2, 2, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(H2250LogAckParams_t, mediaChannel),
         H245TransportAddr},
        {FNAME("mediaControlChannel") ASN_CHOICE, 1, 2, 2, ASN_DECODE | ASN_EXT | ASN_OPT,
         offsetof(H2250LogAckParams_t, mediaControlChannel),
         H245TransportAddr},
        {FNAME("dynamicRTPPayloadType") ASN_INT, 5, 96, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("flowControlToZero") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("portNumber") ASN_INT, ASN_WORD, 0, 0, ASN_SKIP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t openLogAckFwdMpexParams[] = 
{       /* CHOICE */
        {FNAME("h2250LogicalChannelAckParameters") ASN_SEQ, 5, 5, 7, ASN_DECODE | ASN_EXT,
         offsetof(openLogAckFwdMpexParams_t,
                  H2250LogAckParams),
         H2250LogAckParameters},
};

static asnField_t openLogAck[] = 
{     /* SEQUENCE */
        {FNAME("forwardLogicalChannelNumber") ASN_INT, ASN_WORD, 1, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("reverseLogicalChannelParameters") ASN_SEQ, 2, 3, 4,
         ASN_DECODE | ASN_EXT | ASN_OPT, offsetof(openLogAckParams_t,
                                      reverseParams),
         openLogAckRevMpexParams},
        {FNAME("separateStack") ASN_SEQ, 2, 4, 5, ASN_SKIP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("forwardMultiplexAckParameters") ASN_CHOICE, 0, 1, 1,
         ASN_DECODE | ASN_EXT | ASN_OPT, offsetof(openLogAckParams_t,
                                      fwdMpexParams),
         openLogAckFwdMpexParams},
        {FNAME("encryptionSync") ASN_SEQ, 2, 4, 4, ASN_STOP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t responseMessage[] = 
{   /* CHOICE */
        {FNAME("nonStandard") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("masterSlaveDeterminationAck") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0,
         L7_NULLPTR},
        {FNAME("masterSlaveDeterminationReject") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0,
         L7_NULLPTR},
        {FNAME("terminalCapabilitySetAck") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("terminalCapabilitySetReject") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0,
         L7_NULLPTR},
        {FNAME("openLogicalChannelAck") ASN_SEQ, 1, 2, 5, ASN_DECODE | ASN_EXT,
         offsetof(responseMessage_t, openLogAckParams),
        openLogAck },
        {FNAME("openLogicalChannelReject") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("closeLogicalChannelAck") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("requestChannelCloseAck") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("requestChannelCloseReject") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0,
         L7_NULLPTR},
        {FNAME("multiplexEntrySendAck") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("multiplexEntrySendReject") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("requestMultiplexEntryAck") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("requestMultiplexEntryReject") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0,
         L7_NULLPTR},
        {FNAME("requestModeAck") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("requestModeReject") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("roundTripDelayResponse") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("maintenanceLoopAck") ASN_SEQ, 0, 1, 1, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("maintenanceLoopReject") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("communicationModeResponse") ASN_CHOICE, 0, 1, 1,ASN_STOP | ASN_EXT, 0,
         L7_NULLPTR},
        {FNAME("conferenceResponse") ASN_CHOICE, 3, 8, 16, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("multilinkResponse") ASN_CHOICE, 3, 5, 5, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("logicalChannelRateAcknowledge") ASN_SEQ, 0, 3, 3, ASN_STOP | ASN_EXT, 0,
         L7_NULLPTR},
        {FNAME("logicalChannelRateReject") ASN_SEQ, 1, 4, 4, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t mediaMessage[] = 
{    /* CHOICE */
        {FNAME("request") ASN_CHOICE, 4, 11, 15, ASN_DECODE | ASN_EXT,
         offsetof(mediaMessage_t,u.request), requestMessage},
        {FNAME("response") ASN_CHOICE, 5, 19, 24, ASN_DECODE | ASN_EXT,
         offsetof(mediaMessage_t, u.response),
         responseMessage},
        {FNAME("command") ASN_CHOICE, 3, 7, 12, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
        {FNAME("indication") ASN_CHOICE, 4, 14, 23, ASN_STOP | ASN_EXT, 0, L7_NULLPTR},
};

static asnField_t h323PduH245Control[] = 
{   /* SEQUENCE OF */
        {FNAME("item") ASN_CHOICE, 2, 4, 4, ASN_DECODE | ASN_OPEN | ASN_EXT,
         sizeof(mediaMessage_t),
         mediaMessage}
        ,
};

static asnField_t h323Pdu[] = 
{       /* SEQUENCE */
        {FNAME("h323-message-body") ASN_CHOICE, 3, 7, 13, ASN_DECODE | ASN_EXT,
         offsetof(h323Pdu_t,h323MessageBody),
         h323MessageBody},
        {FNAME("nonStandardData") ASN_SEQ, 0, 2, 2, ASN_SKIP | ASN_OPT, 0,
         nonStandardParameter},
        {FNAME("h4501SupplementaryService") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_SKIP | ASN_OPT, 0,
         L7_NULLPTR},
        {FNAME("h245Tunneling") ASN_BOOL, ASN_FIXD, 0, 0, ASN_SKIP, 0, L7_NULLPTR},
        {FNAME("h245Control") ASN_SEQOF, ASN_SEMI, 0, 4, ASN_DECODE | ASN_OPT,
         offsetof(h323Pdu_t, h245Control), h323PduH245Control},
        {FNAME("nonStandardControl") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_STOP | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("callLinkage") ASN_SEQ, 2, 2, 2, ASN_STOP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("tunnelledSignallingMessage") ASN_SEQ, 2, 4, 4, ASN_STOP | ASN_EXT | ASN_OPT,
         0, L7_NULLPTR},
        {FNAME("provisionalRespToH245Tunneling") ASN_NUL, ASN_FIXD, 0, 0, ASN_STOP | ASN_OPT,
         0, L7_NULLPTR},
        {FNAME("stimulusControl") ASN_SEQ, 3, 3, 3, ASN_STOP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
        {FNAME("genericData") ASN_SEQOF, ASN_SEMI, 0, 0, ASN_STOP | ASN_OPT, 0, L7_NULLPTR},
};

static asnField_t h323UserInformation[] = 
{      /* SEQUENCE */
        {FNAME("h323-uu-pdu") ASN_SEQ, 1, 2, 11, ASN_DECODE | ASN_EXT,
         offsetof(h323UserInfo_t, h323Pdu), h323Pdu},
        {FNAME("user-data") ASN_SEQ, 0, 2, 2, ASN_STOP | ASN_EXT | ASN_OPT, 0, L7_NULLPTR},
};


#endif

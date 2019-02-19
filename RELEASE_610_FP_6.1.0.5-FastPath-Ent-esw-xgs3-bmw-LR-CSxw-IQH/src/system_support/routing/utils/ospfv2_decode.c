#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "ospfv2_decode.ext"
#include "osapi_support.h"
#include "sysapi.h"
#include "iptypes.h"

/* These decode functions all assume the LSA header and body are in one
 * contiguous buffer. This is currently true, since the decode is only used
 * by "show ip ospf database."  Unlike OSPFv3, OSPFv2 tracing does not
 * use these decode functions. So no need for a separate LSA body pointer. */
typedef L7_RC_t (*decodeFuncPtrv2)(L7_char8 *, outputFuncPtr, void *, L7_uint32);

decodeFuncPtrv2 decodev2[OSPF_LSA_FC_COUNT] = {
/* 0  */ ospfUnknownLsaDecode,
/* 1  */ ospfRouterLsaDecode,
/* 2  */ ospfNetworkLsaDecode,
/* 3  */ ospfNetworkSummaryLsaDecode,
/* 4  */ ospfAsbrSummaryLsaDecode,
/* 5  */ ospfASExternalLsaDecode,
/* 6  */ ospfGroupMembershipLsaDecode,
/* 7  */ ospfASExternalLsaDecode,
/* 8  */ ospfTemp2LsaDecode,
/* 9  */ ospfLinkOpaqueLsaDecode,
/* 10 */ ospfAreaOpaqueLsaDecode,
/* 11 */ ospfASOpaqueLsaDecode
};


/* LSA header output strings */

char *v2lsaFuncCodeToStr[OSPF_LSA_FC_COUNT] = {
/* 0  */ "Unknown",
/* 1  */ "Router LSA",
/* 2  */ "Network LSA",
/* 3  */ "Network Summary LSA",
/* 4  */ "ASBR Summary LSA",
/* 5  */ "AS External LSA",
/* 6  */ "Group membership LSA",
/* 7  */ "Type 7 LSA",
/* 8  */ "Type 8 LSA",
/* 9  */ "Link Opaque LSA",
/* 10 */ "Area Opaque LSA",
/* 11 */ "AS Opaque LSA",
};

char *v2lsaHdrLsidDescStr[OSPF_LSA_FC_COUNT] = {
  /* 0  */ "Unknown",
  /* 1  */ "address of router",
  /* 2  */ "address of designated router",
  /* 3  */ "network prefix",
  /* 4  */ "address of ASBR",
  /* 5  */ "network prefix",
};

char *v2lsaHdrLsAgeStr = "LS Age";
char *v2lsaHdrLsTypeStr = "LS Type";
char *v2lsaHdrOpaqueTypeStr = "Opaque Type";
char *v2lsaHdrOpaqueIdStr = "Opaque Id";
char *v2lsaHdrLsidStr = "LS Id";
char *v2lsaHdrAdvRtrStr = "Advertising Router";
char *v2lsaHdrLsSeqNoStr = "LS Seq Number";
char *v2lsaHdrCsumStr = "Checksum";
char *v2lsaHdrLengthStr = "Length";

char *v2lsaOptionsStr = "LS options";

/* Per-LSA-type output strings */

char *v2lsaOptEBitStr = "E-Bit";
char *v2lsaOptMCBitStr = "MC-Bit";
char *v2lsaOptNPBitStr = "N/P-Bit";
char *v2lsaOptEABitStr = "E/A-Bit";
char *v2lsaOptDCBitStr = "DC-Bit";
char *v2lsaOptOBitStr = "O-Bit";

/* Router-LSA strings */

 /* Options field in RTR LSA DATA PART */

char *v2rtrLsaVBitStr = "Virtual Link Endpoint";
char *v2rtrLsaEBitStr = "AS Boundary Router";
char *v2rtrLsaBBitStr = "Area Border Router";
char *v2rtrLsaWBitStr = "Wildcard Multicast Receiver";
char *v2rtrLsaNtBitStr = "NSSA Translator";
char *v2rtrLsaNumLinksStr = "Number of Links";

 /* TYPE Field */
typedef enum {
  RTR_LSA_LINK_TYPE_UNUSED = 0,
  RTR_LSA_LINK_TYPE_P2P,
  RTR_LSA_LINK_TYPE_TRANSIT_NET,
  RTR_LSA_LINK_TYPE_RESERVED,
  RTR_LSA_LINK_TYPE_VLINK,
  RTR_LSA_LINK_TYPE_COUNT,
} v2rtrLsaIfType_t;

char *v2rtrLsaIfType [RTR_LSA_LINK_TYPE_COUNT] =
{
/* 0 */ NULL,
/* 1 */ "another router (point-to-point)",
/* 2 */ "transit network",
/* 3 */ "stub network",
/* 4 */ "another router (virtual link)"
};

 /* other Fields */
char *v2rtrLsaLinkTypeStr = "Link connected to";
char *v2rtrLsaMetricStr = " Metric";
char *v2rtrLsaLinkIdStr = "Link ID";
char *v2rtrLsaLinkDataStr = "Link Data";
char *v2rtrLSADRAddressStr = "Designated Router address";
char *v2rtrLSAIfAddrStr = "Router Interface address";

/* Network LSA Strings */
char *v2networkLSAAttachedRtrIdStr = "Attached Router";
char *v2networkMaskStr = "Network Mask";

char *v2metricStr = "Metric";


/* AS-External-LSA strings */
char *v2aseLsaOptionsStr = "Options";
char *v2aseLsaMetricStr = "Metric";
char *v2aseLsaMetricStrType="Metric Type";
char *v2aseLsaForwAddrStr = "Forwarding Address";
char *v2aseLsaTagStr = "Tag";
char *v2aseLsaOptionsEBitStr = "E-Bit";

ospfLsaFuncCode_t 
ospfLsaFuncCodeDecode(byte lsType)
{
  ospfLsaFuncCode_t fcType = lsType;

  if(! ((fcType > 0) &&
        (fcType != OSPF_LSA_FC_GROUP_MEMBERSHIP) &&
        (fcType < OSPF_LSA_FC_COUNT)) )
    fcType = OSPF_LSA_FC_UNUSED;

  return fcType;
}

L7_RC_t ospfLsaDecode(L7_char8 * lsaHdr, outputFuncPtr output, void * context)
{
  ospfLsaFuncCode_t lsaTypeFC;
  L7_char8 *lsaPayload;
  L7_uint32 lsaLen;

  assert(lsaHdr != NULL);

  lsaPayload = lsaHdr + L7_OSPF_LSA_HDR_LSTYPE_OFFSETOF;
  lsaTypeFC = lsaPayload[0] & L7_OSPF_LSA_HDR_LSTYPE_FUNCTION_CODE_MASK;
  ospfLsaHeaderDecode(lsaHdr, output, context, &lsaLen);
  
  if ((lsaTypeFC < OSPF_LSA_FC_UNUSED || 
      lsaTypeFC > OSPF_LSA_FC_AS_OPAQUE))
  {
    sysapiPrintf("Invalid LSA Function Code %d\n", (int)lsaTypeFC);
    return L7_FAILURE;
  }

  /* invoke lsa-specific decode by indexing into func ptr table */
  return decodev2[lsaTypeFC](lsaHdr, output, context, lsaLen);
}

L7_RC_t 
ospfLsaHeaderDecode(L7_char8 * lsaHdr, outputFuncPtr output, void * context, L7_uint32 *lsaLength)
{
  char outBuf[128];
  ospfLsaFuncCode_t lsaTypeFC;
  L7_ushort16 age;

  age = A_GET_2B((L7_uchar8 *)lsaHdr);
  sprintf(outBuf, "\r\n%s: %hu\r\n", v2lsaHdrLsAgeStr, age);
  output(context, outBuf);
  lsaHdr += 2;
  
  ospfLsaOptionsDecode( lsaHdr, output, context);
  lsaHdr += 1;
  lsaTypeFC = lsaHdr[0]& L7_OSPF_LSA_HDR_LSTYPE_FUNCTION_CODE_MASK;

  if(lsaTypeFC != 0)
  {
    sprintf(outBuf, "%s: %s\r\n", v2lsaHdrLsTypeStr, v2lsaFuncCodeToStr[lsaTypeFC]);
  }
  else
  {
    sprintf(outBuf, "%s: 0x%04x\r\n", v2lsaHdrLsTypeStr, A_GET_2B((L7_uchar8 *)lsaHdr));
  }  
  output(context, outBuf);
  lsaHdr += 1;
  
  if ((lsaTypeFC >= OSPF_LSA_FC_LINK_OPAQUE) && (lsaTypeFC <= OSPF_LSA_FC_AS_OPAQUE) )
  {
   sprintf(outBuf, "%s: %u\r\n", v2lsaHdrOpaqueTypeStr, lsaHdr[0]);
   lsaHdr += 1;
   output(context, outBuf);
   sprintf(outBuf, "%s: %lu\r\n", v2lsaHdrOpaqueIdStr, A_GET_3B((L7_uchar8 *)lsaHdr));
   lsaHdr += 3;
   output(context, outBuf);
  }
  else
  {
    if (lsaTypeFC > OSPF_LSA_FC_UNUSED && lsaTypeFC <= OSPF_LSA_FC_AS_OPAQUE)
      sprintf(outBuf, "%s: %s (%s)\r\n", v2lsaHdrLsidStr, osapiInet_ntoa(A_GET_4B((L7_uchar8 *)lsaHdr)),v2lsaHdrLsidDescStr[lsaTypeFC]);
    else
    {
      sprintf(outBuf, "%s: %s\r\n", v2lsaHdrLsidStr, osapiInet_ntoa(A_GET_4B((L7_uchar8 *)lsaHdr)));
    }
    output(context, outBuf);
    lsaHdr += 4;
  }

  sprintf(outBuf, "%s: %s\r\n", v2lsaHdrAdvRtrStr, osapiInet_ntoa(A_GET_4B((L7_uchar8 *)lsaHdr)));
  output(context, outBuf);
  lsaHdr += 4;

  sprintf(outBuf, "%s: 0x%08lx\r\n", v2lsaHdrLsSeqNoStr, A_GET_4B((L7_uchar8 *)lsaHdr));
  output(context, outBuf);
  lsaHdr += 4;

  sprintf(outBuf, "%s: 0x%04x\r\n", v2lsaHdrCsumStr, (L7_ushort16) A_GET_2B((L7_uchar8 *)lsaHdr));
  output(context, outBuf);
  lsaHdr += 2;

  *lsaLength = A_GET_2B((L7_uchar8 *)lsaHdr);
  sprintf(outBuf, "%s: %u\r\n", v2lsaHdrLengthStr, *lsaLength);
  output(context, outBuf);

  return L7_SUCCESS;
}

L7_RC_t 
ospfLsaOptionsDecode(L7_char8 * options, outputFuncPtr output, void * context)
{
  char outBuf[80];
  int  opt = options[0];

  memset(outBuf, 0x00, sizeof(outBuf));
  sprintf(outBuf, "%s: (", v2lsaOptionsStr);

  opt &= L7_OSPF_LSA_OPTIONS_KNOWNBITS_MASK;
  if (opt == 0)
  {
    strncat(outBuf, "None)\r\n", sizeof(outBuf));
  } 
  else {

    /* this do while is just a kludge to avoid goto */
    do {
      if ((opt & L7_OSPF_LSA_OPTIONS_OBIT_MASK) != 0)
      {
        strncat(outBuf, v2lsaOptOBitStr, sizeof(outBuf));
        opt &= ~L7_OSPF_LSA_OPTIONS_OBIT_MASK;
        if (opt != 0)
        {
          strncat(outBuf, " ", sizeof(outBuf));
        }
        else
        {
          strncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }

      if ((opt & L7_OSPF_LSA_OPTIONS_DCBIT_MASK) != 0)
      {
        strncat(outBuf, v2lsaOptDCBitStr, sizeof(outBuf));
        opt &= ~L7_OSPF_LSA_OPTIONS_DCBIT_MASK;
        if (opt != 0) 
        {
          strncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          strncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((opt & L7_OSPF_LSA_OPTIONS_EABIT_MASK) != 0)
      {
        strncat(outBuf, v2lsaOptEABitStr, sizeof(outBuf));
        opt &= ~L7_OSPF_LSA_OPTIONS_EABIT_MASK;
        if (opt != 0) 
        {
          strncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          strncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((opt & L7_OSPF_LSA_OPTIONS_NPBIT_MASK) != 0)
      {
        strncat(outBuf, v2lsaOptNPBitStr, sizeof(outBuf));
        opt &= ~L7_OSPF_LSA_OPTIONS_NPBIT_MASK;
        if (opt != 0) 
        {
          strncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          strncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((opt & L7_OSPF_LSA_OPTIONS_MCBIT_MASK) != 0)
      {
        strncat(outBuf, v2lsaOptMCBitStr, sizeof(outBuf));
        opt &= ~L7_OSPF_LSA_OPTIONS_MCBIT_MASK;
        if (opt != 0) 
        {
          strncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          strncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((opt & L7_OSPF_LSA_OPTIONS_EBIT_MASK) != 0)
      {
        strncat(outBuf, v2lsaOptEBitStr, sizeof(outBuf));
        opt &= ~L7_OSPF_LSA_OPTIONS_EBIT_MASK;
        if (opt != 0) 
        {
          strncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          strncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      /* if some other bit we didn't anticipate is set, we won't get the
       * close parens and newline, so force the issue here */
      if (opt != 0) 
      {
        strncat(outBuf, ")\r\n", sizeof(outBuf));
      }
    } while (0);
  }
  output(context, outBuf);
  return L7_SUCCESS;
}

L7_RC_t 
ospfRouterLsaDecode(L7_char8 * rtrLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  L7_char8 *lsaPayload;
  L7_int32 numLinks;
  char outBuf[128];

  lsaPayload = rtrLsa + L7_OSPF_LSA_HDR_LENGTH;
  numLinks =  A_GET_2B(lsaPayload + L7_OSPF_LSA_ROUTER_NUMLINKS_OFFSETOF);
  
  ospfRouterLsaBitsDecode(lsaPayload, output, context);

  sprintf(outBuf, "%s: %u\r\n", v2rtrLsaNumLinksStr, numLinks);
  output(context, outBuf);
  
  lsaPayload +=4;

  while (numLinks--)
  {
    ospfRouterLsaIfDecode(lsaPayload , output, context);
    lsaPayload += 12;
  }
  return L7_SUCCESS;
}

L7_RC_t 
ospfRouterLsaBitsDecode(L7_char8 * rtrLsa, outputFuncPtr output, void * context)
{
  char outBuf[128];
  int bits = *rtrLsa & L7_OSPF_LSA_ROUTER_KNOWNBITS_MASK;

    if (bits == 0)
    {
      return L7_SUCCESS;
    } 
    else 
    {
      outBuf[0] = '\0';
    }
    /* clear set bits as we go to avoid having to check all bits */
    /* this do while is just a kludge to avoid goto */
    do {
      if ((bits & L7_OSPF_LSA_ROUTER_NTBIT_MASK) != 0)
      {
        strncat(outBuf, v2rtrLsaNtBitStr, sizeof(outBuf));
        bits &= ~L7_OSPF_LSA_ROUTER_NTBIT_MASK;
        if (bits != 0) 
        {
          strncat(outBuf, ", ", sizeof(outBuf));
        }
        else 
        {
          strncat(outBuf, "\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((bits & L7_OSPF_LSA_ROUTER_WBIT_MASK) != 0)
      {
        strncat(outBuf, v2rtrLsaWBitStr, sizeof(outBuf));
        bits &= ~L7_OSPF_LSA_ROUTER_WBIT_MASK;
        if (bits != 0) 
        {
          strncat(outBuf, ", ", sizeof(outBuf));
        }
        else 
        {
          strncat(outBuf, "\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((bits & L7_OSPF_LSA_ROUTER_VBIT_MASK) != 0)
      {
        strncat(outBuf, v2rtrLsaVBitStr, sizeof(outBuf));
        bits &= ~L7_OSPF_LSA_ROUTER_VBIT_MASK;
        if (bits != 0) 
        {
          strncat(outBuf, ", ", sizeof(outBuf));
        }
        else 
        {
          strncat(outBuf, "\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((bits & L7_OSPF_LSA_ROUTER_EBIT_MASK) != 0)
      {
        strncat(outBuf, v2rtrLsaEBitStr, sizeof(outBuf));
        bits &= ~L7_OSPF_LSA_ROUTER_EBIT_MASK;
        if (bits != 0) 
        {
          strncat(outBuf, ", ", sizeof(outBuf));
        }
        else 
        {
          strncat(outBuf, "\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((bits & L7_OSPF_LSA_ROUTER_BBIT_MASK) != 0)
      {
         strncat(outBuf, v2rtrLsaBBitStr, sizeof(outBuf));
         bits &= ~L7_OSPF_LSA_ROUTER_BBIT_MASK;
         if (bits != 0) 
         {
           strncat(outBuf, ", ", sizeof(outBuf));
         }
         else 
         {
           strncat(outBuf, "\r\n", sizeof(outBuf));
           break;
         }
      }
      /* if some other bit we didn't anticipate is set, we won't get the
       * close parens and newline, so force the issue here */
      if (bits != 0) 
      {
         strncat(outBuf, "\r\n", sizeof(outBuf));
      }
    } while (0);
  output(context, outBuf);
  return L7_SUCCESS;
}

L7_RC_t 
ospfRouterLsaIfDecode(L7_char8 *rtrLsa, outputFuncPtr output, void * context)
{
  char outBuf[128];
  int ifType;

  output(context, "\r\n");

  sprintf(outBuf, "  (%s) %s:%s\r\n", v2rtrLsaLinkIdStr,v2rtrLSADRAddressStr, osapiInet_ntoa(A_GET_4B(rtrLsa)));
  output(context, outBuf);
  rtrLsa += 4;

  sprintf(outBuf, "  (%s) %s:%s\r\n", v2rtrLsaLinkDataStr,v2rtrLSAIfAddrStr, osapiInet_ntoa(A_GET_4B(rtrLsa)));
  output(context, outBuf);
  rtrLsa += 4;
  
  ifType = rtrLsa[0];

  if ((ifType >= RTR_LSA_LINK_TYPE_COUNT) || (ifType < RTR_LSA_LINK_TYPE_P2P))
  {
    sprintf(outBuf, "  %s: Unknown\r\n", v2rtrLsaLinkTypeStr);
  }
  else
  {
    sprintf(outBuf, "  %s: %s\r\n", v2rtrLsaLinkTypeStr, v2rtrLsaIfType[ifType]);
    output(context, outBuf);
  }  
    rtrLsa += 2;
   
    sprintf(outBuf, "  %s: %u\r\n", v2rtrLsaMetricStr, A_GET_2B(rtrLsa));
    output(context, outBuf);

  return L7_SUCCESS;

}


/*
 *
 *                             The Network-LSA 
 *
 *                         LSA Header + Following
 *
 *     0                   1                   2                   3
 *     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         Network Mask                          |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                       Attached Router                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                             ...                               |
 */

L7_RC_t 
ospfNetworkLsaDecode(L7_char8 *netLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  L7_char8 *lsaPayload, *lsaEndPtr;
  char outBuf[80];

  lsaPayload = netLsa + L7_OSPF_LSA_HDR_LENGTH;
  
 sprintf(outBuf, "%s: %s\r\n", v2networkMaskStr,
             osapiInet_ntoa(A_GET_4B(lsaPayload)));
     output(context, outBuf);

  lsaPayload += 4;

  lsaEndPtr = netLsa + lsaLength;

  while (lsaPayload < lsaEndPtr)
  {
    sprintf(outBuf, "  %s: %s\r\n", v2networkLSAAttachedRtrIdStr,
        osapiInet_ntoa(A_GET_4B(lsaPayload)));
    output(context, outBuf);
    lsaPayload += 4;

  }

  return L7_SUCCESS;
}

/*
 *
 *                           The AS-External-LSA 
 *                       (and also NSSA/Type-7-LSA)
 *
 *                         LSA Header + Following
 *
 *     0                   1                   2                   3
 *     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                       Network Mask                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |E| 0           |                Metric                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                Forwarding Address (Optional)                  |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |              External Route Tag (Optional)                    |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |      Tos      |              Tos  Metric                      |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                Forwarding Address (Optional)                  |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |              External Route Tag (Optional)                    |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                             ...                               |
 */

L7_RC_t 
ospfASExternalLsaDecode(L7_char8 *aseLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  L7_char8 *lsaPayload;
  L7_uchar8 opt, flags;
  char outBuf[128];
  char optBuf[80];
  L7_BOOL isExternalMetric = L7_FALSE;

  lsaPayload = aseLsa + L7_OSPF_LSA_HDR_LENGTH;

  sprintf(outBuf, "%s: %s\r\n", v2networkMaskStr,
      osapiInet_ntoa(A_GET_4B(lsaPayload)));
  output(context, outBuf);
  lsaPayload += 4;

  memset(optBuf, 0x00, sizeof(optBuf));
  memset(outBuf, 0x00, sizeof(outBuf));
  /* output option flags */
  flags = opt = *(L7_uchar8 *)lsaPayload;
  if(flags)
  {
    sprintf(optBuf, "%s", "(");
    if ((opt & L7_OSPF_LSA_ASE_OPTIONS_EBIT) != 0)
    {
      strncat(optBuf, v2aseLsaOptionsEBitStr, sizeof(optBuf));
      opt &= ~L7_OSPF_LSA_ASE_OPTIONS_EBIT;
      if (opt != 0) 
      {
        strncat(optBuf, " ", sizeof(optBuf));
      }
      else 
      {
        strncat(optBuf, ")", sizeof(optBuf));
      }
    }
  }
  else
  {
    strncat(optBuf,"(None)",sizeof(optBuf));
  }
  if (opt != 0)
    sprintf(optBuf, "%s", ")");

  sprintf(outBuf, "%s:%s\r\n", v2aseLsaOptionsStr, optBuf);
  output(context, outBuf);
  lsaPayload += 1;

  /* output metric */
  if (flags & L7_OSPF_LSA_ASE_OPTIONS_EBIT) {
    isExternalMetric = L7_TRUE;
  }

  sprintf(outBuf, "%s%s\r\n%s:%lu\r\n", v2aseLsaMetricStrType,
    (isExternalMetric == L7_TRUE?": 2":": 1 "),v2aseLsaMetricStr, 
    A_GET_3B((L7_uchar8 *)lsaPayload));

  output(context, outBuf);
  lsaPayload += 3;

  if (lsaPayload < aseLsa+lsaLength)
  {
    sprintf(outBuf, "%s:%s\r\n", v2aseLsaForwAddrStr,osapiInet_ntoa(A_GET_4B(lsaPayload)));
    output(context, outBuf);
    lsaPayload += 4;
  }
  if (lsaPayload < aseLsa+lsaLength)
  {

    sprintf(outBuf, "%s:%lu\r\n", v2aseLsaTagStr, A_GET_4B(lsaPayload));
    output(context, outBuf);
  }

  return L7_SUCCESS;
}

L7_RC_t
ospfUnknownLsaDecode(L7_char8 *unkLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  return L7_SUCCESS;
}

L7_RC_t 
ospfGroupMembershipLsaDecode(L7_char8 *gmLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  return L7_SUCCESS;
}

L7_RC_t
ospfTemp2LsaDecode(L7_char8 *gmLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  return L7_SUCCESS;
}

L7_RC_t
ospfLinkOpaqueLsaDecode(L7_char8 *gmLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  return L7_SUCCESS;
}  

L7_RC_t
ospfAreaOpaqueLsaDecode(L7_char8 *gmLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
    return L7_SUCCESS;
} 

L7_RC_t
ospfASOpaqueLsaDecode(L7_char8 *gmLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
    return L7_SUCCESS;
}

L7_RC_t
ospfAsbrSummaryLsaDecode(L7_char8 *summaryAsbrLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  L7_char8 *lsaPayload ;
  char outBuf[80];

  lsaPayload = summaryAsbrLsa + L7_OSPF_LSA_HDR_LENGTH;

    sprintf(outBuf, " %s: %s\r\n", v2networkMaskStr,
        osapiInet_ntoa(A_GET_4B(lsaPayload)));
    output(context, outBuf);

    sprintf(outBuf, " %s: %lu\r\n",v2metricStr,
        A_GET_3B(lsaPayload+L7_OSPF_LSA_SUMMARY_METRIC_OFFSET));
    output(context, outBuf);


    return L7_SUCCESS;
}  

L7_RC_t
ospfNetworkSummaryLsaDecode(L7_char8 *summaryLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  L7_char8 *lsaPayload ;
  char outBuf[80];

  lsaPayload = summaryLsa + L7_OSPF_LSA_HDR_LENGTH;

  sprintf(outBuf, " %s: %s\r\n", v2networkMaskStr,
      osapiInet_ntoa(A_GET_4B(lsaPayload)));
  output(context, outBuf);

  sprintf(outBuf, " %s: %lu\r\n",v2metricStr, 
      A_GET_3B(lsaPayload+L7_OSPF_LSA_SUMMARY_METRIC_OFFSET));
  output(context, outBuf);

      return L7_SUCCESS;
}



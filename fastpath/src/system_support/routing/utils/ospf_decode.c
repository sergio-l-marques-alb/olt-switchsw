#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "ospf_decode.ext"
#include "osapi_support.h"
#include "sysapi.h"
#include "iptypes.h"

typedef L7_RC_t (*decodeFuncPtr)(L7_char8 *, L7_char8 *, outputFuncPtr, void *, L7_uint32);

decodeFuncPtr decode[OSPFV3_LSA_FC_COUNT] = {
/* 0  */ ospfv3UnknownLsaDecode,
/* 1  */ ospfv3RouterLsaDecode,
/* 2  */ ospfv3NetworkLsaDecode,
/* 3  */ ospfv3InterAreaPrefixLsaDecode,
/* 4  */ ospfv3InterAreaRouterLsaDecode,
/* 5  */ ospfv3ASExternalLsaDecode,
/* 6  */ ospfv3GroupMembershipLsaDecode,
/* 7  */ ospfv3ASExternalLsaDecode,
/* 8  */ ospfv3LinkLsaDecode,
/* 9  */ ospfv3IntraAreaPrefixLsaDecode,
};

typedef struct ospfv3ByteAsBitmaskDecode_s {
  char *beginningStr;
  char *delimiterStr;
  char *terminalStr;
  char *bitStr[8];
  char *noBitSetStr;
} ospfv3ByteAsBitmaskDecode_t;

typedef struct ospfv3BitMask_s {
  L7_uint32     mask[8];
} ospfv3BitMask_t;

ospfv3BitMask_t bitMask =
{ {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}, };

ospfv3ByteAsBitmaskDecode_t lsaOptionsBitsDecode =
{
  "(",
  " ",
  ")",
  {"V6-Bit", "E-Bit", "MC-Bit", "N/P-Bit", "R-Bit", "DC-Bit", "N-Bit", "*-Bit"},
  "None"
};

/* decode bitmask */

/* LSA header output strings */

char *lsaFuncCodeToStr[OSPFV3_LSA_FC_COUNT] = {
/* 0  */ "Unknown",
/* 1  */ "Router-LSA",
/* 2  */ "Network-LSA",
/* 3  */ "Inter-Area-Prefix-LSA",
/* 4  */ "Inter-Area-Router-LSA",
/* 5  */ "AS-External-LSA",
/* 6  */ "Group-membership-LSA",
/* 7  */ "Type-7-LSA",
/* 8  */ "Link-LSA",
/* 9  */ "Intra-Area-Prefix-LSA",
};

char *lsaHdrLsAgeStr = "LS Age";
char *lsaHdrLsTypeStr = "LS Type";
char *lsaHdrLsidStr = "LS Id";
char *lsaHdrAdvRtrStr = "Advertising Router";
char *lsaHdrLsSeqNoStr = "LS Seq Number";
char *lsaHdrCsumStr = "Checksum";
char *lsaHdrLengthStr = "Length";

char *lsaOptionsStr = "LS options";

/* Per-LSA-type output strings */

char *lsaOptV6BitStr = "V6-Bit";
char *lsaOptEBitStr = "E-Bit";
char *lsaOptMCBitStr = "MC-Bit";
char *lsaOptNPBitStr = "N/P-Bit";
char *lsaOptRBitStr = "R-Bit";
char *lsaOptDCBitStr = "DC-Bit";
char *lsaOptNBitStr = "N-Bit";
char *lsaOptExtensionsBitStr = "*-Bit";

/* Router-LSA strings */

char *rtrLsaVBitStr = "Virtual Link Endpoint";
char *rtrLsaEBitStr = "AS Boundary Router";
char *rtrLsaBBitStr = "Area Border Router";
char *rtrLsaWBitStr = "Wildcard Multicast Receiver";
char *rtrLsaNtBitStr = "NSSA Translator";
char *rtrLsaNumLinksStr = "Number of Links";

typedef enum {
  RTR_LSA_LINK_TYPE_UNUSED = 0,
  RTR_LSA_LINK_TYPE_P2P,
  RTR_LSA_LINK_TYPE_TRANSIT_NET,
  RTR_LSA_LINK_TYPE_RESERVED,
  RTR_LSA_LINK_TYPE_VLINK,
  RTR_LSA_LINK_TYPE_COUNT,
} rtrLsaIfType_t;

char *rtrLsaIfType [RTR_LSA_LINK_TYPE_COUNT] =
{
/* 0 */ NULL,
/* 1 */ "another router (point-to-point)",
/* 2 */ "transit network (designated router)",
/* 3 */ "Reserved",
/* 4 */ "another router (virtual link)"
};

char *rtrLsaIfTypeStr = "Link connected to";
char *rtrLsaIfMetricStr = "Link Metric";
char *rtrLsaIfIdStr = "Local Interface ID";
char *rtrLsaIfNeighborIfIdStr = "Neighbor Interface ID";
char *rtrLsaIfNeighborRtrIdStr = "Neighbor Router ID";

/* Link-LSA strings */
char *linkLsaDRPriorityStr = "Router Priority";
char *linkLsaLinkLocalAddrStr = "Link-Local Address";
char *ipv6PrefixStr = "IPv6 Prefix";

/* Link-LSA Prefix Options strings */
char *linkLsaPfxOptionsNUBitStr = "NU-Bit";
char *linkLsaPfxOptionsLABitStr = "LA-Bit";
char *linkLsaPfxOptionsMCBitStr = "MC-Bit";
char *linkLsaPfxOptionsPBitStr = "P-Bit";

/* ipv6 prefix options strings (used by multiple lsa types) */
char *ipv6LsaPfxCountStr = "Number of Prefixes";
char *ipv6LsaPfxLSTypeStr = "Referenced LS Type";
char *ipv6LsaPfxMetricStr = "Metric";

/* IntraAreaPrefix-LSA */
char *intraAreaPrefixLsaLSIdStr = "Referenced LS Id";
char *intraAreaPrefixLsaAdvRtrStr = "Referenced Advertising Router";

/* Inter-Area-Rotuer-LSA */
char *interAreaRouterLsaDestRtrId = "Destination Router ID";

/* AS-External-LSA strings */
char *aseLsaOptionsStr = "Options";
char *aseLsaMetricStr = "Metric";
char *aseLsaMetricStrType="Metric Type";
char *aseLsaForwAddrStr = "Forwarding Address";
char *aseLsaTagStr = "Tag";
char *aseLsaOptionsEBitStr = "E-Bit";
char *aseLsaOptionsFBitStr = "F-Bit";
char *aseLsaOptionsTBitStr = "T-Bit";

ospfv3LsaFuncCode_t 
ospfv3LsaFuncCodeDecode(byte lsType)
{
  ospfv3LsaFuncCode_t fcType = lsType;

  if(! ((fcType > 0) &&
        (fcType != OSPFV3_LSA_FC_GROUP_MEMBERSHIP) &&
        (fcType <= OSPFV3_LSA_FC_INTRAAREA_PREFIX)) )
    fcType = OSPFV3_LSA_FC_UNUSED;

  return fcType;
}

L7_RC_t 
ospfv3LsaDecode(L7_char8 *lsaHdr, L7_char8 *lsaPayload, outputFuncPtr output, 
                void *context)
{
  ospfv3LsaFuncCode_t lsaTypeFC;
  L7_char8 *lsaType;
  L7_uint32 lsaLen;

  assert(lsaHdr != NULL);

  lsaType = lsaHdr + L7_OSPFV3_LSA_HDR_LSTYPE_OFFSETOF;
  lsaTypeFC = (ospfv3LsaFuncCode_t) 
                 ospfv3LsaFuncCodeDecode(A_GET_2B(lsaType) & 
                 L7_OSPFV3_LSA_HDR_LSTYPE_FUNCTION_CODE_MASK);

  ospfv3LsaHeaderDecode(lsaHdr, output, context, &lsaLen);
  
  if ((lsaTypeFC < OSPFV3_LSA_FC_UNUSED || 
      lsaTypeFC > OSPFV3_LSA_FC_INTRAAREA_PREFIX))
  {
    sysapiPrintf("Invalid LSA Function Code %d\n", (int)lsaTypeFC);
    return L7_FAILURE;
  }

  /* invoke lsa-specific decode by indexing into func ptr table */
  return decode[lsaTypeFC](lsaHdr, lsaPayload, output, context, lsaLen);
}

L7_RC_t 
ospfv3LsaHeaderDecode(L7_char8 * lsaHdr, outputFuncPtr output, void * context, L7_uint32 *lsaLength)
{
  char outBuf[128];
  ospfv3LsaFuncCode_t lsaTypeFC;
  L7_ushort16 age;

  age = A_GET_2B((L7_uchar8 *)lsaHdr);
  sprintf(outBuf, "\r\n%s: %hu\r\n", lsaHdrLsAgeStr, age);
  output(context, outBuf);
  lsaHdr += 2;

  lsaTypeFC = (ospfv3LsaFuncCode_t) 
              ospfv3LsaFuncCodeDecode(A_GET_2B((L7_uchar8 *)lsaHdr) &
                L7_OSPFV3_LSA_HDR_LSTYPE_FUNCTION_CODE_MASK);

  if(lsaTypeFC != 0)
  {
    sprintf(outBuf, "%s: %s\r\n", lsaHdrLsTypeStr, lsaFuncCodeToStr[lsaTypeFC]);
  }
  else
  {
    sprintf(outBuf, "%s: 0x%04x\r\n", lsaHdrLsTypeStr, A_GET_2B((L7_uchar8 *)lsaHdr));
  }  
  output(context, outBuf);
  lsaHdr += 2;

  sprintf(outBuf, "%s: %lu\r\n", lsaHdrLsidStr, A_GET_4B((L7_uchar8 *)lsaHdr));
  output(context, outBuf);
  lsaHdr += 4;

  sprintf(outBuf, "%s: %s\r\n", lsaHdrAdvRtrStr, osapiInet_ntoa(A_GET_4B((L7_uchar8 *)lsaHdr)));
  output(context, outBuf);
  lsaHdr += 4;

  sprintf(outBuf, "%s: 0x%08lx\r\n", lsaHdrLsSeqNoStr, A_GET_4B((L7_uchar8 *)lsaHdr));
  output(context, outBuf);
  lsaHdr += 4;

  sprintf(outBuf, "%s: 0x%04x\r\n", lsaHdrCsumStr, (L7_ushort16) A_GET_2B((L7_uchar8 *)lsaHdr));
  output(context, outBuf);
  lsaHdr += 2;

  *lsaLength = A_GET_2B((L7_uchar8 *)lsaHdr);
  sprintf(outBuf, "%s: %u\r\n", lsaHdrLengthStr, *lsaLength);
  output(context, outBuf);

  return L7_SUCCESS;
}

#if     1
L7_RC_t 
ospfv3LsaOptionsDecode(L7_char8 * options, outputFuncPtr output, void * context)
{
  char outBuf[80];
  int  opt = options[2];

  memset(outBuf, 0x00, sizeof(outBuf));
  sprintf(outBuf, "%s: (", lsaOptionsStr);

  opt &= L7_OSPFV3_LSA_OPTIONS_KNOWNBITS_MASK;
  if (opt == 0)
  {
    osapiStrncat(outBuf, "None)\r\n", sizeof(outBuf));
  } 
  else {

    /* this do while is just a kludge to avoid goto */
    do {
      if ((opt & L7_OSPFV3_LSA_OPTIONS_DCBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, lsaOptDCBitStr, sizeof(outBuf));
        opt &= ~L7_OSPFV3_LSA_OPTIONS_DCBIT_MASK;
        if (opt != 0) 
        {
          osapiStrncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((opt & L7_OSPFV3_LSA_OPTIONS_RBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, lsaOptRBitStr, sizeof(outBuf));
        opt &= ~L7_OSPFV3_LSA_OPTIONS_RBIT_MASK;
        if (opt != 0) 
        {
          osapiStrncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((opt & L7_OSPFV3_LSA_OPTIONS_NBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, lsaOptNBitStr, sizeof(outBuf));
        opt &= ~L7_OSPFV3_LSA_OPTIONS_NBIT_MASK;
        if (opt != 0) 
        {
          osapiStrncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((opt & L7_OSPFV3_LSA_OPTIONS_MCBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, lsaOptMCBitStr, sizeof(outBuf));
        opt &= ~L7_OSPFV3_LSA_OPTIONS_MCBIT_MASK;
        if (opt != 0) 
        {
          osapiStrncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((opt & L7_OSPFV3_LSA_OPTIONS_EBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, lsaOptEBitStr, sizeof(outBuf));
        opt &= ~L7_OSPFV3_LSA_OPTIONS_EBIT_MASK;
        if (opt != 0) 
        {
          osapiStrncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((opt & L7_OSPFV3_LSA_OPTIONS_V6BIT_MASK) != 0)
      {
        osapiStrncat(outBuf, lsaOptV6BitStr, sizeof(outBuf));
        opt &= ~L7_OSPFV3_LSA_OPTIONS_V6BIT_MASK;
        if (opt != 0) 
        {
          osapiStrncat(outBuf, " ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, ")\r\n", sizeof(outBuf));
          break;
        }
      }
      /* if some other bit we didn't anticipate is set, we won't get the
       * close parens and newline, so force the issue here */
      if (opt != 0) 
      {
        osapiStrncat(outBuf, ")\r\n", sizeof(outBuf));
      }
    } while (0);
  }
  output(context, outBuf);
  return L7_SUCCESS;
}
#else
L7_RC_t 
ospfv3LsaOptionsDecode(L7_char8 * options, outputFuncPtr output, void * context)
{
  char outBuf[80];
  int  opt = options[2];

  sprintf(outBuf, "%s: %s", 
      lsaOptionsStr, lsaOptionsBitsDecode.beginningStr);

  /* must clear bits not known by CLI for this to work correctly */
  opt &= L7_OSPFV3_LSA_OPTIONS_KNOWNBITS_MASK;
  if (opt == 0)
  {
    osapiStrncat(outBuf, lsaOptionsBitsDecode.noBitSetStr, sizeof(outBuf));
  } 
  else {
    int i;

    /* MSB->LSB */
    for (i = 7; i > 0 && opt; i--) {
      if (opt & bitMask.mask[i])
      {
        if (lsaOptionsBitsDecode.bitStr[i] != NULL)
        {
          osapiStrncat(outBuf, lsaOptionsBitsDecode.bitStr[i], sizeof(outBuf));
        }
        opt &= ~bitMask.mask[i];
        if (!opt)
        {
          break;
        }
      }
    }
  }

  osapiStrncat(outBuf, lsaOptionsBitsDecode.terminalStr, sizeof(outBuf));
  osapiStrncat(outBuf, "\r\n", sizeof(outBuf));
  output(context, outBuf);
  return L7_SUCCESS;
}
#endif

L7_RC_t 
ospfv3RouterLsaDecode(L7_char8 * rtrLsa, L7_char8 *lsaPayload, outputFuncPtr output, 
                      void * context, L7_uint32 lsaLength)
{
  L7_int32 numLinks;
  char outBuf[128];

  numLinks = L7_OSPFV3_LSA_ROUTER_GET_NUM_LINKS(rtrLsa);

  ospfv3RouterLsaBitsDecode(lsaPayload, output, context);
  ospfv3LsaOptionsDecode(lsaPayload + L7_OSPFV3_LSA_OPTIONS_OFFSETOF, output,context);

  sprintf(outBuf, "%s: %u\r\n", rtrLsaNumLinksStr, numLinks);
  output(context, outBuf);

  while (numLinks--)
  {
    ospfv3RouterLsaIfDecode(lsaPayload + 4, output, context);
    lsaPayload += 16;
  }
  return L7_SUCCESS;
}

L7_RC_t 
ospfv3RouterLsaBitsDecode(L7_char8 * rtrLsa, outputFuncPtr output, void * context)
{
  char outBuf[128];
  int bits = *rtrLsa & L7_OSPFV3_LSA_ROUTER_KNOWNBITS_MASK;

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
      if ((bits & L7_OSPFV3_LSA_ROUTER_NTBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, rtrLsaNtBitStr, sizeof(outBuf));
        bits &= ~L7_OSPFV3_LSA_ROUTER_NTBIT_MASK;
        if (bits != 0) 
        {
          osapiStrncat(outBuf, ", ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, "\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((bits & L7_OSPFV3_LSA_ROUTER_WBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, rtrLsaWBitStr, sizeof(outBuf));
        bits &= ~L7_OSPFV3_LSA_ROUTER_WBIT_MASK;
        if (bits != 0) 
        {
          osapiStrncat(outBuf, ", ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, "\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((bits & L7_OSPFV3_LSA_ROUTER_VBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, rtrLsaVBitStr, sizeof(outBuf));
        bits &= ~L7_OSPFV3_LSA_ROUTER_VBIT_MASK;
        if (bits != 0) 
        {
          osapiStrncat(outBuf, ", ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, "\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((bits & L7_OSPFV3_LSA_ROUTER_EBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, rtrLsaEBitStr, sizeof(outBuf));
        bits &= ~L7_OSPFV3_LSA_ROUTER_EBIT_MASK;
        if (bits != 0) 
        {
          osapiStrncat(outBuf, ", ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, "\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((bits & L7_OSPFV3_LSA_OPTIONS_EBIT_MASK) != 0)
      {
        osapiStrncat(outBuf, rtrLsaEBitStr, sizeof(outBuf));
        bits &= ~L7_OSPFV3_LSA_OPTIONS_EBIT_MASK;
        if (bits != 0) 
        {
          osapiStrncat(outBuf, ", ", sizeof(outBuf));
        }
        else 
        {
          osapiStrncat(outBuf, "\r\n", sizeof(outBuf));
          break;
        }
      }
      if ((bits & L7_OSPFV3_LSA_ROUTER_BBIT_MASK) != 0)
      {
         osapiStrncat(outBuf, rtrLsaBBitStr, sizeof(outBuf));
         bits &= ~L7_OSPFV3_LSA_ROUTER_BBIT_MASK;
         if (bits != 0) 
         {
           osapiStrncat(outBuf, ", ", sizeof(outBuf));
         }
         else 
         {
           osapiStrncat(outBuf, "\r\n", sizeof(outBuf));
           break;
         }
      }
      /* if some other bit we didn't anticipate is set, we won't get the
       * close parens and newline, so force the issue here */
      if (bits != 0) 
      {
         osapiStrncat(outBuf, "\r\n", sizeof(outBuf));
      }
    } while (0);
  output(context, outBuf);
  return L7_SUCCESS;
}
/*

 |     Type      |       0       |          Metric                |
 +-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                      Interface ID                              |
 +-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                   Neighbor Interface ID                        |
 +-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                    Neighbor Router ID                          |
 */

L7_RC_t 
ospfv3RouterLsaIfDecode(L7_char8 *rtrLsa, outputFuncPtr output, void * context)
{
  char outBuf[128];
  int ifType;

  output(context, "\r\n");
  ifType = *rtrLsa;
  if ((ifType >= RTR_LSA_LINK_TYPE_COUNT) || (ifType < RTR_LSA_LINK_TYPE_P2P))
  {
    sprintf(outBuf, "  %s: Unknown\r\n", rtrLsaIfTypeStr);
  }
  else
  {
    sprintf(outBuf, "  %s: %s\r\n", rtrLsaIfTypeStr, rtrLsaIfType[ifType]);
    output(context, outBuf);
    rtrLsa += 2;

    sprintf(outBuf, "  %s: %u\r\n", rtrLsaIfMetricStr, A_GET_2B(rtrLsa));
    output(context, outBuf);
    rtrLsa += 2;

    sprintf(outBuf, "  %s: %lu\r\n", rtrLsaIfIdStr, A_GET_4B(rtrLsa));
    output(context, outBuf);
    rtrLsa += 4;

    sprintf(outBuf, "  %s: %lu\r\n", rtrLsaIfNeighborIfIdStr, A_GET_4B(rtrLsa));
    output(context, outBuf);
    rtrLsa += 4;

    sprintf(outBuf, "  %s: %s\r\n", rtrLsaIfNeighborRtrIdStr, 
        osapiInet_ntoa(A_GET_4B(rtrLsa)));
    output(context, outBuf);
    rtrLsa += 4;
  }

  return L7_SUCCESS;

}

L7_RC_t 
ospfv3PrefixLsaDecode(L7_char8 *pfxLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  L7_char8 *lsaPayload, *lsaEndPtr;
  L7_int32 metric;
  char outBuf[128];

  lsaPayload = pfxLsa + L7_OSPFV3_LSA_HDR_LENGTH + 1;

  metric = A_GET_3B(lsaPayload);
  sprintf(outBuf, "%s: %u\r\n", ipv6LsaPfxMetricStr, metric);
  output(context, outBuf);

  lsaPayload += 3;

  lsaEndPtr = pfxLsa + lsaLength;

  while (lsaPayload < lsaEndPtr)
  {
    ospfv3LsaPrefixDecode(&lsaPayload, output, context, 
        OSPFV3_LSA_FC_INTRAAREA_PREFIX);
  }

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
 *    |      0        |              Options                          |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                       Attached Router                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                             ...                               |
 */

L7_RC_t 
ospfv3NetworkLsaDecode(L7_char8 *netLsa, L7_char8 *lsaPayload, 
                       outputFuncPtr output, void *context, L7_uint32 lsaLength)
{
  L7_char8 *lsaEndPtr = lsaPayload + lsaLength - L7_OSPFV3_LSA_HDR_LENGTH;
  L7_char8 *pos = lsaPayload + 1;             /* current location in LSA */
  char outBuf[80];

  ospfv3LsaOptionsDecode(pos, output, context);
  pos += 3;

  while (pos < lsaEndPtr)
  {
    sprintf(outBuf, "%s: %s\r\n", rtrLsaIfNeighborRtrIdStr,
        osapiInet_ntoa(A_GET_4B(pos)));
    output(context, outBuf);
    pos += 4;
  }

  return L7_SUCCESS;
}

L7_RC_t 
ospfv3InterAreaPrefixLsaDecode(L7_char8 *iapLsa, L7_char8 *lsaPayload, 
                               outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  lsaPayload++;

  ospfv3LsaOptionsDecode(lsaPayload, output, context);

  lsaPayload += 3;

  ospfv3LsaPrefixDecode(&lsaPayload, output, context, 
      OSPFV3_LSA_FC_INTRAAREA_PREFIX);

  return L7_SUCCESS;
}

L7_RC_t 
ospfv3InterAreaRouterLsaDecode(L7_char8 *iarLsa, L7_char8 *lsaPayload, 
                               outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  char outBuf[128];

  lsaPayload++;

  ospfv3LsaOptionsDecode(lsaPayload, output, context);

  lsaPayload += 4;

  sprintf(outBuf,"Metric: %d\r\n", (int)A_GET_3B(lsaPayload));
  output(context, outBuf);

  lsaPayload += 3;

  sprintf(outBuf, "%s: %s\r\n", interAreaRouterLsaDestRtrId, 
          osapiInet_ntoa(A_GET_4B(lsaPayload)));

  output(context, outBuf);

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
 *    |         |E|F|T|                Metric                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    | PrefixLength  | PrefixOptions |     Referenced LS Type        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        Address Prefix                         |
 *    |                             ...                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +-                                                             -+
 *    |                                                               |
 *    +-                Forwarding Address (Optional)                -+
 *    |                                                               |
 *    +-                                                             -+
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |              External Route Tag (Optional)                    |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |               Referenced Link State ID (Optional)             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
L7_RC_t 
ospfv3ASExternalLsaDecode(L7_char8 *aseLsa, L7_char8 *lsaPayload, 
                          outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  L7_uchar8 opt, flags;
  char outBuf[128], v6Addr[46];
  char optBuf[80];
  L7_BOOL isExternalMetric = L7_FALSE;

  memset(optBuf, 0x00, sizeof(optBuf));
  memset(outBuf, 0x00, sizeof(outBuf));
  /* output option flags */
  flags = opt = *(L7_uchar8 *)lsaPayload;
  if(flags)
  {
    sprintf(optBuf, "%s", "(");
    if ((opt & L7_OSPFV3_LSA_ASE_OPTIONS_EBIT) != 0)
    {
      osapiStrncat(optBuf, aseLsaOptionsEBitStr, sizeof(optBuf));
      opt &= ~L7_OSPFV3_LSA_ASE_OPTIONS_EBIT;
      if (opt != 0) 
      {
        osapiStrncat(optBuf, " ", sizeof(optBuf));
      }
      else 
      {
        osapiStrncat(optBuf, ")", sizeof(optBuf));
      }
    }
    if ((opt & L7_OSPFV3_LSA_ASE_OPTIONS_FBIT) != 0)
    {
      osapiStrncat(optBuf, aseLsaOptionsFBitStr, sizeof(optBuf));
      opt &= ~L7_OSPFV3_LSA_ASE_OPTIONS_FBIT;
      if (opt != 0) 
      {
        osapiStrncat(optBuf, "  ", sizeof(optBuf));
      }
      else 
      {
        osapiStrncat(optBuf, ")", sizeof(optBuf));
      }
    }
    if ((opt & L7_OSPFV3_LSA_ASE_OPTIONS_TBIT) != 0)
    {
      osapiStrncat(optBuf, aseLsaOptionsTBitStr, sizeof(optBuf));
      opt &= ~L7_OSPFV3_LSA_ASE_OPTIONS_TBIT;
      if (opt != 0) 
      {
        osapiStrncat(optBuf, " ", sizeof(optBuf));
      }
      else 
      {
        osapiStrncat(optBuf, ")", sizeof(optBuf));
      }
      
    }
  }
  else
  {
    osapiStrncat(optBuf,"(None)",sizeof(optBuf));
  }

  sprintf(outBuf, "%s:%s\r\n", aseLsaOptionsStr, optBuf);
  output(context, outBuf);
  lsaPayload += 1;

  /* output metric */
  if (flags & L7_OSPFV3_LSA_ASE_OPTIONS_EBIT) {
    isExternalMetric = L7_TRUE;
  }

  sprintf(outBuf, "%s%s\r\n%s:%lu\r\n", aseLsaMetricStrType,
    (isExternalMetric == L7_TRUE?": 2":": 1 "),aseLsaMetricStr, 
    A_GET_3B((L7_uchar8 *)lsaPayload));

  output(context, outBuf);
  lsaPayload += 3;

  ospfv3LsaPrefixDecode(&lsaPayload, output, context, OSPFV3_LSA_FC_AS_EXTERNAL);

  if (flags & L7_OSPFV3_LSA_ASE_OPTIONS_FBIT) {
    /* output forwarding address */
    sprintf(outBuf, "%s: %s\r\n", aseLsaForwAddrStr,
        osapiInetNtop(L7_AF_INET6, lsaPayload, v6Addr, sizeof(v6Addr)));
    output(context, outBuf);
    lsaPayload += 16;
  }

  if (flags & L7_OSPFV3_LSA_ASE_OPTIONS_TBIT) {
    /* output tag */
    sprintf(outBuf, "%s: %lu\r\n", aseLsaTagStr, A_GET_4B((L7_uchar8 *)lsaPayload));
    output(context, outBuf);
    lsaPayload += 4;
  }

  return L7_SUCCESS;
}

L7_RC_t 
ospfv3Type7LsaDecode(L7_char8 *aseLsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  return L7_SUCCESS;
}

L7_RC_t 
ospfv3ASExtOrType7LsaDecode(L7_char8 *lsa, outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  L7_char8 *lsaPayload;
  /* char outBuf[128]; */

  /* ROBRICE - not used apparently. But assumes LSA header and payload are in one 
   * contiguous buffer, which may not be true. */
  lsaPayload = lsa + L7_OSPFV3_LSA_HDR_LENGTH + 4;
  /* output flags */
  /* output metric */
  ospfv3LsaPrefixDecode(&lsaPayload, output, context, OSPFV3_LSA_FC_AS_EXTERNAL);

  return L7_SUCCESS;
}

L7_RC_t
ospfv3UnknownLsaDecode(L7_char8 *unkLsa, L7_char8 *lsaPayload, outputFuncPtr output, 
                       void * context, L7_uint32 lsaLength)
{
  return L7_SUCCESS;
}

L7_RC_t 
ospfv3GroupMembershipLsaDecode(L7_char8 *gmLsa, L7_char8 *lsaPayload, 
                               outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  return L7_SUCCESS;
}


/*
 *     0                   1                   2                   3
 *     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    | Rtr Priority  |                Options                        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +-                                                             -+
 *    |                                                               |
 *    +-                Link-local Interface Address                 -+
 *    |                                                               |
 *    +-                                                             -+
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         # prefixes                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |  PrefixLength | PrefixOptions |             0                 |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        Address Prefix                         |
 *    |                             ...                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                             ...                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |  PrefixLength | PrefixOptions |             0                 |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        Address Prefix                         |
 *    |                             ...                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
L7_RC_t 
ospfv3LinkLsaDecode(L7_char8 *linkLsa, L7_char8 *lsaPayload, outputFuncPtr output, 
                    void * context, L7_uint32 lsaLength)
{
  L7_int32 numPrefixes;
  char outBuf[128], v6Addr[46];

  sprintf(outBuf, "%s: %u\r\n", linkLsaDRPriorityStr, *lsaPayload);
  output(context, outBuf);
  lsaPayload += 1;

  ospfv3LsaOptionsDecode(lsaPayload, output,context);
  lsaPayload += 3;

  sprintf(outBuf, "%s: %s\r\n", linkLsaLinkLocalAddrStr,
      osapiInetNtop(L7_AF_INET6, lsaPayload, v6Addr, sizeof(v6Addr)));
  output(context, outBuf);
  lsaPayload += 16;

  numPrefixes = A_GET_4B(lsaPayload);
  lsaPayload += 4;

  sprintf(outBuf, "%s: %u\r\n", ipv6LsaPfxCountStr, numPrefixes);
  output(context, outBuf);

  while (numPrefixes--)
  {
    ospfv3LsaPrefixDecode(&lsaPayload, output, context, OSPFV3_LSA_FC_LINK);
  }
  return L7_SUCCESS;
}

L7_RC_t
ospfv3LsaPrefixDecode(L7_char8 **prefix, outputFuncPtr output, void * context, ospfv3LsaFuncCode_t lsaFuncCode)
{
  byte pfxLength;  /* must be unsigned */
  int i;
  size_t pfxBytes;
  char outBuf[128], v6Addr[46], pfx[16];
  char *pfxOptPtr;

  pfxLength = **prefix;

  /* store location of PrefixOptions for later decode */
  pfxOptPtr = *prefix + 1;

  /* skip over prefix options to address prefix */
  *prefix += 4;

  /* convert pfx length in bits to bytes */
  pfxBytes = ((pfxLength + 31) / 32) * 4;

  memcpy(pfx, *prefix, pfxBytes);
  *prefix += pfxBytes;

  /* zero pad prefix */
  for (i = pfxBytes; i < 16; i++) 
  {
    pfx[i] = 0;
  }

  sprintf(outBuf, "%s: %s/%u", ipv6PrefixStr,
      osapiInetNtop(L7_AF_INET6, pfx, v6Addr, sizeof(v6Addr)), pfxLength);
  output(context, outBuf);
  ospfv3LsaPrefixOptionsDecode(pfxOptPtr, output, context);

  switch (lsaFuncCode)
  {
    case OSPFV3_LSA_FC_AS_EXTERNAL:
    case OSPFV3_LSA_FC_TYPE7:
      /* ReferencedLSType;*/
      break;
    case OSPFV3_LSA_FC_INTRAAREA_PREFIX:
      /* Metric;*/
      break;
    default:
      break;
  }

  return L7_SUCCESS;
}

L7_RC_t
ospfv3LsaPrefixOptionsDecode(L7_char8 *linkLsa, outputFuncPtr output, void * context)
{
  char optBuf[80];
  int  opt = *linkLsa;

  sprintf(optBuf, " (");

  opt &= L7_OSPFV3_LSA_LINK_PREFIX_OPTIONS_KNOWNBITS_MASK;
  if (opt == 0)
  {
        osapiStrncat(optBuf, "None)\r\n", sizeof(optBuf));
  } 
  else {

    /* this do while is just a kludge to avoid goto */
    do {
      if ((opt & L7_OSPFV3_LSA_LINK_PREFIX_OPTIONS_PBIT) != 0)
      {
        osapiStrncat(optBuf, linkLsaPfxOptionsPBitStr, sizeof(optBuf));
        opt &= ~L7_OSPFV3_LSA_LINK_PREFIX_OPTIONS_PBIT;
        if (opt != 0) 
        {
          osapiStrncat(optBuf, " ", sizeof(optBuf));
        }
        else 
        {
          osapiStrncat(optBuf, ")\r\n", sizeof(optBuf));
          break;
        }
      }
      if ((opt & L7_OSPFV3_LSA_LINK_PREFIX_OPTIONS_MCBIT) != 0)
      {
        osapiStrncat(optBuf, linkLsaPfxOptionsMCBitStr, sizeof(optBuf));
        opt &= ~L7_OSPFV3_LSA_LINK_PREFIX_OPTIONS_MCBIT;
        if (opt != 0) 
        {
          osapiStrncat(optBuf, " ", sizeof(optBuf));
        }
        else 
        {
          osapiStrncat(optBuf, ")\r\n", sizeof(optBuf));
          break;
        }
      }
      if ((opt & L7_OSPFV3_LSA_LINK_PREFIX_OPTIONS_LABIT) != 0)
      {
        osapiStrncat(optBuf, linkLsaPfxOptionsLABitStr, sizeof(optBuf));
        opt &= ~L7_OSPFV3_LSA_LINK_PREFIX_OPTIONS_LABIT;
        if (opt != 0) 
        {
          osapiStrncat(optBuf, " ", sizeof(optBuf));
        }
        else 
        {
          osapiStrncat(optBuf, ")\r\n", sizeof(optBuf));
          break;
        }
      }
      if ((opt & L7_OSPFV3_LSA_LINK_PREFIX_OPTIONS_NUBIT) != 0)
      {
        osapiStrncat(optBuf, linkLsaPfxOptionsNUBitStr, sizeof(optBuf));
        opt &= ~L7_OSPFV3_LSA_LINK_PREFIX_OPTIONS_NUBIT;
        if (opt != 0) 
        {
          osapiStrncat(optBuf, " ", sizeof(optBuf));
        }
        else 
        {
          osapiStrncat(optBuf, ")\r\n", sizeof(optBuf));
          break;
        }
      }
      /* if some other bit we didn't anticipate is set, we won't get the
       * close parens and newline, so force the issue here */
      if (opt != 0) 
      {
        osapiStrncat(optBuf, ")\r\n", sizeof(optBuf));
      }
    } while (0);
  }
  output(context, optBuf);
  return L7_SUCCESS;
}

L7_RC_t 
ospfv3IntraAreaPrefixLsaDecode(L7_char8 *intraPfxLsa, L7_char8 *lsaPayload, 
                               outputFuncPtr output, void * context, L7_uint32 lsaLength)
{
  L7_char8 *lsaEndPtr;
  L7_char8 *pos = lsaPayload;               /* current position in LSA */
  char outBuf[128];
  ospfv3LsaFuncCode_t lsaTypeFC;
  L7_uint32 lsaData;

  sprintf(outBuf, "%s: %u\r\n", ipv6LsaPfxCountStr, A_GET_2B(pos));
  output(context, outBuf);
  pos += 2;

  lsaTypeFC = (ospfv3LsaFuncCode_t)
    ospfv3LsaFuncCodeDecode(A_GET_2B(pos) &
        L7_OSPFV3_LSA_HDR_LSTYPE_FUNCTION_CODE_MASK);

  sprintf(outBuf, "%s: %s\r\n", ipv6LsaPfxLSTypeStr,
      lsaFuncCodeToStr[lsaTypeFC]);
  output(context, outBuf);
  pos += 2;

  lsaData = A_GET_4B(pos);
  sprintf(outBuf, "%s: %s\r\n", intraAreaPrefixLsaLSIdStr,
      osapiInet_ntoa(lsaData));
  output(context, outBuf);
  pos += 4;

  lsaData = A_GET_4B(pos);
  sprintf(outBuf, "%s: %s\r\n", intraAreaPrefixLsaAdvRtrStr,
      osapiInet_ntoa(lsaData));
  output(context, outBuf);
  pos += 4;

  lsaEndPtr = lsaPayload + lsaLength - L7_OSPFV3_LSA_HDR_LENGTH;

  while (pos < lsaEndPtr)
  {
    ospfv3LsaPrefixDecode(&pos, output, context, 
                          OSPFV3_LSA_FC_INTRAAREA_PREFIX);
  }

  return L7_SUCCESS;
}

void cli(void *context, char *buf)
{
#if     0
  ewsTelnetWrite(context, buf);
#else
  fprintf(stdout, "%s", buf);
#endif
}

void devshell(void * context, char * buf)
{
#if     0
  sysapiPrintf(buf);
#else
  fprintf(stdout, "%s", buf);
#endif
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  dos.c
*
* @purpose   Denial of Service functions for FASTPATH
*
* @component DOSCONTROL
*
* @comments  none
*
* @create    03/28/2005
*
* @author    esmiley
*
* @end
*
**********************************************************************/

#include "dos_api.h"
#include "dos.h"
#include "dtlapi.h"
#include "dos_util.h"
#include "l7_icmp.h"
extern osapiRWLock_t       doSCfgRWLock;

doSCfgData_t  *doSCfgData = L7_NULLPTR;

/*********************************************************************
* @purpose  Determine next sequential queue config interface index
*
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dosIntfIndexGetNext(L7_uint32 intIfNum, L7_uint32 *pNext)
{
  L7_RC_t       rc;

  if (pNext == L7_NULLPTR)
    return L7_FAILURE;

  rc = nimNextValidIntfNumberByType(L7_PHYSICAL_INTF, intIfNum, pNext);

  /* normalize return codes from NIM */
  if (rc != L7_SUCCESS)
    rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
* @purpose  Sets DoS SIP=DIP mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSSIPDIPSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemSIPDIPMode != mode)
    {
       doSCfgData->systemSIPDIPMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSSIPDIPSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system SIP=DIP DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets DoS SMAC=DMAC mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSSMACDMACSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemSMACDMACMode != mode)
    {
       doSCfgData->systemSMACDMACMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSSMACDMACSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system SMAC=DMAC DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DoS First Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSFirstFragModeSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemFirstFragMode != mode)
    {
       doSCfgData->systemFirstFragMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSFirstFragSet(mode, doSCfgData->systemMinTCPHdrLength) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system First Fragment DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DoS Minimum TCP Header Size
*
* @param    minTCPHdrSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSFirstFragSizeSet(L7_uint32 minTCPHdrSize)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemMinTCPHdrLength != minTCPHdrSize)
    {
       doSCfgData->systemMinTCPHdrLength = minTCPHdrSize;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSFirstFragSet(doSCfgData->systemFirstFragMode, minTCPHdrSize) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system First Fragment min TCP Hdr Size to %d\n", minTCPHdrSize);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets DoS TCP Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPFragSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemTCPFragMode != mode)
    {
       doSCfgData->systemTCPFragMode = mode;
       doSCfgData->systemTCPOffsetMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSTCPFragSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system TCP Fragment DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets DoS TCP Offset mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPOffsetSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemTCPOffsetMode != mode)
    {
       doSCfgData->systemTCPOffsetMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSTCPOffsetSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system TCP Offset DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets DoS TCP Flag mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPFlagSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemTCPFlagMode != mode)
    {
       doSCfgData->systemTCPFlagMode = mode;
       doSCfgData->systemTCPFinUrgPshMode = mode;
       doSCfgData->systemTCPSynFinMode = mode;
       doSCfgData->systemTCPSynMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSTCPFlagSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system TCP Flag DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets DoS TCP Flag & Sequence mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPFlagSeqSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemTCPFlagMode != mode)
    {
       doSCfgData->systemTCPFlagMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSTCPFlagSeqSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system TCP Flag & Sequence DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets DoS TCP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPPortSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemL4PortMode != mode)
    {
       doSCfgData->systemL4PortMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSTCPPortSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system TCP Port DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets DoS UDP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSUDPPortSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemUDPPortMode != mode)
    {
       doSCfgData->systemUDPPortMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSUDPPortSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system UDP Port DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets DoS TCP SYN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPSynSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemTCPSynMode != mode)
    {
       doSCfgData->systemTCPSynMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSTCPSynSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system TCP SYN DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets DoS TCP SYN&FIN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPSynFinSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemTCPSynFinMode != mode)
    {
       doSCfgData->systemTCPSynFinMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSTCPSynFinSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system TCP SYN&FIN DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets DoS TCP FIN&URG&PSH mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPFinUrgPshSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemTCPFinUrgPshMode != mode)
    {
       doSCfgData->systemTCPFinUrgPshMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSTCPFinUrgPshSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system TCP FIN&URG&PSH DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets DoS L4 Port Mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSL4PortSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemL4PortMode != mode)
    {
       doSCfgData->systemL4PortMode = mode;
       doSCfgData->systemUDPPortMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSL4PortSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system L4 Port DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets DoS ICMP Mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSICMPModeSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemICMPMode != mode)
    {
       doSCfgData->systemICMPMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSICMPSet(mode, doSCfgData->systemMaxICMPSize) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system ICMP DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets Maximum ICMP Payload Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSICMPSizeSet(L7_uint32 maxICMPSize)
{
    maxICMPSize = maxICMPSize + ICMP_MINLEN; /*adding space for ICMP header*/
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemMaxICMPSize != maxICMPSize)
    {
       doSCfgData->systemMaxICMPSize = maxICMPSize;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSICMPSet(doSCfgData->systemICMPMode, maxICMPSize) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system ICMP DoS Max ICMP Size to %d\n", maxICMPSize);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets Maximum ICMPv6 Payload Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSICMPv6SizeSet(L7_uint32 maxICMPSize)
{
    maxICMPSize = maxICMPSize + ICMP_MINLEN; /*adding space for ICMP header*/
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemMaxICMPv6Size != maxICMPSize)
    {
       doSCfgData->systemMaxICMPv6Size = maxICMPSize;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSICMPv6Set(doSCfgData->systemICMPMode, maxICMPSize) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system ICMP DoS Max ICMP Size to %d\n", maxICMPSize);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets DoS ICMP Fragment Mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSICMPFragModeSet(L7_uint32 mode)
{
    (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (doSCfgData->systemICMPFragMode != mode)
    {
       doSCfgData->systemICMPFragMode = mode;
       doSCfgData->cfgHdr.dataChanged = L7_TRUE;

       if (dtlDoSICMPFragSet(mode) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
                 "Failure setting system ICMP Fragment DoS mode to %d\n", mode);
         (void)osapiWriteLockGive(doSCfgRWLock);
         return L7_FAILURE;
       }
    }
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets DoS SIP=DIP mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSSIPDIPModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemSIPDIPMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS SMAC=DMAC mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSSMACDMACModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemSMACDMACMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS First Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSFirstFragModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemFirstFragMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS Minimum TCP Header Size
*
* @param    minTCPHdrSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSFirstFragSizeGet(L7_uint32 *minTCPHdrSize)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *minTCPHdrSize = doSCfgData->systemMinTCPHdrLength;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS TCP Offset mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPOffsetModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemTCPOffsetMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS TCP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPPortModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemL4PortMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS UDP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSUDPPortModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemUDPPortMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS TCP Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPFragModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemTCPFragMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS TCP Flag mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPFlagModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID) != L7_TRUE)
    {
       *mode = doSCfgData->systemTCPFlagMode;
    }
    else
    {
       if ((doSCfgData->systemTCPFlagMode == L7_ENABLE) && (doSCfgData->systemTCPFinUrgPshMode == L7_ENABLE) &&
           (doSCfgData->systemTCPSynFinMode == L7_ENABLE) && (doSCfgData->systemTCPSynMode == L7_ENABLE))
       {
         *mode = L7_ENABLE;
       }
       else
       {
         *mode = L7_DISABLE;
       }
    }
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS TCP Flag & Sequence mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPFlagSeqModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemTCPFlagMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS TCP SYN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPSynModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemTCPSynMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS TCP SYN&FIN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPSynFinModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemTCPSynFinMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS TCP FIN&URG&PSH mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSTCPFinUrgPshModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemTCPFinUrgPshMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS L4 Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSL4PortModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPPORT_FEATURE_ID) != L7_TRUE)
    {
       *mode = doSCfgData->systemL4PortMode;
    }
    else
    {
       if ((doSCfgData->systemUDPPortMode == L7_ENABLE) && (doSCfgData->systemUDPPortMode == L7_ENABLE))
       {
         *mode = L7_ENABLE;
       }
       else
       {
         *mode = L7_DISABLE;
       }
    }
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS ICMP mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSICMPModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemICMPMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS Maximum ICMP Payload Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSICMPSizeGet(L7_uint32 *maxICMPSize)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *maxICMPSize = doSCfgData->systemMaxICMPSize;
    (void)osapiReadLockGive(doSCfgRWLock);
    *maxICMPSize = *maxICMPSize - ICMP_MINLEN; /*Removing ICMP header space*/
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enables or disables the mode for preventing PING
*           Flooding DOS Attack
*
* @param   L7_uint32 interface (input)   Interface on which mode needs to
*                                        be enabled or disabled
* @param   L7_uint32 mode      (input)   Mode value
* @param   L7_uint32 param     (input)   Param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfPingFloodingSet(L7_uint32 interface, L7_uint32 mode, L7_uint32 param)
{
  dosIntfCfgData_t *pCfg;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;
  (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);

  if (dosMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
  {
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_FAILURE;
  }

  if ((mode == pCfg->intfPingFlooding) && (param == pCfg->intfparams[DOSINTFPINGPARAM]))
  {
    (void)osapiWriteLockGive(doSCfgRWLock);
     return L7_SUCCESS;
  }

  pCfg->intfPingFlooding = mode;
  pCfg->intfparams[DOSINTFPINGPARAM] = param;

  /* Apply the config change */
  if(dosIntfPingFloodingApply(interface, mode,param)!=L7_SUCCESS)
    return L7_FAILURE;

  doSCfgData->cfgHdr.dataChanged = L7_TRUE;
  (void)osapiWriteLockGive(doSCfgRWLock);
  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Gets the PING Flooding mode for an interface
*
* @param   L7_uint32 interface (input)   Interface on which mode needs to
*                                        be fetched
* @param   L7_uint32 *mode     (output)  Mode value
* @param   L7_uint32 *param    (output)  param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfPingFloodingGet(L7_uint32 interface, L7_uint32 *mode , L7_uint32 *param)
{
  dosIntfCfgData_t *pCfg;

  if (dosIsValidIntf(interface) != L7_TRUE)
    return L7_FAILURE;

  (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
  if (dosMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
  {
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_FAILURE;
  }

  *mode = pCfg->intfPingFlooding;
  *param = pCfg->intfparams[DOSINTFPINGPARAM];

  (void)osapiReadLockGive(doSCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Enables or disables the mode for preventing Smurf DOS Attack
*
* @param   L7_uint32 interface (input) Interface on which mode needs to
*                                      be enabled or disabled
* @param   L7_uint32 mode      (input) Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfSmurfAttackSet(L7_uint32 interface, L7_uint32 mode)
{
  dosIntfCfgData_t *pCfg;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;
  (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);

  if (dosMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
  {
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_FAILURE;
  }

  if (mode == pCfg->intfSmurfAttack)
  {
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
  }

  pCfg->intfSmurfAttack = mode;

  /* Apply the config change */
  if(dosIntfSmurfAttackApply(interface, mode)!=L7_SUCCESS)
    return L7_FAILURE;

  doSCfgData->cfgHdr.dataChanged = L7_TRUE;
  (void)osapiWriteLockGive(doSCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Smurf Attack mode for an interface
*
* @param   L7_uint32 interface (input)  Interface on which mode needs to
*                                       be fetched
* @param   L7_uint32 *mode     (output) Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfSmurfAttackGet(L7_uint32 interface, L7_uint32 *mode)
{
  dosIntfCfgData_t *pCfg;

  if (dosIsValidIntf(interface) != L7_TRUE)
    return L7_FAILURE;

  (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
  if (dosMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
  {
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_FAILURE;
  }

  *mode = pCfg->intfSmurfAttack;
  (void)osapiReadLockGive(doSCfgRWLock);

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Enables or disables the mode for preventing SynAck
*           Flooding DOS Attack
*
* @param   L7_uint32 interface (input) Interface on which mode needs to
*                                      enabled or disabled
* @param   L7_uint32 mode      (input) Mode value
* @param   L7_uint32 param     (input) Param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfSynAckFloodingSet(L7_uint32 interface, L7_uint32 mode , L7_uint32 param)
{
  dosIntfCfgData_t *pCfg;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;
  (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);

  if (dosMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
  {
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_FAILURE;
  }

  if ((mode == pCfg->intfSynAckFlooding) && (param == pCfg->intfparams[DOSINTFSYNACKPARAM]))
  {
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
  }

  pCfg->intfSynAckFlooding = mode;
  pCfg->intfparams[DOSINTFSYNACKPARAM] = param;

  /* Apply the config change */
  if(dosIntfSynAckFloodingApply(interface, mode, param)!=L7_SUCCESS)
    return L7_FAILURE;

  doSCfgData->cfgHdr.dataChanged = L7_TRUE;
  (void)osapiWriteLockGive(doSCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enables or disables the mode for preventing SynAck
*           Flooding DOS Attack
*
* @param   L7_uint32 interface (input) Interface on which mode needs to
*                                      enabled or disabled
* @param   L7_uint32 mode      (input) Mode value
* @param   L7_uint32 param     (input) Param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t dosGlobalSynAckFloodingSet(L7_uint32 mode , L7_uint32 param)
{
  L7_uint32     intIfNum;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);

  if ((mode == doSCfgData->synAckFlooding) && (param == doSCfgData->params[DOSINTFSYNACKPARAM]))
  {
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
  }

  (void)osapiWriteLockGive(doSCfgRWLock);

  intIfNum = 0;
  while(dosIntfIndexGetNext(intIfNum, &intIfNum)==L7_SUCCESS)
  {
    if (dosIntfSynAckFloodingSet(intIfNum, mode, param)!=L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
              "Unable to set global DOS SYN/ACK Flooding intf %s\n",
                  ifName);
    }
  }

  (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
  doSCfgData->synAckFlooding = mode;
  doSCfgData->params[DOSINTFSYNACKPARAM] = param;
  doSCfgData->cfgHdr.dataChanged = L7_TRUE;
  (void)osapiWriteLockGive(doSCfgRWLock);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Enables or disables the mode for preventing Smurf
*           DOS Attack
*
* @param   L7_uint32 interface (input) Interface on which mode needs to
*                                      enabled or disabled
* @param   L7_uint32 mode      (input) Mode value
* @param   L7_uint32 param     (input) Param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t dosGlobalSmurfAttackSet(L7_uint32 mode)
{
  L7_uint32    intIfNum;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);

  if (mode == doSCfgData->smurfAttack)
  {
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
  }

  (void)osapiWriteLockGive(doSCfgRWLock);

  intIfNum = 0;
  while(dosIntfIndexGetNext(intIfNum, &intIfNum)==L7_SUCCESS)
  {
    if (dosIntfSmurfAttackSet(intIfNum, mode)!=L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
              "Unable to set global DOS Smurf Attack Config intf %s\n",
                  ifName);
    }
  }

  (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
  doSCfgData->smurfAttack = mode;
  doSCfgData->cfgHdr.dataChanged = L7_TRUE;
  (void)osapiWriteLockGive(doSCfgRWLock);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Enables or disables the mode for preventing ping
*           Flooding DOS Attack
*
* @param   L7_uint32 interface (input) Interface on which mode needs to
*                                      enabled or disabled
* @param   L7_uint32 mode      (input) Mode value
* @param   L7_uint32 param     (input) Param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t dosGlobalPingFloodingSet(L7_uint32 mode, L7_uint32 param)
{
  L7_uint32 intIfNum;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);

  if ((mode == doSCfgData->pingFlooding) &&
      (param == doSCfgData->params[DOSINTFPINGPARAM]))
  {
    (void)osapiWriteLockGive(doSCfgRWLock);
    return L7_SUCCESS;
  }
  (void)osapiWriteLockGive(doSCfgRWLock);


  intIfNum = 0;
  while(dosIntfIndexGetNext(intIfNum, &intIfNum) ==L7_SUCCESS)
  {
    if (dosIntfPingFloodingSet(intIfNum, mode, param)!=L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
              "Unable to set global DOS SYN/ACK Flooding intf %s\n",
                  ifName);
    }
  }

  (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
  doSCfgData->pingFlooding = mode;
  doSCfgData->params[DOSINTFPINGPARAM] = param;
  doSCfgData->cfgHdr.dataChanged = L7_TRUE;
  (void)osapiWriteLockGive(doSCfgRWLock);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Gets the SynAck Flooding mode for an interface
*
* @param   L7_uint32 interface (input)  Interface on which mode needs to
*                                       be fetched
* @param   L7_uint32 *mode     (output) Mode value
* @param   L7_uint32 *param    (output) param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfSynAckFloodingGet(L7_uint32 interface, L7_uint32 *mode ,L7_uint32 *param)
{
  dosIntfCfgData_t *pCfg;

  if (dosIsValidIntf(interface) != L7_TRUE)
    return L7_FAILURE;

  (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
  if (dosMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
  {
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_FAILURE;
  }

  *mode = pCfg->intfSynAckFlooding;
  *param = pCfg->intfparams[DOSINTFSYNACKPARAM];

  (void)osapiReadLockGive(doSCfgRWLock);

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Gets the Smurf Attack mode for an interface
*
* @param   L7_uint32 interface (input)  Interface on which mode needs to
*                                       be fetched
* @param   L7_uint32 *mode     (output) Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosGlobalSmurfAttackGet(L7_uint32 *mode)
{

  (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
  *mode = doSCfgData->smurfAttack;
  (void)osapiReadLockGive(doSCfgRWLock);

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Gets the SynAck Flooding mode for an interface
*
* @param   L7_uint32 interface (input)  Interface on which mode needs to
*                                       be fetched
* @param   L7_uint32 *mode     (output) Mode value
* @param   L7_uint32 *param    (output) param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosGlobalSynAckFloodingGet(L7_uint32 *mode ,L7_uint32 *param)
{

  (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);

  *mode = doSCfgData->synAckFlooding;
  *param = doSCfgData->params[DOSINTFSYNACKPARAM];

  (void)osapiReadLockGive(doSCfgRWLock);

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Gets the PING Flooding mode for an interface
*
* @param   L7_uint32 interface (input)   Interface on which mode needs to
*                                        be fetched
* @param   L7_uint32 *mode     (output)  Mode value
* @param   L7_uint32 *param    (output)  param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosGlobalPingFloodingGet(L7_uint32 *mode , L7_uint32 *param)
{

  (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);

  *mode = doSCfgData->pingFlooding;
  *param = doSCfgData->params[DOSINTFPINGPARAM];

  (void)osapiReadLockGive(doSCfgRWLock);

  return L7_SUCCESS;
}

/********************** DEBUG FUNCTIONS **********************************/

void dosDumpIntfCfg(int unit,int slot, int port)
{
  nimUSP_t usp;
  L7_uint32 interface;
  dosIntfCfgData_t *ppCfg;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;

  if (nimGetIntIfNumFromUSP(&usp,&interface)!= L7_SUCCESS)
  {
    printf("Bad USP\n");
    return;
  }

  if (dosMapIntfIsConfigurable(interface, &ppCfg) != L7_TRUE)
  {
    printf("Could not get pCfg\n");
    return;
  }

  printf("intfPingFlooding = %u\n",ppCfg->intfPingFlooding);
  printf("intfSmurfAttack  = %u\n",ppCfg->intfSmurfAttack);
  printf("intfSynAckFlooding = %u\n",ppCfg->intfSynAckFlooding);

  printf("intfparams[DOSINTFPINGPARAM] = %u\n",ppCfg->intfparams[DOSINTFPINGPARAM]);
  printf("intfparams[DOSINTFSYNACKPARAM] = %u\n",ppCfg->intfparams[DOSINTFSYNACKPARAM]);
}

/* use TYPE as 1,2,3 for ping , smurf , synack */
void dosDebugIntfModeSet(int unit,int slot, int port,int type, int mode, int param)
{
  nimUSP_t usp;
  L7_uint32 interface;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;

  if (nimGetIntIfNumFromUSP(&usp,&interface)!= L7_SUCCESS)
  {
    printf("Bad USP\n");
    return;
  }
  if(mode!=0 && mode!=1)
  {
    printf("Invalid Mode \n");
    return;
  }
  switch(type)
  {
    /* intfPingFlooding */
    case 1:
      dosIntfPingFloodingSet(interface,mode,param);
      break;
   /* intfSmurfAttack */
    case 2:
      dosIntfSmurfAttackSet(interface,mode);
      break;
   /* intfSynAckFlooding */
    case 3:
      dosIntfSynAckFloodingSet(interface,mode,param);
      break;
    default:
      printf("ERROR in Type\n");
  }
}


/*********************************************************************
* @purpose  Gets Maximum ICMP Payload Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSICMPv4SizeGet(L7_uint32 *maxICMPSize)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *maxICMPSize = doSCfgData->systemMaxICMPSize;
    *maxICMPSize = *maxICMPSize - ICMP_MINLEN; /*Removing ICMP header space*/
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets Maximum ICMPv6 Payload Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSICMPv6SizeGet(L7_uint32 *maxICMPSize)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *maxICMPSize = doSCfgData->systemMaxICMPv6Size;
    *maxICMPSize = *maxICMPSize - ICMP_MINLEN; /*Removing ICMP header space*/
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets DoS ICMP Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t doSICMPFragModeGet(L7_uint32 *mode)
{
    (void)osapiReadLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
    *mode = doSCfgData->systemICMPFragMode;
    (void)osapiReadLockGive(doSCfgRWLock);
    return L7_SUCCESS;
}







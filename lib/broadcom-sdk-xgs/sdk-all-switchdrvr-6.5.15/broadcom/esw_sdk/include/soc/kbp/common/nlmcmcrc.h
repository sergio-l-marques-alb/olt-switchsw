/*
 * $Id: nlmcmcrc.h,v 1.2.8.2 2012/12/20 10:43:07 kirankum Exp $
 * $Copyright: (c) 2011 Broadcom Corp.
 * All Rights Reserved.$
 */


 
#ifndef INCLUDED_NLMCM_CRC_H
#define INCLUDED_NLMCM_CRC_H

#ifndef NLMPLATFORM_BCM
#include <nlmcmbasic.h>
#include "nlmcmexterncstart.h"
#else
#include <soc/kbp/common/nlmcmbasic.h>
#include <soc/kbp/common/nlmcmexterncstart.h>
#endif


void
NlmCm__ComputeCRC10Table(void);

nlm_u32
NlmCm__FastCRC10(
    nlm_u8 *data_p,
    nlm_u32 numBytes);

#ifndef NLMPLATFORM_BCM
#include "nlmcmexterncend.h"
#else
#include <soc/kbp/common/nlmcmexterncend.h>
#endif


#endif


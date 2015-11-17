/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  falcon_tsc_functions.c                                        *
 *  Created On    :  29/04/2013                                                    *
 *  Created By    :  Kiran Divakar                                                 *
 *  Description   :  APIs for Serdes IPs                                           *
 *  Revision      :  $Id: falcon_tsc_functions.c 755 2014-10-29 23:33:53Z kirand $ *
 *                                                                                 *
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$                                                           *
 *  No portions of this material may be reproduced in any form without             *
 *  the written permission of:                                                     *
 *      Broadcom Corporation                                                       *
 *      5300 California Avenue                                                     *
 *      Irvine, CA  92617                                                          *
 *                                                                                 *
 *  All information contained in this document is Broadcom Corporation             *
 *  company private proprietary, and trade secret.                                 *
 *                                                                                 *
 ***********************************************************************************
 ***********************************************************************************/

/** @file falcon_tsc_functions.c
 * Implementation of API functions
 */

#ifdef _MSC_VER
/* Enclose all standard headers in a pragma to remove warings for MS compiler */
#pragma warning( push, 0 )
#endif

#ifdef SERDES_API_FLOATING_POINT
#include <math.h>
#include <phymod/phymod.h>
#endif
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#include <phymod/phymod.h>
#include "falcon_tsc_functions.h"

#include "falcon_tsc_field_access_c.h"
#include "falcon_tsc_internal_c.h"
#include "falcon_tsc_dv_functions_c.h"



    #include "falcon_tsc_pll_config_c.h"
#define UINT32_MAX (4294967295UL)



/************************************/
/*  Display Eye Scan                */
/************************************/

/* This is best method for terminal ASCII display */
err_code_t falcon_tsc_display_eye_scan( const phymod_access_t *pa ) {
  int8_t y;
  err_code_t err_code;
  uint16_t status = 0;
  uint32_t stripe[64];

  /* start horizontal acquisition */
  err_code = falcon_tsc_meas_eye_scan_start( pa, EYE_SCAN_HORIZ);
  if (err_code) {
    falcon_tsc_meas_eye_scan_done( pa );
    return (err_code);
  }

  falcon_tsc_display_eye_scan_header( pa, 1);
  /* display stripe */
  for (y = 62;y>=-62;y=y-2) { 
 	err_code = falcon_tsc_read_eye_scan_stripe( pa, &stripe[0], &status);
	if (err_code) {
		falcon_tsc_meas_eye_scan_done( pa );
		return (err_code);
	}

    falcon_tsc_display_eye_scan_stripe( pa, y,&stripe[0]);
	USR_PRINTF(("\n"));  
  }  
  /* stop acquisition */
  err_code = falcon_tsc_meas_eye_scan_done( pa );
  if (err_code) {
	return (err_code);
  }

  falcon_tsc_display_eye_scan_footer( pa, 1);

  return(ERR_CODE_NONE);
}

/* This function is for Falcon and is configured for passive mode */
err_code_t falcon_tsc_meas_lowber_eye( const phymod_access_t *pa, const struct falcon_tsc_eyescan_options_st eyescan_options, uint32_t *buffer) {
  int8_t y,x;
  int16_t i;
  uint16_t status;
  uint32_t errors = 0;
  uint16_t timeout;
  uint8_t stopped_state;

  if(!buffer) {
	  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  i = 0;
  ESTM(stopped_state = rdv_usr_sts_micro_stopped());

  timeout = eyescan_options.timeout_in_milliseconds;
  wrcv_diag_max_time_control((uint8_t)timeout);
 
  wrv_usr_diag_mode(eyescan_options.mode);

  USR_PRINTF(("Calculating\n"));
  for (y = eyescan_options.vert_max;y>=eyescan_options.vert_min;y=y-eyescan_options.vstep) {   
	for (x=eyescan_options.horz_min;x<=eyescan_options.horz_max;x=x+eyescan_options.hstep) {
	  /* acquire sample */
        EFUN(falcon_tsc_pmd_uc_cmd_with_data( pa, CMD_DIAG_EN,CMD_UC_DIAG_GET_EYE_SAMPLE,((uint16_t)x)<<8 | (uint8_t)y,200));
        /* wait for sample complete */
#if 0
        do {
            falcon_tsc_delay_us(1000);
            ESTM(status=rdv_usr_diag_status());
            USR_PRINTF(("status=%04x\n",status));
        } while((status & 0x8000) == 0); 
#else
        EFUN(falcon_tsc_poll_diag_done( pa, &status, (((uint32_t)timeout)<<7)*10 + 20000));
#endif
        EFUN(falcon_tsc_prbs_err_count_ll( pa, &errors));
        /* USR_PRINTF(("(%d,%d) = %u\n",x,y,errors & 0x7FFFFFF)); */

        buffer[i] = errors & 0x7FFFFFFF;
     
        i++;
        USR_PRINTF(("."));
    }
    USR_PRINTF(("\n"));        
  }
  USR_PRINTF(("\n"));
  EFUN(falcon_tsc_meas_eye_scan_done( pa ));
  EFUN(wrv_usr_sts_micro_stopped(stopped_state));
  return(ERR_CODE_NONE);
}


/* Display the LOW BER EyeScan */
err_code_t falcon_tsc_display_lowber_eye( const phymod_access_t *pa, const struct falcon_tsc_eyescan_options_st eyescan_options, uint32_t *buffer) {
    int8_t x,y,i,z;
    int16_t j; /* buffer pointer */
    uint32_t val;
    uint8_t overflow;
    uint32_t limits[13]; /* allows upto 400 sec test time per point (1e-13 BER @ 10G) */
    int8_t data_thresh; 

    if(!buffer) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
     }
    ESTM(data_thresh = rd_rx_data_thresh_sel());
    /* Calculate initial total bitcount BER 1e-1 */
    limits[0] = _mult_with_overflow_check(eyescan_options.linerate_in_khz/10, eyescan_options.timeout_in_milliseconds, &overflow);
    if(overflow > 0) {
        limits[0] = UINT32_MAX;
        USR_PRINTF(("Very long timout_in_milliseconds results in saturation of Err counter can cause in accurate results\n"));
    }

    for(i=1;i<13;i++) {            /* calculate thresholds */
        limits[i] = limits[i-1]/10;
    }
    
    falcon_tsc_display_eye_scan_header( pa, 1);
	j = 0;
    for (y = eyescan_options.vert_max;y>=eyescan_options.vert_min;y=y-eyescan_options.vstep) {
        ESTM(USR_PRINTF(("%6dmV : ",_ladder_setting_to_mV(y,0)-_ladder_setting_to_mV(data_thresh,0))));
	  for(z=-31;z<eyescan_options.horz_min;z++) {
              USR_PRINTF((" ")); /* add leading spaces if any */
	  }
      for (x=eyescan_options.horz_min;x<=eyescan_options.horz_max;x=x+eyescan_options.hstep) {
/*        val = float8_to_int32(buffer[j); */
		val = buffer[j];

        for (i=0;i<13;i++) {
          if ((val != 0) & ((val>=limits[i]) | (limits[i] == 0))) {
		    for(z=1;z<=eyescan_options.hstep;z++) {
				if(z==1) {
				  if(i<=8) {
					 USR_PRINTF(("%c", '1'+i));
				  } else {
					  USR_PRINTF(("%c", 'A'+i-9));
				  }
                                }
				else {
			          USR_PRINTF((" "));
                                }
		    }
		    break;
		  }
	    }
        if (i==13) {
	      for(z=1;z<=eyescan_options.hstep;z++) {
			  if(z==1) {
				if ((x%5)==0 && (y%5)==0) {
				   USR_PRINTF(("+"));
                                }
				else if ((x%5)!=0 && (y%5)==0) {
				   USR_PRINTF(("-"));
                                }
				else if ((x%5)==0 && (y%5)!=0) {
				   USR_PRINTF((":"));
                                }
				else {
				   USR_PRINTF((" "));
                                }
                          }
			  else {
				USR_PRINTF((" "));
                          }
              }
        }
        j++;
     }
     USR_PRINTF(("\n"));  
   }  
   falcon_tsc_display_eye_scan_footer( pa, 1);
   return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_meas_eye_scan_start( const phymod_access_t *pa, uint8_t direction) {
  err_code_t err_code;
  uint8_t lock;
  ESTM(lock = rd_pmd_rx_lock());
  if(lock == 0) {
      USR_PRINTF(("Error: No PMD_RX_LOCK on lane requesting 2D eye scan\n"));
      return(ERR_CODE_DIAG_SCAN_NOT_COMPLETE);
  }
  if(direction == EYE_SCAN_VERTICAL) {
	err_code = falcon_tsc_pmd_uc_diag_cmd( pa, CMD_UC_DIAG_START_VSCAN_EYE,200);
  } else {
	  err_code = falcon_tsc_pmd_uc_diag_cmd( pa, CMD_UC_DIAG_START_HSCAN_EYE,200);
  }
  	if (err_code) {
		return (err_code);
	}

  return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_read_eye_scan_stripe( const phymod_access_t *pa, uint32_t *buffer,uint16_t *status){
	int8_t i;
	uint32_t val[2] = {0,0};
    err_code_t err_code;
	uint16_t sts = 0;

	if(!buffer || !status) {
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
	}
	*status = 0;
	for(i=0;i<32;i++) {
		err_code = falcon_tsc_poll_diag_eye_data(pa,&val[0], &sts, 200);
 		*status |= sts & 0xF000;
		if (err_code) {
			return (err_code);
		}
		buffer[i*2]     = val[0];
		buffer[(i*2)+1] = val[1];
	}
	*status |= sts & 0x00FF;
    return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_display_eye_scan_stripe( const phymod_access_t *pa, int8_t y,uint32_t *buffer) {

  const uint32_t limits[7] = {1835008, 183501, 18350, 1835, 184, 18, 2};

  int8_t x,i;
  int16_t level;

  /* ESTM(data_thresh = rd_rx_data_thresh_sel()); */
  level = _ladder_setting_to_mV(y,0);

  	if(!buffer) {
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
	}

    USR_PRINTF(("%6dmV : ",level));
 
    for (x=-31;x<32;x++) {
	  for (i=0;i<7;i++) 
		if (buffer[x+31]>=limits[i]) {
			USR_PRINTF(("%c", '0'+i+1));
			break;
		}
		if (i==7) {
			if ((x%5)==0 && (y%5)==0) {
				USR_PRINTF(("+"));
                        }
			else if ((x%5)!=0 && (y%5)==0) {
				USR_PRINTF(("-"));
                        }
			else if ((x%5)==0 && (y%5)!=0) {
				USR_PRINTF((":"));
                        }
			else {
				USR_PRINTF((" "));
                        }
		}
	}
	return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_display_eye_scan_header( const phymod_access_t *pa, int8_t i) {
int8_t x;
	USR_PRINTF(("\n"));
	for(x=1;x<=i;x++) {
		USR_PRINTF(("  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30"));
	}
	USR_PRINTF(("\n"));
	for(x=1;x<=i;x++) {
		USR_PRINTF(("         : -|----|----|----|----|----|----|----|----|----|----|----|----|-"));
	}
	USR_PRINTF(("\n"));
	return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_display_eye_scan_footer( const phymod_access_t *pa, int8_t i) {
int8_t x;
	for(x=1;x<=i;x++) {
		USR_PRINTF(("         : -|----|----|----|----|----|----|----|----|----|----|----|----|-"));
	}
	USR_PRINTF(("\n"));
	for(x=1;x<=i;x++) {
		USR_PRINTF(("  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30"));
	}
	USR_PRINTF(("\n"));
	return(ERR_CODE_NONE);
}


/*eye_scan_status_t falcon_tsc_read_eye_scan_status() */
err_code_t falcon_tsc_read_eye_scan_status( const phymod_access_t *pa, uint16_t *status) {

   if(!status) {
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
	}

   ESTM(*status=rdv_usr_diag_status());	

	return(ERR_CODE_NONE);
}


err_code_t falcon_tsc_meas_eye_scan_done( const phymod_access_t *pa ) {
  err_code_t err_code;
  err_code = falcon_tsc_pmd_uc_diag_cmd( pa, CMD_UC_DIAG_DISABLE,200);
  if (err_code) return(err_code);
  return(ERR_CODE_NONE);
}


err_code_t falcon_tsc_start_ber_scan_test( const phymod_access_t *pa, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control) {
    uint8_t lock,sts;
    ESTM(lock = rd_pmd_rx_lock());
    if(lock == 0) {
        USR_PRINTF(("Error: No PMD_RX_LOCK on lane requesting BER scan\n"));
        return(ERR_CODE_DIAG_SCAN_NOT_COMPLETE);
    }
    ESTM(sts =rdv_usr_sts_micro_stopped());
    if(sts > 1) {
       USR_PRINTF(("Error: Lane is busy (%d) requesting BER scan\n",sts));
        return(ERR_CODE_DIAG_SCAN_NOT_COMPLETE);
    }

    wrcv_diag_max_time_control(timer_control);
    wrcv_diag_max_err_control(max_error_control);
    ESTM(falcon_tsc_pmd_uc_cmd( pa, CMD_CAPTURE_BER_START, ber_scan_mode,500));
    return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_read_ber_scan_data( const phymod_access_t *pa, uint32_t *errors, uint32_t *timer_values, uint8_t *cnt, uint32_t timeout) {
    err_code_t err_code;
    uint8_t i,prbs_byte,prbs_multi,time_byte,time_multi;
    uint16_t sts,dataword;

    if(!errors || !timer_values || !cnt) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    /* init data arrays */
    for(i=0;i< DIAG_MAX_SAMPLES;i++) {
        errors[i]=0;
        timer_values[i]=0;
    }
	/* Check for completion read ln.diag_status byte?*/
 ESTM(sts = rdv_usr_diag_status());
	if((sts & 0x8000) == 0) {
		return(_error(ERR_CODE_DATA_NOTAVAIL));
	}
	*cnt = (sts & 0x00FF)/3;
	for(i=0;i < *cnt;i++) {
		/* Read 2 bytes of data */
		err_code = falcon_tsc_pmd_uc_cmd( pa,  CMD_READ_DIAG_DATA_WORD, 0, timeout);
		if(err_code) return(err_code);
		ESTM(dataword = rd_uc_dsc_data());           /* LSB contains 2 -4bit nibbles */
		time_byte = (uint8_t)(dataword>>8);    /* MSB is time byte */
		prbs_multi = (uint8_t)dataword & 0x0F; /* split nibbles */
		time_multi = (uint8_t)dataword>>4;
		/* Read 1 bytes of data */
		err_code = falcon_tsc_pmd_uc_cmd( pa,  CMD_READ_DIAG_DATA_BYTE, 0, timeout);
		if(err_code) return(err_code);
		ESTM(prbs_byte = (uint8_t)rd_uc_dsc_data());
		errors[i] = _float12_to_uint32( pa, prbs_byte,prbs_multi); /* convert 12bits to uint32 */
		timer_values[i] = (_float12_to_uint32( pa, time_byte,time_multi)<<3);
		/*USR_PRINTF(("Err=%d (%02x<<%d); Time=%d (%02x<<%d)\n",errors[i],prbs_byte,prbs_multi,timer_values[i],time_byte,time_multi<<3));*/
		/*if(timer_values[i] == 0 && errors[i] == 0) break;*/
	}

  return(ERR_CODE_NONE);
}


/* This is good example function to do BER extrapolation */
err_code_t falcon_tsc_eye_margin_proj( const phymod_access_t *pa, USR_DOUBLE rate, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control) {
    uint32_t errs[DIAG_MAX_SAMPLES];
	uint32_t time[DIAG_MAX_SAMPLES];
	uint8_t i,cnt;
	uint16_t sts;
    uint8_t verbose = 0;
    int16_t offset_start;

    for(i=0;i<DIAG_MAX_SAMPLES;i++) {
		errs[i]=0;
		time[i]=0;
	}
	/* start UC acquisition */
	if(verbose > 2) USR_PRINTF(("start begin\n"));
	EFUN(falcon_tsc_start_ber_scan_test( pa, ber_scan_mode, timer_control, max_error_control));
    ESTM(offset_start = rd_uc_dsc_data());
	if(verbose > 2) USR_PRINTF(("offset_start = %d:%dmV\n",offset_start,_ladder_setting_to_mV((int8_t)offset_start,0)));
	if(verbose > 2) USR_PRINTF(("start done\n"));

	/* This wait is VERY LONG and should be replaced with interupt or something */
	if(verbose > 5) {
		do {
			EFUN(falcon_tsc_delay_us(2000000));
            ESTM(sts = rdv_usr_diag_status());
			USR_PRINTF(("sts=%04x\n",sts));
			
		} while ((sts & 0x8000) == 0);
	} else {
		USR_PRINTF(("Waiting for measurement time\n"));
        EFUN(falcon_tsc_poll_diag_done( pa, &sts,timer_control*2000));
	}
	if(verbose > 2) USR_PRINTF(("delay done\n"));

	EFUN(falcon_tsc_read_ber_scan_data( pa,  &errs[0], &time[0], &cnt, 2000));
	
	if(verbose > 2) USR_PRINTF(("read done cnt=%d\n",cnt));

	EFUN(falcon_tsc_pmd_uc_cmd( pa, CMD_CAPTURE_BER_END,0x00,200));

	if(verbose > 2) USR_PRINTF(("end function done\n"));
	/* if(cnt == 1) {*/
	/*	USR_PRINTF(("Not enough points found to extrapolate BER\n"));*/
	/*	return(ERR_CODE_NONE);*/
	/*} */

   	EFUN(falcon_tsc_display_ber_scan_data(rate, ber_scan_mode, &errs[0], &time[0],(uint8_t)_abs(offset_start)));
	
	if(verbose > 2) USR_PRINTF(("display done\n"));

	return(ERR_CODE_NONE);
}



err_code_t falcon_tsc_display_ber_scan_data (USR_DOUBLE rate, uint8_t ber_scan_mode, uint32_t *total_errs, uint32_t *total_time, uint8_t max_offset) {
#ifdef SERDES_API_FLOATING_POINT
	/* 'margins_mv[]' vector maps the p1 threshold code with actual mV
		Only relevant when mode=0
		This is not totally linear: for code 0~25 step=6mV; code 25~30 step=18mV; code 30~31 step=12
		'margins_mv[]' is valid only for Merlin. This vector would need to be modified accordingly for different Serdes
	USR_DOUBLE margins_mv[] = {0,6,12,18,24,30,36,42,48,54,60,66,72,78,84,
	                       90,96,102,108,114,120,126,132,138,144,150,168,186,204,222,240,252};
	const USR_DOUBLE narrow_margins_mv[] = {0,3.6,7.2,10.8,14.4,18,21.6,25.5,28.8,32.4,36,39.6,43.2,46.8,50.4,
	                                    54,57.6,61.2,64.8,68.4,72,75.6,79.2,82.8,86.4,90,100.8,111.6,122.4,133.2,144,151.2}; */
    const unsigned int HI_CONFIDENCE_ERR_CNT = 100;      /* bit errors */
    const unsigned int HI_CONFIDENCE_MIN_ERR_CNT = 20;   /* bit errors */
    const int MIN_BER_TO_REPORT = -24;   /* we clip the projected BER using this number */
	const USR_DOUBLE MIN_BER_FOR_FIT = -8.0;	/*	all points with BER <= 10^MIN_BER_FOR_FIT will be used for line fit (i.e used for extrapolation) */
	const int8_t verbose = 1;	/* set verbosity to 1 for normal API operation */

    /* BER confidence scale */
    const USR_DOUBLE ber_conf_scale[104] = { 
		2.9957,5.5717,3.6123,2.9224,2.5604,2.3337,2.1765,2.0604,1.9704,1.8983,
		1.8391,1.7893,1.7468,1.7100,1.6778,1.6494,1.6239,1.6011,1.5804,1.5616,
		1.5444,1.5286,1.5140,1.5005,1.4879,1.4762,1.4652,1.4550,1.4453,1.4362,
		1.4276,1.4194,1.4117,1.4044,1.3974,1.3908,1.3844,1.3784,1.3726,1.3670,
		1.3617,1.3566,1.3517,1.3470,1.3425,1.3381,1.3339,1.3298,1.3259,1.3221,
		1.3184,1.3148,1.3114,1.3080,1.3048,1.3016,1.2986,1.2956,1.2927,1.2899,
		1.2872,1.2845,1.2820,1.2794,1.2770,1.2746,1.2722,1.2700,1.2677,1.2656,
		1.2634,1.2614,1.2593,1.2573,1.2554,1.2535,1.2516,1.2498,1.2481,1.2463,
		1.2446,1.2429,1.2413,1.2397,1.2381,1.2365,1.2350,1.2335,1.2320,1.2306,
		1.2292,1.2278,1.2264,1.2251,1.2238,1.2225,1.2212,1.2199,1.2187,1.2175,	
		1.2163,	/* starts in index: 100 for #errors: 100,200,300,400 */
		1.1486,	/* 200 */
		1.1198,	/* 300 */
		1.1030};	/*400 */


    /* Define variables */
	USR_DOUBLE lbers[DIAG_MAX_SAMPLES];				/* Internal linear scale sqrt(-log(ber)) */
	USR_DOUBLE margins[DIAG_MAX_SAMPLES];				/* Eye margin @ each measurement */
	USR_DOUBLE bers[DIAG_MAX_SAMPLES];				/* computed bit error rate */
	uint32_t i;
	int8_t offset[DIAG_MAX_SAMPLES];
	int8_t mono_flags[DIAG_MAX_SAMPLES];
	
	int8_t direction;
	int8_t delta_n;
	USR_DOUBLE Exy = 0.0;
	USR_DOUBLE Eyy = 0.0;
	USR_DOUBLE Exx = 0.0;
	USR_DOUBLE Ey  = 0.0;
	USR_DOUBLE Ex  = 0.0;
	USR_DOUBLE alpha;
	USR_DOUBLE beta;
	USR_DOUBLE proj_ber, proj_ber_aux;
	USR_DOUBLE proj_margin_12;
	USR_DOUBLE proj_margin_15;
	USR_DOUBLE proj_margin_18;
	USR_DOUBLE sq_err1, sq_err2;
	USR_DOUBLE ierr;
	uint8_t start_n;
	uint8_t stop_n;
	uint8_t low_confidence;
	uint8_t loop_index;
	uint8_t n_mono = 0;
	uint8_t eye_cnt = 1;
	uint8_t hi_confidence_cnt = 0;
	uint8_t heye;
	int8_t first_good_ber_idx = -1;
	int8_t first_small_errcnt_idx = -1;
	uint8_t range250;
	uint8_t intrusive;
	uint8_t ber_clipped=0;
	uint8_t last_point_discard;
	
	uint8_t fit_count;	
	USR_DOUBLE sq_r, alpha2;	/* added for v02 */

	if(!total_errs || !total_time ) {
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
	}


	/* Initialize BER array */
    for (i = 0; i < DIAG_MAX_SAMPLES; i++) {
        bers[i] = 0;
        mono_flags[i] = 0;
	}
	/* Decode mode/direction */
    heye = (ber_scan_mode & DIAG_BER_HORZ)>>1;
	direction = (ber_scan_mode & DIAG_BER_NEG) ? -1 : 1 ;
	range250 = (ber_scan_mode & DIAG_BER_P1_NARROW) ? 0 : 1;
	intrusive = (ber_scan_mode & DIAG_BER_INTR) ? 1 : 0;

    if (heye == 1) {
 		if (verbose > 0) {
			if (direction==-1) USR_PRINTF(("\n\n********** HORIZONTAL PROJECTION: LEFT SIDE ******************\n"));
			if (direction==1) USR_PRINTF(("\n\n********** HORIZONTAL PROJECTION: RIGHT SIDE *****************\n"));
		}
    } else {
		if (verbose > 0) {
			if (direction==-1) USR_PRINTF(("\n\n********** VERTICAL PROJECTION: BOTTOM ***********************\n"));
			if (direction==1) USR_PRINTF(("\n\n********** VERTICAL PROJECTION: TOP **************************\n"));
		}
    }

	/* *******************************************
		* Generate margins[]
		* Generate ber[]
		* Printout data
		* Find first and last points for linear fit 
		******************************************* */
	i=0;
	do {
 		if(heye == 1) {
			offset[i] = (int8_t)(max_offset-i);
			margins[i] = direction*offset[i]*1000.0/64.0;
		} else {
			offset[i] = (int8_t)(max_offset-i);
			if(intrusive) {
				margins[i] = direction*_ladder_setting_to_mV(offset[i], 0);
			} else {
				margins[i] = direction*_ladder_setting_to_mV(offset[i], range250);
			}
		}
        if (total_errs[i] == 0) {
            bers[i] = 1.0/(((USR_DOUBLE)total_time[i])*0.00001*rate);
        } else {
            bers[i] = (USR_DOUBLE)total_errs[i]/(((USR_DOUBLE)total_time[i])*0.00001*rate);
        }

        /* Find the first data point with good BER (1e-8 or better) */
        if ((log10(bers[i]) <= MIN_BER_FOR_FIT) && (first_good_ber_idx == -1)) {
            first_good_ber_idx = (int8_t)i;
        }

        /* Determine high-confidence iterations */
        if (total_errs[i] >= HI_CONFIDENCE_ERR_CNT) {
            hi_confidence_cnt++;
        } else if ((total_errs[i] < HI_CONFIDENCE_MIN_ERR_CNT) && (first_small_errcnt_idx == -1)) {
            /* find the first data point with small error count */
            first_small_errcnt_idx = (int8_t)i;
        }
      i++;

	} while(((total_errs[i] != 0) || (total_time[i] != 0)) && (i<=max_offset));
	
	
	eye_cnt = (int8_t) i;
	
	/* Setting up stop_n variable. Check if the very last point's measurement time was "too short"  */
	i = eye_cnt - 1;
	if ((total_time[i] >= 0.5*total_time[i-1]) || (total_errs[i] >= HI_CONFIDENCE_MIN_ERR_CNT) ){
		stop_n = eye_cnt;	/* last point will be included in linear fit */
		last_point_discard = 0;
	} else {
		stop_n = eye_cnt - 1;	/* discards the very last point */
		last_point_discard = 1;
	}


	/* Print on screen */
	if (verbose > 0) {
            i = 0;
		do {
			if (total_errs[i] == 0) {
				if (heye == 1) {
					USR_PRINTF(("BER @ %4.0f mUI < 1e%6.2f (%u errors in %0.2f sec)\n", 
							margins[i], log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
				} else {
					USR_PRINTF(("BER @ %4.0f mV < 1e%6.2f (%u errors in %0.2f sec)\n", 
							margins[i], log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
				}
			} else {
				if (heye == 1) {
					USR_PRINTF(("BER @ %4.0f mUI = 1e%6.2f (%u errors in %0.2f sec)\n", 
							margins[i], log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
				} else {
					USR_PRINTF(("BER @ %4.0f mV = 1e%6.2f (%u errors in %0.2f sec)\n", 
									margins[i], log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
				}
			}
                        i++;
		} while (i<stop_n);
	}


    /* Assign highest data point to use */
    if (first_good_ber_idx == -1) {
        start_n = stop_n;
    } else {
        start_n = first_good_ber_idx;
    }

    if (start_n >= eye_cnt) {
        proj_ber = log10(bers[eye_cnt-1]);
        proj_margin_12 = -100.0;
        proj_margin_15 = -100.0;
        proj_margin_18 = -100.0;

		/* No need to print out the decimal portion of the BER */
		USR_PRINTF(("BER *worse* than 1e%d\n", (int)proj_ber));
		USR_PRINTF(("Negative margin @ 1e-12, 1e-15 & 1e-18\n"));
		
    } else {
	
		/* 	Correcting *all* BER values using confidence factors in 'ber_conf_scale' vector
			This step is done for extrapolation purposes */
		for (loop_index=0; loop_index < eye_cnt; loop_index++) {
			if (total_errs[loop_index] <= 100) {
				bers[loop_index] = ber_conf_scale[total_errs[loop_index]] * bers[loop_index];
			} else if (total_errs[loop_index] > 100 && total_errs[loop_index] < 200) {
				bers[loop_index] = ber_conf_scale[100] * bers[loop_index];
			} else if (total_errs[loop_index] >= 200 && total_errs[loop_index] < 300) {
				bers[loop_index] = ber_conf_scale[101] * bers[loop_index];
			} else if (total_errs[loop_index] >= 300 && total_errs[loop_index] < 400) {
				bers[loop_index] = ber_conf_scale[102] * bers[loop_index];
			} else if (total_errs[loop_index] >= 400) {
				bers[loop_index] = ber_conf_scale[103] * bers[loop_index];
			}
		}

		/* Detect and record nonmonotonic data points */
		for (loop_index=0; loop_index < stop_n; loop_index++) {
			if ((loop_index > start_n) && (log10(bers[loop_index]) > log10(bers[loop_index-1]))) {
				mono_flags[loop_index] = 1;
				if (first_good_ber_idx != -1) {
					n_mono++;
				}
			}
		}
		
		/* Code below assumes statistically valid points available */
		delta_n = (stop_n-start_n-n_mono);

		/* Computes the "linearised" ber vector */
		for (loop_index=0; loop_index<eye_cnt; loop_index++) {
			lbers[loop_index] = (USR_DOUBLE)sqrt(-log10(bers[loop_index]));
		}

		/* Compute covariances and means */
		fit_count = 0;
		for (loop_index=start_n; loop_index < stop_n; loop_index++) {
			if (mono_flags[loop_index] == 0) {
				Exy += (margins[loop_index]*lbers[loop_index]/(USR_DOUBLE)delta_n);
				Eyy += (lbers[loop_index]*lbers[loop_index]/(USR_DOUBLE)delta_n);
				Exx += (margins[loop_index]*margins[loop_index]/(USR_DOUBLE)delta_n);
				Ey  += (lbers[loop_index]/(USR_DOUBLE)delta_n);
				Ex  += (margins[loop_index]/(USR_DOUBLE)delta_n);
				fit_count++;	/* added for v02 */
			}
		}

		/*	HIGH CONFIDENCE case */
		
		if (fit_count >= 2) {	/* there are at least 2 points to trace a line */
		
			low_confidence = 0;

			/* Compute fit slope and offset: ber = alpha*margin + beta */
			alpha = (Exy - Ey*Ex) / (Exx - Ex*Ex);
			beta = Ey - Ex*alpha;
			/* Compute alpha2: slope of regression: margin = alpha2*ber + beta2 */
			alpha2 = (Exy - Ey*Ex) / (Eyy - Ey*Ey);
			/* Compute correlation index sq_r */
			sq_r = alpha*alpha2;

			proj_ber = pow(10,(-beta*beta));
			proj_margin_12 = (sqrt(-log10(1e-12))-beta)/alpha;
			proj_margin_15 = (sqrt(-log10(1e-15))-beta)/alpha;
			proj_margin_18 = (sqrt(-log10(1e-18))-beta)/alpha;

			sq_err1 = (Eyy + (beta*beta) + (Exx*alpha*alpha) - 
					   (2*Ey*beta) - (2*Exy*alpha) + (2*Ex*beta*alpha));
			sq_err2 = 0;
			for (loop_index=start_n; loop_index<stop_n; loop_index++) {
				ierr = (lbers[loop_index] - (alpha*margins[loop_index] + beta));
				sq_err2 += (ierr*ierr/(USR_DOUBLE)delta_n);
			}
		} 
			
		/*	LOW CONFIDENCE case */
		
		else {	/*	NEW APPROACH (08/28/2014): consider very first point and very last point for linear fit. This will give pessimistic/conservative extrapolation */
		
			low_confidence = 1;
			delta_n = 2;	/* 'delta_n' and 'fit_count' variables were kept for future use in case a new approach to handle low confidence case is adopted */
			fit_count = 2;
			
			/* Compute covariances and means... AGAIN, but only for two points: first and last */
			Exy = ((margins[stop_n-1]*lbers[stop_n-1] + margins[0]*lbers[0])/(USR_DOUBLE)delta_n);
			Eyy = ((lbers[stop_n-1]*lbers[stop_n-1] + lbers[0]*lbers[0])/(USR_DOUBLE)delta_n);
			Exx = ((margins[stop_n-1]*margins[stop_n-1] + margins[0]*margins[0])/(USR_DOUBLE)delta_n);
			Ey  = ((lbers[stop_n-1] + lbers[0])/(USR_DOUBLE)delta_n);
			Ex  = ((margins[stop_n-1] + margins[0])/(USR_DOUBLE)delta_n);
					
			/* Compute fit slope and offset: ber = alpha*margin + beta */
			alpha = (Exy - Ey*Ex) / (Exx - Ex*Ex);
			beta = Ey - Ex*alpha;
			/* Compute alpha2: slope of regression: margin = alpha2*ber + beta2 */
			alpha2 = (Exy - Ey*Ex) / (Eyy - Ey*Ey);
			/* Compute correlation index sq_r */
			sq_r = alpha*alpha2;

			proj_ber = pow(10,(-beta*beta));
			proj_margin_12 = (sqrt(-log10(1e-12))-beta)/alpha;
			proj_margin_15 = (sqrt(-log10(1e-15))-beta)/alpha;
			proj_margin_18 = (sqrt(-log10(1e-18))-beta)/alpha;

			sq_err1 = (Eyy + (beta*beta) + (Exx*alpha*alpha) - 
					   (2*Ey*beta) - (2*Exy*alpha) + (2*Ex*beta*alpha));
			sq_err2 = 0;
		}
		
		proj_ber = log10(proj_ber);
		proj_ber_aux = proj_ber;

		if (proj_ber < MIN_BER_TO_REPORT) {
			proj_ber = MIN_BER_TO_REPORT;
			ber_clipped = 1;
		}
		
		/* USR_PRINTF(("Estimates AFTER CORRECTING above BER values for 95pct confidence level:\n"));	*/
		/* Extrapolated results, low confidence */
		if (low_confidence == 1) {

			if(heye == 1) {
				USR_PRINTF(("BER(extrapolated) < 1e%.2f\n", proj_ber));
				if(proj_ber < -12) USR_PRINTF(("Margin @ 1e-12    > %0.2f mUI\n", proj_margin_12));
				if(proj_ber < -15) USR_PRINTF(("Margin @ 1e-15    > %0.2f mUI\n", proj_margin_15));
				if(proj_ber < -18) USR_PRINTF(("Margin @ 1e-18    > %0.2f mUI\n", proj_margin_18));
			} else {
				USR_PRINTF(("BER(extrapolated) < 1e%0.2f\n", proj_ber));
				if(proj_ber < -12) USR_PRINTF(("Margin @ 1e-12    > %0.2f mV\n", proj_margin_12));
				if(proj_ber < -15) USR_PRINTF(("Margin @ 1e-15    > %0.2f mV\n", proj_margin_15));
				if(proj_ber < -18) USR_PRINTF(("Margin @ 1e-18    > %0.2f mV\n", proj_margin_18));
			}
			
 		/* Extrapolated results, HIGH confidence */			
		} else {
          
			if(heye == 1) {
				if (ber_clipped == 1) {
					USR_PRINTF(("BER(extrapolated) < 1e%0.2f\n", proj_ber));
				} else {
					USR_PRINTF(("BER(extrapolated) = 1e%0.2f\n", proj_ber));
				}
				if(proj_ber < -12) USR_PRINTF(("Margin @ 1e-12    = %0.2f mUI\n", proj_margin_12));
				if(proj_ber < -15) USR_PRINTF(("Margin @ 1e-15    = %0.2f mUI\n", proj_margin_15));
				if(proj_ber < -18) USR_PRINTF(("Margin @ 1e-18    = %0.2f mUI\n", proj_margin_18));
			} else {
				if (ber_clipped == 1) {
					USR_PRINTF(("BER(extrapolated) < 1e%0.2f\n", proj_ber));
				} else {
					USR_PRINTF(("BER(extrapolated) = 1e%0.2f\n", proj_ber));
				}
				if(proj_ber < -12) USR_PRINTF(("Margin @ 1e-12    = %0.2f mV\n", proj_margin_12));
				if(proj_ber < -15) USR_PRINTF(("Margin @ 1e-15    = %0.2f mV\n", proj_margin_15));
				if(proj_ber < -18) USR_PRINTF(("Margin @ 1e-18    = %0.2f mV\n", proj_margin_18));
			}
		}
		
		USR_PRINTF(("\n\n"));
		
		/* Print non-monotonic outliers */
		if (n_mono != 0) {
			USR_PRINTF(("Detected non-monotonicity at { "));
			for (loop_index = start_n; loop_index < stop_n; loop_index++) {
				if (mono_flags[loop_index] == 1) {
					USR_PRINTF(("%0.2f ", margins[loop_index]));
				}
			} 
			USR_PRINTF(("}\n\n\n"));
		}

		/* SUMMARY (for debugging purposes*/
		if (verbose > 2) USR_PRINTF (("\t=====> DEBUG INFO (start)\n\n"));
		if (verbose > 2) {
			USR_PRINTF((" loop   Margin(mV)  total_errors   time(sec)  logBER       lber"));
			for (loop_index=0; loop_index < stop_n+last_point_discard; loop_index++) {
				USR_PRINTF(("\n%5d %11.0f %14d %10.3f %8.2f %10.3f", loop_index, margins[loop_index], total_errs[loop_index], ((USR_DOUBLE)total_time[loop_index])*0.00001, log10(bers[loop_index]), lbers[loop_index]));
			}
			USR_PRINTF(("\n\n"));
		}
		if (verbose > 2) USR_PRINTF(("Max Offset = %d\n",max_offset));	
		if (verbose > 2) USR_PRINTF(("ber_clipped: %d, Projected BER (proj_ber_aux) = %.2f\n", ber_clipped, proj_ber_aux));	
		if (verbose > 2) USR_PRINTF(("first good ber idx at %d, ber = 1e%f\n", first_good_ber_idx, log10(bers[first_good_ber_idx])));
		if (verbose > 2) USR_PRINTF(("first small errcnt idx at %d, errors = %d\n", first_small_errcnt_idx, total_errs[first_small_errcnt_idx]));
		if (verbose > 2) USR_PRINTF(("last point discarded?: %d, low_confidence: %d, start_n: %d, stop_n: %d, eye_cnt: %d, n_mono: %d, first_good_ber_idx = %d, first_small_errcnt_idx = %d, fit_count: %d, delta_n: %d\n", 
					last_point_discard, low_confidence, start_n, stop_n, eye_cnt, n_mono, first_good_ber_idx, first_small_errcnt_idx, fit_count, delta_n));
		if (verbose > 2) USR_PRINTF(("Exy=%.2f, Eyy=%.4f, Exx=%.2f, Ey=%.4f, Ex=%.2f, alpha=%.4f, beta=%.4f, alpha2=%.3f, sq_r=%.3f, sq_err1=%.2f, sq_err2=%.2f\n", Exy,Eyy,Exx,Ey,Ex,alpha,beta,alpha2,sq_r,sq_err1,sq_err2));
		if (verbose > 2) USR_PRINTF (("\n\t=====> DEBUG INFO (end)\n\n"));
		USR_PRINTF(("\n\n\n"));
	
    }
#else
 USR_PRINTF(("This functions needs SERDES_API_FLOATING_POINT define to operate \n"));
 if(!total_errs || !total_time ) {
 	return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
 }
 rate = 0; ber_scan_mode = 0; max_offset = 0;     /* to avoid compile warnings */ 
 ber_scan_mode = rate & 0xff; /* to avoid Coverity Wanings - param_set_but_not_used */
 max_offset = ber_scan_mode;
#endif
	return(ERR_CODE_NONE);

}




/*****************************/
/*  Display Lane/Core State  */
/*****************************/

err_code_t falcon_tsc_display_lane_state_hdr( const phymod_access_t *pa ) {
  USR_PRINTF(("LN (CDRxN  ,UC_CFG) "));
  USR_PRINTF(("SD LCK RXPPM "));
  USR_PRINTF(("CLK90 CLKP1 "));
  USR_PRINTF(("PF(M,L) "));
  USR_PRINTF(("VGA DCO "));
  USR_PRINTF(("P1mV "));
  USR_PRINTF(("M1mV "));
  USR_PRINTF((" DFE(1,2,3,4,5,6)        "));
  USR_PRINTF(("TXPPM TXEQ(n1,m,p1,p2,p3) EYE(L,R,U,D)  "));
  USR_PRINTF(("LINK_TIME"));
  USR_PRINTF(("\n"));
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_display_lane_state_legend( const phymod_access_t *pa ) {
  USR_PRINTF(("\n"));
  USR_PRINTF(("**********************************************************************************************\n")); 
  USR_PRINTF(("****                Legend of Entries in display_lane_state()                             ****\n")); 
  USR_PRINTF(("**********************************************************************************************\n"));
  USR_PRINTF(("LN       : lane index within IP core\n"));
  USR_PRINTF(("(CDRxN,UC_CFG) : CDR type x OSR ratio, micro lane configuration variable\n"));
  USR_PRINTF(("SD       : signal detect\n"));
  USR_PRINTF(("LOCK     : pmd_rx_lock\n"));
  USR_PRINTF(("RXPPM    : Frequency offset of local reference clock with respect to RX data in ppm\n"));
  USR_PRINTF(("CLK90    : Delay of zero crossing slicer, m1, wrt to data in PI codes\n"));
  USR_PRINTF(("CLKP1    : Delay of diagnostic/lms slicer, p1, wrt to data in PI codes\n"));
  USR_PRINTF(("PF(M,L)  : Peaking Filter Main (0..15) and Low Frequency (0..7) settings\n"));
  USR_PRINTF(("VGA      : Variable Gain Amplifier settings (0..39)\n"));
  USR_PRINTF(("DCO      : DC offset DAC control value\n"));
  USR_PRINTF(("P1mV     : Vertical threshold voltage of p1 slicer\n"));
  USR_PRINTF(("M1mV     : Vertical threshold voltage of m1 slicer\n"));
  USR_PRINTF(("DFE taps : ISI correction taps in units of 2.35mV\n"));   
  USR_PRINTF(("TXPPM            : Frequency offset of local reference clock with respect to TX data in ppm\n"));  
  USR_PRINTF(("TXEQ(n1,m,p1,p2,p3) : TX equalization FIR tap weights in units of 1Vpp/160 units\n"));
  USR_PRINTF(("EYE(L,R,U,D)     : Eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV\n"));
  USR_PRINTF(("LINK_TIME        : Link time in milliseconds\n"));
  USR_PRINTF(("**********************************************************************************************\n")); 
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_display_lane_state( const phymod_access_t *pa ) {
  err_code_t err_code;
  err_code = _falcon_tsc_display_lane_state_no_newline( pa );
  USR_PRINTF(("\n"));
  return (err_code);
}

err_code_t falcon_tsc_display_core_state_hdr( const phymod_access_t *pa ) {
  USR_PRINTF(("CORE RST_ST  PLL_PWDN  UC_ATV   COM_CLK   UCODE_VER  AFE_VER   TEMPERATURE_IDX   RESCAL   VCO_RATE  ANA_VCO_RANGE  PLL_DIV   PLL_LOCK\n"));
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_display_core_state_line( const phymod_access_t *pa ) {
  err_code_t err_code;
  err_code = _falcon_tsc_display_core_state_no_newline( pa ); 
  USR_PRINTF(("\n"));
  return (err_code);
}

err_code_t falcon_tsc_display_core_state_legend() {
  USR_PRINTF(("\n"));
  USR_PRINTF(("**************************************************************************************************************\n")); 
  USR_PRINTF(("****                          Legend of Entries in display_core_state()                                   ****\n")); 
  USR_PRINTF(("**************************************************************************************************************\n")); 
  USR_PRINTF(("*  RST_ST           : Core DP Reset State (hex) [3'{reset_active, reset_occured, reset_held}]                *\n"));
  USR_PRINTF(("*  PLL_PWDN         : PLL Powerdown Control Bit (active high)                                                *\n"));
  USR_PRINTF(("*  UC_ATV           : UC Active bit                                                                          *\n"));
  USR_PRINTF(("*  COM_CLK          : COM Clock frequency in MHz                                                             *\n"));
  USR_PRINTF(("*  UCODE_VER        : Microcode Version [majorversion_minorversion]                                          *\n"));
  USR_PRINTF(("*  AFE_VER          : AFE Hardware Vesrion                                                                   *\n"));
  USR_PRINTF(("*  TEMPERATURE_IDX  : Temperature Index [index followed by temperature range]                                *\n"));
  USR_PRINTF(("*  AVG_TMON         : Average TMON index                                                                     *\n"));
  USR_PRINTF(("*  RESCAL           : Analog Resistor Calibration value                                                      *\n"));
  USR_PRINTF(("*  VCO_RATE         : VCO Rate in GHz (approximate)                                                          *\n"));
  USR_PRINTF(("*  ANA_VCO_RANGE    : Analog VCO Range                                                                       *\n"));
  USR_PRINTF(("*  PLL_DIV          : PLL Divider value                                                                      *\n"));
  USR_PRINTF(("*  PLL_Lock       : PLL Lock                                                                               *\n"));

  USR_PRINTF(("**************************************************************************************************************\n")); 
  return (ERR_CODE_NONE);
}

/**********************************************/
/*  Display Lane/Core Config and Debug Status */
/**********************************************/

err_code_t falcon_tsc_display_core_config( const phymod_access_t *pa ) {

  err_code_t  err_code;
  struct falcon_tsc_uc_core_config_st core_cfg;
  uint16_t vco_rate_mhz;

  USR_MEMSET(&core_cfg, 0, sizeof(struct falcon_tsc_uc_core_config_st));

  USR_PRINTF(("\n\n***********************************\n")); 
  USR_PRINTF(("**** SERDES CORE CONFIGURATION ****\n")); 
  USR_PRINTF(("***********************************\n\n")); 

  err_code = falcon_tsc_get_uc_core_config( pa, &core_cfg);
  if (err_code) {
    return (err_code);
  }

    vco_rate_mhz = (((((uint16_t) core_cfg.field.vco_rate) * 125)/2) + 14000);
  USR_PRINTF(("uC Config VCO Rate   = %d (~%d.%dGHz)\n",core_cfg.field.vco_rate,vco_rate_mhz/1000,vco_rate_mhz % 1000));
  USR_PRINTF(("Core Config from PCS = %d\n\n", core_cfg.field.core_cfg_from_pcs));

  ESTM(USR_PRINTF(("Lane Addr 0          = %d\n", rdc_lane_addr_0())));
  ESTM(USR_PRINTF(("Lane Addr 1          = %d\n", rdc_lane_addr_1())));
  ESTM(USR_PRINTF(("Lane Addr 2          = %d\n", rdc_lane_addr_2())));
  ESTM(USR_PRINTF(("Lane Addr 3          = %d\n", rdc_lane_addr_3())));

  ESTM(USR_PRINTF(("TX Lane Map 0        = %d\n", rdc_tx_lane_map_0())));
  ESTM(USR_PRINTF(("TX Lane Map 1        = %d\n", rdc_tx_lane_map_1())));
  ESTM(USR_PRINTF(("TX Lane Map 2        = %d\n", rdc_tx_lane_map_2())));
  ESTM(USR_PRINTF(("TX Lane Map 3        = %d\n\n", rdc_tx_lane_map_3())));
  return(ERR_CODE_NONE); 
}


err_code_t falcon_tsc_display_lane_config( const phymod_access_t *pa ) {

  err_code_t  err_code;
  struct falcon_tsc_uc_lane_config_st lane_cfg;

  USR_MEMSET(&lane_cfg, 0, sizeof(struct falcon_tsc_uc_lane_config_st));

  USR_PRINTF(("\n\n*************************************\n")); 
  USR_PRINTF(("**** SERDES LANE %d CONFIGURATION ****\n",falcon_tsc_get_lane(pa))); 
  USR_PRINTF(("*************************************\n\n")); 

  err_code = falcon_tsc_get_uc_lane_cfg( pa, &lane_cfg);
  if (err_code) {
    return (err_code);
  }

  USR_PRINTF(("Auto-Neg Enabled      = %d\n", lane_cfg.field.an_enabled));
  USR_PRINTF(("DFE on                = %d\n", lane_cfg.field.dfe_on));
  USR_PRINTF(("DFE low power mode    = %d\n", lane_cfg.field.dfe_lp_mode));
  USR_PRINTF(("Brdfe_on              = %d\n", lane_cfg.field.force_brdfe_on));
  USR_PRINTF(("Media Type            = %d\n", lane_cfg.field.media_type));
  USR_PRINTF(("Unreliable LOS        = %d\n", lane_cfg.field.unreliable_los));
  USR_PRINTF(("Scrambling Disable    = %d\n", lane_cfg.field.scrambling_dis));
  USR_PRINTF(("Lane Config from PCS  = %d\n\n", lane_cfg.field.lane_cfg_from_pcs));

  ESTM(USR_PRINTF(("CL93/72 Training Enable  = %d\n", rd_cl93n72_ieee_training_enable())));
  ESTM(USR_PRINTF(("EEE Mode Enable       = %d\n", rd_eee_mode_en())));
  ESTM(USR_PRINTF(("OSR Mode Force        = %d\n", rd_osr_mode_frc())));
  ESTM(USR_PRINTF(("OSR Mode Force Val    = %d\n", rd_osr_mode_frc_val())));
  ESTM(USR_PRINTF(("TX Polarity Invert    = %d\n", rd_tx_pmd_dp_invert())));
  ESTM(USR_PRINTF(("RX Polarity Invert    = %d\n\n", rd_rx_pmd_dp_invert())));

  ESTM(USR_PRINTF(("TXFIR Post2           = %d\n", rd_txfir_post2())));
  ESTM(USR_PRINTF(("TXFIR Post3           = %d\n", rd_txfir_post3())));
  ESTM(USR_PRINTF(("TXFIR Main            = %d\n", rd_cl93n72_txfir_main())));
  ESTM(USR_PRINTF(("TXFIR Pre             = %d\n", rd_cl93n72_txfir_pre())));
  ESTM(USR_PRINTF(("TXFIR Post            = %d\n", rd_cl93n72_txfir_post())));

  return(ERR_CODE_NONE); 
}


err_code_t falcon_tsc_display_core_state( const phymod_access_t *pa ) {

  err_code_t  err_code;
  uint8_t     temp_idx;
  int8_t      temp_val;
  uint16_t    vco_rate_mhz;
  struct falcon_tsc_uc_core_config_st core_cfg;

  USR_PRINTF(("\n\n***********************************\n")); 
  USR_PRINTF(("**** SERDES CORE DISPLAY STATE ****\n")); 
  USR_PRINTF(("***********************************\n\n")); 

  ESTM(USR_PRINTF(("Average Die TMON_reg13bit = %d\n", rdcv_avg_tmon_reg13bit())));
  ESTM(USR_PRINTF(("Temperature Force Val     = %d\n", rdcv_temp_frc_val())));

  ESTM(temp_idx = rdcv_temp_idx());
  if (temp_idx == 0) {
    USR_PRINTF(("Temperature Index         = 0  [T_MIN < -36C; T_CENTRE = -36C; T_MAX = -32C]\n"));
  }
  else if (temp_idx < 20) {
    temp_val = temp_idx * 8;
    temp_val = temp_val - 40;
    USR_PRINTF(("Temperature Index         = %d  [%dC to %dC]\n",temp_idx,temp_val,temp_val+8));
  }
  else if (temp_idx == 20) {
    USR_PRINTF(("Temperature Index         = 20  [T_MIN = 120C; T_CENTRE = 124C; T_MAX > 124C]\n"));
  }
  else {
    return (_error(ERR_CODE_INVALID_TEMP_IDX));
  }
  ESTM(USR_PRINTF(("Core Event Log Level      = %d\n\n", rdcv_usr_ctrl_core_event_log_level())));

  ESTM(USR_PRINTF(("Core DP Reset State       = %d\n\n", rdc_core_dp_reset_state())));

  ESTM(USR_PRINTF(("Common Ucode Version       = 0x%x\n", rdcv_common_ucode_version())));
  ESTM(USR_PRINTF(("Common Ucode Minor Version = 0x%x\n", rdcv_common_ucode_minor_version())));
  ESTM(USR_PRINTF(("AFE Hardware Version       = 0x%x\n\n", rdcv_afe_hardware_version())));

  err_code = falcon_tsc_get_uc_core_config( pa, &core_cfg);
  if (err_code) {
    return (err_code);
  }
    vco_rate_mhz = (((((uint16_t) core_cfg.field.vco_rate) * 125)/2) + 14000);
  USR_PRINTF(("VCO Rate                   = %d (~%d.%dGHz)\n",core_cfg.field.vco_rate,vco_rate_mhz/1000,vco_rate_mhz % 1000)); 
    ESTM(USR_PRINTF(("Analog VCO Range           = %d\n", rdc_ams_pll_range())));
    ESTM(USR_PRINTF(("PLL Divider                = %d\n\n", rdc_ams_pll_ndiv())));

  return(ERR_CODE_NONE); 
}



err_code_t falcon_tsc_display_lane_debug_status( const phymod_access_t *pa ) {

  err_code_t  err_code;

  /* startup */
  struct falcon_tsc_usr_ctrl_disable_functions_st ds; 
  struct falcon_tsc_usr_ctrl_disable_dfe_functions_st dsd;

  /* steady state */
  struct falcon_tsc_usr_ctrl_disable_functions_st dss;
  struct falcon_tsc_usr_ctrl_disable_dfe_functions_st dssd;

  USR_MEMSET(&ds, 0, sizeof(struct falcon_tsc_usr_ctrl_disable_functions_st));
  USR_MEMSET(&dsd, 0, sizeof(struct falcon_tsc_usr_ctrl_disable_dfe_functions_st));
  USR_MEMSET(&dss, 0, sizeof(struct falcon_tsc_usr_ctrl_disable_functions_st));
  USR_MEMSET(&dssd, 0, sizeof(struct falcon_tsc_usr_ctrl_disable_dfe_functions_st));

  USR_PRINTF(("\n\n************************************\n")); 
  USR_PRINTF(("**** SERDES LANE %d DEBUG STATUS ****\n",falcon_tsc_get_lane(pa))); 
  USR_PRINTF(("************************************\n\n")); 

  ESTM(USR_PRINTF(("Restart Count       = %d\n", rdv_usr_sts_restart_counter())));
  ESTM(USR_PRINTF(("Reset Count         = %d\n", rdv_usr_sts_reset_counter())));
  ESTM(USR_PRINTF(("PMD Lock Count      = %d\n\n", rdv_usr_sts_pmd_lock_counter())));
 

  err_code = falcon_tsc_get_usr_ctrl_disable_startup( pa, &ds);
  if (err_code) {
    return (err_code);
  }

  USR_PRINTF(("Disable Startup PF Adaptation           = %d\n", ds.field.pf_adaptation));
  USR_PRINTF(("Disable Startup PF2 Adaptation          = %d\n", ds.field.pf2_adaptation));
  USR_PRINTF(("Disable Startup DC Adaptation           = %d\n", ds.field.dc_adaptation));
  USR_PRINTF(("Disable Startup VGA Adaptation          = %d\n", ds.field.vga_adaptation));
  USR_PRINTF(("Disable Startup Slicer vOffset Tuning   = %d\n", ds.field.slicer_voffset_tuning));
  USR_PRINTF(("Disable Startup Slicer hOffset Tuning   = %d\n", ds.field.slicer_hoffset_tuning));
  USR_PRINTF(("Disable Startup Phase offset Adaptation = %d\n", ds.field.phase_offset_adaptation));
  USR_PRINTF(("Disable Startup Eye Adaptaion           = %d\n", ds.field.eye_adaptation));
  USR_PRINTF(("Disable Startup All Adaptaion           = %d\n\n", ds.field.all_adaptation));

  err_code = falcon_tsc_get_usr_ctrl_disable_startup_dfe( pa, &dsd);
  if (err_code) {
    return (err_code);
  }

  USR_PRINTF(("Disable Startup DFE Tap1 Adaptation    = %d\n",dsd.field.dfe_tap1_adaptation));
  USR_PRINTF(("Disable Startup DFE FX Taps Adaptation = %d\n",dsd.field.dfe_fx_taps_adaptation));
  USR_PRINTF(("Disable Startup DFE FL Taps Adaptation = %d\n",dsd.field.dfe_fl_taps_adaptation));
  USR_PRINTF(("Disable Startup DFE Tap DCD            = %d\n",dsd.field.dfe_dcd_adaptation));       

  err_code = falcon_tsc_get_usr_ctrl_disable_steady_state( pa, &dss);
  if (err_code) {
    return (err_code);
  }

  USR_PRINTF(("Disable Steady State PF Adaptation           = %d\n", dss.field.pf_adaptation));
  USR_PRINTF(("Disable Steady State PF2 Adaptation          = %d\n", dss.field.pf2_adaptation));
  USR_PRINTF(("Disable Steady State DC Adaptation           = %d\n", dss.field.dc_adaptation));
  USR_PRINTF(("Disable Steady State VGA Adaptation          = %d\n", dss.field.vga_adaptation));
  USR_PRINTF(("Disable Steady State Slicer vOffset Tuning   = %d\n", dss.field.slicer_voffset_tuning));
  USR_PRINTF(("Disable Steady State Slicer hOffset Tuning   = %d\n", dss.field.slicer_hoffset_tuning));
  USR_PRINTF(("Disable Steady State Phase offset Adaptation = %d\n", dss.field.phase_offset_adaptation));
  USR_PRINTF(("Disable Steady State Eye Adaptaion           = %d\n", dss.field.eye_adaptation));
  USR_PRINTF(("Disable Steady State All Adaptaion           = %d\n\n", dss.field.all_adaptation));

  err_code = falcon_tsc_get_usr_ctrl_disable_steady_state_dfe( pa, &dssd);
  if (err_code) {
    return (err_code);
  }

  USR_PRINTF(("Disable Steady State DFE Tap1 Adaptation    = %d\n",dssd.field.dfe_tap1_adaptation));
  USR_PRINTF(("Disable Steady State DFE FX Taps Adaptation = %d\n",dssd.field.dfe_fx_taps_adaptation));
  USR_PRINTF(("Disable Steady State DFE FL Taps Adaptation = %d\n",dssd.field.dfe_fl_taps_adaptation));
  USR_PRINTF(("Disable Steady State DFE Tap DCD            = %d\n",dssd.field.dfe_dcd_adaptation));       

  ESTM(USR_PRINTF(("Retune after Reset    = %d\n", rdv_usr_ctrl_retune_after_restart())));               
  ESTM(USR_PRINTF(("Clk90 offset Adjust   = %d\n", rdv_usr_ctrl_clk90_offset_adjust())));                
  ESTM(USR_PRINTF(("Clk90 offset Override = %d\n", rdv_usr_ctrl_clk90_offset_override())));              
  ESTM(USR_PRINTF(("Lane Event Log Level  = %d\n", rdv_usr_ctrl_lane_event_log_level())));

  return(ERR_CODE_NONE); 
}



/*************************/
/*  Stop/Resume uC Lane  */
/*************************/

err_code_t falcon_tsc_stop_uc_lane( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    return(falcon_tsc_pmd_uc_control( pa, CMD_UC_CTRL_STOP_GRACEFULLY,2000));
  }
  else {
    return(falcon_tsc_pmd_uc_control( pa, CMD_UC_CTRL_RESUME,2000));
  }
}


err_code_t falcon_tsc_stop_uc_lane_status( const phymod_access_t *pa, uint8_t *uc_lane_stopped) {
  
  if(!uc_lane_stopped) {
	  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*uc_lane_stopped = rdv_usr_sts_micro_stopped());

  return (ERR_CODE_NONE);
}


/*******************************/
/*  Stop/Resume RX Adaptation  */
/*******************************/

err_code_t falcon_tsc_stop_rx_adaptation( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    return(falcon_tsc_pmd_uc_control( pa, CMD_UC_CTRL_STOP_GRACEFULLY,5000));
  }
  else {
    return(falcon_tsc_pmd_uc_control( pa, CMD_UC_CTRL_RESUME,2000));
  }
}



/**********************/
/*  uCode CRC Verify  */
/**********************/

err_code_t falcon_tsc_ucode_crc_verify( const phymod_access_t *pa, uint16_t ucode_len,uint16_t expected_crc_value) {
    uint16_t calc_crc;

    EFUN(falcon_tsc_pmd_uc_cmd_with_data( pa, CMD_CALC_CRC,0,ucode_len,2000));

    ESTM(calc_crc = rd_uc_dsc_data());
    if(calc_crc != expected_crc_value) {
        USR_PRINTF(("UC CRC did not match expected=%04x : calculated=%04x\n",expected_crc_value, calc_crc));
        return(_error(ERR_CODE_UC_CRC_NOT_MATCH));
    }

    return(ERR_CODE_NONE);
}


/******************************************************/
/*  Commands through Serdes FW DSC Command Interface  */
/******************************************************/

err_code_t falcon_tsc_pmd_uc_cmd( const phymod_access_t *pa, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint32_t timeout_ms) {

  err_code_t err_code;
  uint16_t cmddata;
  uint8_t uc_dsc_error_found;

  err_code = falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1( pa, timeout_ms); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate falcon_tsc ready for command */
  if (err_code) {
    ESTM(USR_PRINTF(("ERROR : DSC ready for command timed out (before cmd) cmd = %d, supp_info = x%02x err=%d !\n", cmd, supp_info,err_code)));      
    return (err_code);
  }
  /*wr_uc_dsc_supp_info(supp_info);*/                                   /* supplement info field */
  /*wr_uc_dsc_error_found(0x0);    */                                   /* Clear error found field */
  /*wr_uc_dsc_gp_uc_req(cmd);      */                                   /* Set command code */
  /*wr_uc_dsc_ready_for_cmd(0x0);  */                                   /* Issue command, by clearing "ready for command" field */
  cmddata = (((uint16_t)supp_info)<<8) | (uint16_t)cmd;     /* combine writes to single write instead of 4 RMW */

  EFUN(falcon_tsc_pmd_wr_reg(pa,DSC_A_DSC_UC_CTRL, cmddata));         /* This address is same for Eagle, and all merlin */

  EFUN(falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1( pa, timeout_ms)); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate falcon_tsc ready for command */
  ESTM(uc_dsc_error_found = rd_uc_dsc_error_found());
  if(uc_dsc_error_found) {
    ESTM(USR_PRINTF(("ERROR : DSC ready for command return error ( after cmd) cmd = %d, supp_info = x%02x !\n", cmd, rd_uc_dsc_supp_info())));      
      return(_error(ERR_CODE_UC_CMD_RETURN_ERROR));
  }
  return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_pmd_uc_cmd_with_data( const phymod_access_t *pa, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint16_t data, uint32_t timeout_ms) {
  uint16_t cmddata;
  err_code_t err_code;
  uint8_t uc_dsc_error_found;

  err_code = falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1( pa, timeout_ms); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate falcon_tsc ready for command */
  if (err_code) {
	 USR_PRINTF(("ERROR : DSC ready for command timed out (before cmd) cmd = %d, supp_info = x%02x, data = x%04x err=%d !\n", cmd, supp_info, data,err_code));      
    return (err_code);
  }

  EFUN(wr_uc_dsc_data(data));                                       /* Write value written to uc_dsc_data field */
  /*wr_uc_dsc_supp_info(supp_info);  */                               /* supplement info field */
  /*wr_uc_dsc_error_found(0x0);      */                               /* Clear error found field */
  /*wr_uc_dsc_gp_uc_req(cmd);        */                               /* Set command code */
  /*wr_uc_dsc_ready_for_cmd(0x0);    */                               /* Issue command, by clearing "ready for command" field */
  cmddata = (((uint16_t)supp_info)<<8) | (uint16_t)cmd;   /* combine writes to single write instead of 4 RMW */

  EFUN(falcon_tsc_pmd_wr_reg(pa,DSC_A_DSC_UC_CTRL, cmddata));         /* This address is same for Eagle, and all merlin */

  EFUN(falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1( pa, timeout_ms)); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate falcon_tsc ready for command */
  ESTM(uc_dsc_error_found = rd_uc_dsc_error_found());
  if(uc_dsc_error_found) {
    ESTM(USR_PRINTF(("ERROR : DSC ready for command return error ( after cmd) cmd = %d, supp_info = x%02x !\n", cmd, rd_uc_dsc_supp_info())));      
    return(_error(ERR_CODE_UC_CMD_RETURN_ERROR));
}

  return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_pmd_uc_control( const phymod_access_t *pa, enum srds_pmd_uc_ctrl_cmd_enum control, uint32_t timeout_ms) {
  return(falcon_tsc_pmd_uc_cmd( pa, CMD_UC_CTRL, (uint8_t) control, timeout_ms));
}

err_code_t falcon_tsc_pmd_uc_diag_cmd( const phymod_access_t *pa, enum srds_pmd_uc_diag_cmd_enum control, uint32_t timeout_ms) {
  return(falcon_tsc_pmd_uc_cmd( pa, CMD_DIAG_EN, (uint8_t) control, timeout_ms));
}



/************************************************************/
/*      Serdes IP RAM access - Lane RAM Variables           */
/*----------------------------------------------------------*/
/*   - through Micro Register Interface for PMD IPs         */
/*   - through Serdes FW DSC Command Interface for Gallardo */
/************************************************************/

/* Micro RAM Lane Byte Read */
uint8_t falcon_tsc_rdbl_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
	uint8_t rddata;



	if(!err_code_p) {
		return(0);
	}

    EPSTM(rddata = falcon_tsc_rdb_uc_ram( pa, err_code_p, (LANE_VAR_RAM_BASE+addr+(falcon_tsc_get_lane(pa)*LANE_VAR_RAM_SIZE)))); /* Use Micro register interface for reading RAM */

	return (rddata);
} 

/* Micro RAM Lane Byte Signed Read */
int8_t falcon_tsc_rdbls_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  return ((int8_t) falcon_tsc_rdbl_uc_var( pa, err_code_p, addr));
} 

/* Micro RAM Lane Word Read */
uint16_t falcon_tsc_rdwl_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  uint16_t rddata;

  if(!err_code_p) {
	  return(0);
  }

  if (addr%2 != 0) {                                                                /* Validate even address */
	  *err_code_p = ERR_CODE_INVALID_RAM_ADDR;
	  return (0);
  }

  EPSTM(rddata = falcon_tsc_rdw_uc_ram( pa, err_code_p, (LANE_VAR_RAM_BASE+addr+(falcon_tsc_get_lane(pa)*LANE_VAR_RAM_SIZE)))); /* Use Micro register interface for reading RAM */

  return (rddata);
}


/* Micro RAM Lane Word Signed Read */
int16_t falcon_tsc_rdwls_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  return ((int16_t) falcon_tsc_rdwl_uc_var( pa, err_code_p, addr));
}
  
/* Micro RAM Lane Byte Write */
err_code_t falcon_tsc_wrbl_uc_var( const phymod_access_t *pa, uint16_t addr, uint8_t wr_val) {


    return (falcon_tsc_wrb_uc_ram( pa, (LANE_VAR_RAM_BASE+addr+(falcon_tsc_get_lane(pa)*LANE_VAR_RAM_SIZE)), wr_val));    /* Use Micro register interface for writing RAM */
}

/* Micro RAM Lane Byte Signed Write */
err_code_t falcon_tsc_wrbls_uc_var( const phymod_access_t *pa, uint16_t addr, int8_t wr_val) {
  return (falcon_tsc_wrbl_uc_var( pa, addr, wr_val));
}

/* Micro RAM Lane Word Write */
err_code_t falcon_tsc_wrwl_uc_var( const phymod_access_t *pa, uint16_t addr, uint16_t wr_val) {


	if (addr%2 != 0) {                                                                /* Validate even address */
		return (_error(ERR_CODE_INVALID_RAM_ADDR));
	}
    return (falcon_tsc_wrw_uc_ram( pa, (LANE_VAR_RAM_BASE+addr+(falcon_tsc_get_lane(pa)*LANE_VAR_RAM_SIZE)), wr_val));    /* Use Micro register interface for writing RAM */
}

/* Micro RAM Lane Word Signed Write */
err_code_t falcon_tsc_wrwls_uc_var( const phymod_access_t *pa, uint16_t addr, int16_t wr_val) {
  return (falcon_tsc_wrwl_uc_var( pa, addr,wr_val));
}


/************************************************************/
/*      Serdes IP RAM access - Core RAM Variables           */
/*----------------------------------------------------------*/
/*   - through Micro Register Interface for PMD IPs         */
/*   - through Serdes FW DSC Command Interface for Gallardo */
/************************************************************/

/* Micro RAM Core Byte Read */
uint8_t falcon_tsc_rdbc_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {

  uint8_t rddata;

  if(!err_code_p) {
	  return(0);
  }

  EPSTM(rddata = falcon_tsc_rdb_uc_ram( pa, err_code_p, (CORE_VAR_RAM_BASE+addr)));                      /* Use Micro register interface for reading RAM */

  return (rddata);
} 

/* Micro RAM Core Byte Signed Read */
int8_t falcon_tsc_rdbcs_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {
  return ((int8_t) falcon_tsc_rdbc_uc_var( pa, err_code_p, addr));
}

/* Micro RAM Core Word Read */
uint16_t falcon_tsc_rdwc_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {

  uint16_t rddata;

  if(!err_code_p) {
	  return(0);
  }
  if (addr%2 != 0) {                                                                /* Validate even address */
	  *err_code_p = ERR_CODE_INVALID_RAM_ADDR;
	  return (0);
  }

  EPSTM(rddata = falcon_tsc_rdw_uc_ram( pa, err_code_p, (CORE_VAR_RAM_BASE+addr)));                  /* Use Micro register interface for reading RAM */

  return (rddata);
}

/* Micro RAM Core Word Signed Read */
int16_t falcon_tsc_rdwcs_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {
  return ((int16_t) falcon_tsc_rdwc_uc_var( pa, err_code_p, addr));
}

/* Micro RAM Core Byte Write  */
err_code_t falcon_tsc_wrbc_uc_var( const phymod_access_t *pa, uint8_t addr, uint8_t wr_val) {


    return (falcon_tsc_wrb_uc_ram( pa, (CORE_VAR_RAM_BASE+addr), wr_val));                                /* Use Micro register interface for writing RAM */
} 


/* Micro RAM Core Byte Signed Write */
err_code_t falcon_tsc_wrbcs_uc_var( const phymod_access_t *pa, uint8_t addr, int8_t wr_val) {
  return (falcon_tsc_wrbc_uc_var( pa, addr, wr_val));
}

/* Micro RAM Core Word Write  */
err_code_t falcon_tsc_wrwc_uc_var( const phymod_access_t *pa, uint8_t addr, uint16_t wr_val) {


	if (addr%2 != 0) {                                                                /* Validate even address */
		return (_error(ERR_CODE_INVALID_RAM_ADDR));
	}
    return (falcon_tsc_wrw_uc_ram( pa, (CORE_VAR_RAM_BASE+addr), wr_val));                                 /* Use Micro register interface for writing RAM */
}

/* Micro RAM Core Word Signed Write */
err_code_t falcon_tsc_wrwcs_uc_var( const phymod_access_t *pa, uint8_t addr, int16_t wr_val) {
  return(falcon_tsc_wrwc_uc_var( pa, addr,wr_val));
}



/*******************************************************************/
/*  APIs to Write Core/Lane Config and User variables into uC RAM  */
/*******************************************************************/

err_code_t falcon_tsc_set_uc_core_config( const phymod_access_t *pa, struct falcon_tsc_uc_core_config_st struct_val) {
  _update_uc_core_config_word( pa, &struct_val);
  return(wrcv_config_word(struct_val.word));
}

err_code_t falcon_tsc_set_usr_ctrl_core_event_log_level( const phymod_access_t *pa, uint8_t core_event_log_level) {
  return(wrcv_usr_ctrl_core_event_log_level(core_event_log_level));
}

err_code_t falcon_tsc_set_uc_lane_cfg( const phymod_access_t *pa, struct falcon_tsc_uc_lane_config_st struct_val) {
  _update_uc_lane_config_word( pa, &struct_val);
  return(wrv_config_word(struct_val.word));
}

err_code_t falcon_tsc_set_usr_ctrl_lane_event_log_level( const phymod_access_t *pa, uint8_t lane_event_log_level) {
  return(wrv_usr_ctrl_lane_event_log_level(lane_event_log_level));
}

err_code_t falcon_tsc_set_usr_ctrl_disable_startup( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_functions_st set_val) {
  _update_usr_ctrl_disable_functions_byte( pa, &set_val);
  return(wrv_usr_ctrl_disable_startup_functions_word(set_val.word));
}

err_code_t falcon_tsc_set_usr_ctrl_disable_startup_dfe( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_dfe_functions_st set_val) {
  _update_usr_ctrl_disable_dfe_functions_byte( pa, &set_val);
  return(wrv_usr_ctrl_disable_startup_dfe_functions_byte(set_val.byte));
}

err_code_t falcon_tsc_set_usr_ctrl_disable_steady_state( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_functions_st set_val) {
  _update_usr_ctrl_disable_functions_byte( pa, &set_val);
  return(wrv_usr_ctrl_disable_steady_state_functions_word(set_val.word));
}

err_code_t falcon_tsc_set_usr_ctrl_disable_steady_state_dfe( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_dfe_functions_st set_val) {
  _update_usr_ctrl_disable_dfe_functions_byte( pa, &set_val);
  return(wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(set_val.byte));
}



/******************************************************************/
/*  APIs to Read Core/Lane Config and User variables from uC RAM  */
/******************************************************************/

err_code_t falcon_tsc_get_uc_core_config( const phymod_access_t *pa, struct falcon_tsc_uc_core_config_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdcv_config_word());
  _update_uc_core_config_st( pa, get_val);

  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_get_usr_ctrl_core_event_log_level( const phymod_access_t *pa, uint8_t *core_event_log_level) {

  if(!core_event_log_level) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*core_event_log_level = rdcv_usr_ctrl_core_event_log_level());

  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_get_uc_lane_cfg( const phymod_access_t *pa, struct falcon_tsc_uc_lane_config_st *get_val) { 

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_config_word());
  _update_uc_lane_config_st( pa, get_val);
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_get_usr_ctrl_lane_event_log_level( const phymod_access_t *pa, uint8_t *lane_event_log_level) {

  if(!lane_event_log_level) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*lane_event_log_level = rdv_usr_ctrl_lane_event_log_level());
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_get_usr_ctrl_disable_startup( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_usr_ctrl_disable_startup_functions_word());
  _update_usr_ctrl_disable_functions_st( pa, get_val);
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_get_usr_ctrl_disable_startup_dfe( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_dfe_functions_byte());
  _update_usr_ctrl_disable_dfe_functions_st( pa, get_val);
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_get_usr_ctrl_disable_steady_state( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_usr_ctrl_disable_steady_state_functions_word());
  _update_usr_ctrl_disable_functions_st( pa, get_val);
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_get_usr_ctrl_disable_steady_state_dfe( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_dfe_functions_byte());
  _update_usr_ctrl_disable_dfe_functions_st( pa, get_val);
  return (ERR_CODE_NONE);
}



/******************************************/
/*  Serdes Register field Poll functions  */
/******************************************/

/* poll for microcontroller to populate the dsc_data register */
err_code_t falcon_tsc_poll_diag_done( const phymod_access_t *pa, uint16_t *status, uint32_t timeout_ms) {
 uint8_t loop;

 if(!status) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

 for(loop=0;loop < 100; loop++) {
	 ESTM(*status=rdv_usr_diag_status());

	 if((*status & 0x8000) > 0) {
		return(ERR_CODE_NONE);
	 }
	 if(loop>10) {
		 EFUN(falcon_tsc_delay_us(10*timeout_ms));
	 }
 }
 return(_error(ERR_CODE_DIAG_TIMEOUT));
}

/* poll for microcontroller to populate the dsc_data register */
err_code_t falcon_tsc_poll_diag_eye_data(const phymod_access_t *pa,uint32_t *data,uint16_t *status, uint32_t timeout_ms) {
 uint8_t loop;
 uint16_t dscdata;
 if(!data || !status) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

 for(loop=0;loop < 100; loop++) {
	 ESTM(*status=rdv_usr_diag_status());
	 if(((*status & 0x00FF) > 2) || ((*status & 0x8000) > 0)) {
		EFUN(falcon_tsc_pmd_uc_cmd( pa,  CMD_READ_DIAG_DATA_WORD, 0, 200));
        ESTM(dscdata = rd_uc_dsc_data());
		data[0] = _float8_to_int32((float8_t)(dscdata >>8));
		data[1] = _float8_to_int32((float8_t)(dscdata & 0x00FF));
		return(ERR_CODE_NONE);
	 }
	 if(loop>10) {
		 EFUN(falcon_tsc_delay_us(10*timeout_ms));
	 }
 }
 return(_error(ERR_CODE_DIAG_TIMEOUT));
}

#ifndef CUSTOM_REG_POLLING

/* Poll for field "uc_dsc_ready_for_cmd" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1( const phymod_access_t *pa, uint32_t timeout_ms) {
  
  uint16_t loop;
  err_code_t  err_code;
  /* read quickly for 4 tries */
#ifndef ATE_LOG
  for (loop = 0; loop < 100; loop++) {
    uint16_t rddata;
    EFUN(falcon_tsc_pmd_rdt_reg(pa,DSC_A_DSC_UC_CTRL, &rddata));
    if (rddata & 0x0080) {    /* bit 7 is uc_dsc_ready_for_cmd */
      if (rddata & 0x0040) {  /* bit 6 is uc_dsc_error_found   */
		ESTM(USR_PRINTF(("ERROR : DSC command returned error (after cmd) cmd = 0x%x, supp_info = 0x%02x !\n", rd_uc_dsc_gp_uc_req(), rd_uc_dsc_supp_info()))); 
		return(_error(ERR_CODE_UC_CMD_RETURN_ERROR));
      } 
      return (ERR_CODE_NONE);
    }     
	if(loop>10) {
		err_code = falcon_tsc_delay_us(10*timeout_ms);
		if(err_code) return(err_code);
	}
  }

#else
  { uint16_t rddata;
      USR_PRINTF(("// ATE_LOG falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1 begin\n"));
      EFUN(falcon_tsc_delay_ms(timeout_ms/10));
      EFUN(falcon_tsc_pmd_rdt_reg(pa,DSC_A_DSC_UC_CTRL, &rddata));
      if (rddata & 0x0080) {    /* bit 7 is uc_dsc_ready_for_cmd */
          if (rddata & 0x0040) {  /* bit 6 is uc_dsc_error_found   */
              ESTM(USR_PRINTF(("ERROR : DSC command returned error (after cmd) cmd = 0x%x, supp_info = 0x%02x !\n", rd_uc_dsc_gp_uc_req(), rd_uc_dsc_supp_info()))); 
              return(_error(ERR_CODE_UC_CMD_RETURN_ERROR));
          } 
          USR_PRINTF(("// ATE_LOG falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1 end\n"));
          return (ERR_CODE_NONE);
      }     
      USR_PRINTF(("// ATE_LOG falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1 timeout\n"));
  }
#endif
  USR_PRINTF(("ERROR : DSC ready for command is not working, applying workaround and getting debug info !\n"));
  ESTM(DISP(rd_uc_dsc_supp_info()));
  ESTM(DISP(rd_uc_dsc_gp_uc_req()));
  ESTM(DISP(rd_dsc_state()));
  /* artifically terminate the command to re-enable the command interface */
  wr_uc_dsc_ready_for_cmd(0x1);        
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}    

/* Poll for field "dsc_state" = DSC_STATE_UC_TUNE [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon_tsc_poll_dsc_state_equals_uc_tune( const phymod_access_t *pa, uint32_t timeout_ms) {
  
  uint16_t loop;
  err_code_t  err_code;
  /* poll 10 times to avoid longer delays later */
  for (loop = 0; loop < 100; loop++) {
    uint16_t dsc_state;
    ESTM(dsc_state = rd_dsc_state());
    if (dsc_state == DSC_STATE_UC_TUNE) {
      return (ERR_CODE_NONE);
    }    
	if(loop>10) {
		err_code = falcon_tsc_delay_us(10*timeout_ms);
		if(err_code) return(err_code);
	}
  }
  ESTM(USR_PRINTF(("DSC_STATE = %d\n", rd_dsc_state())));
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}    




/* Poll for field "micro_ra_initdone" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon_tsc_poll_micro_ra_initdone( const phymod_access_t *pa, uint32_t timeout_ms) {
  
  uint16_t loop;
  err_code_t  err_code;
  uint8_t result;
  for (loop = 0; loop <= 100; loop++) {
    ESTM(result = rdc_micro_ra_initdone());
    if (result) {
      return (ERR_CODE_NONE);
    }    
    err_code = falcon_tsc_delay_us(10*timeout_ms);
    if (err_code) {
      return (err_code);
    }
  }
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}

#endif /* CUSTOM_REG_POLLING */


err_code_t falcon_tsc_init_pram_for_uc_load( const phymod_access_t *pa, uint16_t ucode_len) {

    wrc_micro_master_clk_en(0x1);                           /* Enable clock to microcontroller subsystem */
    wrc_micro_master_rstb(0x1);                             /* De-assert reset to microcontroller sybsystem */
    wrc_micro_master_rstb(0x0);                             /* Assert reset to microcontroller sybsystem - Toggling reset*/
    wrc_micro_master_rstb(0x1);                             /* De-assert reset to microcontroller sybsystem */

    wrc_micro_ra_init(0x1);                                 /* Set initialization command to initialize code RAM */
    EFUN(falcon_tsc_poll_micro_ra_initdone( pa, 250));               /* Poll for micro_ra_initdone = 1 to indicate initialization done */
    
    wrc_micro_ra_init(0x0);                                 /* Clear initialization command */
    /* ucode_len_padded = ((ucode_len + 3) & 0xFFFC); */    /* make sure it is on 32bit boundary */
    wrc_micro_pramif_ahb_wraddr_msw(0x0000);
    wrc_micro_pramif_ahb_wraddr_lsw(0x0000);
    wrc_micro_pram_if_rstb(1);
    wrc_micro_pramif_en(1);
    ucode_len = 0;                                          /* To avoid compile warnings */

    /* Wait 8 pram clocks */
    /* write ucode into pram */
    /* Wait 8 pram clocks */
    /* call falcon_tsc_finish_pram_load */
    return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_finish_pram_load( const phymod_access_t *pa ) {
    wrc_micro_pramif_en(0);
    wrc_micro_core_clk_en(1);
   /* wrc_micro_core_rstb(1); */
    return(ERR_CODE_NONE);
}

/****************************************/
/*  Serdes Register/Variable Dump APIs  */
/****************************************/

err_code_t falcon_tsc_reg_dump( const phymod_access_t *pa ) {

  uint16_t addr, rddata;

  USR_PRINTF(("\n\n**********************************\n")); 
  USR_PRINTF(("****  SERDES REGISTER DUMP    ****\n")); 
  USR_PRINTF(("**********************************\n")); 
  USR_PRINTF(("****    ADDR      RD_VALUE    ****\n")); 
  USR_PRINTF(("**********************************\n"));

  for (addr = 0x0; addr < 0xF; addr++) {
    EFUN(falcon_tsc_pmd_rdt_reg(pa,addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }

  for (addr = 0x90; addr < 0x9F; addr++) {
    EFUN(falcon_tsc_pmd_rdt_reg(pa,addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }

  for (addr = 0xD000; addr < 0xD150; addr++) {
    EFUN(falcon_tsc_pmd_rdt_reg(pa,addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }

  for (addr = 0xD200; addr < 0xD230; addr++) {
    EFUN(falcon_tsc_pmd_rdt_reg(pa,addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }

  for (addr = 0xFFD0; addr < 0xFFE0; addr++) {
    EFUN(falcon_tsc_pmd_rdt_reg(pa,addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }
  return (ERR_CODE_NONE);
}


err_code_t falcon_tsc_uc_core_var_dump( const phymod_access_t *pa ) {

  uint8_t     addr, rddata;
  err_code_t  err_code = ERR_CODE_NONE;

  USR_PRINTF(("\n\n******************************************\n")); 
  USR_PRINTF(("**** SERDES UC CORE RAM VARIABLE DUMP ****\n")); 
  USR_PRINTF(("******************************************\n")); 
  USR_PRINTF(("****       ADDR       RD_VALUE        ****\n")); 
  USR_PRINTF(("******************************************\n")); 

  for (addr = 0x0; addr < 0xFF; addr++) {  
    rddata = falcon_tsc_rdbc_uc_var( pa, &err_code, addr);
    if (err_code) {
      return (err_code);
    }
    USR_PRINTF(("           0x%02x         0x%02x\n",addr,rddata));
  }
  return (ERR_CODE_NONE);
}


err_code_t falcon_tsc_uc_lane_var_dump( const phymod_access_t *pa ) {

  uint8_t     rddata;
  uint16_t    addr;
  err_code_t  err_code = ERR_CODE_NONE;

  USR_PRINTF(("\n\n********************************************\n")); 
  USR_PRINTF(("**** SERDES UC LANE %d RAM VARIABLE DUMP ****\n",falcon_tsc_get_lane(pa))); 
  USR_PRINTF(("********************************************\n")); 
  USR_PRINTF(("*****       ADDR       RD_VALUE        *****\n")); 
  USR_PRINTF(("********************************************\n")); 

    for (addr = 0x0; addr < LANE_VAR_RAM_SIZE; addr++) {  
      rddata = falcon_tsc_rdbl_uc_var( pa, &err_code, addr);
      if (err_code) {
        return (err_code);
      }
      USR_PRINTF(("            0x%03x         0x%02x\n",addr,rddata));
    }
  return (ERR_CODE_NONE);
}



/************************/
/*  Serdes API Version  */
/************************/

err_code_t falcon_tsc_version( const phymod_access_t *pa, uint32_t *api_version) {

	if(!api_version) {
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
	}
	*api_version = 0xA1010C;
	return (ERR_CODE_NONE);
}


/***************************************/
/*  API Function to Read Event Logger  */
/***************************************/

err_code_t falcon_tsc_read_event_log( const phymod_access_t *pa, uint8_t *trace_mem,enum srds_event_log_display_mode_enum display_mode) {

	/* validate input arguments */
	if (trace_mem == NULL || (display_mode >= EVENT_LOG_MODE_MAX)) 
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

	/* stop writing event log */
	EFUN(falcon_tsc_event_log_stop( pa ));

	EFUN(falcon_tsc_event_log_readmem( pa, trace_mem));

	EFUN(falcon_tsc_event_log_display( pa, trace_mem, display_mode));

	return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_event_log_stop( const phymod_access_t *pa ) {


	USR_PRINTF(("\n\n********************************************\n")); 
	USR_PRINTF(("**** SERDES UC TRACE MEMORY DUMP ***********\n")); 
	USR_PRINTF(("********************************************\n")); 

	/* Start Read to stop uC logging and read the word at last event written by uC */
	EFUN(falcon_tsc_pmd_uc_cmd( pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_START, 10));

	return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_event_log_readmem( const phymod_access_t *pa, uint8_t *trace_mem) {
	uint8_t uc_dsc_supp_info;
	uint16_t addr=0,read_idx=0,trace_mem_size=0;
	/* validate input arguments */
	if (trace_mem == NULL)  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

	ESTM(read_idx = rdcv_trace_mem_rd_idx());
	USR_PRINTF(("\n  DEBUG INFO: trace memory read index = 0x%04x\n", read_idx));

	trace_mem_size = 768;
	USR_PRINTF(("  DEBUG INFO: trace memory size = 0x%04x\n\n", trace_mem_size));

	do {
		/* Read Next */
		EFUN(falcon_tsc_pmd_uc_cmd( pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_NEXT, 10));

		if (addr >= trace_mem_size) {
			return (ERR_CODE_INVALID_EVENT_LOG_READ);
		}
		else {
			addr++;
		}

		ESTM(*(trace_mem++) = (uint8_t) rd_uc_dsc_data());
		ESTM(uc_dsc_supp_info = rd_uc_dsc_supp_info());
	} while (uc_dsc_supp_info != 1);

	/* Read Done to resume logging  */
	EFUN(falcon_tsc_pmd_uc_cmd( pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_DONE, 10));

	return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_event_log_display( const phymod_access_t *pa, uint8_t *trace_mem,enum srds_event_log_display_mode_enum display_mode) {
#define MAX_ENTRY_SIZE   (7)
#define NOR_ENTRY_SIZE   (4)
	uint16_t trace_mem_size=0;
    uint8_t num_lanes=0;
	uint8_t is_ref_time_found=0,is_last_event_timewrap=0;
	uint8_t post_cursor=0,curr_cursor=0,prev_cursor=0;
	uint16_t addr=0, rr=0, prev_rr=0;
	uint8_t cc=0;
	uint16_t time=0,ref_time=0,num_time_wraparound=0,this_num_time_wraparound=0;
	USR_DOUBLE prev_time=0,curr_time=0;
	uint8_t word_per_row=8;
	uint8_t prev_event=0;
    uint8_t supp_info[MAX_ENTRY_SIZE-NOR_ENTRY_SIZE];
    char lane_id_str[256];
    /* char *lane_id_str; */
	/* validate input arguments */
	if (trace_mem == NULL || (display_mode >= EVENT_LOG_MODE_MAX))
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

	trace_mem_size = 768;
    num_lanes = 4;

	/* output */
	if (display_mode == EVENT_LOG_HEX || display_mode == EVENT_LOG_HEX_AND_DECODED) {
		/* print the 16-bit words in Hex format to screen, 8 words per row */
		for (rr=0; rr<trace_mem_size; rr+=(2*word_per_row)) {
			for (cc=0; cc<2*word_per_row; cc+=2) {
				USR_PRINTF(("  0x%02x%02x", *(trace_mem+rr+cc), *(trace_mem+rr+cc+1)));
			}
			USR_PRINTF(("    %d\n",rr));
		}
	}
	if (display_mode == EVENT_LOG_DECODED || display_mode == EVENT_LOG_HEX_AND_DECODED) {
		/* decode for level 1 events */

		/* print a text log of the events going backwards in time, showing time as T-10.340ms, where T is when the Start Read happened. */
		addr = 0;
		while (addr < trace_mem_size-8) {                                                            /* last 8-byte reserved for trace memory wraparound */
			
			if (*(trace_mem+addr) == 0x0) {                                                          /* reach event log end */
				USR_PRINTF(("\n========== End of Event Log ==================\n"));
				break;
			}

			if (*(trace_mem+addr) == 0xff) {                                                         /* timestamp wraparound event handling */

				this_num_time_wraparound = *(trace_mem+(++addr));
				this_num_time_wraparound = ((this_num_time_wraparound<<8) | *(trace_mem+(++addr)));
				num_time_wraparound += this_num_time_wraparound;

				if (!is_last_event_timewrap) {                                                         /* display the rest of previous event info */
					USR_PRINTF((","));
					_falcon_display_event( pa, prev_event,(uint8_t)prev_rr,prev_cursor,curr_cursor,post_cursor,supp_info);
					is_last_event_timewrap = 1;
				}

				USR_PRINTF(("\n  %5d timestamp wraparound(s). \n\n", this_num_time_wraparound));
				this_num_time_wraparound = 0;
				
				addr++;
				continue; 
			}
			else {
				cc = (trace_mem[addr] & 0x1f);                                                         /* lane id */
                if (cc >= num_lanes) {
                    USR_PRINTF(("\n\n  Incorrect lane ID. Terminating event log display for current core... \n\n"));
                    break;
                }

				rr = (uint16_t) ((trace_mem[addr]>>5) & 0x7);                                          /* determine event entry length */
				if (rr > MAX_ENTRY_SIZE) return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

				time = *(trace_mem+(++addr));                                                          /* timestamp */
				time = ((time<<8) | *(trace_mem+(++addr)));
				if (is_ref_time_found == 0) {                                                          /* determine the reference time origin */
					is_ref_time_found = 1;
					ref_time = time;
				}
				else {
#ifdef SERDES_API_FLOATING_POINT
					curr_time = (time-ref_time-(num_time_wraparound*65536))/100.0;   
#else
                    curr_time = (time-ref_time-(num_time_wraparound*65536))/100;
#endif
					if (!is_last_event_timewrap) {
#ifdef SERDES_API_FLOATING_POINT
						USR_PRINTF((" (+%.2f),", prev_time-curr_time));
#else
                        USR_PRINTF((" (+%d),", prev_time-curr_time));
#endif
                        _falcon_display_event( pa, prev_event,(uint8_t)prev_rr,prev_cursor,curr_cursor,post_cursor,supp_info);
					}
					else {
						is_last_event_timewrap = 0;
					}
				}

                falcon_tsc_uc_lane_idx_to_system_id(lane_id_str, cc);
                USR_PRINTF(("  Lane %s: ",lane_id_str));
#ifdef SERDES_API_FLOATING_POINT
				USR_PRINTF(("  t= %.2f ms", curr_time));
#else
				USR_PRINTF(("  t= %d ms", curr_time));
#endif                
                prev_time = curr_time;
				prev_rr = rr;

				prev_event = *(trace_mem+(++addr));
				switch (prev_event) {
				case EVENT_CODE_CL72_READY_FOR_COMMAND:
				case EVENT_CODE_EACH_WRITE_TO_CL72_TX_CHANGE_REQUEST:
					if (rr != NOR_ENTRY_SIZE) {
						post_cursor = (*(trace_mem+(++addr))&0x30)>>4;
						curr_cursor = (*(trace_mem+addr)&0x0C)>>2;
						prev_cursor = *(trace_mem+addr)&0x03;
						addr--;                                                         /* rewind to populate supplement info */
					}
					break;
				case EVENT_CODE_GENERAL_EVENT_0:
				case EVENT_CODE_GENERAL_EVENT_1:
				case EVENT_CODE_GENERAL_EVENT_2:
					post_cursor = *(trace_mem+(++addr));
					prev_cursor = *(trace_mem+(++addr));
					addr -= 2;                                                         /* rewind to populate supplement info */
					break;
                case EVENT_CODE_ERROR_EVENT:
                    post_cursor = *(trace_mem+(++addr));
                    addr--;
                    break;
                case EVENT_CODE_SM_STATUS_RESTART:
                    post_cursor = *(trace_mem+(++addr));
                    addr--;
                    break;
				default:
					break;
				}
				
				/* retrieve supplement info, if any */
				for(cc=0; cc<rr-NOR_ENTRY_SIZE; cc++) {
					supp_info[cc] = *(trace_mem+(++addr));
				}
				
				addr++;
			}
		}
	}

	return(ERR_CODE_NONE);
}


err_code_t falcon_tsc_display_state ( const phymod_access_t *pa ) {

  err_code_t err_code;
  err_code = ERR_CODE_NONE;

  if (!err_code) err_code = falcon_tsc_display_core_state( pa );
  if (!err_code) err_code = falcon_tsc_display_lane_state_hdr( pa );
  if (!err_code) err_code = falcon_tsc_display_lane_state( pa );

  return (err_code);
}

err_code_t falcon_tsc_display_config ( const phymod_access_t *pa ) {

  err_code_t err_code;
  err_code = ERR_CODE_NONE;

  if (!err_code) err_code = falcon_tsc_display_core_config( pa );
  if (!err_code) err_code = falcon_tsc_display_lane_config( pa );

  return (err_code);
}


/**********************************************/
/*  Temperature forcing and reading			  */
/**********************************************/

err_code_t falcon_tsc_force_die_temperature ( const phymod_access_t *pa, int16_t die_temp) {
    /* Formula for PVTMON: T = 410.04-0.48705*reg10bit (from PVTMON Analog Module Specification v5.0 section 6.2) */
    int32_t die_temp_int;
    err_code_t err_code = 0;
    if (die_temp>130)
        die_temp = 130;
    if (die_temp<-45)
        die_temp = -45;

    die_temp_int = (int32_t)die_temp;
    EFUN(wrcv_temp_frc_val((uint16_t)((431045 - ((die_temp_int<<10) + (die_temp_int<<4) + (die_temp_int<<3) + (die_temp_int<<1) + die_temp_int))>>9)));
    
    return(err_code);    
}

err_code_t falcon_tsc_read_die_temperature ( const phymod_access_t *pa, int16_t *die_temp) {
    /* Formula for PVTMON: T = 410.04-0.48705*reg10bit (from PVTMON Analog Module Specification v5.0 section 6.2) */
    int32_t die_temp_sensor_reading;
    
    EFUN(falcon_tsc_pmd_uc_cmd( pa, CMD_UC_DBG,CMD_UC_DBG_DIE_TEMP,100));
    ESTM(die_temp_sensor_reading=rd_uc_dsc_data());
       
    *die_temp = (int16_t)((420697 - ((die_temp_sensor_reading<<8) + (die_temp_sensor_reading<<7) + (die_temp_sensor_reading<<6) + (die_temp_sensor_reading<<5) + (die_temp_sensor_reading<<4) + (die_temp_sensor_reading<<1) + die_temp_sensor_reading))>>10); 
    
    return(ERR_CODE_NONE);    
}


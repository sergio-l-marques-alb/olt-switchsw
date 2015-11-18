/***********************************************************************************/
/***********************************************************************************/
/*  File Name     :  falcon_api_functions.c                                        */
/*  Created On    :  29/04/2013                                                    */
/*  Created By    :  Kiran Divakar                                                 */
/*  Description   :  APIs for Serdes IPs                                           */
/*  Revision      :  $Id: falcon_api_functions.c 651 2014-08-04 23:22:26Z kirand $ */
/*                                                                                 */
/*  All Rights Reserved                                                            */
/*  No portions of this material may be reproduced in any form without             */
/*  the written permission of:                                                     */
/*      Broadcom Corporation                                                       */
/*      5300 California Avenue                                                     */
/*      Irvine, CA  92617                                                          */
/*                                                                                 */
/*  All information contained in this document is Broadcom Corporation             */
/*  company private proprietary, and trade secret.                                 */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/
/*
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
* ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/

/** @file falcon_api_functions.c
 * Implementation of API functions
 */

#ifdef _MSC_VER
/* Enclose all standard headers in a pragma to remove warings for MS compiler */
#pragma warning( push, 0 )
#endif

#ifdef SERDES_API_FLOATING_POINT
/* #include <math.h> */
#endif
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#include "falcon_tsc_functions.h"

#include "falcon_tsc_field_access_c.h"
#include "falcon_tsc_internal_c.h"
#include "falcon_tsc_dv_functions_c.h"

#ifdef MERLIN
 #ifndef MERLIN_COUNTACH
  #include "merlin_pll_config.c"
 #endif
  #include "merlin_tx_analog_functions.c"
#endif

#ifdef EAGLE
  #include "eagle_pll_config.c"
#endif

#ifdef FALCON
  #ifdef FALCON_TSC
    #include "falcon_tsc_pll_config_c.h"
    #define UINT32_MAX (4294967295UL)
  #endif
  #ifdef FALCON_FURIA
    #include "falcon_furia_pll_config.c"
  #endif
#endif

#ifdef SERDES_EVAL
extern err_code_t _eval_ucode_debug(void);
err_code_t global_err_code;
#endif


/************************************/
/*  Display Eye Scan                */
/************************************/

/* This is best method for terminal ASCII display */
err_code_t falcon_display_eye_scan(const phymod_access_t *pa) {
  int8_t y;
  err_code_t err_code;
  uint16_t status = 0;
  uint32_t stripe[64];

  /* start horizontal acquisition */
  err_code = falcon_tsc_meas_eye_scan_start(pa, EYE_SCAN_HORIZ);
  if (err_code) {
    falcon_tsc_meas_eye_scan_done(pa );
    return (err_code);
  }

  falcon_tsc_display_eye_scan_header(pa, 1);
  /* display stripe */
#ifdef FALCON
  for (y = 62;y>=-62;y=y-2) { 
#else
   for (y = 31;y>=-31;y=y-1) { 
#endif
 	err_code = falcon_tsc_read_eye_scan_stripe(pa, &stripe[0], &status);
	if (err_code) {
		falcon_tsc_meas_eye_scan_done(pa );
		return (err_code);
	}

    falcon_tsc_display_eye_scan_stripe(pa, y,&stripe[0]);
	USR_PRINTF(("\n"));  
  }  
  /* stop acquisition */
  err_code = falcon_tsc_meas_eye_scan_done(pa );
  if (err_code) {
	return (err_code);
  }

  falcon_tsc_display_eye_scan_footer(pa, 1);

  return(ERR_CODE_NONE);
}

/* This function is for Falcon and is configured for passive mode */
err_code_t falcon_tsc_meas_lowber_eye(const phymod_access_t *pa, const struct falcon_tsc_eyescan_options_st eyescan_options, uint32_t *buffer) {
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
        EFUN(falcon_pmd_uc_cmd_with_data(pa, CMD_DIAG_EN,CMD_UC_DIAG_GET_EYE_SAMPLE,((uint16_t)x)<<8 | (uint8_t)y,200));
        /* wait for sample complete */
#if 0
        do {
            falcon_delay_us(1000);
            ESTM(status=rdv_usr_diag_status());
            USR_PRINTF(("status=%04x\n",status));
        } while((status & 0x8000) == 0); 
#else
        EFUN(falcon_poll_diag_done(pa, &status, (((uint32_t)timeout)<<7)*10 + 20000));
#endif
#ifdef FALCON
        EFUN(falcon_prbs_err_count_ll(pa, &errors));
#else
        {
            uint8_t osr_mode; ESTM(osr_mode = rd_osr_mode());
            if(osr_mode == SERDES_ENUM_OSX1) {
                EFUN(falcon_prbs_err_count_ll(pa, &errors));
            } else if(osr_mode == SERDES_ENUM_OSX2) {
                ESTM(errors = ((uint32_t)rdv_usr_var_msb())<<16 | rdv_usr_var_lsb());
            } else {
                USR_PRINTF(("Error: 2D eye scan is not supported for OSR Mode > 2\n"));
                return(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
            }
        }
#endif
        /* USR_PRINTF(("(%d,%d) = %u\n",x,y,errors & 0x7FFFFFF)); */

        buffer[i] = errors & 0x7FFFFFFF;
     
        i++;
        USR_PRINTF(("."));
    }
    USR_PRINTF(("\n"));        
  }
  USR_PRINTF(("\n"));
  EFUN(falcon_tsc_meas_eye_scan_done(pa ));
  EFUN(wrv_usr_sts_micro_stopped(stopped_state));
  return(ERR_CODE_NONE);
}


/* Display the LOW BER EyeScan */
err_code_t falcon_tsc_display_lowber_eye(const phymod_access_t *pa, const struct falcon_tsc_eyescan_options_st eyescan_options, uint32_t *buffer) {
    int8_t x,y,i,z;
    int16_t j; /* buffer pointer */
    uint32_t val;
    uint8_t overflow;
    uint32_t limits[13]; /* allows upto 400 sec test time per point (1e-13 BER @ 10G) */
#ifdef FALCON
    int8_t data_thresh; 
#endif

    if(!buffer) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
     }
#ifdef FALCON
    ESTM(data_thresh = rd_rx_data_thresh_sel());
#endif
    /* Calculate initial total bitcount BER 1e-1 */
    limits[0] = _mult_with_overflow_check(pa, eyescan_options.linerate_in_khz/10, eyescan_options.timeout_in_milliseconds, &overflow);
    if(overflow > 0) {
        limits[0] = UINT32_MAX;
        USR_PRINTF(("Very long timout_in_milliseconds results in saturation of Err counter can cause in accurate results\n"));
    }

    for(i=1;i<13;i++) {            /* calculate thresholds */
        limits[i] = limits[i-1]/10;
    }
    
    falcon_tsc_display_eye_scan_header(pa, 1);
	j = 0;
    for (y = eyescan_options.vert_max;y>=eyescan_options.vert_min;y=y-eyescan_options.vstep) {
#ifndef FALCON
      ESTM(USR_PRINTF(("%6dmV : ",_ladder_setting_to_mV(y, rd_p1_thresh_sel()))));
#else
        ESTM(USR_PRINTF(("%6dmV : ",_ladder_setting_to_mV(y,0)-_ladder_setting_to_mV(data_thresh,0))));
#endif
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
   falcon_tsc_display_eye_scan_footer(pa, 1);
   return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_meas_eye_scan_start(const phymod_access_t *pa, uint8_t direction) {
  err_code_t err_code;
  uint8_t lock;
  ESTM(lock = rd_pmd_rx_lock());
  if(lock == 0) {
      USR_PRINTF(("Error: No PMD_RX_LOCK on lane requesting 2D eye scan\n"));
      return(ERR_CODE_DIAG_SCAN_NOT_COMPLETE);
  }
  if(direction == EYE_SCAN_VERTICAL) {
	err_code = falcon_pmd_uc_diag_cmd(pa, CMD_UC_DIAG_START_VSCAN_EYE,200);
  } else {
	  err_code = falcon_pmd_uc_diag_cmd(pa, CMD_UC_DIAG_START_HSCAN_EYE,200);
  }
  	if (err_code) {
		return (err_code);
	}

  return(ERR_CODE_NONE);
}

err_code_t falcon_tsc_read_eye_scan_stripe(const phymod_access_t *pa, uint32_t *buffer,uint16_t *status){
	int8_t i;
	uint32_t val[2] = {0,0};
    err_code_t err_code;
	uint16_t sts = 0;

	if(!buffer || !status) {
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
	}
	*status = 0;
	for(i=0;i<32;i++) {
		err_code = falcon_poll_diag_eye_data(pa, &val[0], &sts, 200);
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

err_code_t falcon_tsc_display_eye_scan_stripe(const phymod_access_t *pa, int8_t y,uint32_t *buffer) {

  const uint32_t limits[7] = {1835008, 183501, 18350, 1835, 184, 18, 2};
  int8_t x,i;
#ifndef FALCON
  int8_t data_thresh;
#endif
  int16_t level;

#ifdef FALCON
  /* ESTM(data_thresh = rd_rx_data_thresh_sel()); */
  level = _ladder_setting_to_mV(y,0);
#else
  ESTM(data_thresh = rd_p1_thresh_sel());
  level = _ladder_setting_to_mV(y,data_thresh);
#endif

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

#ifndef FALCON
/* Measure LOW BER Eye Scan.   */
err_code_t falcon_meas_eye_density_data(const phymod_access_t *pa, const struct falcon_tsc_eyescan_options_st eyescan_options, int32_t *buffer,uint16_t *buffer_size) {
  int8_t y,x,z;
  int16_t i;
  int8_t hzcnt;
  /*uint32_t errcnt; */
  /*uint8_t lock_lost; */
  err_code_t err_code;
  
  if(!buffer || !buffer_size) {
	  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  err_code = falcon_pmd_uc_diag_cmd(pa, CMD_UC_DIAG_DENSITY,2000);
  if (err_code) {
    return (err_code);
  }
  i = 0;hzcnt=0;
  ESTM(USR_PRINTF(("Calculating %d\n",rd_cnt_d_minus_m1())));
  for (y = eyescan_options.vert_max;y>=eyescan_options.vert_min;y=y-eyescan_options.vstep) {   
    _set_p1_threshold(y);
    _move_clkp1_offset(eyescan_options.horz_min-1);   /* Walk back 32 steps */
    _move_clkp1_offset(1);                              /* Walk forward one (net -31). This sets up the registers for 1 write increments */
	hzcnt = eyescan_options.horz_min;
	for (x=eyescan_options.horz_min;x<=eyescan_options.horz_max;x=x+eyescan_options.hstep) {
		 _trnsum_clear_and_enable();
         err_code = falcon_poll_dsc_state_equals_uc_tune(pa, 2000);
	     if (err_code) 
           return(err_code);
	     ESTM(buffer[i] = (((int32_t)rd_trnsum_high())<<10) | rd_trnsum_low());
		 USR_PRINTF(("D %d\n",(uint32_t)buffer[i]));
		 i++;
	     for(z=1;z<=eyescan_options.hstep;z++) {
		   wr_rx_pi_manual_strobe(1); /* hstep */
		   hzcnt++;
		 }
		 USR_PRINTF(("."));
    }
	_move_clkp1_offset(-hzcnt);     /* Walk back center */

    USR_PRINTF(("\n"));        
  }
  USR_PRINTF(("\n");  *buffer_size = i);
  err_code = falcon_tsc_meas_eye_scan_done(pa );
  if (err_code) {
	return (err_code);
  }

  return(ERR_CODE_NONE);
}

err_code_t falcon_display_eye_density_data(const phymod_access_t *pa, const struct falcon_tsc_eyescan_options_st eyescan_options, int32_t *buffer,uint16_t buffer_size) {
	int8_t x,y,i,z;
	int16_t j; /* buffer pointer */
	int32_t maxval,val;
	uint8_t range;

	ESTM(range = rd_p1_thresh_sel());

	if(!buffer) {
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
	}
  
        /* Find maxval */
	i=0;
	for(x=eyescan_options.horz_min;x<=eyescan_options.horz_max;x=x+eyescan_options.hstep)
		i++;/* determine row length */
        maxval = 0;j=buffer_size; /* init to last row */
        for (y=eyescan_options.vert_min;y<=eyescan_options.vert_max;y=y+eyescan_options.vstep) {
           for (x=eyescan_options.horz_min;x<=eyescan_options.horz_max;x=x+eyescan_options.hstep) {
             if (y==eyescan_options.vert_max) {
               buffer[i] = 0;   /* zero out top row */
               i--;  /* count number of samples in row */
             } else {
               val = buffer[j] - buffer[j-i];    /* Subtract from above row */
               if (val<0) val = 0;
               if (val>maxval) maxval = val;
               buffer[j] = val;
               j--;
			 }

           }
        }  
	falcon_tsc_display_eye_scan_header(pa, 1);
        for (y = eyescan_options.vert_max-1;y>=eyescan_options.vert_min;y=y-eyescan_options.vstep) {
          USR_PRINTF(("%6dmV : ",(_ladder_setting_to_mV(y, range) + _ladder_setting_to_mV(y+1, range))/2));    
			for(z=-31;z<eyescan_options.horz_min;z++) {
				USR_PRINTF((" ")); /* add leading spaces if any */
			}
           for (x=eyescan_options.horz_min;x<=eyescan_options.horz_max;x=x+eyescan_options.hstep) {
            val = buffer[j]/(maxval/16);             /* Normalize peak to 15 */
            if (val>15) val = 15;
		    for(z=1;z<=eyescan_options.hstep;z++) {
				if(z==1) {
					if (val) {
						USR_PRINTF(("%X",(uint32_t)val));
                                        }
					else {
						if ((x%5)==0 && ((y+3)%5)==0) {
						  USR_PRINTF(("+"));
                                                }
					        else if ((x%5)!=0 && ((y+3)%5)==0) {
					          USR_PRINTF(("-"));
                                                }
                                                else if ((x%5)==0 && ((y+3)%5)!=0) {
						 USR_PRINTF((":"));
                                                }
					        else {
						 USR_PRINTF((" "));
                                                }
					}
				} else {
					USR_PRINTF((" "));
                                }
			}
            j++;
          }
          USR_PRINTF(("\n"));  
        }  
	falcon_tsc_display_eye_scan_footer(pa, 1);
	return(ERR_CODE_NONE);
}
#endif
err_code_t falcon_tsc_display_eye_scan_header(const phymod_access_t *pa, int8_t i) {
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

err_code_t falcon_tsc_display_eye_scan_footer(const phymod_access_t *pa, int8_t i) {
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


/*eye_scan_status_t falcon_read_eye_scan_status(pa, void) */
err_code_t falcon_read_eye_scan_status(const phymod_access_t *pa, uint16_t *status) {

   if(!status) {
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
	}

   ESTM(*status=rdv_usr_diag_status());	

	return(ERR_CODE_NONE);
}


err_code_t falcon_tsc_meas_eye_scan_done(const phymod_access_t *pa) {
  err_code_t err_code;
  err_code = falcon_pmd_uc_diag_cmd(pa, CMD_UC_DIAG_DISABLE,200);
  if (err_code) return(err_code);
  return(ERR_CODE_NONE);
}


err_code_t falcon_start_ber_scan_test(const phymod_access_t *pa, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control) {
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
    ESTM(falcon_tsc_pmd_uc_cmd(pa, CMD_CAPTURE_BER_START, ber_scan_mode,500));
    return(ERR_CODE_NONE);
}

err_code_t falcon_read_ber_scan_data(const phymod_access_t *pa, uint32_t *errors, uint32_t *timer_values, uint8_t *cnt, uint32_t timeout) {
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
		err_code = falcon_tsc_pmd_uc_cmd(pa,  CMD_READ_DIAG_DATA_WORD, 0, timeout);
		if(err_code) return(err_code);
		ESTM(dataword = rd_uc_dsc_data());           /* LSB contains 2 -4bit nibbles */
		time_byte = (uint8_t)(dataword>>8);    /* MSB is time byte */
		prbs_multi = (uint8_t)dataword & 0x0F; /* split nibbles */
		time_multi = (uint8_t)dataword>>4;
		/* Read 1 bytes of data */
		err_code = falcon_tsc_pmd_uc_cmd(pa,  CMD_READ_DIAG_DATA_BYTE, 0, timeout);
		if(err_code) return(err_code);
		ESTM(prbs_byte = (uint8_t)rd_uc_dsc_data());
		errors[i] = _float12_to_uint32(pa, prbs_byte,prbs_multi); /* convert 12bits to uint32 */
		timer_values[i] = (_float12_to_uint32(pa, time_byte,time_multi)<<3);
		/*USR_PRINTF(("Err=%d (%02x<<%d); Time=%d (%02x<<%d)\n",errors[i],prbs_byte,prbs_multi,timer_values[i],time_byte,time_multi<<3));*/
		/*if(timer_values[i] == 0 && errors[i] == 0) break;*/
	}

  return(ERR_CODE_NONE);
}


/* This is good example function to do BER extrapolation */
err_code_t falcon_eye_margin_proj(const phymod_access_t *pa, USR_DOUBLE rate, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control) {
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
	EFUN(falcon_start_ber_scan_test(pa, ber_scan_mode, timer_control, max_error_control));
    ESTM(offset_start = rd_uc_dsc_data());
	if(verbose > 2) USR_PRINTF(("offset_start = %d:%dmV\n",offset_start,_ladder_setting_to_mV((int8_t)offset_start,0)));
	if(verbose > 2) USR_PRINTF(("start done\n"));

	/* This wait is VERY LONG and should be replaced with interupt or something */
	if(verbose > 5) {
		do {
			EFUN(falcon_delay_us(2000000));
            ESTM(sts = rdv_usr_diag_status());
			USR_PRINTF(("sts=%04x\n",sts));
			
		} while ((sts & 0x8000) == 0);
	} else {
		USR_PRINTF(("Waiting for measurement time\n"));
        EFUN(falcon_poll_diag_done(pa, &sts,timer_control*2000));
	}
	if(verbose > 2) USR_PRINTF(("delay done\n"));

	EFUN(falcon_read_ber_scan_data(pa,  &errs[0], &time[0], &cnt, 2000));
	
	if(verbose > 2) USR_PRINTF(("read done cnt=%d\n",cnt));

	EFUN(falcon_tsc_pmd_uc_cmd(pa, CMD_CAPTURE_BER_END,0x00,200));

	if(verbose > 2) USR_PRINTF(("end function done\n"));
	/* if(cnt == 1) {*/
	/*	USR_PRINTF(("Not enough points found to extrapolate BER\n"));*/
	/*	return(ERR_CODE_NONE);*/
	/*} */

   	EFUN(falcon_display_ber_scan_data(pa, rate, ber_scan_mode, &errs[0], &time[0],(uint8_t)_abs(offset_start)));
	
	if(verbose > 2) USR_PRINTF(("display done\n"));

	return(ERR_CODE_NONE);
}

err_code_t falcon_display_ber_scan_data(const phymod_access_t *pa, USR_DOUBLE rate, uint8_t ber_scan_mode, uint32_t *total_errs, uint32_t *total_time, uint8_t max_offset) {
#ifdef SERDES_API_FLOATING_POINT
	/* 'margins_mv[]' vector maps the p1 threshold code with actual mV
		Only relevant when mode=0
		This is not totally linear: for code 0~25 step=6mV; code 25~30 step=18mV; code 30~31 step=12
		'margins_mv[]' is valid only for Merlin. This vector would need to be modified accordingly for different Serdes
	USR_DOUBLE margins_mv[] = {0,6,12,18,24,30,36,42,48,54,60,66,72,78,84,
	                       90,96,102,108,114,120,126,132,138,144,150,168,186,204,222,240,252};
	const USR_DOUBLE narrow_margins_mv[] = {0,3.6,7.2,10.8,14.4,18,21.6,25.5,28.8,32.4,36,39.6,43.2,46.8,50.4,
	                                    54,57.6,61.2,64.8,68.4,72,75.6,79.2,82.8,86.4,90,100.8,111.6,122.4,133.2,144,151.2}; */
#ifndef FALCON
	const USR_DOUBLE intrusive_margins_mv[] = {2,6,10,14,18,22,26,30,32,36,40,44,48,52,56,60};
#endif
    const unsigned int HI_CONFIDENCE_ERR_CNT = 100;      /* bit errors */
    const unsigned int HI_CONFIDENCE_MIN_ERR_CNT = 20;   /* bit errors */

    /* BER confidence scale */
    const USR_DOUBLE ber_conf_scale[101] = { 
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
		1.2163 };

    /* Define variables */
	USR_DOUBLE lbers[DIAG_MAX_SAMPLES];				/* Internal linear scale sqrt(-log(ber)) */
	USR_DOUBLE margins[DIAG_MAX_SAMPLES];				/* Eye margin @ each measurement */
	USR_DOUBLE bers[DIAG_MAX_SAMPLES];				/* computed bit error rate */
	uint32_t i;
	int8_t offset[DIAG_MAX_SAMPLES];
	int8_t mono_flags[DIAG_MAX_SAMPLES];
	
	int8_t direction;
	int8_t verbose = 2;
	int8_t delta_n;
	USR_DOUBLE Exy = 0.0;
	USR_DOUBLE Eyy = 0.0;
	USR_DOUBLE Exx = 0.0;
	USR_DOUBLE Ey  = 0.0;
	USR_DOUBLE Ex  = 0.0;
	USR_DOUBLE alpha;
	USR_DOUBLE beta;
	USR_DOUBLE proj_ber;
	USR_DOUBLE proj_margin_12;
	USR_DOUBLE proj_margin_15;
	USR_DOUBLE proj_margin_18;
	USR_DOUBLE sq_err1, sq_err2;
	USR_DOUBLE ierr;
	int8_t start_n;
	int8_t stop_n;
	int8_t low_confidence;
	int8_t loop_index;
	USR_DOUBLE outputs[4];
	int8_t n_mono = 0;
	int8_t eye_cnt = 1;
	int8_t hi_confidence_cnt = 0;
	int8_t heye;
	int8_t first_good_ber_idx = -1;
	int8_t first_small_errcnt_idx = -1;
	uint8_t range250;
	uint8_t intrusive;

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
	if(verbose > 2) USR_PRINTF(("Max Offset = %d\n",max_offset));

    if (heye == 1) {
 		if (verbose > 1) {
			if (direction==-1) USR_PRINTF(("********** HORIZONTAL PROJECTION: LEFT SIDE ******************\n"));
			if (direction==1) USR_PRINTF(("********** HORIZONTAL PROJECTION: RIGHT SIDE *****************\n"));
		}
    } else {
		if (verbose > 1) {
			if (direction==-1) USR_PRINTF(("********** VERTICAL PROJECTION: BOTTOM ***********************\n"));
			if (direction==1) USR_PRINTF(("********** VERTICAL PROJECTION: TOP **************************\n"));
		}
    }

	/* Computer BER and extrapolate */
	i=0;
	do {
 		if(heye == 1) {
			offset[i] = (int8_t)(max_offset-i);
			margins[i] = direction*offset[i]*1000.0/64.0;
		} else {
			offset[i] = (int8_t)(max_offset-i);
			if(intrusive) {
#ifdef FALCON
				margins[i] = direction*_ladder_setting_to_mV(offset[i], 0);
#else
                margins[i] = direction*intrusive_margins_mv[offset[i]];
#endif
			} else {
				margins[i] = direction*_ladder_setting_to_mV(offset[i], range250);
			}
		}
        if (total_errs[i] == 0) {
            bers[i] = 1.0/(((USR_DOUBLE)total_time[i])*0.00001*rate);
            if (verbose > 0) {
				if (heye == 1) {
					USR_PRINTF(("BER @ %4.0f mUI < 1e%0.2f (%u errors in %0.2f sec)\n", 
							margins[i], log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
				} else {
					USR_PRINTF(("BER @ %4.0f mV < 1e%0.2f (%u errors in %0.2f sec)\n", 
							margins[i], log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
				}
            }
        } else {
            bers[i] = (USR_DOUBLE)total_errs[i]/(((USR_DOUBLE)total_time[i])*0.00001*rate);
            if (verbose > 0) {
				if (heye == 1) {
					USR_PRINTF(("BER @ %4.0f mUI = 1e%0.2f (%u errors in %0.2f sec)\n", 
							margins[i], log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
				} else {
					USR_PRINTF(("BER @ %4.0f mV = 1e%0.2f (%u errors in %0.2f sec)\n", 
									margins[i], log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
				}
            }
        }

        /* Find the first data point with good BER (1e-8 or better) */
        if ((floor(log10(bers[i])) <= -6) && (first_good_ber_idx == -1)) {
            first_good_ber_idx = (int8_t)i;
            if (verbose > 2) {
                USR_PRINTF(("Found first good ber idx at %d, ber = 1e%f\n", i, log10(bers[i])));
            }
        }

        /* Detect and record nonmonotonic data points */
        if ((i > 0) && (log10(bers[i]) > log10(bers[i-1]))) {
            mono_flags[i] = 1;
            if (first_good_ber_idx != -1) {
                n_mono++;
            }
        }

        /* Determine high-confidence iterations */
        if (total_errs[i] >= HI_CONFIDENCE_ERR_CNT) {
            hi_confidence_cnt++;
        } else if ((total_errs[i] < HI_CONFIDENCE_MIN_ERR_CNT) && (first_small_errcnt_idx == -1)) {
            /* find the first data point with small error count */
            first_small_errcnt_idx = (int8_t)i;
            if (verbose > 2) {
                USR_PRINTF(("Found first small errcnt idx at %d, errors = %d\n", i, total_errs[i]));
            }
        }
      i++;if (verbose > 2) USR_PRINTF(("i=%d:%d:%d\n",i,total_errs[i],total_time[i]));
	} while(((total_errs[i] != 0) || (total_time[i] != 0)) && (i<=max_offset));
	eye_cnt = (int8_t)i;

	/* Find the lowest data point to use, currently based on at HI_CONFIDENCE_MIN_ERR_CNT (20) errors */
    if (first_small_errcnt_idx == -1) {
        stop_n = eye_cnt;
    } else {
        stop_n = first_small_errcnt_idx;
    }
        
    /* Find the highest data point to use, currently based on at least 1e-8 BER level */
    if (first_good_ber_idx == -1) {
        start_n = stop_n;
    } else {
        start_n = first_good_ber_idx;
    }

    if (verbose > 2) {
        USR_PRINTF(("start_n: %d, stop_n: %d, eye_cnt: %d, n_mono: %d, first_good_ber_idx = %d, first_small_errcnt_idx = %d\n", 
                start_n, stop_n, eye_cnt, n_mono, first_good_ber_idx, first_small_errcnt_idx));
    }

    if (start_n >= eye_cnt) {
        outputs[0] = log10(bers[eye_cnt-1]);
        outputs[1] = -100.0;
        outputs[2] = -100.0;
        outputs[3] = -100.0;

		/* No need to print out the decimal portion of the BER */
		USR_PRINTF(("BER *worse* than 1e%d\n", (int)outputs[0]));
		USR_PRINTF(("Negative margin @ 1e-12, 1e-15 & 1e-18\n"));
    } else {
		/* JG moved outside of if statement below to be consistent with ml version. */
		for (loop_index=stop_n; loop_index<eye_cnt; loop_index++) {
			if (total_errs[loop_index] < 101) {
				bers[loop_index] = ber_conf_scale[total_errs[loop_index]] * bers[loop_index];
			} else {
				bers[loop_index] = ber_conf_scale[100] * bers[loop_index];
			}
			mono_flags[loop_index] = 0;    /* remove flags; or assess again */
		}
		low_confidence = 0;
		if ((stop_n-start_n-n_mono) < 2) {      /* <2 statistically valid extrapolation points */

			/* Add artificial point at 100% margin to enable interpolation */
			if(heye == 1) {
				margins[eye_cnt] = direction*32*1000.0/64.0;
			} else {
				margins[eye_cnt] = direction*_ladder_setting_to_mV(DIAG_MAX_SAMPLES, 0);
			}

			bers[eye_cnt] = 0.1;
			low_confidence = 1;
			stop_n = eye_cnt + 1;
		}

		/* Code below assumes statistically valid points available */
		delta_n = (stop_n-start_n-n_mono);

		/* Find all the correlations */
		for (loop_index=start_n; loop_index<stop_n; loop_index++) {
			lbers[loop_index] = (USR_DOUBLE)sqrt(-log10(bers[loop_index]));
		}

		if (verbose > 2) {
			USR_PRINTF(("\tstart=%d, stop=%d, delta_n: %d, low_confidence=%d\n", start_n, stop_n, 
			delta_n, low_confidence));
			for (loop_index=start_n; loop_index<stop_n; loop_index++) {
				USR_PRINTF(("@Margin[%f]: ", margins[loop_index]));
				USR_PRINTF(("total_errs=%u, ", total_errs[loop_index]));
				USR_PRINTF(("bers=1e%f, ", log10(bers[loop_index])));
				USR_PRINTF(("lbers=%f\n", lbers[loop_index]));
			}
			USR_PRINTF(("Samples included in extrapolation = "));
		}

		for (loop_index=start_n; loop_index<stop_n; loop_index++) {
			if (mono_flags[loop_index] == 0) {
				Exy += (margins[loop_index]*lbers[loop_index]/(USR_DOUBLE)delta_n);
				Eyy += (lbers[loop_index]*lbers[loop_index]/(USR_DOUBLE)delta_n);
				Exx += (margins[loop_index]*margins[loop_index]/(USR_DOUBLE)delta_n);
				Ey  += (lbers[loop_index]/(USR_DOUBLE)delta_n);
				Ex  += (margins[loop_index]/(USR_DOUBLE)delta_n);
				if (verbose > 2) {
					USR_PRINTF((",%d",loop_index));
				}
			}
		}

		/* Compute fit slope and offset */
		alpha = (Exy - Ey*Ex) / (Exx - Ex*Ex);
		beta = Ey - Ex*alpha;

		if (verbose > 2) {
		USR_PRINTF(("\nExy=%f, Eyy=%f, Exx=%f, Ey=%f, Ex=%f, alpha=%f, beta=%f\n", Exy,Eyy,Exx,Ey,Ex,alpha,beta));
		}

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

		outputs[0] = log10(proj_ber);
		outputs[1] = proj_margin_12;
		outputs[2] = proj_margin_15;
		outputs[3] = proj_margin_18;

		if (verbose > 2) {
			USR_PRINTF(("\t\tsq_err1=%f\n",sq_err1));
			USR_PRINTF(("\t\tsq_err2=%f\n",sq_err2));
			USR_PRINTF(("\t\tproj_ber=1e%f\n",proj_ber));
			USR_PRINTF(("\t\tproj_margin12=%f\n",proj_margin_12));
			USR_PRINTF(("\t\tproj_margin15=%f\n",proj_margin_15));
			USR_PRINTF(("\t\tproj_margin18=%f\n",proj_margin_18));
			USR_PRINTF(("\t\toutputs[0]=%f\n",outputs[0]));
			USR_PRINTF(("\t\toutputs[1]=%f\n",outputs[1]));
			USR_PRINTF(("\t\toutputs[2]=%f\n",outputs[2]));
		}

		/* Extrapolated results, low confidence */
		if (low_confidence == 1) {
			if(heye == 1) {
				USR_PRINTF(("BER(extrapolated) is *better* than 1e%0.2f\n", outputs[0]));
				if(outputs[0] < -12) USR_PRINTF(("Margin @ 1e-12    is *better* than %0.2f mUI\n", outputs[1]));
				if(outputs[0] < -15) USR_PRINTF(("Margin @ 1e-15    is *better* than %0.2f mUI\n", outputs[2]));
				if(outputs[0] < -18) USR_PRINTF(("Margin @ 1e-18    is *better* than %0.2f mUI\n\n\n", outputs[3]));
			} else {
				USR_PRINTF(("BER(extrapolated) is *better* than 1e%0.2f\n", outputs[0]));
				if(outputs[0] < -12) USR_PRINTF(("Margin @ 1e-12    is *better* than %0.2f mV\n", outputs[1]));
				if(outputs[0] < -15) USR_PRINTF(("Margin @ 1e-15    is *better* than %0.2f mV\n", outputs[2]));
				if(outputs[0] < -18) USR_PRINTF(("Margin @ 1e-18    is *better* than %0.2f mV\n\n\n", outputs[3]));
			}
		} else {           
			if(heye == 1) {
				USR_PRINTF(("BER(extrapolated) = 1e%0.2f\n", outputs[0]));
				if(outputs[0] < -12) USR_PRINTF(("Margin @ 1e-12    = %0.2f mUI\n", outputs[1]));
				if(outputs[0] < -15) USR_PRINTF(("Margin @ 1e-15    = %0.2f mUI\n", outputs[2]));
				if(outputs[0] < -18) USR_PRINTF(("Margin @ 1e-18    = %0.2f mUI\n\n\n", outputs[3]));
			} else {
				USR_PRINTF(("BER(extrapolated) = 1e%0.2f\n", outputs[0]));
				if(outputs[0] < -12) USR_PRINTF(("Margin @ 1e-12    = %0.2f mV\n", outputs[1]));
				if(outputs[0] < -15) USR_PRINTF(("Margin @ 1e-15    = %0.2f mV\n", outputs[2]));
				if(outputs[0] < -18) USR_PRINTF(("Margin @ 1e-18    = %0.2f mV\n\n\n", outputs[3]));
			}
		}

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

    }
#else
 USR_PRINTF(("This functions needs SERDES_API_FLOATING_POINT define to operate \n"));
 if(!total_errs || !total_time ) {
 	return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
 }
 rate = 0; ber_scan_mode = 0; max_offset = 0;     /* to avoid compile warnings */ 
#endif
	return(ERR_CODE_NONE);

}



/*****************************/
/*  Display Lane/Core State  */
/*****************************/

err_code_t falcon_tsc_display_lane_state_hdr(const phymod_access_t *pa) {
  USR_PRINTF(("LN (CDRxN  ,UC_CFG) "));
  USR_PRINTF(("SD LCK RXPPM "));
#ifndef FALCON
  USR_PRINTF(("CLK90 CLKP1 "));
#endif
  USR_PRINTF(("PF(M,L) "));
  USR_PRINTF(("VGA DCO "));
#ifndef FALCON
  USR_PRINTF(("P1mV "));
#endif
#ifndef FALCON
  USR_PRINTF((" DFE(1,2,3,4,5,dcd1,dcd2)   SLICER(ze,zo,pe,po,me,mo) "));
#else
  USR_PRINTF((" DFE(1,2,3,4,5,6)        "));
#endif
#ifdef MERLIN
  USR_PRINTF(("TXPPM TXEQ(n1,m,p1,p2) EYE(L,R,U,D) "));
#else
  USR_PRINTF(("TXPPM TXEQ(n1,m,p1,p2,p3) EYE(L,R,U,D)  "));
#endif
  USR_PRINTF(("LINK_TIME"));
  USR_PRINTF(("\n"));
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_display_lane_state_legend(const phymod_access_t *pa) {
  USR_PRINTF(("\n"));
  USR_PRINTF(("**********************************************************************************************\n")); 
  USR_PRINTF(("****                Legend of Entries in display_lane_state()                             ****\n")); 
  USR_PRINTF(("**********************************************************************************************\n"));
  USR_PRINTF(("LN       : lane index within IP core\n"));
  USR_PRINTF(("(CDRxN,UC_CFG) : CDR type x OSR ratio, micro lane configuration variable\n"));
  USR_PRINTF(("SD       : signal detect\n"));
  USR_PRINTF(("LOCK     : pmd_rx_lock\n"));
  USR_PRINTF(("RXPPM    : Frequency offset of local reference clock with respect to RX data in ppm\n"));
#ifndef FALCON
  USR_PRINTF(("CLK90    : Delay of zero crossing slicer, m1, wrt to data in PI codes\n"));
  USR_PRINTF(("CLKP1    : Delay of diagnostic/lms slicer, p1, wrt to data in PI codes\n"));
#endif
  USR_PRINTF(("PF(M,L)  : Peaking Filter Main (0..15) and Low Frequency (0..7) settings\n"));
#ifndef FALCON
  USR_PRINTF(("VGA      : Variable Gain Amplifier settings (0..42)\n"));
#else
  USR_PRINTF(("VGA      : Variable Gain Amplifier settings (0..39)\n"));
#endif
  USR_PRINTF(("DCO      : DC offset DAC control value\n"));
#ifndef FALCON
  USR_PRINTF(("P1mV     : Vertical threshold voltage of p1 slicer\n"));
  USR_PRINTF(("DFE taps : ISI correction taps in units of 2.35mV (for 1 & 2 even values are displayed, dcd = even-odd)\n"));   
#else
  USR_PRINTF(("DFE taps : ISI correction taps in units of 2.35mV\n"));   
#endif
#ifndef FALCON
  USR_PRINTF(("SLICER(ze,zo,pe,po,me,mo) : Slicer calibration control codes\n"));
#endif
  USR_PRINTF(("TXPPM            : Frequency offset of local reference clock with respect to TX data in ppm\n"));  
#ifdef MERLIN
  USR_PRINTF(("TXEQ(n1,m,p1,p2) : TX equalization FIR tap weights in units of 1Vpp/60 units\n"));
#else
  USR_PRINTF(("TXEQ(n1,m,p1,p2,p3) : TX equalization FIR tap weights in units of 1Vpp/160 units\n"));
#endif
  USR_PRINTF(("EYE(L,R,U,D)     : Eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV\n"));
  USR_PRINTF(("LINK_TIME        : Link time in milliseconds\n"));
  USR_PRINTF(("**********************************************************************************************\n")); 
  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_display_lane_state(const phymod_access_t *pa) {
  err_code_t err_code;
  err_code = _falcon_display_lane_state_no_newline(pa );
  USR_PRINTF(("\n"));
  return (err_code);
}

err_code_t falcon_display_core_state_hdr(const phymod_access_t *pa) {
  USR_PRINTF(("\n\n**************************************************************************************************************\n")); 
  USR_PRINTF(("****                                      SERDES DISPLAY CORE STATE                                       ****\n")); 
  USR_PRINTF(("**************************************************************************************************************\n"));
  USR_PRINTF((" RST_ST"));
#ifndef MERLIN_COUNTACH
  USR_PRINTF(("  PLL_PWDN"));
#endif
  USR_PRINTF(("  UC_ATV   COM_CLK   UCODE_VER  AFE_VER   TEMPERATURE_IDX   VCO_RATE  ANA_VCO_RANGE  PLL_DIV \n"));
  return (ERR_CODE_NONE);
}

err_code_t falcon_display_core_state_line(const phymod_access_t *pa) {
  err_code_t err_code;
  err_code = _falcon_display_core_state_no_newline(pa ); 
  USR_PRINTF(("\n"));
  return (err_code);
}

err_code_t falcon_display_core_state_legend(const phymod_access_t *pa) {
  USR_PRINTF(("\n"));
  USR_PRINTF(("**************************************************************************************************************\n")); 
  USR_PRINTF(("****                          Legend of Entries in display_core_state()                                   ****\n")); 
  USR_PRINTF(("**************************************************************************************************************\n")); 
  USR_PRINTF(("*  RST_ST           : Core DP Reset State (hex) [3'{reset_active, reset_occured, reset_held}]                *\n"));
#ifndef MERLIN_COUNTACH
  USR_PRINTF(("*  PLL_PWDN         : PLL Powerdown Control Bit (active high)                                                *\n"));
#endif
  USR_PRINTF(("*  UC_ATV           : UC Active bit                                                                          *\n"));
  USR_PRINTF(("*  COM_CLK          : COM Clock frequency in MHz                                                             *\n"));
  USR_PRINTF(("*  UCODE_VER        : Microcode Version [majorversion_minorversion]                                          *\n"));
  USR_PRINTF(("*  AFE_VER          : AFE Hardware Vesrion                                                                   *\n"));
  USR_PRINTF(("*  TEMPERATURE_IDX  : Temperature Index [index followed by temperature range]                                *\n"));
  USR_PRINTF(("*  VCO_RATE         : VCO Rate in GHz                                                                        *\n"));
  USR_PRINTF(("*  ANA_VCO_RANGE    : Analog VCO Range                                                                       *\n"));
  USR_PRINTF(("*  PLL_DIV          : PLL Divider value                                                                      *\n"));
  USR_PRINTF(("**************************************************************************************************************\n")); 
  return (ERR_CODE_NONE);
}

/**********************************************/
/*  Display Lane/Core Config and Debug Status */
/**********************************************/

err_code_t falcon_display_core_config(const phymod_access_t *pa) {

  err_code_t  err_code;
  struct falcon_uc_core_config_st core_cfg;
  uint16_t vco_rate_mhz;

  USR_MEMSET(&core_cfg, 0, sizeof(struct falcon_uc_core_config_st));

  USR_PRINTF(("\n\n***********************************\n")); 
  USR_PRINTF(("**** SERDES CORE CONFIGURATION ****\n")); 
  USR_PRINTF(("***********************************\n\n")); 

  err_code = falcon_tsc_get_uc_core_config(pa, &core_cfg);
  if (err_code) {
    return (err_code);
  }

  #ifdef FALCON
    vco_rate_mhz = (((((uint16_t) core_cfg.field.vco_rate) * 125)/2) + 14000);
  #else
    vco_rate_mhz = ((((uint16_t) core_cfg.field.vco_rate) * 250) + 5500);
  #endif
  USR_PRINTF(("uC Config VCO Rate   = %d (%d.%dGHz)\n",core_cfg.field.vco_rate,vco_rate_mhz/1000,vco_rate_mhz % 1000));
  USR_PRINTF(("Core Config from PCS = %d\n\n", core_cfg.field.core_cfg_from_pcs));

  #ifndef MERLIN  
  ESTM(USR_PRINTF(("Lane Addr 0          = %d\n", rdc_lane_addr_0())));
  ESTM(USR_PRINTF(("Lane Addr 1          = %d\n", rdc_lane_addr_1())));
  ESTM(USR_PRINTF(("Lane Addr 2          = %d\n", rdc_lane_addr_2())));
  ESTM(USR_PRINTF(("Lane Addr 3          = %d\n", rdc_lane_addr_3())));
  #endif

  #ifdef EAGLE
  ESTM(USR_PRINTF(("TX Lane Map 0        = %d\n", rdc_tx_lane_map_0())));
  ESTM(USR_PRINTF(("TX Lane Map 1        = %d\n", rdc_tx_lane_map_1())));
  ESTM(USR_PRINTF(("TX Lane Map 2        = %d\n", rdc_tx_lane_map_2())));
  ESTM(USR_PRINTF(("TX Lane Map 3        = %d\n\n", rdc_tx_lane_map_3())));
  #endif

  #ifdef FALCON_TSC
  ESTM(USR_PRINTF(("TX Lane Map 0        = %d\n", rdc_tx_lane_map_0())));
  ESTM(USR_PRINTF(("TX Lane Map 1        = %d\n", rdc_tx_lane_map_1())));
  ESTM(USR_PRINTF(("TX Lane Map 2        = %d\n", rdc_tx_lane_map_2())));
  ESTM(USR_PRINTF(("TX Lane Map 3        = %d\n\n", rdc_tx_lane_map_3())));
  #endif  

  return(ERR_CODE_NONE); 
}


err_code_t falcon_display_lane_config(const phymod_access_t *pa) {

  err_code_t  err_code;
  struct falcon_uc_lane_config_st lane_cfg;

  USR_MEMSET(&lane_cfg, 0, sizeof(struct falcon_uc_lane_config_st));

  USR_PRINTF(("\n\n*************************************\n")); 
  USR_PRINTF(("**** SERDES LANE %d CONFIGURATION ****\n",falcon_get_lane(pa ))); 
  USR_PRINTF(("*************************************\n\n")); 

  err_code = falcon_tsc_get_uc_lane_cfg(pa, &lane_cfg);
  if (err_code) {
    return (err_code);
  }

  USR_PRINTF(("Auto-Neg Enabled      = %d\n", lane_cfg.field.an_enabled));
  USR_PRINTF(("DFE on                = %d\n", lane_cfg.field.dfe_on));
#ifdef FALCON  
  USR_PRINTF(("DFE low power mode    = %d\n", lane_cfg.field.dfe_lp_mode));
#endif
  USR_PRINTF(("Brdfe_on              = %d\n", lane_cfg.field.force_brdfe_on));
  USR_PRINTF(("Media Type            = %d\n", lane_cfg.field.media_type));
  USR_PRINTF(("Unreliable LOS        = %d\n", lane_cfg.field.unreliable_los));
  USR_PRINTF(("Scrambling Disable    = %d\n", lane_cfg.field.scrambling_dis));
#ifndef FALCON  
  USR_PRINTF(("CL72 Emulation Enable = %d\n", lane_cfg.field.cl72_emulation_en));
#endif
  USR_PRINTF(("Lane Config from PCS  = %d\n\n", lane_cfg.field.lane_cfg_from_pcs));

#ifdef FALCON  
  ESTM(USR_PRINTF(("CL93/72 Training Enable  = %d\n", rd_cl93n72_ieee_training_enable())));
#else
  ESTM(USR_PRINTF(("CL72 Training Enable  = %d\n", rd_cl72_ieee_training_enable())));
#endif
  ESTM(USR_PRINTF(("EEE Mode Enable       = %d\n", rd_eee_mode_en())));
  ESTM(USR_PRINTF(("OSR Mode Force        = %d\n", rd_osr_mode_frc())));
  ESTM(USR_PRINTF(("OSR Mode Force Val    = %d\n", rd_osr_mode_frc_val())));
  ESTM(USR_PRINTF(("TX Polarity Invert    = %d\n", rd_tx_pmd_dp_invert())));
  ESTM(USR_PRINTF(("RX Polarity Invert    = %d\n\n", rd_rx_pmd_dp_invert())));

  ESTM(USR_PRINTF(("TXFIR Post2           = %d\n", rd_txfir_post2())));
#ifndef MERLIN
  ESTM(USR_PRINTF(("TXFIR Post3           = %d\n", rd_txfir_post3())));
#endif
#ifndef FALCON  
  ESTM(USR_PRINTF(("TXFIR Override Enable = %d\n", rd_txfir_override_en())));
  ESTM(USR_PRINTF(("TXFIR Main Override   = %d\n", rd_txfir_main_override())));
  ESTM(USR_PRINTF(("TXFIR Pre Override    = %d\n", rd_txfir_pre_override())));
  ESTM(USR_PRINTF(("TXFIR Post Override   = %d\n", rd_txfir_post_override())));
#else
  ESTM(USR_PRINTF(("TXFIR Main            = %d\n", rd_cl93n72_txfir_main())));
  ESTM(USR_PRINTF(("TXFIR Pre             = %d\n", rd_cl93n72_txfir_pre())));
  ESTM(USR_PRINTF(("TXFIR Post            = %d\n", rd_cl93n72_txfir_post())));
#endif

  return(ERR_CODE_NONE); 
}


err_code_t falcon_tsc_display_core_state(const phymod_access_t *pa) {

  uint8_t     temp_idx;
  int8_t      temp_val;

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

  return(ERR_CODE_NONE); 
}



err_code_t falcon_tsc_display_lane_debug_status(const phymod_access_t *pa) {

  err_code_t  err_code;

  /* startup */
  struct falcon_usr_ctrl_disable_functions_st ds; 
  struct falcon_usr_ctrl_disable_dfe_functions_st dsd;

  /* steady state */
  struct falcon_usr_ctrl_disable_functions_st dss;
  struct falcon_usr_ctrl_disable_dfe_functions_st dssd;

  USR_MEMSET(&ds, 0, sizeof(struct falcon_usr_ctrl_disable_functions_st));
  USR_MEMSET(&dsd, 0, sizeof(struct falcon_usr_ctrl_disable_dfe_functions_st));
  USR_MEMSET(&dss, 0, sizeof(struct falcon_usr_ctrl_disable_functions_st));
  USR_MEMSET(&dssd, 0, sizeof(struct falcon_usr_ctrl_disable_dfe_functions_st));

  USR_PRINTF(("\n\n************************************\n")); 
  USR_PRINTF(("**** SERDES LANE %d DEBUG STATUS ****\n",falcon_get_lane(pa ))); 
  USR_PRINTF(("************************************\n\n")); 

  ESTM(USR_PRINTF(("Restart Count       = %d\n", rdv_usr_sts_restart_counter())));
  ESTM(USR_PRINTF(("Reset Count         = %d\n", rdv_usr_sts_reset_counter())));
  ESTM(USR_PRINTF(("PMD Lock Count      = %d\n\n", rdv_usr_sts_pmd_lock_counter())));
 

  err_code = falcon_get_usr_ctrl_disable_startup(pa, &ds);
  if (err_code) {
    return (err_code);
  }

#ifdef FALCON
  USR_PRINTF(("Disable Startup PF Adaptation           = %d\n", ds.field.pf_adaptation));
  USR_PRINTF(("Disable Startup PF2 Adaptation          = %d\n", ds.field.pf2_adaptation));
  USR_PRINTF(("Disable Startup DC Adaptation           = %d\n", ds.field.dc_adaptation));
  USR_PRINTF(("Disable Startup VGA Adaptation          = %d\n", ds.field.vga_adaptation));
  USR_PRINTF(("Disable Startup Slicer vOffset Tuning   = %d\n", ds.field.slicer_voffset_tuning));
  USR_PRINTF(("Disable Startup Slicer hOffset Tuning   = %d\n", ds.field.slicer_hoffset_tuning));
  USR_PRINTF(("Disable Startup Phase offset Adaptation = %d\n", ds.field.phase_offset_adaptation));
  USR_PRINTF(("Disable Startup Eye Adaptaion           = %d\n", ds.field.eye_adaptation));
  USR_PRINTF(("Disable Startup All Adaptaion           = %d\n\n", ds.field.all_adaptation));
#else
  USR_PRINTF(("Disable Startup PF Adaptation           = %d\n", ds.field.pf_adaptation));
  USR_PRINTF(("Disable Startup DC Adaptation           = %d\n", ds.field.dc_adaptation));
  USR_PRINTF(("Disable Startup Slicer Offset Tuning    = %d\n", ds.field.slicer_offset_tuning));
  USR_PRINTF(("Disable Startup Clk90 offset Adaptation = %d\n", ds.field.clk90_offset_adaptation));
  USR_PRINTF(("Disable Startup P1 level Tuning         = %d\n", ds.field.p1_level_tuning));
  USR_PRINTF(("Disable Startup Eye Adaptaion           = %d\n", ds.field.eye_adaptation));
  USR_PRINTF(("Disable Startup All Adaptaion           = %d\n\n", ds.field.all_adaptation));
#endif

  err_code = falcon_get_usr_ctrl_disable_startup_dfe(pa, &dsd);
  if (err_code) {
    return (err_code);
  }

#ifdef FALCON
  USR_PRINTF(("Disable Startup DFE Tap1 Adaptation    = %d\n",dsd.field.dfe_tap1_adaptation));
  USR_PRINTF(("Disable Startup DFE FX Taps Adaptation = %d\n",dsd.field.dfe_fx_taps_adaptation));
  USR_PRINTF(("Disable Startup DFE FL Taps Adaptation = %d\n",dsd.field.dfe_fl_taps_adaptation));
  USR_PRINTF(("Disable Startup DFE Tap DCD            = %d\n",dsd.field.dfe_dcd_adaptation));       
#else
  USR_PRINTF(("Disable Startup DFE Tap1 Adaptation = %d\n",dsd.field.dfe_tap1_adaptation));
  USR_PRINTF(("Disable Startup DFE Tap2 Adaptation = %d\n",dsd.field.dfe_tap2_adaptation));
  USR_PRINTF(("Disable Startup DFE Tap3 Adaptation = %d\n",dsd.field.dfe_tap3_adaptation));
  USR_PRINTF(("Disable Startup DFE Tap4 Adaptation = %d\n",dsd.field.dfe_tap4_adaptation));
  USR_PRINTF(("Disable Startup DFE Tap5 Adaptation = %d\n",dsd.field.dfe_tap5_adaptation));
  USR_PRINTF(("Disable Startup DFE Tap1 DCD        = %d\n",dsd.field.dfe_tap1_dcd));       
  USR_PRINTF(("Disable Startup DFE Tap2 DCD        = %d\n\n",dsd.field.dfe_tap2_dcd));       
#endif

  err_code = falcon_get_usr_ctrl_disable_steady_state(pa, &dss);
  if (err_code) {
    return (err_code);
  }

#ifdef FALCON
  USR_PRINTF(("Disable Steady State PF Adaptation           = %d\n", dss.field.pf_adaptation));
  USR_PRINTF(("Disable Steady State PF2 Adaptation          = %d\n", dss.field.pf2_adaptation));
  USR_PRINTF(("Disable Steady State DC Adaptation           = %d\n", dss.field.dc_adaptation));
  USR_PRINTF(("Disable Steady State VGA Adaptation          = %d\n", dss.field.vga_adaptation));
  USR_PRINTF(("Disable Steady State Slicer vOffset Tuning   = %d\n", dss.field.slicer_voffset_tuning));
  USR_PRINTF(("Disable Steady State Slicer hOffset Tuning   = %d\n", dss.field.slicer_hoffset_tuning));
  USR_PRINTF(("Disable Steady State Phase offset Adaptation = %d\n", dss.field.phase_offset_adaptation));
  USR_PRINTF(("Disable Steady State Eye Adaptaion           = %d\n", dss.field.eye_adaptation));
  USR_PRINTF(("Disable Steady State All Adaptaion           = %d\n\n", dss.field.all_adaptation));
#else
  USR_PRINTF(("Disable Steady State PF Adaptation           = %d\n", dss.field.pf_adaptation));
  USR_PRINTF(("Disable Steady State DC Adaptation           = %d\n", dss.field.dc_adaptation));
  USR_PRINTF(("Disable Steady State Slicer Offset Tuning    = %d\n", dss.field.slicer_offset_tuning));
  USR_PRINTF(("Disable Steady State Clk90 offset Adaptation = %d\n", dss.field.clk90_offset_adaptation));
  USR_PRINTF(("Disable Steady State P1 level Tuning         = %d\n", dss.field.p1_level_tuning));
  USR_PRINTF(("Disable Steady State Eye Adaptaion           = %d\n", dss.field.eye_adaptation));
  USR_PRINTF(("Disable Steady State All Adaptaion           = %d\n\n", dss.field.all_adaptation));
#endif

  err_code = falcon_get_usr_ctrl_disable_steady_state_dfe(pa, &dssd);
  if (err_code) {
    return (err_code);
  }

#ifdef FALCON
  USR_PRINTF(("Disable Steady State DFE Tap1 Adaptation    = %d\n",dssd.field.dfe_tap1_adaptation));
  USR_PRINTF(("Disable Steady State DFE FX Taps Adaptation = %d\n",dssd.field.dfe_fx_taps_adaptation));
  USR_PRINTF(("Disable Steady State DFE FL Taps Adaptation = %d\n",dssd.field.dfe_fl_taps_adaptation));
  USR_PRINTF(("Disable Steady State DFE Tap DCD            = %d\n",dssd.field.dfe_dcd_adaptation));       
#else
  USR_PRINTF(("Disable Steady State DFE Tap1 Adaptation = %d\n",dssd.field.dfe_tap1_adaptation));
  USR_PRINTF(("Disable Steady State DFE Tap2 Adaptation = %d\n",dssd.field.dfe_tap2_adaptation));
  USR_PRINTF(("Disable Steady State DFE Tap3 Adaptation = %d\n",dssd.field.dfe_tap3_adaptation));
  USR_PRINTF(("Disable Steady State DFE Tap4 Adaptation = %d\n",dssd.field.dfe_tap4_adaptation));
  USR_PRINTF(("Disable Steady State DFE Tap5 Adaptation = %d\n",dssd.field.dfe_tap5_adaptation));
  USR_PRINTF(("Disable Steady State DFE Tap1 DCD        = %d\n",dssd.field.dfe_tap1_dcd));       
  USR_PRINTF(("Disable Steady State DFE Tap2 DCD        = %d\n\n",dssd.field.dfe_tap2_dcd));  
#endif

  ESTM(USR_PRINTF(("Retune after Reset    = %d\n", rdv_usr_ctrl_retune_after_restart())));               
  ESTM(USR_PRINTF(("Clk90 offset Adjust   = %d\n", rdv_usr_ctrl_clk90_offset_adjust())));                
  ESTM(USR_PRINTF(("Clk90 offset Override = %d\n", rdv_usr_ctrl_clk90_offset_override())));              
  ESTM(USR_PRINTF(("Lane Event Log Level  = %d\n", rdv_usr_ctrl_lane_event_log_level())));

  return(ERR_CODE_NONE); 
}



/*************************/
/*  Stop/Resume uC Lane  */
/*************************/

err_code_t falcon_stop_uc_lane(const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    return(falcon_pmd_uc_control(pa, CMD_UC_CTRL_STOP_GRACEFULLY,2000));
  }
  else {
    return(falcon_pmd_uc_control(pa, CMD_UC_CTRL_RESUME,2000));
  }
}


err_code_t falcon_stop_uc_lane_status(const phymod_access_t *pa, uint8_t *uc_lane_stopped) {
  
  if(!uc_lane_stopped) {
	  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*uc_lane_stopped = rdv_usr_sts_micro_stopped());

  return (ERR_CODE_NONE);
}


/*******************************/
/*  Stop/Resume RX Adaptation  */
/*******************************/

err_code_t falcon_tsc_stop_rx_adaptation(const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    return(falcon_pmd_uc_control(pa, CMD_UC_CTRL_STOP_GRACEFULLY,2000));
  }
  else {
    return(falcon_pmd_uc_control(pa, CMD_UC_CTRL_RESUME,2000));
  }
}



/**********************/
/*  uCode CRC Verify  */
/**********************/

#ifdef SERDES_DEFAULT_MICRO
err_code_t falcon_ucode_crc_verify(const phymod_access_t *pa, uint16_t ucode_len,uint16_t expected_crc_value) {
    uint16_t calc_crc;

    EFUN(falcon_pmd_uc_cmd_with_data(pa, CMD_CALC_CRC,0,ucode_len,2000));

    ESTM(calc_crc = rd_uc_dsc_data());
    if(calc_crc != expected_crc_value) {
        USR_PRINTF(("UC CRC did not match expected=%04x : calculated=%04x\n",expected_crc_value, calc_crc));
        return(_error(ERR_CODE_UC_CRC_NOT_MATCH));
    }

    return(ERR_CODE_NONE);
}
#endif


/******************************************************/
/*  Commands through Serdes FW DSC Command Interface  */
/******************************************************/

err_code_t falcon_tsc_pmd_uc_cmd(const phymod_access_t *pa, enum _falcon_comn_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint32_t timeout_ms) {

  err_code_t err_code;
  uint16_t cmddata;
  uint8_t uc_dsc_error_found;

  err_code = falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1(pa, timeout_ms); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate serdes ready for command */
  if (err_code) {
    ESTM(USR_PRINTF(("ERROR : DSC ready for command timed out (before cmd) cmd = %d, supp_info = x%02x err=%d !\n", cmd, supp_info,err_code)));      
    return (err_code);
  }
  /*wr_uc_dsc_supp_info(supp_info);*/                                   /* supplement info field */
  /*wr_uc_dsc_error_found(0x0);    */                                   /* Clear error found field */
  /*wr_uc_dsc_gp_uc_req(cmd);      */                                   /* Set command code */
  /*wr_uc_dsc_ready_for_cmd(0x0);  */                                   /* Issue command, by clearing "ready for command" field */
  cmddata = (((uint16_t)supp_info)<<8) | (uint16_t)cmd;     /* combine writes to single write instead of 4 RMW */

  EFUN(falcon_pmd_wr_reg(pa, DSC_A_DSC_UC_CTRL, cmddata));         /* This address is same for Eagle, and all merlin */

  EFUN(falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1(pa, timeout_ms)); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate serdes ready for command */
  ESTM(uc_dsc_error_found = rd_uc_dsc_error_found());
  if(uc_dsc_error_found) {
    ESTM(USR_PRINTF(("ERROR : DSC ready for command return error ( after cmd) cmd = %d, supp_info = x%02x !\n", cmd, rd_uc_dsc_supp_info())));      
      return(_error(ERR_CODE_UC_CMD_RETURN_ERROR));
  }
  return(ERR_CODE_NONE);
}

err_code_t falcon_pmd_uc_cmd_with_data(const phymod_access_t *pa, enum _falcon_comn_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint16_t data, uint32_t timeout_ms) {
  uint16_t cmddata;
  err_code_t err_code;
  uint8_t uc_dsc_error_found;

  err_code = falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1(pa, timeout_ms); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate serdes ready for command */
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

  EFUN(falcon_pmd_wr_reg(pa, DSC_A_DSC_UC_CTRL, cmddata));         /* This address is same for Eagle, and all merlin */

  EFUN(falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1(pa, timeout_ms)); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate serdes ready for command */
  ESTM(uc_dsc_error_found = rd_uc_dsc_error_found());
  if(uc_dsc_error_found) {
    ESTM(USR_PRINTF(("ERROR : DSC ready for command return error ( after cmd) cmd = %d, supp_info = x%02x !\n", cmd, rd_uc_dsc_supp_info())));      
    return(_error(ERR_CODE_UC_CMD_RETURN_ERROR));
}

  return(ERR_CODE_NONE);
}

err_code_t falcon_pmd_uc_control(const phymod_access_t *pa, enum _falcon_comn_pmd_uc_ctrl_cmd_enum control, uint32_t timeout_ms) {
  return(falcon_tsc_pmd_uc_cmd(pa, CMD_UC_CTRL, (uint8_t) control, timeout_ms));
}

err_code_t falcon_pmd_uc_diag_cmd(const phymod_access_t *pa, enum _falcon_comn_pmd_uc_diag_cmd_enum control, uint32_t timeout_ms) {
  return(falcon_tsc_pmd_uc_cmd(pa, CMD_DIAG_EN, (uint8_t) control, timeout_ms));
}



/************************************************************/
/*      Serdes IP RAM access - Lane RAM Variables           */
/*----------------------------------------------------------*/
/*   - through Micro Register Interface for PMD IPs         */
/*   - through Serdes FW DSC Command Interface for Gallardo */
/************************************************************/

/* Micro RAM Lane Byte Read */
uint8_t falcon_rdbl_uc_var(const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
	uint8_t rddata;



	if(!err_code_p) {
		return(0);
	}

#ifndef SERDES_DEFAULT_MICRO
	{
    falcon_uc_lane_info_st uc_lane_info;
    USR_MEMSET(&uc_lane_info, 0, sizeof(falcon_uc_lane_info_st));
    EPFUN(falcon_get_uc_ln_info(&uc_lane_info));
	if (uc_lane_info.is_direct_ram_access_avail) {
		EPFUN(falcon_rdb_ram(&rddata, uc_lane_info.lane_var_base_address+addr, 1));    /* Direct RAM access */
	}
	else {
		if (addr >= uc_lane_info.lane_ram_size || addr > 255) {                       /* uC command only support 8-bit addr */                 
			*err_code_p = ERR_CODE_INVALID_RAM_ADDR;                                  /* Error Code indicating Out of Range access */
			return (0);
		}

		EPFUN(falcon_tsc_pmd_uc_cmd(pa, CMD_READ_UC_LANE_BYTE, (uint8_t)addr, 100));          /* Use Serdes FW DSC Command Interface for reading RAM */

		EPSTM(rddata = (uint8_t)rd_uc_dsc_data());                                    /* Read_Value read from uc_dsc_data field */
	}
    }
#else
    EPSTM(rddata = falcon_rdb_uc_ram(pa, err_code_p, (LANE_VAR_RAM_BASE+addr+(falcon_get_lane(pa )*LANE_VAR_RAM_SIZE)))); /* Use Micro register interface for reading RAM */
#endif

	return (rddata);
} 

/* Micro RAM Lane Byte Signed Read */
int8_t falcon_rdbls_uc_var(const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  return ((int8_t) falcon_rdbl_uc_var(pa, err_code_p, addr));
} 

/* Micro RAM Lane Word Read */
uint16_t falcon_rdwl_uc_var(const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  uint16_t rddata;

  if(!err_code_p) {
	  return(0);
  }

  if (addr%2 != 0) {                                                                /* Validate even address */
	  *err_code_p = ERR_CODE_INVALID_RAM_ADDR;
	  return (0);
  }

#ifndef SERDES_DEFAULT_MICRO
  {
  falcon_uc_lane_info_st uc_lane_info;
  USR_MEMSET(&uc_lane_info, 0, sizeof(falcon_uc_lane_info_st));
  EFUN(falcon_get_uc_ln_info(&uc_lane_info));
  if (uc_lane_info.is_direct_ram_access_avail) {
	  EFUN(falcon_rdw_ram(&rddata, uc_lane_info.lane_var_base_address+addr, 1));                    /* Direct RAM access */
  }
  else {
	  if (addr >= uc_lane_info.lane_ram_size || addr > 255) {                       /* uC command only support 8-bit addr */                 
		  *err_code_p = ERR_CODE_INVALID_RAM_ADDR;                                  /* Error Code indicating Out of Range access */
		  return (0);
	  }

	  EPFUN(falcon_tsc_pmd_uc_cmd(pa, CMD_READ_UC_LANE_WORD, (uint8_t)addr, 100));                   /* Use Serdes FW DSC Command Interface for reading RAM */

    EPSTM(rddata = rd_uc_dsc_data());                                                    /* Read_Value read from uc_dsc_data field */
  }
  }
#else
  EPSTM(rddata = falcon_rdw_uc_ram(pa, err_code_p, (LANE_VAR_RAM_BASE+addr+(falcon_get_lane(pa )*LANE_VAR_RAM_SIZE)))); /* Use Micro register interface for reading RAM */
#endif

  return (rddata);
}


/* Micro RAM Lane Word Signed Read */
int16_t falcon_rdwls_uc_var(const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  return ((int16_t) falcon_rdwl_uc_var(pa, err_code_p, addr));
}
  
/* Micro RAM Lane Byte Write */
err_code_t falcon_wrbl_uc_var(const phymod_access_t *pa, uint16_t addr, uint8_t wr_val) {


#ifndef SERDES_DEFAULT_MICRO
    {
    falcon_uc_lane_info_st uc_lane_info;
    USR_MEMSET(&uc_lane_info, 0, sizeof(falcon_uc_lane_info_st));

    EFUN(falcon_get_uc_ln_info(&uc_lane_info));
	if (uc_lane_info.is_direct_ram_access_avail) {
		EFUN(falcon_wrb_ram(uc_lane_info.lane_var_base_address+addr, 1, &wr_val));
		return (ERR_CODE_NONE);
	}
	else {
		if (addr >= uc_lane_info.lane_ram_size || addr > 255) {                      /* uC command only support 8-bit addr */                             
		  return (_error(ERR_CODE_INVALID_RAM_ADDR));
		}
		return (falcon_pmd_uc_cmd_with_data(pa, CMD_WRITE_UC_LANE_BYTE, (uint8_t)addr, (uint16_t)wr_val, 10)); /* Use Serdes FW DSC Command Interface for writing RAM */
	}
    }
#else
    return (falcon_wrb_uc_ram(pa, (LANE_VAR_RAM_BASE+addr+(falcon_get_lane(pa )*LANE_VAR_RAM_SIZE)), wr_val));    /* Use Micro register interface for writing RAM */
#endif
}

/* Micro RAM Lane Byte Signed Write */
err_code_t falcon_wrbls_uc_var(const phymod_access_t *pa, uint16_t addr, int8_t wr_val) {
  return (falcon_wrbl_uc_var(pa, addr, wr_val));
}

/* Micro RAM Lane Word Write */
err_code_t falcon_wrwl_uc_var(const phymod_access_t *pa, uint16_t addr, uint16_t wr_val) {


	if (addr%2 != 0) {                                                                /* Validate even address */
		return (_error(ERR_CODE_INVALID_RAM_ADDR));
	}
#ifndef SERDES_DEFAULT_MICRO
    {
    falcon_uc_lane_info_st uc_lane_info;
    USR_MEMSET(&uc_lane_info, 0, sizeof(falcon_uc_lane_info_st));

    EFUN(falcon_get_uc_ln_info(&uc_lane_info));
	if (uc_lane_info.is_direct_ram_access_avail) {
		EFUN(falcon_wrw_ram(uc_lane_info.lane_var_base_address+addr, 1, &wr_val));
		return (ERR_CODE_NONE);
	}
	else {
		if (addr >= uc_lane_info.lane_ram_size || addr > 255 ) {                   /* uC command only support 8-bit addr */                  
		  return (_error(ERR_CODE_INVALID_RAM_ADDR));
		}
		return (falcon_pmd_uc_cmd_with_data(pa, CMD_WRITE_UC_LANE_WORD, (uint8_t)addr, (uint16_t)wr_val, 10)); /* Use Serdes FW DSC Command Interface for writing RAM */
	}
    }
#else
    return (falcon_wrw_uc_ram(pa, (LANE_VAR_RAM_BASE+addr+(falcon_get_lane(pa )*LANE_VAR_RAM_SIZE)), wr_val));    /* Use Micro register interface for writing RAM */
#endif
}

/* Micro RAM Lane Word Signed Write */
err_code_t falcon_wrwls_uc_var(const phymod_access_t *pa, uint16_t addr, int16_t wr_val) {
  return (falcon_wrwl_uc_var(pa, addr,wr_val));
}


/************************************************************/
/*      Serdes IP RAM access - Core RAM Variables           */
/*----------------------------------------------------------*/
/*   - through Micro Register Interface for PMD IPs         */
/*   - through Serdes FW DSC Command Interface for Gallardo */
/************************************************************/

/* Micro RAM Core Byte Read */
uint8_t falcon_rdbc_uc_var(const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {

  uint8_t rddata;

  if(!err_code_p) {
	  return(0);
  }

#ifndef SERDES_DEFAULT_MICRO
  {
    falcon_uc_lane_info_st uc_lane_info;
    USR_MEMSET(&uc_lane_info, 0, sizeof(falcon_uc_lane_info_st));

  EPFUN(falcon_get_uc_ln_info(&uc_lane_info));
  if (uc_lane_info.is_direct_ram_access_avail) {
	  EPFUN(falcon_rdb_ram(&rddata, uc_lane_info.core_var_base_address+(uint16_t)addr, 1));                    /* Direct RAM access */
  }
  else {
	  if (addr >= uc_lane_info.lane_ram_size) {   
		  *err_code_p = ERR_CODE_INVALID_RAM_ADDR;                                  /* Error Code indicating Out of Range access */
		  return (0);
	  }

	  EPFUN(falcon_tsc_pmd_uc_cmd(pa, CMD_READ_UC_CORE_BYTE, addr, 10));                   /* Use Serdes FW DSC Command Interface for reading RAM */
	  EPSTM(rddata = (uint8_t)rd_uc_dsc_data());                                            /* Read_Value read from uc_dsc_data field */
  }
  }
#else
  EPSTM(rddata = falcon_rdb_uc_ram(pa, err_code_p, (CORE_VAR_RAM_BASE+addr)));                      /* Use Micro register interface for reading RAM */
#endif

  return (rddata);
} 

/* Micro RAM Core Byte Signed Read */
int8_t falcon_rdbcs_uc_var(const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {
  return ((int8_t) falcon_rdbc_uc_var(pa, err_code_p, addr));
}

/* Micro RAM Core Word Read */
uint16_t falcon_rdwc_uc_var(const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {

  uint16_t rddata;

  if(!err_code_p) {
	  return(0);
  }
  if (addr%2 != 0) {                                                                /* Validate even address */
	  *err_code_p = ERR_CODE_INVALID_RAM_ADDR;
	  return (0);
  }

#ifndef SERDES_DEFAULT_MICRO
  {
    falcon_uc_lane_info_st uc_lane_info;
    USR_MEMSET(&uc_lane_info, 0, sizeof(falcon_uc_lane_info_st));

  EFUN(falcon_get_uc_ln_info(&uc_lane_info));
  if (uc_lane_info.is_direct_ram_access_avail) {
	  EFUN(falcon_rdw_ram(&rddata, uc_lane_info.core_var_base_address+(uint16_t)addr, 1));                    /* Direct RAM access */
  }
  else {
	  if (addr >= uc_lane_info.lane_ram_size) {         
		  *err_code_p = ERR_CODE_INVALID_RAM_ADDR;                                  /* Error Code indicating Out of Range access */
		  return (0);
	  }

	  EPFUN(falcon_tsc_pmd_uc_cmd(pa, CMD_READ_UC_CORE_WORD, addr, 10));                   /* Use Serdes FW DSC Command Interface for reading RAM */

    EPSTM(rddata = rd_uc_dsc_data());                                                    /* Read_Value read from uc_dsc_data field */
  }
  }
#else
  EPSTM(rddata = falcon_rdw_uc_ram(pa, err_code_p, (CORE_VAR_RAM_BASE+addr)));                  /* Use Micro register interface for reading RAM */
#endif

  return (rddata);
}

/* Micro RAM Core Word Signed Read */
int16_t falcon_rdwcs_uc_var(const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {
  return ((int16_t) falcon_rdwc_uc_var(pa, err_code_p, addr));
}

/* Micro RAM Core Byte Write  */
err_code_t falcon_wrbc_uc_var(const phymod_access_t *pa, uint8_t addr, uint8_t wr_val) {


#ifndef SERDES_DEFAULT_MICRO
    {	
    falcon_uc_lane_info_st uc_lane_info;
    USR_MEMSET(&uc_lane_info, 0, sizeof(falcon_uc_lane_info_st));
    EFUN(falcon_get_uc_ln_info(&uc_lane_info));
	if (uc_lane_info.is_direct_ram_access_avail) {
		EFUN(falcon_wrb_ram(uc_lane_info.core_var_base_address+(uint16_t)addr, 1, &wr_val));
		return (ERR_CODE_NONE);
	}
	else {
		if (addr >= uc_lane_info.lane_ram_size) {                      
		  return (_error(ERR_CODE_INVALID_RAM_ADDR));
		}
		return (falcon_pmd_uc_cmd_with_data(pa, CMD_WRITE_UC_CORE_BYTE, addr, (uint16_t)wr_val, 10)); /* Use Serdes FW DSC Command Interface for writing RAM */
	}
    }
#else
    return (falcon_wrb_uc_ram(pa, (CORE_VAR_RAM_BASE+addr), wr_val));                                /* Use Micro register interface for writing RAM */
#endif
} 


/* Micro RAM Core Byte Signed Write */
err_code_t falcon_wrbcs_uc_var(const phymod_access_t *pa, uint8_t addr, int8_t wr_val) {
  return (falcon_wrbc_uc_var(pa, addr, wr_val));
}

/* Micro RAM Core Word Write  */
err_code_t falcon_wrwc_uc_var(const phymod_access_t *pa, uint8_t addr, uint16_t wr_val) {


	if (addr%2 != 0) {                                                                /* Validate even address */
		return (_error(ERR_CODE_INVALID_RAM_ADDR));
	}
#ifndef SERDES_DEFAULT_MICRO
    {
    falcon_uc_lane_info_st uc_lane_info;
    USR_MEMSET(&uc_lane_info, 0, sizeof(falcon_uc_lane_info_st));

    EFUN(falcon_get_uc_ln_info(&uc_lane_info));
	if (uc_lane_info.is_direct_ram_access_avail) {
		EFUN(falcon_wrw_ram(uc_lane_info.core_var_base_address+addr, 1, &wr_val));
		return (ERR_CODE_NONE);
	}
	else {
		if (addr >= uc_lane_info.lane_ram_size) {                                     
		  return (_error(ERR_CODE_INVALID_RAM_ADDR));
		}
		return (falcon_pmd_uc_cmd_with_data(pa, CMD_WRITE_UC_CORE_WORD, addr, wr_val, 10)); /* Use Serdes FW DSC Command Interface for writing RAM */
	}
    }
#else
    return (falcon_wrw_uc_ram(pa, (CORE_VAR_RAM_BASE+addr), wr_val));                                 /* Use Micro register interface for writing RAM */
#endif
}

/* Micro RAM Core Word Signed Write */
err_code_t falcon_wrwcs_uc_var(const phymod_access_t *pa, uint8_t addr, int16_t wr_val) {
  return(falcon_wrwc_uc_var(pa, addr,wr_val));
}



/*******************************************************************/
/*  APIs to Write Core/Lane Config and User variables into uC RAM  */
/*******************************************************************/

err_code_t falcon_tsc_set_uc_core_config(const phymod_access_t *pa, struct falcon_uc_core_config_st struct_val) {
  _update_uc_core_config_word(&struct_val);
  return(wrcv_config_word(struct_val.word));
}

err_code_t falcon_set_usr_ctrl_core_event_log_level(const phymod_access_t *pa, uint8_t core_event_log_level) {
  return(wrcv_usr_ctrl_core_event_log_level(core_event_log_level));
}

err_code_t falcon_tsc_set_uc_lane_cfg(const phymod_access_t *pa, struct falcon_uc_lane_config_st struct_val) {
  _update_uc_lane_config_word(&struct_val);
  return(wrv_config_word(struct_val.word));
}

err_code_t falcon_set_usr_ctrl_lane_event_log_level(const phymod_access_t *pa, uint8_t lane_event_log_level) {
  return(wrv_usr_ctrl_lane_event_log_level(lane_event_log_level));
}

err_code_t falcon_set_usr_ctrl_disable_startup(const phymod_access_t *pa, struct falcon_usr_ctrl_disable_functions_st set_val) {
  _update_usr_ctrl_disable_functions_byte(&set_val);
#ifdef FALCON
  return(wrv_usr_ctrl_disable_startup_functions_word(set_val.word));
#else
  return(wrv_usr_ctrl_disable_startup_functions_byte(set_val.byte));
#endif
}

err_code_t falcon_set_usr_ctrl_disable_startup_dfe(const phymod_access_t *pa, struct falcon_usr_ctrl_disable_dfe_functions_st set_val) {
  _update_usr_ctrl_disable_dfe_functions_byte(&set_val);
  return(wrv_usr_ctrl_disable_startup_dfe_functions_byte(set_val.byte));
}

err_code_t falcon_set_usr_ctrl_disable_steady_state(const phymod_access_t *pa, struct falcon_usr_ctrl_disable_functions_st set_val) {
  _update_usr_ctrl_disable_functions_byte(&set_val);
#ifdef FALCON
  return(wrv_usr_ctrl_disable_steady_state_functions_word(set_val.word));
#else
  return(wrv_usr_ctrl_disable_steady_state_functions_byte(set_val.byte));
#endif
}

err_code_t falcon_set_usr_ctrl_disable_steady_state_dfe(const phymod_access_t *pa, struct falcon_usr_ctrl_disable_dfe_functions_st set_val) {
  _update_usr_ctrl_disable_dfe_functions_byte(&set_val);
  return(wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(set_val.byte));
}



/******************************************************************/
/*  APIs to Read Core/Lane Config and User variables from uC RAM  */
/******************************************************************/

err_code_t falcon_tsc_get_uc_core_config(const phymod_access_t *pa, struct falcon_uc_core_config_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdcv_config_word());
  _update_uc_core_config_st(get_val);

  return (ERR_CODE_NONE);
}

err_code_t falcon_get_usr_ctrl_core_event_log_level(const phymod_access_t *pa, uint8_t *core_event_log_level) {

  if(!core_event_log_level) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*core_event_log_level = rdcv_usr_ctrl_core_event_log_level());

  return (ERR_CODE_NONE);
}

err_code_t falcon_tsc_get_uc_lane_cfg(const phymod_access_t *pa, struct falcon_uc_lane_config_st *get_val) { 

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_config_word());
  _update_uc_lane_config_st(get_val);
  return (ERR_CODE_NONE);
}

err_code_t falcon_get_usr_ctrl_lane_event_log_level(const phymod_access_t *pa, uint8_t *lane_event_log_level) {

  if(!lane_event_log_level) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*lane_event_log_level = rdv_usr_ctrl_lane_event_log_level());
  return (ERR_CODE_NONE);
}

err_code_t falcon_get_usr_ctrl_disable_startup(const phymod_access_t *pa, struct falcon_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

#ifdef FALCON
  ESTM(get_val->word = rdv_usr_ctrl_disable_startup_functions_word());
#else
  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_functions_byte());
#endif
  _update_usr_ctrl_disable_functions_st(get_val);
  return (ERR_CODE_NONE);
}

err_code_t falcon_get_usr_ctrl_disable_startup_dfe(const phymod_access_t *pa, struct falcon_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_dfe_functions_byte());
  _update_usr_ctrl_disable_dfe_functions_st(get_val);
  return (ERR_CODE_NONE);
}

err_code_t falcon_get_usr_ctrl_disable_steady_state(const phymod_access_t *pa, struct falcon_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

#ifdef FALCON
  ESTM(get_val->word = rdv_usr_ctrl_disable_steady_state_functions_word());
#else
  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_functions_byte());
#endif
  _update_usr_ctrl_disable_functions_st(get_val);
  return (ERR_CODE_NONE);
}

err_code_t falcon_get_usr_ctrl_disable_steady_state_dfe(const phymod_access_t *pa, struct falcon_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_dfe_functions_byte());
  _update_usr_ctrl_disable_dfe_functions_st(get_val);
  return (ERR_CODE_NONE);
}



/******************************************/
/*  Serdes Register field Poll functions  */
/******************************************/

/* poll for microcontroller to populate the dsc_data register */
err_code_t falcon_poll_diag_done(const phymod_access_t *pa, uint16_t *status, uint32_t timeout_ms) {
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
		 EFUN(falcon_delay_us(10*timeout_ms));
	 }
 }
 return(_error(ERR_CODE_DIAG_TIMEOUT));
}

/* poll for microcontroller to populate the dsc_data register */
err_code_t falcon_poll_diag_eye_data(const phymod_access_t *pa, uint32_t *data,uint16_t *status, uint32_t timeout_ms) {
 uint8_t loop;
 uint16_t dscdata;
 if(!data || !status) {
	 return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

 for(loop=0;loop < 100; loop++) {
	 ESTM(*status=rdv_usr_diag_status());
	 if(((*status & 0x00FF) > 2) || ((*status & 0x8000) > 0)) {
		EFUN(falcon_tsc_pmd_uc_cmd(pa,  CMD_READ_DIAG_DATA_WORD, 0, 200));
        ESTM(dscdata = rd_uc_dsc_data());
		data[0] = _float8_to_int32(pa, (float8_t)(dscdata >>8));
		data[1] = _float8_to_int32(pa, (float8_t)(dscdata & 0x00FF));
		return(ERR_CODE_NONE);
	 }
	 if(loop>10) {
		 EFUN(falcon_delay_us(10*timeout_ms));
	 }
 }
 return(_error(ERR_CODE_DIAG_TIMEOUT));
}

#ifndef CUSTOM_REG_POLLING

/* Poll for field "uc_dsc_ready_for_cmd" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1(const phymod_access_t *pa, uint32_t timeout_ms) {
  
  uint16_t loop;
  err_code_t  err_code;
  /* read quickly for 4 tries */
  for (loop = 0; loop < 100; loop++) {
    uint16_t rddata;
    EFUN(falcon_pmd_rdt_reg(pa, DSC_A_DSC_UC_CTRL, &rddata));
    if (rddata & 0x0080) {    /* bit 7 is uc_dsc_ready_for_cmd */
      if (rddata & 0x0040) {  /* bit 6 is uc_dsc_error_found   */
		ESTM(USR_PRINTF(("ERROR : DSC command returned error (after cmd) cmd = 0x%x, supp_info = 0x%02x !\n", rd_uc_dsc_gp_uc_req(), rd_uc_dsc_supp_info()))); 
		return(_error(ERR_CODE_UC_CMD_RETURN_ERROR));
      } 
      return (ERR_CODE_NONE);
    }     
	if(loop>10) {
		err_code = falcon_delay_us(10*timeout_ms);
		if(err_code) return(err_code);
	}
  }
  USR_PRINTF(("ERROR : DSC ready for command is not working, applying workaround and getting debug info !\n"));
  ESTM(DISP(rd_uc_dsc_supp_info()));
  ESTM(DISP(rd_uc_dsc_gp_uc_req()));
  ESTM(DISP(rd_dsc_state()));
  /* artifically terminate the command to re-enable the command interface */
  wr_uc_dsc_ready_for_cmd(0x1);        
#ifdef SERDES_EVAL
  _eval_ucode_debug();                                /* Switch control back to emulation debug code */
#endif
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}    

/* Poll for field "dsc_state" = DSC_STATE_UC_TUNE [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon_poll_dsc_state_equals_uc_tune(const phymod_access_t *pa, uint32_t timeout_ms) {
  
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
		err_code = falcon_delay_us(10*timeout_ms);
		if(err_code) return(err_code);
	}
  }
  ESTM(USR_PRINTF(("DSC_STATE = %d\n", rd_dsc_state())));
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}    


#ifdef SERDES_RPTR
/* Poll for field "st_afe_tx_fifo_resetb" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon_poll_st_afe_tx_fifo_resetb_equals_1(uint32_t timeout_ms) {
  
  uint16_t loop;
  err_code_t  err_code;
  for (loop = 0; loop <= 100; loop++) {
    uint8_t st_afe_tx_fifo_resetb;
    ESTM(st_afe_tx_fifo_resetb = rd_st_afe_tx_fifo_resetb());
    if (st_afe_tx_fifo_resetb) {
      return (ERR_CODE_NONE);
    }
    err_code = falcon_delay_us(10*timeout_ms);
    if (err_code) {
      return (err_code);
    }
  }
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
} 
#endif


#ifdef FALCON_TSC
/* Poll for field "micro_ra_initdone" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon_poll_micro_ra_initdone(const phymod_access_t *pa, uint32_t timeout_ms) {
  
  uint16_t loop;
  err_code_t  err_code;
  uint8_t result;
  for (loop = 0; loop <= 100; loop++) {
    ESTM(result = rdc_micro_ra_initdone());
    if (result) {
      return (ERR_CODE_NONE);
    }    
    err_code = falcon_delay_us(10*timeout_ms);
    if (err_code) {
      return (err_code);
    }
  }
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}
#endif

#endif /* CUSTOM_REG_POLLING */

#ifdef SERDES_DEFAULT_MICRO
#ifdef FALCON_TSC

err_code_t falcon_init_pram_for_uc_load(const phymod_access_t *pa, uint16_t ucode_len) {

    wrc_micro_master_clk_en(0x1);                           /* Enable clock to microcontroller subsystem */
    wrc_micro_master_rstb(0x1);                             /* De-assert reset to microcontroller sybsystem */
    wrc_micro_master_rstb(0x0);                             /* Assert reset to microcontroller sybsystem - Toggling reset*/
    wrc_micro_master_rstb(0x1);                             /* De-assert reset to microcontroller sybsystem */

    wrc_micro_ra_init(0x1);                                 /* Set initialization command to initialize code RAM */
    EFUN(falcon_poll_micro_ra_initdone(pa, 250));           /* Poll for micro_ra_initdone = 1 to indicate initialization done */
    
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
    /* call falcon_finish_pram_load */
    return(ERR_CODE_NONE);
}

err_code_t falcon_finish_pram_load(const phymod_access_t *pa) {
    wrc_micro_pramif_en(0);
    wrc_micro_core_clk_en(1);
   /* wrc_micro_core_rstb(1); */
    return(ERR_CODE_NONE);
}

#else 
err_code_t falcon_init_pram_for_uc_load(const phymod_access_t *pa, uint16_t ucode_len) {
    uint8_t result;

    if (ucode_len > UCODE_MAX_SIZE) {                     /* uCode size should be less than UCODE_MAX_SIZE  */
      return (_error(ERR_CODE_INVALID_UCODE_LEN));
    } 
    wrc_micro_mdio_dw8051_reset_n(0x0);
    wrc_micro_system_clk_en(0x1);                         /* Enable clock to micro  */
    wrc_micro_system_reset_n(0x1);                        /* De-assert reset to micro */
    wrc_micro_system_reset_n(0x0);                        /* Assert reset to micro - Toggling micro reset*/
    wrc_micro_system_reset_n(0x1);                        /* De-assert reset to micro */ 

    wrc_micro_pram_if_flop_bypass(1);
    wrc_micro_mdio_ram_access_mode(0x0);                  /* Select Program Memory access mode - Program RAM load when Serdes DW8051 in reset */
    wrc_micro_byte_mode(0);

    wrc_micro_ram_address(0x0);                           /* RAM start address */
         
    wrc_micro_init_cmd(0x0);                              /* Clear initialization command */ 
    wrc_micro_init_cmd(0x1);                              /* Set initialization command */
    wrc_micro_init_cmd(0x0);                              /* Clear initialization command */

    EFUN(falcon_delay_us(500));
 
    ESTM(result = !rdc_micro_init_done());
    if (result) {                                         /* Check if initialization done within 500us time interval */
        return (_error(ERR_CODE_MICRO_INIT_NOT_DONE));    /* Else issue error code */
    }

    /* ucode_len_padded = ((ucode_len + 7) & 0xFFF8); */  /* Aligning ucode size to 8-byte boundary */
    wrc_micro_pram_if_en(1);
    /* set the pram_abilty input of the pram interface to 1b1 */
    /* pram_abilty = 1; */
    /* Wait 8 pram clocks */
    wrc_micro_pram_if_rstb(1);
    /* Wait 8 pram clocks */
    /* write ucode into pram */
    /* Wait 8 pram clocks */
    /* call falcon_finish_pram_load */
    return(ERR_CODE_NONE);
}

err_code_t falcon_finish_pram_load(const phymod_access_t *pa) {
    /* set the pram_abilty input of the pram interface to 1b0 */
    /* pram_abilty = 0; */
    wrc_micro_pram_if_en(0);
    wrc_micro_pram_if_rstb(0);
    /* wr_mdio_dw8051_reset_n(1); */
    return(ERR_CODE_NONE);
}
#endif
#endif
/****************************************/
/*  Serdes Register/Variable Dump APIs  */
/****************************************/

err_code_t falcon_reg_dump(const phymod_access_t *pa) {

  uint16_t addr, rddata;

  USR_PRINTF(("\n\n**********************************\n")); 
  USR_PRINTF(("****  SERDES REGISTER DUMP    ****\n")); 
  USR_PRINTF(("**********************************\n")); 
  USR_PRINTF(("****    ADDR      RD_VALUE    ****\n")); 
  USR_PRINTF(("**********************************\n"));

  for (addr = 0x0; addr < 0xF; addr++) {
    EFUN(falcon_pmd_rdt_reg(pa, addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }

  for (addr = 0x90; addr < 0x9F; addr++) {
    EFUN(falcon_pmd_rdt_reg(pa, addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }

  for (addr = 0xD000; addr < 0xD150; addr++) {
    EFUN(falcon_pmd_rdt_reg(pa, addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }

  for (addr = 0xD200; addr < 0xD230; addr++) {
    EFUN(falcon_pmd_rdt_reg(pa, addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }

  for (addr = 0xFFD0; addr < 0xFFE0; addr++) {
    EFUN(falcon_pmd_rdt_reg(pa, addr,&rddata));
    USR_PRINTF(("       0x%04x      0x%04x\n",addr,rddata));
  }
  return (ERR_CODE_NONE);
}


err_code_t falcon_uc_core_var_dump(const phymod_access_t *pa) {

  uint8_t     addr, rddata;
  err_code_t  err_code = ERR_CODE_NONE;

  USR_PRINTF(("\n\n******************************************\n")); 
  USR_PRINTF(("**** SERDES UC CORE RAM VARIABLE DUMP ****\n")); 
  USR_PRINTF(("******************************************\n")); 
  USR_PRINTF(("****       ADDR       RD_VALUE        ****\n")); 
  USR_PRINTF(("******************************************\n")); 

  for (addr = 0x0; addr < 0xFF; addr++) {  
    rddata = falcon_rdbc_uc_var(pa, &err_code, addr);
    if (err_code) {
      return (err_code);
    }
    USR_PRINTF(("           0x%02x         0x%02x\n",addr,rddata));
  }
  return (ERR_CODE_NONE);
}


err_code_t falcon_uc_lane_var_dump(const phymod_access_t *pa) {

  uint8_t     rddata;
  uint16_t    addr;
  err_code_t  err_code = ERR_CODE_NONE;
#ifndef SERDES_DEFAULT_MICRO
  falcon_uc_lane_info_st lane_info;
#endif

  USR_PRINTF(("\n\n********************************************\n")); 
  USR_PRINTF(("**** SERDES UC LANE %d RAM VARIABLE DUMP ****\n",falcon_get_lane(pa ))); 
  USR_PRINTF(("********************************************\n")); 
  USR_PRINTF(("*****       ADDR       RD_VALUE        *****\n")); 
  USR_PRINTF(("********************************************\n")); 

  #ifndef SERDES_DEFAULT_MICRO                                      /* MERLIN GALLARDO, QUADRA, SESTO, COUNTACH */
    err_code = falcon_get_uc_ln_info(&lane_info);
    if (err_code) {
      return (err_code);
    }

    for (addr = 0x0; addr < lane_info.lane_ram_size; addr++) {  
      rddata = falcon_rdbl_uc_var(pa, &err_code, addr);
      if (err_code) {
        return (err_code);
      }
      USR_PRINTF(("            0x%03x         0x%02x\n",addr,rddata));
    }
  #else
    for (addr = 0x0; addr < LANE_VAR_RAM_SIZE; addr++) {  
      rddata = falcon_rdbl_uc_var(pa, &err_code, addr);
      if (err_code) {
        return (err_code);
      }
      USR_PRINTF(("            0x%03x         0x%02x\n",addr,rddata));
    }
  #endif 
  return (ERR_CODE_NONE);
}



/************************/
/*  Serdes API Version  */
/************************/

err_code_t falcon_api_version(const phymod_access_t *pa, uint32_t *api_version) {

	if(!api_version) {
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
	}
	*api_version = 0xA10107;
	return (ERR_CODE_NONE);
}


/***************************************/
/*  API Function to Read Event Logger  */
/***************************************/

err_code_t falcon_read_event_log(const phymod_access_t *pa, uint8_t *trace_mem,enum _falcon_comn_event_log_display_mode_enum display_mode) {

	/* validate input arguments */
	if (trace_mem == NULL || (display_mode >= EVENT_LOG_MODE_MAX)) 
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

	/* stop writing event log */
	EFUN(falcon_event_log_stop(pa ));

	EFUN(falcon_event_log_readmem(pa, trace_mem));

	EFUN(falcon_event_log_display(pa, trace_mem, display_mode));

	return(ERR_CODE_NONE);
}

err_code_t falcon_event_log_stop(const phymod_access_t *pa) {


	USR_PRINTF(("\n\n********************************************\n")); 
	USR_PRINTF(("**** SERDES UC TRACE MEMORY DUMP ***********\n")); 
	USR_PRINTF(("********************************************\n")); 

	/* Start Read to stop uC logging and read the word at last event written by uC */
	EFUN(falcon_tsc_pmd_uc_cmd(pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_START, 10));

	return(ERR_CODE_NONE);
}

err_code_t falcon_event_log_readmem(const phymod_access_t *pa, uint8_t *trace_mem) {
	uint8_t uc_dsc_supp_info;
	uint16_t addr=0,read_idx=0,trace_mem_size=0;
#ifndef SERDES_DEFAULT_MICRO
    falcon_uc_lane_info_st uc_lane_info;
#endif
	/* validate input arguments */
	if (trace_mem == NULL)  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

	ESTM(read_idx = rdcv_trace_mem_rd_idx());
	USR_PRINTF(("\n  DEBUG INFO: trace memory read index = 0x%04x\n", read_idx));

#ifndef SERDES_DEFAULT_MICRO
	/* get trace memory size */
    EFUN(falcon_get_uc_ln_info(&uc_lane_info));
	trace_mem_size = uc_lane_info.trace_mem_size;
#else
	trace_mem_size = 768;
#endif
	USR_PRINTF(("  DEBUG INFO: trace memory size = 0x%04x\n\n", trace_mem_size));

	do {
		/* Read Next */
		EFUN(falcon_tsc_pmd_uc_cmd(pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_NEXT, 10));

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
	EFUN(falcon_tsc_pmd_uc_cmd(pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_DONE, 10));

	return(ERR_CODE_NONE);
}

err_code_t falcon_event_log_display(const phymod_access_t *pa, uint8_t *trace_mem,enum _falcon_comn_event_log_display_mode_enum display_mode) {
#define MAX_ENTRY_SIZE   (7)
#define NOR_ENTRY_SIZE   (4)
	uint16_t trace_mem_size=0;
	uint8_t is_ref_time_found=0,is_last_event_timewrap=0;
	uint8_t post_cursor=0,curr_cursor=0,prev_cursor=0;
	uint16_t addr=0, rr=0, prev_rr=0;
	uint8_t cc=0;
	uint16_t time=0,ref_time=0,num_time_wraparound=0,this_num_time_wraparound=0;
	USR_DOUBLE prev_time=0,curr_time=0;
	uint8_t word_per_row=8;
	uint8_t prev_event=0;
    uint8_t supp_info[MAX_ENTRY_SIZE-NOR_ENTRY_SIZE];
#ifndef SERDES_DEFAULT_MICRO
    falcon_uc_lane_info_st uc_lane_info;
#endif
	/* validate input arguments */
	if (trace_mem == NULL || (display_mode >= EVENT_LOG_MODE_MAX))
		return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

#ifndef SERDES_DEFAULT_MICRO
	/* get trace memory size */
    EFUN(falcon_get_uc_ln_info(&uc_lane_info));
	trace_mem_size = uc_lane_info.trace_mem_size;
#else
	trace_mem_size = 768;
#endif

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
		while (addr < trace_mem_size) {
			
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
					_display_event(pa, prev_event,(uint8_t)prev_rr,prev_cursor,curr_cursor,post_cursor,supp_info);
					is_last_event_timewrap = 1;
				}

				USR_PRINTF(("\n  %5d timestamp wraparound(s). \n\n", this_num_time_wraparound));
				this_num_time_wraparound = 0;
				
				addr++;
				continue; 
			}
			else {
				cc = (trace_mem[addr] & 0x1f);                                                         /* lane id */
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
                        _display_event(pa, prev_event,(uint8_t)prev_rr,prev_cursor,curr_cursor,post_cursor,supp_info);
					}
					else {
						is_last_event_timewrap = 0;
					}
				}

				USR_PRINTF(("  Lane %d: ",cc));
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


err_code_t falcon_display_state (const phymod_access_t *pa) {

  err_code_t err_code;
  err_code = ERR_CODE_NONE;

  if (!err_code) err_code = falcon_tsc_display_core_state(pa );
  if (!err_code) err_code = falcon_tsc_display_lane_state_hdr(pa );
  if (!err_code) err_code = falcon_tsc_display_lane_state(pa );

  return (err_code);
}

err_code_t falcon_display_config (const phymod_access_t *pa) {

  err_code_t err_code;
  err_code = ERR_CODE_NONE;

  if (!err_code) err_code = falcon_display_core_config(pa );
  if (!err_code) err_code = falcon_display_lane_config(pa );

  return (err_code);
}


/**********************************************/
/*  Temperature forcing and reading			  */
/**********************************************/

err_code_t falcon_force_die_temperature (const phymod_access_t *pa, int16_t die_temp) {
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

err_code_t falcon_tsc_read_die_temperature (const phymod_access_t *pa, int16_t *die_temp) {
    /* Formula for PVTMON: T = 410.04-0.48705*reg10bit (from PVTMON Analog Module Specification v5.0 section 6.2) */
    int32_t die_temp_sensor_reading;
    
    EFUN(falcon_tsc_pmd_uc_cmd(pa, CMD_UC_DBG,CMD_UC_DBG_DIE_TEMP,100));
    ESTM(die_temp_sensor_reading=rd_uc_dsc_data());
       
    *die_temp = (int16_t)((420697 - ((die_temp_sensor_reading<<8) + (die_temp_sensor_reading<<7) + (die_temp_sensor_reading<<6) + (die_temp_sensor_reading<<5) + (die_temp_sensor_reading<<4) + (die_temp_sensor_reading<<1) + die_temp_sensor_reading))>>10); 
    
    return(ERR_CODE_NONE);    
}




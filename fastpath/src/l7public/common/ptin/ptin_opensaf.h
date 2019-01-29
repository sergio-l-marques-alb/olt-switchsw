/**
 * ptin_opensaf.h
 *
 * Implements the opensaf interface module
 *
 * Created on: 2016/07/04 Author: Rui Fernandes(rui-f-fernandes@alticealbs.com)
 * Notes:
 *
 */
#ifndef _SAEVT_H
#define _SAEVT_H

#include "ptin_include.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define MAC_SIZE_BYTES 6
#define ENCRYPTION_KEY_FIELD_SIZE 16
#define MAX_NGPON2_PORTS          32

/* external stuctures from MAC PON*/
typedef struct  {
    L7_uint8 losi ;  /* LOSi */
    L7_uint8 lofi ;  /* LOFi */
    L7_uint8 loami ; /* LOAMi */
    L7_uint8 lcdgi ; /* LCDGi */
    L7_uint8 rdii ;  /* RDIi */
    L7_uint8 sufi ;  /* SUFi */
    L7_uint8 loai;   /* LOAi */
    L7_uint8 dgi;    /* DGi */
    L7_uint8 dfi;    /* DFi */
    L7_uint8 dowi;   /* DOWi */
    L7_uint8 tiwi;   /* TIWi */
    L7_uint8 sfi;    /* SFi */
    L7_uint8 sdi;    /* SDi */
    L7_uint8 loki;   /* LOKi */
    L7_uint8 tca_fec_corrected_byte;         /* TCA FEC corrected byte */
    L7_uint8 tca_fec_corrected_code_word;    /* TCA FEC corrected code word */
    L7_uint8 tca_fec_uncorrected_code_word;  /* TCA FEC uncorrected code word */
    L7_uint8 tca_bip_error;                  /* TCA BIP error */
    L7_uint8 tca_rei_error;                  /* TCA REI error */
} __attribute__ ((packed)) ptin_oltPonOnuAlarms;

typedef struct {
    L7_uint8             state;
    L7_uint32            eqd;
    L7_uint8             aes_key[ENCRYPTION_KEY_FIELD_SIZE];
    L7_uint8             serial[8];
    L7_uint8             password[10];
    ptin_oltPonOnuAlarms alarms;
    L7_uint8             activeMember;   //NOVO
    L7_uint8             futureMember;
    L7_uint8             availableChannels;
    L7_uint16            stateSync[MAX_NGPON2_PORTS]; //NOVO
}  __attribute__ ((packed))ptin_onuStatus;



/*********************************************************** *
 *  Init routine of opensaf event task
 *
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_opensaf_event_task_init();

/**
 * Read a event
 *
 * @param void data
 * @param int len
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_opensaf_read_event(void *data, int len, int id, char *chName, char *pubName);

#endif


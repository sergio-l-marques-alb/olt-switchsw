/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OAM test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * $Id: cint_oam.c,v 1.21 Broadcom SDK $
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
 * 
 * File: cint_oam.c
 * Purpose: Example of using OAM APIs.
 *
 * Usage:
 * 
 * cint
 * print bcm_oam_init(unit);
 * print oam_example(0);
 * 
 * This cint uses cint_vswitch_metro_mp_single_vlan.c to build the following vswitch:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  |                                                                       |
 *  |                   o  \                         o                      |
 *  |                      \  \<4>        <10,20>/  /                       |
 *  |                  /\ \  \   -----------    /  /                        |
 *  |                   \  \ \/ /  \   /\   \  /  / /\                      |
 *  |                <40>\  \  |    \  /     |\/ /  /                       |
 *  |                     \  \ |     \/      |  /  /<30>                    |
 *  |                       p3\|     /\      | /  /                         |
 *  |                          |    /  \     |/p1                           |             
 *  |                         /|   \/   \    |                              |
 *  |                        /  \  VSwitch  /                               | 
 *  |                   /\  /p2  -----------                                |
 *  |                <5>/  /  /                                             |
 *  |                  /  /  /<3>                                           |
 *  |                 /  /  /                                               |
 *  |                   /  \/                                               |
 *  |                  o                                                    |
 *  |                                                                       | 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * The following MAs:
 * 		1) MA with id 0 & long name format:       1-const; 32-long format; 13-length; all the rest - MA name
 *         long_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 32, 13, 1, 2, 3, 4, 5, 6, 7, 8, 9}
 * 		2) MA with id 1 & short name format:    1-const; 3-short format; 2-length; all the rest - MA name
 *        short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xcd, 0xab}
 * 
 * The following MEPs:
 * 		1) MEP with id 0: non-accellerated, mac address {0x00, 0x00, 0x00, 0x01, 0x02, 0x03}, in group 2, mdlevel 4, downmep
 *  	2) MEP with id 4096: accellerated,     mac address {0x00, 0x00, 0x00, 0xff, 0xff, 0xff}, in group 2, mdlevel 5, upmep
 *  	3) RMEP with id 0
 * 
 * In addition, get & delete APIs are used for testing.
 * 
 * comments:
 * 1) In order to prevent from OAMP send packets do: BCM.0> m OAMP_MODE_REGISTER TRANSMIT_ENABLE=0
 * 
 */
 
/*
 * Creating vswitch and adding mac table entries
 */

/* set in order to do rmep_id encoding using utility functions and not using APIs. */
int encoding = 0;
bcm_oam_endpoint_t remote_id_system = 0;
bcm_oam_endpoint_t local_id_system = 0;

/* Globals - MAC addresses , ports & gports*/
  bcm_multicast_t mc_group = 1234;
  bcm_mac_t mac_mep_1 = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
  bcm_mac_t mac_mep_2 = {0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
  bcm_mac_t mac_mep_3 = {0x00, 0x00, 0x00, 0x01, 0x02, 0xff};
  bcm_mac_t mac_mep_4 = {0x00, 0x00, 0x00, 0xff, 0xff, 0xfd};
  bcm_mac_t mac_mip = {0x00, 0x00, 0x00, 0x01, 0x02, 0xfe};
  bcm_mac_t src_mac_mep_2 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
  bcm_mac_t src_mac_mep_3 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x01};
  bcm_mac_t src_mac_mep_4 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x07};
  bcm_mac_t mac_mep_2_mc = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x35}; /* 01-80-c2-00-00-3 + md_level_2 */
  bcm_gport_t gport1, gport2, gport3; /* these are the ports created by the vswitch*/
  uint32 port_1 = 13; /* physical port (signal generator)*/
  uint32 port_2 = 14;
  uint32 port_3 = 15;
  bcm_oam_group_info_t group_info_long_ma;
  bcm_oam_group_info_t group_info_short_ma;
  bcm_oam_group_info_t group_info_11b_ma;
  bcm_oam_group_info_t group_info_48b_ma;
  bcm_oam_endpoint_info_t mep_acc_info;
  bcm_oam_endpoint_info_t rmep_info;
  /*1-const; 32-long format; 13-length; all the rest - MA name*/
  uint8 long_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 32, 13, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0xa, 0xb, 0xc, 0xd};
  /*1-const; 3-short format; 2-length; all the rest - MA name*/
  uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
  /*4-const; 3-MD-Length; MD-Name+Zero-padding; 2-const; MA-Length; MA-name*/
  uint8 str_11b_name[BCM_OAM_GROUP_NAME_LENGTH] = {4, 3, 'A', 'T', 'T', 00, 00, 02, 04, 'E', 'V', 'C', '1', 0, 0};
  
  uint8 str_48b_name[BCM_OAM_GROUP_NAME_LENGTH] = {0};

  int timeout_events_count = 0;
  int remote_event_count=0;
  int port_interface_event_count=0;
  int timeout_events_count_multi_oam[2] ={ 0 };
  bcm_vlan_t  vsi;
  int md_level_mip = 7;
  bcm_oam_endpoint_info_t mip_info;
  int sampling_ratio = 0;
  int use_port_interface_status = 0;
  int use_11b_maid = 0;

/* enable OAM statistics per up mep session, need to load the cint: cint_field_oam_statistics.c*/
int oam_up_statistics_enable=0;


/**
 * NTP timestamp format will be used by default, unless disabled 
 * by soc property 
 * 
 * @author sinai (09/12/2014)
 * 
 * @param unit 
 * 
 * @return bcm_oam_timestamp_format_t 
 */
bcm_oam_timestamp_format_t get_oam_timestamp_format(int unit) {
    return soc_property_get(unit , "oam_dm_ntp_enable",1)  ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1;
}


/****************************************************************/
/*                              OAM UTILITIES FUNCTIONS                                                       */
/****************************************************************/



/**
 * Set the ID when calling bcm_oam_endpoint_create() with the 
 * flag _WITH_ID set. 
 *  
 * 
 * @param mep_info 
 * 
 * @return int 
 */
int system_endpoint_to_endpoint_id_oam(bcm_oam_endpoint_info_t *mep_info) {
    if (mep_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
        mep_info->id |= (1 << 25/*_BCM_OAM_REMOTE_MEP_INDEX_BIT*/);
        
    }
    else if (!(mep_info->opcode_flags & BCM_OAM_OPCODE_CCM_IN_HW)) {
        printf("Can not create non-accelerated endpoint with id\n");
		return BCM_E_FAIL;
    }
    return BCM_E_NONE;
}


/**
 * Functions used to read various event counters.
 * 
 * 
 * @param expected_event_count 
 * 
 * @return int 
 */
int read_timeout_event_count(int expected_event_count) {
	printf("timeout_events_count=%d\n",timeout_events_count);
	if (timeout_events_count==expected_event_count) {
		return BCM_E_NONE;
	}
	else {
		return BCM_E_FAIL;
	}
}

int read_remote_timeout_event_count(int expected_count) {
    printf("timeout_events_count=%d\n",remote_event_count);
    if (remote_event_count==expected_count) {
        return BCM_E_NONE;
    }
    else {
        return BCM_E_FAIL;
    }
}

int read_port_interface_event_count(int expected_count) {
    printf("port_interface_event_count=%d\n",port_interface_event_count);
    if (port_interface_event_count==expected_count) {
        return BCM_E_NONE;
    }
    else {
        return BCM_E_FAIL;
    }
}

/**
 * Function verifies that for each of the two accelerated 
 * endpoints, the cb was called as many times as expected. 
 * 
 * @author sinai (24/11/2013)
 * 
 * @param expected_event_count 
 * 
 * @return int 
 */
int read_timeout_event_count_multi_oam(int expected_event_count) {
    int i;
    for (i=0 ; i<2 ; ++i) {
        if (timeout_events_count_multi_oam[i] !=expected_event_count) {
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}




/* Send a packet from string input */
int tx(int unit, char *data, int *ptch) {
    char tmp, data_iter;
    int data_len, i, pkt_len, ind, data_base;
    bcm_pkt_t *pkt_info;
    bcm_error_t rv;
    
    /* If string data starts with 0x or 0X, skip it */
    if ((data[0] == '0') && ((data[1] == 'x') || (data[1] == 'X'))) {
        data_base = 2;
    } else {
        data_base = 0;
    }
    
    /* figure out packet data_length (no strdata_len in cint) */
    data_len=0;
    i=0;
    while (data[data_base+data_len] != 0) ++data_len;
    
    pkt_len = 64+2; /* two ptch bytes */
    pkt_len = (pkt_len < data_len+1 ? (data_len+2) : pkt_len);
    
        rv = bcm_pkt_alloc(unit, pkt_len, BCM_TX_CRC_ALLOC, &pkt_info);
        BCM_IF_ERROR_RETURN(rv);
        
        sal_memset(pkt_info->_pkt_data.data, 0, pkt_len);
    
        pkt_info->pkt_data = &pkt_info->_pkt_data;

    /* PTCH - 2B */
    pkt_info->_pkt_data.data[0] = ptch[0];
    pkt_info->_pkt_data.data[1] = ptch[1];

    /* Convert char to value */
    i = 0;
    while (i < data_len) {
        data_iter=data[data_base+i];
        if (('0' <= data_iter) && (data_iter <= '9')) {
            tmp = data_iter - '0';
        } else if (('a' <= data_iter) && (data_iter <= 'f')) {
            tmp = data_iter - 'a' + 10;
        } else if (('A' <= data_iter) && (data_iter <= 'F')) {
            tmp = data_iter - 'A' + 10;
        } else {
            printf("Unexpected char: %c\n", data_iter);
            return BCM_E_PARAM;
        }       
        
        /* String input is in 4b unit. Below we're filling in 8b:
           offset is /2, and we shift by 4b if the input char is odd */        
        pkt_info->_pkt_data.data[2+i/2] |= tmp << (((i+1)%2)*4);
        ++i;
    }
    
    rv = bcm_tx(unit, pkt_info, NULL);
    
    return rv;
}

/**
 * Wrapper function for transmitting packet, including PTCH.
 * 
 * 
 * @param unit 
 * @param data 
 * @param ptch0 - set local port from which packet should be 
 *              transmitted.
 * @param ptch1 
 * 
 * @return int 
 */
int oam_tx(int unit, char *data, int ptch0, int ptch1) {
    int ptch[2];
    ptch[0] = ptch0;
    ptch[1] = ptch1;

    return  tx(unit, data, &ptch);

}


/* OAM-TS header:
 *
 * OAM-TS header is the following:
 * bits                       meaning
 * ===================================
 * 47:46                      type of OAM-TS extension: 0-OAM 
 * 45:43                      OAM-Sub-Type: 0-CCM; 1-LM; 2-DM (1588 ToD) 3-DM (NTP ToD)
 * 42                         Up-MEP ('1') or Down-MEP ('0')
 * 41:8                       OAM-TS-Data: Interpretation of OAM-TS-Data depends on the OAM-TS type, sub type, and location in the system.
 *										Location in the system may be:
 *										IRPP - following ITMH on an injected packet
 *										ETPP or Out I/F - following FTMH
 *										OAM-Data is a function of (Location, Sub-Type, MEP-Type, Inject/Trap):
 *										"	(IRPP, LM, Up, Inj) - NA
 *										"	(IRPP, LM, Dn, Inj) - Counter-Pointer // bypass
 *										"	(IRPP, DM, Up, Inj) - NA
 *										"	(IRPP, DM, Dn, Inj) - Null // bypass
 *										"	(IRPP, LM/DM, Up/Dn, Trp) - NA
 *										"	(ETPP, LM, Up, Inj) - Counter-Value // Stamp into packet
 *										"	(ETPP, LM, Dn, Inj) - Counter-Pointer // Read counter and stamp into packet
 *										"	(ETPP, DM, Up, Inj) - ToD // Stamp into packet
 *										"	(ETPP, DM, Dn, Inj) -Null //Read ToD and stamp into packet
 *
 *										"	(ETPP, LM, Up, Trp) - NA // ETPP build OAM-TS Header w/ Counter-Value
 *										"	(ETPP, LM, Dn, Trp) - Counter-Value // bypass to Out I/F
 *										"	(ETPP, DM, Up, Trp) - NA // ETPP build OAM-TS Header w/ ToD
 *  										"	(ETPP, DM, Dn, Trp) - ToD// bypass to Out I/F
 * 7:0                         Offset from end of System Headers to where to stamp/update.
 * 
 * parsing pkt_dm_down packet:
 * 0x800c000d|180000000016|000000010203000000ffffff8100|000a8902|A02f002000000000000003e70000
 *    ITMH       OAM-TS            ETH.                  VLAN       OAM
 * ITMH - send packet to port 13
 * OAM-TS - packet type DM, stamping offset - 16
 * 
 * parsing pkt_lm_down packet:
 * 0x800c000d|080000000516|000000ffffff0000000000018100|000a8902|802b000c000000000000000000000000
 *    ITMH       OAM-TS            ETH.                  VLAN       OAM
 * ITMH - send packet to port 13
 * OAM-TS - packet type LM, stamping offset - 16, counter id - 5
 *  
 * parsing pkt_lm_up packet: no headers
 * 0x000000010203000000ffffff8100|000a8902|A02b000c0000000000000000000000000
 *            ETH.                  VLAN       OAM
 */
int inject_dm_and_lm_packets(int unit) {
  bcm_error_t rv;
  char *pkt_dm_down, *pkt_dm_up, *pkt_lm_down, *pkt_lm_up;
  int ptch_down[2];
  int ptch_up[2];

  pkt_dm_down = "0x800c000d180000000016000000010203000000ffffff8100000a8902A02f002000000000000003e70000"; /*DM down*/
  pkt_lm_down = "0x800c000d080000000516000000ffffff0000000000018100000a8902802b000c000000000000000000000000"; /*LM down*/
  ptch_down[0] = 0; /* The next header is ITMH */
  ptch_down[1] = 0; /* in port is port_1 */

  pkt_dm_up = "0x000000010203000000ffffff810000058902A02f0020000000000000000000000000"; /*DM up*/
  pkt_lm_up = "0x000000010203000000ffffff810000058902A02b000c0000000000000000000000000"; /*LM up*/
  ptch_up[0] = 240; /* Next header should be deduced from the SSP; Opaque-PT-Attributes = 7 */
  ptch_up[1] = port_2; /* SSP = port_2 */

  rv = tx(unit, pkt_lm_down, ptch_down);
  rv = tx(unit, pkt_dm_down, ptch_down);
  rv = tx(unit, pkt_lm_up, ptch_up);
  rv = tx(unit, pkt_dm_up, ptch_up);
  return rv;
}




/*****************************************************************************/
/*                                        OAM BASIC EXAMPLE                                                                               */
/*****************************************************************************/



/**
 * Basic OAM example.
 * Creates vswitch on 3 given ports and the folowing endpoint: 
 * 1) Default endpoint 
 * 2) Non accelerated down MEP on port1
 * 3) Accelerated up MEP on port2 + RMEP 
 * 4) Accelerated down MEP on port1 + RMEP
 *  
 * 
 * @param unit 
 * @param port1 
 * @param port2 
 * @param port3 
 * 
 * @return int 
 */
int oam_run_with_defaults(int unit, int port1, int port2, int port3) {
  bcm_error_t rv;

  single_vlan_tag = 1;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;

/*enable oam statistics per mep session*/
  if (oam_up_statistics_enable) {
	  rv = oam_up_stat(unit); 
	  if (rv != BCM_E_NONE) {
		  printf("(%s) \n",bcm_errmsg(rv));
		  return rv;
	  }
  }

  
  rv = oam_example(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  rv = register_events(unit);
  return rv;
}


/**
 * Basic OAM example. creates the following entities: 
 * 1) vswitch on which OAM endpoints are defined. 
 * 2) OAM group with short MA name (set on outgoing CCMs for
 * accelerated endpoints). 
 * 3) Default endpoint. 
 * 4) Non accelerated endpoint 
 * 5) Accelerated down MEP 
 * 6) Accelerated up MEP 
 * 
 * 
 * @param unit 
 * 
 * @return int 
 */
int oam_example(int unit) {
  bcm_error_t rv;
  bcm_oam_group_info_t group_info_long_ma_test;
  bcm_oam_group_info_t *group_info;
  bcm_oam_endpoint_info_t mep_not_acc_info;
  bcm_oam_endpoint_info_t mep_not_acc_test_info;
  bcm_oam_endpoint_info_t default_info;

  int md_level_1 = 4;
  int md_level_2 = 5;
  int md_level_3 = 2;
  int md_level_4 = 1;
  int lm_counter_base_id_1  ;
  int lm_counter_base_id_2  ;

  rv = create_vswitch_and_mac_entries(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  printf("Creating two groups (short, long and 11b-maid name format)\n");
  bcm_oam_group_info_t_init(&group_info_long_ma);
  sal_memcpy(group_info_long_ma.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
  rv = bcm_oam_group_create(unit, &group_info_long_ma);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  bcm_oam_group_info_t_init(&group_info_short_ma);
  sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
  rv = bcm_oam_group_create(unit, &group_info_short_ma);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  /*Endpoints will be created on 2 different LIFs. */
  rv = set_counter_source_and_engines(unit,&lm_counter_base_id_2);
  BCM_IF_ERROR_RETURN(rv); 
  rv = set_counter_source_and_engines(unit,&lm_counter_base_id_1);
  BCM_IF_ERROR_RETURN(rv); 



  if (use_11b_maid) {
      /* Add a group with sttring based 11b MAID */
      bcm_oam_group_info_t_init(&group_info_11b_ma);
      sal_memcpy(group_info_11b_ma.name, str_11b_name, BCM_OAM_GROUP_NAME_LENGTH);
      rv = bcm_oam_group_create(unit, &group_info_11b_ma);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n", bcm_errmsg(rv));
          return rv;
      }
      /* Set the used group for the MEPs to this group */
      group_info = &group_info_11b_ma;
  }
  else {
      /* Set the used group for the MEPs to the group with the short name */
      group_info = &group_info_short_ma;
  }

  bcm_oam_group_info_t_init(&group_info_long_ma_test);
  printf("bcm_oam_group_get\n"); 
  rv = bcm_oam_group_get(unit, group_info_long_ma.id, &group_info_long_ma_test);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }


  printf("bcm_oam_group_destroy\n"); 
  rv = bcm_oam_group_destroy(unit, group_info_long_ma.id);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  rv = bcm_oam_group_create(unit, &group_info_long_ma);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  rv = get_device_type(unit, &device_type);
  if (rv < 0) {
      printf("Error checking whether the device is arad+.\n");
      print rv;
      return rv;
  }

  if (device_type<device_type_jericho) {
      /*
      * Adding a default MEP
      */
      
      printf("Add default mep\n");
      bcm_oam_endpoint_info_t_init(&default_info);
      if (device_type < device_type_arad_plus) {
          default_info.id = -1;
      }
      else {
          default_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS0;
      }
      default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
      default_info.timestamp_format = get_oam_timestamp_format(unit);
      rv = bcm_oam_endpoint_create(unit, &default_info);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }
  }

  /*
  * Adding non acc MEP
  */

  bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
  mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
  mep_not_acc_info.group = group_info->id;
  mep_not_acc_info.level = md_level_1;
  mep_not_acc_info.gport = gport1;
  mep_not_acc_info.name = 0;     
  mep_not_acc_info.ccm_period = 0;
  sal_memcpy(mep_not_acc_info.dst_mac_address, mac_mep_1, 6);
  mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_1;

  printf("bcm_oam_endpoint_create mep_not_acc_info\n"); 
  mep_not_acc_info.timestamp_format = get_oam_timestamp_format(unit);
  rv = bcm_oam_endpoint_create(unit, &mep_not_acc_info);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }
  printf("created MEP with id %d\n", mep_not_acc_info.id);

  /*
  * Adding acc MEP - upmep
  */

  bcm_oam_endpoint_info_t_init(&mep_acc_info);

  /*TX*/
  mep_acc_info.type = bcmOAMEndpointTypeEthernet;
  mep_acc_info.group = group_info->id;
  mep_acc_info.level = md_level_2;
  mep_acc_info.tx_gport = BCM_GPORT_INVALID; /*Up MEP requires gport invalid.*/
  mep_acc_info.name = 123;     
  mep_acc_info.ccm_period = 1;
  mep_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
  mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

  mep_acc_info.vlan = 5;
  mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
  mep_acc_info.outer_tpid = 0x8100;
  mep_acc_info.inner_vlan = 0;
  mep_acc_info.inner_pkt_pri = 0;     
  mep_acc_info.inner_tpid = 0;
  mep_acc_info.int_pri = 3 + (1<<2);
  mep_acc_info.sampling_ratio = sampling_ratio;
  if (device_type >= device_type_arad_plus) {
      /* Take RDI only from scanner*/
      mep_acc_info.flags2 =  BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE;

      if (use_port_interface_status) {
          mep_acc_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
          mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
      }
  }

  mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

  if (device_type < device_type_arad_plus) {
      src_mac_mep_2[5] = port_2; /* In devices older than Arad Plus the LSB of the src mac address must equal the local port. No such restriction in Arad+.*/
  }
  /* The MAC address that the CCM packets are sent with*/
  sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_2, 6);

  /*RX*/
  mep_acc_info.gport = gport2;
  sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_2, 6);
  mep_acc_info.lm_counter_base_id = lm_counter_base_id_2;

  if (encoding) {
      printf("Encoding\n"); 
      mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
      mep_acc_info.id = local_id_system;
      rv = system_endpoint_to_endpoint_id_oam(&mep_acc_info);
      if (rv != BCM_E_NONE){
          printf("Error, system_endpoint_to_endpoint_id_oam\n"); 
          print rv;
          return rv;
      }
  }

  printf("bcm_oam_endpoint_create mep_acc_info\n"); 
  rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  printf("created MEP with id %d\n", mep_acc_info.id);

  /*
  * Adding Remote MEP
  */
  bcm_oam_endpoint_info_t_init(&rmep_info);
  rmep_info.name = 0xff;
  rmep_info.local_id = mep_acc_info.id;
  rmep_info.type = bcmOAMEndpointTypeEthernet;
  rmep_info.ccm_period = 0;
  rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
  rmep_info.loc_clear_threshold = 1;
  rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
  rmep_info.id = mep_acc_info.id;
  if (device_type >= device_type_arad_plus) {
	  rmep_info.flags2 = BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT2_RDI_ON_LOC;
      if (use_port_interface_status) {
          rmep_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
          rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
      }
  }

  if (encoding) {
      rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
      
      rv = system_endpoint_to_endpoint_id_oam(&rmep_info);
      if (rv != BCM_E_NONE){
          printf("Error, system_endpoint_to_endpoint_id_oam\n"); 
          print rv;
          return rv;
      }
  }

  printf("bcm_oam_endpoint_create RMEP\n"); 
  rv = bcm_oam_endpoint_create(unit, &rmep_info);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }
  printf("created RMEP with id %d\n", rmep_info.id);


  /*
  * Adding acc MEP - downmep
  */

  bcm_oam_endpoint_info_t_init(&mep_acc_info);

  /*TX*/
  mep_acc_info.type = bcmOAMEndpointTypeEthernet;
  mep_acc_info.group = group_info->id;
  mep_acc_info.level = md_level_3;
  BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_1);
  mep_acc_info.name = 456;
  mep_acc_info.ccm_period = 100;
  mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

  mep_acc_info.vlan = 10;
  mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
  mep_acc_info.outer_tpid = 0x8100;
  mep_acc_info.inner_vlan = 0;
  mep_acc_info.inner_pkt_pri = 0;
  mep_acc_info.inner_tpid = 0;
  mep_acc_info.int_pri = 1 + (3<<2);


  if (device_type >= device_type_arad_plus) {
      /* Take RDI only from RX*/
      mep_acc_info.flags2 = BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE ;

      if (use_port_interface_status) {
          mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
          mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
      }
  }

  if (device_type == device_type_arad_a0) {
      /* Arad A0 bug.*/
      src_mac_mep_3[5] = port_1;
  }

  /* The MAC address that the CCM packets are sent with*/
  sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_3, 6);

  /*RX*/
  mep_acc_info.gport = gport1;
  sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_3, 6);
  mep_acc_info.lm_counter_base_id = lm_counter_base_id_1;
  mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

  printf("bcm_oam_endpoint_create mep_acc_info\n");
  rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  printf("created MEP with id %d\n", mep_acc_info.id);

  /*
  * Adding Remote MEP
  */
  bcm_oam_endpoint_info_t_init(&rmep_info);
  rmep_info.name = 0x11;
  rmep_info.local_id = mep_acc_info.id;
  rmep_info.type = bcmOAMEndpointTypeEthernet;
  rmep_info.ccm_period = 0;
  rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
  rmep_info.loc_clear_threshold = 1;
  rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
  rmep_info.id = mep_acc_info.id;
  if (device_type >= device_type_arad_plus) {
	  rmep_info.flags2 = BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT2_RDI_ON_LOC;
      if (use_port_interface_status) {
          rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
          rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
      }
  }

  printf("bcm_oam_endpoint_create RMEP\n");
  rv = bcm_oam_endpoint_create(unit, &rmep_info);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }
  printf("created RMEP with id %d\n", rmep_info.id);

  if (use_port_interface_status) {
      /*
      * Adding acc MEP - downmep
      */

      bcm_oam_endpoint_info_t_init(&mep_acc_info);

      /*TX*/
      mep_acc_info.type = bcmOAMEndpointTypeEthernet;
      mep_acc_info.group = group_info->id;
      mep_acc_info.level = md_level_4;
      BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_1);
      mep_acc_info.name = 789;
      mep_acc_info.ccm_period = 100;
      mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

      mep_acc_info.vlan = 10;
      mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
      mep_acc_info.outer_tpid = 0x8100;
      mep_acc_info.inner_vlan = 0;
      mep_acc_info.inner_pkt_pri = 0;
      mep_acc_info.inner_tpid = 0;
      mep_acc_info.int_pri = 1 + (3<<2);
      mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

      if (device_type >= device_type_arad_plus) {
          /* Take RDI only from RX*/
          mep_acc_info.flags2 = BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE ;

          if (use_port_interface_status) {
              mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
              mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
              mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
          }
      }

      /* The MAC address that the CCM packets are sent with*/
      sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_4, 6);

      /*RX*/
      mep_acc_info.gport = gport2;
      sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_4, 6);
      mep_acc_info.lm_counter_base_id = lm_counter_base_id_2;

      printf("bcm_oam_endpoint_create mep_acc_info\n");
      rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }

      printf("created MEP with id %d\n", mep_acc_info.id);

      /*
      * Adding Remote MEP
      */
      bcm_oam_endpoint_info_t_init(&rmep_info);
      rmep_info.name = 0x12;
      rmep_info.local_id = mep_acc_info.id;
      rmep_info.type = bcmOAMEndpointTypeEthernet;
      rmep_info.ccm_period = 0;
      rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
      rmep_info.loc_clear_threshold = 1;
      rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
      rmep_info.id = mep_acc_info.id;
      if (device_type >= device_type_arad_plus) {
          rmep_info.flags2 = BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT2_RDI_ON_LOC;
          if (use_port_interface_status) {
              rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
              rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
              rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
          }
      }

      printf("bcm_oam_endpoint_create RMEP\n");
      rv = bcm_oam_endpoint_create(unit, &rmep_info);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }
      printf("created RMEP with id %d\n", rmep_info.id);
  }

  return rv;
}






/*****************************************************************************/
/*                                        OAM  MIP BASIC EXAMPLE                                                                        */
/*****************************************************************************/


int create_mip(int unit, int md_level_mip, bcm_oam_group_t group, bcm_gport_t gport, bcm_mac_t dst_mac) {
  bcm_error_t rv;
  /*
  * Adding a MIP
  */

  bcm_oam_endpoint_info_t_init(&mip_info);
  mip_info.type = bcmOAMEndpointTypeEthernet;
  mip_info.group = group;
  mip_info.level = md_level_mip;
  mip_info.gport = gport;
  mip_info.name = 0;     
  mip_info.ccm_period = 0;
  mip_info.flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;
  sal_memcpy(mip_info.dst_mac_address, dst_mac, 6);

  printf("bcm_oam_endpoint_create mip_info\n"); 
  rv = bcm_oam_endpoint_create(unit, &mip_info);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }
  printf("created MEP with id %d\n", mip_info.id);

  return rv;
}

/**
 * Create OAM group and MIP
 * 
 * 
 * @param unit 
 * 
 * @return int 
 */
int oam_create_mip_with_defaults (int unit) {
  int rv;

  printf("Creating group\n");
  bcm_oam_group_info_t_init(&group_info_short_ma);
  sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
  rv = bcm_oam_group_create(unit, &group_info_short_ma);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  rv =  create_mip(unit, md_level_mip, group_info_short_ma.id, gport2, mac_mep_2);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }
   return rv;
}



/**
 * Equivalent to oam_run_with_defaults, but only creates a MIP.
 * MIP will be created on port2.
 * 
 * @param unit 
 * @param port1 
 * @param port2 
 * @param port3 
 * 
 * @return int 
 */
int oam_mip_only_run_with_defaults (int unit, int port1, int port2, int port3) {
  bcm_error_t rv;

  single_vlan_tag = 1;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;
  
  rv = oam_initialize_mip_settings(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  rv = oam_create_mip_with_defaults(unit);
   if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }


   return 0;
}


/**
 * Initialize settings and global variables for creating a MIP.
 * 
 * @param unit 
 * 
 * @return int 
 */
int oam_initialize_mip_settings(int unit) {
  bcm_error_t rv;

  int md_level_1 = 4;
  int md_level_2 = 5;
  int md_level_3 = 2;
  int lm_counter_base_id_1  = 5;
  int lm_counter_base_id_2  = 6;

  rv = create_vswitch_and_mac_entries(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }



  return rv;
}



/*****************************************************************************/
/*                                        OAM API WRAPPER FUNCTIONS AND EXAMPLES                                         */
/*****************************************************************************/


/*
 * default_ep_example
 *
 * This example uses the OAM default profile to create an upmep with MDL=5
 * and a downmep with MDL=3 for ports with OAM trap profile = 1.
 * If the inlif profiles are in simple mode (indicated by advanced_mode=0),
 * then port1's inlif profile bits allocated to the OAM trap profile are modified to 0x1
 * In advanced mode, a mapping is done between inlif-profile=5 -> oam-trap-profile=1 and
 * port1's inlif profile is set to 5.
 */
int default_ep_example(int unit, int port1, int port2, int port3, int advanced_mode) {
  bcm_error_t rv;
  bcm_oam_endpoint_info_t default_info;

  single_vlan_tag = 1;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;

  printf("create_vswitch_and_mac_entries\n");
  rv = create_vswitch_and_mac_entries(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  printf("Add default up mep (EGRESS0)\n");
  bcm_oam_endpoint_info_t_init(&default_info);
  default_info.id = BCM_OAM_ENDPOINT_DEFAULT_EGRESS0;
  default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID|BCM_OAM_ENDPOINT_UP_FACING;
  default_info.level = 5;
  default_info.timestamp_format = get_oam_timestamp_format(unit);


  rv = bcm_oam_endpoint_create(unit, &default_info);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  if (advanced_mode) {
      printf("Set LIF profile for port: %d\n", port1);
      rv = bcm_port_class_set(unit, gport1, bcmPortClassFieldIngress, 5 /* InLIF profile.*/);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }
      printf("Set mapping from inlif profile 5 to OAM trap profile 1\n", port1);
  }
  else {
      printf("Set LIF profile for port: %d\n", port1);
  }
  rv = bcm_port_control_set(unit, gport1, bcmPortControlOamDefaultProfile, 1/*OAM trap profile */ );
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  printf("Add default down mep (INGRESS1)\n");
  bcm_oam_endpoint_info_t_init(&default_info);
  default_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS1;
  default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
  default_info.level = 3;
  default_info.timestamp_format = get_oam_timestamp_format(unit);

  rv = bcm_oam_endpoint_create(unit, &default_info);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  return rv;
}



/**
 * Function creates an accelerated endpoint.
 *  group_info_short_ma  must be initialized before this
 *  function is called.
 *  Likewise the function create_vswitch_and_mac_entries() must
 *  also  be called before this function.
 *  
 * @author sinai (26/11/2013)
 * 
 * @param unit 
 * @param is_up - direction of the endpoint.
 * @param ts_format - one of bcmOAMTimestampFormatNTP or 
 *                  bcmOAMTimestampFormatIEEE1588v1
 * 
 * @return int 
 */
int create_acc_endpoint(int unit , int is_up, bcm_oam_timestamp_format_t ts_format) {
    bcm_oam_endpoint_info_t  acc_endpoint;
    int counter_base_id;
    int rv;

    /** Down*/
   bcm_oam_endpoint_info_t_init(&acc_endpoint);

  /*TX*/
  acc_endpoint.type = bcmOAMEndpointTypeEthernet;
  acc_endpoint.group = group_info_short_ma.id;
  acc_endpoint.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
  acc_endpoint.outer_tpid = 0x8100;
  acc_endpoint.timestamp_format = ts_format;


  rv = set_counter_source_and_engines(unit,&counter_base_id);
  BCM_IF_ERROR_RETURN(rv); 
  acc_endpoint.lm_counter_base_id = counter_base_id;



  if (is_up) {
        /*TX*/
      acc_endpoint.level = 5;
      acc_endpoint.name = 123;     
      acc_endpoint.flags |= BCM_OAM_ENDPOINT_UP_FACING;

      acc_endpoint.vlan = 5;
      acc_endpoint.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
      acc_endpoint.outer_tpid = 0x8100;
      acc_endpoint.int_pri = 3 + (1<<2);
      /* The MAC address that the CCM packets are sent with*/
      src_mac_mep_2[5] = port_2;
      sal_memcpy(acc_endpoint.src_mac_address, src_mac_mep_2, 6);
      /*RX*/
      acc_endpoint.gport = gport2;
      sal_memcpy(acc_endpoint.dst_mac_address, mac_mep_2, 6);
      acc_endpoint.tx_gport = BCM_GPORT_INVALID;
  } else { /** Down*/
      acc_endpoint.level = 2;
      BCM_GPORT_SYSTEM_PORT_ID_SET(acc_endpoint.tx_gport, port_1);
      acc_endpoint.name = 456;     
      acc_endpoint.ccm_period = 100;
      acc_endpoint.vlan = 10;
      acc_endpoint.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
      acc_endpoint.int_pri = 1 + (3<<2);
      /* The MAC address that the CCM packets are sent with*/
      sal_memcpy(acc_endpoint.src_mac_address, src_mac_mep_3, 6);

      /*RX*/
      acc_endpoint.gport = gport1;
      sal_memcpy(acc_endpoint.dst_mac_address, mac_mep_3, 6);
  }

  return  bcm_oam_endpoint_create(unit, &acc_endpoint);
}




/**
 * OAM_endpoint_action_set calling sequence example.
 * 
 * @param unit 
 * @param port - Must be BCM_GPORT_INVALID for actions requiring
 *             invalid gport.
 * @param endpoint_id 
 * @param action_type 
 * @param opcode - OAM opcode upon which action will be applied
 * 
 * @return int 
 */
int oam_action_set(int unit, int port, int endpoint_id, bcm_oam_action_type_t action_type, int opcode) {
  bcm_error_t rv;
  bcm_oam_endpoint_action_t action;
  int rx_trap;

  BCM_OAM_ACTION_CLEAR_ALL(action);
  BCM_OAM_OPCODE_CLEAR_ALL(action);
  action.flags = 0;
  if (port == BCM_GPORT_INVALID) {
       action.destination = BCM_GPORT_INVALID;
  } else {
      BCM_GPORT_LOCAL_SET(action.destination, port);
  }

  BCM_OAM_OPCODE_SET(action, opcode); 
  BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable);
  BCM_OAM_ACTION_SET(action, action_type );


  rv = bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
  rx_trap = BCM_GPORT_TRAP_GET_ID(action.rx_trap);
  printf( "rx_trap = %d\n",rx_trap);
  return rv;
}



/* This is an example of using bcm_oam_event_register api.
 * A simple callback is created for CCM timeout event. 
 * After a mep and rmep are created, the callback is called 
 * whenever CCMTimeout event is generated. 
 */
int cb_oam(
    int unit,
    uint32 flags,
    bcm_oam_event_type_t event_type,
    bcm_oam_group_t group,
    bcm_oam_endpoint_t endpoint,
    void *user_data)
    {
		print unit;
		print flags;
		print event_type;
		print group;
		print endpoint;

        if ((event_type == bcmOAMEventEndpointInterfaceDown)
            || (event_type == bcmOAMEventEndpointPortDown)) {
            ++port_interface_event_count;
        }
        else {
            timeout_events_count++;
        }

        if (flags & BCM_OAM_EVENT_FLAGS_MULTIPLE) {
            timeout_events_count_multi_oam[endpoint & 0xff]++ ;
        }

        if (event_type == bcmOAMEventEndpointRemote || event_type == bcmOAMEventEndpointRemoteUp) {
            ++remote_event_count;
        }


		return BCM_E_NONE;
    }

int register_events(int unit) {
  bcm_error_t rv;
  bcm_oam_event_types_t timeout_event, timein_event, port_interface_event;

  BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
  rv = bcm_oam_event_register(0, timeout_event, cb_oam, (void*)1);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);
  rv = bcm_oam_event_register(0, timein_event, cb_oam, (void*)2);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemote);
  rv = bcm_oam_event_register(0, timein_event, cb_oam, (void*)3);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemoteUp);
  rv = bcm_oam_event_register(0, timein_event, cb_oam, (void*)4);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointPortDown);
  rv = bcm_oam_event_register(0, port_interface_event, cb_oam, (void*)5);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointInterfaceDown);
  rv = bcm_oam_event_register(0, port_interface_event, cb_oam, (void*)6);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  return rv;
}



/**
 * Snoop OAM packets by endpoint and opcode to given 
 * destination. 
 * Note: Only applies for enbpoints. For MIPs two traps with two 
 * consecutive trap codes must be allocated, one for the ingress 
 * and another for the egress. 
 * 
 * 
 * @param unit 
 * @param dest_snoop_port 
 * @param endpoint_id 
 * @param action_type 
 * @param opcode 
 * 
 * @return int 
 */
int oam_change_mep_destination_to_snoop(int unit, int dest_snoop_port, int endpoint_id, bcm_oam_action_type_t action_type, int opcode) {
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config_snoop;
    bcm_rx_snoop_config_t snoop_config_cpu;
    int snoop_cmnd;
    int trap_code;
    bcm_gport_t gport;

    rv = bcm_rx_snoop_create(unit, 0, &snoop_cmnd);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    bcm_rx_snoop_config_t_init(&snoop_config_cpu);
    snoop_config_cpu.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO);
    snoop_config_cpu.dest_port = dest_snoop_port;
    snoop_config_cpu.size = -1;
    snoop_config_cpu.probability = 100000;

    rv =  bcm_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    rv =  bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }
    printf("Trap created trap_code=%d \n",trap_code);

    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = 0;
    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = snoop_cmnd;

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_snoop);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    BCM_GPORT_TRAP_SET(gport, trap_code, 7, 3); /*Taken from default values*/

    bcm_oam_endpoint_action_t action;
    BCM_OAM_ACTION_CLEAR_ALL(action);
    BCM_OAM_OPCODE_CLEAR_ALL(action);
    action.flags = 0;
    action.destination = gport;
    BCM_OAM_OPCODE_SET(action, opcode); /*1-CCM*/
    BCM_OAM_ACTION_SET(action, action_type);  /*bcmOAMActionMcFwd,bcmOAMActionUcFwd*/
    rv =  bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
   }

   return rv;

}

/**
 * Count SLM packets for a given endpoint.
 * 
 * 
 * @param unit 
 * @param endpoint_id 
 * 
 * @return int 
 */
int slm_set(int unit, int endpoint_id)  {
    bcm_error_t rv;
    bcm_oam_endpoint_action_t action;

    BCM_OAM_ACTION_CLEAR_ALL(action);
    BCM_OAM_OPCODE_CLEAR_ALL(action);
    action.flags = 0;
    action.destination = BCM_GPORT_INVALID;
    BCM_OAM_OPCODE_SET(action, 55); /*SLM*/
    BCM_OAM_ACTION_SET(action, bcmOAMActionSLMEnable);
    BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable);
    rv =  bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
   }

   return rv;
}




int snoop_up_mip = 2;
int snoop_down_mip = 1;


/**
 * Function changes the MIP snoop destination for both 
 * directions (up and down). 
 * Function assumes the soc property "egress_snooping_advanced" 
 * is set to 1. 
 * 
 * @author sinai (22/01/2014)
 * 
 * @param unit 
 * @param dest_snoop_port - destination for both directions.
 * @param mip_endpoint_id 
 * @param action_type 
 * @param opcode 
 * 
 * @return int 
 */
int mip_egress_snooping_advanced(int unit, int dest_snoop_port, int mip_endpoint_id, bcm_oam_action_type_t action_type, int opcode) {
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config_snoop;
    bcm_rx_snoop_config_t snoop_config_cpu;
    int snoop_cmnd = snoop_down_mip;
    int trap_code;
    bcm_gport_t gport;


    bcm_rx_snoop_config_t_init(&snoop_config_cpu);
    snoop_config_cpu.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO);
    snoop_config_cpu.dest_port = dest_snoop_port;
    snoop_config_cpu.size = -1;
    snoop_config_cpu.probability = 100000;

    rv =  bcm_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* create only the trap for the down direction*/
    rv =  bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Trap created trap_code=%d \n", trap_code); 



    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = 0;
    trap_config_snoop.trap_strength = 0; 
    trap_config_snoop.snoop_cmnd = snoop_cmnd;

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_snoop);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }



    BCM_GPORT_TRAP_SET(gport, trap_code, 7, 3); /*Taken from default values*/

    bcm_oam_endpoint_action_t action;
    BCM_OAM_ACTION_CLEAR_ALL(action);
    BCM_OAM_OPCODE_CLEAR_ALL(action);
    action.flags = 0;
    action.destination = gport;
    BCM_OAM_OPCODE_SET(action, opcode); 
    BCM_OAM_ACTION_SET(action, action_type);  /*bcmOAMActionMcFwd,bcmOAMActionUcFwd*/
    rv =  bcm_oam_endpoint_action_set(unit, mip_endpoint_id, &action);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
   }

   return rv;
}

/**
 * enable protection packets in addition to interrupts. Global
 * setting for all OAM. Function assumes OAM has been 
 * inititalized. 
 *  
 * @author sinai (13/02/2014)
 * 
 * @param unit 
 * @param dest_port
 * 
 * @return int 
 */
int enable_oam_protection_packets(int unit, int dest_port) {
	bcm_rx_trap_config_t trap_config_protection;
	int trap_code=0x401; /* trap code on FHEI  will be (trap_code & 0xff), in this case 1.*/
	/* valid trap codes for oamp traps are 0x401 - 0x4ff */
	int rv;

	rv=  bcm_rx_trap_type_create(0, BCM_RX_TRAP_WITH_ID, bcmRxTrapOampProtection, &trap_code);
    if (rv != BCM_E_NONE) {
       printf("trap create: (%s) \n",bcm_errmsg(rv));
       return rv;
   }

	trap_config_protection.dest_port = dest_port;
	rv = bcm_rx_trap_set(0, trap_code, trap_config_protection); 
    if (rv != BCM_E_NONE) {
       printf("trap set: (%s) \n",bcm_errmsg(rv));
       return rv;
   }

	return rv;
}

/**
 * sets trap for OAM MAID error to specific port
 *  
 * @author aviv (14/12/2014)
 * 
 * @param unit 
 * @param dest_port - should be local port.
 * 
 * @return int 
 */
int maid_trap_set(int unit, int dest_port) {
	bcm_rx_trap_config_t trap_config;
	int trap_code=0x416; /* trap code on FHEI  will be (trap_code & 0xff), in this case 1.*/
	/* valid trap codes for oamp traps are 0x401 - 0x4ff */
	int rv;

	rv=  bcm_rx_trap_type_create(0, BCM_RX_TRAP_WITH_ID, bcmRxTrapOampMaidErr, &trap_code);
    if (rv != BCM_E_NONE) {
       printf("trap create: (%s) \n",bcm_errmsg(rv));
       return rv;
   }

    BCM_GPORT_SYSTEM_PORT_ID_SET(trap_config.dest_port,dest_port );
	rv = bcm_rx_trap_set(0, trap_code, trap_config); 
    if (rv != BCM_E_NONE) {
       printf("trap set: (%s) \n",bcm_errmsg(rv));
       return rv;
   }

	return rv;
}

/**
 * Create an accelerated down MEP with ID 4095 and level 4
 * The MEP belongs to a long MA group which makes its CCMs 
 * include a long MEG id. vlan = 10 
 *  
 * @author Aviv (20/05/2014)
 * 
 * @param unit 
 * @param port - physical port to be used 
 * 
 * @return int 
 */
int oam_long_ma_example(int unit, int port1, int port2, int port3) {
  bcm_error_t rv;
  int counter_base_id;

  single_vlan_tag = 1;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;

  rv = create_vswitch_and_mac_entries(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  printf("Creating group (long name format)\n");
  bcm_oam_group_info_t_init(&group_info_long_ma);
  sal_memcpy(group_info_long_ma.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
  rv = bcm_oam_group_create(unit, &group_info_long_ma);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  rv = set_counter_source_and_engines(unit,&counter_base_id);
  BCM_IF_ERROR_RETURN(rv); 


  /* Adding acc MEP - downmep */
  bcm_oam_endpoint_info_t_init(&mep_acc_info);
  mep_acc_info.id = 4095;
  /*TX*/
  mep_acc_info.type = bcmOAMEndpointTypeEthernet;
  mep_acc_info.group = group_info_long_ma.id;
  mep_acc_info.level = 4;
  BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_1);
  mep_acc_info.name = 456;
  mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
  mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
  mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
  mep_acc_info.vlan = 10;
  mep_acc_info.pkt_pri = 0 +(2<<1);  /*dei(1bit) + (pcp(3bit) << 1) */
  mep_acc_info.outer_tpid = 0x8100;
  mep_acc_info.inner_vlan = 20;
  mep_acc_info.inner_pkt_pri = 0;     
  mep_acc_info.inner_tpid = 0x9100;
  mep_acc_info.int_pri = 3 +(1<<2);
  mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);


  rv = get_device_type(unit, &device_type);
  if (rv < 0) {
      printf("Error checking whether the device is arad+.\n");
      print rv;
      return rv;
  }
  if (device_type >= device_type_arad_plus) {
      /* Take RDI only from RX*/
      mep_acc_info.flags2 = BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE;
  }

  /* The MAC address that the CCM packets are sent with*/
  src_mac_mep_3[5] = port_1;
  sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_3, 6);

  /*RX*/
  mep_acc_info.gport = gport1;
  sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_3, 6);
  mep_acc_info.lm_counter_base_id = counter_base_id;

  printf("bcm_oam_endpoint_create mep_acc_info\n"); 
  rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  printf("created MEP with id %d\n", mep_acc_info.id);

  /* Adding Remote MEP */
  bcm_oam_endpoint_info_t_init(&rmep_info);
  rmep_info.name = 460;
  rmep_info.local_id = mep_acc_info.id;
  rmep_info.type = bcmOAMEndpointTypeEthernet;
  rmep_info.ccm_period = 0;
  rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
  rmep_info.loc_clear_threshold = 1;
  rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
  rmep_info.id = mep_acc_info.id; 
  if (device_type >= device_type_arad_plus) {
	  rmep_info.flags2 = BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT2_RDI_ON_LOC;
  }

  printf("bcm_oam_endpoint_create RMEP\n"); 
  rv = bcm_oam_endpoint_create(unit, &rmep_info);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }
  printf("created RMEP with id %d\n", rmep_info.id);

  rv = register_events(unit);
  return rv;
}

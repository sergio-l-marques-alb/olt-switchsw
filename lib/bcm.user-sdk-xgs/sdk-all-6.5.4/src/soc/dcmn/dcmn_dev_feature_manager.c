/*
 * $Id:$
 *
 * $Copyright: Copyright 2016 Broadcom Corporation.
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


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_INTR



#include <shared/bsl.h>
#include <shared/util.h>

#include <soc/error.h>
#include <soc/drv.h>
#include <soc/dcmn/error.h>

/* 
 *  include  
 */ 



#include <soc/dcmn/dcmn_dev_feature_manager.h>



STATIC soc_error_t dcmn_keywords_cb (int unit,void  *value,int nof_ranges,void *data)
{
    char **keywords = (char **)data;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

/*    v = _shr_ctoi((char *)value); */
    if (value==NULL ||  !sal_strlen(value)) {
        SOC_EXIT;
    }
    for (i = 0; keywords[i]; i++) {
        if (!sal_strcmp(value,keywords[i]))
        {
            SOC_EXIT;
        }
    }
    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);

exit:
    SOCDNX_FUNC_RETURN;

}


STATIC soc_error_t dcmn_range_cb (int unit,void  *value,int nof_ranges,void *data)
{
    uint32 v;
    int i;
    dcmn_range_t *ranges = (dcmn_range_t *) data;


    SOCDNX_INIT_FUNC_DEFS;

/*    v = _shr_ctoi((char *)value); */

    v = *(uint32 *)value;
    for (i=0;i<nof_ranges;i++) {
        if (ranges[i].from <=v && v<=ranges[i].to) {
            SOC_EXIT;
        }
    }
    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);

exit:
    SOCDNX_FUNC_RETURN;

}



STATIC soc_error_t dcmn_otn_enable_cb (int unit,void  *value,int nof_ranges,void *data)
{
    uint16 dev_id; 
    uint8 rev_id;

    SOCDNX_INIT_FUNC_DEFS;
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id==BCM88675_DEVICE_ID  || dev_id==BCM88375_DEVICE_ID) {
        uint32 val;
        SOCDNX_IF_ERR_EXIT(READ_ECI_OGER_1004r_REG32(unit,&val));
        /* Register ECI_OGER_1004 - Bit  9
        * If the bit is "1" it means that OTN is disabled in the device.
        */
        if (!(val && 1<<8)) {
            SOC_EXIT;
        }
        SOC_EXIT;
    }


    SOCDNX_IF_ERR_EXIT(dcmn_keywords_cb(unit,value,nof_ranges,data));

exit:
    SOCDNX_FUNC_RETURN;

}



dcmn_range_t range_0_0[] = {{0,0}};
dcmn_range_t range_0_720000[] = {{0,720000}};
char *none_keywords[]={"0","none",0};
char *fabric_mesh_keywords[]={"SINGLE_FAP",0};

dcmn_feature_cb_t  dc_feature_range[] = {
    {
        DCMN_DC_FEATURE,
        dcmn_range_cb,  /*  cb function*/
        sizeof(range_0_0)/sizeof(dcmn_range_t), /*num of ranges*/
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  dc_feature_none[] = {
    {
        DCMN_DC_FEATURE,
        dcmn_keywords_cb,  /*  cb function*/
        0, /*not relevant*/
        none_keywords
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  metro_feature_range[] = {
    {
        DCMN_METRO_FEATURE,
        dcmn_range_cb,  /*  cb function*/
        sizeof(range_0_0)/sizeof(dcmn_range_t), /*num of ranges*/
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  otn_feature_none[] = {
    {
        DCMN_OTN_FEATURE,
        dcmn_otn_enable_cb,  /*  cb function*/
        0, /*not relevant*/
        none_keywords
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  fabric_ilkn_feature_range[] = {
    {
        DCMN_FABRIC_ILKN_FEATURE,
        dcmn_range_cb,  /*  cb function*/
        sizeof(range_0_0)/sizeof(dcmn_range_t), /*num of ranges*/
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  fabric_mesh_feature_kwywords[] = {
    {
        DCMN_FABRIC_MESH_FEATURE,
        dcmn_keywords_cb,  /*  cb function*/
        0, /*not relevant*/
        fabric_mesh_keywords
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  ext_ram_feature_range[] = {
    {
        DCMN_EXT_RAM_FEATURE,
        dcmn_range_cb,  /*  cb function*/
        sizeof(range_0_0)/sizeof(dcmn_range_t), /*num of ranges*/
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  core_freq_feature_range[] = {
    {
        DCMN_CORE_FREQ_FEATURE,
        dcmn_range_cb,  /*  cb function*/
        sizeof(range_0_720000)/sizeof(dcmn_range_t), /*num of ranges*/
        range_0_720000
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
/* feature propert map we connect to each property list of features that the property should blocked if feature disabled*/
dcmn_property_features_t  properies_feature_map[] = {
                                                        {
                                                            spn_TRILL_MODE,
                                                            dc_feature_range
                                                        },
                                                        {
                                                            spn_BCM886XX_VXLAN_ENABLE,
                                                            dc_feature_range
                                                        },
                                                        {
                                                            spn_BCM886XX_L2GRE_ENABLE ,
                                                            dc_feature_range
                                                        },
                                                        {
                                                            spn_BCM886XX_FCOE_SWITCH_MODE  ,
                                                            dc_feature_range
                                                        },
                                                        {
                                                            spn_NUM_OAMP_PORTS ,
                                                            metro_feature_range
                                                        },
                                                        {
                                                            spn_FAP_TDM_BYPASS  ,
                                                            otn_feature_none
                                                        },
                                                        {
                                                            spn_FABRIC_CONNECT_MODE  ,
                                                            fabric_mesh_feature_kwywords
                                                        },
                                                        {
                                                            spn_USE_FABRIC_LINKS_FOR_ILKN_NIF  ,
                                                            fabric_ilkn_feature_range
                                                        },
                                                        {
                                                             spn_EXT_RAM_PRESENT  ,
                                                             ext_ram_feature_range
                                                        },
                                                        {
                                                             spn_CORE_CLOCK_SPEED  ,
                                                             core_freq_feature_range
                                                        }

                                                    };



/*    feature device map. for each feature we connect list of devices for them the feature disabled*/
int metro_devices[] = 
    {
         BCM88674_DEVICE_ID,
         BCM88674_DEVICE_ID,
         BCM88676_DEVICE_ID,
         BCM88676M_DEVICE_ID,
         BCM88683_DEVICE_ID,
         BCM88376_DEVICE_ID,
         BCM88376M_DEVICE_ID,
         BCM88385_DEVICE_ID,
         INVALID_DEV_ID
    }; /* DCMN_METRO_FEATURE*/
int dc_devices[] = 
    {
        BCM88671_DEVICE_ID,
        BCM88671M_DEVICE_ID,
        BCM88673_DEVICE_ID,
        BCM88674_DEVICE_ID,
        BCM88678_DEVICE_ID,
        BCM88683_DEVICE_ID,
        BCM88371_DEVICE_ID,
        BCM88371M_DEVICE_ID,
        BCM88377_DEVICE_ID,
        BCM88378_DEVICE_ID,
        BCM88382_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_DC_FEATURE*/

int otn_devices[] = 
    {
        BCM88671_DEVICE_ID,
        BCM88671M_DEVICE_ID,
        BCM88675_DEVICE_ID,
        BCM88675M_DEVICE_ID,
        BCM88676_DEVICE_ID,
        BCM88676M_DEVICE_ID,
        BCM88370_DEVICE_ID,
        BCM88371_DEVICE_ID,
        BCM88371M_DEVICE_ID,
        BCM88375_DEVICE_ID,
        BCM88376_DEVICE_ID,
        BCM88376M_DEVICE_ID,
        BCM88385_DEVICE_ID,
        BCM88686_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_OTN_FEATURE*/

int nif_24_44_devices[] =
    {
        BCM88670_DEVICE_ID,
        BCM88673_DEVICE_ID,
        BCM88674_DEVICE_ID,
        BCM88377_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_NIF_24_44_FEATURE*/

int nif_24_48_devices[] =
    {
        BCM88686_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_NIF_24_48_FEATURE*/

int nif_44_16_devices[] =
    {
        BCM88682_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_NIF_44_16_FEATURE*/

int nif_48_4_devices[] =
    {
        BCM88683_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_NIF_48_4_FEATURE*/

int nif_40_4_devices[] =
    {
        BCM88385_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_NIF_40_4_FEATURE*/

int ilkn_4_ports_devices[] =
    {
        BCM88670_DEVICE_ID,
        BCM88672_DEVICE_ID,
        BCM88673_DEVICE_ID,
        BCM88674_DEVICE_ID,
        BCM88377_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_ILKN_4_PORTS_FEATURE*/

int ilkn_2_ports_devices[] =
    {
        BCM88385_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_2_ILKN_PORTS_FEATURE*/

int fabric_ilkn_devices[] = 
    {
        BCM88670_DEVICE_ID,
        BCM88672_DEVICE_ID,
        BCM88673_DEVICE_ID,
        BCM88674_DEVICE_ID,
        BCM88378_DEVICE_ID,
       INVALID_DEV_ID
    }; /* DCMN_FABRIC_ILKN_FEATURE*/

int fabric_mesh_devices[] = 
    {
        BCM88671M_DEVICE_ID,
        BCM88676M_DEVICE_ID,
        BCM88370_DEVICE_ID,
        BCM88371_DEVICE_ID,
        BCM88376_DEVICE_ID,
        BCM88378_DEVICE_ID,
        BCM88381_DEVICE_ID,
        INVALID_DEV_ID
    }; /* DCMN_FABRIC_MESH_FEATURE*/

int ext_ram_devices[] =
    {
         BCM88683_DEVICE_ID,
         INVALID_DEV_ID
    }; /* DCMN_EXT_RAM_FEATURE*/

int core_freq_devices[] =
    {
         BCM88682_DEVICE_ID,
         BCM88683_DEVICE_ID,
         BCM88381_DEVICE_ID,
         BCM88382_DEVICE_ID,
         BCM88385_DEVICE_ID,
         BCM88686_DEVICE_ID,
         INVALID_DEV_ID
    }; /* DCMN_CORE_FREQ_FEATURE*/

int lpm_extended_devices[] =
    {
         BCM88682_DEVICE_ID,
         BCM88683_DEVICE_ID,
         BCM88382_DEVICE_ID,
         BCM88385_DEVICE_ID,
         INVALID_DEV_ID
    }; /* DCMN_LPM_EXTENDED_FEATURE*/

/*This array must aligned with the ENUM dcmn_feature_e in file dcmn_dev_feature_manager.h*/
int   *device_features_map[DCMN_NUM_OF_FEATURES] = {
    metro_devices,
    dc_devices,
    otn_devices,
    nif_24_44_devices,
    nif_24_48_devices,
    nif_44_16_devices,
    nif_48_4_devices,
    nif_40_4_devices,
    ilkn_4_ports_devices,
    ilkn_2_ports_devices,
    fabric_ilkn_devices,
    fabric_mesh_devices,
    ext_ram_devices,
    core_freq_devices,
    lpm_extended_devices
    };


soc_error_t dcmn_is_property_value_permited(int unit,const char *property,void *value) {
    int i,j,k;
    int len = sizeof(properies_feature_map)/sizeof(dcmn_property_features_t);
    uint16 dev_id; 
    uint8 rev_id;
    dcmn_feature_cb_t  *features_array;
    dcmn_feature_t feature;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    for (i=0;i<len;i++) {

        /*  continue if not the soc property entry*/
        if (sal_strcmp(property,properies_feature_map[i].property)) {
            continue;
        }
        /*  getting the features entry for soc property*/
        features_array = properies_feature_map[i].features_array;
        for (j=0;features_array[j].feature!=DCMN_INVALID_FEATURE;j++) {
            feature = features_array[j].feature;
            /* scanning device list for feature to see if our device belong */
            for (k=0;device_features_map[feature][k]!=INVALID_DEV_ID && device_features_map[feature][k]!=dev_id;k++) {
            }
            if (device_features_map[feature][k]!=dev_id) {
                continue;
            }
            /* at this point it turn out that our device  is block to some of the soc property values so we need to check*/
            rc = features_array[j].cb(unit,value,features_array[j].nof_ranges,features_array[j].data);
            if (rc!= SOC_E_NONE) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("property:%s  blocked for device %s\n"), property, soc_dev_name(unit)));  
            }

            SOCDNX_IF_ERR_EXIT(rc);


        }
    }

exit:
    SOCDNX_FUNC_RETURN;




}


soc_error_t dcmn_property_get(int unit, const char *name, uint32 defl,uint32 *value)
{
    SOCDNX_INIT_FUNC_DEFS;
    *value = soc_property_get(unit,name, defl);

    SOCDNX_IF_ERR_EXIT(dcmn_is_property_value_permited(unit,name,value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t dcmn_property_suffix_num_get(int unit, int num, const char *name, const char *suffix, uint32 defl,uint32 *value)
{
    SOCDNX_INIT_FUNC_DEFS;
    *value = soc_property_suffix_num_get(unit, num, name, suffix, 0);

    SOCDNX_IF_ERR_EXIT(dcmn_is_property_value_permited(unit,name,value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t dcmn_property_get_str(int unit, const char *name,char **value)
{
    SOCDNX_INIT_FUNC_DEFS;
    *value = soc_property_get_str(unit,name);
    SOCDNX_IF_ERR_EXIT(dcmn_is_property_value_permited(unit,name,*value));

exit:
    SOCDNX_FUNC_RETURN;

}

uint8       dcmn_device_block_for_feature(int unit,dcmn_feature_t feature)
{
    uint32 i;
    uint16 dev_id;
    uint8 rev_id;
    soc_cm_get_id(unit, &dev_id, &rev_id);
    for (i=0;device_features_map[feature][i]!=INVALID_DEV_ID && device_features_map[feature][i]!=dev_id;i++) {
    }

    return device_features_map[feature][i]==dev_id;

}


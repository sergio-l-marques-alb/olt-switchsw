#ifndef _DCMN_DEV_FEATURE_MANAGER_H_
#define _DCMN_DEV_FEATURE_MANAGER_H_

#include <soc/error.h>

#define INVALID_DEV_ID -1
typedef enum dcmn_feature_e {
    DCMN_INVALID_FEATURE=-1,
    DCMN_NO_METRO_FEATURE,
    DCMN_NO_DC_FEATURE,/*Data Center*/
    DCMN_NO_OTN_FEATURE,
    DCMN_JER_NIF_24_44_FEATURE,/*DCMN_<device>_NIF_<num of 25G>_<num of 12.5G>_FEATURE*/
    DCMN_JER_NIF_24_48_FEATURE,/*DCMN_<device>_NIF_<num of 25G>_<num of 12.5G>_FEATURE*/
    DCMN_JER_NIF_44_16_FEATURE,/*DCMN_<device>_NIF_<num of 25G>_<num of 12.5G>_FEATURE*/
    DCMN_JER_NIF_48_4_FEATURE,/*DCMN_<device>_NIF_<num of 25G>_<num of 12.5G>_FEATURE*/
    DCMN_JER_NIF_40_4_FEATURE,/*DCMN_<device>_NIF_<num of 25G>_<num of 12.5G>_FEATURE*/
    DCMN_JER_NIF_12_32_FEATURE,/*DCMN_<device>_NIF_<num of 25G>_<num of 12.5G>_FEATURE*/
    DCMN_QAX_NIF_10_32_FEATURE,/*DCMN_<device>_NIF_<num of 25G>_<num of 12.5G>_FEATURE*/
    DCMN_QAX_NIF_12_24_FEATURE,/*DCMN_<device>_NIF_<num of 25G>_<num of 12.5G>_FEATURE*/
    DCMN_JER_4_ILKN_PORTS_FEATURE,/*Number of ILKN I/F for device*/
    DCMN_JER_2_ILKN_PORTS_FEATURE,/*Number of ILKN I/F for device*/
    DCMN_NO_FABRIC_ILKN_FEATURE,/*Fabric links can't be used for ILKN*/
    DCMN_NO_FABRIC_MESH_FEATURE,/*Device can be used only in Single FAP mode*/
    DCMN_NO_EXT_RAM_FEATURE,
    DCMN_CORE_FREQ_720_FEATURE,/*Maximum core clock frequency is 720MNz*/
    DCMN_CORE_FREQ_325_FEATURE,/*Maximum core clock frequency is 325MNz*/
    DCMN_NO_EXTENDED_LPM_FEATURE,/*No Extended LPM (KAPS)*/
    DCMN_NUM_OF_FEATURES
} dcmn_feature_t;





typedef struct dcmn_range_s {
    int from;
    int to;
} dcmn_range_t;

typedef soc_error_t (*dcmn_propery_value_valid_cb) (int unit, void  *value,int nof_ranges,void *data);

typedef struct dcmn_feature_cb_s
{
    dcmn_feature_t feature;
    dcmn_propery_value_valid_cb cb;
    int nof_ranges;
    void *data;
} dcmn_feature_cb_t;

typedef struct dcmn_property_features_s
{
    char *property;
    dcmn_feature_cb_t  *features_array;
} dcmn_property_features_t;


soc_error_t dcmn_is_property_value_permited(int unit,const char *property,void *value);
soc_error_t dcmn_property_get(int unit, const char *name, uint32 defl,uint32 *value);
soc_error_t dcmn_property_suffix_num_get(int unit, int num, const char *name, const char *suffix, uint32 defl,uint32 *value);
soc_error_t dcmn_property_get_str(int unit, const char *name,char **value);
uint8       dcmn_device_block_for_feature(int unit,dcmn_feature_t feature);






#endif


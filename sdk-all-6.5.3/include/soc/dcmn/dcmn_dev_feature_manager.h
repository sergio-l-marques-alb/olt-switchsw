#ifndef _DCMN_DEV_FEATURE_MANAGER_H_
#define _DCMN_DEV_FEATURE_MANAGER_H_

#include <soc/error.h>

#define INVALID_DEV_ID -1
typedef enum dcmn_feature_e {
    DCMN_INVALID_FEATURE=-1,
    DCMN_METRO_FEATURE,
    DCMN_DC_FEATURE,
    DCMN_OTN_FEATURE,
    DCMN_NIF_SERDES_FEATURE,
    DCMN_ILKN_PORTS_FEATURE,
    DCMN_FABRIC_ILKN_FEATURE,
    DCMN_FABRIC_MESH_FEATURE,
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


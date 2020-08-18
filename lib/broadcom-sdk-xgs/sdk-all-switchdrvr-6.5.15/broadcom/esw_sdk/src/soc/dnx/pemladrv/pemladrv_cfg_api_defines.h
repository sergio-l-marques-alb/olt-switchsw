#define PEM_CFG_API_MAP_CHUNK_N_ENTRIES         256
#define PEM_CFG_API_MAP_CHUNK_WIDTH             32
#define PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES    32
#define PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH        16
#define PEM_CFG_API_CAM_RESULT_CHUNK_N_ENTRIES  32
#define PEM_CFG_API_CAM_RESULT_CHUNK_WIDTH      16
#define PEM_CFG_API_CAM_TCAM_KEY_WIDTH          16
#define UINT_WIDTH_IN_BITS                      (8*sizeof(unsigned int))

#define PEM_CFG_API_NOF_CORES 2

#if defined(PROJECT_J2_A0) || defined(SOC_IS_JERICHO_2_A0) || defined(PROJECT_J2) || defined(SOC_IS_JERICHO_2)
#define PEM_CFG_API_NOF_CORES 2
#endif /*defined(PROJECT_J2_A0) || defined(SOC_IS_JERICHO_2_A0) || defined(PROJECT_J2) || defined(SOC_IS_JERICHO_2)*/
#if defined(PROJECT_J2C) || defined(SOC_IS_J2C)
#define PEM_CFG_API_NOF_CORES 1
#endif /*defined(PROJECT_J2C) || defined(SOC_IS_J2C)*/
  

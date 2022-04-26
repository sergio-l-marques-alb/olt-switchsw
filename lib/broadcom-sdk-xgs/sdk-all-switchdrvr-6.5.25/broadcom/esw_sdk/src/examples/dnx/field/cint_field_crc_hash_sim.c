 /*
 * Configuration:
 *
 * cint ../../../../src/examples/dnx/field/cint_field_crc_hash_sim.c
 *
 * A HW simulator to generating hash processing keys
 *
 * This cint expects to get a hash context configuration: hash_function, hash_action, crc_select
 * and keys values used for calculating the hashing process: lb_key_value, key_a_value, key_b_value
 * and it outputs the action_key expected value after hashing process
 *
 * - hash_function - Hashing polynomial for hashing mechanism, supported values are:
 * bcmFieldContextHashFunctionCrc16Bisync
 * bcmFieldContextHashFunctionCrc16Xor1
 * bcmFieldContextHashFunctionCrc16Xor2
 * bcmFieldContextHashFunctionCrc16Xor4
 * bcmFieldContextHashFunctionCrc16Xor8
 * bcmFieldContextHashFunctionXor16
 * bcmFieldContextHashFunctionCrc16Ccitt
 *
 * - hash_action - Hashing action to perform on key, supported values are:
 * bcmFieldContextHashActionValueReplaceCrc
 * bcmFieldContextHashActionValueAugmentCrc
 * bcmFieldContextHashActionValueAugmentKey
 *
 * - crc_select - The CRC function to be performed on the augmentation output, In case the hashing action selected is one of the augmentation actions, supported values are:
 * bcmFieldCrcSelectCrc16P0x10039
 * bcmFieldCrcSelectCrc16P0x100d7
 * bcmFieldCrcSelectCrc16P0x1015d
 * bcmFieldCrcSelectCrc16P0x10939
 * bcmFieldCrcSelectCrc16P0x109e7
 * bcmFieldCrcSelectCrc16P0x10ac5
 * bcmFieldCrcSelectCrc16P0x1203d
 * bcmFieldCrcSelectCrc16P0x12105
 *
 * - lb_key_value - The value of the requested lb key, that goes into IPMF1, generating by load balancing process
 *
 * - key_a_value, key_b_value - The value of keys I & J, according to the hash context key_info and the sent packet
 * this values are expected as an array of 5 uint32 each, when the values are LSB to MSB
 *
 * Example:
 * For the following input packet:
 * Ethernet
 * IPv6
 *      SIP: 00aa:00bb:00cc:00dd:00ee:00ff:0077:0088
 *      DIP: 1100:2200:3300:4400:5500:6600:7700:8800
 *      Next Header: 0x6
 * TCP
 *      Src_Port: 10000 (0x2710)
 *      Dst_Port: 80 (0x50)
 *
 * and a hash context that configured with the 5-tuple qualifiers: Src-IPv6, L4 Src-port, L4 Dst-port, Dst-IPv6, Next_Header
 * the keys will be set as the following (note the keys values are LSB to MSB):
 *
 * First, each qualifier value should be reversed separately:
 *
 *                          SIP                       Src_Port     Dst_Port
 * Key_a:     0x1100EE00FF007700BB003300DD005500   |   0x08E4   |   0x0A00      =>  0x1100EE00FF007700BB003300DD00550008E40A00
 *
 *                          DIP                     Next Header
 * Key_b:     0x001100EE006600AA002200CC00440088   |   0x6000   |   0x0000      =>  0x001100EE006600AA002200CC0044008860000000
 *
 * Then first chuck of 160 bits (MSBs) should be extracted to key_a and the second chunk to key_b, when key_a[0] is the 32 LSB bits of the first chuck.
 * According to the example:
 *
 * key_a_value[0] = 0x08E40A00;
 * key_a_value[1] = 0xDD005500;
 * key_a_value[2] = 0xBB003300;
 * key_a_value[3] = 0xFF007700;
 * key_a_value[4] = 0x1100EE00;
 *
 * key_b_value[0] = 0x60000000;
 * key_b_value[1] = 0x00440088;
 * key_b_value[2] = 0x002200CC;
 * key_b_value[3] = 0x006600AA;
 * key_b_value[4] = 0x001100EE;
 *
 * main_func:
 *  cint_field_crc_hash_sim_main(unit, hash_function, crc_select, hash_action, lb_key_value, key_a_value, key_b_value, action_key_value);
 *
 */

/*
 * A list of supported hash function polynomials
 */
uint32 crc16_bisync_polynomial =    0x8005;
uint32 crc16_ccit_polynomial =      0x1021;
uint32 crc32_eth_polynomial =       0x04C11DB7;
uint32 crc32_c_polynomial =         0x1EDC6F41;
uint32 crc32_k_polynomial =         0x741B8CD7;

/*
 * A list of supported crc select polynomials
 */
uint32 crc16_10039 =                0x0039;
uint32 crc16_100d7 =                0x00d7;
uint32 crc16_1015d =                0x015d;
uint32 crc16_10939 =                0x0939;
uint32 crc16_109e7 =                0x09e7;
uint32 crc16_10ac5 =                0x0ac5;
uint32 crc16_1203d =                0x203d;
uint32 crc16_12105 =                0x2105;

int CRC_HASH_SIM_UINT32_NOF_BITS                 = 32;
int CRC_HASH_SIM_UINT32_FULL_MASK                = 0xFFFFFFFF;
int CRC_HASH_SIM_INT_NOF_UINT32                  = 10;

bcm_field_context_hash_function_t cint_field_crc_hash_sim_function;
bcm_field_crc_select_t cint_field_crc_hash_sim_crc_select;
bcm_field_context_hash_action_value_t cint_field_crc_hash_sim_hash_action;
uint32 cint_field_crc_hash_sim_augmented_lb_key_value;
/**
 * Some of the devices process the device from LSB to MSB, and some of them from MSB to LSB
 */
int cint_field_crc_hash_sim_lsb_to_msb;

struct cint_field_crc_hash_sim_key_t
{
    int size;                             /* Size of the key in bits. */
    uint32 data[CRC_HASH_SIM_INT_NOF_UINT32];                       /* Set value for key. */
};

/**
* \brief
*   This function runs the external main function
* \param [in] unit                 - device id
* \param [in] hash_function        - hash function for the simulation
* \param [in] hash_action          - hash action for the simulation
* \param [in] crc_select           - crc select for the simulation
* \param [in] lb_key_value         - lb key value that goes into IPMF1
* \param [in] key_a_value          - key I used for the simulation, expected MSB to LSB
* \param [in] key_b_value          - key J used for the simulation, expected MSB to LSB
* \param [out] action_key          - output value of the simulation
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_sim_main(
        int unit,
        bcm_field_context_hash_function_t hash_function,
        bcm_field_context_hash_action_value_t hash_action,
        bcm_field_crc_select_t crc_select,
        uint32 lb_key_value,
        uint32 *key_a_value,
        uint32 *key_b_value,
        uint32* action_key_value)
{
    cint_field_crc_hash_sim_key_t key_a, key_b;
    key_a.size = 160;
    key_b.size = 160;
    int ii;

    cint_field_crc_hash_sim_function = hash_function;
    cint_field_crc_hash_sim_crc_select = crc_select;
    cint_field_crc_hash_sim_hash_action = hash_action;
    cint_field_crc_hash_sim_augmented_lb_key_value = lb_key_value;

    /**
     * This simulator processes the bits from LSB to MSB, therefore index 0 of expected keys a & b values presents the MSB
     */
    for (ii = 0; ii < key_a.size / CRC_HASH_SIM_UINT32_NOF_BITS; ii++)
    {
        key_a.data[ii] = key_a_value[ii];
        key_b.data[ii] = key_b_value[ii];
    }

    cint_field_crc_hash_sim_lsb_to_msb = *(dnxc_data_get(unit, "field", "features", "hashing_process_lsb_to_msb", NULL));

    BCM_IF_ERROR_RETURN(cint_field_crc_hash_sim_hashing(key_a, key_b, action_key_value));

    return BCM_E_NONE;
}

/**
* \brief
*   This function simulates the hashing process
* \param [in] key_a                - first key used for this process
* \param [in] key_b                - second key used for this process
* \param [out] action_key          - action key value, output of the simulation
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_sim_hashing(
        cint_field_crc_hash_sim_key_t key_a,
        cint_field_crc_hash_sim_key_t key_b,
        uint32 *action_key)
{
    uint32 compressed_key_a, compressed_key_b;
    cint_field_crc_hash_sim_key_t crc_input, crc_output, key, additional_lb_key;
    crc_input.size = 320;
    crc_output.size = 16;
    additional_lb_key.size = 16;
    key.size = 64;
    key.data[1] = key_b.data[4];
    key.data[0] = key_b.data[3];

    BCM_IF_ERROR_RETURN(cint_field_crc_hash_compress_and_switch(1, 0, key_a, key_b, &compressed_key_a, &compressed_key_b));

    BCM_IF_ERROR_RETURN(cint_field_crc_hash_stamp_logic(1, 0, key_b, key_a, compressed_key_a, compressed_key_b, &crc_input));

    BCM_IF_ERROR_RETURN(cint_field_crc_hash_perform_crc(crc_input, &crc_output));

    BCM_IF_ERROR_RETURN(cint_field_crc_hash_update_lb_keys(crc_output, key, &additional_lb_key));

    action_key[0] = additional_lb_key.data[0];
    action_key[1] = additional_lb_key.data[1];
    return BCM_E_NONE;
}

/**
* \brief
*   This function compresses the keys
* \param [in] compress_enable      - boolean to compress
* \param [in] switch_enable        - boolean to switch
* \param [in] key_a                - first key to compress
* \param [in] key_b                - second key to compress
* \param [out] compressed_key_a    - first compressed key
* \param [out] compressed_key_b    - second compressed key
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_compress_and_switch(
        int compress_enable,
        int switch_enable,
        cint_field_crc_hash_sim_key_t key_a,
        cint_field_crc_hash_sim_key_t key_b,
        uint32 *compressed_key_a,
        uint32 *compressed_key_b)
{
    if(compress_enable)
    {
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_collapse(key_a, compressed_key_a));
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_collapse(key_b, compressed_key_b));
    }
    else
    {
        *compressed_key_a = cint_field_crc_hash_get_lsb_n_bits(key_a, 0, 32);
        *compressed_key_b = cint_field_crc_hash_get_lsb_n_bits(key_a, 32, 32);
    }
    if (switch_enable)
    {
        cint_field_crc_hash_sim_key_t temp;
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_copy_key(key_a, &temp));
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_copy_key(key_b, &key_a));
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_copy_key(temp, &key_b));
    }
    return BCM_E_NONE;
}

/**
* \brief
*   This function collapses the keys
* \param [in] key                - key to collapse
* \param [out] compressed_key    - collapsed key
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_collapse(cint_field_crc_hash_sim_key_t key, uint32 *compressed_key)
{
    *compressed_key = cint_field_crc_hash_get_lsb_n_bits(key, 96, 32) ^ cint_field_crc_hash_get_lsb_n_bits(key, 64, 32) ^ cint_field_crc_hash_get_lsb_n_bits(key, 32, 32) ^ cint_field_crc_hash_get_lsb_n_bits(key, 0, 32);
    return BCM_E_NONE;
}


/**
* \brief
*   This function generates the hashing crc input
* \param [in] compress_enable      - boolean to compress
* \param [in] switch_enable        - boolean to switch
* \param [in] key_a                - first key to compress
* \param [in] key_b                - second key to compress
* \param [in] compressed_key_a     - first compressed key
* \param [in] compressed_key_b     - second compressed key
* \param [out] output              - generated crc input
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_stamp_logic(int compress_enable, int switch_enable, cint_field_crc_hash_sim_key_t key_a, cint_field_crc_hash_sim_key_t key_b, uint32 compressed_key_a, uint32 compressed_key_b, cint_field_crc_hash_sim_key_t *output)
{
    if(compress_enable)
    {
        output->data[3] = cint_field_crc_hash_get_lsb_n_bits(key_a, key_a.size - 32, 32);
        output->data[4] = cint_field_crc_hash_get_lsb_n_bits(key_b, key_b.size - 32, 32);
        output->data[5] = compressed_key_a;
        output->data[6] = compressed_key_b;
    }
    else
    {
        if (switch_enable)
        {
            output->data[3] = cint_field_crc_hash_get_lsb_n_bits(key_b, key_b.size - 32, 32);
            output->data[4] = cint_field_crc_hash_get_lsb_n_bits(key_b, key_b.size - 64, 32);
            output->data[5] = compressed_key_a;
            output->data[6] = compressed_key_b;
        }
        else
        {
            int i;
            for (i = 0; i < CRC_HASH_SIM_INT_NOF_UINT32 / 2; i++)
            {
                output->data[i] = key_a[i];
            }
            for (; i < CRC_HASH_SIM_INT_NOF_UINT32; i++)
            {
                output->data[i] = key_b[i];
            }
        }
    }
    return BCM_E_NONE;
}

/**
* \brief
*   This function performs the crc hashing
* \param [in] in         - input to crc function
* \param [out] output    - output to crc function
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_perform_crc(
        cint_field_crc_hash_sim_key_t input,
        cint_field_crc_hash_sim_key_t *output)
{
    cint_field_crc_hash_sim_key_t crc;
    cint_field_crc_hash_sim_key_t xor;
    switch(cint_field_crc_hash_sim_function)
    {
    case bcmFieldContextHashFunctionCrc16Bisync:
        cint_field_crc_hash_crc16_bisync(input, output);
        break;
    case bcmFieldContextHashFunctionCrc16Xor1:
        crc.size = 16;
        xor.size = 1;
        cint_field_crc_hash_crc16_bisync(input, &crc);
        cint_field_crc_hash_xor(xor.size, input, &xor);
        cint_field_crc_hash_set_lsb_n_bits(&crc, 0, 8, 0);
        xor.data[0] = xor.data[0] << (16 - xor.size);
        output->data[0] = crc.data[0] | xor.data[0];
        break;
    case bcmFieldContextHashFunctionCrc16Xor2:
        crc.size = 16;
        xor.size = 2;
        cint_field_crc_hash_crc16_bisync(input, &crc);
        cint_field_crc_hash_xor(xor.size, input, &xor);
        cint_field_crc_hash_set_lsb_n_bits(&crc, 0, 8, 0);
        xor.data[0] = xor.data[0] << (16 - xor.size);
        output->data[0] = crc.data[0] | xor.data[0];
        break;
    case bcmFieldContextHashFunctionCrc16Xor4:
        crc.size = 16;
        xor.size = 4;
        cint_field_crc_hash_crc16_bisync(input, &crc);
        cint_field_crc_hash_xor(xor.size, input, &xor);
        cint_field_crc_hash_set_lsb_n_bits(&crc, 0, 8, 0);
        xor.data[0] = xor.data[0] << (16 - xor.size);
        output->data[0] = crc.data[0] | xor.data[0];
        break;
    case bcmFieldContextHashFunctionCrc16Xor8:
        crc.size = 16;
        xor.size = 8;
        cint_field_crc_hash_crc16_bisync(input, &crc);
        cint_field_crc_hash_xor(xor.size, input, &xor);
        cint_field_crc_hash_set_lsb_n_bits(&crc, 0, 8, 0);
        xor.data[0] = xor.data[0] << (16 - xor.size);
        output->data[0] = crc.data[0] | xor.data[0];
        break;
    case bcmFieldContextHashFunctionXor16:
        cint_field_crc_hash_xor(output->size, input, output);
        break;
    case bcmFieldContextHashFunctionCrc16Ccitt:
        cint_field_crc_hash_crc16_ccitt(input, output);
        break;
    default:
        printf("Error: perform_crc: hash function %d is out of range (%d, %d) \n", cint_field_crc_hash_sim_function, bcmFieldContextHashFunctionCrc16Bisync, (bcmFieldContextHashFunctionSpnResult0 - 1));
        return BCM_E_PARAM;
        break;
    }
    return BCM_E_NONE;
}

int cint_field_crc_hash_crc16_bisync(cint_field_crc_hash_sim_key_t input, cint_field_crc_hash_sim_key_t *output)
{
    return cint_field_crc_hash_hash_crc_base(16, input, crc16_bisync_polynomial, output);
}

int cint_field_crc_hash_crc16_ccitt(cint_field_crc_hash_sim_key_t input, cint_field_crc_hash_sim_key_t *output)
{
    return cint_field_crc_hash_hash_crc_base(16, input, crc16_ccit_polynomial, output);
}

int cint_field_crc_hash_crc32_a(cint_field_crc_hash_sim_key_t input, cint_field_crc_hash_sim_key_t *output)
{
    return cint_field_crc_hash_hash_crc_base(32, input, crc32_eth_polynomial, output);
}

int cint_field_crc_hash_crc32_b(cint_field_crc_hash_sim_key_t input, cint_field_crc_hash_sim_key_t *output)
{
    return cint_field_crc_hash_hash_crc_base(32, input, crc32_c_polynomial, output);
}

int cint_field_crc_hash_crc32_c(cint_field_crc_hash_sim_key_t input, cint_field_crc_hash_sim_key_t *output)
{
    return cint_field_crc_hash_hash_crc_base(32, input, crc32_k_polynomial, output);
}

/**
* \brief
*   This function performs the crc for the hash process
* \param [in] size       - size for the crc
* \param [in] input      - input for the crc
* \param [in] polynomial - polynomial used for the crc
* \param [out] output    - output of the crc
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_hash_crc_base(
        int size,
        cint_field_crc_hash_sim_key_t input,
        int polynomial,
        cint_field_crc_hash_sim_key_t *output)
{
    cint_field_crc_hash_sim_key_t temp, crc_result;
    int i, j;
    temp.size = size;
    crc_result.size = size;
    for (i = 0; i < input.size; i++) {
        temp.data[0] = crc_result.data[0];
        if (cint_field_crc_hash_msb_bit(temp, 0))
        {
            temp.data[0] = crc_result.data[0] ^ (polynomial / 2);
            cint_field_crc_hash_set_msb_n_bits(&crc_result, 0, size, temp.data[0]);
        }
        crc_result.data[0] = crc_result.data[0] >> 1;
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_set_lsb_n_bits(&crc_result, 0, 1, cint_field_crc_hash_lsb_bit(input, i) ^ cint_field_crc_hash_msb_bit(temp, 0)));
    }
    BCM_IF_ERROR_RETURN(cint_field_crc_hash_lsb_n_bits(crc_result, 0, size, output));
    return BCM_E_NONE;
}

/**
* \brief
*   This function performs xor for the hash process
* \param [in] size       - size for the xor
* \param [in] input      - input for the xor
* \param [out] result    - output of the xor
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_xor(int size, cint_field_crc_hash_sim_key_t input, cint_field_crc_hash_sim_key_t *result)
{
    int temp, i, j;
    for (i = 0; i < size; i++)
    {
        for (j = 0; (i + j * size) < input.size; j++)
        {
            temp = cint_field_crc_hash_lsb_bit(input, i + j * size) ^ cint_field_crc_hash_lsb_bit(*result, i);
            BCM_IF_ERROR_RETURN(cint_field_crc_hash_set_lsb_n_bits(result, i, 1, temp));
        }
    }
    return BCM_E_NONE;
}

/**
* \brief
*   This function updates the key value
* \param [in] input_crc  - input for the augment
* \param [in] key        - key for the augment
* \param [out] output    - output of the augment
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_update_lb_keys(
        cint_field_crc_hash_sim_key_t input_crc,
        cint_field_crc_hash_sim_key_t key,
        cint_field_crc_hash_sim_key_t *output)
{
    output->data[0] = cint_field_crc_hash_sim_augmented_lb_key_value;
    output->size = 16;
    BCM_IF_ERROR_RETURN(cint_field_crc_hash_reverse_key_bits(output));

    switch(cint_field_crc_hash_sim_hash_action)
    {
    case bcmFieldContextHashActionValueReplaceCrc:
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_copy_key(input_crc, output));
        break;
    case bcmFieldContextHashActionValueAugmentCrc:
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_augment(input_crc, output));
        break;
    case bcmFieldContextHashActionValueAugmentKey:
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_augment(key, output));
        break;
    }
    BCM_IF_ERROR_RETURN(cint_field_crc_hash_reverse_key_bits(output));

    return BCM_E_NONE;
}

/**
* \brief
*   This function performs the augmentation
* \param [in] augmented_value  - value for augmentation
* \param [out] output          - output of the augment
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_augment(
        cint_field_crc_hash_sim_key_t augmented_value,
        cint_field_crc_hash_sim_key_t *output)
{
    cint_field_crc_hash_sim_key_t hash_initial_value;
    hash_initial_value.size = 80;
    uint32 polynomial = 0;
    uint16 seed = 0;
    switch (cint_field_crc_hash_sim_crc_select)
    {
    case bcmFieldCrcSelectCrc16P0x10039:
        polynomial = crc16_10039;
        break;
    case bcmFieldCrcSelectCrc16P0x100d7:
        polynomial = crc16_100d7;
        break;
    case bcmFieldCrcSelectCrc16P0x1015d:
        polynomial = crc16_1015d;
        break;
    case bcmFieldCrcSelectCrc16P0x10939:
        polynomial = crc16_10939;
        break;
    case bcmFieldCrcSelectCrc16P0x109e7:
        polynomial = crc16_109e7;
        break;
    case bcmFieldCrcSelectCrc16P0x10ac5:
        polynomial = crc16_10ac5;
        break;
    case bcmFieldCrcSelectCrc16P0x1203d:
        polynomial = crc16_1203d;
        break;
    case bcmFieldCrcSelectCrc16P0x12105:
        polynomial = crc16_12105;
        break;
    }
    if (cint_field_crc_hash_sim_lsb_to_msb)
    {
        seed = output->data[0];
        hash_initial_value.data[0] = augmented_value.data[0];
        hash_initial_value.data[1] = augmented_value.data[1];
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_shift_to_lsb(&hash_initial_value, 80 - augmented_value.size));
    }
    else
    {
        seed = 0;
        hash_initial_value.data[0] = output->data[0];
        cint_field_crc_hash_shift_to_lsb(&hash_initial_value, augmented_value.size);
        hash_initial_value.data[0] = hash_initial_value.data[0] | augmented_value.data[0];
        hash_initial_value.data[1] = hash_initial_value.data[1] | augmented_value.data[1];
        BCM_IF_ERROR_RETURN(cint_field_crc_hash_shift_to_lsb(&hash_initial_value, 64 - augmented_value.size));
    }
    BCM_IF_ERROR_RETURN(cint_field_crc_hash_calc_crc(16, seed, hash_initial_value, polynomial, output));

    return BCM_E_NONE;
}

/**
* \brief
*   This function performs the crc on the augmented value
* \param [in] size       - size of the seed, polynomial and the output of the hash
* \param [in] seed       - seed used for this hash function
* \param [in] data       - crc input
* \param [in] polynomial - polynomial used for the hash
* \param [out] crc       - result of the hash
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_calc_crc(
        int size,
        uint16 seed,
        cint_field_crc_hash_sim_key_t data,
        uint32 polynomial,
        cint_field_crc_hash_sim_key_t *crc)
{
    int i, j, input_bit, msb_bit, hash_bit, taps_bit, new_bit;
    uint16 hash_output = cint_field_crc_hash_reverse_bits(seed, 16);
    uint32 size_mask = CRC_HASH_SIM_UINT32_FULL_MASK;
    size_mask = size_mask >> (CRC_HASH_SIM_UINT32_NOF_BITS - size);
    if(!cint_field_crc_hash_sim_lsb_to_msb)
    {
        data.size += 16;
    }
    for (i = 0; i < data.size; i++)
    {
        input_bit = cint_field_crc_hash_sim_lsb_to_msb ? cint_field_crc_hash_lsb_bit(data, i) : cint_field_crc_hash_msb_bit(data, i);
        msb_bit = (hash_output >> (size - 1)) & 1;
        hash_output <<= 1;
        hash_output &= size_mask;
        hash_output |= input_bit;
        if (msb_bit)
        {
            hash_output = (hash_output ^ polynomial) & size_mask;
        }
    }
    msb_bit = (hash_output >> (size - 1)) & 1;
    crc->data[0] = cint_field_crc_hash_reverse_bits(hash_output, 16);
    BCM_IF_ERROR_RETURN(cint_field_crc_hash_set_msb_n_bits(crc, 0, 1, msb_bit));
    return BCM_E_NONE;
}

uint32 cint_field_crc_hash_get_lsb_n_bits(cint_field_crc_hash_sim_key_t key, int lsb, int count)
{
    uint32 mask, mask1 = 1, mask2 = 1, data;
    lsb += CRC_HASH_SIM_UINT32_NOF_BITS - (key.size % CRC_HASH_SIM_UINT32_NOF_BITS);
    key.size += CRC_HASH_SIM_UINT32_NOF_BITS - (key.size % CRC_HASH_SIM_UINT32_NOF_BITS);
    mask1 = mask1 << (31 - (lsb % CRC_HASH_SIM_UINT32_NOF_BITS));
    mask1 =  mask1 << 1;
    mask1 = 0 - mask1;
    mask2 = mask2 << (32 - (lsb % CRC_HASH_SIM_UINT32_NOF_BITS) - count);
    mask2 = 0 - mask2;
    mask = mask1 ^ mask2;
    mask = mask >> (32 - key.size);
    data = key.data[(key.size - 1 - lsb) / CRC_HASH_SIM_UINT32_NOF_BITS] & mask;
    data = data >> (32-count - (lsb % CRC_HASH_SIM_UINT32_NOF_BITS) - (32 - key.size));
    return data;
}

int cint_field_crc_hash_lsb_n_bits(cint_field_crc_hash_sim_key_t input, int lsb, int count, cint_field_crc_hash_sim_key_t *output)
{
    int ii, jj;
    uint32 shift = 0;
    int to_lsb = lsb+count;
    if (count == 0)
    {
        return BCM_E_NONE;
    }
    output->size = 1;
    output->data[0] = cint_field_crc_hash_lsb_bit(input, lsb);
    for (ii = lsb+1; ii < to_lsb; ii++)
    {
        cint_field_crc_hash_shift_to_lsb(output, 1);
        output->data[0] |= cint_field_crc_hash_lsb_bit(input, ii);
        output->size++;
    }
    return BCM_E_NONE;
}

uint32 cint_field_crc_hash_lsb_bit(cint_field_crc_hash_sim_key_t key, int lsb)
{
    uint32 mask = 1;
    int index = key.size - 1 - lsb;
    int shift = (index % CRC_HASH_SIM_UINT32_NOF_BITS);
    mask = mask << shift;
    return ((key.data[index/CRC_HASH_SIM_UINT32_NOF_BITS] & mask) >> shift);
}

uint32 cint_field_crc_hash_msb_bit(cint_field_crc_hash_sim_key_t key, int msb)
{
    return (key.data[msb / CRC_HASH_SIM_UINT32_NOF_BITS] >> (msb % CRC_HASH_SIM_UINT32_NOF_BITS)) & 1;
}

int cint_field_crc_hash_set_msb_n_bits(cint_field_crc_hash_sim_key_t *key, int msb, int count, uint32 data)
{
    if(count > 32)
    {
        return BCM_E_PARAM;
    }
    uint32 mask, mask1, mask2, data1, data2;
    int msb_shift = msb % CRC_HASH_SIM_UINT32_NOF_BITS;

    mask1 = (1 << msb_shift) - 1;
    mask2 = 0 - (1 << ((count + msb_shift) % 32));

    data1 = data << msb_shift;
    data2 = data >> (CRC_HASH_SIM_UINT32_NOF_BITS - msb_shift);

    if(msb_shift + count > CRC_HASH_SIM_UINT32_NOF_BITS)
    {
        key->data[msb / CRC_HASH_SIM_UINT32_NOF_BITS] = (key->data[msb / CRC_HASH_SIM_UINT32_NOF_BITS] & (mask1)) | (~mask1 & data1);
        key->data[(msb + count) / CRC_HASH_SIM_UINT32_NOF_BITS] = (key->data[(msb + count) / CRC_HASH_SIM_UINT32_NOF_BITS] & (mask2)) | (~mask2 & data2);
    }
    else {
        mask = mask1 | mask2;
        key->data[msb / CRC_HASH_SIM_UINT32_NOF_BITS] = (key->data[msb / CRC_HASH_SIM_UINT32_NOF_BITS] & (mask)) | (~mask & data1);
    }
    return BCM_E_NONE;
}

int cint_field_crc_hash_set_lsb_n_bits(cint_field_crc_hash_sim_key_t *key, int lsb, int count, uint32 data)
{
    if (count > CRC_HASH_SIM_UINT32_NOF_BITS || count < 0) {
        printf("Error: set_lsb_n_bits: count %d is out of range[%d:%d] \n", count, 0, CRC_HASH_SIM_UINT32_NOF_BITS);
        return BCM_E_PARAM;
    }
    if (count == 0)
    {
        return BCM_E_NONE;
    }
    cint_field_crc_hash_set_lsb_bits(key, lsb, lsb + count - 1, data);
    return BCM_E_NONE;
}

int cint_field_crc_hash_set_lsb_bit(cint_field_crc_hash_sim_key_t *key, int lsb, uint32 bit_data)
{
    if (lsb < 0 || lsb >= key->size)
    {
        printf("Error: set_lsb_bit: bit %d is out of range[%d:%d] \n", lsb, 0, key->size-1);
        return BCM_E_PARAM;
    }
    int mask;
    lsb = key->size - lsb - 1;
    bit_data = bit_data << (lsb % CRC_HASH_SIM_UINT32_NOF_BITS);
    mask = 1 << (lsb % CRC_HASH_SIM_UINT32_NOF_BITS);
    key->data[lsb / CRC_HASH_SIM_UINT32_NOF_BITS] = (key->data[lsb / CRC_HASH_SIM_UINT32_NOF_BITS] & (~mask)) | bit_data;
    return BCM_E_NONE;
}

int cint_field_crc_hash_set_msb_bit(cint_field_crc_hash_sim_key_t *key, int msb, uint32 bit_data)
{
    if (msb < 0 || msb >= key->size)
    {
        printf("Error: set_msb_bit: bit %d is out of range[%d:%d] \n", msb, 0, key->size-1);
        return BCM_E_PARAM;
    }
    int mask;
    bit_data = bit_data << (msb % CRC_HASH_SIM_UINT32_NOF_BITS);
    mask = 1 << (msb % CRC_HASH_SIM_UINT32_NOF_BITS);
    key->data[msb / CRC_HASH_SIM_UINT32_NOF_BITS] = (key->data[msb / CRC_HASH_SIM_UINT32_NOF_BITS] & (~mask)) | bit_data;
    return BCM_E_NONE;
}

int cint_field_crc_hash_set_lsb_bits(cint_field_crc_hash_sim_key_t *key, int from_lsb, int to_lsb, uint32 data)
{
    int bit_idx;
    for(bit_idx = from_lsb ; bit_idx <= to_lsb; bit_idx++, data >>= 1)
    {
        cint_field_crc_hash_set_lsb_bit(key, bit_idx, data & 1);
    }
    return BCM_E_NONE;
}

uint32 cint_field_crc_hash_reverse_bits(uint32 input, int size)
{
    int count = size * 8- 1;
    uint32 output = input;
    input >>= 1;
    while(input)
    {
        output <<= 1;
        output |= input & 1;
        input >>= 1;
        count--;
    }
    output <<= count;
    output >>= (32 - size);
    return output;
}

/**
* \brief
*   This function reverses the key's data
* \param [in/out] key   - key to reverse
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_reverse_key_bits(cint_field_crc_hash_sim_key_t *key)
{
    int ii;
    int msb, lsb;
    if (key->size <= 0)
    {
        return BCM_E_NONE;
    }
    for (ii = 0; ii < key->size / 2; ii++)
    {
        lsb = cint_field_crc_hash_lsb_bit(*key, ii);
        msb = cint_field_crc_hash_msb_bit(*key, ii);
        cint_field_crc_hash_set_msb_bit(key, ii, lsb);
        cint_field_crc_hash_set_lsb_bit(key, ii, msb);
    }
    return BCM_E_NONE;
}

/**
* \brief
*   This function shifts the key to the lsb (LSB)
* \param [in/out] key   - input key
* \param [in] shift     - number of shifts
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_shift_to_lsb(cint_field_crc_hash_sim_key_t *key, int shift)
{
    int i, curr_shift;
    while(shift > 0) {
        curr_shift = shift > 31 ? 31 : shift;
        for (i = CRC_HASH_SIM_INT_NOF_UINT32 - 1; i > 0; i--)
        {
            key->data[i] = (key->data[i] << curr_shift) | (key->data[i - 1] >> (CRC_HASH_SIM_UINT32_NOF_BITS - curr_shift));
        }
        key->data[0] <<= curr_shift;
        shift -= 31;
    }
    return BCM_E_NONE;
}

/**
* \brief
*   This function shifts the key to the right (MSB)
* \param [in/out] key   - input key
* \param [in] shift     - number of shifts
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_shift_to_msb(cint_field_crc_hash_sim_key_t *key, int shift)
{
    int i, curr_shift;
    while(shift > 0) {
        curr_shift = shift > 31 ? 31 : shift;
        for (i = 0; i < CRC_HASH_SIM_INT_NOF_UINT32 - 1; i++)
        {
            key->data[i] = (key->data[i] >> curr_shift) | (key->data[i - 1] << (CRC_HASH_SIM_UINT32_NOF_BITS - curr_shift));
        }
        key->data[CRC_HASH_SIM_INT_NOF_UINT32 - 1] >>= curr_shift;
        shift -= 31;
    }
    return BCM_E_NONE;
}

/**
* \brief
*   This function copies input key to output key
* \param [in] input       - input key
* \param [out] output     - output key
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_crc_hash_copy_key(cint_field_crc_hash_sim_key_t input, cint_field_crc_hash_sim_key_t *output)
{
    int i;
    output->size = input.size;
    for (i = 0; i < CRC_HASH_SIM_INT_NOF_UINT32; i++)
    {
        output->data[i] = input.data[i];
    }
    return BCM_E_NONE;
}

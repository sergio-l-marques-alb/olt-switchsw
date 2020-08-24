/*
 * $Id: cint_load_balancing_sim.c
 */
/*
 * A HW simulator to generating load balancing keys.
 */


/*
 * A list of supported devices by the simulator
 */
int DEVICE_TYPE_JERICHO2_A0 = 0;
int DEVICE_TYPE_JERICHO2_B0 = 1;
int DEVICE_TYPE_QUMRAN2_A0 = 2;
int DEVICE_TYPE_JERICHO2C = 3;
int DEVICE_TYPE_JERICHO2P = 4;
/*
 * Used as constant values in this file
 */
int ELI_VALUE                       = 7;
int UINT32_NOF_BITS                 = 32;
int UINT32_FULL_MASK                = 0xFFFFFFFF;
int HASH_INPUT_IN_BITS              = 128;
int HASH_WIDTH_WITH_PADDING         = 128;
int NOF_LAYERS                      = 8;
int NOF_CLIENTS                     = 5;
int NOF_BYTE_IN_UINT32              = 4;
int MAX_NOF_MPLS_LABELS_IN_STACK    = 16;
int MAX_NOF_MPLS_STACKS             = 2;
int MAX_NOF_MPLS_STACKS_PLUS_DUMMY  = MAX_NOF_MPLS_STACKS + 1;
int MPLS_LABEL_SIZE                 = 20;
int HASH_LB_KEYS_WIDTH              = 128;
int HEADER_BUFFER_SIZE              = 512;
int MPLS_HASH_WIDTH                 = 320;

/*
 * This header is part of the previous header
 */
uint32 LB_SIM_HEADER_OFFSET_USE_PREV_LAYER = 1;
/*
 * A flag which indicates which MPLS layer is the forwarding one.
 * Relevant for J2P only.
 */
uint32 LB_SIM_HEADER_OFFSET_FWD_LABEL = 2;

/*
 * This structure holds information regarding a single header inside the packet (e.g. IPv4, Ethernet, UDP etc..).
 */
struct header_offsets_t
{
    /*
     * The offset of the header in bits inside the packet
     */
    int offset;
    /*
     * The type of the header, taken from the parser auto generated file cint_load_balancing_fields_ag.c
     */
    int header_type;
    /*
     * Additional information for the header offset (LB_SIM_HEADER_OFFSET_XXX flags)
     */
    int flags;
};

/*
 * This structure holds all the relevant HW values to generate the load balancing keys.
 * Using the load_balancing_sim_hw_cfg_get function in a real device will return this structure filled with the device
 * load balancing configuration.
 */
struct load_blancing_sim_hw_cfg_t
{
    /*
     * The device which the simulation should simulate.
     */
    int device;
    /*
     * The seed that is used by the parser
     */
    uint32 parser_seed;
    /*
     * Seed that are used to generate the MPLS hash fields
     */
    uint32 mpls_seed[2];
    /*
     * The seeds of the CRC functions that each LB client uses.
     */
    uint32 clients_polynomial_seeds[5];
    /*
     * The CRC function that each LBclient uses
     */
    uint32 clients_polynomial[5];
    /*
     * The CRC 16 bit selection that each client uses.
     */
    uint32 clients_16_crc_selection[5];
};


/*
 * Represent a stack of MPLS labels (this structure is for internal use and isn't used by the simulation APIs).
 */
struct mpls_stack_internal_t
{
    /*
     * The CRC layer that this stack should update
     */
    uint32 crc_layer;
    /*
     * NOF MPLS labels in the stack
     */
    uint32 stack_size;
    /*
     * The labels in the stack
     */
    uint32 labels[16];
};


/*
 * This structure holds the packet headers information.
 */
struct load_blancing_sim_headers_info_t
{
    /*
     * an array that hold in each instance a single header offset inside the packet in bits and the header type.
     * which is taken from the value that are provided in the simulation auto generated file.
     */
    header_offsets_t header_offsets[20];

    /*
     * the size of the header_offsets array
     */
    int nof_header_offsets;

    /*
     * the forwarding header index inside the header_offsets array.
     */
    int forwarding_layer;
};

/** Bitmap valid for J2P which indicates which MPLS labels are used as input for CRC calculation */
uint16 mpls_label_include_in_key[4] = {0, 0, 0, 0};
/**
 * Set the global variable constants to their correct values based on the device.
 */
void load_balancing_update_device_constants(int device)
{
    if (device == DEVICE_TYPE_QUMRAN2_A0 || device == DEVICE_TYPE_JERICHO2C || device == DEVICE_TYPE_JERICHO2P)
    {
        HASH_WIDTH_WITH_PADDING = 160;
        if (device == DEVICE_TYPE_QUMRAN2_A0 || device == DEVICE_TYPE_JERICHO2P)
        {
            HASH_LB_KEYS_WIDTH = 144;
            MPLS_HASH_WIDTH = 352;
            if (device == DEVICE_TYPE_JERICHO2P)
            {
                MAX_NOF_MPLS_STACKS = 4;
            }
        }
    }
}

/**
 * This function decides whether the algorithm should take the bits from LSB to MSB or from MSB to LSB.
 * Then it retrieves the input bit for the hashing from the input data based on the current iteration index.
 */
int load_balancing_retrieve_input_bit(int device, uint32 * data, uint32 input_width, int idx)
{
    int input_bit;
    uint8 lsb_to_msb = 0;
    uint32 data_in_last_index = ((input_width / UINT32_NOF_BITS) - 1);
    /*
     * JR2_A0 and JR2_B0 always process LSB to MSB.
     * J2C processes the bits from LSB to MSB in case building the MPLS layer record and when building the LB keys.
     * Q2A always processes bits from MSB to LSB.
     */
    lsb_to_msb = (device == DEVICE_TYPE_JERICHO2_A0 || device == DEVICE_TYPE_JERICHO2_B0) ? 1 :
            ((device == DEVICE_TYPE_JERICHO2C) ? (input_width != HASH_WIDTH_WITH_PADDING ? 1 : 0) : 0);
    if (lsb_to_msb)
    {
        /** LSB to MSB */
        input_bit = (data[data_in_last_index - (idx/UINT32_NOF_BITS)] >> (idx%UINT32_NOF_BITS)) & 0x1;
    }
    else
    {
        /** MSB to LSB */
        input_bit = (data[(idx/UINT32_NOF_BITS)] >> (UINT32_NOF_BITS - idx%UINT32_NOF_BITS - 1)) & 0x1;
    }
    return input_bit;
}


/*
 * As the simulator uses a uint32 bits and the input to simulation is in bytes as other packet APIs, this function
 * convert between the two.
 */
void
load_balancing_sim_uint8_to_uint32_array(uint8 *buffer_byte, int buffer_size_byte, uint32* buffer_out)
{
    int byte_iter;
    int byte_count = 0;
    int buffer_size_32 = 0;
    while(byte_count < buffer_size_byte)
    {
        buffer_out[buffer_size_32] = 0;
        for(byte_iter = 0; (byte_iter < NOF_BYTE_IN_UINT32) && (byte_count < buffer_size_byte);byte_iter++)
        {
            buffer_out[buffer_size_32] |= buffer_byte[byte_count++] << ((NOF_BYTE_IN_UINT32 - 1 - byte_iter) << 3);
        }
        buffer_size_32++;
    }
}
/*
 * shift right a uint32 buffer
 * buffer - the buffer to shift
 * buffer_size - the size in uint32 of the buffer to shift
 * shift_in_bits = nof bits to shift.
 */
void
load_balancing_buffer_shift_right(uint32 *buffer,uint32 buffer_size, uint32 shift_in_bits)
{

    int iter;
    int source_buffer;
    for(iter = 0; iter < buffer_size; iter++)
    {
        /** this is due to cint issue that doesn't clear all bits in case of 32 bits shift right**/
        if(shift_in_bits == UINT32_NOF_BITS)
        {
            buffer[iter] = 0;
        }
        else
        {
            buffer[iter] >>= shift_in_bits;
        }

        source_buffer = iter + ((shift_in_bits + UINT32_NOF_BITS -1)/UINT32_NOF_BITS);
        if(source_buffer < buffer_size)
        {
            buffer[iter] |= buffer[source_buffer] << (UINT32_NOF_BITS - shift_in_bits);
        }
    }
}

/*
 * shift left a uint32 buffer
 * buffer - the buffer to shift
 * buffer_size - the size in uint32 of the buffer to shift
 * shift_in_bits = nof bits to shift.
 */
void
load_balancing_buffer_shift_left(uint32 *buffer,uint32 buffer_size, uint32 shift_in_bits)
{
    int iter;
    int source_buffer;
    for(iter = (buffer_size -1); iter >= 0; iter--)
    {
        /** this is due to cint issue that doesn't clear all bits in case of 32 bits shift left**/
        if(shift_in_bits == UINT32_NOF_BITS)
        {
            buffer[iter] = 0;
        }
        else
        {
            buffer[iter] <<= shift_in_bits;
        }


        source_buffer = iter - ((shift_in_bits + UINT32_NOF_BITS -1)/UINT32_NOF_BITS);
        if(source_buffer >= 0)
        {
            buffer[iter] |= buffer[source_buffer] >> (UINT32_NOF_BITS - shift_in_bits);
        }
    }
}

/*
 * Barrel shift right the buffer
 * buffer - the buffer to shift
 * buffer_size - the size in uint32 of the buffer to shift
 * shift_in_bits = nof bits to shift.
 */
void
load_balancing_barrel_shift_right(uint32 *buffer,uint32 buffer_size, uint32 shift_in_bits)
{
    uint32 *tmp_buffer = sal_alloc(buffer_size * NOF_BYTE_IN_UINT32, "barrel shift buffer");
    uint32 uint32_shift = shift_in_bits / UINT32_NOF_BITS;
    uint32 bits_shift = shift_in_bits % UINT32_NOF_BITS;
    uint32 iter;
    sal_memset(tmp_buffer, 0x0, buffer_size * NOF_BYTE_IN_UINT32);

    for(iter = 0; iter < buffer_size; iter++)
    {
        uint32 index = (buffer_size + iter - uint32_shift - 1) % buffer_size;
        /** this is due to cint issue that doesn't clear all bits in case of 32 bits shift left**/
        if(bits_shift != 0)
        {
            tmp_buffer[iter] |= (buffer[index] << (UINT32_NOF_BITS - bits_shift));
        }
        index = (index+1)% buffer_size;
        tmp_buffer[iter] |= buffer[index] >> bits_shift;
    }
    sal_memcpy(buffer, tmp_buffer, buffer_size * NOF_BYTE_IN_UINT32);

    sal_free(tmp_buffer);
}

/*
 * Perform a XOR and fold on a single header to get a 128 bit vector
 * header_input - the header input to fold and XOR
 * header_buffer_size - the header_input size in uint32 units
 * xor_fold_buffer - the output of 128 bits.
 */
void
load_balancing_xor_and_fold(uint32 *header_input,
                            uint32 header_buffer_size,
                            uint32 *xor_fold_buffer,
                            uint32 nof_bits_to_shift)
{
    int fold_iter, buffer_iter;
    int nof_uint32_in_one_fold = HASH_INPUT_IN_BITS / UINT32_NOF_BITS;
    int nof_folds = (HEADER_BUFFER_SIZE / HASH_INPUT_IN_BITS) - 1;

    sal_memset(xor_fold_buffer, 0x0, nof_uint32_in_one_fold * NOF_BYTE_IN_UINT32);
    for(fold_iter = 0; fold_iter < nof_folds; fold_iter++)
    {
        for(buffer_iter = 0;(buffer_iter < nof_uint32_in_one_fold) && ((fold_iter * nof_uint32_in_one_fold + buffer_iter) < header_buffer_size);buffer_iter++)
        {
            xor_fold_buffer[buffer_iter] ^= header_input[fold_iter * nof_uint32_in_one_fold + buffer_iter];
        }
    }

    if(nof_bits_to_shift != 0)
    {
        load_balancing_barrel_shift_right(xor_fold_buffer,nof_uint32_in_one_fold,nof_bits_to_shift);
    }
}


/*
 * This is the hash function for both the 32 and 16 bits outputs/inputs
 * seed - the seed used for this hash function
 * data_in - a 128 bits input
 * input_width - the width of data_in in bits
 * polynomial - the polynomial used for the hash
 * hash_width - the size of the seed, polynomial and the output of the hash.
 * device - the device to simulate
 * hash_output - the result of the hash
 */
void
load_balancing_calculate_hash(
    uint32 seed, uint32 * data_in, uint32 input_width, uint32 polynomial, uint32 hash_width, int device, uint32 *hash_output)
{
    uint32 taps = polynomial;
    uint32 int_hash_output[2];
    uint32 msb_bit;
    uint32 hash_bit;
    uint32 taps_bit;
    uint32 i,j;
    uint32 size_mask = 0xFFFFFFFF;
    int new_bit;

    int_hash_output[0] = seed;

    /* Prepare the mask for the correct size of the seed and polynomial */
    size_mask = (size_mask >> (UINT32_NOF_BITS - hash_width));

    for (i = 0; i < input_width; ++i) {
        int input_bit = load_balancing_retrieve_input_bit(device, data_in, input_width, i);

        msb_bit = (int_hash_output[0] >> (hash_width - 1)) & 1;

        load_balancing_buffer_shift_left(int_hash_output[0],2,1);

        int_hash_output[0] &= size_mask;
        int_hash_output[0] = int_hash_output[0] | input_bit;

        for (j = 0; j < hash_width; ++j)
        {
            hash_bit = (int_hash_output[0] >> j) & 1;
            taps_bit = (taps >> j) & 1;
            new_bit = hash_bit ^ (msb_bit & taps_bit);
            int_hash_output[0] ^= ((-new_bit) ^ int_hash_output[0]) & (1 << j);
        }
    }
    msb_bit = (int_hash_output[0] >> (hash_width - 1)) & 1;
    *hash_output = int_hash_output[0] & (UINT32_FULL_MASK >> 1);
}


/**
 * \brief
 *   copy a range from one uint32 array into another uint32 array.
 *   This function assumes that:
 *   1.index 0 is the left most bit at the first member of the array (array[0] bit 31)
 *   2.the destination array is set to zero in the range that is about to by copied.
 *  Parameters:
 *  dst_ptr - The destination uint32 array.
 *  dst_first - The index to copy to in the destination array.
 *  src_ptr - The source array.
 *  src_first - The index to copy from in the source array.
 *  range - the range to copy
 */
void
load_balancing_util_copy_range(
                     uint32 *dst_ptr,
                     int dst_first,
                     uint32 *src_ptr,
                     int src_first,
                     int range)
{
    while(range > 0)
    {
        uint32 dst_array_index = dst_first / UINT32_NOF_BITS;
        uint32 dst_member_pos = dst_first % UINT32_NOF_BITS;
        uint32 src_array_index = src_first / UINT32_NOF_BITS;
        uint32 src_member_pos = src_first % UINT32_NOF_BITS;
        uint32 rage_2_cpy = (dst_member_pos > src_member_pos) ? (UINT32_NOF_BITS - dst_member_pos) : (UINT32_NOF_BITS - src_member_pos);
        if(range < rage_2_cpy)
        {
            rage_2_cpy = range;
        }
        uint32 mask =  (UINT32_FULL_MASK << (UINT32_NOF_BITS -rage_2_cpy)) ;
        dst_ptr[dst_array_index] |=  ((src_ptr[src_array_index] << src_member_pos) & mask) >> dst_member_pos;
        range -= rage_2_cpy;
        dst_first += rage_2_cpy;
        src_first += rage_2_cpy;
    }
}

/*
 * Simulation of the FLP part which select a 16 bit chunk from the 32 bit CRC input and takes only the relevant 32 CRC
 * inputs for the hash to generate the keys.
 * forwarding_layer - the forwarding layer
 * hw_cfg - The HW configuration structure.
 * crc_32 - the 8 layers of CRC 32 that were generated in the parser/VTT
 * force_layers - record layers that must be taking even if above the forwarding header (terminated).
 * lb_keys - The output of 5 LB keys
 */
void
load_balancing_sim_flp(
        int forwarding_layer,
        load_blancing_sim_hw_cfg_t *hw_cfg,
        uint32 *crc_32,
        uint8 *force_layers,
        uint32 *lb_keys)
{
    int client_iter;
    int layer_index;
    uint32 client_crc[HASH_WIDTH_WITH_PADDING / UINT32_NOF_BITS];

    for(client_iter = 0; client_iter < NOF_CLIENTS;client_iter++)
    {
        sal_memset(client_crc, 0x0, sizeof(client_crc[0]) * (HASH_WIDTH_WITH_PADDING / UINT32_NOF_BITS));
        for(layer_index = 0; layer_index < NOF_LAYERS;layer_index++)
        {
            if(layer_index >= forwarding_layer || force_layers[layer_index])
            {
                load_balancing_util_copy_range(client_crc,(NOF_LAYERS - layer_index -1)*16,&crc_32[layer_index],((1-(hw_cfg->clients_16_crc_selection[client_iter] >> layer_index) & 0x1)) * 16,16);
            }
        }

        uint32 seed = hw_cfg->clients_polynomial_seeds[client_iter];
        uint32 polynomial = hw_cfg->clients_polynomial[client_iter];
        load_balancing_calculate_hash(seed, client_crc, HASH_LB_KEYS_WIDTH, polynomial, 16, hw_cfg->device, &lb_keys[client_iter]);
    }
}

/**
 * \brief
 *   Simulate the FLP CRC construction from the MPLS stacks
 *
 *  mpls_stacks - The MPLS stacks
 *  hw_cfg - Configuration which should be extracted from a device that provides all the LB relevant HW configuration.
 *  crc_32 - The 32 bit CRC to update with the MPLS information.
 */
void
load_balancing_sim_update_crc_with_mpls_info(
        mpls_stack_internal_t *mpls_stacks,
        load_blancing_sim_hw_cfg_t *hw_cfg,
        uint32 *crc_32)
{
    int stack_iter;
    uint32 empty_label = 0;
    uint32 mpls_buffer[MPLS_HASH_WIDTH/UINT32_NOF_BITS];

    for(stack_iter = 0; stack_iter < MAX_NOF_MPLS_STACKS; stack_iter++)
    {
        if(mpls_stacks[stack_iter].stack_size > 0)
        {
            uint32 label_iter;
            sal_memset(mpls_buffer, 0x0, (MPLS_HASH_WIDTH/UINT32_NOF_BITS) * NOF_BYTE_IN_UINT32);
            for(label_iter = 0; label_iter < mpls_stacks[stack_iter].stack_size;label_iter++)
            {
                if (hw_cfg->device == DEVICE_TYPE_JERICHO2P && ((mpls_label_include_in_key[stack_iter] >> label_iter) & 0x1) == 0)
                {
                    load_balancing_util_copy_range(mpls_buffer,MPLS_LABEL_SIZE*(MAX_NOF_MPLS_LABELS_IN_STACK-label_iter-1), &empty_label, 0, MPLS_LABEL_SIZE);
                }
                else
                {
                    load_balancing_util_copy_range(mpls_buffer,MPLS_LABEL_SIZE*(MAX_NOF_MPLS_LABELS_IN_STACK-1-label_iter), &(mpls_stacks[stack_iter].labels[label_iter]), 0, MPLS_LABEL_SIZE);
                }
            }
            if (hw_cfg->device == DEVICE_TYPE_JERICHO2C || hw_cfg->device == DEVICE_TYPE_JERICHO2_B0)
            {
                mpls_buffer[0] = hw_cfg->mpls_seed[stack_iter];
                hw_cfg->mpls_seed[stack_iter] = 0;
            }
            load_balancing_calculate_hash(hw_cfg->mpls_seed[stack_iter], mpls_buffer, MPLS_HASH_WIDTH, 0xDD22D287, 32, hw_cfg->device, &(crc_32[mpls_stacks[stack_iter].crc_layer]));
        }
    }
}


/**
 * \brief
 *   Validate the simulation input parameters.
 *
 *  packet_size - The array size of the packet (the packet size in byte chunks).
 *  headers_info - Holds all the headers offsets in the packet and there type along with the forwarding header.
 *  hw_cfg - Configuration which should be extracted from a device that provides all the LB relevant HW configuration.
 */
int
load_balancing_validate_simulation_inputs(
        int packet_size_byte,
        load_blancing_sim_headers_info_t *headers_info,
        load_blancing_sim_hw_cfg_t *hw_cfg
        )
{

    int iter;
    if(packet_size_byte <= 0)
    {
        printf("Packet header size (%d) must be a positive number  \n",packet_size_byte);
        return BCM_E_PARAM;
    }

    if(headers_info->nof_header_offsets <= 0)
    {
        printf("The number of header offsets (%d) must be a positive number  \n",headers_info->nof_header_offsets);
        return BCM_E_PARAM;
    }

    if(headers_info->forwarding_layer < 0 || headers_info->forwarding_layer > headers_info->nof_header_offsets)
    {
        printf("The forwarding layer (%d) must be a non negative number the is smaller than the number of header offsets (%d)\n",headers_info->forwarding_layer,headers_info->nof_header_offsets);
        return BCM_E_PARAM;
    }

    if(hw_cfg->device != DEVICE_TYPE_JERICHO2_A0 && hw_cfg->device != DEVICE_TYPE_JERICHO2_B0
            && hw_cfg->device != DEVICE_TYPE_QUMRAN2_A0 && hw_cfg->device != DEVICE_TYPE_JERICHO2C
            && hw_cfg->device != DEVICE_TYPE_JERICHO2P)
    {
        printf("The device number %d isn't supported \n",hw_cfg->device );
        return BCM_E_PARAM;
    }

    if (headers_info->header_offsets[0].flags & LB_SIM_HEADER_OFFSET_USE_PREV_LAYER)
    {
        printf("First header offset can't hold the LB_SIM_HEADER_OFFSET_USE_PREV_LAYER flag \n");
        return BCM_E_PARAM;
    }

    for(iter = 0; iter < headers_info->nof_header_offsets;iter++)
    {
        if(iter < headers_info->nof_header_offsets -1)
        {
            if(headers_info->header_offsets[iter].offset >= headers_info->header_offsets[iter+1].offset)
            {
                printf("Header offset %d (%d) is larger or equal than the following header offset %d (%d) \n",iter,headers_info->header_offsets[iter].offset,iter+1,headers_info->header_offsets[iter+1].offset);
                return BCM_E_PARAM;
            }
        }

        if(headers_info->header_offsets[iter].offset  >= (packet_size_byte * 8))
        {
            printf("Header offset %d (%d) is larger than the packet size in bits \n",iter,headers_info->header_offsets[iter].offset,packet_size_byte * 8);
            return BCM_E_PARAM;
        }

        if(headers_info->header_offsets[iter].header_type < -1 || headers_info->header_offsets[iter].header_type >= LB_SIM_NOF_HEADERS)
        {
            printf("Header type number %d is %d but must be between -1 and  %d \n",iter,headers_info->header_offsets[iter].header_type,LB_SIM_NOF_HEADERS);
            return BCM_E_PARAM;
        }
    }

    for(iter = 0; iter < NOF_CLIENTS;iter++)
    {
        if(hw_cfg->clients_polynomial_seeds[iter] > 0xFFFF)
        {
            printf("Polynomial seed number %d is 0x%x and must be below or equal to 0xFFFF \n",iter,hw_cfg->clients_polynomial_seeds[iter]);
            return BCM_E_PARAM;
        }

        if(hw_cfg->clients_polynomial[iter] > 0xFFFF)
        {
            printf("Polynomial number %d is 0x%x and must be below or equal to 0xFFFF \n",iter,hw_cfg->clients_polynomial[iter]);
            return BCM_E_PARAM;
        }

        if(hw_cfg->clients_16_crc_selection[iter] > 0xFF)
        {
            printf("CRC 16 bit selection number %d is 0x%x and must be below or equal to 0xFF \n",iter,hw_cfg->clients_16_crc_selection[iter]);
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *      Parse headers_info and build MPLS stacks
 *  Parameters:
 *  header_offset_iter - Offset in index from beginning of packet
 *  headers_info - Holds all the headers offsets in the packet and there type along with the forwarding header.
 *  hw_cfg - Configuration which should be extracted from a device that provides all the LB relevant HW configuration.
 *  layer_record_counter - number of layer_record entries
 *  mpls_stack_counter - ID of the MPLS stack.
 *  mpls_stacks - structure that holds all relevant data for the MPLS stack.
 *  packet_32 - The packet that LB keys are to extract from.
 *  force_layers - MPLS layers which are forcibly used in hashing.
 */
void
load_balancing_build_mpls_stacks(
    int *header_offset_iter,
    load_blancing_sim_headers_info_t *headers_info,
    load_blancing_sim_hw_cfg_t *hw_cfg,
    uint32 *layer_record_counter,
    uint32 mpls_stack_counter,
    mpls_stack_internal_t *mpls_stacks,
    uint32 * packet_32,
    uint8 *force_layers)
{
    uint32 label_index = 0;
    mpls_stacks[mpls_stack_counter].crc_layer = *layer_record_counter;

    if (hw_cfg->device != DEVICE_TYPE_JERICHO2P)
    {
        while((*header_offset_iter < headers_info->nof_header_offsets) && (headers_info->header_offsets[*header_offset_iter].header_type == LB_SIM_HEADER_MPLS))
        {
            if(label_index < MAX_NOF_MPLS_LABELS_IN_STACK)
            {
                load_balancing_util_copy_range((mpls_stacks[mpls_stack_counter].labels[label_index]), 0, packet_32,
                        headers_info->header_offsets[*header_offset_iter].offset, MPLS_LABEL_SIZE);
                label_index += 1;
            }

            *header_offset_iter += 1;
        }

        force_layers[*layer_record_counter] = 1;
        *layer_record_counter += 1;
        force_layers[*layer_record_counter] = 1;
        *layer_record_counter += 1;
        mpls_stacks[mpls_stack_counter].stack_size = label_index;

        /** only the first two MPLS stacks will be in the hashing and the rest won't be processed */
        if (mpls_stack_counter < MAX_NOF_MPLS_STACKS)
        {
            mpls_stack_counter += 1;
        }
    }
    else
    {
        int part_term = -1;
        int passed_fwd_layer = 0;
        int el_in_mpls_stack = 0;
        int mpls_header_size = 32;
        int mpls_label_size = 20;
        int nof_term_labels = 0;
        int temp_offset = *header_offset_iter;
        /** Look for ELI in the stack as in J2P it is handled differently, break if ELI is found and save the header offset. */
        while((temp_offset < headers_info->nof_header_offsets) && (headers_info->header_offsets[temp_offset].header_type == LB_SIM_HEADER_MPLS))
        {
            uint32 mpls_label;
            load_balancing_util_copy_range(mpls_label, 0, packet_32,
                                    headers_info->header_offsets[temp_offset].offset, MPLS_LABEL_SIZE);
            mpls_label = mpls_label >> (mpls_header_size - mpls_label_size);
            printf("MPLS label = %d\n", mpls_label);
            if (mpls_label == ELI_VALUE)
            {
                el_in_mpls_stack = 1;
                break;
            }
            temp_offset++;
        }

        while((*header_offset_iter < headers_info->nof_header_offsets) && (headers_info->header_offsets[*header_offset_iter].header_type == LB_SIM_HEADER_MPLS))
        {
            /** Check if EL was found in stack, if it was and it wasn't terminated, only EL will participate in hashing */
            if (el_in_mpls_stack != 0)
            {
                if (temp_offset == *header_offset_iter)
                {
                    el_in_mpls_stack = 2;
                }
                /** If in the previous iteration we have found the ELI, this iteration we need to work on the EL. */
                else if (el_in_mpls_stack == 2)
                {
                    el_in_mpls_stack = 3;
                }
                if (el_in_mpls_stack > 0 && el_in_mpls_stack < 3)
                {
                    if (headers_info->header_offsets[*header_offset_iter].flags & LB_SIM_HEADER_OFFSET_FWD_LABEL)
                    {
                        passed_fwd_layer = 1;
                    }
                    if ((*layer_record_counter + label_index - 1) <= headers_info->forwarding_layer)
                    {
                        *layer_record_counter += 1;
                        part_term = 0;
                    }
                    *header_offset_iter = *header_offset_iter + 1;
                    nof_term_labels++;
                    continue;
                }

                /** If EL was found in the stack but was terminated, then only the non-terminated layers participate in the hashing */
                if (el_in_mpls_stack == 3 && passed_fwd_layer == 0)
                {
                    if (headers_info->header_offsets[*header_offset_iter].flags & LB_SIM_HEADER_OFFSET_FWD_LABEL)
                    {
                        printf("This is FWD layer\n");
                        label_index += nof_term_labels + 1;
                        *layer_record_counter+=1;
                        passed_fwd_layer = 1;
                    }
                    else
                    {
                        if ((*layer_record_counter + label_index - 1) <= headers_info->forwarding_layer)
                        {
                            *layer_record_counter += 1;
                            part_term = 0;
                        }
                        *header_offset_iter = *header_offset_iter + 1;
                        continue;
                    }
                }
            }

            if(label_index < MAX_NOF_MPLS_LABELS_IN_STACK)
            {
                load_balancing_util_copy_range((mpls_stacks[mpls_stack_counter].labels[label_index]), 0, packet_32,
                        headers_info->header_offsets[*header_offset_iter].offset, MPLS_LABEL_SIZE);
                load_balancing_util_copy_range((mpls_stacks[mpls_stack_counter + 1].labels[label_index]), 0, packet_32,
                        headers_info->header_offsets[*header_offset_iter].offset, MPLS_LABEL_SIZE);
                label_index += 1;
            }
            if (headers_info->header_offsets[*header_offset_iter].flags & LB_SIM_HEADER_OFFSET_FWD_LABEL)
            {
                passed_fwd_layer = 1;
            }
            if (passed_fwd_layer)
            {
                mpls_label_include_in_key[mpls_stack_counter + 1] |= (1 << (label_index - 1));
            }
            else
            {
                mpls_label_include_in_key[mpls_stack_counter] |= (1 << (label_index - 1));
            }

            if ((*layer_record_counter + label_index - 1) <= headers_info->forwarding_layer)
            {
                part_term = 0;
            }
            else
            {
                force_layers[*layer_record_counter] = 1;
                if (part_term == 0)
                {
                    part_term = 1;
                }
            }
            if (el_in_mpls_stack == 0)
            {
                *layer_record_counter += 1;
            }
            else
            {
                *layer_record_counter -= 1;
            }

            *header_offset_iter = *header_offset_iter + 1;
        }
        if (part_term == 1)
        {
            mpls_stacks[mpls_stack_counter].stack_size = label_index;
            mpls_stacks[mpls_stack_counter + 1].stack_size = label_index;
            mpls_stacks[mpls_stack_counter + 1].crc_layer = mpls_stacks[mpls_stack_counter].crc_layer + 1;
        }
        else
        {
            mpls_stacks[mpls_stack_counter].stack_size = label_index;
            mpls_stacks[mpls_stack_counter + 1].stack_size = 0;
        }
        mpls_stacks[2].stack_size = 0;
        mpls_stacks[3].stack_size = 0;
    }
}

/**
 * \brief
 *   This is the simulation main function.
 *   It gets a packet, headers offsets and types details, HW configuration and and returns the 5 load balancing keys.
 *  Parameters:
 *  packet - The packet that LB keys are to extract from.
 *  packet_size - The array size of the packet (the packet size in byte chunks).
 *  headers_info - Holds all the headers offsets in the packet and there type along with the forwarding header.
 *  hw_cfg - Configuration which should be extracted from a device that provides all the LB relevant HW configuration.
 *  lb_keys - an array of size 5 that is used to return all the load balancing keys.
 */
int
load_balancing_sim_main(
    uint8* packet,
    int packet_size_byte,
    load_blancing_sim_headers_info_t *headers_info,
    load_blancing_sim_hw_cfg_t *hw_cfg,
    uint32* lb_keys)
{
    int rv= BCM_E_NONE;
    int header_offset_iter = 0;
    uint32 * header_buffer;
    uint32 * symmetrical_buffer;
    uint32 * packet_32;
    int packet_size;
    uint32 crc_32[8];
    uint32 xor_fold_buffer[5];
    uint32 layer_record_counter = 0;
    mpls_stack_internal_t mpls_stacks[4];
    uint32 mpls_stack_counter = 0;
    uint8 force_layers[8];

    load_balancing_update_device_constants(hw_cfg->device);
    rv = load_balancing_validate_simulation_inputs(packet_size_byte,headers_info,hw_cfg);
    if(rv != BCM_E_NONE)
    {
        printf("Load balancing simulation validation failed \n");
        return BCM_E_PARAM;
    }

    packet_size = (packet_size_byte + NOF_BYTE_IN_UINT32 - 1 )/ NOF_BYTE_IN_UINT32;

    packet_32 = sal_alloc(packet_size * NOF_BYTE_IN_UINT32, "packet in uint32 units");

    load_balancing_sim_uint8_to_uint32_array(packet,packet_size_byte,packet_32);


    header_buffer = sal_alloc(packet_size * NOF_BYTE_IN_UINT32, "header buffer allocation");
    symmetrical_buffer = sal_alloc(packet_size * NOF_BYTE_IN_UINT32, "symmetrical buffer allocation");
    sal_memset(header_buffer, 0x0, packet_size * NOF_BYTE_IN_UINT32);
    sal_memset(symmetrical_buffer, 0x0, packet_size * NOF_BYTE_IN_UINT32);
    sal_memset(crc_32, 0x0, NOF_LAYERS * NOF_BYTE_IN_UINT32);
    sal_memset(mpls_stacks, 0x0,  sizeof(mpls_stacks));
    sal_memset(force_layers, 0x0,  NOF_LAYERS);

    while((header_offset_iter < headers_info->nof_header_offsets) && (layer_record_counter < NOF_LAYERS))
    {
        uint32 offset_from_header_start = 0;
        if(headers_info->header_offsets[header_offset_iter].flags & LB_SIM_HEADER_OFFSET_USE_PREV_LAYER)
        {
            uint32 prev_header = header_offset_iter - 1;
            layer_record_counter--;
            while((headers_info->header_offsets[prev_header].flags & LB_SIM_HEADER_OFFSET_USE_PREV_LAYER) != 0)
            {
                prev_header--;
            }
            offset_from_header_start = headers_info->header_offsets[header_offset_iter].offset - headers_info->header_offsets[prev_header].offset;
        }

        /*
         * MPLS isn't part of the parser handling and should be skipped
         */
        if(headers_info->header_offsets[header_offset_iter].header_type == LB_SIM_HEADER_MPLS)
        {
            load_balancing_build_mpls_stacks(&header_offset_iter, headers_info, hw_cfg, &layer_record_counter, mpls_stack_counter,
                    mpls_stacks, packet_32, force_layers);
            continue;
        }

        int symmetrical_found = 0;
        int header_start = headers_info->header_offsets[header_offset_iter].offset;
        int next_header = header_offset_iter + 1;
        while((next_header < headers_info->nof_header_offsets) && (headers_info->header_offsets[next_header].flags & LB_SIM_HEADER_OFFSET_USE_PREV_LAYER))
        {
            next_header++;
        }
        int header_end = ((next_header == headers_info->nof_header_offsets) ? packet_size * 32 : headers_info->header_offsets[next_header].offset) -1 ;
        sal_memset(xor_fold_buffer, 0x0, NOF_CLIENTS*NOF_BYTE_IN_UINT32 );
        int field_iter;
        for(field_iter = 0; field_iter < MAX_NOF_FIELDS_OFFSETS;field_iter++)
        {
            field_offset_t field_offset = headers_fields_offsets_info[headers_info->header_offsets[header_offset_iter].header_type][field_iter];
            /** In case the offset is -1 it means that there are no more field to parse in this header */
            if(field_offset.offset == -1)
                break;

            load_balancing_util_copy_range(header_buffer,field_offset.offset, packet_32, headers_info->header_offsets[header_offset_iter].offset + field_offset.offset, field_offset.size);
            if(field_offset.symmetrical_offset != -1)
            {
                load_balancing_util_copy_range(symmetrical_buffer,field_offset.offset, packet_32, headers_info->header_offsets[header_offset_iter].offset + field_offset.symmetrical_offset, field_offset.size);
                symmetrical_found = 1;
            }
        }
        if(symmetrical_found)
        {
            int xor_iter = 0;
            for(xor_iter = 0; xor_iter < (((header_end - header_start)/32) + 1);xor_iter++)
            {
                header_buffer[xor_iter] ^=  symmetrical_buffer[xor_iter];
                symmetrical_buffer[xor_iter] = 0;
            }
        }

        load_balancing_xor_and_fold(header_buffer,(((header_end - header_start)/32) + 1),xor_fold_buffer,offset_from_header_start);

        uint32 seed_value = (headers_info->header_offsets[header_offset_iter].flags & LB_SIM_HEADER_OFFSET_USE_PREV_LAYER) ? 0 : hw_cfg->parser_seed;
        uint32 seed;
        uint32 crc_32_tmp;

        if(hw_cfg->device == DEVICE_TYPE_JERICHO2_B0)
        {
            seed = xor_fold_buffer[0];
            xor_fold_buffer[0] = seed_value;
        }
        else
        {
            seed = seed_value;
        }

        load_balancing_calculate_hash(seed, xor_fold_buffer, HASH_WIDTH_WITH_PADDING, 0xDD22D287, 32, hw_cfg->device, &crc_32_tmp);
        crc_32[layer_record_counter] ^= crc_32_tmp;

/*
 * Clear the laster header bits
 * header_end - header_start + 1 -> the size in bits of the header
 * the + 31 is to round up to a uint32 size
 * the /32 is to get the number of uint32 to clear.
 */
        sal_memset(header_buffer, 0x0,(((header_end - header_start)/32) + 1)*NOF_BYTE_IN_UINT32);

        layer_record_counter++;
        header_offset_iter++;
    }
    while(layer_record_counter < NOF_LAYERS)
    {
        crc_32[layer_record_counter++] = hw_cfg->parser_seed;
    }
    printf("CRC32 = %04x %04x %04x %04x %04x\n", crc_32[0], crc_32[1], crc_32[2], crc_32[3], crc_32[4]);
    load_balancing_sim_update_crc_with_mpls_info(mpls_stacks, hw_cfg, crc_32);
    printf("CRC32 + MPLS = %04x %04x %04x %04x %04x\n", crc_32[0], crc_32[1], crc_32[2], crc_32[3], crc_32[4]);
    load_balancing_sim_flp(headers_info->forwarding_layer, hw_cfg, crc_32, force_layers, lb_keys);

    sal_free(symmetrical_buffer);
    sal_free(header_buffer);
    sal_free(packet_32);
    return rv;
}

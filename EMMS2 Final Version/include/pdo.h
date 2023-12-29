#ifndef PDO_H
#define PDO_H
/* Master 0, Slave 0, "JM1001"
 * Vendor ID:       0x00000532
 * Product code:    0x07125055
 * Revision number: 0x00000007
 */

ec_pdo_entry_info_t slave_0_pdo_entries[] = {
    {0x7000, 0x01, 16}, /* PPI-CW */
    {0x7000, 0x02, 16}, /* ui16_JM_par_data */
    {0x7000, 0x03, 16}, /* ui16_JM_par_addr */
    {0x7000, 0x04, 16}, /* ui16_JM_par_cmd */
    {0x7000, 0x05, 8}, /* ui8_pwr_qlty_00_addr */
    {0x7000, 0x06, 8}, /* ui8_pwr_qlty_01_addr */
    {0x7000, 0x07, 8}, /* ui8_pwr_qlty_02_addr */
    {0x7000, 0x08, 8}, /* ui8_pwr_qlty_03_addr */
    {0x7000, 0x09, 16}, /* ui16_harmonic_addr */
    {0x7000, 0x0a, 16}, /* ui16_pwr_qlty_cmd */
    {0x7000, 0x0b, 16}, /* var_in_1 */
    {0x7000, 0x0c, 16}, /* var_in_2 */
    {0x7000, 0x0d, 16}, /* var_in_3 */
    {0x6000, 0x01, 16}, /* PPI-SW */
    {0x6000, 0x02, 16}, /* in16_i_L1_act */
    {0x6000, 0x03, 16}, /* in16_i_L2_act */
    {0x6000, 0x04, 16}, /* in16_i_L3_act */
    {0x6000, 0x05, 16}, /* in16_i_N_act */
    {0x6000, 0x06, 16}, /* in16_u_L1N_act */
    {0x6000, 0x07, 16}, /* in16_u_L2N_act */
    {0x6000, 0x08, 16}, /* in16_u_L3N_act */
    {0x6000, 0x09, 16}, /* in16_u_L1N_FU_act */
    {0x6000, 0x0a, 16}, /* in16_u_L2N_FU_act */
    {0x6000, 0x0b, 16}, /* in16_u_L3N_FU_act */
    {0x6000, 0x0c, 16}, /* ui16_pwr_qlty_00_data */
    {0x6000, 0x0d, 8}, /* ui8_pwr_qlty_00_addr_fb */
    {0x6000, 0x0e, 8}, /* ui8_pwr_qlty_00_status */
    {0x6000, 0x0f, 16}, /* ui64_pwr_qlty_00_time */
    {0x6000, 0x10, 16}, /* ui16_pwr_qlty_01_data */
    {0x6000, 0x11, 8}, /* ui8_pwr_qlty_01_addr_fb */
    {0x6000, 0x12, 8}, /* ui8_pwr_qlty_01_status */
    {0x6000, 0x13, 16}, /* ui64_pwr_qlty_01_time */
    {0x6000, 0x14, 16}, /* ui16_pwr_qlty_02_data */
    {0x6000, 0x15, 8}, /* ui8_pwr_qlty_02_addr_fb */
    {0x6000, 0x16, 8}, /* ui8_pwr_qlty_02_status */
    {0x6000, 0x17, 16}, /* ui64_pwr_qlty_02_time */
    {0x6000, 0x18, 16}, /* ui16_pwr_qlty_03_data */
    {0x6000, 0x19, 8}, /* ui8_pwr_qlty_03_addr_fb */
    {0x6000, 0x1a, 8}, /* ui8_pwr_qlty_03_status */
    {0x6000, 0x1b, 16}, /* ui64_pwr_qlty_03_time */
    {0x6000, 0x1c, 16}, /* fl32_u_LxN_harmonic */
    {0x6000, 0x1d, 16}, /* fl32_i_LxN_harmonic */
    {0x6000, 0x1e, 16}, /* fl32_p_LxN_harmonic */
    {0x6000, 0x1f, 16}, /* fl32_q_LxN_harmonic */
    {0x6000, 0x20, 8}, /* ui8_harmonic_addr_fb */
    {0x6000, 0x21, 8}, /* ui8_harmonic_status */
    {0x6000, 0x22, 16}, /* ui64_harmonic_time */
    {0x6000, 0x23, 16}, /* ui16_JM_fb_par_data */
    {0x6000, 0x24, 8}, /* ui8_JM_fb_par_addr */
    {0x6000, 0x25, 8}, /* ui8_JM_fb_par_status */
    {0x6000, 0x26, 16}, /* var_out_1 */
    {0x6000, 0x27, 16}, /* var_out_2 */
    {0x6000, 0x28, 16}, /* var_out_3 */
    {0x6000, 0x29, 16}, /* var_out_4 */
    {0x6000, 0x2a, 16}, /* var_out_5 */
};

ec_pdo_info_t slave_0_pdos[] = {
    {0x1600, 13, slave_0_pdo_entries + 0}, /* Outputs */
    {0x1a00, 42, slave_0_pdo_entries + 13}, /* Inputs */
};

ec_sync_info_t slave_0_syncs[] = {
    {0, EC_DIR_OUTPUT, 1, slave_0_pdos + 0, EC_WD_ENABLE},
    {1, EC_DIR_INPUT, 1, slave_0_pdos + 1, EC_WD_ENABLE},
    {0xff}
};

#endif

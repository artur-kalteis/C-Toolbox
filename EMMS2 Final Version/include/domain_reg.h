#ifndef DOMAIN_REG_H
#define DOMAIN_REG_H
int ana_in_count[1]={36}; 
int ana_out_count[1]={11}; 
int dig_in_count[1]={0}; 
int dig_out_count[1]={0}; 
int ana_in[1][40]; 
int ana_out[1][40]; 
int dig_in[1][40]; 
int dig_out[1][40]; 
const static ec_pdo_entry_reg_t domain1_regs[] = { 
/*########Slave 1 ################*/  
{SlavePos1,  JM1001 , 0x6000, 1, &ana_in[0][0]}, 
{SlavePos1,  JM1001 , 0x6000, 2, &ana_in[0][1]}, 
{SlavePos1,  JM1001 , 0x6000, 3, &ana_in[0][2]}, 
{SlavePos1,  JM1001 , 0x6000, 4, &ana_in[0][3]}, 
{SlavePos1,  JM1001 , 0x6000, 5, &ana_in[0][4]}, 
{SlavePos1,  JM1001 , 0x6000, 6, &ana_in[0][5]}, 
{SlavePos1,  JM1001 , 0x6000, 7, &ana_in[0][6]}, 
{SlavePos1,  JM1001 , 0x6000, 8, &ana_in[0][7]}, 
{SlavePos1,  JM1001 , 0x6000, 9, &ana_in[0][8]}, 
{SlavePos1,  JM1001 , 0x6000, 10, &ana_in[0][9]}, 
{SlavePos1,  JM1001 , 0x6000, 11, &ana_in[0][10]}, 
{SlavePos1,  JM1001 , 0x6000, 12, &ana_in[0][11]}, 
{SlavePos1,  JM1001 , 0x6000, 13, &ana_in[0][12]}, 
{SlavePos1,  JM1001 , 0x6000, 14, &ana_in[0][13]}, 
{SlavePos1,  JM1001 , 0x6000, 15, &ana_in[0][14]}, 
{SlavePos1,  JM1001 , 0x6000, 16, &ana_in[0][15]}, 
{SlavePos1,  JM1001 , 0x6000, 17, &ana_in[0][16]}, 
{SlavePos1,  JM1001 , 0x6000, 18, &ana_in[0][17]}, 
{SlavePos1,  JM1001 , 0x6000, 19, &ana_in[0][18]}, 
{SlavePos1,  JM1001 , 0x6000, 20, &ana_in[0][19]}, 
{SlavePos1,  JM1001 , 0x6000, 21, &ana_in[0][20]}, 
{SlavePos1,  JM1001 , 0x6000, 22, &ana_in[0][21]}, 
{SlavePos1,  JM1001 , 0x6000, 23, &ana_in[0][22]}, 
{SlavePos1,  JM1001 , 0x6000, 24, &ana_in[0][23]}, 
{SlavePos1,  JM1001 , 0x6000, 25, &ana_in[0][24]}, 
{SlavePos1,  JM1001 , 0x6000, 26, &ana_in[0][25]}, 
{SlavePos1,  JM1001 , 0x6000, 27, &ana_in[0][26]}, 
{SlavePos1,  JM1001 , 0x6000, 28, &ana_in[0][27]}, 
{SlavePos1,  JM1001 , 0x6000, 29, &ana_in[0][28]}, 
{SlavePos1,  JM1001 , 0x6000, 30, &ana_in[0][29]}, 
{SlavePos1,  JM1001 , 0x6000, 31, &ana_in[0][30]}, 
{SlavePos1,  JM1001 , 0x6000, 32, &ana_in[0][31]}, 
{SlavePos1,  JM1001 , 0x6000, 33, &ana_in[0][32]}, 
{SlavePos1,  JM1001 , 0x6000, 34, &ana_in[0][33]}, 
{SlavePos1,  JM1001 , 0x6000, 35, &ana_in[0][34]}, 
{SlavePos1,  JM1001 , 0x6000, 36, &ana_in[0][35]}, 
{SlavePos1,  JM1001 , 0x7000, 1, &ana_out[0][0]}, 
{SlavePos1,  JM1001 , 0x7000, 2, &ana_out[0][1]}, 
{SlavePos1,  JM1001 , 0x7000, 3, &ana_out[0][2]}, 
{SlavePos1,  JM1001 , 0x7000, 4, &ana_out[0][3]}, 
{SlavePos1,  JM1001 , 0x7000, 5, &ana_out[0][4]}, 
{SlavePos1,  JM1001 , 0x7000, 6, &ana_out[0][5]}, 
{SlavePos1,  JM1001 , 0x7000, 7, &ana_out[0][6]}, 
{SlavePos1,  JM1001 , 0x7000, 8, &ana_out[0][7]}, 
{SlavePos1,  JM1001 , 0x7000, 9, &ana_out[0][8]}, 
{SlavePos1,  JM1001 , 0x7000, 10, &ana_out[0][9]}, 
{SlavePos1,  JM1001 , 0x7000, 11, &ana_out[0][10]}, 
{}
};
#endif

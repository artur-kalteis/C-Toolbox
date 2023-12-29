#!/bin/bash




write_influx_cmd_master()
{
  
  curl -i -XPOST --user 'root:sgcdm2017' 'http://1.100.168.1:8086/write?db=sgcdm' --data-binary " par/2748779 fl32_f_ref=55,fl32_U_re_pos_ref=211,fl32_U_im_pos_ref=0,fl32_U_re_neg_ref=0,fl32_U_im_neg_ref=0,fl32_U_re_zero_ref=0,fl32_U_im_zero_ref=0,fl32_P_pos_ref=0,fl32_Q_pos_ref=0,fl32_P_neg_ref=0,fl32_Q_neg_ref=0,fl32_P_zero_ref=0,fl32_Q_zero_ref=0,fl32_P_Bat_ref=0,fl32_P_Ext_ref=0,fl32_cmd_0=0,fl32_cmd_1=0"
}


write_influx_cmd_master
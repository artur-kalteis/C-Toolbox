#!/bin/bash



write_influx_cmd_topology()
{
  
  curl -i -XPOST --user 'root:sgcdm2017' 'http://192.168.72.139:8086/write?db=ecat_master' --data-binary "pos_0_EL6692 fl32_ana_out_01=1107558400,fl32_ana_out_02=1107558400"

}




write_influx_cmd_topology


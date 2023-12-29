#!/bin/bash




write_influx_cmd_master()
{
  
  curl -i -XPOST --user 'root:sgcdm2017' 'http://192.168.72.139:8086/write?db=ecat_master' --data-binary "pos_0_AB1001 bool_sync_cmd_close_deadbus=0,bool_sync_cmd_close_livebus=0,bool_sync_cmd_open=1"
}


write_influx_cmd_master
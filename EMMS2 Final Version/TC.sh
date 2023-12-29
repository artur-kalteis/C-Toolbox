#!/bin/bash

buffer=0


t_cmd_off=0
t_cmd_on=0
t_cmd_pause=0
t_cmd_reset=0
t_cmd_sleep=0
t_cmd_standby=0
t_cmd_start=0
cmd_topology_machine=0

m_cmd_off=0
m_cmd_on=0
m_cmd_pause=0
m_cmd_reset=0
m_cmd_sleep=0
m_cmd_standby=0
m_cmd_start=0
cmd_master_machine=0




write_influx_cmd_topology()
{
  
  curl -i -XPOST --user 'root:sgcdm2017' 'http://1.100.167.1:8086/write?db=sgcdm' --data-binary "topology_cmd_statemachine int_cmd_topology=${cmd_topology_machine},bool_cmd_topology_start=${t_cmd_start},bool_cmd_topology_standby=${t_cmd_standby},bool_cmd_topology_sleep=${t_cmd_sleep},bool_cmd_topology_reset=${t_cmd_reset},bool_cmd_topology_pause=${t_cmd_pause},bool_cmd_topology_on=${t_cmd_on},bool_cmd_topology_off=${t_cmd_off}"
}

write_influx_cmd_master()
{
  
  curl -i -XPOST --user 'root:sgcdm2017' 'http://1.100.167.1:8086/write?db=sgcdm' --data-binary "master_cmd_statemachine int_cmd_master=${cmd_master_machine},bool_cmd_master_start=${m_cmd_start},bool_cmd_master_standby=${m_cmd_standby},bool_cmd_master_sleep=${m_cmd_sleep},bool_cmd_master_reset=${m_cmd_reset},bool_cmd_master_pause=${m_cmd_pause},bool_cmd_master_on=${m_cmd_on},bool_cmd_master_off=${m_cmd_off}"
}

menue()
{
echo "-------------------------------------" 
printf "Topology-State-Machine => t_state=[1-8] \n" 
printf "Topology-cmd_off       => t_off  \n" 
printf "Topology-cmd_on        => t_on  \n" 
printf "Topology-cmd_pause     => t_pause  \n" 
printf "Topology-cmd_reset     => t_reset  \n" 
printf "Topology-cmd_sleep     => t_sleep  \n" 
printf "Topology-cmd_standby   => t_standby  \n" 
printf "Topology-cmd_start     => t_start  \n"  
echo "-------------------------------------" 
printf "Master-State-Machine   => m_state=[1-3] \n" 
printf "Master-cmd_off         => m_off  \n" 
printf "Master-cmd_on          => m_on  \n" 
printf "Master-cmd_pause       => m_pause  \n" 
printf "Master-cmd_reset       => m_reset  \n" 
printf "Master-cmd_sleep       => m_sleep  \n" 
printf "Master-cmd_standby     => m_standby  \n" 
printf "Master-cmd_start       => m_start  \n"  
echo "-------------------------------------" 
echo "Eingabe= " 
}



influx_controller()
{
clear
menue

while true
do
read cmd



if [ "$cmd" == "t_off" ]
    then
t_cmd_off=1
t_cmd_on=0
t_cmd_pause=0
t_cmd_reset=0
t_cmd_sleep=0
t_cmd_standby=0
t_cmd_start=0
buffer=1
    
fi

if [ "$cmd" == "m_off" ]
    then
m_cmd_off=1
m_cmd_on=0
m_cmd_pause=0
m_cmd_reset=0
m_cmd_sleep=0
m_cmd_standby=0
m_cmd_start=0
buffer=2
    
fi

if [ "$cmd" == "t_on" ]
    then
t_cmd_off=0
t_cmd_on=1
t_cmd_pause=0
t_cmd_reset=0
t_cmd_sleep=0
t_cmd_standby=0
t_cmd_start=0
buffer=1
    
fi
if [ "$cmd" == "m_on" ]
    then
m_cmd_off=0
m_cmd_on=1
m_cmd_pause=0
m_cmd_reset=0
m_cmd_sleep=0
m_cmd_standby=0
m_cmd_start=0
buffer=2
    
fi

if [ "$cmd" == "t_pause" ]
    then
t_cmd_off=0
t_cmd_on=0
t_cmd_pause=1
t_cmd_reset=0
t_cmd_sleep=0
t_cmd_standby=0
t_cmd_start=0
buffer=1
    
fi

if [ "$cmd" == "m_pause" ]
    then
m_cmd_off=0
m_cmd_on=0
m_cmd_pause=1
m_cmd_reset=0
m_cmd_sleep=0
m_cmd_standby=0
m_cmd_start=0
buffer=2
    
fi

if [ "$cmd" == "t_reset" ]
    then
t_cmd_off=0
t_cmd_on=0
t_cmd_pause=0
t_cmd_reset=1
t_cmd_sleep=0
t_cmd_standby=0
t_cmd_start=0
buffer=1
    
fi
if [ "$cmd" == "m_reset" ]
    then
m_cmd_off=0
m_cmd_on=0
m_cmd_pause=0
m_cmd_reset=1
m_cmd_sleep=0
m_cmd_standby=0
m_cmd_start=0
buffer=2
    
fi


if [ "$cmd" == "t_sleep" ]
    then
t_cmd_off=0
t_cmd_on=0
t_cmd_pause=0
t_cmd_reset=0
t_cmd_sleep=1
t_cmd_standby=0
t_cmd_start=0
buffer=1
    
fi
if [ "$cmd" == "m_sleep" ]
    then
m_cmd_off=0
m_cmd_on=0
m_cmd_pause=0
m_cmd_reset=0
m_cmd_sleep=1
m_cmd_standby=0
m_cmd_start=0
buffer=2
    
fi


if [ "$cmd" == "t_standby" ]
    then
t_cmd_off=0
t_cmd_on=0
t_cmd_pause=0
t_cmd_reset=0
t_cmd_sleep=0
t_cmd_standby=1
t_cmd_start=0
buffer=1
    
fi

if [ "$cmd" == "m_standby" ]
    then
m_cmd_off=0
m_cmd_on=0
m_cmd_pause=0
m_cmd_reset=0
m_cmd_sleep=0
m_cmd_standby=1
m_cmd_start=0
buffer=2
    
fi



if [ "$cmd" == "t_start" ]
    then
t_cmd_off=0
t_cmd_on=0
t_cmd_pause=0
t_cmd_reset=0
t_cmd_sleep=0
t_cmd_standby=0
t_cmd_start=1
buffer=1
    
fi

if [ "$cmd" == "m_start" ]
    then
m_cmd_off=0
m_cmd_on=0
m_cmd_pause=0
m_cmd_reset=0
m_cmd_sleep=0
m_cmd_standby=0
m_cmd_start=1
buffer=2
    
fi


if [ "$cmd" == "t_state=4" ]
then
cmd_topology_machine=4 
buffer=1   
fi

if [ "$cmd" == "t_state=5" ]
then
cmd_topology_machine=5   
buffer=1 
fi

if [ "$cmd" == "t_state=1" ]
then
cmd_topology_machine=1  
buffer=1 
fi

if [ "$cmd" == "m_state=1" ]
then
cmd_master_machine=1    
buffer=2
fi

if [ "$cmd" == "m_state=2" ]
then
cmd_master_machine=2    
buffer=2
fi

if [ "$cmd" == "m_state=3" ]
then
cmd_master_machine=3   
buffer=2
fi



if [ ${buffer} -eq 1 ]
then
write_influx_cmd_topology
fi

if [ ${buffer} -eq 2 ]
then
write_influx_cmd_master

fi


clear

menue 

done
}




influx_controller


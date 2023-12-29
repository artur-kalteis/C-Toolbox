#!/bin/bash


working_path="/root/ethercat/EMMS2/"

archiv_path="${working_path}archiv/"
temporary_path="${working_path}tmp/"
library_path="${working_path}lib/"
include_path="${working_path}include/"
compiled_path="${working_path}bin/"
config_path="${working_path}config/"

read_config()
{
#Parameter werden aus der Config ausgelesen und ohne '\n' in einer Variable zwischengespeichert  
master_id=`head -n1 ${config_path}tm_config.cfg | tail -n1 | cut -c11-`
master_id=`echo $master_id | sed s/'\r'//g`

arangodb_ip=`head -n2 ${config_path}tm_config.cfg | tail -n1 | cut -c13-`
arangodb_ip=`echo $arangodb_ip | sed s/'\r'//g`
arangodb_user=`head -n3 ${config_path}tm_config.cfg | tail -n1 | cut -c15-`
arangodb_user=`echo $arangodb_user | sed s/'\r'//g`
arangodb_password=`head -n4 ${config_path}tm_config.cfg | tail -n1 | cut -c19-`
arangodb_password=`echo $arangodb_password | sed s/'\r'//g`
arangodb_database=`head -n5 ${config_path}tm_config.cfg | tail -n1 | cut -c19-`
arangodb_database=`echo $arangodb_database | sed s/'\r'//g`

influxdb_ip=`head -n6 ${config_path}tm_config.cfg | tail -n1 | cut -c13-`
influxdb_ip=`echo $influxdb_ip | sed s/'\r'//g`
influxdb_user=`head -n7 ${config_path}tm_config.cfg | tail -n1 | cut -c15-`
influxdb_user=`echo $influxdb_user | sed s/'\r'//g`
influxdb_password=`head -n8 ${config_path}tm_config.cfg | tail -n1 | cut -c19-`
influxdb_password=`echo $influxdb_password | sed s/'\r'//g`
influxdb_database=`head -n9 ${config_path}tm_config.cfg | tail -n1 | cut -c19-`
influxdb_database=`echo $influxdb_database | sed s/'\r'//g`


#echo $influxdb_database | tr 'm' 'HH' # this works
#influxdb_database=`echo $influxdb_database | tr '\r' ''`

#influxdb_database=`echo $influxdb_database | sed s/'\r'//g` # this works

}

read_config


#Globale Variablen
#unterschiedliche Versionen der JM slaves
JM1001buffer1=0 # buffer verhindert doppelten deklaration eines slaves im slaves.h
JM1001buffer2=0
JM1001buffer3=0
JM1001buffer4=0

topology_ok=1 # Für den Test ob die Topologie die ausgelesen wurde und die aus der ArangoDB gleich sind
error=0 # kann beim kompilieren entstehen
##################### Aus ArangoDB






# cmd variablen Master

m_cmd_off=0
m_cmd_on=0
m_cmd_pause=0
m_cmd_reset=0
m_cmd_sleep=0
m_cmd_standby=0
m_cmd_start=0
cmd_master_machine=0

# Status variablen Master

m_sts_start=0
m_sts_standby=0
m_sts_sleep=0
m_sts_reset=0
m_sts_pause=0
m_sts_on=0
m_sts_off=0
sts_master_machine=0


# cmd variablen Topology_manager

cmd_off=0
cmd_on=1
cmd_pause=0
cmd_reset=0
cmd_sleep=0
cmd_standby=1
cmd_start=0
cmd_topology_machine=0

# Status variablen Topology_manager

sts_start=0
sts_standby=1
sts_sleep=0
sts_reset=0
sts_pause=0
sts_on=1
sts_off=0
sts_topology_machine=1


compile_parser()
{

cp ${archiv_path}topology_arango_parser.c ${compiled_path}topology_arango_parser.c
cp ${archiv_path}topology_influx_parser.c ${compiled_path}topology_influx_parser.c
cp ${archiv_path}get_all_device_ids_arango_parser.c ${compiled_path}get_all_device_ids_arango_parser.c

gcc ${compiled_path}topology_arango_parser.c -o ${compiled_path}topology_arango_parser -lcurl -ljson-c
gcc ${compiled_path}topology_influx_parser.c -o ${compiled_path}topology_influx_parser -lcurl -ljson-c
gcc ${compiled_path}get_all_device_ids_arango_parser.c -o ${compiled_path}get_all_device_ids_arango_parser -lcurl -ljson-c

}



main()
{


echo "compiling parser..."
compile_parser

echo "write_influx_master_init..."
write_influx_master_init  

echo "write_influx_topology_init..."
write_influx_topology_init

echo "read_arangodb_topology..."
read_arangodb_topology

echo "get_all_device_ids..."
get_all_device_ids



while true
do
echo "-------------------------------------------------"
printf "$(tput setaf 1)$(tput setab 7)State= %i  $(tput sgr 0)\n" ${sts_topology_machine}
#echo "ST $(tput setab 7)and white background$(tput sgr 0)"

read_cmd_topology_influxdb



if [ ${cmd_standby} -eq 1 ]
then
sts_start=0
sts_standby=1
sts_sleep=0
sts_reset=0
sts_pause=0
sts_on=1
sts_off=0
echo "Mode= standby"
write_sts_topology_influxdb
fi


if [ ${cmd_off} -eq 1 ]
then
sts_start=0
sts_standby=0
sts_sleep=0
sts_reset=0
sts_pause=0
sts_on=0
sts_off=1
printf "Mode=off \n"
write_sts_topology_influxdb
exit 0
fi









if [ ${cmd_start} -eq 1 ]
then
sts_start=1
sts_standby=0
sts_sleep=0
sts_reset=0
sts_pause=0
sts_on=1
sts_off=0  
printf "Mode=start \n"
write_sts_topology_influxdb
###########################
if [ ${sts_topology_machine} -lt 4 ]
then 
#Erster Start

if [ ${cmd_topology_machine} -eq 1 ]
then
sts_topology_machine=1
cmd_topology_machine=0
write_cmd_topology_influxdb
write_sts_topology_influxdb
fi


echo "Datenbank auslesen"
####Download Topology/Scan Slave Topology / compare Topology upload  topology/
compare_topology
if [ ${topology_ok} -eq 1 ] # wenn die topologie übereinstimmt
then

sts_topology_machine=2 #Status okay und wird an arangoDB gesendet

write_sts_topology_influxdb

else
sts_topology_machine=3 #Status nicht okay und wird an arangoDB gesendet
write_sts_topology_influxdb

fi
fi
###########################

if [  ${cmd_topology_machine} -eq 4 ]
then
#start_compile topologie okay
sts_topology_machine=4
cmd_topology_machine=0
write_cmd_topology_influxdb
write_sts_topology_influxdb

echo "Master wird Kompiliert und gestartet"
cp ${archiv_path}ecat_master.c ${compiled_path}ecat_master.c
cp ${archiv_path}control.c ${compiled_path}control.c


domain_reg
#shared_memory
save_slaves_topology
#cp /root/ethercat/test/ecrt_dummy/uploader.c /root/ethercat/test/uploader.c
#cp /root/ethercat/test/ecrt_dummy/control.c /root/ethercat/test/control.c
#cp /root/ethercat/test/ecrt_dummy/write_arango.sh /root/ethercat/test/write_arango.sh
echo "compiling.."
gcc ${compiled_path}ecat_master.c -o ${compiled_path}ecat_master -I/opt/etherlab/include /opt/etherlab/lib/libethercat.a -lrt -lcurl -ljson-c
if [ $? -ne 0 ]
then
error=1
fi

#rm -r ${compiled_path}ecat_master.c
gcc ${compiled_path}control.c -o ${compiled_path}control -I/opt/etherlab/include /opt/etherlab/lib/libethercat.a -lrt -lcurl -ljson-c
if [ $? -ne 0 ]
then
error=1
fi
#rm -r ${compiled_path}control.c
#gcc  ${working_path}control.c -o ${working_path}control.a -lrt -lcurl -ljson-c
#if [ $? -ne 0 ]
#then
#error=1
#fi

if [ ${error} -eq 1 ]
then

echo "Compiling ERROR"
sts_topology_machine=7

else

#starte alle programme
echo "System Start"
#müssen in dieser Reihenfolge gestartet werden

#echo "Uploader wird gestartet"
#${working_path}uploader.a & #das & startet einen seperaten Thread und ist notwendig, um die Shared-Memory Blöcke seperat Starten zu können
echo "Master wird gestartet"
chmod +x bin/ecat_master

${compiled_path}ecat_master &

sleep 3

echo "Control wird gestartet"
${compiled_path}control &
sts_topology_machine=6
fi
write_sts_topology_influxdb

fi


if [  ${cmd_topology_machine} -eq 5 ]
then
echo "Master Update wird durchgeführt!!!!"
sts_topology_machine=5
cmd_topology_machine=0
write_cmd_topology_influxdb
write_sts_topology_influxdb
echo "Master Update wurde durchgeführt!!!!"
sts_topology_machine=1
write_sts_topology_influxdb
fi

if [  ${sts_topology_machine} -eq 6 ]
then

compare_topology
if [  ${topology_ok} -ne 1 ] # wenn die Topologie nicht übereinstimmt
then

#master wird beendet

sts_topology_machine=7 # zurück zum start

write_sts_topology_influxdb

fi
fi


if [  ${sts_topology_machine} -eq 7 ]
then
printf "Topologie ERROR .....\n"
write_sts_topology_influxdb
fi

if [  ${cmd_topology_machine} -eq 8 ]
then

sts_topology_machine=8 # zurück zum start
cmd_topology_machine=0

sts_start=0
sts_standby=0
sts_sleep=0
sts_reset=0
sts_pause=0
sts_on=0
sts_off=1

write_cmd_topology_influxdb
write_sts_topology_influxdb
printf "Topologiemanger stop .....\n"
exit 0

fi

fi # ende von if [ ${cmd_start} -eq 1 ] 


sleep 5

#read_arangodb_state # Status aus ArangoDB für neu compilieren oder übernehmen
done

}

#arangodb_ip=`head -n2 tm_config.cfg | tail -n1 | cut -c12-`
#arangodb_user=`head -n3 tm_config.cfg | tail -n1 | cut -c14-`
#arangodb_password=`head -n4 tm_config.cfg | tail -n1 | cut -c18-`
#arangodb_database=`head -n5 tm_config.cfg | tail -n1 | cut -c18-`

#influxdb_ip=`head -n6 tm_config.cfg | tail -n1 | cut -c12-`
#influxdb_user=`head -n7 tm_config.cfg | tail -n1 | cut -c14-`
#influxdb_password=`head -n8 tm_config.cfg | tail -n1 | cut -c18-`
#influxdb_database=`head -n9 tm_config.cfg | tail -n1 | cut -c18-`


write_influx_master_init()  #bleibt erstmal statisch
{

 # curl -i -XPOST --user 'root:sgcdm2017' 'http://192.168.72.139:8086/write?db=ecat_master' --data-binary "master_sts_statemachine int_sts_master=${sts_master_machine},bool_sts_master_start=${m_sts_start},bool_sts_master_standby=${m_sts_standby},bool_sts_master_sleep=${m_sts_sleep},bool_sts_master_reset=${m_sts_reset},bool_sts_master_pause=${m_sts_pause},bool_sts_master_on=${m_sts_on},bool_sts_master_off=${m_sts_off}"
  
 # curl -i -XPOST --user 'root:sgcdm2017' 'http://192.168.72.139:8086/write?db=ecat_master' --data-binary "master_cmd_statemachine int_cmd_master=${cmd_master_machine},bool_cmd_master_start=${m_cmd_start},bool_cmd_master_standby=${m_cmd_standby},bool_cmd_master_sleep=${m_cmd_sleep},bool_cmd_master_reset=${m_cmd_reset},bool_cmd_master_pause=${m_cmd_pause},bool_cmd_master_on=${m_cmd_on},bool_cmd_master_off=${m_cmd_off}"
  
  curl="curl -i -XPOST --user 'flag1:flag2' 'http://flag3/write?db=flag4' --data-binary 'master_sts_statemachine flag5'"

values="int_sts_master=${sts_master_machine},bool_sts_master_start=${m_sts_start},bool_sts_master_standby=${m_sts_standby},bool_sts_master_sleep=${m_sts_sleep},bool_sts_master_reset=${m_sts_reset},bool_sts_master_pause=${m_sts_pause},bool_sts_master_on=${m_sts_on},bool_sts_master_off=${m_sts_off}"
#String wird zusammen gebaut
curl=`echo $curl | sed s/'flag1'/${influxdb_user}/g`
curl=`echo $curl | sed s/'flag2'/${influxdb_password}/g`
curl=`echo $curl | sed s/'flag3'/${influxdb_ip}/g`
curl=`echo $curl | sed s/'flag4'/${influxdb_database}/g`
curl=`echo $curl | sed s/'flag5'/${values}/g`

echo $curl > tmp/write_influx_master_init


curl="curl -i -XPOST --user 'flag1:flag2' 'http://flag3/write?db=flag4' --data-binary 'master_cmd_statemachine flag5'"

values="int_cmd_master=${cmd_master_machine},bool_cmd_master_start=${m_cmd_start},bool_cmd_master_standby=${m_cmd_standby},bool_cmd_master_sleep=${m_cmd_sleep},bool_cmd_master_reset=${m_cmd_reset},bool_cmd_master_pause=${m_cmd_pause},bool_cmd_master_on=${m_cmd_on},bool_cmd_master_off=${m_cmd_off}"
curl=`echo $curl | sed s/'flag1'/${influxdb_user}/g`
curl=`echo $curl | sed s/'flag2'/${influxdb_password}/g`
curl=`echo $curl | sed s/'flag3'/${influxdb_ip}/g`
curl=`echo $curl | sed s/'flag4'/${influxdb_database}/g`
curl=`echo $curl | sed s/'flag5'/${values}/g`

echo $curl >> tmp/write_influx_master_init
chmod +x tmp/write_influx_master_init
./tmp/write_influx_master_init



}



write_influx_topology_init()
{

#curl -i -XPOST --user 'root:sgcdm2017' 'http://192.168.72.139:8086/write?db=ecat_master' --data-binary "topology_sts_statemachine int_sts_topology=${sts_topology_machine},bool_sts_topology_start=${sts_start},bool_sts_topology_standby=${sts_standby},bool_sts_topology_sleep=${sts_sleep},bool_sts_topology_reset=${sts_reset},bool_sts_topology_pause=${sts_pause},bool_sts_topology_on=${sts_on},bool_sts_topology_off=${sts_off}"
  
  curl="curl -i -XPOST --user 'flag1:flag2' 'http://flag3/write?db=flag4' --data-binary 'topology_sts_statemachine flag5'"

values="int_sts_topology=${sts_topology_machine},bool_sts_topology_start=${sts_start},bool_sts_topology_standby=${sts_standby},bool_sts_topology_sleep=${sts_sleep},bool_sts_topology_reset=${sts_reset},bool_sts_topology_pause=${sts_pause},bool_sts_topology_on=${sts_on},bool_sts_topology_off=${sts_off}"
curl=`echo $curl | sed s/'flag1'/${influxdb_user}/g`
curl=`echo $curl | sed s/'flag2'/${influxdb_password}/g`
curl=`echo $curl | sed s/'flag3'/${influxdb_ip}/g`
curl=`echo $curl | sed s/'flag4'/${influxdb_database}/g`
curl=`echo $curl | sed s/'flag5'/${values}/g`

echo $curl > tmp/write_influx_topology_init

  
  
#curl -i -XPOST --user 'root:sgcdm2017' 'http://192.168.72.139:8086/write?db=ecat_master' --data-binary "topology_cmd_statemachine int_cmd_topology=${cmd_topology_machine},bool_cmd_topology_start=${cmd_start},bool_cmd_topology_standby=${cmd_standby},bool_cmd_topology_sleep=${cmd_sleep},bool_cmd_topology_reset=${cmd_reset},bool_cmd_topology_pause=${cmd_pause},bool_cmd_topology_on=${cmd_on},bool_cmd_topology_off=${cmd_off}"

curl="curl -i -XPOST --user 'flag1:flag2' 'http://flag3/write?db=flag4' --data-binary 'topology_cmd_statemachine flag5'"

values="int_cmd_topology=${cmd_topology_machine},bool_cmd_topology_start=${cmd_start},bool_cmd_topology_standby=${cmd_standby},bool_cmd_topology_sleep=${cmd_sleep},bool_cmd_topology_reset=${cmd_reset},bool_cmd_topology_pause=${cmd_pause},bool_cmd_topology_on=${cmd_on},bool_cmd_topology_off=${cmd_off}"
curl=`echo $curl | sed s/'flag1'/${influxdb_user}/g`
curl=`echo $curl | sed s/'flag2'/${influxdb_password}/g`
curl=`echo $curl | sed s/'flag3'/${influxdb_ip}/g`
curl=`echo $curl | sed s/'flag4'/${influxdb_database}/g`
curl=`echo $curl | sed s/'flag5'/${values}/g`

echo $curl >> tmp/write_influx_topology_init
chmod +x tmp/write_influx_topology_init
./tmp/write_influx_topology_init




}

write_cmd_topology_influxdb()
{

 # curl -i -XPOST --user 'root:sgcdm2017' 'http://192.168.72.139:8086/write?db=ecat_master' --data-binary "topology_cmd_statemachine int_cmd_topology=${cmd_topology_machine},bool_cmd_topology_start=${cmd_start},bool_cmd_topology_standby=${cmd_standby},bool_cmd_topology_sleep=${cmd_sleep},bool_cmd_topology_reset=${cmd_reset},bool_cmd_topology_pause=${cmd_pause},bool_cmd_topology_on=${cmd_on},bool_cmd_topology_off=${cmd_off}"




curl="curl -i -XPOST --user 'flag1:flag2' 'http://flag3/write?db=flag4' --data-binary 'topology_cmd_statemachine flag5'"

values="int_cmd_topology=${cmd_topology_machine},bool_cmd_topology_start=${cmd_start},bool_cmd_topology_standby=${cmd_standby},bool_cmd_topology_sleep=${cmd_sleep},bool_cmd_topology_reset=${cmd_reset},bool_cmd_topology_pause=${cmd_pause},bool_cmd_topology_on=${cmd_on},bool_cmd_topology_off=${cmd_off}"
curl=`echo $curl | sed s/'flag1'/${influxdb_user}/g`
curl=`echo $curl | sed s/'flag2'/${influxdb_password}/g`
curl=`echo $curl | sed s/'flag3'/${influxdb_ip}/g`
curl=`echo $curl | sed s/'flag4'/${influxdb_database}/g`
curl=`echo $curl | sed s/'flag5'/${values}/g`

echo $curl > tmp/write_cmd_topology_influxdb
chmod +x tmp/write_cmd_topology_influxdb
./tmp/write_cmd_topology_influxdb


}


write_sts_topology_influxdb()
{

  #flag1="${influxdb_database}\' \"--data-binary topology_sts_statemachine int_sts_topology=${sts_topology_machine},bool_sts_topology_start=${sts_start},bool_sts_topology_standby=${sts_standby},bool_sts_topology_sleep=${sts_sleep},bool_sts_topology_reset=${sts_reset},bool_sts_topology_pause=${sts_pause},bool_sts_topology_on=${sts_on},bool_sts_topology_off=${sts_off}"

  #echo "#!/bin/bash" > write_influx.sh
  #echo "curl -i -XPOST --user 'root:sgcdm2017' 'http://192.168.72.139:8086/write?db=flag_1" >> write_influx.sh
  #chmod +x write_influx.sh

  #flag_1="flag_1"
  #sed -i "s/$flag_1/$flag1/" write_influx.sh


  #####influxdb_database=`echo $influxdb_database | sed s/'\r'//g`

curl="curl -i -XPOST --user 'flag1:flag2' 'http://flag3/write?db=flag4' --data-binary 'topology_sts_statemachine flag5'"

values="int_sts_topology=${sts_topology_machine},bool_sts_topology_start=${sts_start},bool_sts_topology_standby=${sts_standby},bool_sts_topology_sleep=${sts_sleep},bool_sts_topology_reset=${sts_reset},bool_sts_topology_pause=${sts_pause},bool_sts_topology_on=${sts_on},bool_sts_topology_off=${sts_off}"
curl=`echo $curl | sed s/'flag1'/${influxdb_user}/g`
curl=`echo $curl | sed s/'flag2'/${influxdb_password}/g`
curl=`echo $curl | sed s/'flag3'/${influxdb_ip}/g`
curl=`echo $curl | sed s/'flag4'/${influxdb_database}/g`
curl=`echo $curl | sed s/'flag5'/${values}/g`

echo $curl > tmp/write_sts_topology_influxdb
chmod +x tmp/write_sts_topology_influxdb
./tmp/write_sts_topology_influxdb

}

read_cmd_topology_influxdb()
{

./bin/topology_influx_parser ${influxdb_ip} ${influxdb_user} ${influxdb_password} ${influxdb_database} ${master_id}

sleep 1

cmd_off=`head -n1 ${temporary_path}influx_parameter.cfg | tail -n1`
cmd_on=`head -n2 ${temporary_path}influx_parameter.cfg | tail -n1`
cmd_pause=`head -n3 ${temporary_path}influx_parameter.cfg | tail -n1`
cmd_reset=`head -n4 ${temporary_path}influx_parameter.cfg | tail -n1`
cmd_sleep=`head -n5 ${temporary_path}influx_parameter.cfg | tail -n1`
cmd_standby=`head -n6 ${temporary_path}influx_parameter.cfg | tail -n1`
cmd_start=`head -n7 ${temporary_path}influx_parameter.cfg | tail -n1`
cmd_topology_machine=`head -n8 ${temporary_path}influx_parameter.cfg | tail -n1`


#printf "topology_machine %i \n" ${cmd_topology_machine}
#printf "start %i \n" ${cmd_start}
#printf "standby %i \n" ${cmd_standby}
#printf "sleep %i \n" ${cmd_sleep}
#printf "reset %i \n" ${cmd_reset}
#printf "pause Ausgabe %i \n" ${cmd_pause}
#printf "on %i \n" ${cmd_on}
#printf "off %i \n" ${cmd_off}

}


read_arangodb_topology()
{


./bin/topology_arango_parser ${arangodb_ip} ${arangodb_user} ${arangodb_password} ${arangodb_database} ${master_id}

sleep 1

anzahl=`cat ${temporary_path}arango_parameter.cfg | wc -l`  # wird benötigt zum abspeichern der Eingänge/Ausgänge

slave_count=`head -n1 ${temporary_path}arango_parameter.cfg | tail -n1` # Die Anzahl der Slaves wird zwischengespeicehrt

z=2

slave_anzahl=1
s_position=1  # um die arrays richtig zu befüllen

while [ ${z} -ne `expr $anzahl + 1` ] # Solange Variablen existieren
do

i=1
while [ ${i} -lt 10 ]
do
s=`head -n${z} ${temporary_path}arango_parameter.cfg | tail -n1`  #Wert wird eingelesen
#printf "HHHHHHHHHiiiiiiiiiieeeeeeeeeeeerrrrrrrrrrrrrrrr %s \n" ${s}
if [ ${i} -eq 1 ]
then
s_serial[${s_position}]=${s}
#printf "id %i \n" ${s_id[${s_position}]}
fi

if [ ${i} -eq 2 ]
then
s_posi[${s_position}]=${s}
#printf "position %i \n" ${s_posi[${s_position}]}
fi

if [ ${i} -eq 3 ]
then
s_name[${s_position}]=${s}
#printf "nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnname %s \n" ${s_name[${s_position}]}
fi

if [ ${i} -eq 4 ]
then
dig_in[${s_position}]=${s}
#printf "dig_in %i \n" ${dig_in[${s_position}]}
fi

if [ ${i} -eq 5 ]
then
dig_out[${s_position}]=${s}
#printf "dig_out %i \n" ${dig_out[${s_position}]}
fi

if [ ${i} -eq 6 ]
then
ana_in[${s_position}]=${s}
#printf "ana_in %i \n" ${ana_in[${s_position}]}
fi

if [ ${i} -eq 7 ]
then
ana_out[${s_position}]=${s}
#printf "ana_out %i \n" ${ana_out[${s_position}]}
fi

if [ ${i} -eq 8 ]
then
if [ ${s} == "true" ] # is the Bool true a
then
return_value=1
else
return_value=0
fi

s_serialized[${s_position}]=${return_value}
#printf "serialized %s \n" ${s_serialized[${s_position}]}
fi

if [ ${i} -eq 9 ]
then
s_datasets[${s_position}]=${s}
#printf "datasets %i \n" ${s_datasets[${s_position}]}
fi

i=`expr $i + 1`	
z=`expr $z + 1`	
	
done
slave_anzahl=`expr $z + 1`
s_position=`expr $s_position + 1`
done

}


get_all_device_ids()
{


./bin/get_all_device_ids_arango_parser ${arangodb_ip} ${arangodb_user} ${arangodb_password} ${arangodb_database} ${slave_count}

sleep 1

anzahl=`cat ${temporary_path}all_device_ids.cfg | wc -l`  # wird benötigt zum abspeichern der Eingänge/Ausgänge

z=1

while [ ${z} -ne `expr $anzahl + 1` ] # Solange Variablen existieren
do

s_id[${z}]=`head -n${z} ${temporary_path}all_device_ids.cfg | tail -n1`  #Wert wird eingelesen

z=`expr $z + 1`	
	
done

}




domain_reg()
{
echo "#ifndef DOMAIN_REG_H" > ${include_path}domain_reg.h
echo "#define DOMAIN_REG_H" >> ${include_path}domain_reg.h
#cp ${working_path}ecat_master.c ${compiled_path}ecat_master.c

############################################# Erstellt Anzahl der Eingänge/ausgänge
printf "int ana_in_count[%i]={" ${slave_count} >> ${include_path}domain_reg.h

b=1
while [ ${b} -ne `expr ${slave_count} + 1` ]
do

if [ ${b} -eq ${slave_count} ]
then
printf "%i}; \n" ${ana_in[${b}]} >> ${include_path}domain_reg.h

else
printf "%i," ${ana_in[${b}]} >> ${include_path}domain_reg.h

fi

b=`expr $b + 1`
done

printf "int ana_out_count[%i]={" ${slave_count} >> ${include_path}domain_reg.h

b=1
while [ ${b} -ne `expr ${slave_count} + 1` ]
do

if [ ${b} -eq ${slave_count} ]
then
printf "%i}; \n" ${ana_out[${b}]} >> ${include_path}domain_reg.h

else
printf "%i," ${ana_out[${b}]} >> ${include_path}domain_reg.h

fi

b=`expr $b + 1`
done



printf "int dig_in_count[%i]={" ${slave_count} >> ${include_path}domain_reg.h

b=1
while [ ${b} -ne `expr ${slave_count} + 1` ]
do

if [ ${b} -eq ${slave_count} ]
then
printf "%i}; \n" ${dig_in[${b}]} >> ${include_path}domain_reg.h

else
printf "%i," ${dig_in[${b}]} >> ${include_path}domain_reg.h

fi

b=`expr $b + 1`
done



printf "int dig_out_count[%i]={" ${slave_count} >> ${include_path}domain_reg.h

b=1
while [ ${b} -ne `expr ${slave_count} + 1` ]
do

if [ ${b} -eq ${slave_count} ]
then
printf "%i}; \n" ${dig_out[${b}]} >> ${include_path}domain_reg.h

else
printf "%i," ${dig_out[${b}]} >> ${include_path}domain_reg.h

fi

b=`expr $b + 1`
done


#slave namen
printf "char slave_name[%i][64]={" ${slave_count} >> ${include_path}domain_reg.h

b=1
while [ ${b} -ne `expr ${slave_count} + 1` ]
do

if [ ${b} -eq ${slave_count} ]
then
printf '"%s"}; \n' ${s_name[${b}]} >> ${include_path}domain_reg.h

else
printf '"%s",' ${s_name[${b}]} >> ${include_path}domain_reg.h

fi

b=`expr $b + 1`
done


###################################################################################


# jeder slave bekommt erstmal 24 Ausgänge oder Eingänge
printf "int ana_in[%i][%i]; \n" ${slave_count} 40 >> ${include_path}domain_reg.h
printf "int ana_out[%i][%i]; \n" ${slave_count} 40 >> ${include_path}domain_reg.h
printf "int dig_in[%i][%i]; \n" ${slave_count} 40 >> ${include_path}domain_reg.h
printf "int dig_out[%i][%i]; \n" ${slave_count} 40 >> ${include_path}domain_reg.h


echo "const static ec_pdo_entry_reg_t domain1_regs[] = { " >> ${include_path}domain_reg.h
   
i=1 

while [ ${i} -ne `expr ${slave_count} + 1` ] 	
do

printf "/*########Slave %i ################*/  \n" $i >> ${include_path}domain_reg.h


######################Analog Input
b=1
while [ ${b} -ne  `expr ${ana_in[${i}]} + 1` ] 	
do

#Bridge
printf "{SlavePos%i,  %s , 0x6000, %i, &ana_in[%i][%i]}, \n" $i ${s_name[${i}]} $b `expr $i - 1` `expr $b - 1` >> ${include_path}domain_reg.h

#JM
#printf "{SlavePos%i,  %s , 0x6000, %i, &ana_in[%i][%i]}, \n" $i ${s_name[${i}]} $b `expr $i - 1` `expr $b - 1` >> ${include_path}domain_reg.h
#Beckhoff
#printf "{SlavePos%i,  %s , 0x310%i, 2, &ana_in[%i][%i]}, \n" $i ${s_name[${i}]} $b `expr $i - 1` `expr $b - 1` >> ${include_path}domain_reg.h

b=`expr $b + 1`
done
######################Analog Output
b=1
while [ ${b} -ne  `expr ${ana_out[${i}]} + 1` ] 	
do

#Bridge
printf "{SlavePos%i,  %s , 0x7000, %i, &ana_out[%i][%i]}, \n" $i ${s_name[${i}]} $b `expr $i - 1` `expr $b - 1` >> ${include_path}domain_reg.h

#JM
#printf "{SlavePos%i,  %s , 0x7000, %i, &ana_out[%i][%i]}, \n" $i ${s_name[${i}]} $b `expr $i - 1` `expr $b - 1` >> ${include_path}domain_reg.h
#Beckhoff
#printf "{SlavePos%i,  %s , 0x300%i, 1, &ana_out[%i][%i]}, \n" $i ${s_name[${i}]} $b `expr $i - 1` `expr $b - 1` >> ${include_path}domain_reg.h

b=`expr $b + 1`
done
######################Digital Input
b=1
while [ ${b} -ne  `expr ${dig_in[${i}]} + 1` ] 	
do

printf "{SlavePos%i,  %s , 0x3100, %i, &dig_in[%i][%i]}, \n" $i ${s_name[${i}]} $b `expr $i - 1` `expr $b - 1` >> ${include_path}domain_reg.h

b=`expr $b + 1`
done
######################Digital Output
b=1
while [ ${b} -ne  `expr ${dig_out[${i}]} + 1` ] 	
do

printf "{SlavePos%i,  %s , 0x3000, %i, &dig_out[%i][%i]}, \n" $i ${s_name[${i}]} $b `expr $i - 1` `expr $b - 1` >> ${include_path}domain_reg.h

b=`expr $b + 1`
done

######################

c=`expr $i - 1`

   string_cut=`echo ${s_name[${i}]} | cut -c1-2` #Für die Überprüfung ob es  ein JM, El oder koppler ist


  if [ ${string_cut} == "EK" ]
  then

  string="sc = ecrt_master_slave_config(master, SlavePos${i}, EK1100);if (!sc) {return -1;} FLAG_XY"


  else

if [ ${i} -eq ${slave_count} ] # beim letzten slave darf der FLAG_XY nicht mehr dabei sein
then # wenn der letzte Slave dran ist dann wird der Flag nicht mehr benötigt
string="if (!(sc_ana_in= ecrt_master_slave_config(master,SlavePos${i}, ${s_name[${i}]}))) {return -1;} if(ecrt_slave_config_pdos(sc_ana_in, EC_END, slave_${c}_syncs)) {return -1;}"
else
string="if (!(sc_ana_in= ecrt_master_slave_config(master,SlavePos${i}, ${s_name[${i}]}))) {return -1;} if(ecrt_slave_config_pdos(sc_ana_in, EC_END, slave_${c}_syncs)) {return -1;} FLAG_XY"
fi


fi # end of if [ ${string_cut} == "EK" ]

flag="FLAG_XY" # der Flaf markiert den Ort in basic.c wo der Inhalt eingefügt wird

sed -i "s/$flag/$string/" ${compiled_path}ecat_master.c



i=`expr $i + 1`	


done	
echo "{}" >> ${include_path}domain_reg.h
echo "};" >> ${include_path}domain_reg.h
	
echo "#endif" >> ${include_path}domain_reg.h

}


save_slaves_topology() 
{	

echo "#ifndef PDO_H" > ${include_path}pdo.h
echo "#define PDO_H" >> ${include_path}pdo.h


i=0
while [ ${i} -ne ${slave_count} ]
do


if [ ${s_name[`expr ${i} + 1`]} == "EL6692" ]
then
echo '
/* Master 0, Slave 0, "EL6692"
 * Vendor ID:       0x00000002
 * Product code:    0x1a243052
 * Revision number: 0x00140002
 */

ec_pdo_entry_info_t slave_0_pdo_entries[] = { ' >> ${include_path}pdo.h

# add the ana_out
out_count=0
while [ ${out_count} -ne ${ana_out[`expr ${i} + 1`]}  ]
do


# the 0x0%x is importnt
printf '{0x7000, 0x0%x, 32}, \n' `expr $out_count + 1` >> ${include_path}pdo.h
out_count=`expr $out_count + 1`
done


printf "{0x10f4, 0x02, 2},
    {0x0000, 0x00, 6},
    {0x0000, 0x00, 3},
    {0x1800, 0x09, 1},
    {0x1800, 0x07, 1},
    {0x10f4, 0x0e, 1},
    {0x10f4, 0x0f, 1},
    {0x10f4, 0x10, 1}, \n" >> ${include_path}pdo.h


# add the ana_in
in_count=0
while [ ${in_count} -ne ${ana_in[`expr ${i} + 1`]}  ]
do

printf '{0x6000, 0x0%x, 32}, \n' `expr $in_count + 1` >> ${include_path}pdo.h
in_count=`expr $in_count + 1`
done


printf '};

ec_pdo_info_t slave_0_pdos[] = {
    {0x1600, %i, slave_0_pdo_entries + 0}, /* RxPDO-Map */
    {0x1a01, 8, slave_0_pdo_entries + %i}, /* TxPDO-Map External Sync Compact */
    {0x1a00, %i, slave_0_pdo_entries + %i}, /* TxPDO-Map */
};

ec_sync_info_t slave_0_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, slave_0_pdos + 0, EC_WD_DISABLE},
    {3, EC_DIR_INPUT, 2, slave_0_pdos + 1, EC_WD_DISABLE},
    {0xff}
}; \n' ${ana_out[`expr ${i} + 1`]} ${ana_out[`expr ${i} + 1`]} ${ana_in[`expr ${i} + 1`]} $((${ana_out[`expr ${i} + 1`]}+8)) >> ${include_path}pdo.h


else

  ethercat cstruct -p${i} >> ${include_path}pdo.h
fi



  i=`expr $i + 1`
done


echo "#endif" >> ${include_path}pdo.h   





echo "#ifndef SLAVES_H" > ${include_path}slaves.h
echo "#define SLAVES_H" >> ${include_path}slaves.h
printf "int slave_count=%i; \n" ${slave_count} >> ${include_path}slaves.h

printf "#define slave_X %i // For Measurement Buffer\n" ${slave_count} >> ${include_path}slaves.h

printf "int slave_datasets[%i]={%i" ${slave_count} ${s_datasets[1]} >> ${include_path}slaves.h

i=2
while [ ${i} -ne `expr ${slave_count} + 1` ]
do
printf ",%i" ${s_datasets[${i}]} >> ${include_path}slaves.h

  i=`expr $i + 1`
done

printf "}; \n" >> ${include_path}slaves.h

# slave id's
#s_id

printf 'char slave_id[%i][64]={"%s"' ${slave_count} ${s_id[1]} >> ${include_path}slaves.h

i=2
while [ ${i} -ne `expr ${slave_count} + 1` ]
do
printf ',"%s"' ${s_id[${i}]} >> ${include_path}slaves.h

  i=`expr $i + 1`
done

printf "}; \n" >> ${include_path}slaves.h


# slave serial

printf 'char slave_serial[%i][64]={"%s"' ${slave_count} ${s_serial[1]} >> ${include_path}slaves.h

i=2
while [ ${i} -ne `expr ${slave_count} + 1` ]
do
printf ',"%s"' ${s_serial[${i}]} >> ${include_path}slaves.h

  i=`expr $i + 1`
done

printf "}; \n" >> ${include_path}slaves.h


## is Slave serialized


s_serialized[${s_position}]=${s}
printf "serialized %s \n" ${s_serialized[${s_position}]}


printf "int slave_serilized[%i]={%i" ${slave_count} ${s_serialized[1]} >> ${include_path}slaves.h

i=2
while [ ${i} -ne `expr ${slave_count} + 1` ]
do
printf ",%i" ${s_serialized[${i}]} >> ${include_path}slaves.h

  i=`expr $i + 1`
done

printf "}; \n" >> ${include_path}slaves.h


i=1

while [ ${i} -ne  `expr $slave_count + 1` ]
do

  string_cut=`echo ${s_name[${i}]} | cut -c1-2` #Für die Überprüfung ob es  ein JM, El oder koppler ist





if [ ${string_cut} == "EK" ] # EtherCAT-Koppler von Beckhoff erkannt
then
printf "//---------------------------------------------\n"  >> ${include_path}slaves.h

g=`expr $i - 1`

printf '#define SlavePos1  0, %i \n' ${g}   >> ${include_path}slaves.h


echo "#define EK1100 0x00000002, 0x044c2c52" >> ${include_path}slaves.h


printf "//---------------------------------------------\n"  >> ${include_path}slaves.h

fi

if [ ${string_cut} == "JM" ] # EtherCAT-Koppler von Beckhoff erkannt
then
printf "//---------------------------------------------\n"  >> ${include_path}slaves.h

printf "#define SlavePos%i  0, %i \n" ${i} `expr $i - 1`  >> ${include_path}slaves.h


printf "#define %s %s, %s \n" ${s_name[${i}]} ${vendor_id[${i}]} ${product_code[${i}]} >> ${include_path}slaves.h


printf "//---------------------------------------------\n"  >> ${include_path}slaves.h

fi


if [ ${string_cut} == "EL" ] # EtherCAT-Slave von Beckhoff erkannt
then

printf "#define SlavePos%i  0, %i \n" ${i} `expr $i - 1`  >> ${include_path}slaves.h

#vendor_id[${b}]=`ethercat cstruct -p${i} | grep Vendor | cut -c20-30`
#product_code[${b}]=`ethercat cstruct -p${i} | grep Product | cut -c20-30`

printf "#define %s %s, %s \n" ${s_name[${i}]} ${vendor_id[${i}]} ${product_code[${i}]} >> ${include_path}slaves.h


fi

if [ ${string_cut} == "AB" ] # EtherCAT-Slave von Beckhoff erkannt
then

printf "#define SlavePos%i  0, %i \n" ${i} `expr $i - 1`  >> ${include_path}slaves.h

#vendor_id[${b}]=`ethercat cstruct -p${i} | grep Vendor | cut -c20-30`
#product_code[${b}]=`ethercat cstruct -p${i} | grep Product | cut -c20-30`

printf "#define %s %s, %s \n" ${s_name[${i}]} ${vendor_id[${i}]} ${product_code[${i}]} >> ${include_path}slaves.h


fi


	
  i=`expr $i + 1`
done

echo "#endif" >> ${include_path}slaves.h


}












compare_topology()
{
buffer=0
topology_ok=1 
beckhoff_koppler=0
i=0


while true
do

 
  if [ ${i} -gt "9" ]	# -gt	greater than
	then

slave1=`ethercat cstruct -p${i} | grep Master | cut -c25-30`

#works for beckhoff 	  slave1=`ethercat slaves -p${i} | cut -c21-26`
	else
#works for beckhoff 		slave1=`ethercat slaves -p${i} | cut -c19-25`
  slave1=`ethercat cstruct -p${i} | grep Master | cut -c24-29`
	fi



if [ -z "$slave1" ]
then
break
fi



if [ ${slave1} == "EK1100" ]
then
beckhoff_koppler=1
fi


test[${i}]=${slave1}
i=`expr $i + 1`

done

#printf "buffer %i   \n" ${i}
buffer=${i}

#if [ ${beckhoff_koppler} == 1 ]  #ein counter muss abgezogen werden da der koppler mit eingerechnet wird
#then  

#buffer=`expr $buffer - 1`

#i=1

#else
#i=0
#fi

i=0
b=1

########################################################################################
#vendor id ethercat cstruct -p1 | grep Vendor | cut -c20-30
#product code ethercat cstruct -p1 | grep Product | cut -c20-30

#printf "buffer eingelsen %i   \n" ${buffer}

#printf "buffer arango %i   \n" ${slave_count}

if [ ${buffer} -ne ${slave_count} ]
then
printf "Buffer not match\n"
topology_ok=0

else


while [ "$b" -le "$buffer" ] 		#-le	less than or equal to
do

  vendor_id[${b}]=`ethercat cstruct -p${i} | grep Vendor | cut -c20-30`
  product_code[${b}]=`ethercat cstruct -p${i} | grep Product | cut -c20-30`

#printf "------------------------------------------------------------------\n"
#printf "ausgeleseer slave  %s ----------------\n" ${test[${b}]}
#printf "arangodb slave  %s ----------------\n" ${s_name[${b}]}
#printf "------------------------------------------------------------------\n"

if [ ${test[${i}]} != ${s_name[${b}]} ]
then

echo ${test[${i}]}
echo ${s_name[${b}]}

printf "Slave not match \n"
topology_ok=0

fi


i=`expr $i + 1`
b=`expr $b + 1`


done

fi



if [ ${topology_ok} -eq 1 ]
then

printf "Topology match \n"

else

printf "Topology not Match \n"

fi

}




main




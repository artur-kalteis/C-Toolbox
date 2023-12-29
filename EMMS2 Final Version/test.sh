#!/bin/bash




fetch_all_app_ids()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://1.100.167.1:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" : "For a in app Filter a.type == 'ethercat_master' For sp IN ANY a belongs_to OPTIONS {bfs: true, uniqueVertices: 'global'} return sp._id"
}
EOF

}


#fetch_all_app_ids




fetch_app_par()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://1.100.167.1:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" : "For d in par Filter d.name == 'master' return d"
}
EOF

}


#fetch_app_par




fetch_device_attribute()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://1.100.167.1:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" : "For a in device Filter a.name == 'Device_AIB_1' For sp IN ANY a belongs_to OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('par',sp._id) return sp"
}
EOF

}

#fetch_device_attribute

fetch_all_slave_ids()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://1.100.167.1:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" : "For d in device Filter d.ethercat_slave == true return d._id"
}
EOF

}

#fetch_all_slave_ids



fetch_all_slave_ids_in_order()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://192.168.72.237:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" : "For a in device Filter a.type == 'controller' for b in inbound a included_in OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('interface', b._id) For sp IN outbound b included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp1 in outbound sp included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp2 in inbound sp1 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} For sp3 IN outbound sp2 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp4 in outbound sp3 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp5 in inbound sp4 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} For sp6 IN outbound sp5 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp7 in outbound sp6 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp8 in inbound sp7 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} limit 1 return {'1':sp1._id ,'2':sp4._id ,'3':sp7._id}"
    
}
EOF

}



fetch_all_1_id()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://1.100.167.1:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" :  "For a in device Filter a.type == 'controller' for b in inbound a included_in OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('interface', b._id) For sp IN outbound b included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp1 in outbound sp included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp2 in inbound sp1 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} limit 1 return {'1':sp1._id }"
}
EOF

}

fetch_all_1_id()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://192.168.72.237:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" :  "For a in device Filter a.type == 'controller' for b in inbound a included_in OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('interface', b._id) For sp IN outbound b included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp1 in outbound sp included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp2 in inbound sp1 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} limit 1 return {'1':sp1._id }"
}
EOF

}


fetch_all_2_id()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://192.168.72.237:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" :  "For a in device Filter a.type == 'controller' for b in inbound a included_in OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('interface', b._id) For sp IN outbound b included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp1 in outbound sp included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp2 in inbound sp1 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} For sp3 IN outbound sp2 included_in OPTIONS {bfs: true, uniqueVertices: 'global'}For sp4 IN outbound sp3 included_in OPTIONS {bfs: true, uniqueVertices: 'global'}For sp5 IN outbound sp4 included_in OPTIONS {bfs: true, uniqueVertices: 'global'}limit 1 return {'1':sp1._id ,'2':sp4._id}"
}
EOF

}


fetch_all_3_id()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://192.168.72.237:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" :  "For a in device Filter a.type == 'controller' for b in inbound a included_in OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('interface', b._id) For sp IN outbound b included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp1 in outbound sp included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp2 in inbound sp1 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} For sp3 IN outbound sp2 included_in OPTIONS {bfs: true, uniqueVertices: 'global'}For sp4 IN outbound sp3 included_in OPTIONS {bfs: true, uniqueVertices: 'global'}For sp5 IN outbound sp4 included_in OPTIONS {bfs: true, uniqueVertices: 'global'}For sp6 IN outbound sp5 included_in OPTIONS {bfs: true, uniqueVertices: 'global'}For sp7 IN outbound sp6 included_in OPTIONS {bfs: true, uniqueVertices: 'global'}For sp8 IN outbound sp7 included_in OPTIONS {bfs: true, uniqueVertices: 'global'}limit 1 return {'1':sp1._id ,'2':sp4._id,'3':sp7._id}"

}
EOF

}


fetch_all_1_id()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://192.168.72.237:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" :  " For a in device Filter a.type == 'controller' for b in inbound a included_in OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('interface', b._id) For sp IN outbound b included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp1 in outbound sp included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp2 in inbound sp1 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} limit 1 return {'1':sp1._id }"
}
EOF

}


get_all_device_ids()
{


#./bin/get_all_device_ids_arango_parser ${arangodb_ip} ${arangodb_user} ${arangodb_password} ${arangodb_database} ${slave_count}

sleep 1

#anzahl=`cat ${temporary_path}all_device_ids.cfg | wc -l`  # wird benötigt zum abspeichern der Eingänge/Ausgänge

anzahl=`cat tmp/all_device_ids.cfg | wc -l`

z=1

while [ ${z} -ne `expr $anzahl + 1` ] # Solange Variablen existieren
do

#s_id[${z}]=`head -n${z} ${temporary_path}all_device_ids.cfg | tail -n1`  #Wert wird eingelesen
s_id[${z}]=`head -n${z} tmp/all_device_ids.cfg | tail -n1`  #Wert wird eingelesen
printf "HHHHHHHHHiiiiiiiiiieeeeeeeeeeeerrrrrrrrrrrrrrrr %s \n" ${s_id[${z}]}

z=`expr $z + 1`	
	
done

}

###################################test


fetch_state_id()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://1.100.167.1:8529/_db/sgcdm/_api/cursor <<EOF
{
    "query" : "For a in device Filter a.type == 'controller' for b in inbound a included_in OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('interface', b._id) For sp IN inbound b connected_to OPTIONS {bfs: true, uniqueVertices: 'global'} for sp1 in outbound sp included_in OPTIONS {bfs: true, uniqueVertices: 'global'} for sp2 in inbound sp1 included_in OPTIONS {bfs: true, uniqueVertices: 'global'} limit 1 return sp1._id"
}
EOF

}





printf 'char slave_serial[%i][64]={"%s"' ${slave_count} ${s_serial[1]} >> ${include_path}slaves.h

i=2
while [ ${i} -ne `expr ${slave_count} + 1` ]
do
printf ',"/%s"' ${s_serial[${i}]} >> ${include_path}slaves.h

  i=`expr $i + 1`
done

printf "}; \n" >> ${include_path}slaves.h



get_all_collections_from_device()
{


echo "rm -r /root/ethercat/EMMS2/tmp/all_collection_from_device.cfg"

i=1

while [ ${i} -ne `expr ${slave_count} + 1` ]
do

./bin/get_all_collections_from_device${arangodb_ip} ${arangodb_user} ${arangodb_password} ${arangodb_database}  ${s_id[${i}]}

i=`expr $i + 1`

done


i=1
i2=1

while [ ${i} -ne `expr ${slave_count} + 1` ]
do






i=`expr $i + 1`
i2=`expr $i2 + 1`

done



}



fetch_all_app_ids()
{
curl -X POST --data @- --basic --user "root:sgcdm2017" http://1.100.168.1:8529/_db/sgcdm/_api/cursor <<EOF
{
 "query" : "For a in device Filter a._id == 'device/2747873' For sp IN ANY a belongs_to OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('par',sp._id) return sp"
}
EOF

}








write_influx_cmd_master()
{
  
  curl -i -G --user 'root:sgcdm2017' 'http://192.168.72.139:8086/query?pretty=true' --data-urlencode "db=ecat_master" --data-urlencode "q= SELECT last(*) from  pos_7_AB1001"
}
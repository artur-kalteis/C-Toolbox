#!/bin/bash

service ethercat stop

rm -r -f /etc/init.d/ethercat
rm -r -f /opt/etherlab/etc/init.d/ethercat
rm -r -f /opt/etherlab/etc/sysconfig/ethercat
rm -r -f /opt/etherlab/bin/ethercat
rm -r -f /etc/rc1.d/K01ethercat
rm -r -f /etc/rc0.d/K01ethercat
rm -r -f /etc/rc2.d/K01ethercat
rm -r -f /etc/init.d/ethercat
rm -r -f /etc/sysconfig/ethercat
rm -r -f /etc/rc6.d/K01ethercat
rm -r -f /etc/rc5.d/S02ethercat
rm -r -f /etc/rc3.d/S02ethercat
rm -r -f /lib/modules/3.0.95-rt124/ethercat
rm -r -f /run/systemd/generator.late/runlevel5.target.wants/ethercat.service
rm -r -f /run/systemd/generator.late/runlevel3.target.wants/ethercat.service
rm -r -f /run/systemd/generator.late/ethercat.service
rm -r -f /lib/modules/3.0.95-rt124/ethercat
rm -r -f /lib/modules/3.16.0-4-686-pae/ethercat
rm -r -f /usr/bin/ethercat
rm -r -f /opt/etherlab/etc/ethercat.conf
rm -r -f /opt/etherlab/sbin/ethercatctl
rm -r -f /opt/etherlab/lib/systemd/system/ethercat.service
rm -r -f /lib/modules/3.16.0-4-rpi/ethercat

rm -r -f /run/systemd/generator.late/graphical.target.wants/ethercat.service
rm -r -f /run/systemd/generator.late/multi-user.target.wants/ethercat.service
rm -r -f /lib/modules/4.9.0-4-amd64/ethercat
rm -r -f /lib/modules/4.9.0-rt1/ethercat
rm -r -f /opt/etherlab/include/ethercat_wrapper_slave.h
rm -r -f /opt/etherlab/include/ethercat_wrapper.h

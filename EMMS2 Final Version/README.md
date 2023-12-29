# IgH-EtherCAT-Master

Currently this project is using the latest version IgH-Ethercat-Master 1.5.2
and is running on a Debian 8 destribution with a 3.0.95-rt124 PREEMT kernel

## Setup The IgH-EtherCAT-Master


```sh
$ wget https://github.com/synapticon/Etherlab_EtherCAT_Master/releases/download/v1.5.2-sncn-5/EtherCAT_Master-1.5.2-sncn-5.zip

$ unzip EtherCAT_Master-1.5.2-sncn-5.zip

./configure --disable-8139too --enable-e100 --enable-r8169

$  apt-get install linux-headers-$(uname -r) build-essential

$ make ethercatMaster

$ make ethercatMasterInstall

$ make ethercatMasterInstallWithAutoStart

$ service ethercat start

```

## Start The EtherCAT-Measurmement-Management-System (EMMS)

The config tm_config.cfg in the directory /root/git/sgcc_ecatmaster/config contains the database connections. This connections needet to be adjusted.

```sh
$ cd /root/git/sgcc_ecatmaster

#Start the Topology-Manager
$ ./TM.sh
#Start the Topology-Controller
$ ./TC.sh

```



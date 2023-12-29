#!/bin/bash
#Erstellt von Artur Kalteis
#am 28.05.2018
#artur.kalteis@gmx.net
#
#Das Script installiert auf dem Fitlet2 alle Bibliotheken die fÃ¼r die Installatation vom IGH Ethercat master notwendig sind.
#AuÃŸerdem wird der Master selbstÃ¤ndig runter geladen und in das Verzeichnis /root/ethercat installiert.


        apt-get update

        yes "" | apt-get upgrade

        yes "" | apt-get install linux-source 

        yes "" | apt-get install libncurses-dev

        yes "" | apt-get install linux-headers-$(uname -r)

        yes "" | apt-get install kernel-package

        yes "" | apt-get install build-essential 

        yes "" | apt-get install bc

        yes "" | apt-get install libcurl4-gnutls-dev

        yes "" | apt-get libcurl4-openssl-dev

        yes "" | apt-get libcurl4-nss-dev

        yes "" | apt-get libjsoncpp-dev 
        
        yes "" | apt-get libjson-c-dev

        yes "" | apt-get install module-init-tools

        cd /root

        mkdir ethercat

        cd ethercat

        wget https://github.com/synapticon/Etherlab_EtherCAT_Master/releases/download/v1.5.2-sncn-5/EtherCAT_Master-1.5.2-sncn-5.zip

        unzip EtherCAT_Master-1.5.2-sncn-5.zip

        cd sncn_installer
	
	sed -i "s/sudo//" Makefile  # remove all sudo's

	make ethercatMasterInstallWithAutoStart




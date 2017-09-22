#!/bin/bash
sudo cat /dev/null > /etc/network/interfaces && sudo echo -e "auto eth0 \niface eth0 inet dhcp" > /etc/network/interfaces
sudo ifdown eth0
sudo ifup eth0
#exit 0

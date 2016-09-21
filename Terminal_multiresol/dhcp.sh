#!/bin/bash
INTER_DIR=/etc/network
ROOT_UID=0
if [ "$UID" -ne "$ROOT_UID" ];then
  echo "Must be root to do it"
  exit 1
fi
cd $INTER_DIR ||
{
   echo "The dir $INTER_DIR is not exist"
   exit 1
}

sudo cat /dev/null > interfaces && sudo echo -e "auto lo \niface lo inet loopback \nauto eth0 \niface eth0 inet dhcp" > interfaces
sudo /etc/init.d/networking restart
#sudo ifdown eth0
#sudo ifup eth0
#sudo rmmod r8169
#sudo insmod /lib/modules/4.4.0-34-generic/kernel/drivers/net/ethernet/realtek/r8169.ko
exit 0

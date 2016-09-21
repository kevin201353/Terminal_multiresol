#!/bin/bash
tmp=""
if [ $1 = $tmp -a $2 = $tmp -a $3 = $tmp ]
then
    sudo echo "#dns" > /etc/resolv.conf
    cd /etc/resolvconf/resolv.conf.d
    sudo echo "#dns" > tail
else
    sudo echo -e "nameserver $1 \nnameserver $2 \nnameserver $3" > /etc/resolv.conf
    cd /etc/resolvconf/resolv.conf.d
    sudo echo -e "nameserver $1 \nnameserver $2 \nnameserver $3" > tail
fi
sudo chmod +x tail
exit 0

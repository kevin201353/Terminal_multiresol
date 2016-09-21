#! /bin/bash

default_route=$(sudo ip route show)
default_interface=$(sudo echo $default_route | sed -e 's/^.*dev \([^ ]*\).*$/\1/' | head -n 1)
address=$(ip addr show label $default_interface scope global | awk '$1 == "inet" { print $2,$4}')

#ip address
ip=$(sudo echo $address | awk '{print $1 }')
ip=${ip%%/*}

#broadcast
broadcast=$(sudo echo $address | awk '{print $2 }')

#mask address
mask=$(sudo route -n | grep 'U[ \t]' | head -n 1 | awk '{print $3}')

#gateway address
gateway=$(sudo route -n | grep 'UG[ \t]' | awk '{print $2}')

#dns
dns=$(sudo cat /etc/resolv.conf | grep nameserver | awk '{print $2}')

#echo ip:$ip,mask:$mask,broadcast:$broadcast,gateway:$gateway,dns:$dns

sudo echo -e "$ip\n$mask\n$gateway\n$dns" > nettmp.txt
exit 0

#!/bin/bash

wget -q --spider http://google.com

if [ $? -eq 0 ]; then
    echo "Online"
    cd /home/pi/lampi
    git pull --rebase
else
    echo "Offline"
    wpa_cli wps-pbc
fi

#!/bin/sh
SSHPASS="1"
sshpass -p $SSHPASS ssh root@10.42.0.1 'systemctl start robot'
./station-main
sshpass -p $SSHPASS ssh root@10.42.0.1 'systemctl stop robot'

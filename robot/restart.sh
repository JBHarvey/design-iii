SSHPASS="1"
sshpass -p $SSHPASS ssh root@10.42.0.1 'systemctl stop robot && sleep 2 && systemctl start robot'

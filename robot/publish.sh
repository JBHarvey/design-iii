git commit -am "Flash Robot"
wifi-switch design3robot
sleep 4


BRANCH=$(git rev-parse --abbrev-ref HEAD)
SSHPASS="1"
sshpass -p $SSHPASS git push --force root@10.42.0.1:/home/design3/robot/git $BRANCH
sshpass -p $SSHPASS ssh root@10.42.0.1 "cd /home/design3/robot/ && rm -rf robot && git clone --depth=1 -b $BRANCH file:///home/design3/robot/git robot && cd robot/robot && sh deploy.sh"
sshpass -p $SSHPASS ssh root@10.42.0.1 'systemctl restart robot'

sudo cp fancontrol /usr/bin
sudo cp fancontrol.service /etc/systemd/system
sudo systemctl enable fancontrol
sudo systemctl start fancontrol


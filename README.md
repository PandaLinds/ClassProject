# ClassProject
self-localization with mapping of devices in a system, as well as message sending.

This system was built and tested on a Raspberry Pi 3b+, using the operating system Raspbian GNU/Linux 9

For enabling localizaiton with GPS:

#Initial setup
1. Edit /boot/config.txt and add "enable_uart=1" at the end
2. Edit /boot/cmdline.txt and remove "console=serial0,115200"
3. systemctl enable ssh
4. sudo apt-get install gpsd libgps-dev libasound2 libasound2-dev ffmpeg libbluetooth-dev
5. Edit /etc/rc.local and add “chmod a+w /sys/class/leds/led1/brightness” before the last line
6. Edit /etc/default/gpsd and add "/dev/serial0" to DEVICES
7. cd ~/Desktop ; git clone https://github.com/PandaLinds/ClassProject/

#Build
1. *make* compiles the localization, map, server, and client.

#run
1. type "./localization" to run the localization process.
2. type Ctrl+C to end the process.

#Troubleshooting
*Check if GPS daemon is holding the tcp port:*
sudo netstat -anp|grep 2947

*Start/Stop GPS daemon:*
sudo systemctl stop gpsd
sudo systemctl start gpsd
sudo systemctl disable gpsd
sudo systemctl enable gpsd

*GPS not acquiring: stop/kill the GPS service and then run manually to see what's happening:*
gpsd -n -N -D 2 -S 6666 /dev/serial0

*List ALSA devices:*
aplay -L

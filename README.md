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
2. *make -f MakefileTest* compiles localization with its unit test.
3. *make -f MakefileMapTest* compiles map with its unit test.

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



Testing:
In order for this to work, localization and map must compile on the machine.

#Build
1. *make -f MakefileTest* compiles localization with its unit test.
2. *make -f MakefileMapTest* compiles map with its unit test.
3. *make -f Makefile2* to compile map and localization into a test driver.

#run
1. type *./maptest* and enter promped information to run tests.
2. type *./locationTest* to run the localization module with spoofed data.
3. type *./module* to run the localization and mapping concurrently 

#Client startup
1. type "hostname" in the terminal and then use that as the input for ./inet_client2 [hostname] 
Note: This only works if it is run on the same machine as the server. Otherwise, use the hostname of the server's machine.

#Server startup
1. To use the default port (54321), start the server with just ./inet_server
2. To use a custom port, start with ./inet_server [port]

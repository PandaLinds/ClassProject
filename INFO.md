# Wireless Sensor Network for Acoustic Sensors

## About
This project is a Wireless Sensor Network which tracks the location of each node. 
Additionally, it enables the use of an accoustic and motion detection. Both sensors passively collect data which must be reviewed after tests. 

## Layout of the Project
The main file holds all of the code needed for MonitorCode and ComMan. The Makefile in the main file will compile the files needed for MonitorCode and ComMan. ComMan's Makefile will compile its code and pull the files it needs from the main file. MonitorCode's Makefile will similarly compile its code and pull the files it needs from the main file. OctTest is the latest test performed, this code can be deleted.

## Outstanding issues
- Commenting is not to coding standards.
- There are some uses of "magic" numbers in module code.
- Client, server, and monitor currently does not integrate. Temperary static code is in MonitorCode and ComMan until it can be pulled from the Client and Server .cpp files in the main file.
- Server code is able to accept client code and log, but client code can not send data. 
- Monitor is not fully developed, but can function on its own. 
- There is some error handling to localization, but it is underdeveloped, and will need perfecting.  
- Adding explicit destructors helped with the memory leak, but there may still be issues with this.

## Future Goals
- Fix outstanding issues
- Make the .hpp and .cpp files in the main file into a library.
- Make sensors update the Communication Manager when something is detected
- Have the Communication Manager send data to a centeralized computer.
- Expand to have multiple Communication Managers monitor 1km radius while sending data to the centeralized computer. 
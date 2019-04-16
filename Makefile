#*******************************************************************************
#
# Team 5
#
# Makefile for Class Project
#
# Tested on Linux  2.4.2-2 #1 Sun Apr 8 20:41:30 EDT 2001 i686 unknown
#    change
#
#*******************************************************************************

CC = g++

INCLUDES =-I.

CFLAGS = -Wall

GPSFLAGS = `pkg-config --cflags --libs libgps`

LDFLAGS = -L /usr/lib/

LIBS = -lm 

#all: localization inet_server inet_client
all: localization

localization: localization.o
	$(CC) $(GPSFLAGS) $(LDFLAGS) $(LIBS) $^ -o $@
 
inet_server: inet_server.o
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@
	
inet_clent: inet_clent.o
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@


.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean :
	echo *.o *.i *.s *~ \#*# core .#* .new* inet_server inet_client
	rm -f *.o *.i *.s *~ \#*# core .#* .new* inet_server inet_client

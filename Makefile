#*******************************************************************************
#
# Sam Siewert
#
# Makefile for Unix-based serial test tool
#
# Tested on Linux  2.4.2-2 #1 Sun Apr 8 20:41:30 EDT 2001 i686 unknown
#
#*******************************************************************************

CC = g++

INCLUDES =-I.

CFLAGS = -Wall

LDFLAGS = 

LIBS = -lm libgps

all: inet_client inet_server localization

localization: localization.o
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@
 
inet_server: inet_server.o
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@
	
inet_clent: inet_clent.o
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@


.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean :
	echo *.o *.i *.s *~ \#*# core .#* .new* inet_server inet_client
	rm -f *.o *.i *.s *~ \#*# core .#* .new* inet_server inet_client

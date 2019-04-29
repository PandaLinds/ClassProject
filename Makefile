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

CC = gcc

INCLUDES =
CFLAGS = 
LDFLAGS = 
LIBS = -lm 

all: inet_server inet_client2

inet_server: inet_server.o
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@
	
inet_clent2: inet_clent2.o
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@


.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean :
	echo *.o *.i *.s *~ \#*# core .#* .new* inet_server inet_client2 localization map
	rm -f *.o *.i *.s *~ \#*# core .#* .new* inet_server inet_client2 localization map

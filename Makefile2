INCLUDE_DIRS = 
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= `pkg-config --cflags --libs libgps`
GPSFLAGS = `pkg-config --cflags --libs libgps`
LDFLAGS = -L /usr/lib/
LIBS = -lm 
MONLIBS = -pthread

TARGET=monitor

HFILES= localization.hpp map.hpp acousticEm.hpp
CFILES= localization.cpp map.cpp acousticEm.cpp monitor.cpp

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.cpp=.o}

all:	${TARGET}

clean:
	-rm -f *.o *.NEW *~
	-rm -f ${TARGET} ${DERIVED} ${GARBAGE}

${TARGET}:	${OBJS}
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(MONLIBS)

depend:

.cpp.o:
	$(CC) $(CFLAGS) -c $<

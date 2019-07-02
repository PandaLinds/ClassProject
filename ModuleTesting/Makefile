INCLUDE_DIRS = 
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= -Wall
GPSFLAGS = `pkg-config --cflags --libs libgps`
LDFLAGS = -L /usr/lib/
LLIBS = -lm 
MLIBS = 
ALIBS = -lm

LDRIVER=locationTest
MDRIVER=mapTest
ADRIVER=acousticTest

LHFILES= localization.hpp
MHFILES=  map.hpp
AHFILES=  acousticEm.hpp

LCFILES= locationTest.cpp localization.cpp
MCFILES= map.cpp maptest.cpp
ACFILES= acousticEm.cpp acousticTest.cpp

LSRCS= ${LHFILES} ${LCFILES}
MSRCS= ${MHFILES} ${MCFILES}
ASRCS= ${MHFILES} ${ACFILES}

LOBJS= ${LCFILES:.cpp=.o}
MOBJS= ${MCFILES:.cpp=.o}
AOBJS= ${ACFILES:.cpp=.o}

all:	${LDRIVER} ${MDRIVER} ${ADRIVER} 

clean:
	-rm -f *.o *.NEW *~
	-rm -f ${LDRIVER} ${MDRIVER} ${ADRIVER} ${DERIVED} ${GARBAGE}

${LDRIVER}:	${LOBJS}
	$(CC) $(LDFLAGS) $(CFLAGS) $(GPSFLAGS) -o $@ $(LOBJS) $(LLIBS)

${MDRIVER}:	${MOBJS}
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(MOBJS) $(MLIBS)

${ADRIVER}:	${AOBJS}
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(AOBJS) $(ALIBS)


depend:

.cpp.o:
	$(CC) $(CFLAGS) -c $<

INCLUDE_DIRS = 
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= `pkg-config --cflags --libs libgps`
AFLAGS = -march=armv8-a -mtune=cortex-a53 -mfpu=vfpv4 -O3 -MMD
LDFLAGS = -L /usr/lib/
LIBS = -lm -lrt
ALIBS = -lasound
MONLIBS = -pthread

TARGET=monitor

HFILES=  monitor.hpp
CFILES=  monitor.cpp

SRCS= ${HFILES} ${CFILES}
OBJS=  monitor.o  ../localization.o ../acoustic.o ../inet_client2.o

all:	${TARGET}

clean:
	-rm -f *.o *.NEW *~
	-rm -f ${TARGET} ${DERIVED} ${GARBAGE}
	-cd .. && rm -f placeHolder *.o *.NEW *~

${TARGET}:	${OBJS}
	$(CC) $(CFLAGS) $(AFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(ALIBS) $(MONLIBS)

depend:

.cpp.o:
	@make -C ../
	$(CC) $(CFLAGS) -c $<

INCLUDE_DIRS = 
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= `pkg-config --cflags --libs libgps --libs opencv`
AFLAGS = -march=armv8-a -mtune=cortex-a53 -mfpu=vfpv4 -O3 -MMD
LDFLAGS = -L /usr/lib/
LIBS = -lm -lrt
ALIBS = -lasound
CPPLIBS= -L/usr/lib -lopencv_core -lopencv_imgproc -lopencv_flann -lopencv_video
MONLIBS = -pthread

TARGET=monitor

HFILES=  monitor.hpp
CFILES=  monitor.cpp

SRCS= ${HFILES} ${CFILES}
OBJS=  monitor.o  ../localization.o ../detectMotion.o ../acoustic.o ../inet_client2.o

all:	${TARGET}

clean:
	-rm -f *.o *.NEW *~
	-rm -f ${TARGET} ${DERIVED} ${GARBAGE}
	-cd .. && rm -f placeHolder *.o *.NEW *~

${TARGET}:	${OBJS}
	$(CC) $(AFLAGS) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(ALIBS)  $(CPPLIBS) $(MONLIBS)

depend:

.cpp.o:
	@make -C ../
	$(CC) $(CFLAGS) -c $<

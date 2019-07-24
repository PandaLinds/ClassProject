INCLUDE_DIRS = 
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= `pkg-config --cflags --libs libgps --libs opencv`
LDFLAGS = -L /usr/lib/
LIBS = -lm -lrt
CPPLIBS= -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video
MONLIBS = -pthread

TARGET=monitor

HFILES=  monitor.hpp
CFILES=  monitor.cpp

SRCS= ${HFILES} ${CFILES}
OBJS=  monitor.o  ../localization.o ../capture.o 

all:	${TARGET}

clean:
	-rm -f *.o *.NEW *~
	-rm -f ${TARGET} ${DERIVED} ${GARBAGE}
	-cd .. && rm -f placeHolder *.o *.NEW *~

${TARGET}:	${OBJS}
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(CPPLIBS) $(MONLIBS)

depend:

.cpp.o:
	@make -C ../
	$(CC) $(CFLAGS) -c $<

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

HFILES= localization.hpp capture.hpp
CFILES= localization.cpp capture.cpp monitor.cpp

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.cpp=.o}

all:	${TARGET}

clean:
	-rm -f *.o *.NEW *~
	-rm -f ${TARGET} ${DERIVED} ${GARBAGE}

${TARGET}:	${OBJS}
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(CPPLIBS) $(MONLIBS)

depend:

.cpp.o:
	$(CC) $(CFLAGS) -c $<

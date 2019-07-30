INCLUDE_DIRS = 
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= `pkg-config --cflags --libs libgps --libs opencv`
LDFLAGS = -L /usr/lib/
LIBS = -lm -lrt
CPPLIBS= -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video
MONLIBS = -pthread

TARGET=placeHolder

HFILES= localization.hpp map.hpp capture.hpp inet_client.hpp
CPPFILES= localization.cpp map.cpp capture.cpp detectMotion.cpp inet_client2.cpp placeHolder.cpp

SRCS= ${HFILES} ${CFILES}
OBJS= ${CPPFILES:.cpp=.o} 

all:	${TARGET}

clean:
	-rm -f *.o *.NEW *~
	-rm -f ${TARGET} ${DERIVED} ${GARBAGE}

${TARGET}:	${OBJS}
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(CPPLIBS) $(MONLIBS)

depend:

.cpp.o:
	$(CC) $(CFLAGS) -c $<

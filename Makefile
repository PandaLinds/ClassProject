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

TARGET=placeHolder

HFILES= localization.hpp map.hpp detectMotion.hpp inet_client.hpp inet_server.hpp
CPPFILES= localization.cpp map.cpp detectMotion.cpp inet_client2.cpp inet_server.cpp acoustic.cpp placeHolder.cpp

SRCS= ${HFILES} ${CFILES}
OBJS= ${CPPFILES:.cpp=.o} 

all:	${TARGET}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${TARGET} ${DERIVED} ${GARBAGE}

${TARGET}:	${OBJS}
	$(CC) $(CFLAGS) $(AFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(CPPLIBS) $(MONLIBS) $(ALIBS)

depend:

.cpp.o:
	$(CC) -c $<

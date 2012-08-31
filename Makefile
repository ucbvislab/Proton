ENDIANESS=OSC_HOST_LITTLE_ENDIAN
UNAME=$(shell uname)
CC=g++
INCLUDES=-I./oscpack -I.
CFLAGS=-c -Wall $(INCLUDES) -D$(ENDIANESS)
#EXECUTABLE=proton
LDFLAGS=-lGL -lGLU -lglut
ifeq ($(UNAME), Darwin)
	LDFLAGS=-framework GLUT -framework OpenGL
endif
OSCSOURCES=$(wildcard oscpack/osc/*.cpp) $(wildcard oscpack/ip/*.cpp) $(wildcard oscpack/ip/posix/*.cpp)
TUIOSOURCES=$(wildcard TUIO/*.cpp)
PTSOURCES=$(wildcard Pt/*.cpp)
SOURCES=$(OSCSOURCES) $(TUIOSOURCES) $(PTSOURCES)
SOBJECTS=$(SOURCES:.cpp=.o) streamSplitter.o
HOBJECTS=$(SOURCES:.cpp=.o) hitTester.o

all: streamSplitter hitTester

streamSplitter: $(SOBJECTS)
	$(CC) $(LDFLAGS) $(SOBJECTS) -o $@ $(LDFLAGS)

hitTester: $(HOBJECTS)
	$(CC) $(LDFLAGS) $(HOBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o streamSplitter hitTester
	rm -rf oscpack/*o oscpack/*/*o oscpack/*/*/*o
	rm -rf TUIO/*o
	rm -rf Pt/*o
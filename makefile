
CXXFLAGS=-Wall --std=c++11 -g
OBJS=escape.o map.o generate.o
LIBS=-lncurses
TARGET=escape

all: $(TARGET)

$(TARGET): $(OBJS)
	g++ $(OBJS) $(LIBS) -o $(TARGET)

maptest: maptest.o map.o generate.o
	g++  maptest.o map.o generate.o -lpng -lz -o maptest

clean:
	$(RM) *.o $(TARGET)

.PHONY: all clean
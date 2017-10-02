
CXXFLAGS=-Wall --std=c++11 -g
OBJS=escape.o generate.o
LIBS=-lncurses
TARGET=escape

all: $(TARGET)

$(TARGET): $(OBJS)
	g++ $(OBJS) $(LIBS) -o $(TARGET)

.PHONY: all clean
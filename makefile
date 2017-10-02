
CXXFLAGS=-Wall --std=c++11 -g
OBJS=escape.o map.o generate.o
LIBS=-lncurses
TARGET=escape

all: $(TARGET)

$(TARGET): $(OBJS)
	g++ $(OBJS) $(LIBS) -o $(TARGET)

clean:
	$(RM) *.o $(TARGET)

.PHONY: all clean
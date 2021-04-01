CC := clang++
FLAGS := -Iinclude -DDEBUG


OBJ := $(patsubst %.cpp, %.o, $(wildcard src/*.cpp))
OBJ := $(subst src, bin, $(OBJ))
SRC := $(wildcard src/*.cpp)
TARGET := bin/compiler


all: Bin $(TARGET)

Bin: 
	mkdir -p bin

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

bin/%.o: src/%.cpp
	$(CC) $(FLAGS) -c -o $@ $<

clean:
	rm -rf $(TARGET) $(OBJ)
	
.PHONY: all clean

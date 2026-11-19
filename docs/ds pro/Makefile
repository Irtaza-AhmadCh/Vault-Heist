# Compiler and Flags
CXX = E:/mingw32/bin/g++.exe
SFML_DIR = E:/SFML-2.6.2

CXXFLAGS = -std=c++17 -Wall -Wextra -I$(SFML_DIR)/include
LDFLAGS = -L$(SFML_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -static-libgcc -static-libstdc++

SRC = src/main.cpp
TARGET = main.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	if exist $(TARGET) del /f $(TARGET)

.PHONY: all run clean


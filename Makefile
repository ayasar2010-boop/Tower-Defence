CC      = gcc
SRC     = src/main.c
TARGET  = game
CFLAGS  = -std=c99 -Wall -Wextra -O2

# Platform tespiti
ifeq ($(OS),Windows_NT)
    LIBS    = -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET  = game.exe
else
    LIBS    = -lraylib -lm -lpthread -ldl -lrt
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	del /f $(TARGET) 2>NUL || true
else
	rm -f $(TARGET)
endif

run: $(TARGET)
ifeq ($(OS),Windows_NT)
	./$(TARGET).exe
else
	./$(TARGET)
endif

.PHONY: all clean run

CC = gcc

TARGET = app

SRC = main.c Files/FileManager.c DataStructs/*.c Algorithms/FIFO.c Algorithms/LFU.c Algorithms/LRU.c Algorithms/OPT.c Algorithms/SecondChance.c 
HEADERS = $(wildcard Files/*.h DataStructs/*.h Algorithms/*h *.h)

CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk4`
LIBS = `pkg-config --libs gtk4`

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

run: all
	./$(TARGET)

test: Files/FileManager.c Files/FileManager.h
	$(CC) -Wall -Wextra -g -fsanitize=address -DTEST_FILEMANAGER Files/FileManager.c -o test_fm
	./test_fm

clean:
	rm -f $(TARGET) test_fm

.PHONY: all run test clean 

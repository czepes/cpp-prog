CC = g++
CFLAGS = -Wall -std=c++17
TARGET = prog
OBJS = main.o words-stat.o words-writer.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean

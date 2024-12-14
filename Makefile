# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Targets
TARGET = portScanner
OBJS = portMain.o portScanner.o

# Default target
all: $(TARGET)

# Link the object files to create the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile portMain.c
portMain.o: portMain.c portScanner.h
	$(CC) $(CFLAGS) -c portMain.c

# Compile portScanner.c
portScanner.o: portScanner.c portScanner.h
	$(CC) $(CFLAGS) -c portScanner.c

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)
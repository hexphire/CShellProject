CC = gcc
CFLAGS = -g -Wall -Werror
INCLUDE = -Iinclude
LIBS = -lm
TARGET = esh
VPATH = src include
OBJDIR = obj
BINDIR = bin

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJDIR)/$(TARGET).o
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDE) -o $@ $<

$(OBJDIR)/%.o: $(TARGET).c
	$(CC) -c $(CFLAGS) $(LIBS) $(INCLUDE) $< -o $@ 

clean:
	rm obj/$(TARGET).o bin/$(TARGET)
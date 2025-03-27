CC = gcc

LINK_EXE = ./bin/windowBase

# Libraries to include
LIBS = -lgdi32

# Prevent terminal window also opening
CFLAGS = -Wl,-subsystem,windows -mwindows

# Main source file
SRC = src/windowBase.c

# Object file(s)
OBJS = build/windowBase.o

all: $(OBJS) $(LINK_EXE)

$(OBJS):$(SRC)

$(OBJS):
	$(CC) -c $(SRC) $(LIBS) -o $@

$(LINK_EXE): $(OBJS)
	$(CC) $(LIBS) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f *~
	rm -f $(LINK_EXE)
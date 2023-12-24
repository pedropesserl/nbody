CC = gcc
PROGRAM = nbody

CFLAGS = -Iinclude -Wall -Wextra -g
LFLAGS = -lm -lraylib

_OBJ = main.o nbody.o
_DEPS = raylib.h nbody.h vector.h

SDIR = src
IDIR = include

OBJ = $(_OBJ:%=$(SDIR)/%)
DEPS = $(_DEPS:%=$(IDIR)/%)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	rm -f $(OBJ)

purge:
	rm -f $(OBJ) $(PROGRAM)

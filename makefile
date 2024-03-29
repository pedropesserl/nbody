CC = gcc
PROGRAM = nbody

CFLAGS = -Iinclude -Wall -Wextra -g
LFLAGS = -lm -lraylib

_OBJ = main.o nbody_simulation.o nbody_ui.o
_DEPS = raylib.h raymath.h nbody_simulation.h nbody_ui.h

SDIR = src
IDIR = include

OBJ = $(_OBJ:%=$(SDIR)/%)
DEPS = $(_DEPS:%=$(IDIR)/%)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(PROGRAM)
all: clean

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	rm -f $(OBJ)

purge:
	rm -f $(OBJ) $(PROGRAM)

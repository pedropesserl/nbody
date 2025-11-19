CC = gcc
PROGRAM = nbody
SRC_PATH = src
INC_PATH = include
RAYLIB_PATH = /home/pedro/software/raylib/src

CFLAGS = -Iinclude -I$(RAYLIB_PATH) -Wall -Wextra -g
LFLAGS = -L$(RAYLIB_PATH) -lm

_SRC = main nbody_simulation nbody_ui
_DEPS = nbody_simulation.h nbody_ui.h

SRC = $(_SRC:%=$(SRC_PATH)/%.c)
DEPS = $(_DEPS:%=$(INC_PATH)/%) $(RAYLIB_PATH)/raylib.h $(RAYLIB_PATH)/raymath.h

$(PROGRAM): $(DEPS)
	$(CC) -o $@ $(SRC) $(CFLAGS) $(LFLAGS) -lraylib

all: $(PROGRAM) web

web: $(DEPS)
	emcc -o $(PROGRAM).html $(SRC) $(CFLAGS) $(LFLAGS) -lraylib.web -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --preload-file resources

purge:
	rm -f $(PROGRAM) *.html *.js *.wasm *.data

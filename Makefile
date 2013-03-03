# Hi and welcome to the "snowman Makefile". This Makefile is for compiling
# snowman on unix based systems or Windows with Mingw32/Cygwin (not tested)

# === First of all some defines. If your compilation is slow or instable, check
# === the defines and changes them for your Target.

# -DFAST_MULTIPLICATION enables a faster multiplication for fixed point
# arithmetics, it may look ugly on bigger resolution or may be too inaccurate.

# -DFAST_DIVISION does the same as -DFAST_MULTIPLICATION, but for division.

# -DDEBUG shows some debug informations => slower and may be confusing

# === Now some globale variables. These are the default values for compilation on
# === a Linux machine. The other targets then change, where their have differences

# DYNAMIC says, which libraries will be linked dynamicly. Most of the time these
# are all used libraries, but some systems also need static linking, too. Because
# as default no library is linked static, STATIC is not defined yet.
DYNAMIC = -lSDL_mixer -lSDL_ttf -lSDL_image -lSDL -lm

# CFLAGS defines some globale flags for gcc. Even on the gp2x with only 16 KB
# CPU Cache, -O3 is faster than -Os. So most things you don't have to change
CFLAGS = -O2 -fsingle-precision-constant -fPIC

# GENERAL_TWEAKS are some flags for gcc, which should make the compilation
# faster, but some of them are just poinsoness snake oil - they may help a bit,
# but could also kill you. ^^
GENERAL_TWEAKS = -ffast-math -DDEBUG

# every device using SMALL_RESOLUTION_DEVICES in the compilation will enable
# the faster multiplication and division
SMALL_RESOLUTION_DEVICES = -DFAST_MULTIPLICATION -DFAST_DIVISION

# The default Compiler is gcc with debug symbols and a X86CPU (that's a define
# for sparrow or your game, not used by gcc.
CPP = gcc -g -march=native -DX86CPU $(GENERAL_TWEAKS)

# SDL sets some SDL options with the program "sdl-config".
SDL = `sdl-config --cflags`

# SDL_PATH determines, where the SDL includes are (necessary if SDL is in subfolders)
SDL_PATH = -I/usr/include/SDL

SPARROW_FOLDER = ../sparrow3d

ifdef TARGET
include $(SPARROW_FOLDER)/target-files/$(TARGET).mk
BUILD = ./build/$(TARGET)/schwerkraft
SPARROW_LIB = $(SPARROW_FOLDER)/build/$(TARGET)/sparrow3d
else
TARGET = "Default (change with make TARGET=otherTarget. See All targets with make targets)"
BUILD = .
SPARROW_LIB = $(SPARROW_FOLDER)
endif
LIB += -L$(SPARROW_LIB)
DYNAMIC += -lsparrow3d

all: schwerkraft

targets:
	@echo "gp2x, open2x (like gp2x, but dynamic compiled => smaller), wiz caanoo, dingux, pandora, maemo5, maemo6"

schwerkraft: schwerkraft.c planet.o level.o ship.o font.o trace.o menu.o music.o makeBuildDir
	cp $(SPARROW_LIB)/libsparrow3d.so $(BUILD)
	$(CPP) $(CFLAGS) schwerkraft.c planet.o level.o ship.o font.o trace.o menu.o music.o $(SDL) $(INCLUDE) -L. $(LIB) $(STATIC) $(DYNAMIC) -o $(BUILD)/schwerkraft

makeBuildDir:
	 @if [ ! -d $(BUILD:/schwerkraft=/) ]; then mkdir $(BUILD:/schwerkraft=/);fi
	 @if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi

planet.o: planet.c planet.h
	$(CPP) $(CFLAGS) -c planet.c $(SDL) $(INCLUDE)

level.o: level.c level.h
	$(CPP) $(CFLAGS) -c level.c $(SDL) $(INCLUDE)

ship.o: ship.c ship.h
	$(CPP) $(CFLAGS) -c ship.c $(SDL) $(INCLUDE)

font.o: font.c font.h
	$(CPP) $(CFLAGS) -c font.c $(SDL) $(INCLUDE)

trace.o: trace.c trace.h
	$(CPP) $(CFLAGS) -c trace.c $(SDL) $(INCLUDE)

menu.o: menu.c menu.h
	$(CPP) $(CFLAGS) -c menu.c $(SDL) $(INCLUDE)

music.o: music.c music.h
	$(CPP) $(CFLAGS) -c music.c $(SDL) $(INCLUDE)

clean:
	rm *.o
	rm schwerkraft

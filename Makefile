#==stuff linked to
DYNAMIC = -lSDL_ttf -lSDL_mixer -lSDL_image -lSDL -lm
#==global Flags. Even on the gp2x with 16 kb Cache, -O3 is much better then -Os
CFLAGS = -O3 -fsingle-precision-constant -fPIC
# Testtweaks: -fgcse-lm -fgcse-sm -fsched-spec-load -fmodulo-sched -funsafe-loop-optimizations -Wunsafe-loop-optimizations -fgcse-las -fgcse-after-reload -fvariable-expansion-in-unroller -ftracer -fbranch-target-load-optimize
GENERAL_TWEAKS =  -ffast-math
#==PC==
FLAGS = -g -DDESKTOP
SDL = `sdl-config --cflags`

SPARROW_FOLDER = ../sparrow3d

SPARROW3D_LIB = libsparrow3d.so
SPARROWNET_LIB = libsparrowNet.so
SPARROWSOUND_LIB = libsparrowSound.so

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
DYNAMIC += -lsparrow3d -lsparrowSound -lsparrowNet

CFLAGS += $(PARAMETER) $(FLAGS)

all: schwerkraft

targets:
	@echo "gp2x, open2x (like gp2x, but dynamic compiled => smaller), wiz caanoo, dingux, pandora, maemo5, maemo6"

schwerkraft: schwerkraft.c planet.o level.o ship.o font.o trace.o menu.o music.o makeBuildDir
	cp -u $(SPARROW_LIB)/$(SPARROW3D_LIB) $(BUILD)
	cp -u $(SPARROW_LIB)/$(SPARROWNET_LIB) $(BUILD)
	cp -u $(SPARROW_LIB)/$(SPARROWSOUND_LIB) $(BUILD)
	$(CC) $(CFLAGS) schwerkraft.c planet.o level.o ship.o font.o trace.o menu.o music.o $(SDL) $(INCLUDE) -I$(SPARROW_FOLDER) $(LIB) $(SDL_LIB) $(STATIC) $(DYNAMIC) -o $(BUILD)/schwerkraft$(SUFFIX)

makeBuildDir:
	 @if [ ! -d $(BUILD:/schwerkraft=/) ]; then mkdir $(BUILD:/schwerkraft=/);fi
	 @if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi

planet.o: planet.c planet.h
	$(CC) $(CFLAGS) -c planet.c $(SDL) $(INCLUDE)

level.o: level.c level.h
	$(CC) $(CFLAGS) -c level.c $(SDL) $(INCLUDE)

ship.o: ship.c ship.h
	$(CC) $(CFLAGS) -c ship.c $(SDL) $(INCLUDE)

font.o: font.c font.h
	$(CC) $(CFLAGS) -c font.c $(SDL) $(INCLUDE)

trace.o: trace.c trace.h
	$(CC) $(CFLAGS) -c trace.c $(SDL) $(INCLUDE)

menu.o: menu.c menu.h
	$(CC) $(CFLAGS) -c menu.c $(SDL) $(INCLUDE)

music.o: music.c music.h
	$(CC) $(CFLAGS) -c music.c $(SDL) $(INCLUDE)

clean:
	rm -f *.o
	rm -f schwerkraft

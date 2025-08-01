#Formido makefile for Linux/Unix adapted to OS/2
#Make sure you have SDL, SDL_mixer and SDL_image
#installed!

#Formido directories
#You can use these defaults, or change them if you like to
#(just don't include the ending slash)


#Data directory (default: ./data)
DATDIR = ./data

#High score directory (default: ./data)
HISCOREDIR = /.formido

#Config file directory (default: .)
CONFIGDIR = /.formido
DEFCONFIGDIR = .

CC=g++
FLAGS=`sdl-config --cflags` -Wall -I./include
LINKFLAGS=-O2 -ffast-math -fomit-frame-pointer -Zomf
LIBS=`sdl-config --libs` -lm -lSDL_mixer -lSDL_image
CPPFILES=blend.cpp credits.cpp explo.cpp hof.cpp menu.cpp player.cpp \
	bomb.cpp critter.cpp font.cpp init.cpp mymath.cpp powerup.cpp \
	config.cpp effect.cpp game.cpp main.cpp particle.cpp timer.cpp

DEFINES=-DDATA_DIR=\"${DATDIR}\" -DHISCORE_DIR=\"${HISCOREDIR}\" -DCONFIG_DIR=\"${CONFIGDIR}\" -DDEFCONFIG_DIR=\"${DEFCONFIGDIR}\"

SOURCES=$(addprefix src/, $(CPPFILES))
OBJECTS=$(addprefix obj/, $(CPPFILES:.cpp=.o))
PROGRAM=formido.exe



all: $(OBJECTS) $(PROGRAM)


$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) $(LINKFLAGS) -o $(PROGRAM) $(LIBS)

obj/%.o: src/%.cpp
	$(CC) -o $@ $(FLAGS) $(DEFINES) -c $<


clean:
	rm  -r obj/*.o $(PROGRAM)




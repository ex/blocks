
SDL_CFLAGS=$(shell sdl-config --cflags --libs)

GAME_FLAGS=-DSTC_SHOW_GHOST_PIECE -DSTC_WALL_KICK_ENABLED -DSTC_AUTO_ROTATION

all: stc stc++

stc:
	gcc $(SDL_CFLAGS) $(GAME_FLAGS) main.c game.c sdl/sdl_game.c -o ../bin/stc -lSDL

stc++:
	g++ $(SDL_CFLAGS) $(GAME_FLAGS) main.cpp game.cpp sdl/sdl_game.cpp -o ../bin/stc++ -lSDL -lSDL_mixer -lSDL_image

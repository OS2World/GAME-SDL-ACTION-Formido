/*
    Formido - a shooting game (see README for additional information)
    Copyright (C) 2002, 2003 Mika Halttunen

    You can contact me by email: lsoft@mbnet.fi
    -----------------------------------------------------------------

    This file is part of Formido.

    Formido is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Formido is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Formido; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "init.h"
#include "critter.h"
#include "mymath.h"
#include "game.h"
#include "player.h"
#include "font.h"
#include "timer.h"
#include "effect.h"
#include "powerup.h"
#include "blend.h"
#include "explo.h"
#include "bomb.h"
#include "menu.h"
#include "credits.h"


// Version string
#define GAME_VERSION	"v1.0.1"



//////////////////////
// Progress bar things
//////////////////////
SDL_Surface *progbar[2];	// Graphics

class PROGRESS_BAR {
public:

	float value;			// Value between 0.0 and 1.0
	int length;				// Length

	// Initialize
	void init(int _length) {
		length = _length;
		value = 0;

		SDL_FillRect(screen, NULL, 0);
		SDL_Flip(screen);
	}

	// Update
	void update(float val) {
		value = val;
		
		SDL_FillRect(screen, NULL, 0);

		draw(screen, SCREEN_H-100);
		font_write(screen, FONT_BOLD, SCREEN_W/2-font_length(FONT_BOLD, "Loading")/2, SCREEN_H-60, "Loading");
		font_write(screen, FONT_NORMAL, SCREEN_W/2-20, SCREEN_H-35, GAME_VERSION);
		SDL_Flip(screen);
	}

	// Draw
	void draw(SDL_Surface *bmp, int y) {

		int x = SCREEN_W/2 - ((progbar[0]->w+2)*length)/2;

		for(int f=0; f<length; f++) {
			SDL_Rect r;
			r.x = x + f*(progbar[0]->w+4);
			r.y = y;
			r.w = progbar[0]->w;
			r.h = progbar[0]->h;
			SDL_BlitSurface(((float)f/(float)length >= value) ? progbar[0]:progbar[1], NULL, bmp, &r);
		}
	}

	// Load
	void load() {
		SDL_Surface *tmp;
			
		tmp = IMG_Load(DAT("progress1.dat"));
		if(tmp == NULL)
			error_msg("Unable to load progress bar!\n%s", IMG_GetError());
		progbar[0] = SDL_DisplayFormat(tmp);
		if(progbar[0] == NULL)
			error_msg("Unable to load progress bar!\n%s", IMG_GetError());
		SDL_FreeSurface(tmp);

		tmp = IMG_Load(DAT("progress2.dat"));
		if(tmp == NULL)
			error_msg("Unable to load progress bar!\n%s", IMG_GetError());
		progbar[1] = SDL_DisplayFormat(tmp);
		if(progbar[1] == NULL)
			error_msg("Unable to load progress bar!\n%s", IMG_GetError());
		SDL_FreeSurface(tmp);
	}

	// Unload
	void unload() {
		if(progbar[0])
			SDL_FreeSurface(progbar[0]);
		if(progbar[1])
			SDL_FreeSurface(progbar[1]);
	}
};

//////////////////////
//////////////////////


// Load sounds
void load_sounds() {

	int f;
	// Player hit sounds
	for(f=0; f<5; f++) {
		char file[100] = "";
		sprintf(file, "plhit%d.dat", f+1);

		snd_hit[f] = Mix_LoadWAV_RW(SDL_RWFromFile(DAT(file), "rb"),1);
		if(snd_hit[f] == NULL)
			error_msg("Unable to load sounds!\n%s", Mix_GetError());
	}

	// Player bullet sounds
	for(f=0; f<4; f++) {
		char file[100] = "";
		sprintf(file, "bulcol%d.dat", f+1);

		snd_bullet[f] = Mix_LoadWAV_RW(SDL_RWFromFile(DAT(file), "rb"),1);
		if(snd_bullet[f] == NULL)
			error_msg("Unable to load sounds!\n%s", Mix_GetError());
	}

	// Player dying sounds
	for(f=0; f<3; f++) {
		char file[100] = "";
		sprintf(file, "pldie%d.dat", f+1);

		snd_die[f] = Mix_LoadWAV_RW(SDL_RWFromFile(DAT(file), "rb"),1);
		if(snd_die[f] == NULL)
			error_msg("Unable to load sounds!\n%s", Mix_GetError());
	}

	// Critter dying sounds
	for(f=0; f<5; f++) {
		char file[100] = "";
		sprintf(file, "critter_die%d.dat", f+1);

		snd_cdie[f] = Mix_LoadWAV_RW(SDL_RWFromFile(DAT(file), "rb"),1);
		if(snd_cdie[f] == NULL)
			error_msg("Unable to load sounds!\n%s", Mix_GetError());
	}

	// Explosions
	for(f=0; f<2; f++) {
		char file[100] = "";
		sprintf(file, "exp%d.dat", f+1);

		snd_explo[f] = Mix_LoadWAV_RW(SDL_RWFromFile(DAT(file), "rb"),1);
		if(snd_explo[f] == NULL)
			error_msg("Unable to load sounds!\n%s", Mix_GetError());
	}

	// Shooting sound
	snd_shoot = Mix_LoadWAV_RW(SDL_RWFromFile(DAT("shooting.dat"), "rb"),1);
	if(snd_shoot == NULL)
		error_msg("Unable to load sounds!\n%s", Mix_GetError());

	snd_throw = Mix_LoadWAV_RW(SDL_RWFromFile(DAT("throw.dat"), "rb"),1);
	if(snd_throw == NULL)
		error_msg("Unable to load sounds!\n%s", Mix_GetError());

	// Glass break sound
	snd_glass_break = Mix_LoadWAV_RW(SDL_RWFromFile(DAT("glass_break.dat"), "rb"),1);
	if(snd_glass_break == NULL)
		error_msg("Unable to load sounds!\n%s", Mix_GetError());

	// Power-up sound
	snd_power_up = Mix_LoadWAV_RW(SDL_RWFromFile(DAT("power_up.dat"), "rb"),1);
	if(snd_power_up == NULL)
		error_msg("Unable to load sounds!\n%s", Mix_GetError());

	// Mushroom sound
	snd_mushroom = Mix_LoadWAV_RW(SDL_RWFromFile(DAT("mushroom.dat"), "rb"),1);
	if(snd_mushroom == NULL)
		error_msg("Unable to load sounds!\n%s", Mix_GetError());

	// Menu sounds
	snd_menu[0] = Mix_LoadWAV_RW(SDL_RWFromFile(DAT("menu1w.dat"), "rb"),1);
	if(snd_menu[0] == NULL)
		error_msg("Unable to load sounds!\n%s", Mix_GetError());

	snd_menu[1] = Mix_LoadWAV_RW(SDL_RWFromFile(DAT("menu2w.dat"), "rb"),1);
	if(snd_menu[1] == NULL)
		error_msg("Unable to load sounds!\n%s", Mix_GetError());

	snd_menu[2] = Mix_LoadWAV_RW(SDL_RWFromFile(DAT("keypress.dat"), "rb"),1);
	if(snd_menu[2] == NULL)
		error_msg("Unable to load sounds!\n%s", Mix_GetError());

}


// Unload sounds
void unload_sounds() {

	int f;
	for(f=0; f<5; f++)
		if(snd_hit[f])
			Mix_FreeChunk(snd_hit[f]);

	for(f=0; f<4; f++)
		if(snd_bullet[f])
			Mix_FreeChunk(snd_bullet[f]);

	for(f=0; f<5; f++)
		if(snd_cdie[f])
			Mix_FreeChunk(snd_cdie[f]);

	for(f=0; f<3; f++)
		if(snd_die[f])
			Mix_FreeChunk(snd_die[f]);

	for(f=0; f<2; f++)
		if(snd_explo[f])
			Mix_FreeChunk(snd_explo[f]);

	if(snd_shoot)
		Mix_FreeChunk(snd_shoot);

	if(snd_throw)
		Mix_FreeChunk(snd_throw);

	if(snd_glass_break)
		Mix_FreeChunk(snd_glass_break);

	if(snd_power_up)
		Mix_FreeChunk(snd_power_up);

	if(snd_mushroom)
		Mix_FreeChunk(snd_mushroom);

	if(snd_menu[0])
		Mix_FreeChunk(snd_menu[0]);
	if(snd_menu[1])
		Mix_FreeChunk(snd_menu[1]);
	if(snd_menu[2])
		Mix_FreeChunk(snd_menu[2]);
}


// The good old main()
int main(int argc, char *argv[]) {

	// Initialize SDL
	init_sdl();

	// Load fonts
	load_fonts();

	// Load progress bar
	PROGRESS_BAR pb;
	pb.load();
	pb.init(8);

	pb.update(0);


	// Init blend look-ups
	init_blend();

	// Initialize math tables
	init_math();


	// Load graphics
	load_critters(); pb.update(0.1);
	load_player_gfx(); pb.update(0.3);
	load_power_ups(); pb.update(0.4);
	load_explosions(); pb.update(0.6);
	load_bombs(); pb.update(0.7);
	load_menus(); pb.update(0.8);

	// Load sounds
	if(config.sound)
		load_sounds();
	pb.update(1.0);
	pb.unload();

	// Add timer to run 60 fps
	start_timer(60);

	// Menu logic
	int ans = show_menu(MENU_ID_MAIN);
	while(ans != MENU_EXIT) {
		// New game?
		if(ans == MENU_NEW_GAME)
			start_game();

		// Show the menu again
		ans = show_menu(MENU_ID_MAIN);
	}

	// Show the credits
	show_credits();

	// Destroy timer
	kill_timer();

	// Unload graphics
	unload_critters();
	unload_player_gfx();
	unload_power_ups();
	unload_explosions();
	unload_bombs();
	unload_menus();

	// Unload sounds
	if(config.sound)
		unload_sounds();

	// Unload fonts
	unload_fonts();

	Mix_CloseAudio();

	// Free the surfaces created on game.c
	if(pause_bmp)
		SDL_FreeSurface(pause_bmp);
	if(cur_bground)
		SDL_FreeSurface(cur_bground);
	if(orig_bground)
		SDL_FreeSurface(orig_bground);
	if(col_map)
		SDL_FreeSurface(col_map);
	if(distort_tmp)
		SDL_FreeSurface(distort_tmp);


	// Save the config
	//save_config("formido.cfg", &config);
	save_config(CONFIG("formido.cfg"), &config);

	return 0;
}


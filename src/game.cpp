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
#include <stdio.h>
#include <string.h>
#include "SDL.h"
#include "SDL_image.h"
#include "game.h"
#include "critter.h"
#include "mymath.h"
#include "init.h"
#include "font.h"
#include "player.h"
#include "timer.h"
#include "effect.h"
#include "powerup.h"
#include "particle.h"
#include "explo.h"
#include "bomb.h"
#include "hof.h"


// Array containing currently pressed keys
Uint8 key[SDLK_LAST];

// Current background surface
SDL_Surface *cur_bground = NULL;

// Clean background
SDL_Surface *orig_bground = NULL;

// Collision map
SDL_Surface *col_map = NULL;

// Pause surface
SDL_Surface *pause_bmp = NULL;


bool main_loop_done;

// Current kills
// (patch by Jarmo Hekkanen)
int kills = 0;

// Fps amount
float fps;

// Delay before exiting after player's death
int death_delay;


// Load background images
void load_bgrounds() {

	// Load the background
	SDL_Surface *tmp;
	if(orig_bground == NULL) {
		if(!screen->format->palette)
			tmp = IMG_Load(DAT("ground.dat"));
		else
			tmp = IMG_Load(DAT("ground_8bit.dat"));
		if(tmp == NULL)
			error_msg("Unable to load background!\n%s", IMG_GetError());

		// Convert the background to display format
		orig_bground = SDL_DisplayFormat(tmp);
		if(orig_bground == NULL)
			error_msg("Unable to load background!\n%s", IMG_GetError());

		SDL_FreeSurface(tmp);
	}

	if(cur_bground == NULL) {
		tmp = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_W, SCREEN_H, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
		if(!tmp)
			error_msg("Unable to load background!\n%s", SDL_GetError());

		cur_bground = SDL_DisplayFormat(tmp);
		if(cur_bground == NULL)
			error_msg("Unable to load background!\n%s", SDL_GetError());

		SDL_FreeSurface(tmp);
	}
	SDL_BlitSurface(orig_bground, NULL, cur_bground, NULL);


	// Load the collision map
	if(col_map == NULL) {
		col_map = IMG_Load(DAT("collisionmap.dat"));
		if(col_map == NULL)
			error_msg("Unable to load collision map!");
	}

}


int create_count = 0;
int create_delay = 60;
unsigned long time_passed = 0;

// Create more critters
void create_critters() {

	if(!player.ok)
		return;

	if(create_count > 0)
		create_count--;

	time_passed++;
	if(time_passed == 120) {
		create_delay--;
		if(create_delay < 20)
			create_delay = 20;
		time_passed = 0;
	}

	if(create_count == 0 && num_critters < 200) {
		// Create a critter

		// Choose the side
		int side = RAND(1,4);

		float x,y;
		if(side == 1) {			// Up
			x = RANDF(-50,SCREEN_W+50);
			y = -50;
		}
		else if(side == 2) {	// Down
			x = RANDF(-50,SCREEN_W+50);
			y = SCREEN_H+50;
		}
		else if(side == 3) {	// Left
			x = -50;
			y = RANDF(-50,SCREEN_H+50);
		}
		else {					// Right
			x = SCREEN_W+50;
			y = RANDF(-50,SCREEN_H+50);
		}

		add_critter(x,y,0);

		create_count = create_delay;
	}
}


// Take a screenshot
void take_screenshot() {

	char file[100] = "";
	int num = 0;
	sprintf(file, "shot%03d.bmp", num);

	game_paused = true;

	// Check if exists
	FILE *f = fopen(CONFIG(file), "r");
	if(!f) {
		// Doesn't exist, save it.
		SDL_SaveBMP(screen, CONFIG(file));
	}
	else {
		fclose(f);

		// Exists, choose another name
		while(num < 1000) {
			num++;
			sprintf(file, "shot%03d.bmp", num);
			

			// Check if exists
			f = fopen(CONFIG(file), "r");
			if(!f) {
				// Doesn't exist, save it.
				SDL_SaveBMP(screen, CONFIG(file));
				break;
			}
			else
				fclose(f);

		}
	}

	game_paused = false;
}


// Drugs distortion
//////////////////////////////
SDL_Surface *distort_tmp = NULL;

int distort_table[360];

int dis_a_start;
int dis_shake;

// Distort the screen
void distort(SDL_Surface *bmp) {

	SDL_BlitSurface(bmp, NULL, distort_tmp, NULL);
	SDL_FillRect(bmp, NULL, 0);

	int dis_a = dis_a_start;

	for(int y=0; y<SCREEN_H; y++) {
		int dis = distort_table[dis_a];
		
		dis_a += 2;
		if(dis_a > 359)
			dis_a -= 360;

		// Random shaking.. sick!
		if(dis_shake > 0)
			dis += RAND(-5,5);

		SDL_Rect src, dst;
		src.x = 0;
		src.y = y;
		src.w = SCREEN_W;
		src.h = 1;

		dst.x = dis;
		dst.y = y;
		dst.w = SCREEN_W;
		dst.h = 1;
		SDL_BlitSurface(distort_tmp, &src, bmp, &dst);
	}

/*	SDL_BlitSurface(bmp, NULL, distort_tmp, NULL);
	SDL_FillRect(bmp, NULL, 0);

	for(int x=0; x<SCREEN_W; x++) {
		int dis = distort_table[dis_a];//SIN(dis_a) * 10;

		dis_a++;
		if(dis_a > 359)
			dis_a -= 360;

		SDL_Rect src, dst;
		src.x = x;
		src.y = 0;
		src.w = 1;
		src.h = SCREEN_H;

		dst.x = x;
		dst.y = dis;
		dst.w = 1;
		dst.h = SCREEN_H;
		SDL_BlitSurface(distort_tmp, &src, bmp, &dst);
	}
*/
}

///////////////////////////


// Draw the scene
void draw(SDL_Surface *bmp) {

	// Blit the background
	SDL_BlitSurface(cur_bground, NULL, bmp, NULL);

	// Draw the gibs
	draw_gibs(bmp);

	// Draw the player (before critters if dead)
	if(!player.ok)
		player.draw(bmp);

	// Draw the bombs
	draw_bombs(bmp);

	// Draw the power-ups
	draw_power_ups(bmp);

	// Draw the critters
	draw_critters(bmp);

	// Draw the player (after critters if alive)
	if(player.ok)
		player.draw(bmp);

	// Draw the particles
	draw_particles(bmp);

	// Draw the explosions
	draw_explosions(bmp);

	// Draw the player energy
	float health = player.health / 200.0;
	for(int i=0; i<10; i++) {
		SDL_Rect r;
		r.x = 5+i*energy_bar[0]->w;
		r.y = SCREEN_H-5-energy_bar[0]->h;
		r.w = energy_bar[0]->w;
		r.h = energy_bar[0]->h;
		SDL_BlitSurface(((float)i/10.0 >= health) ? energy_bar[1] : energy_bar[0], NULL, bmp, &r);
	}

	// Draw the player weapon amount
	if(player.special_weapon != 0) {
		int bul = player.swp_amount;
		for(int i=0; i<10;  i++) {
			SDL_Rect r;
			r.x = 5+i*weapon_bar[0]->w;
			r.y = SCREEN_H-10-2*weapon_bar[0]->h;
			r.w = weapon_bar[0]->w;
			r.h = weapon_bar[0]->h;
			SDL_BlitSurface((i >= bul) ? weapon_bar[1] : weapon_bar[0], NULL, bmp, &r);
		}
	}


	// Draw the fps amount
	if(config.show_fps) {
		char fps_str[100] = "";
		sprintf(fps_str, "%03.0f fps", fps);
		font_write(bmp, FONT_NORMAL, 0,0, fps_str);
	}

	// Draw the current kills
	char kill_str[100] = "";
	sprintf(kill_str, "%03d", kills);
	font_write(bmp, FONT_BOLD, SCREEN_W-22-font_length(FONT_BOLD, kill_str), 5, kill_str);


	// Distort the screen if player is on drugs
	if(player.drugs)
		distort(bmp);

	// Flip to screen
	SDL_Flip(bmp);
}


// Handle SDL events
// Return true if we're going to quit
bool handle_event(SDL_Event &event) {

	switch(event.type) {
		case SDL_QUIT:		// Quit
			return true;

		case SDL_KEYDOWN:
			// Update the 'key' array
			key[event.key.keysym.sym] = 1;
			return false;

		case SDL_KEYUP:
			// Update the 'key' array
			key[event.key.keysym.sym] = 0;
			return false;
	}

	return false;
}


// Music playing stuff
int music_num = -1;
char music_files[3][512];


// Start the game
void start_game() {
	
	// Set the music file names
	strcpy(music_files[0], DAT("music1.ogg"));
	strcpy(music_files[1], DAT("music2.ogg"));
	strcpy(music_files[2], DAT("music3.ogg"));

	// Play some music
	if(music_num == -1)
		music_num = RAND(0,2);

	Mix_Music *music = NULL;
	if(config.sound) {
		music = Mix_LoadMUS(music_files[music_num]);
		//Mix_PlayMusic(music, -1);
		Mix_FadeInMusic(music, -1, 5000);
	}
	music_num++;
	if(music_num > 2)
		music_num = 0;


	// Initialize distortion stuff
	if(distort_tmp == NULL) {
		SDL_Surface *tmp;
		tmp = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_W, SCREEN_H, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
		if(!tmp)
			error_msg("Unable to create distortion surface!\n");
		distort_tmp = SDL_DisplayFormat(tmp);
		if(!distort_tmp)
			error_msg("Unable to create distortion surface!\n");
		SDL_FreeSurface(tmp);
	}
	for(int d=0; d<360; d++)
		distort_table[d] = (int)((float)SIN(d) * 20);

	dis_a_start = RAND(0,359);
	dis_shake = 0;


	// Initialize
	clear_critters();
	clear_gibs();
	clear_power_ups();
	clear_particles();
	clear_explosions();
	clear_bombs();
	player.clear();
	player.x = SCREEN_W/2;
	player.y = SCREEN_H/2;
	memset(key, 0, sizeof(key));
	time_passed = 0;
	create_count = 0;
	create_delay = 60;
	kills = 0;
	death_delay = 3 * 60;

	// Create the pause surface
	if(pause_bmp == NULL) {
		SDL_Surface *tmp;
		tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_W, SCREEN_H, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
		if(!tmp)
			error_msg("Unable to create the pause surface!\n");
		pause_bmp = SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
	}
	if(!pause_bmp)
		error_msg("Unable to create the pause surface!\n");
	SDL_FillRect(pause_bmp, NULL, 0);
	SDL_SetAlpha(pause_bmp, SDL_RLEACCEL|SDL_SRCALPHA, 128);

	// Load backgrounds
	load_bgrounds();

	// Main loop
	bool pause_key_down = false;
	int power_up_counter = 0;
	timer_count = 0;
	Uint32 fps_counter = 0;
	int frames_drawn = 0;
	Uint32 prev_ticks;
	prev_ticks = SDL_GetTicks();

	main_loop_done = false;
	bool f12_down = false;
	while(!main_loop_done) {
		// Handle events
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			main_loop_done = handle_event(event);
		}

		// Pause key
		if(key[SDLK_p]) {
			if(pause_key_down == false) {
				game_paused = !game_paused;
				pause_key_down = true;

#ifndef WIN32
				// While game is paused, let the WM get the input first
				// (patch by Tim O'Brien) 
				if(game_paused)
					SDL_WM_GrabInput(SDL_GRAB_OFF);
				else
					SDL_WM_GrabInput(SDL_GRAB_ON);
#endif

				// Update the pause surface
				if(game_paused) {
					SDL_BlitSurface(pause_bmp, NULL, screen, NULL);
					font_write(screen, FONT_BOLD, SCREEN_W/2-font_length(FONT_BOLD, "PAUSED")/2, SCREEN_H/2+90, "PAUSED");
					SDL_Flip(screen);
				}
			}
		}
		else
			pause_key_down = false;

		// Game loop
		while(timer_count > 0 && !game_paused) {

			// Create power-ups?
			power_up_counter++;
			if(power_up_counter >= 60*7) {
				power_up_counter = 0;
				create_power_up();
			}

			// Check keypresses
			if(key[SDLK_ESCAPE]) {
				main_loop_done = true;
			}

			// Screenshot?
			if(key[SDLK_F12]) {
				if(f12_down == false) {
					f12_down = true;
					take_screenshot();
				}
			}
			else
				f12_down = false;

			// Create more critters
			create_critters();

			// Move critters
			move_critters();

			// Move critter gibs
			move_gibs();

			// Move bombs
			move_bombs();

			// Move player
			player.move();

			// Move particles
			move_particles();

			// Animate power-ups
			move_power_ups();

			// Animate explosions
			move_explosions();


			// Advance the fps_counter
			if(config.show_fps) {
				Uint32 ticks_now = SDL_GetTicks();
				Uint32 diff = ticks_now - prev_ticks;
				fps_counter += diff;
				prev_ticks = ticks_now;

				if(fps_counter >= 1000) {
					fps = (float)frames_drawn / (float)(fps_counter/1000.0f);
					frames_drawn = 0;
					fps_counter = 0;
				}
			}


			// Animate distortion if player's on drugs
			if(player.drugs) {
				dis_a_start += 2;
				if(dis_a_start > 359)
					dis_a_start -= 360;

				if(RAND(0,100) > 98 && dis_shake == 0)
					dis_shake = RAND(5,20);
				if(dis_shake > 0)
					dis_shake--;
			}

			// If player's dead, reduce the death delay
			if(player.ok == false && death_delay > 0)
				death_delay--;
			if(death_delay <= 0)
				main_loop_done = true;

			timer_count--;
		}


		// Draw the scene
		if(!game_paused) {
			draw(screen);
			frames_drawn++;
		}

	}


	// ..and silence, please
	if(player.shoot_chan != -1 && config.sound) {
		Mix_HaltChannel(player.shoot_chan);
		player.shoot_chan = -1;
	}

	wipe_screen();

	// Unload music
	if(music) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}

	// Clean up
	clear_critters();
	clear_gibs();

	/*SDL_FreeSurface(pause_bmp);
	SDL_FreeSurface(cur_bground);
	SDL_FreeSurface(orig_bground);
	SDL_FreeSurface(col_map);
	SDL_FreeSurface(distort_tmp);*/

	// Hiscore stuff
	HISCORE_LIST list;
	list.load(HISCORE("hiscore.dat"));
	int place = list.add("", kills);
	if(place != -1) {
		// Player is allowed to the Hall of Fame
		list.input_name(place);
		list.save(HISCORE("hiscore.dat"));
	}
}


// Get a pixel color from a surface
inline Uint32 getpixel(SDL_Surface *bmp, int x, int y) {

	if(x < 0 || y < 0 || x >= bmp->w || y >= bmp->h)
		return 0;

	int bpp = bmp->format->BytesPerPixel;
	Uint8 *p = (Uint8*)bmp->pixels + y * bmp->pitch + x*bpp;

	switch(bpp) {
		case 1:
			return *p;
		case 2:
			return *(Uint16*)p;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return (p[0] << 16 | p[1] << 8 | p[2]);
			else
				return (p[0] | p[1] << 8 | p[2] << 16);

		case 4:
			return *(Uint32*)p;

		default:
			return 0;
	}

}


// Put a pixel to a surface
inline void putpixel(SDL_Surface *dst, int x, int y, Uint32 color) {

	int bpp = dst->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)dst->pixels + y * dst->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = color;
        break;

    case 2:
        *(Uint16 *)p = color;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (color >> 16) & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = color & 0xff;
        } else {
            p[0] = color & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = (color >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = color;
        break;
    }
}


// Does point (x,y) collide with geometry
bool check_collision(int x, int y) {

	if(SDL_MUSTLOCK(col_map))
		SDL_LockSurface(col_map);

	Uint32 color = getpixel(col_map, x,y);

	if(SDL_MUSTLOCK(col_map))
		SDL_UnlockSurface(col_map);

	if(color == 0)
		return false;

	// If color is white or blue, collision is true
	Uint8 r,g,b;
	SDL_GetRGB(color, col_map->format, &r,&g,&b);

	if(r == 255  && g == 255 && b == 255)
		return true;
	else if(r == 0 && g == 0 && b == 255)
		return true;
	else
		return false;
}


// Does bullet at (x,y) collide with geometry
bool check_bul_collision(int x, int y) {

	if(SDL_MUSTLOCK(col_map))
		SDL_LockSurface(col_map);

	Uint32 color = getpixel(col_map, x,y);

	if(SDL_MUSTLOCK(col_map))
		SDL_UnlockSurface(col_map);

	// If color is white, collision is happened
	Uint8 r,g,b;
	SDL_GetRGB(color, col_map->format, &r,&g,&b);

	if(r == 255 && g == 255 && b == 255)
		return true;
	else
		return false;

}



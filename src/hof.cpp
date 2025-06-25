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
#include <time.h>
#include "SDL.h"
#include "hof.h"
#include "init.h"
#include "font.h"
#include "mymath.h"
#include "timer.h"
#include "game.h"
#include "menu.h"


// Input name and update the list
void HISCORE_LIST::input_name(int place) {

	// We modify directly the 'list's names[place] entry
	memset(names[place], 0, sizeof(names[place]));
	char *str = names[place];

	// Current position in string
	int pos = 0;
	bool typed = false;

	// Cursor color
	Uint32 cursor_color = SDL_MapRGB(screen->format, 128, 255, 128);
	int cursor_blink = 0;

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// Loop
	timer_count = 0;
	bool done = false;
	while(!done) {
		while(timer_count > 0) {
			// Handle events
			SDL_Event event;
			while(SDL_PollEvent(&event)) {
				done = handle_event(event);		// from 'game.h'
			}

			// Blink
			cursor_blink++;
			cursor_blink %= 20;

			// Check input
			typed = true;
			if(key[SDLK_0])
				str[pos] = '0';
			else if(key[SDLK_1])
				str[pos] = '1';
			else if(key[SDLK_2])
				str[pos] = '2';
			else if(key[SDLK_3])
				str[pos] = '3';
			else if(key[SDLK_4])
				str[pos] = '4';
			else if(key[SDLK_5])
				str[pos] = '5';
			else if(key[SDLK_6])
				str[pos] = '6';
			else if(key[SDLK_7])
				str[pos] = '7';
			else if(key[SDLK_8])
				str[pos] = '8';
			else if(key[SDLK_9])
				str[pos] = '9';
			else if(key[SDLK_a])
				str[pos] = 'A';
			else if(key[SDLK_b])
				str[pos] = 'B';
			else if(key[SDLK_c])
				str[pos] = 'C';
			else if(key[SDLK_d])
				str[pos] = 'D';
			else if(key[SDLK_e])
				str[pos] = 'E';
			else if(key[SDLK_f])
				str[pos] = 'F';
			else if(key[SDLK_g])
				str[pos] = 'G';
			else if(key[SDLK_h])
				str[pos] = 'H';
			else if(key[SDLK_i])
				str[pos] = 'I';
			else if(key[SDLK_j])
				str[pos] = 'J';
			else if(key[SDLK_k])
				str[pos] = 'K';
			else if(key[SDLK_l])
				str[pos] = 'L';
			else if(key[SDLK_m])
				str[pos] = 'M';
			else if(key[SDLK_n])
				str[pos] = 'N';
			else if(key[SDLK_o])
				str[pos] = 'O';
			else if(key[SDLK_p])
				str[pos] = 'P';
			else if(key[SDLK_q])
				str[pos] = 'Q';
			else if(key[SDLK_r])
				str[pos] = 'R';
			else if(key[SDLK_s])
				str[pos] = 'S';
			else if(key[SDLK_t])
				str[pos] = 'T';
			else if(key[SDLK_u])
				str[pos] = 'U';
			else if(key[SDLK_v])
				str[pos] = 'V';
			else if(key[SDLK_w])
				str[pos] = 'W';
			else if(key[SDLK_x])
				str[pos] = 'X';
			else if(key[SDLK_y])
				str[pos] = 'Y';
			else if(key[SDLK_z])
				str[pos] = 'Z';
			else if(key[SDLK_COMMA])
				str[pos] = ',';
			else if(key[SDLK_PERIOD])
				str[pos] = '.';
			else if(key[SDLK_KP_MINUS] || key[SDLK_MINUS])
				str[pos] = '-';
			else if(key[SDLK_SPACE])
				str[pos] = ' ';
			else if(key[SDLK_BACKSPACE]) {
				if(config.sound)
					Mix_PlayChannel(-1, snd_menu[2], 0);
				typed = false;
				str[pos] = '\0';
				pos--;
				if(pos < 0)
					pos = 0;
				str[pos] = '\0';
			}
			else if(key[SDLK_RETURN] && pos > 0 && str[0] != ' ') {
				done = true;
				typed = false;
			}
			else {
				typed = false;
			}

			// Clear the key array
			memset(key, 0, sizeof(key));

			// Modify string
			if(typed) {
				if(config.sound)
					Mix_PlayChannel(-1, snd_menu[2], 0);
				pos++;
				if(pos > NAME_LEN-1)
					pos = NAME_LEN-1;
				str[pos] = '\0';
				typed = false;
			}
					
			timer_count--;
		}
		// Draw
		// Blit the background
		SDL_BlitSurface(menu_bg[MENU_ID_HALL_OF_FAME], NULL, screen, NULL);

		// Draw the list
		draw(screen, 10, 10, place);

		// Draw the cursor
		if(cursor_blink < 10) {
			SDL_Rect r;
			r.x = font_length(FONT_NORMAL, str) + 16;
			r.y = 12 + place*20;
			r.w = 8;
			r.h = 15;
			SDL_FillRect(screen, &r, cursor_color);
		}

		// Flip to screen
		SDL_Flip(screen);
	}

	SDL_EnableKeyRepeat(0,0);

}


// Draw the list (hilighting the place, it it's != -1)
void HISCORE_LIST::draw(SDL_Surface *bmp, int x, int y, int place) {

	// Position holders
	int px = x;
	int py = y;

	// Draw each entry
	for(int f=0; f<NUM_ENTRIES; f++) {
		char str[256] = "";

		// Background
		if(f != place) {
			SDL_Rect r;
			r.x = px;
			r.y = py;
			r.w = 286;
			r.h = 19;
			float a = (float)f/NUM_ENTRIES;
			SDL_FillRect(bmp, &r, SDL_MapRGB(bmp->format, 0, (Uint8)(128-128*a), (Uint8)(70-50*a)));
			r.x = px;
			r.y = py+19;
			r.w = 286;
			r.h = 1;
			SDL_FillRect(bmp, &r, SDL_MapRGB(bmp->format, 0, 64, 0));
		}
		else {
			// Hilight the background
			float a = (float)f/NUM_ENTRIES;
			SDL_Rect r;
			for(int i=0; i<143; i++) {
				r.x = px+(i*2);
				r.y = py;
				r.w = 2;
				r.h = 19;
				SDL_FillRect(bmp, &r, SDL_MapRGB(bmp->format, RAND(0,70), (Uint8)(128-128*a)+RAND(0,70), (Uint8)(70-50*a)+RAND(0,70)));
			}
			r.x = px;
			r.y = py+19;
			r.w = 286;
			r.h = 1;
			SDL_FillRect(bmp, &r, SDL_MapRGB(bmp->format, 0, 64, 0));
		}


		// Dots
		for(unsigned int c=0; c<25; c++) {
			if(c < strlen(names[f]))
				str[c] = ' ';
			else
				str[c] = '.';
		}
		font_write(bmp, FONT_NORMAL2, px, py, str);

		// Name
		sprintf(str, "%s", names[f]);
		font_write(bmp, FONT_NORMAL, px, py, str);

		// Kills
		sprintf(str, "%03d", kills[f]);
		font_write(bmp, FONT_NORMAL, 260, py, str);

		py += 20;
	}

}


// Add a name to the list (if the record is good enough)
// and return the place which he/she gets in the list.
int HISCORE_LIST::add(char *name, int kill) {

	// Check whether player is allowed to the list in first place
	if(kill < kills[NUM_ENTRIES-1])
		return -1;

	// Overwrite the last entry (which drops out)
	strcpy(names[NUM_ENTRIES-1], name);
	kills[NUM_ENTRIES-1] = kill;

	// Sort the list
	int i;
	for(i=0; i<NUM_ENTRIES; i++) {
		for(int j=0; j<NUM_ENTRIES; j++) {
			// Compare and swap if necessary
			if(kills[i] > kills[j]) {
				char tmp_name[NAME_LEN] = "";
				int tmp_kills;

				strcpy(tmp_name, names[j]);
				tmp_kills = kills[j];

				strcpy(names[j], names[i]);
				kills[j] = kills[i];

				strcpy(names[i], tmp_name);
				kills[i] = tmp_kills;
			}
		}
	}

	// Find the place
	int place = -1;

	for(i=0; i<NUM_ENTRIES; i++) {
		if(strcmp(name, names[i]) == 0 && kills[i] == kill) {
			place = i;
			break;
		}
	}

	return place;
}


// Load the list
void HISCORE_LIST::load(char *file) {

	FILE *f;
	f = fopen(file, "rb");
	if(!f) {
		clear();
		return;
	}

	// Read names and kills
	fread(this, sizeof(HISCORE_LIST), 1, f);
	/*for(int i=0; i<NUM_ENTRIES; i++) {
		fread(names[i], sizeof(char), sizeof(names[i]), f);
		fread(&kills[i], sizeof(int), 1, f);
	}*/

	fclose(f);
}


// Save the list
void HISCORE_LIST::save(char *file) {

	FILE *f;
	f = fopen(file, "wb");
	if(!f)
	{
		system("mkdir -p ~/.formido");	// Pouah, hard coded in source...
		f = fopen(file, "wb");
		if(!f)
			error_msg("Oh crap! Can't save the highscores to %s!", file);
	}
	
	// Write names and kills
	fwrite(this, sizeof(HISCORE_LIST), 1, f);
	/*for(int i=0; i<NUM_ENTRIES; i++) {
		fwrite(names[i], sizeof(char), sizeof(names[i]), f);
		fwrite(&kills[i], sizeof(int), 1, f);
	}*/

	fclose(f);
}


// Clear the list
void HISCORE_LIST::clear() {

	for(int f=0; f<NUM_ENTRIES; f++) {
		strcpy(names[f], "FORMIDO");
		kills[f] = NUM_ENTRIES - f;
	}
}


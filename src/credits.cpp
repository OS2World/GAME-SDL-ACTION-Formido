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
#include "credits.h"
#include "init.h"
#include "font.h"
#include "timer.h"
#include "mymath.h"
#include "effect.h"
#include "config.h"


// Message array
CREDITS_MSG cmsg[MAX_MSGS];
int cur_cmsg;


// Distortion stuff
//////////////////////////////
SDL_Surface *credits_distort_tmp;

int credits_distort_table[360];
int credits_distort_table_x[360];
int credits_dis_a_start;
Uint32 mask_color;


// Distort a surface
void credits_distort(SDL_Surface *bmp) {

	SDL_FillRect(credits_distort_tmp, NULL, mask_color);
	SDL_BlitSurface(bmp, NULL, credits_distort_tmp, NULL);
	SDL_FillRect(bmp, NULL, mask_color);

	int dis_a = credits_dis_a_start;

	for(int y=0; y<SCREEN_H; y++) {
		int dis = credits_distort_table[dis_a];
		
		dis_a++;
		if(dis_a > 359)
			dis_a -= 360;

		SDL_Rect src, dst;
		src.x = 0;
		src.y = y;
		src.w = SCREEN_W;
		src.h = 1;

		dst.x = dis;
		dst.y = y;
		dst.w = SCREEN_W;
		dst.h = 1;
		SDL_BlitSurface(credits_distort_tmp, &src, bmp, &dst);
	}

	SDL_FillRect(credits_distort_tmp, NULL, mask_color);
	SDL_BlitSurface(bmp, NULL, credits_distort_tmp, NULL);
	SDL_FillRect(bmp, NULL, mask_color);

	for(int x=110; x<SCREEN_W-110; x++) {
		int dis = credits_distort_table_x[dis_a];

		dis_a+=1;
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
		SDL_BlitSurface(credits_distort_tmp, &src, bmp, &dst);
	}

}



// Show the credits
void show_credits() {

	// Initialize distortion stuff
	SDL_Surface *tmp;
	tmp = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_W, SCREEN_H, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	if(!tmp)
		error_msg("Unable to create distortion surface!\n");
	credits_distort_tmp = SDL_DisplayFormat(tmp);
	if(!credits_distort_tmp)
		error_msg("Unable to create distortion surface!\n");
	SDL_FreeSurface(tmp);
	for(int d=0; d<360; d++) {
		credits_distort_table[d] = (int)((float)SIN(d) * 20);
		credits_distort_table_x[d] = (int)((float)COS(d) * 10);
	}

	credits_dis_a_start = RAND(0,359);
	mask_color = SDL_MapRGB(screen->format, 255, 0, 255);

	// Load the background
	SDL_Surface *bg;
	if(!screen->format->palette)
		tmp = IMG_Load(DAT("credits.dat"));
	else
		tmp = IMG_Load(DAT("credits_8bit.dat"));
	if(tmp == NULL)
		error_msg("Unable to load background!\n%s", IMG_GetError());

	// Convert the background to display format
	bg = SDL_DisplayFormat(tmp);
	if(bg == NULL)
		error_msg("Unable to load credits background!\n%s", IMG_GetError());

	SDL_FreeSurface(tmp);

	// Font surface
	SDL_Surface *font_bmp;
	tmp = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCCOLORKEY, SCREEN_W, SCREEN_H, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	if(!tmp)
		error_msg("Unable to create credits font surface!\n");
	font_bmp = SDL_DisplayFormat(tmp);
	if(!font_bmp)
		error_msg("Unable to create credits font surface!\n");
	SDL_FreeSurface(tmp);
	//SDL_SetColorKey(font_bmp, SDL_SRCCOLORKEY, mask_color);

	// Initialize the credits
	float pos = SCREEN_H+10;
	clear_credits();
	add_credits("FORMIDO", "credits", pos); pos+=256;
	add_credits("GAME DESIGN", "mika halttunen", pos); pos+=128;
	add_credits("MUSIC", "josh schroeder", pos); pos+=128;
	add_credits("PROGRAMMING", "mika halttunen", pos); pos+=128;
	add_credits("ARTWORK", "mika halttunen", pos); pos+=256;
	add_credits("world wide web", "mhgames.cjb.net", pos); pos+=128;
	add_credits("(null)", "THANKS FOR PLAYING", pos);

	// Play some music
	Mix_Music *music = NULL;
	if(config.sound) {
		music = Mix_LoadMUS(DAT("music_credits.ogg"));
		Mix_PlayMusic(music, 0);
	}

	// Loop
	timer_count = 0;
	bool looping = true;
	while(looping) {
		// Handle events
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN)
						looping = false;
					break;
			}
		}

		while(timer_count > 0) {
			// Move the messages
			move_credits();
			if(cmsg[cur_cmsg-1].ok == false)
				looping = false;

			// Animate distortion
			credits_dis_a_start += 2;
			if(credits_dis_a_start > 359)
				credits_dis_a_start -= 360;

			timer_count--;
		}
		// Draw
		// Blit the background
		SDL_BlitSurface(bg, NULL, screen, NULL);

		// Draw the messages
		SDL_FillRect(font_bmp, NULL, mask_color);
		draw_credits(font_bmp);
		credits_distort(font_bmp);
		SDL_SetColorKey(font_bmp, SDL_SRCCOLORKEY, mask_color);
		SDL_BlitSurface(font_bmp, NULL, screen, NULL);
		SDL_SetColorKey(font_bmp, 0,0);

		// Flip to screen
		SDL_Flip(screen);
	}

	// Break the screen
	break_screen();

	// Unload music
	if(music) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}

	SDL_FreeSurface(bg);
	SDL_FreeSurface(font_bmp);
	SDL_FreeSurface(credits_distort_tmp);
}



// Draw the messages
void draw_credits(SDL_Surface *bmp) {

	for(int f=0; f<MAX_MSGS; f++)
		if(cmsg[f].ok)
			cmsg[f].draw(bmp);
}


// Move the messages
void move_credits() {

	for(int f=0; f<MAX_MSGS; f++)
		if(cmsg[f].ok)
			cmsg[f].move();
}


// Clear the messages
void clear_credits() {

	cur_cmsg = 0;
	for(int f=0; f<MAX_MSGS; f++)
		cmsg[f].clear();
}


// Add a message
void add_credits(char *tit, char *nam, float pos) {

	int f=cur_cmsg;
	cmsg[f].pos_y = pos;
	cmsg[f].speed_y = -0.37;
	cmsg[f].ok = true;
	strcpy(cmsg[f].title, tit);
	strcpy(cmsg[f].name, nam);

	cur_cmsg++;
	if(cur_cmsg > MAX_MSGS-1)
		error_msg("add_credits: Too many messages!\n");
}


// Move the message
void CREDITS_MSG::move() {

	pos_y += speed_y;

	if(pos_y <= -75)
		ok = false;
}


// Draw the message
void CREDITS_MSG::draw(SDL_Surface *bmp) {

	if(strcmp("(null)", title) != 0)
		font_write(bmp, FONT_BOLD, SCREEN_W/2-font_length(FONT_BOLD, title)/2, (int)pos_y, title);
	if(strcmp("(null)", name) != 0)
		font_write(bmp, FONT_BOLD2, SCREEN_W/2-font_length(FONT_BOLD2, name)/2, (int)pos_y+25, name);

}


// Clear the message
void CREDITS_MSG::clear() {

	memset(title, 0, sizeof(title));
	memset(name, 0, sizeof(name));

	pos_y = 0;
	speed_y = 0;

	ok = false;
}

